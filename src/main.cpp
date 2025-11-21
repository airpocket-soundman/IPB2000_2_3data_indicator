#include <Arduino.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <Preferences.h>
#include "config.h"
#include "mqtt.h"

// M5GFXの日本語フォントを使用
#include <lgfx/v1/lgfx_fonts.hpp>

// Preferences（WiFi選択の保存用）
Preferences preferences;

// M5Stack Tab5のWiFi用SDIOピン定義
#define SDIO2_CLK GPIO_NUM_12
#define SDIO2_CMD GPIO_NUM_13
#define SDIO2_D0  GPIO_NUM_11
#define SDIO2_D1  GPIO_NUM_10
#define SDIO2_D2  GPIO_NUM_9
#define SDIO2_D3  GPIO_NUM_8
#define SDIO2_RST GPIO_NUM_15

// データ構造
struct DataItem {
    const char* title;
    float currentValue;
    float previousValue;
};

// 3つのデータアイテム（起動時は0）
DataItem dataItems[3] = {
    {AppConfig::DATA_TITLE_1, 0.0, 0.0},
    {AppConfig::DATA_TITLE_2, 0.0, 0.0},
    {AppConfig::DATA_TITLE_3, 0.0, 0.0}
};

// スプライト（オフスクリーンバッファ）
LGFX_Sprite sprite(&M5.Display);

// WiFiクライアント
WiFiClient wifiClient;

// 関数の前方宣言
void displayAllData();

// データ更新コールバック（MQTTから呼ばれる）
void onDataUpdate(int index, float value) {
    if (index < 0 || index >= 3) return;
    
    // 現在値を前回値に移動
    dataItems[index].previousValue = dataItems[index].currentValue;
    // 新しい値を設定
    dataItems[index].currentValue = value;
    
    // 画面を即座に更新
    displayAllData();
}

// コマンドコールバック（MQTTから呼ばれる）
void onCommand(const char* command) {
    if (strcmp(command, "system_reset") == 0) {
        Serial.println("System reset in 3 seconds...");
        
        // 画面にメッセージ表示
        M5.Display.fillScreen(BLACK);
        M5.Display.setFont(&fonts::lgfxJapanGothic_40);
        M5.Display.setTextSize(2);
        M5.Display.setTextColor(RED, BLACK);
        M5.Display.setCursor(100, 600);
        M5.Display.println("System Reset...");
        
        delay(3000);
        ESP.restart();  // システムリセット
    }
}

// データ取得コールバック（MQTTから呼ばれる）
void onGetData(int index, float* currentValue, float* previousValue) {
    if (index < 0 || index >= 3) return;
    
    *currentValue = dataItems[index].currentValue;
    *previousValue = dataItems[index].previousValue;
}

// WiFi選択関数（タッチパネルで選択）
int selectWiFi() {
    // 前回の選択を読み込み
    preferences.begin("wifi", false);
    int lastSelected = preferences.getInt("selected", 1);  // デフォルトは1
    preferences.end();
    
    Serial.printf("Last selected WiFi: %d\n", lastSelected);
    
    // 選択画面を表示
    M5.Display.fillScreen(BLACK);
    M5.Display.setFont(&fonts::lgfxJapanGothic_40);
    M5.Display.setTextSize(1);
    
    // タイトル
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setCursor(100, 100);
    M5.Display.println("WiFi Select");
    
    // WiFi 1 ボタン（上半分）
    M5.Display.fillRect(50, 250, 620, 200, BLUE);
    M5.Display.setTextColor(WHITE, BLUE);
    M5.Display.setCursor(100, 320);
    M5.Display.printf("WiFi 1: %s", AppConfig::WIFI_SSID_1);
    
    // WiFi 2 ボタン（下半分）
    M5.Display.fillRect(50, 500, 620, 200, GREEN);
    M5.Display.setTextColor(WHITE, GREEN);
    M5.Display.setCursor(100, 570);
    M5.Display.printf("WiFi 2: %s", AppConfig::WIFI_SSID_2);
    
    // タイムアウト表示
    M5.Display.setTextColor(YELLOW, BLACK);
    M5.Display.setCursor(100, 800);
    M5.Display.println("5sec auto select...");
    
    uint32_t startTime = millis();
    int selected = lastSelected;
    
    while (millis() - startTime < AppConfig::WIFI_SELECT_TIMEOUT_MS) {
        M5.update();
        
        // タッチ判定
        auto touch = M5.Touch.getDetail();
        if (touch.wasPressed()) {
            int x = touch.x;
            int y = touch.y;
            
            Serial.printf("Touch: x=%d, y=%d\n", x, y);
            
            // WiFi 1 ボタン
            if (y >= 250 && y <= 450) {
                selected = 1;
                Serial.println("WiFi 1 selected");
                break;
            }
            // WiFi 2 ボタン
            else if (y >= 500 && y <= 700) {
                selected = 2;
                Serial.println("WiFi 2 selected");
                break;
            }
        }
        
        delay(50);
    }
    
    // 選択を保存
    preferences.begin("wifi", false);
    preferences.putInt("selected", selected);
    preferences.end();
    
    Serial.printf("Selected WiFi: %d\n", selected);
    return selected;
}

// WiFi接続関数（Tab5対応版）
bool connectWiFi(int wifiIndex) {
    const char* ssid;
    const char* password;
    
    if (wifiIndex == 2) {
        ssid = AppConfig::WIFI_SSID_2;
        password = AppConfig::WIFI_PASSWORD_2;
    } else {
        ssid = AppConfig::WIFI_SSID_1;
        password = AppConfig::WIFI_PASSWORD_1;
    }
    
    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    // 画面に接続中メッセージ表示
    M5.Display.fillScreen(BLACK);
    M5.Display.setFont(&fonts::lgfxJapanGothic_40);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(CYAN, BLACK);
    M5.Display.setCursor(50, 500);
    M5.Display.print("SSID: ");
    M5.Display.println(ssid);
    M5.Display.setCursor(50, 600);
    M5.Display.print("connecting");
    
    // Tab5用のSDIOピン設定（重要！）
    WiFi.setPins(SDIO2_CLK, SDIO2_CMD, SDIO2_D0, SDIO2_D1, SDIO2_D2, SDIO2_D3, SDIO2_RST);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    int dotCount = 0;
    uint32_t lastDotUpdate = 0;
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        
        // ドットアニメーション（500msごとに更新）
        if (millis() - lastDotUpdate >= 500) {
            lastDotUpdate = millis();
            Serial.print(".");
            
            // ドット部分を更新
            M5.Display.fillRect(400, 600, 300, 50, BLACK);
            M5.Display.setCursor(400, 600);
            for (int i = 0; i < dotCount; i++) {
                M5.Display.print(".");
            }
            dotCount = (dotCount + 1) % 8;
            attempts++;
        }
        
        if (attempts > 60) {  // 30秒でタイムアウト
            Serial.println("");
            Serial.println("WiFi connection timeout!");
            M5.Display.fillScreen(BLACK);
            M5.Display.setCursor(50, 600);
            M5.Display.setTextColor(RED, BLACK);
            M5.Display.print("Connection Failed!");
            delay(3000);
            return false;
        }
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    // 接続成功：IPアドレスを3秒表示
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(GREEN, BLACK);
    M5.Display.setCursor(50, 500);
    M5.Display.println("WiFi Connected!");
    M5.Display.setCursor(50, 600);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.print("IP: ");
    M5.Display.println(WiFi.localIP());
    delay(3000);
    
    return true;
}

void displayDataItem(LGFX_Sprite* spr, int index, int sectionX, int sectionY, int sectionW, int sectionH) {
    // セクションの矩形枠を描画
    spr->drawRect(sectionX, sectionY, sectionW, sectionH, WHITE);
    
    // すべてゴシック40フォントを使用
    spr->setFont(&fonts::lgfxJapanGothic_40);
    
    // タイトル
    spr->setTextSize(AppConfig::FONT_TITLE_SCALE);
    spr->setTextColor(CYAN, BLACK);
    spr->setCursor(sectionX + AppConfig::TEXT_TITLE_OFFSET_X, 
                   sectionY + AppConfig::TEXT_TITLE_OFFSET_Y);
    spr->printf("%s:", dataItems[index].title);
    
    // 今回値ラベル
    spr->setTextSize(1);
    spr->setTextColor(WHITE, BLACK);
    spr->setCursor(sectionX + AppConfig::TEXT_CURRENT_OFFSET_X, 
                   sectionY + AppConfig::TEXT_CURRENT_OFFSET_Y);
    spr->print("今回値: ");
    
    // 今回値の数値（右寄せ、3倍表示、ゼロパディング）
    char valueStr[20];
    snprintf(valueStr, sizeof(valueStr), "%0*.*f", 
             AppConfig::VALUE_TOTAL_WIDTH, 
             AppConfig::VALUE_DECIMAL_PLACES, 
             dataItems[index].currentValue);
    
    spr->setTextSize(AppConfig::FONT_VALUE_SCALE);
    int32_t textWidth = spr->textWidth(valueStr);
    spr->setCursor(sectionX + AppConfig::VALUE_RIGHT_ALIGN_X - textWidth, 
                   sectionY + AppConfig::TEXT_CURRENT_OFFSET_Y);
    spr->print(valueStr);
    
    // 前回値ラベル
    spr->setTextSize(1);
    spr->setTextColor(DARKGREY, BLACK);
    spr->setCursor(sectionX + AppConfig::TEXT_PREVIOUS_OFFSET_X, 
                   sectionY + AppConfig::TEXT_PREVIOUS_OFFSET_Y);
    spr->print("前回値: ");
    
    // 前回値の数値（右寄せ、3倍表示、ゼロパディング）
    snprintf(valueStr, sizeof(valueStr), "%0*.*f", 
             AppConfig::VALUE_TOTAL_WIDTH, 
             AppConfig::VALUE_DECIMAL_PLACES, 
             dataItems[index].previousValue);
    
    spr->setTextSize(AppConfig::FONT_VALUE_SCALE);
    textWidth = spr->textWidth(valueStr);
    spr->setCursor(sectionX + AppConfig::VALUE_RIGHT_ALIGN_X - textWidth, 
                   sectionY + AppConfig::TEXT_PREVIOUS_OFFSET_Y);
    spr->print(valueStr);
}

void displayAllData() {
    // スプライトが正常に作成されている場合のみ使用
    if (sprite.width() > 0 && sprite.height() > 0) {
        // スプライトに描画（オフスクリーン）
        sprite.fillScreen(BLACK);
        
        // 3つのデータセクションを描画（上・中・下）
        displayDataItem(&sprite, 0, 
                       AppConfig::SECTION1_X, AppConfig::SECTION1_Y,
                       AppConfig::SECTION1_WIDTH, AppConfig::SECTION1_HEIGHT);
        
        displayDataItem(&sprite, 1,
                       AppConfig::SECTION2_X, AppConfig::SECTION2_Y,
                       AppConfig::SECTION2_WIDTH, AppConfig::SECTION2_HEIGHT);
        
        displayDataItem(&sprite, 2,
                       AppConfig::SECTION3_X, AppConfig::SECTION3_Y,
                       AppConfig::SECTION3_WIDTH, AppConfig::SECTION3_HEIGHT);
        
        // スプライトを画面に一気に転送（ちらつき防止）
        sprite.pushSprite(0, 0);
    } else {
        // スプライト失敗時は直接画面に描画
        M5.Display.fillScreen(BLACK);
        
        displayDataItem((LGFX_Sprite*)&M5.Display, 0,
                       AppConfig::SECTION1_X, AppConfig::SECTION1_Y,
                       AppConfig::SECTION1_WIDTH, AppConfig::SECTION1_HEIGHT);
        
        displayDataItem((LGFX_Sprite*)&M5.Display, 1,
                       AppConfig::SECTION2_X, AppConfig::SECTION2_Y,
                       AppConfig::SECTION2_WIDTH, AppConfig::SECTION2_HEIGHT);
        
        displayDataItem((LGFX_Sprite*)&M5.Display, 2,
                       AppConfig::SECTION3_X, AppConfig::SECTION3_Y,
                       AppConfig::SECTION3_WIDTH, AppConfig::SECTION3_HEIGHT);
    }
}

void setup() {
    Serial.begin(AppConfig::SERIAL_BAUD_RATE);
    delay(1000);
    
    Serial.println("\n\n=== M5Stack Tab5 - Data Indicator ===");

    // M5Unifiedの初期化
    auto cfg = M5.config();
    cfg.output_power = true;
    M5.begin(cfg);
    
    Serial.println("M5.begin() completed");
    Serial.printf("Display: %dx%d\n", M5.Display.width(), M5.Display.height());

    // ディスプレイ設定
    M5.Display.setRotation(0);  
    M5.Display.setBrightness(AppConfig::DISPLAY_BRIGHTNESS);
    
    // WiFi選択
    int selectedWiFi = selectWiFi();
    
    // WiFi接続
    if (!connectWiFi(selectedWiFi)) {
        Serial.println("Continuing without WiFi...");
    } else {
        // MQTT初期化
        mqttSetup(wifiClient);
        mqttSetDataUpdateCallback(onDataUpdate);
        mqttSetCommandCallback(onCommand);
        mqttSetGetDataCallback(onGetData);
        
        // MQTT接続
        if (mqttConnect()) {
            Serial.println("MQTT setup completed");
        } else {
            Serial.println("MQTT connection failed, but continuing...");
        }
    }
    
    // スプライトの作成（画面と同じサイズ）
    // PSRAMを使用するように明示的に指定
    sprite.setPsram(true);
    bool spriteCreated = sprite.createSprite(M5.Display.width(), M5.Display.height());
    
    if (spriteCreated) {
        Serial.printf("Sprite created successfully: %dx%d\n", sprite.width(), sprite.height());
    } else {
        Serial.println("ERROR: Failed to create sprite!");
        Serial.println("Falling back to direct display rendering");
    }
    
    // 初期表示
    displayAllData();
    
    Serial.println("Data displayed!");
    Serial.println("Setup completed\n");
}

void loop() {
    M5.update();
    
    // MQTT接続維持とメッセージ処理
    mqttLoop();
    
    // 定期的に画面を更新
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();
    if (now - lastUpdate >= AppConfig::STATUS_UPDATE_INTERVAL_MS) {
        displayAllData();
        lastUpdate = now;
    }
    
    delay(100);
}

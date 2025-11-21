#include <Arduino.h>
#include <M5Unified.h>
#include "config.h"

// M5GFXの日本語フォントを使用
#include <lgfx/v1/lgfx_fonts.hpp>

// データ構造
struct DataItem {
    const char* title;
    float currentValue;
    float previousValue;
};

// 3つのデータアイテム
DataItem dataItems[3] = {
    {AppConfig::DATA_TITLE_1, 100.0, 99.8},
    {AppConfig::DATA_TITLE_2, 100.0, 99.8},
    {AppConfig::DATA_TITLE_3, 100.0, 99.8}
};

// スプライト（オフスクリーンバッファ）
LGFX_Sprite sprite(&M5.Display);

// フォントサイズに応じて適切なフォントを取得
const lgfx::v1::IFont* getFont(int size, bool isMincho = false) {
    if (isMincho) {
        // 明朝体
        if (size >= 40) return &fonts::lgfxJapanMincho_40;
        if (size >= 36) return &fonts::lgfxJapanMincho_36;
        if (size >= 32) return &fonts::lgfxJapanMincho_32;
        if (size >= 28) return &fonts::lgfxJapanMincho_28;
        if (size >= 24) return &fonts::lgfxJapanMincho_24;
        if (size >= 20) return &fonts::lgfxJapanMincho_20;
        if (size >= 16) return &fonts::lgfxJapanMincho_16;
        return &fonts::lgfxJapanMincho_12;
    } else {
        // ゴシック体
        if (size >= 40) return &fonts::lgfxJapanGothic_40;
        if (size >= 36) return &fonts::lgfxJapanGothic_36;
        if (size >= 32) return &fonts::lgfxJapanGothic_32;
        if (size >= 28) return &fonts::lgfxJapanGothic_28;
        if (size >= 24) return &fonts::lgfxJapanGothic_24;
        if (size >= 20) return &fonts::lgfxJapanGothic_20;
        if (size >= 16) return &fonts::lgfxJapanGothic_16;
        return &fonts::lgfxJapanGothic_12;
    }
}

void displayDataItem(LGFX_Sprite* spr, int index, int sectionX, int sectionY, int sectionW, int sectionH) {
    bool isMincho = (AppConfig::FONT_TYPE == 1);
    
    // セクションの矩形枠を描画
    spr->drawRect(sectionX, sectionY, sectionW, sectionH, WHITE);
    
    // タイトル（日本語対応フォント）
    spr->setFont(getFont(AppConfig::FONT_TITLE_SIZE, isMincho));
    spr->setTextSize(AppConfig::FONT_TITLE_SCALE);
    spr->setTextColor(CYAN, BLACK);
    spr->setCursor(sectionX + AppConfig::TEXT_TITLE_OFFSET_X, 
                   sectionY + AppConfig::TEXT_TITLE_OFFSET_Y);
    spr->printf("%s:", dataItems[index].title);
    
    // 今回値ラベル（日本語フォント使用）
    spr->setFont(getFont(AppConfig::FONT_CURRENT_SIZE, isMincho));
    spr->setTextSize(1);  // ラベルは等倍
    spr->setTextColor(WHITE, BLACK);
    spr->setCursor(sectionX + AppConfig::TEXT_CURRENT_OFFSET_X, 
                   sectionY + AppConfig::TEXT_CURRENT_OFFSET_Y);
    spr->print("今回値: ");
    
    // 今回値の数値（右寄せ、2倍表示、ゼロパディング）
    char valueStr[20];
    snprintf(valueStr, sizeof(valueStr), "%0*.*f", 
             AppConfig::VALUE_TOTAL_WIDTH, 
             AppConfig::VALUE_DECIMAL_PLACES, 
             dataItems[index].currentValue);
    
    spr->setTextSize(AppConfig::FONT_VALUE_SCALE);  // 2倍表示
    int32_t textWidth = spr->textWidth(valueStr);
    spr->setCursor(sectionX + AppConfig::VALUE_RIGHT_ALIGN_X - textWidth, 
                   sectionY + AppConfig::TEXT_CURRENT_OFFSET_Y);
    spr->print(valueStr);
    
    // 前回値ラベル（日本語フォント使用）
    spr->setFont(getFont(AppConfig::FONT_PREVIOUS_SIZE, isMincho));
    spr->setTextSize(1);  // ラベルは等倍
    spr->setTextColor(DARKGREY, BLACK);
    spr->setCursor(sectionX + AppConfig::TEXT_PREVIOUS_OFFSET_X, 
                   sectionY + AppConfig::TEXT_PREVIOUS_OFFSET_Y);
    spr->print("前回値: ");
    
    // 前回値の数値（右寄せ、3倍表示、ゼロパディング）
    snprintf(valueStr, sizeof(valueStr), "%0*.*f", 
             AppConfig::VALUE_TOTAL_WIDTH, 
             AppConfig::VALUE_DECIMAL_PLACES, 
             dataItems[index].previousValue);
    
    spr->setTextSize(AppConfig::FONT_VALUE_SCALE);  // 3倍表示
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
    
    // 定期的に画面を更新
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();
    if (now - lastUpdate >= AppConfig::STATUS_UPDATE_INTERVAL_MS) {
        displayAllData();
        lastUpdate = now;
    }
    
    delay(100);
}

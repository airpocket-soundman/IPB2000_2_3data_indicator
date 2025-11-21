#pragma once

namespace AppConfig {
    // Serial and timing
    inline constexpr unsigned long SERIAL_BAUD_RATE = 115200;
    inline constexpr unsigned long STATUS_UPDATE_INTERVAL_MS = 1000;

    // Display settings
    inline constexpr uint8_t DISPLAY_BRIGHTNESS = 200; // 0-255
    
    // Wi-Fi settings - 2組のWiFi設定
    inline const char* WIFI_SSID_1 = "J00WLN1305A";
    inline const char* WIFI_PASSWORD_1 = "m1n0ru0869553434@LAN";
    inline const char* WIFI_SSID_2 = "IPB2000-2";
    inline const char* WIFI_PASSWORD_2 = "minoru0869553434";
    inline constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 30000;  // 30秒
    inline constexpr unsigned long WIFI_SELECT_TIMEOUT_MS = 5000;    // WiFi選択タイムアウト（5秒）
    
    // MQTT settings
    inline const char* MQTT_BROKER_IP = "192.168.1.65";
    inline constexpr int MQTT_BROKER_PORT = 1883;
    inline const char* MQTT_TOPIC_SUBSCRIBE = "toIPB2000-2_display";   // 受信トピック
    inline const char* MQTT_TOPIC_PUBLISH = "fromIPB2000-2_display";   // 送信トピック
    inline const char* MQTT_CLIENT_ID = "IPB2000_display_01";
    inline const char* MQTT_DEVICE_ID = "IPB2000_display_01";  // このデバイスのID
    
    // MQTT Data Mapping (JSONキー → データアイテムインデックス)
    // 例: {"device_id":"IPB2000_display_01","oil_pressure":123.45,"parison_temp":192.8,"injection_time":345.9}
    inline const char* MQTT_KEY_DATA1 = "oil_pressure";   // データ1: 油圧
    inline const char* MQTT_KEY_DATA2 = "parison_temp";   // データ2: パリソン温度
    inline const char* MQTT_KEY_DATA3 = "injection_time"; // データ3: 射出時間（注:injction_timeとスペルミスの可能性あり）

    // Data titles
    inline const char* DATA_TITLE_1 = "油圧";
    inline const char* DATA_TITLE_2 = "パリソン温度";
    inline const char* DATA_TITLE_3 = "射出時間";
    
    // Layout settings - 各データセクションの矩形座標
    // データセット1（上）
    inline constexpr int SECTION1_X = 10;
    inline constexpr int SECTION1_Y = 10;
    inline constexpr int SECTION1_WIDTH = 700;
    inline constexpr int SECTION1_HEIGHT = 410;
    
    // データセット2（中）
    inline constexpr int SECTION2_X = 10;
    inline constexpr int SECTION2_Y = 430;
    inline constexpr int SECTION2_WIDTH = 700;
    inline constexpr int SECTION2_HEIGHT = 410;
    
    // データセット3（下）
    inline constexpr int SECTION3_X = 10;
    inline constexpr int SECTION3_Y = 850;
    inline constexpr int SECTION3_WIDTH = 700;
    inline constexpr int SECTION3_HEIGHT = 410;
    
    // テキスト表示位置のオフセット（矩形の左上からの相対位置）
    inline constexpr int TEXT_TITLE_OFFSET_X = 20;
    inline constexpr int TEXT_TITLE_OFFSET_Y = 20;
    inline constexpr int TEXT_CURRENT_OFFSET_X = 30;
    inline constexpr int TEXT_CURRENT_OFFSET_Y = 150;
    inline constexpr int TEXT_PREVIOUS_OFFSET_X = 30;
    inline constexpr int TEXT_PREVIOUS_OFFSET_Y = 270;
    
    // フォント拡大率（setTextSize）- lgfxJapanGothic_40固定
    inline constexpr int FONT_TITLE_SCALE = 2;      // タイトルの拡大率（2倍表示）
    inline constexpr int FONT_VALUE_SCALE = 3;      // 数値の拡大率（3倍表示）
    
    // 数値表示設定
    inline constexpr int VALUE_DECIMAL_PLACES = 1;  // 小数点以下の桁数
    inline constexpr int VALUE_TOTAL_WIDTH = 6;     // 全体の桁数（ゼロパディング用）
    inline constexpr int VALUE_RIGHT_ALIGN_X = 650; // 数値の右寄せ位置（セクション右端からの距離）
}

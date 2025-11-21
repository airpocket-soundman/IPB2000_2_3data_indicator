#pragma once

namespace AppConfig {
    // Serial and timing
    inline constexpr unsigned long SERIAL_BAUD_RATE = 115200;
    inline constexpr unsigned long STATUS_UPDATE_INTERVAL_MS = 1000;

    // Display settings
    inline constexpr uint8_t DISPLAY_BRIGHTNESS = 200; // 0-255

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
    
    // フォント設定
    // 利用可能なフォント:
    // - lgfxJapanGothic_12, _16, _20, _24, _28, _32, _36, _40
    // - lgfxJapanMincho_12, _16, _20, _24, _28, _32, _36, _40
    inline constexpr int FONT_TITLE_SIZE = 40;      // タイトルフォントサイズ
    inline constexpr int FONT_CURRENT_SIZE = 40;    // 今回値フォントサイズ
    inline constexpr int FONT_PREVIOUS_SIZE = 40;   // 前回値フォントサイズ
    
    // フォントタイプ選択 (0=ゴシック, 1=明朝)
    inline constexpr int FONT_TYPE = 0;  // 0: Gothic, 1: Mincho
    
    // フォント拡大率（setTextSize）
    inline constexpr int FONT_TITLE_SCALE = 2;      // タイトルの拡大率（2倍表示）
    inline constexpr int FONT_VALUE_SCALE = 3;      // 数値の拡大率（3倍表示）
    
    // 数値表示設定
    inline constexpr int VALUE_DECIMAL_PLACES = 1;  // 小数点以下の桁数
    inline constexpr int VALUE_TOTAL_WIDTH = 6;     // 全体の桁数（ゼロパディング用）
    inline constexpr int VALUE_RIGHT_ALIGN_X = 650; // 数値の右寄せ位置（セクション右端からの距離）
}

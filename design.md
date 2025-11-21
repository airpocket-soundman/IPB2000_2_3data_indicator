# M5Stack Tab5 アプリ設計

## 目的
- M5Stack Tab5 (ESP32-S3系) 向けのインジケータアプリを PlatformIO 形式で用意する。
- ランタイム設定を `src/config.h` に集約し、メインロジックは `src/main.cpp` に分離する。

## ターゲットハード
- デバイス: M5Stack Tab5（**ESP32-P4**）。PlatformIO のボードIDは `esp32-p4-evboard` を使用。
- プロセッサ: ESP32-P4（PSRAM搭載）
- 内蔵ディスプレイ: 1280x800の大画面。輝度は `config.h` で設定。M5Unified で制御。フォントサイズは大画面向けに調整済み。
- 入力: 内蔵ボタン/タッチ。イベント処理は今後拡張できるようスタブを用意。

## プロジェクト構成
- `platformio.ini`: ボード/フレームワーク/ライブラリ定義。
- `src/config.h`: Wi-Fi や表示テキストなどの設定値を管理。
- `src/main.cpp`: デバイス初期化、Wi-Fi 接続、ディスプレイ更新ループの骨組み。

## PlatformIO 環境
```
[env:tab5]
platform = https://github.com/pioarduino/platform-espressif32.git#54.03.21
framework = arduino
board = esp32-p4-evboard
board_build.mcu = esp32p4
board_build.flash_mode = qio
upload_speed = 1500000
monitor_speed = 115200
build_type = debug
build_flags =
    -DBOARD_HAS_PSRAM
    -DCORE_DEBUG_LEVEL=5
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DARDUINO_USB_MODE=1
lib_deps =
    https://github.com/M5Stack/M5Unified.git
    https://github.com/M5Stack/M5GFX.git
```

### 重要な設定ポイント
- **プラットフォーム**: ESP32-P4対応のカスタムplatform-espressif32を使用
- **ボード**: `esp32-p4-evboard` (M5Stack Tab5はESP32-P4ベース)
- **MCU**: `esp32p4` を明示的に指定
- **PSRAM**: Tab5はPSRAM搭載のため `-DBOARD_HAS_PSRAM` フラグを設定
- **デバッグ**: `CORE_DEBUG_LEVEL=5` で詳細なログ出力
- **USB**: CDC（シリアル）とMODEを設定
- **ライブラリ**: M5UnifiedとM5GFXをGitHubから直接取得（最新版）

## 設定項目 (`config.h`)
- `WIFI_SSID`, `WIFI_PASSWORD`
- `SERIAL_BAUD_RATE`, `STATUS_UPDATE_INTERVAL_MS`, `WIFI_CONNECT_TIMEOUT_MS`
- `DISPLAY_BRIGHTNESS` (0-255)
- 表示用ラベル（タイトル/接続中/接続成功/失敗メッセージ）

## 実行フロー (`main.cpp`)
- `setup()`
  - Serial 初期化
  - M5Unified 初期化（Tab5 ハードウェア設定）
  - 輝度設定と起動メッセージ表示
  - Wi-Fi 接続（タイムアウト付き）と結果表示
- `loop()`
  - `M5.update()` で入力更新
  - millis ベースでステータス更新（表示を定期リフレッシュ）
  - ボタン/タッチの処理フックを確保

## TODO / 確認事項
- ✅ **PlatformIO設定を正しいESP32-P4向けに更新完了**
  - ESP32-P4専用プラットフォーム使用
  - PSRAM、デバッグレベル、USB設定を追加
  - M5Unified/M5GFXを最新版（GitHub）に変更
- ✅ フォントサイズをTab5の大画面（1280x800）向けに調整完了（サイズ4に拡大）
- ⚠️ 実際の Wi-Fi 情報を `config.h` に設定する必要あり（現在はプレースホルダー）
- ボタン・タッチ入力と業務ロジックを追加してインジケータ機能を完成
- 実機での表示確認とレイアウト微調整

## 注意事項
M5Stack Tab5は**ESP32-P4**を搭載しており、ESP32-S3とは異なるアーキテクチャです。
必ず上記のplatform設定を使用してください。

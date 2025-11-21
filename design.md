# M5Stack Tab5 データインジケーター設計書

## 📋 プロジェクト概要

M5Stack Tab5（ESP32-P4）向けのIoTデータ表示システム。MQTTブローカー経由でリアルタイムデータを受信し、大画面に表示します。

### 主な用途
- 産業機器のデータモニタリング
- 油圧、温度、時間などの計測値表示
- リモートコマンド受信（リセット、ステータス送信）

### 特徴
- **大画面表示**: 1280x800ピクセルの見やすい表示
- **タッチ操作**: WiFi選択に対応
- **MQTT通信**: リアルタイムデータ受信とステータス送信
- **モジュール化**: WiFi、MQTT、表示を分離した保守性の高い設計

---

## 🎯 ターゲットハードウェア

### デバイス仕様
- **製品**: M5Stack Tab5
- **プロセッサ**: ESP32-P4
- **メモリ**: PSRAM搭載
- **ディスプレイ**: 1280x800 タッチスクリーン
- **通信**: WiFi（SDIO接続）

### WiFi接続仕様
Tab5のWiFiモジュールはSDIO接続を使用するため、専用のピン設定が必要：

```cpp
#define SDIO2_CLK GPIO_NUM_12
#define SDIO2_CMD GPIO_NUM_13
#define SDIO2_D0  GPIO_NUM_11
#define SDIO2_D1  GPIO_NUM_10
#define SDIO2_D2  GPIO_NUM_9
#define SDIO2_D3  GPIO_NUM_8
#define SDIO2_RST GPIO_NUM_15

WiFi.setPins(SDIO2_CLK, SDIO2_CMD, SDIO2_D0, SDIO2_D1, SDIO2_D2, SDIO2_D3, SDIO2_RST);
```

---

## 📁 プロジェクト構成

```
IPB2000_2_3data_indicator/
├── platformio.ini        # PlatformIO設定
├── design.md            # 本ドキュメント
├── README.md            # プロジェクト概要
└── src/
    ├── main.cpp         # メイン処理（WiFi選択、表示）
    ├── config.h         # 設定ファイル
    ├── mqtt.h           # MQTTインターフェース
    └── mqtt.cpp         # MQTT実装
```

### ファイルの役割

**main.cpp（約400行）**
- M5Stack初期化
- WiFi選択UI（タッチパネル）
- WiFi接続（Tab5 SDIO対応）
- データ表示（3セクション）
- MQTTコールバック処理

**mqtt.cpp（約220行）**
- MQTT通信
- JSON解析
- device_idフィルタリング
- コマンド処理
- ステータス送信

**config.h**
- WiFi設定（2組）
- MQTT設定
- 表示レイアウト設定
- データマッピング

---

## 🔧 PlatformIO環境

### platformio.ini

```ini
[platformio]
core_dir = C:\pio
packages_dir = C:\pio\packages
cache_dir = C:\pio\cache

[env:tab5]
platform = https://github.com/pioarduino/platform-espressif32.git#54.03.21
framework = arduino
board = esp32-p4-evboard
board_build.mcu = esp32p4
board_build.flash_mode = qio
board_build.partitions = huge_app.csv
upload_speed = 1500000
monitor_speed = 115200
build_type = release
build_flags =
    -DBOARD_HAS_PSRAM
    -DCORE_DEBUG_LEVEL=3
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DARDUINO_USB_MODE=1
    -Os
lib_deps =
    https://github.com/M5Stack/M5Unified.git
    https://github.com/M5Stack/M5GFX.git
    knolleary/PubSubClient@^2.8
    bblanchon/ArduinoJson@^7.0.0
```

### 重要な設定

| 項目 | 値 | 説明 |
|------|-----|------|
| platform | pioarduino/platform-espressif32 | ESP32-P4対応カスタムプラットフォーム |
| board | esp32-p4-evboard | Tab5のボード定義 |
| mcu | esp32p4 | ESP32-P4指定 |
| PSRAM | BOARD_HAS_PSRAM | PSRAM有効化 |
| USB CDC | ARDUINO_USB_CDC_ON_BOOT=1 | USB経由シリアル通信 |

### ライブラリ

- **M5Unified**: M5Stack統合ライブラリ
- **M5GFX**: グラフィックスライブラリ（日本語フォント含む）
- **PubSubClient**: MQTT通信
- **ArduinoJson v7**: JSON解析

---

## ⚙️ 設定項目（config.h）

### WiFi設定

```cpp
// 2組のWiFi設定
inline const char* WIFI_SSID_1 = "J00WLN1305A";
inline const char* WIFI_PASSWORD_1 = "m1n0ru0869553434@LAN";
inline const char* WIFI_SSID_2 = "YOUR_SSID_2";
inline const char* WIFI_PASSWORD_2 = "YOUR_PASSWORD_2";
inline constexpr unsigned long WIFI_SELECT_TIMEOUT_MS = 5000;  // 5秒
```

### MQTT設定

```cpp
inline const char* MQTT_BROKER_IP = "192.168.1.65";
inline constexpr int MQTT_BROKER_PORT = 1883;
inline const char* MQTT_TOPIC_SUBSCRIBE = "toIPB2000-2_display";   // 受信
inline const char* MQTT_TOPIC_PUBLISH = "fromIPB2000-2_display";   // 送信
inline const char* MQTT_DEVICE_ID = "IPB2000_display_01";
```

### データマッピング

```cpp
inline const char* MQTT_KEY_DATA1 = "oil_pressure";    // 油圧
inline const char* MQTT_KEY_DATA2 = "parison_temp";    // パリソン温度
inline const char* MQTT_KEY_DATA3 = "injection_time"; // 射出時間
```

### 表示設定

```cpp
// 画面輝度
inline constexpr uint8_t DISPLAY_BRIGHTNESS = 200;

// データセクション（3つ）
inline constexpr int SECTION1_Y = 10;    // 上
inline constexpr int SECTION2_Y = 430;   // 中
inline constexpr int SECTION3_Y = 850;   // 下

// フォント拡大率
inline constexpr int FONT_TITLE_SCALE = 2;  // タイトル2倍
inline constexpr int FONT_VALUE_SCALE = 3;  // 数値3倍
```

---

## 🔄 実行フロー

### 起動シーケンス

```
1. M5Stack初期化
   ├─ シリアル通信開始
   ├─ ディスプレイ設定
   └─ タッチパネル有効化

2. WiFi選択
   ├─ NVSから前回の選択を読み込み
   ├─ 選択画面表示（2つのボタン）
   ├─ 5秒間タッチ待ち
   │  ├─ タッチあり → 選択を保存
   │  └─ タイムアウト → 前回の選択
   └─ 選択をNVSに保存

3. WiFi接続
   ├─ Tab5 SDIOピン設定
   ├─ SSID/パスワード設定
   ├─ 接続中画面表示（ドットアニメーション）
   └─ 接続完了（IPアドレス表示）

4. MQTT接続
   ├─ ブローカー接続
   ├─ トピックサブスクライブ
   └─ コールバック設定

5. スプライト作成
   ├─ PSRAM使用設定
   └─ 1280x800バッファ作成

6. データ表示開始
   └─ 初期画面表示（全データ0.0）
```

### メインループ

```
loop():
  ├─ M5.update() (タッチイベント更新)
  ├─ mqttLoop() (MQTT接続維持・メッセージ処理)
  └─ displayAllData() (1秒ごとに画面更新)
```

---

## 📊 機能詳細

### 1. WiFi選択機能

#### 画面レイアウト

```
┌─────────────────────────────────┐
│         WiFi Select             │
├─────────────────────────────────┤
│  ┌───────────────────────────┐  │
│  │  WiFi 1: J00WLN1305A      │  │ ← 青色ボタン
│  │                           │  │   (y: 250-450)
│  └───────────────────────────┘  │
│                                 │
│  ┌───────────────────────────┐  │
│  │  WiFi 2: YOUR_SSID_2      │  │ ← 緑色ボタン
│  │                           │  │   (y: 500-700)
│  └───────────────────────────┘  │
│                                 │
│  5sec auto select...            │ ← 黄色テキスト
└─────────────────────────────────┘
```

#### 動作仕様

- **タッチ判定**: Y座標で判定（上半分/下半分）
- **タイムアウト**: 5秒後に自動選択
- **保存方式**: NVS（不揮発メモリ）
- **保存内容**: namespace="wifi", key="selected", value=1 or 2

#### コード

```cpp
int selectWiFi() {
    preferences.begin("wifi", false);
    int lastSelected = preferences.getInt("selected", 1);
    preferences.end();
    
    // UI表示とタッチ待ち
    // ...
    
    preferences.begin("wifi", false);
    preferences.putInt("selected", selected);
    preferences.end();
    
    return selected;
}
```

### 2. データ表示機能

#### 画面構成（縦3分割）

```
┌─────────────────────────────┐
│ 油圧:                       │ ← セクション1
│   今回値:  0123.4           │   (10, 10, 700x410)
│   前回値:  0120.0           │
├─────────────────────────────┤
│ パリソン温度:               │ ← セクション2
│   今回値:  0192.8           │   (10, 430, 700x410)
│   前回値:  0190.0           │
├─────────────────────────────┤
│ 射出時間:                   │ ← セクション3
│   今回値:  0345.9           │   (10, 850, 700x410)
│   前回値:  0340.0           │
└─────────────────────────────┘
```

#### 表示仕様

| 要素 | フォント | サイズ | 色 | 配置 |
|------|----------|--------|-----|------|
| タイトル | lgfxJapanGothic_40 | x2 | CYAN | 左上 |
| 今回値ラベル | lgfxJapanGothic_40 | x1 | WHITE | 左 |
| 今回値数値 | lgfxJapanGothic_40 | x3 | WHITE | 右寄せ |
| 前回値ラベル | lgfxJapanGothic_40 | x1 | DARKGREY | 左 |
| 前回値数値 | lgfxJapanGothic_40 | x3 | DARKGREY | 右寄せ |

#### スプライト使用

```cpp
LGFX_Sprite sprite(&M5.Display);
sprite.setPsram(true);  // PSRAM使用
sprite.createSprite(1280, 800);  // オフスクリーンバッファ

// 描画
sprite.fillScreen(BLACK);
displayDataItem(&sprite, 0, ...);  // データ1
displayDataItem(&sprite, 1, ...);  // データ2
displayDataItem(&sprite, 2, ...);  // データ3
sprite.pushSprite(0, 0);  // 一括転送（ちらつき防止）
```

### 3. MQTT通信機能

#### データ受信

**受信トピック**: `toIPB2000-2_display`

**メッセージ形式**:
```json
{
  "device_id": "IPB2000_display_01",
  "oil_pressure": 123.45,
  "parison_temp": 192.8,
  "injection_time": 345.9
}
```

**処理フロー**:
1. JSONパース
2. device_idチェック（フィルタリング）
3. 各データ値を抽出
4. データ更新コールバック呼び出し
5. 画面即座に更新

#### コマンド受信

**1. system_reset コマンド**
```json
{
  "device_id": "IPB2000_display_01",
  "command": "system_reset"
}
```
- 画面に"System Reset..."表示
- 3秒待機
- ESP.restart()実行

**2. send_status コマンド**
```json
{
  "device_id": "IPB2000_display_01",
  "command": "send_status"
}
```
- 現在の全データを取得
- JSON形式で送信
- 送信トピック: `fromIPB2000-2_display`

#### ステータス送信

**送信トピック**: `fromIPB2000-2_display`

**メッセージ形式**:
```json
{
  "device_id": "IPB2000_display_01",
  "oil_pressure_current": 123.5,
  "oil_pressure_previous": 120.0,
  "parison_temp_current": 192.8,
  "parison_temp_previous": 190.0,
  "injection_time_current": 345.9,
  "injection_time_previous": 340.0
}
```

#### MQTT再接続

```cpp
void mqttLoop() {
    if (!mqttClient->connected()) {
        Serial.println("MQTT disconnected. Reconnecting...");
        mqttConnect();
    }
    mqttClient->loop();
}
```

### 4. デバイスIDフィルタリング

他のデバイス宛のメッセージを無視する仕組み：

```cpp
if (!doc["device_id"].isNull()) {
    const char* deviceId = doc["device_id"];
    if (strcmp(deviceId, AppConfig::MQTT_DEVICE_ID) != 0) {
        Serial.printf("Message for different device: %s (ignored)\n", deviceId);
        return;
    }
}
```

---

## 🔌 データフロー図

### 全体フロー

```
MQTTブローカー
    ↓ publish (toIPB2000-2_display)
ESP32-P4 (Tab5)
    ↓ JSONパース
device_idチェック
    ↓ 一致
データ抽出
    ↓
コールバック
    ↓
dataItems更新
    ↓
displayAllData()
    ↓
画面表示更新
```

### コマンドフロー

```
MQTTブローカー
    ↓ {"command":"send_status"}
mqtt.cpp
    ↓
sendStatusResponse()
    ↓ getDataCallback
main.cpp (onGetData)
    ↓ データ取得
mqtt.cpp
    ↓ JSON生成
MQTTブローカー
    ↓ publish (fromIPB2000-2_display)
外部システム
```

---

## 🧪 テスト方法

### 1. ビルドと書き込み

```powershell
# PlatformIOでビルド・書き込み
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run -e tab5 --target upload

# モニター起動
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device monitor
```

### 2. WiFi選択テスト

1. デバイス起動
2. WiFi選択画面表示を確認
3. 上半分をタッチ → WiFi 1選択
4. または下半分をタッチ → WiFi 2選択
5. または5秒待つ → 前回の選択で自動接続
6. WiFi接続成功を確認

### 3. データ受信テスト

```bash
# Mosquittoクライアントでテスト送信
mosquitto_pub -h 192.168.1.65 -t "toIPB2000-2_display" -m '{
  "device_id":"IPB2000_display_01",
  "oil_pressure":123.45,
  "parison_temp":192.8,
  "injection_time":345.9
}'
```

**期待結果**:
- 画面に各データが表示される
- シリアルモニターにログが出力される

### 4. send_statusテスト

```bash
# ステータス要求
mosquitto_pub -h 192.168.1.65 -t "toIPB2000-2_display" -m '{
  "device_id":"IPB2000_display_01",
  "command":"send_status"
}'

# 応答を受信
mosquitto_sub -h 192.168.1.65 -t "fromIPB2000-2_display"
```

**期待結果**:
```json
{
  "device_id":"IPB2000_display_01",
  "oil_pressure_current":123.5,
  "oil_pressure_previous":120.0,
  ...
}
```

### 5. system_resetテスト

```bash
# リセットコマンド送信
mosquitto_pub -h 192.168.1.65 -t "toIPB2000-2_display" -m '{
  "device_id":"IPB2000_display_01",
  "command":"system_reset"
}'
```

**期待結果**:
- 画面に"System Reset..."表示
- 3秒後にデバイスが再起動

---

## 🐛 トラブルシューティング

### WiFi接続失敗

**症状**: WiFi接続が30秒でタイムアウト

**確認事項**:
1. config.hのSSID/パスワードが正しいか
2. WiFiルーターが2.4GHz帯に対応しているか（5GHzは非対応）
3. シリアルモニターでエラーログを確認

### MQTT接続失敗

**症状**: MQTTブローカーに接続できない

**確認事項**:
1. ブローカーIPアドレスが正しいか
2. ブローカーが起動しているか
3. ファイアウォール設定
4. シリアルログで接続状態を確認

### 画面が表示されない

**症状**: スプライトが正常に表示されない

**確認事項**:
1. PSRAMが有効か（build_flags確認）
2. シリアルログで"Sprite created successfully"を確認
3. 失敗時は直接描画にフォールバック

### タッチが反応しない

**症状**: WiFi選択画面でタッチが効かない

**確認事項**:
1. M5.update()が呼ばれているか
2. タッチ座標がシリアルログに出力されるか
3. Y座標の判定範囲（250-450, 500-700）を確認

---

## 📈 拡張可能性

### 表示データの追加

1. **config.hでマッピング追加**:
```cpp
inline const char* MQTT_KEY_DATA4 = "temperature";
```

2. **dataItems配列を拡張**:
```cpp
DataItem dataItems[4] = { ... };
```

3. **表示セクション追加**:
```cpp
displayDataItem(&sprite, 3, ...);
```

### 新しいコマンド追加

mqtt.cppのコマンド処理に追加:

```cpp
else if (strcmp(command, "take_screenshot") == 0) {
    // スクリーンショット処理
}
```

### 複数デバイス対応

config.hでデバイスIDを変更:

```cpp
inline const char* MQTT_DEVICE_ID = "IPB2000_display_02";
```

### グラフ表示

時系列データをグラフ化:
- M5GFXの描画機能を使用
- リングバッファでデータ履歴を保存
- 折れ線グラフとして表示

---

## ✅ TODO / 完了項目

### ✅ 完了
- [x] ESP32-P4向けPlatformIO設定
- [x] Tab5 WiFi SDIO対応
- [x] 大画面対応（1280x800）
- [x] 日本語フォント表示
- [x] WiFi選択UI（タッチパネル）
- [x] WiFi選択のNVS保存
- [x] MQTT通信（受信・送信）
- [x] JSONパース
- [x] device_idフィルタリング
- [x] 3つのデータ表示
- [x] 今回値・前回値管理
- [x] スプライトによるちらつき防止
- [x] system_resetコマンド
- [x] send_statusコマンド
- [x] MQTT自動再接続
- [x] モジュール化（MQTT分離）

### 🔄 今後の拡張

- [ ] グラフ表示機能
- [ ] 設定画面（WiFi、MQTT設定変更）
- [ ] アラート機能（閾値超過時に警告）
- [ ] ログ保存（SDカード）
- [ ] OTA更新対応
- [ ] 複数画面切り替え

---

## 📚 参考資料

### 公式ドキュメント
- [M5Stack Tab5](https://docs.m5stack.com/en/core/M5Stack%20Tab5)
- [M5Unified](https://github.com/M5Stack/M5Unified)
- [M5GFX](https://github.com/M5Stack/M5GFX)
- [PubSubClient](https://pubsubclient.knolleary.net/)
- [ArduinoJson](https://arduinojson.org/)

### ESP32-P4リソース
- [ESP32-P4 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-p4_datasheet_en.pdf)
- [PlatformIO ESP32-P4](https://github.com/pioarduino/platform-espressif32)

---

## 📝 変更履歴

| 日付 | バージョン | 変更内容 |
|------|-----------|---------|
| 2025/11/21 | 1.0.0 | 初版リリース |
| 2025/11/21 | 1.1.0 | WiFi選択機能追加 |
| 2025/11/21 | 1.2.0 | MQTTコマンド機能追加 |
| 2025/11/21 | 1.3.0 | 設計書充実化 |

---

## 👤 作成者

- プロジェクト: IPB2000_2_3data_indicator
- ターゲット: M5Stack Tab5 (ESP32-P4)
- フレームワーク: Arduino + PlatformIO
- ライセンス: (未定)

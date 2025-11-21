#pragma once

#include <PubSubClient.h>
#include <WiFiClient.h>

// MQTT初期化
void mqttSetup(WiFiClient& wifiClient);

// MQTT接続
bool mqttConnect();

// MQTTループ（接続維持とメッセージ処理）
void mqttLoop();

// MQTT接続状態確認
bool mqttIsConnected();

// データ更新コールバック関数の型定義
typedef void (*DataUpdateCallback)(int index, float value);

// コマンドコールバック関数の型定義
typedef void (*CommandCallback)(const char* command);

// 状態取得コールバック関数の型定義（現在値と前回値を取得）
typedef void (*GetDataCallback)(int index, float* currentValue, float* previousValue);

// データ更新コールバックの設定
void mqttSetDataUpdateCallback(DataUpdateCallback callback);

// コマンドコールバックの設定
void mqttSetCommandCallback(CommandCallback callback);

// 状態取得コールバックの設定
void mqttSetGetDataCallback(GetDataCallback callback);

// ステータスを送信

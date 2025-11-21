#include "mqtt.h"
#include "config.h"
#include <ArduinoJson.h>

static PubSubClient* mqttClient = nullptr;
static DataUpdateCallback dataUpdateCallback = nullptr;
static CommandCallback commandCallback = nullptr;
static GetDataCallback getDataCallback = nullptr;

// 前方宣言
void sendStatusResponse();

// MQTTコールバック関数
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    // ペイロードを文字列に変換
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    Serial.println(message);
    
    // JSONパース
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        return;
    }
    
    // device_idチェック（このデバイス宛のメッセージのみ処理）
    if (!doc["device_id"].isNull()) {
        const char* deviceId = doc["device_id"];
        if (strcmp(deviceId, AppConfig::MQTT_DEVICE_ID) != 0) {
            Serial.printf("Message for different device: %s (ignored)\n", deviceId);
            return;
        }
    }
    
    bool updated = false;
    
    // データ1 (oil_pressure)
    if (!doc[AppConfig::MQTT_KEY_DATA1].isNull()) {
        float newValue = doc[AppConfig::MQTT_KEY_DATA1];
        Serial.printf("%s: %.1f\n", AppConfig::MQTT_KEY_DATA1, newValue);
        if (dataUpdateCallback) {
            dataUpdateCallback(0, newValue);
        }
        updated = true;
    }
    
    // データ2 (parison_temp)
    if (!doc[AppConfig::MQTT_KEY_DATA2].isNull()) {
        float newValue = doc[AppConfig::MQTT_KEY_DATA2];
        Serial.printf("%s: %.1f\n", AppConfig::MQTT_KEY_DATA2, newValue);
        if (dataUpdateCallback) {
            dataUpdateCallback(1, newValue);
        }
        updated = true;
    }
    
    // データ3 (injection_time または injction_time)
    if (!doc[AppConfig::MQTT_KEY_DATA3].isNull()) {
        float newValue = doc[AppConfig::MQTT_KEY_DATA3];
        Serial.printf("%s: %.1f\n", AppConfig::MQTT_KEY_DATA3, newValue);
        if (dataUpdateCallback) {
            dataUpdateCallback(2, newValue);
        }
        updated = true;
    } else if (!doc["injction_time"].isNull()) {
        // スペルミスの可能性に対応
        float newValue = doc["injction_time"];
        Serial.printf("injction_time: %.1f\n", newValue);
        if (dataUpdateCallback) {
            dataUpdateCallback(2, newValue);
        }
        updated = true;
    }
    
    if (!updated) {
        Serial.println("No valid data in message");
    }
    
    // コマンド処理
    if (!doc["command"].isNull()) {
        const char* command = doc["command"];
        Serial.printf("Command received: %s\n", command);
        
        if (strcmp(command, "system_reset") == 0) {
            Serial.println("System reset requested");
            if (commandCallback) {
                commandCallback(command);
            }
        } else if (strcmp(command, "send_status") == 0) {
            Serial.println("Status request received");
            sendStatusResponse();
        }
    }
}

// ステータス応答を送信
void sendStatusResponse() {
    if (!mqttClient || !mqttClient->connected()) {
        Serial.println("MQTT not connected, cannot send status");
        return;
    }
    
    if (!getDataCallback) {
        Serial.println("GetData callback not set");
        return;
    }
    
    // JSONドキュメント作成
    JsonDocument doc;
    doc["device_id"] = AppConfig::MQTT_DEVICE_ID;
    
    // 各データを取得
    for (int i = 0; i < 3; i++) {
        float currentValue = 0.0;
        float previousValue = 0.0;
        getDataCallback(i, &currentValue, &previousValue);
        
        if (i == 0) {
            doc["oil_pressure_current"] = currentValue;
            doc["oil_pressure_previous"] = previousValue;
        } else if (i == 1) {
            doc["parison_temp_current"] = currentValue;
            doc["parison_temp_previous"] = previousValue;
        } else if (i == 2) {
            doc["injection_time_current"] = currentValue;
            doc["injection_time_previous"] = previousValue;
        }
    }
    
    // JSON文字列化
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));
    
    // 送信
    if (mqttClient->publish(AppConfig::MQTT_TOPIC_PUBLISH, jsonBuffer)) {
        Serial.println("Status sent successfully");
        Serial.println(jsonBuffer);
    } else {
        Serial.println("Failed to send status");
    }
}

// MQTT初期化
void mqttSetup(WiFiClient& wifiClient) {
    mqttClient = new PubSubClient(wifiClient);
    mqttClient->setServer(AppConfig::MQTT_BROKER_IP, AppConfig::MQTT_BROKER_PORT);
    mqttClient->setCallback(mqttCallback);
}

// MQTT接続
bool mqttConnect() {
    if (!mqttClient) {
        Serial.println("MQTT client not initialized");
        return false;
    }
    
    Serial.print("Connecting to MQTT broker...");
    
    if (mqttClient->connect(AppConfig::MQTT_CLIENT_ID)) {
        Serial.println("connected!");
        
        // トピックをサブスクライブ
        if (mqttClient->subscribe(AppConfig::MQTT_TOPIC_SUBSCRIBE)) {
            Serial.print("Subscribed to: ");
            Serial.println(AppConfig::MQTT_TOPIC_SUBSCRIBE);
            return true;
        } else {
            Serial.println("Failed to subscribe");
            return false;
        }
    } else {
        Serial.print("failed, rc=");
        Serial.println(mqttClient->state());
        return false;
    }
}

// MQTTループ（接続維持とメッセージ処理）
void mqttLoop() {
    if (mqttClient && !mqttClient->connected()) {
        Serial.println("MQTT disconnected. Reconnecting...");
        mqttConnect();
    }
    
    if (mqttClient) {
        mqttClient->loop();
    }
}

// MQTT接続状態確認
bool mqttIsConnected() {
    return mqttClient && mqttClient->connected();
}

// データ更新コールバックの設定
void mqttSetDataUpdateCallback(DataUpdateCallback callback) {
    dataUpdateCallback = callback;
}

// コマンドコールバックの設定
void mqttSetCommandCallback(CommandCallback callback) {
    commandCallback = callback;
}

// 状態取得コールバックの設定
void mqttSetGetDataCallback(GetDataCallback callback) {
    getDataCallback = callback;
}

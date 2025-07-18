#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <algorithm>
#include "MqttClient.h"
#include "Parameter.h"

extern WiFiClient network;

MqttClient::MqttClient(Storage* storage, Logger* logger)
{
    this->storage = storage;
    this->logger = logger;

    this->lastReconnectAttempt = 0;
    this->reconnectInterval = 5000;
    this->lastActivityCheck = 0;
    this->lastMqttActivity = 0;
    this->previouslyConnected = false;
}

bool MqttClient::connectMqtt()
{
    String username = this->storage->getParameter(Parameter::MQTT_USER);
    String password = this->storage->getParameter(Parameter::MQTT_PASS);
    String device = this->storage->getParameter(Parameter::MQTT_DEVICE);
    String willTopic = "esp/analog_distance_meter/" + device + "/availability";
    
    if (client.connected()) {
        client.disconnect();
        delay(100);
    }
    
    client.setKeepAlive(60);
    client.setWill(willTopic.c_str(), "offline", true, 1);
    
    std::function<bool()> connection;

    if (username == "" && password == "") {
        connection = [device, this]() -> bool { 
            return client.connect(device.c_str()); 
        };
    } else {
        connection = [device, username, password, this]() -> bool {
            return client.connect(device.c_str(), username.c_str(), password.c_str());
        };
    }

    auto result = connection();

    if (result) {
        client.publish(willTopic.c_str(), "online", true, 1);
        this->logger->info("Connected to MQTT broker");
        this->reconnectInterval = 5000;
        this->updateActivityTimestamp();
        
        if (this->previouslyConnected) {
            this->publishHomeAssistantAutoconfig();
        }
        this->previouslyConnected = true;
    } else {
        this->logger->warning("Failed to connect to MQTT broker, error code: " + String(client.lastError()));
        this->reconnectInterval = std::min(this->reconnectInterval * 1.5, 30000.0);
    }

    return result;
}

void MqttClient::begin()
{
    client.begin(
        this->storage->getParameter(Parameter::MQTT_HOST).c_str(),
        atoi(this->storage->getParameter(Parameter::MQTT_PORT).c_str()),
        network
    );

    this->connectMqtt();
    this->publishHomeAssistantAutoconfig();
}

void MqttClient::publishHomeAssistantAutoconfig()
{
    String mqttTopic = this->storage->getParameter(Parameter::MQTT_TOPIC_DISTANCE);
    if (mqttTopic.isEmpty()) {
        mqttTopic = "esp/analog_distance_meter/" + this->storage->getParameter(Parameter::MQTT_DEVICE) + "/state";
        this->storage->saveParameter(Parameter::MQTT_TOPIC_DISTANCE, mqttTopic);
        this->logger->info("Setting default MQTT topic: " + mqttTopic);
    }
    
    String deviceId = this->storage->getParameter(Parameter::MQTT_DEVICE) + "_" + String(ESP.getChipId());
    String configTopic = "homeassistant/sensor/" + deviceId + "/level/config";
    
    DynamicJsonDocument doc(512);
    String json;

    doc["state_topic"] = mqttTopic;
    doc["value_template"] = "{{ ((value_json.relative | float) * 100) | round(1) }}";
    doc["unit_of_measurement"] = "%";
    doc["name"] = "Water Level";
    doc["unique_id"] = deviceId + "_level";
    doc["icon"] = "mdi:water-percent";
    doc["state_class"] = "measurement";

    JsonObject device = doc.createNestedObject("device");
    device["identifiers"] = JsonArray().add(deviceId);
    device["name"] = "ESP Analog Distance Meter";
    device["model"] = "ESP8266";
    device["manufacturer"] = "ESP";
    device["sw_version"] = "1.0";

    serializeJson(doc, json);
    
    bool success = client.publish(configTopic.c_str(), json.c_str(), true, 1);
    
    if (success) {
        this->logger->info("Published Home Assistant autodiscovery config");
    } else {
        this->logger->warning("Failed to publish Home Assistant config");
    }
    
    configTopic = "homeassistant/sensor/" + deviceId + "/depth/config";
    doc.clear();
    
    doc["state_topic"] = mqttTopic;
    doc["value_template"] = "{{ value_json.absolute | round(2) }}";
    doc["unit_of_measurement"] = "m";
    doc["name"] = "Water Depth";
    doc["unique_id"] = deviceId + "_depth";
    doc["device_class"] = "distance";
    doc["icon"] = "mdi:ruler";
    doc["state_class"] = "measurement";

    device = doc.createNestedObject("device");
    device["identifiers"] = JsonArray().add(deviceId);
    device["name"] = "ESP Analog Distance Meter";
    device["model"] = "ESP8266";
    device["manufacturer"] = "ESP";
    device["sw_version"] = "1.0";
    
    json = "";
    serializeJson(doc, json);
    
    client.publish(configTopic.c_str(), json.c_str(), true, 1);
}

void MqttClient::updateActivityTimestamp()
{
    this->lastMqttActivity = millis();
}

bool MqttClient::run()
{
    client.loop();
    
    bool isConnected = client.connected();
    unsigned long now = millis();
    
    if (now - this->lastActivityCheck >= 5000) {
        this->lastActivityCheck = now;
        
        if (isConnected) {
            if (now - this->lastMqttActivity >= KEEPALIVE_INTERVAL) {
                String device = this->storage->getParameter(Parameter::MQTT_DEVICE);
                String statusTopic = "esp/analog_distance_meter/" + device + "/status";
                client.publish(statusTopic.c_str(), String("active," + String(ESP.getFreeHeap())).c_str(), false, 0);
                this->updateActivityTimestamp();
                this->logger->info("Sent keepalive message");
            }
        }
        else if (now - this->lastReconnectAttempt >= this->reconnectInterval) {
            this->lastReconnectAttempt = now;
            this->logger->warning("Connection to MQTT lost, reconnecting...");
            isConnected = this->connectMqtt();
        }
    }
    
    return isConnected;
}

void MqttClient::sendDistance(float relative, float absolute, float measured)
{
    if (!client.connected()) {
        this->logger->warning("Cannot publish - MQTT not connected");
        return;
    }
    
    DynamicJsonDocument doc(128);
    String json;

    doc["relative"] = relative;
    doc["absolute"] = absolute;
    doc["measured"] = measured;
    serializeJson(doc, json);

    String topic = this->storage->getParameter(Parameter::MQTT_TOPIC_DISTANCE);
    auto ok = client.publish(topic.c_str(), json.c_str(), false, 0);

    if (ok) {
        this->updateActivityTimestamp();
    } else {
        this->logger->warning("Failed to publish to MQTT: " + json);
    }
}

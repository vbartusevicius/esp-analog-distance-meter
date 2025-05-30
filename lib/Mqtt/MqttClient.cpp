#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "MqttClient.h"
#include "Parameter.h"

extern WiFiClient network;

MqttClient::MqttClient(Storage* storage, Logger* logger)
{
    this->storage = storage;
    this->logger = logger;

    this->lastReconnectAttempt = 0;
}

bool MqttClient::connect()
{
    String username = this->storage->getParameter(Parameter::MQTT_USER);
    String password = this->storage->getParameter(Parameter::MQTT_PASS);
    String device = this->storage->getParameter(Parameter::MQTT_DEVICE);

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
        this->logger->info("Conected to MQTT.");
    } else {
        this->logger->warning("Failed to connect to MQTT.");
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

    this->connect();
    this->publishHomeAssistantAutoconfig();
}

void MqttClient::publishHomeAssistantAutoconfig()
{
    // Check if MQTT topic is set, otherwise set a default
    String mqttTopic = this->storage->getParameter(Parameter::MQTT_TOPIC_DISTANCE);
    if (mqttTopic.isEmpty()) {
        // Set a default topic if none is configured
        mqttTopic = "esp/analog_distance_meter/" + this->storage->getParameter(Parameter::MQTT_DEVICE) + "/state";
        this->storage->saveParameter(Parameter::MQTT_TOPIC_DISTANCE, mqttTopic);
        this->logger->info("Setting default MQTT topic: " + mqttTopic);
    }
    
    String deviceId = this->storage->getParameter(Parameter::MQTT_DEVICE) + "_" + String(ESP.getChipId());
    String configTopic = "homeassistant/sensor/" + deviceId + "/level/config";
    
    // Create a larger document to accommodate device info
    DynamicJsonDocument doc(512);
    String json;

    // Basic sensor configuration
    doc["state_topic"] = mqttTopic;
    doc["value_template"] = "{{ ((value_json.relative | float) * 100) | round(1) }}";
    doc["unit_of_measurement"] = "%";
    doc["name"] = "Water Level";
    doc["unique_id"] = deviceId + "_level";
    doc["device_class"] = "water";
    doc["icon"] = "mdi:water-percent";
    
    // Device information for Home Assistant
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"] = JsonArray().add(deviceId);
    device["name"] = "ESP Analog Distance Meter";
    device["model"] = "ESP8266";
    device["manufacturer"] = "ESP";
    device["sw_version"] = "1.0";

    serializeJson(doc, json);
    
    // Publish with retain flag set to true for persistence
    bool success = client.publish(configTopic, json);
    
    if (success) {
        this->logger->info("Published Home Assistant autodiscovery config");
    } else {
        this->logger->warning("Failed to publish Home Assistant config");
    }
    
    // Also create an absolute measurement sensor
    configTopic = "homeassistant/sensor/" + deviceId + "/depth/config";
    doc.clear();
    
    doc["state_topic"] = mqttTopic;
    doc["value_template"] = "{{ value_json.absolute | round(2) }}";
    doc["unit_of_measurement"] = "m";
    doc["name"] = "Water Depth";
    doc["unique_id"] = deviceId + "_depth";
    doc["device_class"] = "distance";
    doc["icon"] = "mdi:ruler";
    
    // Re-add device information
    device = doc.createNestedObject("device");
    device["identifiers"] = JsonArray().add(deviceId);
    device["name"] = "ESP Analog Distance Meter";
    device["model"] = "ESP8266";
    device["manufacturer"] = "ESP";
    device["sw_version"] = "1.0";
    
    json = "";
    serializeJson(doc, json);
    
    client.publish(configTopic, json);
}

bool MqttClient::run()
{
    client.loop();
    delay(10);

    if (client.connected()) {
        return true;
    }
    
    this->logger->warning("Connection to MQTT lost, reconnecting.");

    return this->connect();
}

void MqttClient::sendDistance(float relative, float absolute, float measured)
{
    DynamicJsonDocument doc(128);
    String json;

    doc["relative"] = relative;
    doc["absolute"] = absolute;
    doc["measured"] = measured;
    serializeJson(doc, json);

    auto ok = client.publish(this->storage->getParameter(Parameter::MQTT_TOPIC_DISTANCE), json);

    if (!ok) {
        this->logger->warning("Failed to publish to MQTT: " + json);
    }
}

#ifndef MY_MQTT_CLIENT_H
#define MY_MQTT_CLIENT_H

#include <MQTT.h>
#include "Storage.h"
#include "Logger.h"

class MqttClient
{
    private:
        Storage* storage;
        Logger* logger;
        MQTTClient client;
        unsigned long lastReconnectAttempt;
        unsigned long reconnectInterval;
        unsigned long lastActivityCheck;
        unsigned long lastMqttActivity;
        bool previouslyConnected;
        const unsigned int KEEPALIVE_INTERVAL = 15000; // 15 seconds

        bool connectMqtt();
        void updateActivityTimestamp();
        void publishHomeAssistantAutoconfig();

    public:
        MqttClient(Storage* storage, Logger* logger);
        void begin();
        bool run();
        void sendDistance(float relative, float absolute, float measured);
};

#endif
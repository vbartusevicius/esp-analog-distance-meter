
#ifndef STATS_H
#define STATS_H

#include <Arduino.h>
#include <IPAddress.h>

class Stats
{
    public:
        String wifiSignal = "";
        String ipAddress = "";
        String network = "";
        String uptime = "";
        String relativeDistance = "";
        float fractionalDistance = 0.0;
        float measurement = 0.0;

        bool mqttConnected = false;
        bool sensorConnected = false;

    public:
        Stats();
        void updateStats(
            float measurement, 
            float relativeDistance, 
            bool mqttConnected
        );
};

#endif
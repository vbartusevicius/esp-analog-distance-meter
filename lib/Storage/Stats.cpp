#include "Stats.h"
#include "TimeHelper.h"
#include <ESP8266WiFi.h>

Stats::Stats() {}

void Stats::updateStats(
    float measurement, 
    float distance, 
    bool mqttConnected
)
{
    this->measurement = measurement;
    this->fractionalDistance = distance;
    this->mqttConnected = mqttConnected;

    if (measurement > 0.0) {
        this->sensorConnected = true;
    } else {
        this->sensorConnected = false;
    }

    this->ipAddress = String(WiFi.localIP().toString());
    this->network = String(WiFi.SSID());
    this->wifiSignal = String(WiFi.RSSI());

    char value[8];
    snprintf(value, sizeof(value), "%.0f%%", distance * 100);
    this->relativeDistance = String(value);

    char buffer[32];
    TimeHelper::getUptime(buffer);
    this->uptime = String(buffer);
}

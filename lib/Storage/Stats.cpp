#include "Stats.h"
#include "TimeHelper.h"
#include <ESP8266WiFi.h>

Stats::Stats() {}

void Stats::updateStats(
    float measurement, 
    float distance,
    float absoluteDistance,
    bool mqttConnected,
    bool sensorConnected
)
{
    this->measurement = measurement;
    this->fractionalDistance = distance;
    this->absoluteDistance = absoluteDistance;
    this->mqttConnected = mqttConnected;
    this->sensorConnected = sensorConnected;

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

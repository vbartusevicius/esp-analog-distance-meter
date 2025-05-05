#ifndef WIFI_CONNECTOR_H
#define WIFI_CONNECTOR_H

#include <WiFiManager.h>
#include "Logger.h"

class WifiConnector
{
    private:
        WiFiManager wm;
        Logger* logger;
        const char* appName;

    public:
        WifiConnector(Logger* logger);
        void run();
        bool begin();
        void resetSettings();
        const char* getAppName();
};

#endif
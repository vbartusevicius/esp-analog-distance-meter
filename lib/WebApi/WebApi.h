#ifndef WEB_API_H
#define WEB_API_H

#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <vector>
#include "Logger.h"
#include "Storage.h"
#include "Stats.h"

using std::vector;

typedef void (*ResetCallback)();

class WebApi {
    private:
        AsyncWebServer server;
        AsyncWebSocket ws;
        Storage* storage;
        Logger* logger;
        ResetCallback resetCallback;
        unsigned int statsUpdateInterval;
        unsigned int logUpdateInterval;
        size_t lastLogCount;
        
        Stats* currentStats;
        float currentAbsoluteDistance;

    public:
        WebApi(Storage* storage, Logger* logger, ResetCallback resetCallback);
        
        void begin();
        void run(Stats* stats);
        
    private:
        void setupStaticFiles();
        void setupApiEndpoints();
        void setupWebSocket();
        
        void handleWebSocketMessage(AsyncWebSocketClient* client, void* arg, uint8_t* data, size_t len, Stats* stats);
        void broadcastStats(Stats* stats);
        void broadcastLogs(const vector<String>& logs);
        void sendLogHistory(AsyncWebSocketClient* client);
};

#endif

#include "WebApi.h"
#include "Parameter.h"

WebApi::WebApi(Storage* storage, Logger* logger, ResetCallback resetCallback) 
    : server(80), ws("/ws") {
    this->storage = storage;
    this->logger = logger;
    this->resetCallback = resetCallback;
    this->lastLogCount = 0;
    this->currentStats = nullptr;
}

void WebApi::begin() {
    if (!LittleFS.begin()) {
        this->logger->error("Failed to mount file system");
        return;
    }
    
    this->setupWebSocket();
    this->setupApiEndpoints();
    this->setupStaticFiles();
    
    server.begin();
    this->logger->info("Web server started");
}

void WebApi::setupStaticFiles() {
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    
    server.onNotFound([this](AsyncWebServerRequest *request) {
        this->logger->warning("File not found: " + request->url());
        request->send(404, "text/plain", "Not found");
    });
}

void WebApi::setupApiEndpoints() {
    server.on("/api/v1/config", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument doc(1024);
        
        doc["sensor_range"] = this->storage->getParameter(Parameter::SENSOR_RANGE, "5");
        doc["distance_empty"] = this->storage->getParameter(Parameter::DISTANCE_EMPTY, "10");
        doc["distance_full"] = this->storage->getParameter(Parameter::DISTANCE_FULL, "100");
        doc["avg_sample_count"] = this->storage->getParameter(Parameter::AVG_SAMPLE_COUNT, "10");
        doc["sampling_interval"] = this->storage->getParameter(Parameter::SAMPLING_INTERVAL, "10");
        doc["maximum_distance_delta"] = this->storage->getParameter(Parameter::MAXIMUM_DISTANCE_DELTA, "15");
        doc["mqtt_host"] = this->storage->getParameter(Parameter::MQTT_HOST, "");
        doc["mqtt_port"] = this->storage->getParameter(Parameter::MQTT_PORT, "1883");
        doc["mqtt_user"] = this->storage->getParameter(Parameter::MQTT_USER, "");
        doc["mqtt_pass"] = this->storage->getParameter(Parameter::MQTT_PASS, "");
        doc["mqtt_device"] = this->storage->getParameter(Parameter::MQTT_DEVICE, "esp_distance_meter");
        doc["mqtt_topic"] = this->storage->getParameter(Parameter::MQTT_TOPIC_DISTANCE, "esp_distance_meter/stat/distance");
        
        serializeJson(doc, *response);
        request->send(response);
        
        this->logger->info("Configuration sent to client");
    });

    server.on("/api/v1/config", HTTP_POST, [](AsyncWebServerRequest *request) {}, 
    nullptr,
    [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        // Handle the JSON POST data
        if (index == 0) {
            // If this is the first chunk, create a new buffer
            request->_tempObject = malloc(total);
        }
        
        // Copy this chunk of data to our buffer
        memcpy((uint8_t*)request->_tempObject + index, data, len);
        
        if (index + len == total) {
            // This is the last chunk, process the complete JSON
            char* jsonData = (char*)request->_tempObject;
            jsonData[total] = '\0'; // Null terminate
            
            DynamicJsonDocument jsonDoc(1024);
            DeserializationError error = deserializeJson(jsonDoc, jsonData);
            
            if (!error) {
                JsonObject jsonObj = jsonDoc.as<JsonObject>();
                
                if (jsonObj.containsKey("sensor_range")) {
                    String value = jsonObj["sensor_range"].as<String>();
                    this->storage->saveParameter(Parameter::SENSOR_RANGE, value);
                }
                
                if (jsonObj.containsKey("distance_empty")) {
                    String value = jsonObj["distance_empty"].as<String>();
                    this->storage->saveParameter(Parameter::DISTANCE_EMPTY, value);
                }
                
                if (jsonObj.containsKey("distance_full")) {
                    String value = jsonObj["distance_full"].as<String>();
                    this->storage->saveParameter(Parameter::DISTANCE_FULL, value);
                }
                
                if (jsonObj.containsKey("avg_sample_count")) {
                    String value = jsonObj["avg_sample_count"].as<String>();
                    this->storage->saveParameter(Parameter::AVG_SAMPLE_COUNT, value);
                }
                
                if (jsonObj.containsKey("sampling_interval")) {
                    String value = jsonObj["sampling_interval"].as<String>();
                    this->storage->saveParameter(Parameter::SAMPLING_INTERVAL, value);
                }
                
                if (jsonObj.containsKey("maximum_distance_delta")) {
                    String value = jsonObj["maximum_distance_delta"].as<String>();
                    this->storage->saveParameter(Parameter::MAXIMUM_DISTANCE_DELTA, value);
                }
                
                if (jsonObj.containsKey("mqtt_host")) {
                    String value = jsonObj["mqtt_host"].as<String>();
                    this->storage->saveParameter(Parameter::MQTT_HOST, value);
                }
                
                if (jsonObj.containsKey("mqtt_port")) {
                    String value = jsonObj["mqtt_port"].as<String>();
                    this->storage->saveParameter(Parameter::MQTT_PORT, value);
                }
                
                if (jsonObj.containsKey("mqtt_user")) {
                    String value = jsonObj["mqtt_user"].as<String>();
                    this->storage->saveParameter(Parameter::MQTT_USER, value);
                }
                
                if (jsonObj.containsKey("mqtt_pass")) {
                    String value = jsonObj["mqtt_pass"].as<String>();
                    this->storage->saveParameter(Parameter::MQTT_PASS, value);
                }
                
                if (jsonObj.containsKey("mqtt_device")) {
                    String value = jsonObj["mqtt_device"].as<String>();
                    this->storage->saveParameter(Parameter::MQTT_DEVICE, value);
                }

                if (jsonObj.containsKey("mqtt_topic")) {
                    String value = jsonObj["mqtt_topic"].as<String>();
                    this->storage->saveParameter(Parameter::MQTT_TOPIC_DISTANCE, value);
                }
                
                this->logger->info("Configuration saved");
                request->send(200, "application/json", "{\"status\":\"ok\"}");
            } else {
                this->logger->error("Failed to parse JSON");
                request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
            }
            
            // Free the buffer
            free(request->_tempObject);
        }
    });

    server.on("/api/v1/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        DynamicJsonDocument doc(512);
        
        doc["water_level"] = this->currentStats->absoluteDistance;
        doc["water_percent"] = this->currentStats->relativeDistance;
        doc["measured_distance"] = this->currentStats->measurement;
        doc["mqtt_connected"] = this->currentStats->mqttConnected;
        doc["wifi_network"] = this->currentStats->network;
        doc["wifi_signal"] = this->currentStats->wifiSignal;
        doc["ip_address"] = this->currentStats->ipAddress;
        doc["uptime"] = this->currentStats->uptime;
        doc["sensor_connected"] = this->currentStats->sensorConnected;
        
        serializeJson(doc, *response);
        request->send(response);
    });

    server.on("/api/v1/restart", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"status\":\"restarting\"}");
        delay(500);
        ESP.restart();
    });

    server.on("/api/v1/reset", HTTP_POST, [this](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"status\":\"resetting\"}");
        this->resetCallback();
    });
}

void WebApi::setupWebSocket() {
    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, 
                AwsEventType type, void *arg, uint8_t *data, size_t len) {
        switch (type) {
            case WS_EVT_CONNECT:
                this->logger->info("WebSocket client connected: " + String(client->id()));
                break;
            case WS_EVT_DISCONNECT:
                this->logger->info("WebSocket client disconnected");
                break;
            case WS_EVT_DATA:
                if (this->currentStats != nullptr) {
                    this->handleWebSocketMessage(client, arg, data, len, this->currentStats);
                }
                break;
            case WS_EVT_PONG:
            case WS_EVT_ERROR:
                break;
        }
    });
    
    server.addHandler(&ws);
}

void WebApi::handleWebSocketMessage(AsyncWebSocketClient* client, void* arg, uint8_t* data, size_t len, Stats* stats) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        // Create a properly null-terminated C-string from the WebSocket data
        char* cstr = new char[len + 1];
        memcpy(cstr, data, len);
        cstr[len] = 0;
        
        // Now safely convert to String
        String message = String(cstr);
        delete[] cstr; // Clean up
        
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, message);
        
        if (error) {
            this->logger->error("Error parsing WebSocket message: " + String(error.c_str()));
            return;
        }
        
        if (doc.containsKey("event")) {
            String event = doc["event"].as<String>();
            
            if (event == "request_status") {
                DynamicJsonDocument statusDoc(512);
                statusDoc["event"] = "stats_update";
                statusDoc["water_level"] = stats->absoluteDistance;
                statusDoc["water_percent"] = stats->fractionalDistance;
                statusDoc["measured_distance"] = stats->measurement;
                statusDoc["mqtt_connected"] = stats->mqttConnected;
                
                String response;
                serializeJson(statusDoc, response);
                client->text(response);
            } else if (event == "request_logs") {
                this->sendLogHistory(client);
            }
        }
    }
}

void WebApi::broadcastStats(Stats* stats) {
    if (ws.count() > 0) {
        DynamicJsonDocument doc(512); // Increased size for additional fields
        doc["event"] = "stats_update";
        
        // Main water level data
        doc["water_level"] = stats->absoluteDistance;
        doc["water_percent"] = stats->fractionalDistance;
        doc["measured_distance"] = stats->measurement;
        
        // Connection statuses
        doc["mqtt_connected"] = stats->mqttConnected;
        doc["sensor_connected"] = stats->sensorConnected;
        
        // WiFi information
        doc["wifi_network"] = stats->network;
        doc["wifi_signal"] = stats->wifiSignal;
        doc["ip_address"] = stats->ipAddress;
        
        // System information
        doc["uptime"] = stats->uptime;
        
        String message;
        serializeJson(doc, message);
        ws.textAll(message);
    }
}

void WebApi::broadcastLogs(const vector<String>& logs) {
    if (ws.count() == 0 || logs.empty()) {
        return;
    }
    
    if (logs.size() <= lastLogCount) {
        return;
    }
    
    DynamicJsonDocument logsDoc(1024);
    logsDoc["event"] = "log_batch";
    JsonArray logsArray = logsDoc.createNestedArray("messages");
    
    for (size_t i = lastLogCount; i < logs.size(); i++) {
        logsArray.add(logs[i]);
    }
    
    lastLogCount = logs.size();
    
    String jsonMessage;
    serializeJson(logsDoc, jsonMessage);
    ws.textAll(jsonMessage);
}

void WebApi::sendLogHistory(AsyncWebSocketClient* client) {
    if (this->logger) {
        vector<String> logBuffer = this->logger->getBuffer();
        
        if (logBuffer.empty()) {
            return;
        }
        
        DynamicJsonDocument logsDoc(1024);
        logsDoc["event"] = "log_batch";
        JsonArray logs = logsDoc.createNestedArray("messages");
        
        for (auto &logEntry : logBuffer) {
            logs.add(logEntry);
        }
        
        String response;
        serializeJson(logsDoc, response);
        client->text(response);
    }
}

void WebApi::run(Stats* stats) {
    ws.cleanupClients();
    
    this->currentStats = stats;

    broadcastStats(stats);
    broadcastLogs(logger->getBuffer());
}

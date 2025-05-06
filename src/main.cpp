#include "WifiConnector.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <TaskManagerIO.h>
#include <ExecWithParameter.h>
#include <LittleFS.h>

#include "Logger.h"
#include "LedController.h"
#include "WebApi.h"
#include "Storage.h"
#include "MqttClient.h"
#include "Meter.h"
#include "DistanceCalculator.h"
#include "Display.h"
#include "Aggregator.h"
#include "Parameter.h"

WiFiClient network;
Display display;

float measuredDistance = 0.0;
float relativeDistance = 0.0;
float absoluteDistance = 0.0;
bool mqttConnected = false;
bool sensorConnected = false;
bool wifiConnected = false;

Logger* logger;
WifiConnector* wifi;
LedController* led;
WebApi* webApi;
Storage* storage;
MqttClient* mqtt;
Meter* meter;
DistanceCalculator* calculator;
Stats* stats;
Aggregator* aggregator;

void resetCallback() {
    wifi->resetSettings();
    storage->reset();
    ESP.eraseConfig();

    delay(2000);
    ESP.restart();
}

void setup()
{
    Serial.begin(9600);
    ArduinoOTA.setPort(8266);
    ArduinoOTA.begin();

    while (!Serial && !Serial.available()) {
    }

    logger = new Logger(&Serial, "System");
    storage = new Storage();
    wifi = new WifiConnector(logger);
    led = new LedController();
    mqtt = new MqttClient(storage, logger);
    webApi = new WebApi(storage, logger, &resetCallback);
    meter = new Meter(logger, storage);
    calculator = new DistanceCalculator(storage);
    stats = new Stats();
    aggregator = new Aggregator(storage, logger);

    wifiConnected = wifi->begin();
    storage->begin();
    webApi->begin();
    mqtt->begin();

    taskManager.schedule(repeatMillis(500), [] { wifi->run(); });
    taskManager.schedule(repeatSeconds(1), [] { webApi->run(stats); });

    if (!wifiConnected) {
        display.displayFirstStep(wifi->getAppName());
        return;
    }
    if (storage->isEmpty()) {
        display.displaySecondStep(WiFi.localIP().toString().c_str());
        return;
    }

    taskManager.schedule(repeatSeconds(1), [] { led->run(); });
    taskManager.schedule(repeatMillis(500), [] { mqttConnected = mqtt->run(); });
    taskManager.schedule(repeatSeconds(1), [] { stats->updateStats(
        measuredDistance,
        relativeDistance,
        absoluteDistance,
        mqttConnected,
        sensorConnected
    ); });

    taskManager.schedule(
        repeatSeconds(atoi(storage->getParameter(Parameter::SAMPLING_INTERVAL, "10").c_str())),
        [] {            
            measuredDistance = aggregator->aggregate(meter->measure());
            relativeDistance = calculator->getRelative(measuredDistance);
            absoluteDistance = calculator->getAbsolute(measuredDistance);

            sensorConnected = meter->checkSensorConnection();
        }
    );
    
    taskManager.schedule(repeatSeconds(1), [] {
        mqtt->sendDistance(relativeDistance, absoluteDistance, measuredDistance);
        display.run(stats);
    });
}

void loop()
{
    ArduinoOTA.handle();
    taskManager.runLoop();
}

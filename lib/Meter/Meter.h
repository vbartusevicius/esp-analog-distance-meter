#ifndef METER_H
#define METER_H

#include <Arduino.h>
#include "Logger.h"
#include "Storage.h"

class Meter
{
    private:
        static constexpr int ANALOG_PIN = A0;
        static constexpr float VOLTAGE_REF = 3.3;
        
        static constexpr float MIN_CURRENT_MA = 4.0; 
        static constexpr float MAX_CURRENT_MA = 20.0;
        static constexpr float FAULT_CURRENT_MA = 4.17;

        Logger* logger;
        Storage* storage;
        bool sensorConnected = false;

    public:
        Meter(Logger* logger, Storage* storage);
        float measure();
        bool checkSensorConnection();
        
    private:
        bool isSensorConnected(float voltage);
        float voltageToDistance(float voltage);
        float voltageToCurrentMA(float voltage);
};

#endif
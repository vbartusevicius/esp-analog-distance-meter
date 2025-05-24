#include "Meter.h"
#include "Parameter.h"

Meter::Meter(Logger* logger, Storage* storage)
{
    this->logger = logger;
    this->storage = storage;
    
    pinMode(Meter::ANALOG_PIN, INPUT);
}

/*
 * returns the measured distance from the sensor in meters
 */
float Meter::measure()
{
    int rawValue = analogRead(Meter::ANALOG_PIN);   
    float voltage = (rawValue / 1023.0) * Meter::VOLTAGE_REF;

    this->sensorConnected = isSensorConnected(voltage);
    float distance = 0.0;
    
    if (this->sensorConnected) {
        distance = voltageToDistance(voltage);
    }
    
    float current = voltageToCurrentMA(voltage);
    this->logger->info("Measurement taken. Raw: " + String(rawValue) + ", Voltage: " + String(voltage) + "V, Current: " + 
                      String(current) + "mA, Sensor: " + String(this->sensorConnected ? "connected" : "disconnected") + 
                      ", Distance: " + String(distance) + "m.");

    return distance;
}

float Meter::voltageToCurrentMA(float voltage)
{
    float current = Meter::MIN_CURRENT_MA + (voltage / Meter::VOLTAGE_REF) * (Meter::MAX_CURRENT_MA - Meter::MIN_CURRENT_MA);
    return current;
}

float Meter::voltageToDistance(float voltage)
{
    float sensorRange = atof(this->storage->getParameter(Parameter::SENSOR_RANGE, "5").c_str());
    float current = voltageToCurrentMA(voltage);
    
    float distance = ((current - Meter::MIN_CURRENT_MA) / (Meter::MAX_CURRENT_MA - Meter::MIN_CURRENT_MA)) * sensorRange;
    
    return distance;
}

bool Meter::isSensorConnected(float voltage)
{
    float current = voltageToCurrentMA(voltage);

    return current >= Meter::FAULT_CURRENT_MA;
}

bool Meter::checkSensorConnection()
{
    return this->sensorConnected;
}

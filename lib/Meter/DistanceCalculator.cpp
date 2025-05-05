#include "DistanceCalculator.h"
#include "Parameter.h"

DistanceCalculator::DistanceCalculator(Storage* storage)
{
    this->storage = storage;
}
/*
 * returns absolute height of water column in meters
 */
float DistanceCalculator::getAbsolute(float sensorToWaterDistance)
{
    float sensorHeightFromBottom = atof(this->storage->getParameter(Parameter::DISTANCE_EMPTY).c_str()) / 100;
    float waterLevel = sensorHeightFromBottom - sensorToWaterDistance;

    return (waterLevel < 0) ? 0.0 : waterLevel; 
}

/*
 * returns relative height of water column in percentage fraction (0.0 - 1.0)
 */
float DistanceCalculator::getRelative(float sensorToWaterDistance)
{
    float maxWaterDepth = atof(this->storage->getParameter(Parameter::DISTANCE_FULL).c_str()) / 100;
    float currentWaterLevel = this->getAbsolute(sensorToWaterDistance);
    
    return (maxWaterDepth > 0) ? (currentWaterLevel / maxWaterDepth) : 0.0;
}

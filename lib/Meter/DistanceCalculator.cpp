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
    float relative = this->getRelative(sensorToWaterDistance);
    float maxWaterDepth = atof(this->storage->getParameter(Parameter::DISTANCE_FULL).c_str()) / 100;
    
    return relative * maxWaterDepth;
}

/*
 * returns relative height of water column in percentage fraction (0.0 - 1.0)
 */
float DistanceCalculator::getRelative(float sensorToWaterDistance)
{
    float emptyDistance = atof(this->storage->getParameter(Parameter::DISTANCE_EMPTY).c_str()) / 100;
    float fullDistance = atof(this->storage->getParameter(Parameter::DISTANCE_FULL).c_str()) / 100;
    
    // Calculate percentage filled
    if (sensorToWaterDistance <= emptyDistance) {
        return 0.0;
    } else if (sensorToWaterDistance >= fullDistance) {
        return 1.0;
    } else {
        return (sensorToWaterDistance - emptyDistance) / (fullDistance - emptyDistance);
    }
}

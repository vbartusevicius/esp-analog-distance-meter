#include "DistanceCalculator.h"
#include "Parameter.h"

DistanceCalculator::DistanceCalculator(Storage* storage)
{
    this->storage = storage;
}
/*
 * returns absolute height of measured column in meters
 */
float DistanceCalculator::getAbsolute(float distance)
{
    float distanceEmpty = atof(this->storage->getParameter(Parameter::DISTANCE_EMPTY).c_str()) / 100;
    float absolute = distanceEmpty - distance;

    return (absolute < 0) ? 0.0 : absolute; 
}

/*
 * returns relative height of measured column in percentage fraction
 */
float DistanceCalculator::getRelative(float distance)
{
    float distanceEmpty = atof(this->storage->getParameter(Parameter::DISTANCE_EMPTY).c_str()) / 100;
    float distanceFull = atof(this->storage->getParameter(Parameter::DISTANCE_FULL).c_str()) / 100;

    return this->getAbsolute(distance) / (distanceEmpty - distanceFull);
}

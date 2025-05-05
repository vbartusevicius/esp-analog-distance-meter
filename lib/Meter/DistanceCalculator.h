#ifndef DISTANCE_CALCULATOR_H
#define DISTANCE_CALCULATOR_H

#include "Storage.h"

class DistanceCalculator
{
    private:
        Storage* storage;

    public:
        DistanceCalculator(Storage* storage);
        float getRelative(float distance);
        float getAbsolute(float distance);
};

#endif
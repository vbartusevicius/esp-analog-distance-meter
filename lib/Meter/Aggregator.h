#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <queue>
#include "Storage.h"
#include "Logger.h"

using namespace std;

class Aggregator
{
    private:
        vector<float> buffer;
        Storage* storage;
        Logger* logger;
    
    public:
        Aggregator(Storage* storage, Logger* logger);
        float aggregate(float value);
        float calculateAverage();

};

#endif
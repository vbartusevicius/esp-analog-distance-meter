#include "Aggregator.h"
#include "Parameter.h"

Aggregator::Aggregator(Storage* storage, Logger* logger)
{
    this->storage = storage;
    this->logger = logger;
}

float Aggregator::aggregate(float value)
{
    const int averageWindow = atoi(this->storage->getParameter(Parameter::AVG_SAMPLE_COUNT, "10").c_str());
    const int maxDelta = atoi(this->storage->getParameter(Parameter::MAXIMUM_DISTANCE_DELTA, "15").c_str());

    const float lastValue = this->buffer.back();

    if (this->buffer.size() >= averageWindow) {
        this->buffer.erase(this->buffer.begin());
    }

    float diff = abs(lastValue - value);

    bool deltaOK = value * (maxDelta / 100.0) > diff || this->buffer.size() == 0;
    bool valueOK = round(value * 100.0) > 0;

    if (deltaOK && valueOK) {
        this->buffer.push_back(value);
    }
 
    if (!deltaOK) {
        this->logger->info(
            String("Value ") 
            + value 
            + " exceeds the maximum diff of " 
            + maxDelta 
            + " compared to last value " 
            + lastValue 
            + ", skipping aggregation"
        );
    }
    if (!valueOK) {
        this->logger->info("Zero value given, skipping aggregation");
    }

    return this->calculateAverage();
}

float Aggregator::calculateAverage()
{
    float data = 0.0;

    for (auto &element : this->buffer) {
        data += element;
    }

    return data / this->buffer.size();
}
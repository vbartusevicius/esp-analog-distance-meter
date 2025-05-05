#ifndef PARAMETER_H
#define PARAMETER_H

#include <queue>
using std::vector;

class Parameter
{
    public:
        static char* DISTANCE_EMPTY;
        static char* DISTANCE_FULL;
        static char* MQTT_HOST;
        static char* MQTT_PORT;
        static char* MQTT_USER;
        static char* MQTT_PASS;
        static char* MQTT_DEVICE;
        static char* MQTT_TOPIC_DISTANCE;
        static char* AVG_SAMPLE_COUNT;
        static char* SAMPLING_INTERVAL;
        static char* MAXIMUM_DISTANCE_DELTA;
        static char* SENSOR_RANGE;

        static vector<char*> REQUIRED_PARAMS;
};

#endif

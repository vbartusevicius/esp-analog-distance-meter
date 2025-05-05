#include "Parameter.h"

using std::vector;

char* Parameter::DISTANCE_EMPTY = "distance_empty";
char* Parameter::DISTANCE_FULL = "distance_full";
char* Parameter::MQTT_HOST = "mqtt_host";
char* Parameter::MQTT_PORT = "mqtt_port";
char* Parameter::MQTT_USER = "mqtt_user";
char* Parameter::MQTT_PASS = "mqtt_pass";
char* Parameter::MQTT_DEVICE = "mqtt_device";
char* Parameter::MQTT_TOPIC_DISTANCE = "mqtt_topic_distance";
char* Parameter::AVG_SAMPLE_COUNT = "avg_sample_count";
char* Parameter::SAMPLING_INTERVAL = "sampling_interval";
char* Parameter::MAXIMUM_DISTANCE_DELTA = "max_distance_delta";

vector<char*> Parameter::REQUIRED_PARAMS = {
    Parameter::DISTANCE_EMPTY,
    Parameter::DISTANCE_FULL,
    Parameter::MQTT_HOST,
    Parameter::MQTT_PORT,
    Parameter::MQTT_DEVICE
};
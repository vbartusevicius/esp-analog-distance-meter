#ifndef LOGGER_H
#define LOGGER_H

#include <queue>
#include <ArduinoLog.h>
#include "TimeHelper.h"

using namespace std;

class Logger
{
    private:
        Logging log;
        const char* channel;
        vector<String> buffer;

    public:
        Logger(Stream* stream, const char* channel);
        void info(String message);
        void warning(String message);
        void error(String message);
        void debug(String message);
        vector<String> getBuffer();
    
    private:
        void formatMessage(const char* message, const char* level, char* buffer);
};

#endif

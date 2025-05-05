#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include "Stats.h"

class Display
{
    private:
        int cursorOffset = 0;
        int displayWidth;

    public:
        Display();
        void run(Stats* stats);
        void displayFirstStep(const char* appName);
        void displaySecondStep(const char* ipAddress);
    
    private:
        void renderProgress(Stats* stats);
        void renderNetwork(Stats* stats);
        void renderBoolStatus(String name, bool status);
        void renderUptime(Stats* stats);
};

#endif
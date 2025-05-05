#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

class LedController
{
    private:
        bool lastState;

    public:
        LedController();
        void run();
};

#endif
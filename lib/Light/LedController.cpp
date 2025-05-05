#include "LedController.h"

LedController::LedController()
{
    this->lastState = false;

    pinMode(LED_BUILTIN, OUTPUT);
}

void LedController::run()
{
    bool state = !this->lastState;
    digitalWrite(LED_BUILTIN, (int) state);

    this->lastState = state;
}

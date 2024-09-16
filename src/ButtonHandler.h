#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include "config.h"

class ButtonHandler {
public:
    static void initialize();
    static void checkButtons();
    static bool isButton1Pressed();
    static bool isButton2Pressed();

private:
    static bool button1State;
    static bool button2State;
};

#endif // BUTTON_HANDLER_H
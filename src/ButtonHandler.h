#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "config.h"
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

class ButtonHandler {
public:
    ButtonHandler();
    void init();
    void update();
    bool isPressed(int buttonIndex);
    bool isAnyButtonPressed();

private:
    Adafruit_MCP23X17 mcp;
    static const int NUM_BUTTONS = 9; // 8 arcade buttons + 1 M5STAMP button
    bool buttonStates[NUM_BUTTONS];
    bool previousButtonStates[NUM_BUTTONS];

    void readMCP23017Buttons();
    void readM5StampButton();
};

#endif // BUTTON_HANDLER_H
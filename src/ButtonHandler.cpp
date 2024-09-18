#include "ButtonHandler.h"

ButtonHandler::ButtonHandler() : mcp() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttonStates[i] = false;
        previousButtonStates[i] = false;
    }
}

void ButtonHandler::init() {
    Wire.begin(MCP23017_SDA_PIN, MCP23017_SCL_PIN);
    if (!mcp.begin_I2C()) {
        Serial.println("MCP23017 initialization failed.");
    }

    // Configure MCP23017 pins as inputs with pull-ups
    for (int i = 0; i < 8; i++) {
        mcp.pinMode(i, INPUT_PULLUP);
    }

    // Configure M5STAMP button
    pinMode(M5STAMP_BUTTON_PIN, INPUT_PULLUP);
}

void ButtonHandler::update() {
    // Store previous button states
    for (int i = 0; i < NUM_BUTTONS; i++) {
        previousButtonStates[i] = buttonStates[i];
    }

    readMCP23017Buttons();
    readM5StampButton();
}

void ButtonHandler::readMCP23017Buttons() {
    for (int i = 0; i < 8; i++) {
        buttonStates[i] = !mcp.digitalRead(i); // Active low
    }
}

void ButtonHandler::readM5StampButton() {
    buttonStates[8] = !digitalRead(M5STAMP_BUTTON_PIN); // Active low
}

bool ButtonHandler::isPressed(int buttonIndex) {
    if (buttonIndex >= 0 && buttonIndex < NUM_BUTTONS) {
        return buttonStates[buttonIndex] && !previousButtonStates[buttonIndex];
    }
    return false;
}

bool ButtonHandler::isAnyButtonPressed() {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (isPressed(i)) {
            return true;
        }
    }
    return false;
}
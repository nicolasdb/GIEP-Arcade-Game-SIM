#include "ButtonHandler.h"
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp;
bool ButtonHandler::button1State = false;
bool ButtonHandler::button2State = false;

void ButtonHandler::initialize() {
    Wire.begin(MCP_SDA_PIN, MCP_SCL_PIN);
    mcp.begin_I2C();
    mcp.pinMode(0, INPUT_PULLUP);
    mcp.pinMode(1, INPUT_PULLUP);
}

void ButtonHandler::checkButtons() {
    button1State = !mcp.digitalRead(0);
    button2State = !mcp.digitalRead(1);
}

bool ButtonHandler::isButton1Pressed() {
    return button1State;
}

bool ButtonHandler::isButton2Pressed() {
    return button2State;
}
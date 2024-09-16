#pragma once

#include <Arduino.h>
#include <Wire.h>

class MCP23017Handler {
public:
    MCP23017Handler(uint8_t address = 0x20);
    void begin();
    uint8_t readButtons();
    void setLEDs(uint8_t ledStates);
    void setLED(uint8_t ledPin, bool state);

private:
    uint8_t _address;
    static const uint8_t IODIRA = 0x00;
    static const uint8_t IODIRB = 0x01;
    static const uint8_t GPPUA = 0x0C;
    static const uint8_t GPIOA = 0x12;
    static const uint8_t GPIOB = 0x13;

    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
};
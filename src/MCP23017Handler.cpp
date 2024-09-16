#include "MCP23017Handler.h"

MCP23017Handler::MCP23017Handler(uint8_t address)
    : _address(address) {
}

void MCP23017Handler::begin() {
    Wire.begin();
}

bool MCP23017Handler::readButton(uint8_t buttonIndex) {
    uint8_t buttonState = readRegister(GPIOA);
    return !(buttonState & (1 << buttonIndex));
}

void MCP23017Handler::setLED(uint8_t ledIndex, bool state) {
    uint8_t ledState = readRegister(GPIOB);
    if (state) {
        ledState |= (1 << ledIndex);
    } else {
        ledState &= ~(1 << ledIndex);
    }
    writeRegister(GPIOB, ledState);
}

void MCP23017Handler::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t MCP23017Handler::readRegister(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(_address, (uint8_t)1);
    return Wire.read();
}
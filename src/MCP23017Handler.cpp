#include "MCP23017Handler.h"

MCP23017Handler::MCP23017Handler(uint8_t address) : _address(address) {}

void MCP23017Handler::begin() {
    Wire.begin();
    // Set GPIOA to inputs with pull-ups for buttons
    writeRegister(IODIRA, 0xFF);
    writeRegister(GPPUA, 0xFF);
    // Set GPIOB to outputs for LEDs
    writeRegister(IODIRB, 0x00);
}

uint8_t MCP23017Handler::readButtons() {
    return readRegister(GPIOA); // Remove inversion
}

void MCP23017Handler::setLEDs(uint8_t ledStates) {
    writeRegister(GPIOB, ledStates);
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
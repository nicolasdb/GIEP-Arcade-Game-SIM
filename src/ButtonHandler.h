#pragma once
#include <Arduino.h>
#include "DebugLogger.h"
#include "MCP23017Handler.h"
#include "config.h"

class ButtonHandler {
public:
    static const uint8_t DEBOUNCE_DELAY = 60; // milliseconds

    ButtonHandler(MCP23017Handler& mcpHandler);
    void update();

    // Add a callback function type
    typedef void (*ButtonCallback)(uint8_t button);

    // Set callback functions
    void setOnButtonPressedCallback(ButtonCallback callback);
    void setOnButtonReleasedCallback(ButtonCallback callback);

private:
    MCP23017Handler& _mcpHandler;
    uint8_t _lastMcpStates;
    uint8_t _mcpButtonStates;
    uint32_t _lastMcpDebounceTime[NUM_MCP_BUTTONS];
    uint8_t _lastBasinGateState;
    uint8_t _basinGateButtonState;
    uint32_t _lastBasinGateDebounceTime;
    uint8_t _lastDebugState;
    uint8_t _debugButtonState;
    uint32_t _lastDebugDebounceTime;

    ButtonCallback _onButtonPressedCallback;
    ButtonCallback _onButtonReleasedCallback;

    void updateMcpButtons(uint32_t now);
    void updateBasinGateButton(uint32_t now);
    void updateDebugButton(uint32_t now);
    void onButtonPressed(uint8_t button);
    void onButtonReleased(uint8_t button);
};
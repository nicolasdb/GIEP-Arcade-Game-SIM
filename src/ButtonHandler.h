#pragma once

#include <Arduino.h>
#include "config.h"
#include "MCP23017Handler.h"

class ButtonHandler {
public:
    static const uint8_t NUM_MCP_BUTTONS = MCP23017Handler::NUM_MCP_BUTTONS;

    typedef void (*ButtonCallback)(uint8_t button);

    ButtonHandler(MCP23017Handler& mcpHandler);
    void update();
    void setOnButtonPressedCallback(ButtonCallback callback);
    void setOnButtonReleasedCallback(ButtonCallback callback);

private:
    MCP23017Handler& _mcpHandler;
    uint8_t _lastMcpStates;
    uint8_t _mcpButtonStates;
    uint32_t _lastMcpDebounceTime[NUM_MCP_BUTTONS];
    
    uint8_t _lastButton9State;
    uint8_t _button9State;
    uint32_t _lastButton9DebounceTime;

    ButtonCallback _onButtonPressedCallback;
    ButtonCallback _onButtonReleasedCallback;

    void updateMcpButtons(uint32_t now);
    void updateButton9(uint32_t now);
    void onButtonPressed(uint8_t button);
    void onButtonReleased(uint8_t button);
};
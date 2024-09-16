#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(MCP23017Handler& mcpHandler)
    : _mcpHandler(mcpHandler), _lastMcpStates(0), _mcpButtonStates(0),
      _lastBasinGateState(HIGH), _basinGateButtonState(HIGH),
      _lastBasinGateDebounceTime(0), _lastDebugState(HIGH),
      _debugButtonState(HIGH), _lastDebugDebounceTime(0),
      _onButtonPressedCallback(nullptr), _onButtonReleasedCallback(nullptr) {
    
    for (int i = 0; i < NUM_MCP_BUTTONS; i++) {
        _lastMcpDebounceTime[i] = 0;
    }
}

void ButtonHandler::setOnButtonPressedCallback(ButtonCallback callback) {
    _onButtonPressedCallback = callback;
}

void ButtonHandler::setOnButtonReleasedCallback(ButtonCallback callback) {
    _onButtonReleasedCallback = callback;
}

void ButtonHandler::update() {
    uint32_t now = millis();
    updateMcpButtons(now);
    updateBasinGateButton(now);
    updateDebugButton(now);
}

void ButtonHandler::updateMcpButtons(uint32_t now) {
    uint8_t currentStates = _mcpHandler.readButtons();
    for (int i = 0; i < NUM_MCP_BUTTONS; i++) {
        if ((currentStates & (1 << i)) != (_lastMcpStates & (1 << i))) {
            _lastMcpDebounceTime[i] = now;
        }

        if ((now - _lastMcpDebounceTime[i]) > DEBOUNCE_DELAY) {
            if ((currentStates & (1 << i)) != (_mcpButtonStates & (1 << i))) {
                _mcpButtonStates ^= (1 << i);
                if (_mcpButtonStates & (1 << i)) {
                    onButtonPressed(i);
                } else {
                    onButtonReleased(i);
                }
            }
        }
    }
    _lastMcpStates = currentStates;
}

void ButtonHandler::updateBasinGateButton(uint32_t now) {
    int reading = digitalRead(BASIN_GATE_BUTTON_PIN);
    if (reading != _lastBasinGateState) {
        _lastBasinGateDebounceTime = now;
    }

    if ((now - _lastBasinGateDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != _basinGateButtonState) {
            _basinGateButtonState = reading;
            if (_basinGateButtonState == LOW) {
                onButtonPressed(NUM_MCP_BUTTONS);
            } else {
                onButtonReleased(NUM_MCP_BUTTONS);
            }
        }
    }
    _lastBasinGateState = reading;
}

void ButtonHandler::updateDebugButton(uint32_t now) {
    int reading = digitalRead(DEBUG_BUTTON_PIN);
    if (reading != _lastDebugState) {
        _lastDebugDebounceTime = now;
    }

    if ((now - _lastDebugDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != _debugButtonState) {
            _debugButtonState = reading;
            if (_debugButtonState == LOW) {
                onButtonPressed(NUM_MCP_BUTTONS + 1);
            } else {
                onButtonReleased(NUM_MCP_BUTTONS + 1);
            }
        }
    }
    _lastDebugState = reading;
}

void ButtonHandler::onButtonPressed(uint8_t button) {
    DebugLogger::debug("Button %d pressed", button);
    if (_onButtonPressedCallback) {
        _onButtonPressedCallback(button);
    }
}

void ButtonHandler::onButtonReleased(uint8_t button) {
    DebugLogger::debug("Button %d released", button);
    if (_onButtonReleasedCallback) {
        _onButtonReleasedCallback(button);
    }
}
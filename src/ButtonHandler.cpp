#include "ButtonHandler.h"
#include "DebugLogger.h"
#include "config.h"

ButtonHandler::ButtonHandler(MCP23017Handler& mcpHandler)
    : _mcpHandler(mcpHandler), _lastMcpStates(0), _mcpButtonStates(0),
      _lastButton9State(HIGH), _button9State(HIGH), _lastButton9DebounceTime(0),
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
    updateButton9(now);
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

void ButtonHandler::updateButton9(uint32_t now) {
    int reading = digitalRead(BUTTON9_PIN);
    if (reading != _lastButton9State) {
        _lastButton9DebounceTime = now;
    }

    if ((now - _lastButton9DebounceTime) > DEBOUNCE_DELAY) {
        if (reading != _button9State) {
            _button9State = reading;
            if (_button9State == LOW) {
                onButtonPressed(NUM_MCP_BUTTONS);
            } else {
                onButtonReleased(NUM_MCP_BUTTONS);
            }
        }
    }
    _lastButton9State = reading;
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
#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(MCP23017Handler& mcpHandler, GameLogic& gameLogic)
    : _mcpHandler(mcpHandler), _gameLogic(gameLogic), _lastMcpStates(0xFF), _mcpButtonStates(0xFF),
      _lastMcpDebounceTime{}, _lastBasinGateState(HIGH), _basinGateButtonState(HIGH),
      _lastBasinGateDebounceTime(0), _lastDebugState(HIGH), _debugButtonState(HIGH),
      _lastDebugDebounceTime(0) {
    
    pinMode(BASIN_GATE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BASIN_GATE_LED_PIN, OUTPUT);
    pinMode(DEBUG_BUTTON_PIN, INPUT_PULLUP);

    DebugLogger::info("ButtonHandler initialized. Basin Gate Button Pin: %d, LED Pin: %d", BASIN_GATE_BUTTON_PIN, BASIN_GATE_LED_PIN);
}

void ButtonHandler::update() {
    uint32_t now = millis();
    updateMcpButtons(now);
    updateBasinGateButton(now);
    updateDebugButton(now);
}

void ButtonHandler::updateMcpButtons(uint32_t now) {
    uint8_t reading = _mcpHandler.readButtons();
    if (reading != _lastMcpStates) {
        for (uint8_t i = 0; i < NUM_MCP_BUTTONS; i++) {
            if ((reading & (1 << i)) != (_lastMcpStates & (1 << i))) {
                _lastMcpDebounceTime[i] = now;
            }
        }
    }
    for (uint8_t i = 0; i < NUM_MCP_BUTTONS; i++) {
        if ((now - _lastMcpDebounceTime[i]) > DEBOUNCE_DELAY) {
            if ((reading & (1 << i)) != (_mcpButtonStates & (1 << i))) {
                if (reading & (1 << i)) {
                    onButtonReleased(i);
                } else {
                    onButtonPressed(i);
                }
                _mcpButtonStates ^= (1 << i);
            }
        }
    }
    _lastMcpStates = reading;
}

void ButtonHandler::updateBasinGateButton(uint32_t now) {
    int reading = digitalRead(BASIN_GATE_BUTTON_PIN);
    DebugLogger::debug("Basin Gate Button reading: %d", reading);
    
    if (reading != _lastBasinGateState) {
        _lastBasinGateDebounceTime = now;
        DebugLogger::debug("Basin Gate Button state changed. New state: %d", reading);
    }
    
    if ((now - _lastBasinGateDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != _basinGateButtonState) {
            _basinGateButtonState = reading;
            DebugLogger::debug("Basin Gate Button debounced. New state: %d", _basinGateButtonState);
            
            if (_basinGateButtonState == LOW) {
                onBasinGateButtonPressed();
            } else {
                onBasinGateButtonReleased();
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
                onDebugButtonPressed();
            }
        }
    }
    _lastDebugState = reading;
}

void ButtonHandler::onButtonPressed(uint8_t button) {
    DebugLogger::info("GIEP Button %d pressed", button + 1);
    _gameLogic.handleButton(button, true);
}

void ButtonHandler::onButtonReleased(uint8_t button) {
    DebugLogger::info("GIEP Button %d released", button + 1);
    _gameLogic.handleButton(button, false);
}

void ButtonHandler::onBasinGateButtonPressed() {
    DebugLogger::info("Basin Gate Button pressed");
    digitalWrite(BASIN_GATE_LED_PIN, HIGH);
    _gameLogic.handleBasinGateButton(true);
    DebugLogger::debug("Basin Gate LED turned ON");
}

void ButtonHandler::onBasinGateButtonReleased() {
    DebugLogger::info("Basin Gate Button released");
    digitalWrite(BASIN_GATE_LED_PIN, LOW);
    _gameLogic.handleBasinGateButton(false);
    DebugLogger::debug("Basin Gate LED turned OFF");
}

void ButtonHandler::onDebugButtonPressed() {
    DebugLogger::info("Debug button pressed");
    DebugLogger::info("Current game state: %s", _gameLogic.getStateString());
    // Add more debug information as needed
}
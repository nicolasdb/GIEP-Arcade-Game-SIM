#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(const uint8_t* gamePins, uint8_t debugPin, GameLogic& gameLogic)
    : _gamePins(gamePins), _debugPin(debugPin), _gameLogic(gameLogic) {
    for (uint8_t i = 0; i < NUM_GAME_BUTTONS; i++) {
        pinMode(_gamePins[i], INPUT_PULLUP);
        _lastGameStates[i] = HIGH;
        _gameButtonStates[i] = HIGH;
        _lastGameDebounceTime[i] = 0;
    }
    pinMode(_debugPin, INPUT_PULLUP);
    _lastDebugState = HIGH;
    _debugButtonState = HIGH;
    _lastDebugDebounceTime = 0;
}

void ButtonHandler::update() {
    uint32_t now = millis();
    updateGameButtons(now);
    updateDebugButton(now);
}

void ButtonHandler::updateGameButtons(uint32_t now) {
    for (uint8_t i = 0; i < NUM_GAME_BUTTONS; i++) {
        int reading = digitalRead(_gamePins[i]);
        if (reading != _lastGameStates[i]) {
            _lastGameDebounceTime[i] = now;
        }
        if ((now - _lastGameDebounceTime[i]) > DEBOUNCE_DELAY) {
            if (reading != _gameButtonStates[i]) {
                _gameButtonStates[i] = reading;
                if (_gameButtonStates[i] == LOW) {
                    onGameButtonPressed(i);
                } else {
                    onGameButtonReleased(i);
                }
            }
        }
        _lastGameStates[i] = reading;
    }
}

void ButtonHandler::updateDebugButton(uint32_t now) {
    int reading = digitalRead(_debugPin);
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

void ButtonHandler::onGameButtonPressed(uint8_t button) {
    if (button < 8) {
        DebugLogger::info("GIEP Button %d pressed", button + 1);
    } else if (button == 8) {
        DebugLogger::info("Basin Gate Button pressed");
    }
    _gameLogic.handleButton(button, true);
}

void ButtonHandler::onGameButtonReleased(uint8_t button) {
    if (button < 8) {
        DebugLogger::info("GIEP Button %d released", button + 1);
    } else if (button == 8) {
        DebugLogger::info("Basin Gate Button released");
    }
    _gameLogic.handleButton(button, false);
}

void ButtonHandler::onDebugButtonPressed() {
    DebugLogger::info("Debug button pressed");
    DebugLogger::info("Current game state: %s", _gameLogic.getStateString());
    // Add more debug information as needed
}
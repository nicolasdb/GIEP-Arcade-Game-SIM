
#pragma once
#include <Arduino.h>
#include "DebugLogger.h"
#include "GameLogic.h"
#include "config.h"

class ButtonHandler {
public:
    static const uint8_t DEBOUNCE_DELAY = 60; // milliseconds

    ButtonHandler(const uint8_t* gamePins, uint8_t debugPin, GameLogic& gameLogic);
    void update();

private:
    const uint8_t* _gamePins;
    uint8_t _debugPin;
    GameLogic& _gameLogic;
    uint8_t _lastGameStates[NUM_GAME_BUTTONS];
    uint8_t _gameButtonStates[NUM_GAME_BUTTONS];
    uint32_t _lastGameDebounceTime[NUM_GAME_BUTTONS];
    uint8_t _lastDebugState;
    uint8_t _debugButtonState;
    uint32_t _lastDebugDebounceTime;

    void updateGameButtons(uint32_t now);
    void updateDebugButton(uint32_t now);
    void onGameButtonPressed(uint8_t button);
    void onGameButtonReleased(uint8_t button);
    void onDebugButtonPressed();
};
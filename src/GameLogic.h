#pragma once
#include <Arduino.h>
#include "Scene.h"
#include "DebugLogger.h"

enum class GameState {
    IDLE,
    RAINING,
    HEAVY,
    STORM,
    FLOOD,
    CANAL,
    WIN
};

class GameLogic {
public:
    GameLogic(Scene& scene);
    void update();
    void handleButton(uint8_t buttonIndex, bool isPressed);
    GameState getState() const { return currentState; }
    const char* getStateString() const;

private:
    Scene& scene;
    GameState currentState;
    unsigned long stateStartTime;
    float sewerLevel;
    float basinLevel;
    bool buttonStates[9];  // For 9 buttons

    void transitionState(GameState newState);
    void updateWaterLevels();
    void checkForStateTransition();
    void updateRainIntensity();
    void handleGIEPEffects();
    void handleBasinGate();
    void resetGame(); 
};
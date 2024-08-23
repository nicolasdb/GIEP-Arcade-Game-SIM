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
    void handleBasinGateButton(bool isPressed);
    GameState getState() const { return currentState; }
    const char* getStateString() const;  
    void initializeGameState();

private:
    Scene& scene;
    GameState currentState;
    unsigned long stateStartTime;
    float sewerLevel;
    float basinLevel;
    bool buttonStates[8];  // For 8 GIEP buttons
    bool basinGateOpen;

    void transitionState(GameState newState);
    void updateWaterLevels();
    void checkForStateTransition();
    void updateRainIntensity();
    void handleGIEPEffects();
    void handleBasinGate();
    void resetGame(); 
    void updateWeatherCycle();  
    void handleGIEPButton(uint8_t buttonIndex, bool isPressed);
};
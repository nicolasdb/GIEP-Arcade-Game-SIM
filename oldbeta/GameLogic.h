#pragma once
#include <Arduino.h>
#include "Scene.h"
#include "DebugLogger.h"
#include "SecondaryLEDHandler.h"

enum class GameState {
    WAITING_RAINING,
    WAITING_DRY,
    RAINING,
    HEAVY,
    STORM,
    FLOOD,
    CANAL,
    WIN
};

class GameLogic {
public:
    GameLogic(Scene& scene, SecondaryLEDHandler& secondaryLEDs);
    void update();
    void handleButton(uint8_t buttonIndex, bool isPressed);
    void handleBasinGateButton(bool isPressed);
    GameState getState() const { return currentState; }
    const char* getStateString() const;
    const char* getStateString(GameState state) const;
    void initializeGameState();

private:
    Scene& scene;
    SecondaryLEDHandler& secondaryLEDs;
    GameState currentState;
    unsigned long stateStartTime;
    float sewerLevel;
    float basinLevel;
    bool buttonStates[8];  // For 8 GIEP buttons
    bool basinGateOpen;
    bool gameActive;

    void transitionState(GameState newState);
    void updateWaterLevels();
    void checkForStateTransition();
    void updateRainIntensity();
    void handleGIEPEffects();
    void handleBasinGate();
    void resetGame(); 
    void updateWeatherCycle();  
    void handleGIEPButton(uint8_t buttonIndex, bool isPressed);
    void updateSecondaryLEDs();
    void updateWaitingMode();
    void updateActiveGame();
    void startGame();
    void endGame(GameState endState);
    void updateGIEPAndBasinGateLEDs();
    void checkEndGameTransition();
    void handleWinStateButtonPress(uint8_t buttonIndex); // Add this line
};
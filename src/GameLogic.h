#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "Scene.h"
#include "SecondaryLEDHandler.h"

enum class GameState {
    WAITING_RAINING,
    WAITING_DRY,
    RAINING,
    HEAVY,
    STORM,
    FLOOD,
    BASIN_OVERFLOW,  // Renamed from CANAL
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
    void transitionState(GameState newState);
    void updateWaterLevels();
    void checkForStateTransition();
    void updateRainIntensity();
    void handleGIEPEffects();
    void handleBasinGate();
    void updateWeatherCycle();  
    void handleGIEPButton(uint8_t buttonIndex, bool isPressed);
    void updateSecondaryLEDs();
    void updateWaitingMode();
    void updateActiveGame();
    void startGame();
    void endGame(GameState endState);
    void updateGIEPAndBasinGateLEDs();
    void checkEndGameTransition();
    void resetGameElements();
    void updateEndGameState();  // Added the new function declaration

    Scene& scene;
    SecondaryLEDHandler& secondaryLEDs;
    GameState currentState;
    unsigned long stateStartTime;
    float sewerLevel;
    float basinLevel;
    bool basinGateOpen;
    bool buttonStates[8];
    bool gameActive;
};

#endif // GAME_LOGIC_H
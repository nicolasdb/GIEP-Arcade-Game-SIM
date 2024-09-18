#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "config.h"
#include "ButtonHandler.h"
#include "MatrixConfig.h"
#include "arrays.h"
#include <FastLED.h>

// Forward declarations
class RainSystem;
class Scene;

class GameLogic {
public:
    GameLogic(CRGB* mainLeds, CRGB* secLeds);
    ~GameLogic();
    void init();
    void update();

private:
    ButtonHandler buttonHandler;
    MatrixConfig mainMatrix;
    MatrixConfig secMatrix;
    RainSystem* rainSystem;
    Scene* scene;
    GameState currentState;
    unsigned long gameStartTime;
    unsigned long lastStateChangeTime;
    int waterLevel;
    int pollutionLevel;

    void handleInput();
    void updateGameState();
    void updateGameProgress();
    void updateDisplay();
    void cycleWaitingState();
    bool checkEndGameCondition();
    void startGame();
    void endGame(GameState endState);
    void resetToWaiting();
};

#endif // GAME_LOGIC_H
#include "GameLogic.h"
#include "RainSystem.h"
#include "Scene.h"
#include "arrays.h"

GameLogic::GameLogic(CRGB* mainLeds, CRGB* secLeds)
    : mainMatrix(MAIN_MATRIX_PIN, mainLeds, MAIN_MATRIX_WIDTH, MAIN_MATRIX_HEIGHT),
      secMatrix(SEC_MATRIX_PIN, secLeds, SEC_MATRIX_WIDTH, SEC_MATRIX_HEIGHT),
      currentState(GameState::WAITING_DRY),
      gameStartTime(0),
      lastStateChangeTime(0),
      waterLevel(0),
      pollutionLevel(0) {
    rainSystem = new RainSystem(mainMatrix);
    scene = new Scene(mainMatrix, secMatrix);
}

GameLogic::~GameLogic() {
    delete rainSystem;
    delete scene;
}

void GameLogic::init() {
    buttonHandler.init();
    mainMatrix.init();
    secMatrix.init();
    rainSystem->init();
    scene->init();
    Serial.println("Game initialized");
}

void GameLogic::update() {
    handleInput();
    updateGameState();
    updateGameProgress();
    updateDisplay();
}

void GameLogic::handleInput() {
    buttonHandler.update();
    
    if (currentState == GameState::WAITING_DRY || currentState == GameState::WAITING_RAIN1) {
        if (buttonHandler.isAnyButtonPressed()) {
            startGame();
        }
    } else if (currentState == GameState::RAIN1 || currentState == GameState::RAIN2 || currentState == GameState::RAIN3) {
        // Handle gameplay button presses
        for (int i = 0; i < 9; i++) {
            if (buttonHandler.isPressed(i)) {
                // Implement button action (e.g., activate pumps, filters)
                waterLevel -= 5; // Example: decrease water level
                pollutionLevel -= 3; // Example: decrease pollution level
                Serial.printf("Button %d pressed. Water: %d, Pollution: %d\n", i, waterLevel, pollutionLevel);
            }
        }
    }
}

void GameLogic::updateGameState() {
    unsigned long currentTime = millis();

    switch (currentState) {
        case GameState::WAITING_DRY:
        case GameState::WAITING_RAIN1:
            cycleWaitingState();
            break;
        case GameState::RAIN1:
            if (currentTime - gameStartTime >= RAIN1_DURATION) {
                currentState = GameState::RAIN2;
                lastStateChangeTime = currentTime;
                Serial.println("Transitioning to RAIN2");
            }
            break;
        case GameState::RAIN2:
            if (currentTime - lastStateChangeTime >= RAIN2_DURATION) {
                currentState = GameState::RAIN3;
                lastStateChangeTime = currentTime;
                Serial.println("Transitioning to RAIN3");
            }
            break;
        case GameState::RAIN3:
            if (checkEndGameCondition()) {
                if (waterLevel >= FLOOD_THRESHOLD) {
                    endGame(GameState::FLOOD);
                } else if (pollutionLevel >= POLLUTION_THRESHOLD) {
                    endGame(GameState::POLLUTION);
                } else {
                    endGame(GameState::WIN);
                }
            }
            break;
        case GameState::WIN:
        case GameState::FLOOD:
        case GameState::POLLUTION:
            // Display end game state for a set duration, then reset
            if (currentTime - lastStateChangeTime >= END_GAME_DISPLAY_DURATION) {
                resetToWaiting();
            }
            break;
    }
}

void GameLogic::updateGameProgress() {
    if (currentState == GameState::RAIN1 || currentState == GameState::RAIN2 || currentState == GameState::RAIN3) {
        waterLevel += 1; // Increase water level over time
        pollutionLevel += 1; // Increase pollution level over time
        
        // Adjust increase rate based on rain intensity
        if (currentState == GameState::RAIN2) {
            waterLevel += 1;
            pollutionLevel += 1;
        } else if (currentState == GameState::RAIN3) {
            waterLevel += 2;
            pollutionLevel += 2;
        }
        
        // Ensure levels don't exceed maximum
        waterLevel = min(waterLevel, 100);
        pollutionLevel = min(pollutionLevel, 100);
        
        if (waterLevel % 10 == 0 || pollutionLevel % 10 == 0) {
            Serial.printf("Water Level: %d, Pollution Level: %d\n", waterLevel, pollutionLevel);
        }
    }
}

void GameLogic::updateDisplay() {
    scene->update(currentState);
    rainSystem->update(currentState);
    mainMatrix.show();
    secMatrix.show();
}

void GameLogic::cycleWaitingState() {
    unsigned long currentTime = millis();
    static const unsigned long WAITING_STATE_DURATION = 5000; // 5 seconds for each waiting state

    if (currentTime - lastStateChangeTime > WAITING_STATE_DURATION) {
        currentState = (currentState == GameState::WAITING_DRY) ? GameState::WAITING_RAIN1 : GameState::WAITING_DRY;
        lastStateChangeTime = currentTime;
        Serial.println(currentState == GameState::WAITING_DRY ? "Waiting Dry" : "Waiting Rain1");
    }
}

bool GameLogic::checkEndGameCondition() {
    return waterLevel >= FLOOD_THRESHOLD || pollutionLevel >= POLLUTION_THRESHOLD || 
           (waterLevel <= WIN_WATER_THRESHOLD && pollutionLevel <= WIN_POLLUTION_THRESHOLD);
}

void GameLogic::startGame() {
    currentState = GameState::RAIN1;
    gameStartTime = millis();
    lastStateChangeTime = gameStartTime;
    waterLevel = 0;
    pollutionLevel = 0;
    Serial.println("Game started, entering RAIN1");
}

void GameLogic::endGame(GameState endState) {
    currentState = endState;
    lastStateChangeTime = millis();
    Serial.print("Game ended. Final state: ");
    switch (endState) {
        case GameState::WIN:
            Serial.println("WIN");
            break;
        case GameState::FLOOD:
            Serial.println("FLOOD");
            break;
        case GameState::POLLUTION:
            Serial.println("POLLUTION");
            break;
        default:
            Serial.println("Unknown");
    }
    Serial.printf("Final Water Level: %d, Pollution Level: %d\n", waterLevel, pollutionLevel);
}

void GameLogic::resetToWaiting() {
    currentState = GameState::WAITING_RAIN1;
    lastStateChangeTime = millis();
    waterLevel = 0;
    pollutionLevel = 0;
    Serial.println("Reset to waiting state (WAITING_RAIN1)");
}
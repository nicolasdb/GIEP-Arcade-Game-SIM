#include "GameLogic.h"
#include "config.h"
#include "game_config.h"
#include "DebugLogger.h"

GameLogic::GameLogic(Scene& scene, SecondaryLEDHandler& secondaryLEDs) 
    : scene(scene), secondaryLEDs(secondaryLEDs), currentState(GameState::WAITING_RAINING), 
    stateStartTime(0), sewerLevel(0), basinLevel(0), basinGateOpen(false), gameActive(false) {
    memset(buttonStates, 0, sizeof(buttonStates));
    initializeGameState();
}

void GameLogic::initializeGameState() {
    currentState = GameState::WAITING_RAINING;
    stateStartTime = millis();
    scene.setRainVisible(true);
    scene.setRainIntensity(RAIN_INTENSITY_RAINING);
    scene.setPollutionState(false);
    updateSecondaryLEDs();
}

void GameLogic::update() {
    if (gameActive) {
        updateActiveGame();
    } else {
        updateWaitingMode();
    }
    updateSecondaryLEDs();
    
    DebugLogger::debug("Update complete - State: %s, Sewer: %.2f, Basin: %.2f", 
                       getStateString(), sewerLevel, basinLevel);
}

void GameLogic::updateActiveGame() {
    DebugLogger::debug("Updating active game. Current state: %s", getStateString());
    updateWeatherCycle();
    updateWaterLevels();
    updateRainIntensity();
    handleGIEPEffects();
    handleBasinGate();
    checkForStateTransition();
    DebugLogger::debug("Active game update complete. Current state: %s", getStateString());
}

void GameLogic::updateWaitingMode() {
    unsigned long currentTime = millis();
    unsigned long stateDuration = currentTime - stateStartTime;

    if (currentState == GameState::WAITING_RAINING) {
        sewerLevel += SEWER_INCREASE_RATE_RAINING - SEWER_DRAIN_RATE;
        if (stateDuration >= WAITING_RAINING_DURATION) {
            transitionState(GameState::WAITING_DRY);
            scene.setRainVisible(false);
        }
    } else if (currentState == GameState::WAITING_DRY) {
        sewerLevel -= SEWER_DRAIN_RATE;
        if (stateDuration >= WAITING_DRY_DURATION) {
            transitionState(GameState::WAITING_RAINING);
            scene.setRainVisible(true);
        }
    }

    sewerLevel = max(0.0f, min(sewerLevel, 1.0f));
    scene.setSewerLevel(sewerLevel);
}

void GameLogic::handleButton(uint8_t buttonIndex, bool isPressed) {
    // Ignore button presses in all end game states
    if (currentState == GameState::WIN || currentState == GameState::FLOOD || currentState == GameState::CANAL) {
        return;
    }

    if (!gameActive && isPressed) {
        startGame();
        return;
    }

    if (buttonIndex < 8) {  // GIEP buttons
        handleGIEPButton(buttonIndex, isPressed);
    } else if (buttonIndex == 8) {  // Basin gate button
        handleBasinGateButton(isPressed);
    }
}

void GameLogic::startGame() {
    gameActive = true;
    sewerLevel = 0;
    basinLevel = 0;
    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);
    scene.setPollutionState(false);
    currentState = GameState::RAINING;
    stateStartTime = millis();
    scene.setRainVisible(true);
    scene.setRainIntensity(RAIN_INTENSITY_RAINING);
    DebugLogger::critical("Game started");
}

void GameLogic::handleGIEPButton(uint8_t buttonIndex, bool isPressed) {
    buttonStates[buttonIndex] = isPressed;
    scene.setGIEPState(buttonIndex, isPressed);
    
    SecondaryLEDZone zone;
    switch (buttonIndex) {
        case 0: zone = SecondaryLEDZone::GIEP_1; break;
        case 1: zone = SecondaryLEDZone::GIEP_2; break;
        case 2: zone = SecondaryLEDZone::GIEP_3; break;
        case 3: zone = SecondaryLEDZone::GIEP_4; break;
        case 4: zone = SecondaryLEDZone::GIEP_5; break;
        case 5: zone = SecondaryLEDZone::GIEP_6; break;
        case 6: zone = SecondaryLEDZone::GIEP_7; break;
        case 7: zone = SecondaryLEDZone::GIEP_8; break;
        default: return; // Invalid button index
    }
    
    secondaryLEDs.setZoneState(zone, isPressed);
}

void GameLogic::handleBasinGateButton(bool isPressed) {
    basinGateOpen = isPressed;
    scene.setBasinGateState(isPressed);
    secondaryLEDs.setZoneState(SecondaryLEDZone::BASIN_GATE, isPressed);
}

const char* GameLogic::getStateString() const {
    return getStateString(currentState);
}

const char* GameLogic::getStateString(GameState state) const {
    switch (state) {
        case GameState::WAITING_RAINING: return "WAITING_RAINING";
        case GameState::WAITING_DRY: return "WAITING_DRY";
        case GameState::RAINING: return "RAINING";
        case GameState::HEAVY: return "HEAVY";
        case GameState::STORM: return "STORM";
        case GameState::FLOOD: return "FLOOD";
        case GameState::CANAL: return "CANAL";
        case GameState::WIN: return "WIN";
        default: return "UNKNOWN";
    }
}

void GameLogic::transitionState(GameState newState) {
    DebugLogger::critical("Game state transition: %s -> %s", getStateString(), getStateString(newState));
    currentState = newState;
    stateStartTime = millis();
    updateSecondaryLEDs();
}

void GameLogic::updateWeatherCycle() {
    unsigned long currentTime = millis();
    unsigned long stateDuration = currentTime - stateStartTime;

    if (currentState == GameState::RAINING && stateDuration >= RAINING_DURATION) {
        transitionState(GameState::HEAVY);
    } else if (currentState == GameState::HEAVY && stateDuration >= HEAVY_DURATION) {
        transitionState(GameState::STORM);
    } else if (currentState == GameState::STORM && stateDuration >= STORM_DURATION) {
        DebugLogger::critical("STORM duration ended. Checking win condition.");
        if (sewerLevel <= WIN_THRESHOLD && basinLevel <= WIN_THRESHOLD) {
            DebugLogger::critical("Win condition met at the end of STORM. Ending game with WIN state.");
            endGame(GameState::WIN);
        } else {
            DebugLogger::critical("Win condition not met. Transitioning back to RAINING state.");
            transitionState(GameState::RAINING);
        }
    }
}

void GameLogic::updateWaterLevels() {
    float sewerIncreaseRate = 0.0f;
    switch (currentState) {
        case GameState::RAINING:
            sewerIncreaseRate = SEWER_INCREASE_RATE_RAINING;
            break;
        case GameState::HEAVY:
            sewerIncreaseRate = SEWER_INCREASE_RATE_HEAVY;
            break;
        case GameState::STORM:
            sewerIncreaseRate = SEWER_INCREASE_RATE_STORM;
            break;
        default:
            break;
    }

    float giepEffect = 0;
    for (int i = 0; i < 8; i++) {
        if (buttonStates[i]) {
            giepEffect += GIEP_EFFECT_STRENGTH;
        }
    }

    if (currentState == GameState::RAINING || currentState == GameState::HEAVY || currentState == GameState::STORM) {
        sewerLevel += sewerIncreaseRate - giepEffect - SEWER_DRAIN_RATE;
    } else {
        sewerLevel -= SEWER_DRAIN_RATE;
    }
    sewerLevel = max(0.0f, min(sewerLevel, 1.0f));

    if (basinGateOpen) {
        float transferAmount = min(BASIN_GATE_TRANSFER_RATE, sewerLevel);
        sewerLevel -= transferAmount;
        basinLevel += transferAmount;
    }
    basinLevel = max(0.0f, min(basinLevel, 1.0f));

    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);

    DebugLogger::critical("Water levels updated - State: %s, Sewer: %.2f, Basin: %.2f, Increase Rate: %.3f, GIEP: %.3f", 
                          getStateString(), sewerLevel, basinLevel, sewerIncreaseRate, giepEffect);
}

void GameLogic::updateRainIntensity() {
    float intensity;
    uint8_t rainLevel = 0;
    switch (currentState) {
        case GameState::WAITING_DRY:
            intensity = 0.0f;
            scene.setRainVisible(false);
            break;
        case GameState::WAITING_RAINING:
        case GameState::RAINING:
            intensity = RAIN_INTENSITY_RAINING;
            scene.setRainVisible(true);
            rainLevel = 1;
            break;
        case GameState::HEAVY:
            intensity = RAIN_INTENSITY_HEAVY;
            scene.setRainVisible(true);
            rainLevel = 2;
            break;
        case GameState::STORM:
            intensity = RAIN_INTENSITY_STORM;
            scene.setRainVisible(true);
            rainLevel = 3;
            break;
        default:
            intensity = 0.0f;
            scene.setRainVisible(false);
            break;
    }
    scene.setRainIntensity(intensity);
    secondaryLEDs.setRainLevel(rainLevel);
}

void GameLogic::handleGIEPEffects() {
    for (int i = 0; i < 8; i++) {
        scene.setGIEPState(i, buttonStates[i]);
        SecondaryLEDZone zone;
        switch (i) {
            case 0: zone = SecondaryLEDZone::GIEP_1; break;
            case 1: zone = SecondaryLEDZone::GIEP_2; break;
            case 2: zone = SecondaryLEDZone::GIEP_3; break;
            case 3: zone = SecondaryLEDZone::GIEP_4; break;
            case 4: zone = SecondaryLEDZone::GIEP_5; break;
            case 5: zone = SecondaryLEDZone::GIEP_6; break;
            case 6: zone = SecondaryLEDZone::GIEP_7; break;
            case 7: zone = SecondaryLEDZone::GIEP_8; break;
            default: continue; // Skip invalid indices
        }
        secondaryLEDs.setZoneState(zone, buttonStates[i]);
    }
}

void GameLogic::handleBasinGate() {
    if (basinGateOpen) {
        float transferAmount = min(BASIN_GATE_TRANSFER_RATE * sewerLevel, BASIN_OVERFLOW_THRESHOLD - basinLevel);
        sewerLevel -= transferAmount;
        basinLevel += transferAmount;
        
        scene.setSewerLevel(sewerLevel);
        scene.setBasinLevel(basinLevel);
    }
}

void GameLogic::checkForStateTransition() {
    DebugLogger::critical("Checking state transition - Current State: %s, Sewer Level: %.2f, Basin Level: %.2f", 
                          getStateString(), sewerLevel, basinLevel);

    if (sewerLevel >= SEWER_OVERFLOW_THRESHOLD) {
        DebugLogger::critical("Sewer overflow detected. Ending game with FLOOD state.");
        endGame(GameState::FLOOD);
    } else if (basinLevel >= BASIN_OVERFLOW_THRESHOLD) {
        DebugLogger::critical("Basin overflow detected. Ending game with CANAL state.");
        endGame(GameState::CANAL);
    } else if (currentState == GameState::STORM) {
        unsigned long stormDuration = millis() - stateStartTime;
        if (sewerLevel <= WIN_THRESHOLD && basinLevel <= WIN_THRESHOLD) {
            DebugLogger::critical("Win condition met during STORM. Ending game with WIN state.");
            endGame(GameState::WIN);
        } else if (stormDuration >= STORM_DURATION) {
            DebugLogger::critical("STORM duration ended. Transitioning to RAINING state.");
            transitionState(GameState::RAINING);
        }
    }
}

void GameLogic::endGame(GameState endState) {
    DebugLogger::critical("Ending game. Previous state: %s, New state: %s", getStateString(), getStateString(endState));
    gameActive = false;
    currentState = endState;  // Set the current state directly
    stateStartTime = millis(); // Start the timer for END_STATE_DURATION
    if (endState == GameState::CANAL) {
        scene.setPollutionState(true);
    }
    if (endState == GameState::WIN) {
        DebugLogger::critical("WIN state set. Updating secondary LEDs for WIN state.");
    }
    updateSecondaryLEDs();  // Update LEDs immediately after changing state
}

// ... (keep all remaining code)

void GameLogic::updateSecondaryLEDs() {
    // Handle endgame states
    if (currentState == GameState::WIN) {
        DebugLogger::debug("Updating secondary LEDs for WIN state");
        secondaryLEDs.setEndGameState(SecondaryLEDZone::WIN);
        // In WIN state, we still want to update GIEP zones for blinking
        updateGIEPAndBasinGateLEDs();
    } else if (currentState == GameState::FLOOD) {
        secondaryLEDs.setEndGameState(SecondaryLEDZone::FLOOD_DEATH);
    } else if (currentState == GameState::CANAL) {
        secondaryLEDs.setEndGameState(SecondaryLEDZone::POLLUTION_DEATH);
    } else {
        secondaryLEDs.setEndGameState(SecondaryLEDZone::NONE);
        
        // Update rain level indicators
        switch (currentState) {
            case GameState::RAINING:
                secondaryLEDs.setRainLevel(1);
                break;
            case GameState::HEAVY:
                secondaryLEDs.setRainLevel(2);
                break;
            case GameState::STORM:
                secondaryLEDs.setRainLevel(3);
                break;
            default:
                secondaryLEDs.setRainLevel(0);
                break;
        }

        // Update GIEP and Basin Gate LEDs
        updateGIEPAndBasinGateLEDs();
    }

    // Check if we need to transition back to waiting state after end game
    checkEndGameTransition();
}

void GameLogic::updateGIEPAndBasinGateLEDs() {
    // Update GIEP LEDs
    for (int i = 0; i < 8; i++) {
        SecondaryLEDZone zone;
        switch (i) {
            case 0: zone = SecondaryLEDZone::GIEP_1; break;
            case 1: zone = SecondaryLEDZone::GIEP_2; break;
            case 2: zone = SecondaryLEDZone::GIEP_3; break;
            case 3: zone = SecondaryLEDZone::GIEP_4; break;
            case 4: zone = SecondaryLEDZone::GIEP_5; break;
            case 5: zone = SecondaryLEDZone::GIEP_6; break;
            case 6: zone = SecondaryLEDZone::GIEP_7; break;
            case 7: zone = SecondaryLEDZone::GIEP_8; break;
            default: continue; // Skip invalid indices
        }
        secondaryLEDs.setZoneState(zone, buttonStates[i]);
    }

    // Update Basin Gate LED
    secondaryLEDs.setZoneState(SecondaryLEDZone::BASIN_GATE, basinGateOpen);
}

void GameLogic::checkEndGameTransition() {
    if (currentState == GameState::WIN || currentState == GameState::FLOOD || currentState == GameState::CANAL) {
        unsigned long endStateDuration = millis() - stateStartTime;
        if (endStateDuration >= END_STATE_DURATION) {
            DebugLogger::critical("End game state duration exceeded. Transitioning to waiting state.");
            initializeGameState(); // Reset to initial waiting state
        }
    }
}

// ... (keep all remaining existing code)

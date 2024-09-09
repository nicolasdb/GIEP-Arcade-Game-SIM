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
}

void GameLogic::updateActiveGame() {
    updateWeatherCycle();
    updateWaterLevels();
    updateRainIntensity();
    handleGIEPEffects();
    handleBasinGate();
    checkForStateTransition();
}

void GameLogic::updateWaitingMode() {
    unsigned long currentTime = millis();
    unsigned long stateDuration = currentTime - stateStartTime;

    if (currentState == GameState::WAITING_RAINING) {
        sewerLevel += RAIN_INTENSITY_RAINING - SEWER_DRAIN_RATE;
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
    if (currentState == GameState::FLOOD || currentState == GameState::CANAL || currentState == GameState::WIN) {
        return; // Ignore button presses in end game states
    }

    if (!gameActive && isPressed) {
        startGame();
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
    
    // Map button index to the correct SecondaryLEDZone
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
    switch (currentState) {
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
    DebugLogger::critical("Game state: %s", getStateString());
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
        transitionState(GameState::RAINING);
    }
}

void GameLogic::updateWaterLevels() {
    float rainIntensity = scene.getRainIntensity();
    float giepEffect = 0;
    for (int i = 0; i < 8; i++) {
        if (buttonStates[i]) {
            giepEffect += GIEP_EFFECT_STRENGTH;
        }
    }

    if (currentState == GameState::RAINING || currentState == GameState::HEAVY || currentState == GameState::STORM) {
        sewerLevel += rainIntensity - giepEffect - SEWER_DRAIN_RATE;
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
    if (sewerLevel >= SEWER_OVERFLOW_THRESHOLD) {
        endGame(GameState::FLOOD);
    } else if (basinLevel >= BASIN_OVERFLOW_THRESHOLD) {
        endGame(GameState::CANAL);
    } else if (currentState == GameState::STORM && sewerLevel <= WIN_THRESHOLD && basinLevel <= WIN_THRESHOLD) {
        endGame(GameState::WIN);
    }
}

void GameLogic::endGame(GameState endState) {
    gameActive = false;
    transitionState(endState);
    stateStartTime = millis(); // Start the timer for END_STATE_DURATION
    if (endState == GameState::CANAL) {
        scene.setPollutionState(true);
    }
    DebugLogger::critical("Game ended: %s", getStateString());
}

void GameLogic::resetGame() {
    gameActive = false;
    sewerLevel = 0;
    basinLevel = 0;
    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);
    scene.setPollutionState(false);
    for (int i = 0; i < 8; i++) {
        buttonStates[i] = false;
        scene.setGIEPState(i, false);
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
        secondaryLEDs.setZoneState(zone, false);
    }
    basinGateOpen = false;
    scene.setBasinGateState(false);
    secondaryLEDs.setZoneState(SecondaryLEDZone::BASIN_GATE, false);
    secondaryLEDs.stopBlinking(); // Stop any ongoing blinking
    initializeGameState();
    DebugLogger::critical("Game reset");
}

void GameLogic::updateSecondaryLEDs() {
    switch (currentState) {
        case GameState::WAITING_RAINING:
        case GameState::WAITING_DRY:
            secondaryLEDs.stopBlinking();
            secondaryLEDs.setRainLevel(0);
            break;
        case GameState::RAINING:
            secondaryLEDs.stopBlinking();
            secondaryLEDs.setRainLevel(1);
            break;
        case GameState::HEAVY:
            secondaryLEDs.stopBlinking();
            secondaryLEDs.setRainLevel(2);
            break;
        case GameState::STORM:
            secondaryLEDs.stopBlinking();
            secondaryLEDs.setRainLevel(3);
            break;
        case GameState::FLOOD:
            secondaryLEDs.blinkAll(scene.getSewerColor());
            secondaryLEDs.setZoneState(SecondaryLEDZone::FLOOD_DEATH, true, CRGB(COLOR_RED));
            secondaryLEDs.setRainLevel(0);
            break;
        case GameState::CANAL:
            secondaryLEDs.blinkAll(CRGB(COLOR_MAGENTA));
            secondaryLEDs.setZoneState(SecondaryLEDZone::POLLUTION_DEATH, true, CRGB(COLOR_RED));
            secondaryLEDs.setRainLevel(0);
            break;
        case GameState::WIN:
            secondaryLEDs.blinkAll(CRGB(COLOR_GREEN_1));
            secondaryLEDs.setZoneState(SecondaryLEDZone::WIN, true, CRGB(COLOR_CYAN));
            secondaryLEDs.setRainLevel(0);
            break;
        default:
            secondaryLEDs.stopBlinking();
            secondaryLEDs.setRainLevel(0);
            // Reset all zones to their default state
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
                    default: continue;
                }
                secondaryLEDs.setZoneState(zone, buttonStates[i]);
            }
            secondaryLEDs.setZoneState(SecondaryLEDZone::BASIN_GATE, basinGateOpen);
            secondaryLEDs.setZoneState(SecondaryLEDZone::FLOOD_DEATH, false);
            secondaryLEDs.setZoneState(SecondaryLEDZone::POLLUTION_DEATH, false);
            secondaryLEDs.setZoneState(SecondaryLEDZone::WIN, false);
            break;
    }

    // Check if we need to transition back to waiting state after end game
    unsigned long currentTime = millis();
    if (currentState == GameState::FLOOD || currentState == GameState::CANAL || currentState == GameState::WIN) {
        unsigned long stateDuration = currentTime - stateStartTime;
        if (stateDuration >= END_STATE_DURATION) {
            resetGame();
        }
    }
}


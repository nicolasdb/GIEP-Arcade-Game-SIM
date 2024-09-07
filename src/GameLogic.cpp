#include "GameLogic.h"
#include "config.h"
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
    updateSecondaryLEDs();
    DebugLogger::info("Game initialized with WAITING_RAINING state");
}

void GameLogic::update() {
    DebugLogger::debug("GameLogic::update() called");
    if (gameActive) {
        updateActiveGame();
    } else {
        updateWaitingMode();
    }
    updateSecondaryLEDs();
    DebugLogger::debug("GameLogic::update() completed");
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

    if (currentState == GameState::WAITING_RAINING && stateDuration >= WAITING_RAINING_DURATION) {
        transitionState(GameState::WAITING_DRY);
        scene.setRainVisible(false);
    } else if (currentState == GameState::WAITING_DRY && stateDuration >= WAITING_DRY_DURATION) {
        transitionState(GameState::WAITING_RAINING);
        scene.setRainVisible(true);
    }
}

void GameLogic::handleButton(uint8_t buttonIndex, bool isPressed) {
    DebugLogger::debug("Button %d %s", buttonIndex, isPressed ? "pressed" : "released");
    if (!gameActive && isPressed) {
        startGame();
        return;
    }
    if (buttonIndex < 8) {  // GIEP buttons
        handleGIEPButton(buttonIndex, isPressed);
    }
}

void GameLogic::startGame() {
    gameActive = true;
    currentState = GameState::RAINING;
    stateStartTime = millis();
    scene.setRainVisible(true);
    scene.setRainIntensity(RAIN_INTENSITY_RAINING);
    DebugLogger::info("Game started, entering RAINING state");
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
    DebugLogger::info("GIEP Button %d %s", buttonIndex + 1, isPressed ? "pressed" : "released");
}

void GameLogic::handleBasinGateButton(bool isPressed) {
    DebugLogger::info("Basin Gate Button %s", isPressed ? "pressed" : "released");
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
    DebugLogger::info("Game state transition: %s -> %s", getStateString(), getStateString());
    currentState = newState;
    stateStartTime = millis();
    updateSecondaryLEDs();
}

void GameLogic::updateWeatherCycle() {
    DebugLogger::debug("Updating weather cycle");
    unsigned long currentTime = millis();
    unsigned long stateDuration = currentTime - stateStartTime;

    if (currentState == GameState::RAINING && stateDuration >= RAINING_DURATION) {
        transitionState(GameState::HEAVY);
        DebugLogger::info("Weather changed to HEAVY");
    } else if (currentState == GameState::HEAVY && stateDuration >= HEAVY_DURATION) {
        transitionState(GameState::STORM);
        DebugLogger::info("Weather changed to STORM");
    } else if (currentState == GameState::STORM && stateDuration >= STORM_DURATION) {
        transitionState(GameState::RAINING);
        DebugLogger::info("Weather cycled back to RAINING");
    }
}

void GameLogic::updateWaterLevels() {
    float rainIntensity = scene.getRainIntensity();
    float previousSewerLevel = sewerLevel;
    float previousBasinLevel = basinLevel;
    
    // Calculate GIEP effect
    float giepEffect = 0;
    for (int i = 0; i < 8; i++) {
        if (buttonStates[i]) {
            giepEffect += GIEP_EFFECT_STRENGTH;
        }
    }

    // Update sewer level
    if (currentState == GameState::RAINING || currentState == GameState::HEAVY || currentState == GameState::STORM) {
        sewerLevel += rainIntensity - giepEffect - SEWER_DRAIN_RATE;
    } else {
        sewerLevel -= SEWER_DRAIN_RATE;
    }
    sewerLevel = max(0.0f, min(sewerLevel, SEWER_OVERFLOW_THRESHOLD));

    // Update basin level (assuming basin gate logic is implemented elsewhere)
    // ... (implement basin level update logic here)

    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);

    DebugLogger::info("Water Levels - Sewer: %.2f (Δ%.4f), Basin: %.2f (Δ%.4f), Rain: %.4f, GIEP: %.4f", 
                      sewerLevel, sewerLevel - previousSewerLevel, 
                      basinLevel, basinLevel - previousBasinLevel,
                      rainIntensity, giepEffect);
}

void GameLogic::updateRainIntensity() {
    DebugLogger::debug("Updating rain intensity");
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
    DebugLogger::info("Rain intensity set to %.2f, Rain level: %d", intensity, rainLevel);
}

void GameLogic::handleGIEPEffects() {
    DebugLogger::debug("Handling GIEP effects");
    // Implement GIEP effects based on button states
    // For now, we're just updating the GIEP states in the Scene and SecondaryLEDs
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
    DebugLogger::debug("Handling basin gate");
    if (basinGateOpen) {
        float transferAmount = min(BASIN_GATE_TRANSFER_RATE * sewerLevel, BASIN_OVERFLOW_THRESHOLD - basinLevel);
        sewerLevel -= transferAmount;
        basinLevel += transferAmount;
        
        scene.setSewerLevel(sewerLevel);
        scene.setBasinLevel(basinLevel);
        
        DebugLogger::info("Gate opened. Transferred %.2f. New levels: Sewer %.2f, Basin %.2f", 
                          transferAmount, sewerLevel, basinLevel);
    }
}

void GameLogic::checkForStateTransition() {
    DebugLogger::debug("Checking for state transition");
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
    DebugLogger::info("Game ended with state: %s", getStateString());
}

void GameLogic::resetGame() {
    DebugLogger::info("Resetting game to WAITING_RAINING state");
    gameActive = false;
    sewerLevel = 0;
    basinLevel = 0;
    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);
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
    initializeGameState();
}

void GameLogic::updateSecondaryLEDs() {
    DebugLogger::debug("Updating secondary LEDs");
    switch (currentState) {
        case GameState::FLOOD:
            secondaryLEDs.setZoneState(SecondaryLEDZone::FLOOD_DEATH, true);
            break;
        case GameState::CANAL:
            secondaryLEDs.setZoneState(SecondaryLEDZone::POLLUTION_DEATH, true);
            break;
        case GameState::WIN:
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
                secondaryLEDs.setZoneState(zone, true);
            }
            break;
        default:
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
            break;
    }
}

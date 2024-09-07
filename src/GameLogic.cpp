#include "GameLogic.h"
#include "config.h"
#include "DebugLogger.h"

GameLogic::GameLogic(Scene& scene, SecondaryLEDHandler& secondaryLEDs) 
    : scene(scene), secondaryLEDs(secondaryLEDs), currentState(GameState::RAINING), 
    stateStartTime(0), sewerLevel(0), basinLevel(0), basinGateOpen(false) {
    memset(buttonStates, 0, sizeof(buttonStates));
    initializeGameState();
}

void GameLogic::initializeGameState() {
    currentState = GameState::RAINING;
    stateStartTime = millis();
    scene.setRainVisible(true);
    scene.setRainIntensity(RAIN_INTENSITY_RAINING);
    updateSecondaryLEDs();
    DebugLogger::info("Game initialized with RAINING state");
}

void GameLogic::update() {
    DebugLogger::debug("GameLogic::update() called");
    updateWeatherCycle();
    updateWaterLevels();
    updateRainIntensity();
    handleGIEPEffects();
    handleBasinGate();
    checkForStateTransition();
    updateSecondaryLEDs();
    DebugLogger::debug("GameLogic::update() completed");
}

void GameLogic::handleButton(uint8_t buttonIndex, bool isPressed) {
    DebugLogger::debug("Button %d %s", buttonIndex, isPressed ? "pressed" : "released");
    if (buttonIndex < 8) {  // GIEP buttons
        handleGIEPButton(buttonIndex, isPressed);
    }
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
        case GameState::IDLE: return "IDLE";
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
        transitionState(GameState::IDLE);
        scene.setRainVisible(false);
        DebugLogger::info("Weather changed to IDLE");
    } else if (currentState == GameState::IDLE && stateDuration >= IDLE_DURATION) {
        transitionState(GameState::HEAVY);
        scene.setRainVisible(true);
        DebugLogger::info("Weather changed to HEAVY");
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
        case GameState::IDLE:
            intensity = RAIN_INTENSITY_IDLE;
            scene.setRainVisible(false);
            break;
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
    unsigned long currentTime = millis();
    unsigned long stateDuration = currentTime - stateStartTime;

    switch (currentState) {
        case GameState::RAINING:
            if (stateDuration >= RAINING_DURATION) {
                transitionState(GameState::IDLE);
            }
            break;
        case GameState::IDLE:
            if (stateDuration >= IDLE_DURATION) {
                transitionState(GameState::HEAVY);
            }
            break;
        case GameState::HEAVY:
            if (stateDuration >= HEAVY_DURATION) {
                transitionState(GameState::STORM);
            }
            break;
        case GameState::STORM:
            if (stateDuration >= STORM_DURATION) {
                transitionState(GameState::WIN);
            }
            break;
        case GameState::FLOOD:
        case GameState::CANAL:
        case GameState::WIN:
            if (stateDuration >= END_STATE_DURATION) {
                resetGame();
            }
            break;
    }

    if (sewerLevel >= SEWER_OVERFLOW_THRESHOLD && currentState != GameState::FLOOD && currentState != GameState::CANAL && currentState != GameState::WIN) {
        transitionState(GameState::FLOOD);
    } else if (basinLevel >= BASIN_OVERFLOW_THRESHOLD && currentState != GameState::FLOOD && currentState != GameState::CANAL && currentState != GameState::WIN) {
        transitionState(GameState::CANAL);
    }
}

void GameLogic::resetGame() {
    DebugLogger::info("Resetting game to RAINING state");
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
            secondaryLEDs.setEndGameState(SecondaryLEDZone::FLOOD_DEATH);
            break;
        case GameState::CANAL:
            secondaryLEDs.setEndGameState(SecondaryLEDZone::POLLUTION_DEATH);
            break;
        case GameState::WIN:
            secondaryLEDs.setEndGameState(SecondaryLEDZone::WIN);
            break;
        default:
            secondaryLEDs.setEndGameState(SecondaryLEDZone::GIEP_1); // Reset end game state
            break;
    }
}

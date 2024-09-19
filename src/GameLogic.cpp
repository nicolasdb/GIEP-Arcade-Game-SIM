#include "GameLogic.h"
#include "config.h"
#include "game_config.h"
#include "DebugLogger.h"

using namespace GameConfig;

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
    scene.setRainIntensity(RainVisuals::RAIN_INTENSITY_RAINING);
    scene.setPollutionState(false);
    updateSecondaryLEDs();
}

void GameLogic::update() {
    if (gameActive) {
        updateActiveGame();
    } else if (currentState == GameState::WIN || currentState == GameState::FLOOD || currentState == GameState::BASIN_OVERFLOW) {
        updateEndGameState();
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
        sewerLevel += SewerMechanics::SEWER_INCREASE_RATE_RAINING - SewerMechanics::SEWER_DRAIN_RATE;
        if (stateDuration >= Timing::WAITING_RAINING_DURATION) {
            transitionState(GameState::WAITING_DRY);
            scene.setRainVisible(false);
        }
    } else if (currentState == GameState::WAITING_DRY) {
        sewerLevel -= SewerMechanics::SEWER_DRAIN_RATE;
        if (stateDuration >= Timing::WAITING_DRY_DURATION) {
            transitionState(GameState::WAITING_RAINING);
            scene.setRainVisible(true);
        }
    }

    sewerLevel = max(0.0f, min(sewerLevel, 1.0f));
    scene.setSewerLevel(sewerLevel);
}

void GameLogic::handleButton(uint8_t buttonIndex, bool isPressed) {
    DebugLogger::debug("Button %d %s", buttonIndex, isPressed ? "pressed" : "released");

    // Ignore button presses in all end game states
    if (currentState == GameState::WIN || currentState == GameState::FLOOD || currentState == GameState::BASIN_OVERFLOW) {
        return;
    }

    // Handle button presses during waiting states
    if (currentState == GameState::WAITING_RAINING || currentState == GameState::WAITING_DRY) {
        if (isPressed) {
            if (buttonIndex < 8) {  // GIEP buttons
                handleGIEPButton(buttonIndex, isPressed);
                scene.setGIEPState(buttonIndex, isPressed);
            } else if (buttonIndex == 8) {  // Basin gate button
                handleBasinGateButton(isPressed);
                scene.setBasinGateState(isPressed);
            }
            updateGIEPAndBasinGateLEDs();
            startGame();
        }
        return;
    }

    // Handle button presses during active game
    if (gameActive) {
        if (buttonIndex < 8) {  // GIEP buttons
            handleGIEPButton(buttonIndex, isPressed);
        } else if (buttonIndex == 8) {  // Basin gate button
            handleBasinGateButton(isPressed);
        }
    }
}

void GameLogic::startGame() {
    DebugLogger::critical("Starting the game");
    gameActive = true;
    sewerLevel = 0;
    basinLevel = 0;
    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);
    scene.setPollutionState(false);
    currentState = GameState::RAINING;
    stateStartTime = millis();
    scene.setRainVisible(true);
    scene.setRainIntensity(RainVisuals::RAIN_INTENSITY_RAINING);
    updateSecondaryLEDs();
    DebugLogger::critical("Game started. Initial state: %s", getStateString());
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
        case GameState::BASIN_OVERFLOW: return "BASIN_OVERFLOW";
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

    if (currentState == GameState::RAINING && stateDuration >= Timing::RAINING_DURATION) {
        transitionState(GameState::HEAVY);
    } else if (currentState == GameState::HEAVY && stateDuration >= Timing::HEAVY_DURATION) {
        transitionState(GameState::STORM);
    } else if (currentState == GameState::STORM && stateDuration >= Timing::STORM_DURATION) {
        DebugLogger::critical("STORM duration ended. Checking win condition.");
        if (sewerLevel <= GameBalance::WIN_THRESHOLD && basinLevel <= GameBalance::WIN_THRESHOLD) {
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
            sewerIncreaseRate = SewerMechanics::SEWER_INCREASE_RATE_RAINING;
            break;
        case GameState::HEAVY:
            sewerIncreaseRate = SewerMechanics::SEWER_INCREASE_RATE_HEAVY;
            break;
        case GameState::STORM:
            sewerIncreaseRate = SewerMechanics::SEWER_INCREASE_RATE_STORM;
            break;
        default:
            break;
    }

    float giepEffect = 0;
    for (int i = 0; i < 8; i++) {
        if (buttonStates[i]) {
            giepEffect += SewerMechanics::GIEP_EFFECT_STRENGTH;
        }
    }

    if (currentState == GameState::RAINING || currentState == GameState::HEAVY || currentState == GameState::STORM) {
        sewerLevel += sewerIncreaseRate - giepEffect - SewerMechanics::SEWER_DRAIN_RATE;
    } else {
        sewerLevel -= SewerMechanics::SEWER_DRAIN_RATE;
    }
    sewerLevel = max(0.0f, min(sewerLevel, 1.0f));

    if (basinGateOpen) {
        float transferAmount = min(SewerMechanics::BASIN_GATE_TRANSFER_RATE, sewerLevel);
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
    RainLevel rainLevel = RainLevel::NONE;
    switch (currentState) {
        case GameState::WAITING_DRY:
            intensity = 0.0f;
            scene.setRainVisible(false);
            break;
        case GameState::WAITING_RAINING:
        case GameState::RAINING:
            intensity = RainVisuals::RAIN_INTENSITY_RAINING;
            scene.setRainVisible(true);
            rainLevel = RainLevel::LIGHT;
            break;
        case GameState::HEAVY:
            intensity = RainVisuals::RAIN_INTENSITY_HEAVY;
            scene.setRainVisible(true);
            rainLevel = RainLevel::MODERATE;
            break;
        case GameState::STORM:
            intensity = RainVisuals::RAIN_INTENSITY_STORM;
            scene.setRainVisible(true);
            rainLevel = RainLevel::HEAVY;
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
        SecondaryLEDZone zone = static_cast<SecondaryLEDZone>(static_cast<int>(SecondaryLEDZone::GIEP_1) + i);
        secondaryLEDs.setZoneState(zone, buttonStates[i]);
    }
}

void GameLogic::handleBasinGate() {
    if (basinGateOpen) {
        float transferAmount = min(SewerMechanics::BASIN_GATE_TRANSFER_RATE * sewerLevel, GameBalance::BASIN_OVERFLOW_THRESHOLD - basinLevel);
        sewerLevel -= transferAmount;
        basinLevel += transferAmount;
        
        scene.setSewerLevel(sewerLevel);
        scene.setBasinLevel(basinLevel);
        
        DebugLogger::debug("Basin gate transfer - Amount: %.2f, New Sewer Level: %.2f, New Basin Level: %.2f",
                           transferAmount, sewerLevel, basinLevel);
    }
}

void GameLogic::checkForStateTransition() {
    DebugLogger::critical("Checking state transition - Current State: %s, Sewer Level: %.2f, Basin Level: %.2f", 
                          getStateString(), sewerLevel, basinLevel);

    if (sewerLevel >= GameBalance::SEWER_OVERFLOW_THRESHOLD) {
        DebugLogger::critical("Sewer overflow detected. Ending game with FLOOD state.");
        endGame(GameState::FLOOD);
    } else if (basinLevel >= GameBalance::BASIN_OVERFLOW_THRESHOLD) {
        DebugLogger::critical("Basin overflow detected. Ending game with BASIN_OVERFLOW state.");
        endGame(GameState::BASIN_OVERFLOW);
    } else if (currentState == GameState::STORM) {
        unsigned long stormDuration = millis() - stateStartTime;
        if (stormDuration >= Timing::STORM_DURATION) {
            if (sewerLevel <= GameBalance::WIN_THRESHOLD && basinLevel <= GameBalance::WIN_THRESHOLD) {
                DebugLogger::critical("Win condition met at the end of STORM. Ending game with WIN state.");
                endGame(GameState::WIN);
            } else {
                DebugLogger::critical("STORM duration ended. Transitioning to RAINING state.");
                transitionState(GameState::RAINING);
            }
        } else {
            DebugLogger::debug("STORM in progress. Duration: %lu / %lu", stormDuration, Timing::STORM_DURATION);
        }
    }
}

void GameLogic::endGame(GameState endState) {
    DebugLogger::critical("Ending game. Previous state: %s, New state: %s", getStateString(), getStateString(endState));
    gameActive = false;
    currentState = endState;
    stateStartTime = millis();
    
    if (endState == GameState::BASIN_OVERFLOW) {
        scene.setPollutionState(true);
    } else if (endState == GameState::FLOOD) {
        sewerLevel = 1.0f;
        scene.setSewerLevel(sewerLevel);
        scene.setFloodState(true);
        DebugLogger::critical("FLOOD state set. Sewer level set to maximum: %.2f", sewerLevel);
    } else if (endState == GameState::WIN) {
        DebugLogger::critical("WIN state set. Updating secondary LEDs for WIN state.");
    }
    
    updateSecondaryLEDs();
}

void GameLogic::updateEndGameState() {
    unsigned long currentTime = millis();
    unsigned long stateDuration = currentTime - stateStartTime;

    if (currentState == GameState::BASIN_OVERFLOW) {
        // Blink the pollution state
        bool shouldBlink = (stateDuration / 500) % 2 == 0;  // Blink every 500ms
        scene.setPollutionState(shouldBlink);
    } else if (currentState == GameState::FLOOD) {
        // Blink the flood state
        bool shouldBlink = (stateDuration / 500) % 2 == 0;  // Blink every 500ms
        scene.setFloodState(shouldBlink);
    }

    // Check if we need to transition back to waiting state
    if (stateDuration >= Timing::END_STATE_DURATION) {
        DebugLogger::critical("End game state duration exceeded. Transitioning to waiting state.");
        initializeGameState();
        resetGameElements();
    }
}

void GameLogic::updateSecondaryLEDs() {
    if (currentState == GameState::WIN) {
        DebugLogger::debug("Updating secondary LEDs for WIN state");
        secondaryLEDs.setEndGameState(SecondaryLEDZone::WIN);
        handleGIEPEffects();  // Keep GIEP zones blinking in WIN state
    } else if (currentState == GameState::FLOOD) {
        secondaryLEDs.setEndGameState(SecondaryLEDZone::FLOOD_DEATH);
        secondaryLEDs.setFloodZoneColor(255, 0, 0);  // Set flood zone color to red
    } else if (currentState == GameState::BASIN_OVERFLOW) {
        secondaryLEDs.setEndGameState(SecondaryLEDZone::POLLUTION_DEATH);
    } else {
        secondaryLEDs.setEndGameState(SecondaryLEDZone::NONE);
        
        // Update rain level indicators
        switch (currentState) {
            case GameState::RAINING:
                secondaryLEDs.setRainLevel(RainLevel::LIGHT);
                break;
            case GameState::HEAVY:
                secondaryLEDs.setRainLevel(RainLevel::MODERATE);
                break;
            case GameState::STORM:
                secondaryLEDs.setRainLevel(RainLevel::HEAVY);
                break;
            default:
                secondaryLEDs.setRainLevel(RainLevel::NONE);
                break;
        }

        // Update GIEP and Basin Gate LEDs
        handleGIEPEffects();
        secondaryLEDs.setZoneState(SecondaryLEDZone::BASIN_GATE, basinGateOpen);
    }
}

void GameLogic::resetGameElements() {
    sewerLevel = 0;
    basinLevel = 0;
    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);
    scene.setPollutionState(false);
    scene.setFloodState(false);
    scene.setRainVisible(true);
    scene.setRainIntensity(RainVisuals::RAIN_INTENSITY_RAINING);
    basinGateOpen = false;
    scene.setBasinGateState(false);
    
    // Reset all GIEP buttons
    for (int i = 0; i < 8; i++) {
        buttonStates[i] = false;
        scene.setGIEPState(i, false);
    }
    
    // Update secondary LEDs
    secondaryLEDs.setRainLevel(RainLevel::LIGHT);
    secondaryLEDs.setZoneState(SecondaryLEDZone::BASIN_GATE, false);
    for (int i = 0; i < 8; i++) {
        SecondaryLEDZone zone = static_cast<SecondaryLEDZone>(static_cast<int>(SecondaryLEDZone::GIEP_1) + i);
        secondaryLEDs.setZoneState(zone, false);
    }
    
    DebugLogger::critical("Game elements reset completed");
}

void GameLogic::updateGIEPAndBasinGateLEDs() {
    handleGIEPEffects();
    secondaryLEDs.setZoneState(SecondaryLEDZone::BASIN_GATE, basinGateOpen);
}

void GameLogic::checkEndGameTransition() {
    if (currentState == GameState::WIN || currentState == GameState::FLOOD || currentState == GameState::BASIN_OVERFLOW) {
        unsigned long endStateDuration = millis() - stateStartTime;
        if (endStateDuration >= Timing::END_STATE_DURATION) {
            DebugLogger::critical("End game state duration exceeded. Transitioning to waiting state.");
            initializeGameState();
            resetGameElements();
        }
    }
}

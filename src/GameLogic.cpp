#include "GameLogic.h"

GameLogic::GameLogic(Scene& scene) : scene(scene), currentState(GameState::IDLE), 
    stateStartTime(0), sewerLevel(0), basinLevel(0) {
    memset(buttonStates, 0, sizeof(buttonStates));
}

void GameLogic::update() {
    updateWaterLevels();
    updateRainIntensity();
    handleGIEPEffects();
    checkForStateTransition();
}

void GameLogic::handleButton(uint8_t buttonIndex, bool isPressed) {
    if (buttonIndex < 9) {
        if (buttonStates[buttonIndex] != isPressed) {
            buttonStates[buttonIndex] = isPressed;
            DebugLogger::info("Button %d %s", buttonIndex + 1, isPressed ? "pressed" : "released");
            
            if (buttonIndex == 8) {  // 9th button (index 8) controls basin gate
                handleBasinGate();
            } else {
                // Handle GIEP buttons
                scene.setGIEPState(buttonIndex, isPressed);
            }
        }
    }
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
}

void GameLogic::checkForStateTransition() {
    unsigned long currentTime = millis();
    unsigned long stateDuration = currentTime - stateStartTime;

    switch (currentState) {
        case GameState::IDLE:
            if (stateDuration >= 30000) {  // 30 seconds
                transitionState(GameState::RAINING);
            }
            break;
        case GameState::RAINING:
            if (stateDuration >= 30000) {
                transitionState(GameState::HEAVY);
            }
            break;
        case GameState::HEAVY:
            if (stateDuration >= 40000) {
                transitionState(GameState::STORM);
            }
            break;
        case GameState::STORM:
            if (stateDuration >= 40000) {
                transitionState(GameState::WIN);
            }
            break;
        case GameState::FLOOD:
        case GameState::CANAL:
        case GameState::WIN:
            if (stateDuration >= 10000) {  // 10 seconds display for end states
                resetGame();
            }
            break;
    }

    // Check for flood and canal overflow conditions
    if (sewerLevel >= 1.0 && currentState != GameState::FLOOD && currentState != GameState::CANAL && currentState != GameState::WIN) {
        transitionState(GameState::FLOOD);
    } else if (basinLevel >= 1.0 && currentState != GameState::FLOOD && currentState != GameState::CANAL && currentState != GameState::WIN) {
        transitionState(GameState::CANAL);
    }
}

void GameLogic::resetGame() {
    DebugLogger::info("Resetting game to IDLE state");
    sewerLevel = 0;
    basinLevel = 0;
    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);
    for (int i = 0; i < 8; i++) {
        scene.setGIEPState(i, false);
    }
    transitionState(GameState::IDLE);
}

void GameLogic::updateRainIntensity() {
    // TODO: Update rain intensity based on current state
}

void GameLogic::handleGIEPEffects() {
    // Implement GIEP effects based on button states
    // For now, we're just updating the GIEP states in the Scene
    for (int i = 0; i < 8; i++) {
        scene.setGIEPState(i, buttonStates[i]);
    }
}

void GameLogic::handleBasinGate() {
    // Implement basin gate logic
    // For example, transfer water from sewer to basin when the 9th button is pressed
    if (buttonStates[8]) {
        float transferAmount = 0.1 * sewerLevel;  // Transfer 10% of sewer water to basin
        sewerLevel -= transferAmount;
        basinLevel += transferAmount;
        scene.setSewerLevel(sewerLevel);
        scene.setBasinLevel(basinLevel);
    }
}

void GameLogic::updateWaterLevels() {
    // Implement water level updates based on current state and GIEP effects
    // This is a simplified example; you'll need to adjust based on your specific game mechanics
    float rainIntensity = 0;
    switch (currentState) {
        case GameState::RAINING: rainIntensity = 0.001; break;
        case GameState::HEAVY: rainIntensity = 0.002; break;
        case GameState::STORM: rainIntensity = 0.003; break;
        default: break;
    }

    // Calculate GIEP effect (simplified)
    float giepEffect = 0;
    for (int i = 0; i < 8; i++) {
        if (buttonStates[i]) {
            giepEffect += 0.0005;  // Each active GIEP reduces water increase
        }
    }

    // Update sewer level
    sewerLevel += rainIntensity - giepEffect;
    sewerLevel = max(0.0f, min(sewerLevel, 1.0f));  // Clamp between 0 and 1

    scene.setSewerLevel(sewerLevel);
    scene.setBasinLevel(basinLevel);
}


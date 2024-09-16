#pragma once
#include <Arduino.h>

// Game-specific configurations
#define GAME_NAME "GIEP Arcade Debug"
#define GAME_VERSION "1.0.0"

// Game state enum
enum class GameState {
    IDLE,
    RUNNING,
    PAUSED
};

// Debug settings
#define DEBUG_BUTTON_BLINK_DURATION 500 // ms

// Add more game-specific configuration variables as needed
// (This line is a comment and won't cause an error)
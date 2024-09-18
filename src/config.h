#ifndef CONFIG_H
#define CONFIG_H

// Pin configurations
#define MAIN_MATRIX_PIN 9
#define SEC_MATRIX_PIN 7
#define M5STAMP_BUTTON_PIN 1
#define MCP23017_SDA_PIN 13
#define MCP23017_SCL_PIN 15

// LED Matrix configurations
#define MATRIX_BRIGHTNESS 50 // Adjust as needed
#define MAIN_MATRIX_WIDTH 25 // Adjust based on your main matrix size
#define MAIN_MATRIX_HEIGHT 25 // Adjust based on your main matrix size
#define SEC_MATRIX_WIDTH 14 // Adjust based on your secondary matrix size
#define SEC_MATRIX_HEIGHT 4 // Adjust based on your secondary matrix size

// Game variables
#define MAX_RAINDROPS 20
#define WAITING_CYCLE_DURATION 5000 // 5 seconds

// Rain levels
#define RAIN1_SPAWN_CHANCE 10
#define RAIN2_SPAWN_CHANCE 15
#define RAIN3_SPAWN_CHANCE 20

#define RAIN1_FALL_SPEED 1
#define RAIN2_FALL_SPEED 2
#define RAIN3_FALL_SPEED 3

// Game progression durations (in milliseconds)
#define RAIN1_DURATION 60000 // 1 minute in RAIN1
#define RAIN2_DURATION 60000 // 1 minute in RAIN2

// End game conditions
#define FLOOD_THRESHOLD 80 // Water level threshold for flood
#define POLLUTION_THRESHOLD 80 // Pollution level threshold for pollution loss
#define WIN_WATER_THRESHOLD 50 // Maximum water level for win condition
#define WIN_POLLUTION_THRESHOLD 50 // Maximum pollution level for win condition

// End game display duration
#define END_GAME_DISPLAY_DURATION 10000 // 10 seconds to display end game state

enum class GameState {
    WAITING_DRY,
    WAITING_RAIN1,
    RAIN1,
    RAIN2,
    RAIN3,
    WIN,
    FLOOD,
    POLLUTION
};

#endif // CONFIG_H
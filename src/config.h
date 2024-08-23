#pragma once
// Hardware configuration
#define LED_PIN 9
#define MATRIX_WIDTH 25
#define MATRIX_HEIGHT 25
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define LED_TYPE WS2813
#define COLOR_ORDER GRB

// MCP23017 configuration
#define MCP23017_ADDRESS 0x20
#define NUM_MCP_BUTTONS 8
#define TOTAL_BUTTONS 9
#define BASIN_GATE_BUTTON_PIN 1
#define BASIN_GATE_LED_PIN 3
#define DEBUG_BUTTON_PIN 0

// Game timing parameters (in milliseconds)
#define IDLE_DURATION 30000
#define RAINING_DURATION 30000
#define HEAVY_DURATION 40000
#define STORM_DURATION 40000
#define END_STATE_DURATION 10000

// Rain intensity parameters
#define RAIN_INTENSITY_IDLE 0.0f
#define RAIN_INTENSITY_RAINING 0.001f
#define RAIN_INTENSITY_HEAVY 0.002f
#define RAIN_INTENSITY_STORM 0.003f

// GIEP effect parameters
#define GIEP_EFFECT_STRENGTH 0.05f

// Water level parameters
#define SEWER_DRAIN_RATE 0.0002f
#define BASIN_GATE_TRANSFER_RATE 0.1f

// Sewer and Basin dimensions and positions
#define SEWER_HEIGHT 7
#define SEWER_WIDTH 5
#define SEWER_START_X 3
#define SEWER_START_Y 16

#define BASIN_HEIGHT 4
#define BASIN_WIDTH 6
#define BASIN_START_X 10
#define BASIN_START_Y 20

// Color definitions
#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000

// Updated GIEP Button colors, MCP GPIO connections for buttons (GPIOA) and LEDs (GPIOB)
#define COLOR_GREEN_1 0x00AB00  // Button 1 - GPIOA 0, LED - GPIOB 0
#define COLOR_GREEN_2 0x003200  // Button 2 - GPIOA 1, LED - GPIOB 1
#define COLOR_GREEN_3 0x005700  // Button 3 - GPIOA 2, LED - GPIOB 2
#define COLOR_GREEN_4 0x007300  // Button 4 - GPIOA 3, LED - GPIOB 3
#define COLOR_GREEN_5 0x008F00  // Button 5 - GPIOA 4, LED - GPIOB 4
#define COLOR_GREEN_6 0x00C700  // Button 6 - GPIOA 5, LED - GPIOB 5
#define COLOR_GREEN_7 0x00E300  // Button 7 - GPIOA 6, LED - GPIOB 6
#define COLOR_GREEN_8 0x00FF00  // Button 8 - GPIOA 7, LED - GPIOB 7

#define COLOR_YELLOW 0xFFFF00
#define COLOR_BLUE 0x0000FF

// New color definitions
#define COLOR_RED 0xFF0000  // for bassin_gate
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_PURPLE 0x6400FF  // for the river

// LED colors
#define SEWER_COLOR CRGB::Yellow
#define BASIN_COLOR CRGB::Blue
#define SEWER_EMPTY_COLOR CRGB(16, 16, 0)  // Dim yellow
#define BASIN_EMPTY_COLOR CRGB(0, 0, 16)   // Dim blue
#define BASIN_GATE_COLOR CRGB::Red
#define BASIN_OVERFLOW_COLOR CRGB::Green
#define RIVER_COLOR CRGB(100, 0, 255)  // Purple color for the river

// Main brighness
#define BRIGHTNESS 30
// Specific brightness levels
#define ACTIVE_BRIGHTNESS 16
#define GIEP_ACTIVE_BRIGHTNESS 255
#define GIEP_INACTIVE_BRIGHTNESS 40
#define RAIN_BRIGHTNESS 128
#define SEWER_BRIGHTNESS 255
#define SEWER_EMPTY_BRIGHTNESS 16
#define BASIN_BRIGHTNESS 255
#define BASIN_EMPTY_BRIGHTNESS 16
#define BASIN_GATE_BRIGHTNESS 255
#define BASIN_GATE_INACTIVE_BRIGHTNESS 40
#define BASIN_OVERFLOW_BRIGHTNESS 255
#define RIVER_BRIGHTNESS 128

// Game balance parameters
#define SEWER_OVERFLOW_THRESHOLD 1.0f
#define BASIN_OVERFLOW_THRESHOLD 1.0f

// Task configuration
#define BUTTON_TASK_STACK_SIZE 2048
#define GAME_UPDATE_TASK_STACK_SIZE 4096
#define LED_UPDATE_TASK_STACK_SIZE 2048
#define TASK_PRIORITY 1

// Watchdog configuration
#define WDT_TIMEOUT 5  // 5 seconds

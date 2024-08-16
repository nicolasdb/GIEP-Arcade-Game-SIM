#pragma once
// Hardware configuration
#define LED_PIN 9
#define MATRIX_WIDTH 25
#define MATRIX_HEIGHT 25
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define BRIGHTNESS 35
#define LED_TYPE WS2813
#define COLOR_ORDER GRB

// Button configuration
#define NUM_GAME_BUTTONS 9
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
#define SEWER_HEIGHT 3
#define SEWER_WIDTH 5
#define SEWER_START_X 4
#define SEWER_START_Y 19

#define BASIN_HEIGHT 3
#define BASIN_WIDTH 5
#define BASIN_START_X 13
#define BASIN_START_Y 19

// Color definitions
#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
#define COLOR_GREEN_1 0x05FF00
#define COLOR_GREEN_2 0x0AFF00
#define COLOR_GREEN_3 0x08FF00
#define COLOR_GREEN_4 0x03FF00
#define COLOR_YELLOW 0xFFFF00
#define COLOR_BLUE 0x0000FF

// LED colors
#define SEWER_COLOR CRGB::Yellow
#define BASIN_COLOR CRGB::Blue
#define SEWER_EMPTY_COLOR CRGB(16, 16, 0)  // Dim yellow
#define BASIN_EMPTY_COLOR CRGB(0, 0, 16)   // Dim blue

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



// The smooth startup sequence gradually increases the LED brightness, which might help prevent the flash you're seeing. If you're still experiencing the flash, it could indeed be related to power delivery. 
// You might want to consider:
// Using a larger capacitor (e.g., 1000µF) + a ceramic 104
// Adding a small resistor (e.g., 470 ohms) in series with the data line to the first LED.
// Ensuring all ground connections are solid, including between the power supply, Arduino, and LED strip.

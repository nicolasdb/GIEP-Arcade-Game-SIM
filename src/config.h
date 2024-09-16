#pragma once
#include <Arduino.h>
#include <FastLED.h>

// Hardware configuration
#define MATRIX_PIN 9
#define MATRIX_WIDTH 25
#define MATRIX_HEIGHT 25
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define LED_TYPE WS2813
#define COLOR_ORDER GRB
#define BRIGHTNESS 64  // Default brightness (0-255)

// MCP23017 configuration
#define MCP23017_ADDRESS 0x20
#define NUM_MCP_BUTTONS 8
#define TOTAL_BUTTONS 9
#define BUTTON9_PIN 1
#define LED9_PIN 3

// Secondary LED configuration
#define SECONDARY_LED_PIN 7
#define TOTAL_SECONDARY_LEDS 60

// Button configuration
#define DEBOUNCE_DELAY 50

// Debug settings
#define DEBUG_BUTTON_BLINK_DURATION 500 // ms

// FreeRTOS task configurations
#define BUTTON_TASK_STACK_SIZE 2048
#define BUTTON_TASK_PRIORITY 2
#define LED_UPDATE_TASK_STACK_SIZE 2048
#define LED_UPDATE_TASK_PRIORITY 1

// Use button LEDs
#define USE_BUTTON_LEDS true

// Helper function to get LED color for a specific LED index in the secondary array
#define GET_SECONDARY_LED_COLOR(led_index) (SECONDARY_LED_BITMAP[led_index])

// Color definitions
#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
#define COLOR_YELLOW 0xFFFF00
#define COLOR_BLUE 0x0000FF
#define COLOR_GREEN_1 0x00FF00
#define COLOR_GREEN_2 0x00E300
#define COLOR_GREEN_3 0x00C700
#define COLOR_GREEN_4 0x00AB00
#define COLOR_GREEN_5 0x008F00
#define COLOR_GREEN_6 0x007300
#define COLOR_GREEN_7 0x005700
#define COLOR_GREEN_8 0x003200
#define COLOR_RED 0xFF0000
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_PURPLE 0x6400FF

// External declaration for LED bitmaps
extern const uint32_t MAIN_LED_BITMAP[NUM_LEDS];
extern const uint32_t SECONDARY_LED_BITMAP[TOTAL_SECONDARY_LEDS];

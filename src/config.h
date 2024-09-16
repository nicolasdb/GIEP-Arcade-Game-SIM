#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "game_config.h"

// Debug configuration
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif

// Hardware configuration
#define MATRIX_PIN 9
#define MATRIX_WIDTH 25
#define MATRIX_HEIGHT 25
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define LED_TYPE WS2813
#define COLOR_ORDER GRB
#define MATRIX_ORIENTATION MatrixOrientation::TOP_LEFT_VERTICAL
#define BRIGHTNESS 64  // Default brightness (0-255)

// MCP23017 configuration
#define MCP23017_ADDRESS 0x20
#define NUM_MCP_BUTTONS 8
#define TOTAL_BUTTONS 9
#define BUTTON9_PIN 1
#define LED9_PIN 3
#define DEBUG_BUTTON_PIN 0

// Global switch for button LEDs (including button 9)
#define USE_BUTTON_LEDS false

// Watchdog configuration
#define WDT_TIMEOUT 15  // 15 seconds

// FreeRTOS task configurations
#define BUTTON_TASK_STACK_SIZE 2048
#define BUTTON_TASK_PRIORITY 2
#define GAME_UPDATE_TASK_STACK_SIZE 4096
#define GAME_UPDATE_TASK_PRIORITY 1
#define LED_UPDATE_TASK_STACK_SIZE 2048
#define LED_UPDATE_TASK_PRIORITY 1

// Secondary LED configuration
#define SECONDARY_LED_PIN 7
#define SECONDARY_NUM_ZONES 15
#define LEDS_PER_ZONE 4
#define TOTAL_SECONDARY_LEDS (SECONDARY_NUM_ZONES * LEDS_PER_ZONE)

// Function to initialize the watchdog timer
void initWatchdog();

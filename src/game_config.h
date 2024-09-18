#pragma once

// Game timing parameters (in milliseconds)
#define WAITING_RAINING_DURATION 10000
#define WAITING_DRY_DURATION 10000
#define RAINING_DURATION 10000
#define HEAVY_DURATION 20000
#define STORM_DURATION 25000
#define END_STATE_DURATION 5000

// Sewer mechanics parameters
#define SEWER_INCREASE_RATE_RAINING 0.002f
#define SEWER_INCREASE_RATE_HEAVY 0.004f
#define SEWER_INCREASE_RATE_STORM 0.006f
#define GIEP_EFFECT_STRENGTH 0.003f
#define SEWER_DRAIN_RATE 0.001f
#define BASIN_GATE_TRANSFER_RATE 0.005f

// Game balance parameters
#define SEWER_OVERFLOW_THRESHOLD 0.65f
#define BASIN_OVERFLOW_THRESHOLD 0.95f
#define WIN_THRESHOLD 0.5f

// Rain visual parameters
#define RAIN_INTENSITY_RAINING 0.005f
#define RAIN_INTENSITY_HEAVY 0.02f
#define RAIN_INTENSITY_STORM 0.04f
#define RAIN_MAX_TRAIL_LENGTH 2
#define RAIN_HEAVY_MULTIPLIER 5.0f
#define RAIN_STORM_MULTIPLIER 10.0f
#define RAIN_STORM_WIND_CHANCE 230 // 100% chance (out of 255)

// Brightness levels
#define BRIGHTNESS 30
#define ACTIVE_BRIGHTNESS 16
#define GIEP_ACTIVE_BRIGHTNESS 255
#define GIEP_INACTIVE_BRIGHTNESS 30
#define RAIN_BRIGHTNESS 190
#define SEWER_BRIGHTNESS 255
#define SEWER_EMPTY_BRIGHTNESS ACTIVE_BRIGHTNESS  // Updated to use ACTIVE_BRIGHTNESS
#define BASIN_BRIGHTNESS 255
#define BASIN_EMPTY_BRIGHTNESS 12
#define BASIN_GATE_BRIGHTNESS 255
#define BASIN_GATE_INACTIVE_BRIGHTNESS 40
#define BASIN_OVERFLOW_BRIGHTNESS 255
#define RIVER_BRIGHTNESS 255

// Blink parameters
#define BLINK_DURATION 500 // milliseconds

// Task configuration
#define BUTTON_TASK_STACK_SIZE 2048
#define GAME_UPDATE_TASK_STACK_SIZE 4096
#define LED_UPDATE_TASK_STACK_SIZE 2048
#define BUTTON_TASK_PRIORITY 3
#define GAME_UPDATE_TASK_PRIORITY 2
#define LED_UPDATE_TASK_PRIORITY 1
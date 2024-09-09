#pragma once

// Task configuration
#define BUTTON_TASK_STACK_SIZE 2048
#define GAME_UPDATE_TASK_STACK_SIZE 4096
#define LED_UPDATE_TASK_STACK_SIZE 2048
#define BUTTON_TASK_PRIORITY 3
#define GAME_UPDATE_TASK_PRIORITY 2
#define LED_UPDATE_TASK_PRIORITY 1

// Include the new BitmapConfig header
#include "BitmapConfig.h"

// Other essential includes and definitions
#include <Arduino.h>
#include <FastLED.h>

// Add any other necessary includes or definitions here

// Note: Game-specific constants will be moved to a JSON/YAML configuration file
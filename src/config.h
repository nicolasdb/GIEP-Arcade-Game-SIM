#pragma once
#include <FastLED.h>
#include "game_config.h"

// Debug configuration
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif

// Hardware configuration
#define LED_PIN 9
#define MATRIX_WIDTH 25
#define MATRIX_HEIGHT 25
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)
#define LED_TYPE WS2813
#define COLOR_ORDER GRB
#define MATRIX_ORIENTATION MatrixOrientation::TOP_LEFT_VERTICAL

// MCP23017 configuration
#define MCP23017_ADDRESS 0x20
#define NUM_MCP_BUTTONS 8
#define TOTAL_BUTTONS 9
#define BASIN_GATE_BUTTON_PIN 1
#define BASIN_GATE_LED_PIN 3
#define DEBUG_BUTTON_PIN 0

// Global switch for button LEDs (including button 9)
#define USE_BUTTON_LEDS false

// Watchdog configuration
#define WDT_TIMEOUT 15  // 15 seconds

// Consolidated color definitions
enum ZoneColors {
    COLOR_WHITE = 0xFFFFFF,
    COLOR_BLACK = 0x000000,
    COLOR_GREEN_1 = 0x00FF00,
    COLOR_GREEN_2 = 0x00E300,
    COLOR_GREEN_3 = 0x00C700,
    COLOR_GREEN_4 = 0x00AB00,
    COLOR_GREEN_5 = 0x008F00,
    COLOR_GREEN_6 = 0x007300,
    COLOR_GREEN_7 = 0x005700,
    COLOR_GREEN_8 = 0x003200,
    COLOR_YELLOW = 0xFFFF00,
    COLOR_BLUE = 0x0000FF,
    COLOR_RED = 0xFF0000,
    COLOR_MAGENTA = 0xFF00FF,
    COLOR_PURPLE = 0x6400FF,
    COLOR_DARK_BLUE = 0x0000E3,     // Used for Rain Level 1
    COLOR_MEDIUM_BLUE = 0x0000C7,   // Used for Rain Level 2
    COLOR_LIGHT_BLUE = 0x0000AB,    // Used for Rain Level 3
    COLOR_DARK_RED = 0xC70000,      // Used for Flood Death
    COLOR_MEDIUM_RED = 0xAB0000,    // Used for Pollution Death
    COLOR_CYAN = 0x00FFFF,          // Used for Win condition
};

// Default bitmap for the scene
const uint32_t DEFAULT_BITMAP[625] = {
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x00FF00, 0x00FF00, 0x00FF00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 0x008F00, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x008F00, 0x008F00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x008F00, 0x008F00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x00E300, 0x008F00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x00C700, 0x00C700, 0x00C700, 0xFFFFFF, 0xFFFFFF, 0x00E300, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x00C700, 0x00C700, 0x00C700, 0x00C700, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x00AB00, 0x00AB00, 0x00AB00, 0xFFFFFF, 0xFFFFFF, 0x00C700, 0x00C700, 0x00C700, 0x00C700, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x007300, 0x007300, 0x007300, 0x007300, 0x007300, 0x007300, 
    0xFFFFFF, 0xFFFFFF, 0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00, 0xFFFFFF, 0xFFFFFF, 0x00C700, 0x00C700, 0x00C700, 0x00C700, 0x005700, 0x005700, 0x005700, 0x005700, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x007300, 0x007300, 0x007300, 0x007300, 0x007300, 0x007300, 
    0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x005700, 0x005700, 0x005700, 0x005700, 0x005700, 0x005700, 0xFFFFFF, 0x007300, 0x007300, 0x007300, 0x007300, 0x007300, 0x007300, 
    0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x005700, 0x005700, 0x005700, 0x005700, 0x005700, 0x005700, 0x005700, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x003200, 0x003200, 0x003200, 0x003200, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x003200, 0x003200, 0x003200, 0x003200, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFFFF, 0x003200, 0x003200, 0x003200, 0x003200, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x003200, 0x003200, 0x003200, 0x003200, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0x000000, 0x000000, 0xFFFF00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 
    0x000000, 0x000000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFF00FF, 0xFF00FF, 0xFF00FF, 0xFF00FF, 0xFF00FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 
    0x000000, 0x000000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000, 0xFF00FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 
    0x000000, 0x000000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFF0000, 0xFF0000, 0xFF0000, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000, 0x6400FF, 0x6400FF, 0x6400FF, 0x6400FF, 0x6400FF, 0x000000, 
    0x000000, 0x000000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000, 0x6400FF, 0x6400FF, 0x6400FF, 0x6400FF, 0x6400FF, 0x000000, 
    0x000000, 0x000000, 0x000000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0x000000, 0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000, 0x6400FF, 0x6400FF, 0x6400FF, 0x6400FF, 0x6400FF, 0x000000, 
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000, 0x6400FF, 0x6400FF, 0x6400FF, 0x6400FF, 0x6400FF, 0x000000, 
    0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
};

// Secondary LED bitmap (4x15 pixels)
// Each uint32_t represents a column of 4 LEDs for a zone
const uint32_t SECONDARY_LED_BITMAP[60] = {
    0x00FF00, 0x00FF00, 0x00FF00, 0x00FF00,
    0x00E300, 0x00E300, 0x00E300, 0x00E300,
    0x00C700, 0x00C700, 0x00C700, 0x00C700,
    0x00AB00, 0x00AB00, 0x00AB00, 0x00AB00,
    0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000,
    0x003200, 0x003200, 0x003200, 0x003200,
    0x005700, 0x005700, 0x005700, 0x005700,
    0x007300, 0x007300, 0x007300, 0x007300,
    0x008F00, 0x008F00, 0x008F00, 0x008F00,  // Changed from 0x00FFFF to 0x008F00 for GIEP_5_ZONE
    0xC70000, 0xC70000, 0xAB0000, 0xAB0000,     
    0x0000AB, 0x0000AB, 0x0000AB, 0x0000AB,   
    0x0000C7, 0x0000C7, 0x0000C7, 0x0000C7,    
    0x0000E3, 0x0000E3, 0x0000E3, 0x0000E3  
};

// Secondary LED configuration
#define SECONDARY_LED_PIN 7
#define SECONDARY_NUM_ZONES 15
#define LEDS_PER_ZONE 4
#define TOTAL_SECONDARY_LEDS (SECONDARY_NUM_ZONES * LEDS_PER_ZONE)

// Secondary LED zone indices (reordered to match hardware setup)
#define GIEP_1_ZONE 0
#define GIEP_2_ZONE 1
#define GIEP_3_ZONE 2
#define GIEP_4_ZONE 3
#define BASIN_GATE_ZONE 4
#define GIEP_8_ZONE 5
#define GIEP_7_ZONE 6
#define GIEP_6_ZONE 7
#define GIEP_5_ZONE 8
#define WIN_ZONE 9
#define FLOOD_DEATH_ZONE 10
#define POLLUTION_DEATH_ZONE 11
#define RAIN_LEVEL_3_ZONE 12
#define RAIN_LEVEL_2_ZONE 13
#define RAIN_LEVEL_1_ZONE 14

// New mapping structure for secondary LED zones
struct SecondaryLEDZoneMapping {
    uint8_t start_index;
    uint8_t length;
};

const SecondaryLEDZoneMapping SECONDARY_LED_ZONE_MAPPING[SECONDARY_NUM_ZONES] = {
    {0, 4},   // GIEP_1_ZONE
    {4, 4},   // GIEP_2_ZONE
    {8, 4},   // GIEP_3_ZONE
    {12, 4},  // GIEP_4_ZONE
    {16, 4},  // BASIN_GATE_ZONE
    {20, 4},  // GIEP_8_ZONE
    {24, 4},  // GIEP_7_ZONE
    {28, 4},  // GIEP_6_ZONE
    {32, 4},  // GIEP_5_ZONE
    {36, 4},  // WIN_ZONE
    {40, 2},  // FLOOD_DEATH_ZONE
    {42, 2},  // POLLUTION_DEATH_ZONE
    {44, 4},  // RAIN_LEVEL_3_ZONE
    {48, 4},  // RAIN_LEVEL_2_ZONE
    {52, 4}   // RAIN_LEVEL_1_ZONE
};

// Helper function to get LED color for a specific zone and LED index
#define GET_SECONDARY_LED_COLOR(zone, led_index) \
    ((SECONDARY_LED_BITMAP[SECONDARY_LED_ZONE_MAPPING[zone].start_index + \
    (led_index % SECONDARY_LED_ZONE_MAPPING[zone].length)]))

// LED colors
#define SEWER_COLOR CRGB::Yellow
#define BASIN_COLOR CRGB::Blue
#define SEWER_EMPTY_COLOR CRGB(GameConfig::Brightness::ACTIVE_BRIGHTNESS, GameConfig::Brightness::ACTIVE_BRIGHTNESS, GameConfig::Brightness::ACTIVE_BRIGHTNESS)  // White with ACTIVE_BRIGHTNESS
#define BASIN_EMPTY_COLOR CRGB(0, 0, 16)   // Dim blue
#define BASIN_GATE_COLOR CRGB::Red
#define BASIN_OVERFLOW_COLOR CRGB::Red  // Changed from Green to Red
#define RIVER_COLOR CRGB(100, 0, 255)  // Purple color for the river

// Function declarations for LED control
void setFloodDeathZone(CRGB* leds, bool active);
void setPollutionDeathZone(CRGB* leds, bool active);
void updateDeathZones(CRGB* leds, bool isGameOver, bool isFloodDeath, bool isPollutionDeath);

#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

enum class SecondaryLEDZone {
    NONE,
    GIEP_1,
    GIEP_2,
    GIEP_3,
    GIEP_4,
    GIEP_5,
    GIEP_6,
    GIEP_7,
    GIEP_8,
    BASIN_GATE,
    RAIN_LEVEL_1,
    RAIN_LEVEL_2,
    RAIN_LEVEL_3,
    FLOOD_DEATH,
    POLLUTION_DEATH,
    WIN
};

class SecondaryLEDHandler {
public:
    SecondaryLEDHandler();
    void begin();
    void update();
    void setZoneState(SecondaryLEDZone zone, bool state);
    void setRainLevel(uint8_t level);
    void setEndGameState(SecondaryLEDZone state);
    static const char* getZoneName(SecondaryLEDZone zone);  // Added declaration

private:
    CRGB leds[TOTAL_SECONDARY_LEDS];
    bool zoneStates[SECONDARY_NUM_ZONES];
    SecondaryLEDZone endGameState;
    uint8_t rainLevel;
    unsigned long lastBlinkTime;

    void updateNormalState();
    void updateEndGameState();
    CRGB getColorForZone(SecondaryLEDZone zone);
    void updateRainLevelIndicators();
    uint8_t getZoneIndexFromBitmap(SecondaryLEDZone zone);
};
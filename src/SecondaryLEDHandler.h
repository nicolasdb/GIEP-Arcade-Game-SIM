#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

enum class SecondaryLEDZone {
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
    void blinkAll(CRGB color);
    void stopBlinking();
    void setZoneState(SecondaryLEDZone zone, bool state, CRGB color);

private:
    CRGB leds[TOTAL_SECONDARY_LEDS];
    bool zoneStates[SECONDARY_NUM_ZONES];
    SecondaryLEDZone endGameState;
    uint8_t rainLevel;
    bool isBlinking;
    CRGB blinkColor;
    unsigned long lastBlinkTime;

    void updateLEDs();
    CRGB getColorForZone(SecondaryLEDZone zone);
    void updateRainLevelIndicators();
    uint8_t getZoneIndexFromBitmap(SecondaryLEDZone zone);
    void updateBlinkEffect();
};
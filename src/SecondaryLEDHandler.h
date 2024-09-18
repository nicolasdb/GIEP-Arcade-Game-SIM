#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <array>
#include "config.h"
#include "game_config.h"

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

enum class RainLevel {
    NONE,
    LIGHT,
    MODERATE,
    HEAVY
};

class SecondaryLEDHandler {
public:
    SecondaryLEDHandler();
    void begin();
    void update();
    void setZoneState(SecondaryLEDZone zone, bool state);
    void setRainLevel(RainLevel level);
    void setEndGameState(SecondaryLEDZone state);
    static const char* getZoneName(SecondaryLEDZone zone);
    void setFloodZoneColor(uint8_t r, uint8_t g, uint8_t b);

private:
    static constexpr size_t SECONDARY_LED_COUNT = TOTAL_SECONDARY_LEDS;
    static constexpr size_t NUM_ZONES = SECONDARY_NUM_ZONES;

    std::array<CRGB, SECONDARY_LED_COUNT> leds;
    std::array<bool, NUM_ZONES> zoneStates;
    SecondaryLEDZone endGameState;
    RainLevel rainLevel;
    unsigned long lastBlinkTime;
    CRGB floodZoneColor;

    void updateNormalState();
    void updateEndGameState();
    CRGB getColorForZone(SecondaryLEDZone zone);
    void updateRainLevelIndicators();
    uint8_t getZoneIndexFromBitmap(SecondaryLEDZone zone);
};
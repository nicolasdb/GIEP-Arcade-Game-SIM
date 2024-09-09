#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

class ConfigLoader {
public:
    static bool loadConfig(const char* filename);

    // Getter methods for configuration values
    static uint32_t getTimingValue(const char* key);
    static float getSewerMechanicsValue(const char* key);
    static float getGameBalanceValue(const char* key);
    static float getRainVisualsValue(const char* key);
    static uint8_t getBrightnessLevel(const char* key);
    static uint16_t getBlinkDuration();

    // New getter methods for specific constants
    static uint32_t WAITING_RAINING_DURATION();
    static uint32_t WAITING_DRY_DURATION();
    static uint32_t RAINING_DURATION();
    static uint32_t HEAVY_DURATION();
    static uint32_t STORM_DURATION();
    static uint32_t END_STATE_DURATION();

    static float SEWER_INCREASE_RATE_RAINING();
    static float SEWER_INCREASE_RATE_HEAVY();
    static float SEWER_INCREASE_RATE_STORM();
    static float GIEP_EFFECT_STRENGTH();
    static float SEWER_DRAIN_RATE();
    static float BASIN_GATE_TRANSFER_RATE();

    static float SEWER_OVERFLOW_THRESHOLD();
    static float BASIN_OVERFLOW_THRESHOLD();
    static float WIN_THRESHOLD();

    static float RAIN_INTENSITY_RAINING();
    static float RAIN_INTENSITY_HEAVY();
    static float RAIN_INTENSITY_STORM();
    static uint8_t RAIN_MAX_TRAIL_LENGTH();
    static float RAIN_HEAVY_MULTIPLIER();
    static float RAIN_STORM_MULTIPLIER();
    static uint8_t RAIN_STORM_WIND_CHANCE();

    static uint8_t BRIGHTNESS();
    static uint8_t ACTIVE_BRIGHTNESS();
    static uint8_t GIEP_ACTIVE_BRIGHTNESS();
    static uint8_t GIEP_INACTIVE_BRIGHTNESS();
    static uint8_t RAIN_BRIGHTNESS();
    static uint8_t SEWER_BRIGHTNESS();
    static uint8_t SEWER_EMPTY_BRIGHTNESS();
    static uint8_t BASIN_BRIGHTNESS();
    static uint8_t BASIN_EMPTY_BRIGHTNESS();
    static uint8_t BASIN_GATE_BRIGHTNESS();
    static uint8_t BASIN_GATE_INACTIVE_BRIGHTNESS();
    static uint8_t BASIN_OVERFLOW_BRIGHTNESS();
    static uint8_t RIVER_BRIGHTNESS();

private:
    static StaticJsonDocument<2048> config; // Adjust size as needed
};
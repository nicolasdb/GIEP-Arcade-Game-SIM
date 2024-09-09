#include "ConfigLoader.h"
#include <SPIFFS.h>
#include "DebugLogger.h"

StaticJsonDocument<2048> ConfigLoader::config;

bool ConfigLoader::loadConfig(const char* filename) {
    if (!SPIFFS.begin(true)) {
        DebugLogger::error("An error occurred while mounting SPIFFS");
        return false;
    }

    File file = SPIFFS.open(filename, "r");
    if (!file) {
        DebugLogger::error("Failed to open config file");
        return false;
    }

    DeserializationError error = deserializeJson(config, file);
    file.close();

    if (error) {
        DebugLogger::error("Failed to parse config file: %s", error.c_str());
        return false;
    }

    DebugLogger::info("Config file loaded successfully");
    return true;
}

uint32_t ConfigLoader::getTimingValue(const char* key) {
    return config["timing"][key] | 0;
}

float ConfigLoader::getSewerMechanicsValue(const char* key) {
    return config["sewer_mechanics"][key] | 0.0f;
}

float ConfigLoader::getGameBalanceValue(const char* key) {
    return config["game_balance"][key] | 0.0f;
}

float ConfigLoader::getRainVisualsValue(const char* key) {
    return config["rain_visuals"][key] | 0.0f;
}

uint8_t ConfigLoader::getBrightnessLevel(const char* key) {
    return config["brightness_levels"][key] | 0;
}

uint16_t ConfigLoader::getBlinkDuration() {
    return config["blink_duration"] | 500;
}

// New getter methods implementation
uint32_t ConfigLoader::WAITING_RAINING_DURATION() { return getTimingValue("waiting_raining_duration"); }
uint32_t ConfigLoader::WAITING_DRY_DURATION() { return getTimingValue("waiting_dry_duration"); }
uint32_t ConfigLoader::RAINING_DURATION() { return getTimingValue("raining_duration"); }
uint32_t ConfigLoader::HEAVY_DURATION() { return getTimingValue("heavy_duration"); }
uint32_t ConfigLoader::STORM_DURATION() { return getTimingValue("storm_duration"); }
uint32_t ConfigLoader::END_STATE_DURATION() { return getTimingValue("end_state_duration"); }

float ConfigLoader::SEWER_INCREASE_RATE_RAINING() { return getSewerMechanicsValue("increase_rate_raining"); }
float ConfigLoader::SEWER_INCREASE_RATE_HEAVY() { return getSewerMechanicsValue("increase_rate_heavy"); }
float ConfigLoader::SEWER_INCREASE_RATE_STORM() { return getSewerMechanicsValue("increase_rate_storm"); }
float ConfigLoader::GIEP_EFFECT_STRENGTH() { return getSewerMechanicsValue("giep_effect_strength"); }
float ConfigLoader::SEWER_DRAIN_RATE() { return getSewerMechanicsValue("drain_rate"); }
float ConfigLoader::BASIN_GATE_TRANSFER_RATE() { return getSewerMechanicsValue("basin_gate_transfer_rate"); }

float ConfigLoader::SEWER_OVERFLOW_THRESHOLD() { return getGameBalanceValue("sewer_overflow_threshold"); }
float ConfigLoader::BASIN_OVERFLOW_THRESHOLD() { return getGameBalanceValue("basin_overflow_threshold"); }
float ConfigLoader::WIN_THRESHOLD() { return getGameBalanceValue("win_threshold"); }

float ConfigLoader::RAIN_INTENSITY_RAINING() { return getRainVisualsValue("intensity_raining"); }
float ConfigLoader::RAIN_INTENSITY_HEAVY() { return getRainVisualsValue("intensity_heavy"); }
float ConfigLoader::RAIN_INTENSITY_STORM() { return getRainVisualsValue("intensity_storm"); }
uint8_t ConfigLoader::RAIN_MAX_TRAIL_LENGTH() { return static_cast<uint8_t>(getRainVisualsValue("max_trail_length")); }
float ConfigLoader::RAIN_HEAVY_MULTIPLIER() { return getRainVisualsValue("heavy_multiplier"); }
float ConfigLoader::RAIN_STORM_MULTIPLIER() { return getRainVisualsValue("storm_multiplier"); }
uint8_t ConfigLoader::RAIN_STORM_WIND_CHANCE() { return static_cast<uint8_t>(getRainVisualsValue("storm_wind_chance")); }

uint8_t ConfigLoader::BRIGHTNESS() { return getBrightnessLevel("default"); }
uint8_t ConfigLoader::ACTIVE_BRIGHTNESS() { return getBrightnessLevel("active"); }
uint8_t ConfigLoader::GIEP_ACTIVE_BRIGHTNESS() { return getBrightnessLevel("giep_active"); }
uint8_t ConfigLoader::GIEP_INACTIVE_BRIGHTNESS() { return getBrightnessLevel("giep_inactive"); }
uint8_t ConfigLoader::RAIN_BRIGHTNESS() { return getBrightnessLevel("rain"); }
uint8_t ConfigLoader::SEWER_BRIGHTNESS() { return getBrightnessLevel("sewer"); }
uint8_t ConfigLoader::SEWER_EMPTY_BRIGHTNESS() { return getBrightnessLevel("sewer_empty"); }
uint8_t ConfigLoader::BASIN_BRIGHTNESS() { return getBrightnessLevel("basin"); }
uint8_t ConfigLoader::BASIN_EMPTY_BRIGHTNESS() { return getBrightnessLevel("basin_empty"); }
uint8_t ConfigLoader::BASIN_GATE_BRIGHTNESS() { return getBrightnessLevel("basin_gate"); }
uint8_t ConfigLoader::BASIN_GATE_INACTIVE_BRIGHTNESS() { return getBrightnessLevel("basin_gate_inactive"); }
uint8_t ConfigLoader::BASIN_OVERFLOW_BRIGHTNESS() { return getBrightnessLevel("basin_overflow"); }
uint8_t ConfigLoader::RIVER_BRIGHTNESS() { return getBrightnessLevel("river"); }
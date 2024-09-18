#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <array>
#include "MatrixConfig.h"
#include "game_config.h"
#include "config.h"

enum class RainMode {
    NORMAL,
    HEAVY,
    STORM
};

struct RainDrop {
    uint8_t y;
    uint8_t trailLength;
};

class RainSystem {
public:
    RainSystem(const MatrixConfig& config);

    void update(const bool* buildingMap);
    void draw(CRGB* leds) const;
    void setIntensity(float intensity);
    float getIntensity() const;
    void setVisible(bool visible);
    void setMode(RainMode mode);

private:
    static constexpr uint8_t RAIN_MAX_TRAIL_LENGTH = 4;
    static constexpr float RAIN_HEAVY_MULTIPLIER = 1.5f;
    static constexpr float RAIN_STORM_MULTIPLIER = 2.0f;
    static constexpr uint8_t RAIN_STORM_WIND_CHANCE = 64; // 25% chance
    static constexpr uint8_t RAIN_BRIGHTNESS = 64;

    const MatrixConfig& matrixConfig;
    std::array<RainDrop, MATRIX_WIDTH> rainDrops;
    uint8_t width;
    uint8_t height;
    float intensity;
    bool isVisible;
    RainMode mode;

    void initializeRain();
};
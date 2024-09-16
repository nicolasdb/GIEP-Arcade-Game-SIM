#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "MatrixConfig.h"
#include "game_config.h"

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
    ~RainSystem();

    void update(const bool* buildingMap);
    void draw(CRGB* leds) const;
    void setIntensity(float intensity);
    float getIntensity() const;
    void setVisible(bool visible);
    void setMode(RainMode mode);

private:
    const MatrixConfig& matrixConfig;
    RainDrop* rainDrops;
    uint8_t width;
    uint8_t height;
    float intensity;
    bool isVisible;
    RainMode mode;

    void initializeRain();
    void cleanupRain();
};
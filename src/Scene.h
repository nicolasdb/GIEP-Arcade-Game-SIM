#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "MatrixConfig.h"
#include "DebugLogger.h"

enum class PixelType {
    ACTIVE,       // White: Rain can pass through
    BUILDING,     // Black: Blocks rain
    STREET_LIGHT, // Yellow: Street light
    INTERACTIVE   // Green: Can be turned on/off with button
};

class Scene {
public:
    Scene(const MatrixConfig& config);
    void loadBitmap(const uint32_t* bitmap, uint8_t width, uint8_t height);
    PixelType getPixelType(uint8_t x, uint8_t y) const;
    void setPixelType(uint8_t x, uint8_t y, PixelType type);
    void draw(CRGB* leds) const;

private:
    const MatrixConfig& matrixConfig;
    PixelType* pixelMap;
    uint8_t width;
    uint8_t height;

    void initializePixelMap();
    void cleanupPixelMap();
    uint32_t getColorForPixelType(PixelType type) const;
};
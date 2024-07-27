#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "MatrixConfig.h"
#include "DebugLogger.h"

enum class PixelType {
    ACTIVE,       // White: Rain can pass through
    BUILDING,     // Black: Blocks rain
    SEWER,        // Yellow: Sewer level gauge
    BASIN,        // Blue: Basin d'orage level gauge
    INTERACTIVE_1 = 100,  // First shade of green
    INTERACTIVE_2,        // Second shade of green
    INTERACTIVE_3,        // Third shade of green
    INTERACTIVE_4,        // Fourth shade of green
    RESERVED_1 = 200,   // Red: Reserved for future use
    RESERVED_2          // Pink: Reserved for future use
};

class Scene {
public:
    Scene(const MatrixConfig& config);
    ~Scene();  // Add destructor declaration
    void loadBitmap(const uint32_t* bitmap, uint8_t width, uint8_t height);
    PixelType getPixelType(uint8_t x, uint8_t y) const;
    void setPixelType(uint8_t x, uint8_t y, PixelType type);
    void update();
    void draw(CRGB* leds) const;

private:
    const MatrixConfig& matrixConfig;
    PixelType* pixelMap;
    uint8_t* rainDrops;
    uint8_t width;
    uint8_t height;

    void initializePixelMap();
    void initializeRain();
    void cleanupPixelMap();
    void cleanupRain();
    CRGB getColorForPixelType(PixelType type) const;
    void updateRain();
};
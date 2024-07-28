#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "MatrixConfig.h"
#include "DebugLogger.h"
#include "config.h"

enum class PixelType {
    ACTIVE,
    BUILDING,
    SEWER,
    BASIN,
    GIEP_1,
    GIEP_2,
    GIEP_3,
    GIEP_4,
    GIEP_5,
    GIEP_6,
    GIEP_7,
    GIEP_8,
    RESERVED_1,
    RESERVED_2
};

class Scene {
public:
    Scene(const MatrixConfig& config);
    ~Scene();
    void loadBitmap(const uint32_t* bitmap, uint8_t width, uint8_t height);
    void loadDefaultScene();
    PixelType getPixelType(uint8_t x, uint8_t y) const;
    void setPixelType(uint8_t x, uint8_t y, PixelType type);
    void update();
    void draw(CRGB* leds) const;
    void setGIEPState(uint8_t giepIndex, bool state);
    void setSewerLevel(float level);
    void setBasinLevel(float level);
    void setRainIntensity(float intensity);
    float getRainIntensity() const;
    void setRainVisible(bool visible); 

private:
    const MatrixConfig& matrixConfig;
    PixelType* pixelMap;
    uint8_t* rainDrops;
    uint8_t width;
    uint8_t height;
    bool giepStates[8];
    float sewerLevel;
    float basinLevel;
    float rainIntensity;
    bool isRainVisible;

    void initializePixelMap();
    void initializeRain();
    void cleanupPixelMap();
    void cleanupRain();
    CRGB getColorForPixelType(PixelType type) const;
    void updateRain();
    void drawWaterLevel(CRGB* leds, uint8_t startX, uint8_t startY, uint8_t width, uint8_t height, float level, CRGB fullColor, CRGB emptyColor) const;

};
#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include <vector>
#include "MatrixConfig.h"
#include "DebugLogger.h"
#include "config.h"
#include "RainSystem.h"

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
    BASIN_GATE,
    BASIN_OVERFLOW,
    RIVER
};

struct Point {
    uint8_t x;
    uint8_t y;
    Point(uint8_t _x, uint8_t _y) : x(_x), y(_y) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
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
    void setBasinGateState(bool state);
    void setSewerLevel(float level);
    void setBasinLevel(float level);
    void setRainIntensity(float intensity);
    float getRainIntensity() const;
    void setRainVisible(bool visible); 
    void setRainMode(RainMode mode);
    CRGB getSewerColor() const;
    void setPollutionState(bool polluted);
    const bool* getBuildingMap() const;
    void setFloodState(bool state);  // New method

private:
    const MatrixConfig& matrixConfig;
    PixelType* pixelMap;
    bool* buildingMap;
    uint8_t width;
    uint8_t height;
    bool giepStates[8];
    bool basinGateActive;
    float sewerLevel;
    float basinLevel;
    bool isBasinOverflow;
    std::vector<Point> sewerShape;
    std::vector<Point> basinShape;
    std::vector<Point> basinGateShape;
    std::vector<Point> basinOverflowShape;
    std::vector<Point> riverShape;
    uint8_t riverFlowOffset;
    bool isPolluted;
    bool isFloodState;  // New member variable
    RainSystem rainSystem;

    void initializePixelMap();
    void initializeBuildingMap();
    void cleanupPixelMap();
    void cleanupBuildingMap();
    CRGB getColorForPixelType(PixelType type) const;
    void drawWaterLevel(CRGB* leds, const std::vector<Point>& shape, float level, CRGB fullColor, CRGB emptyColor) const;
    void detectShapes();
    void floodFill(uint8_t startX, uint8_t startY, PixelType targetType, std::vector<Point>& shape, std::vector<bool>& visited);
    void updateOverflowState();
    void updateRiverFlow();
    void drawRiver(CRGB* leds) const;
};
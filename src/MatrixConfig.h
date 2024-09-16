#ifndef MATRIX_CONFIG_H
#define MATRIX_CONFIG_H

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "array1.h"

enum class MatrixOrientation {
    TOP_LEFT_HORIZONTAL,
    TOP_LEFT_VERTICAL,
    BOTTOM_LEFT_HORIZONTAL,
    BOTTOM_LEFT_VERTICAL,
    BOTTOM_RIGHT_VERTICAL 
};

class MatrixConfig {
public:
    static void initialize(uint8_t width, uint8_t height, MatrixOrientation orientation, bool zigzag);
    static void updateMatrix();
    static void setPixel(uint8_t x, uint8_t y, CRGB color, uint8_t brightness);
    static void setAllPixels(CRGB color, uint8_t brightness);
    static void clearMatrix();
    static void loadFromArray();
    static bool isBluePixel(uint8_t x, uint8_t y);
    static bool isYellowPixel(uint8_t x, uint8_t y);
    static void activateBluePixels();
    static void activateYellowPixels();
    static void activateBlueAndYellowPixels();
    static void deactivateAllPixels();
    
private:
    static CRGB leds[625];
    static bool isActivePixel[625];
    static uint8_t width;
    static uint8_t height;
    static MatrixOrientation orientation;
    static bool zigzag;

    static uint16_t XY(uint8_t x, uint8_t y);
    static const char* orientationToString(MatrixOrientation o);
};

#endif // MATRIX_CONFIG_H
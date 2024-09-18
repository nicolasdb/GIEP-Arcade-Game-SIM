#ifndef MATRIX_CONFIG_H
#define MATRIX_CONFIG_H

#include "config.h"
#include <FastLED.h>

class MatrixConfig {
public:
    MatrixConfig(int pin, CRGB* leds, int width, int height);
    void init();
    void clear();
    void show();
    void setPixel(int x, int y, CRGB color);
    CRGB getPixel(int x, int y);
    void drawBitmap(const uint8_t* bitmap, int width, int height, CRGB color);
    void loadArray(const uint32_t* sourceArray);

private:
    CRGB* leds;
    int width;
    int height;
    int numLEDs;
    int dataPin;

    uint16_t XY(uint8_t x, uint8_t y);
};

#endif // MATRIX_CONFIG_H
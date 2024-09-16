#ifndef MATRIX_CONFIG_H
#define MATRIX_CONFIG_H

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

class MatrixConfig {
public:
    static void initialize();
    static void updateMatrix();
    static void setAllPixels(CRGB color);
    static void clearMatrix();
    
private:
    static CRGB leds[25*25];
};

#endif // MATRIX_CONFIG_H
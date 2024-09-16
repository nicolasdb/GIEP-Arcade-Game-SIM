#ifndef SECONDARY_LED_HANDLER_H
#define SECONDARY_LED_HANDLER_H

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "array2.h"

class SecondaryLEDHandler {
public:
    static void initialize(uint16_t numLeds);
    static void updateLEDs();
    static void setPixel(uint16_t index, CRGB color, uint8_t brightness);
    static void setAllPixels(CRGB color, uint8_t brightness);
    static void clearLEDs();
    static void loadFromArray();
    static bool isBluePixel(uint16_t index);
    static bool isYellowPixel(uint16_t index);
    static void activateBluePixels();
    static void activateYellowPixels();
    static void activateBlueAndYellowPixels();
    static void deactivateAllPixels();
    
private:
    static CRGB* leds;
    static bool* isActivePixel;
    static uint16_t numLeds;
};

#endif // SECONDARY_LED_HANDLER_H
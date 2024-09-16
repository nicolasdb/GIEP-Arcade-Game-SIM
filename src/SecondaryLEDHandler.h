#ifndef SECONDARY_LED_HANDLER_H
#define SECONDARY_LED_HANDLER_H

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

class SecondaryLEDHandler {
public:
    static void initialize();
    static void updateLEDs();
    static void setAllPixels(CRGB color);
    static void clearLEDs();
    
private:
    static CRGB leds[4*14];
};

#endif // SECONDARY_LED_HANDLER_H
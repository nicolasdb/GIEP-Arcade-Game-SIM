#pragma once

#include <FastLED.h>
#include "array2.h"

class SecondaryLEDHandler {
public:
    SecondaryLEDHandler();
    void update();

private:
    CRGB secondaryLeds[sizeof(SECONDARY_LED_BITMAP) / sizeof(SECONDARY_LED_BITMAP[0])];

    void displaySecondaryArray();
    void setZone(uint8_t zone, bool state);
    void blinkZone(uint8_t zone);
};
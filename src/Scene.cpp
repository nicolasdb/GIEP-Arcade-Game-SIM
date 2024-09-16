#include <Arduino.h>
#include <FastLED.h>
#include "../include/array1.h"
#include "../include/array2.h"

void drawMainLEDs(CRGB leds[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(MAIN_LED_BITMAP[i]);
    }
}

void drawSecondaryLEDs(CRGB secondaryLeds[TOTAL_SECONDARY_LEDS]) {
    for (int i = 0; i < TOTAL_SECONDARY_LEDS; i++) {
        secondaryLeds[i] = CRGB(SECONDARY_LED_BITMAP[i]);
    }
}
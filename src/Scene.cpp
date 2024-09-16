#include "Scene.h"
#include <FastLED.h>
#include "config.h"
#include "array1.h"
#include "array2.h" 

void drawMainLEDs(CRGB (&leds)[NUM_LEDS]) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(pgm_read_byte(&MAIN_LED_BITMAP[i]));
    }
}

void drawSecondaryLEDs(CRGB (&secondaryLeds)[NUM_SECONDARY_LEDS]) {
    for (int i = 0; i < NUM_SECONDARY_LEDS; i++) {
        secondaryLeds[i] = CRGB(SECONDARY_LED_BITMAP[i]);
    }
}
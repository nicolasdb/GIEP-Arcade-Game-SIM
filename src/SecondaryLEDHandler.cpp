#include "SecondaryLEDHandler.h"
#include <FastLED.h>
#include "array2.h"
#include <cstdint>
#include "config.h"

SecondaryLEDHandler::SecondaryLEDHandler() {}

void SecondaryLEDHandler::update() {
    displaySecondaryArray();
}

void SecondaryLEDHandler::displaySecondaryArray() {
    for (int i = 0; i < NUM_SECONDARY_LEDS; i++) {
        secondaryLeds[i] = CRGB(SECONDARY_LED_BITMAP[i]);
    }
    FastLED.show(); 
}

void SecondaryLEDHandler::setZone(uint8_t zone, bool state) {
    // TODO: Implement LED control for button presses
}

void SecondaryLEDHandler::blinkZone(uint8_t zone) {
    // TODO: Implement LED blinking for button presses  
}
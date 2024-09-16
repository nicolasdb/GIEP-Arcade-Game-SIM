#include "SecondaryLEDHandler.h"
#include "GIEP4x14.h"

CRGB SecondaryLEDHandler::leds[4*14];

void SecondaryLEDHandler::initialize() {
    FastLED.addLeds<WS2813, SECONDARY_ARRAY_DATA_PIN, GRB>(leds, 4*14);
    FastLED.setBrightness(LED_BRIGHTNESS);
    clearLEDs();
}

void SecondaryLEDHandler::updateLEDs() {
    FastLED.show();
}

void SecondaryLEDHandler::setAllPixels(CRGB color) {
    for (int i = 0; i < 4*14; i++) {
        leds[i] = color;
    }
}

void SecondaryLEDHandler::clearLEDs() {
    setAllPixels(CRGB::Black);
    updateLEDs();
}
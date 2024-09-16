#include "MatrixConfig.h"
#include "MAP25x25.h"

CRGB MatrixConfig::leds[25*25];

void MatrixConfig::initialize() {
    FastLED.addLeds<WS2813, MATRIX_DATA_PIN, GRB>(leds, 25*25);
    FastLED.setBrightness(LED_BRIGHTNESS);
    clearMatrix();
}

void MatrixConfig::updateMatrix() {
    FastLED.show();
}

void MatrixConfig::setAllPixels(CRGB color) {
    for (int i = 0; i < 25*25; i++) {
        leds[i] = color;
    }
}

void MatrixConfig::clearMatrix() {
    setAllPixels(CRGB::Black);
    updateMatrix();
}
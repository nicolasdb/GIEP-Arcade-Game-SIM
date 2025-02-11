#include "SecondaryLEDHandler.h"

CRGB* SecondaryLEDHandler::leds = nullptr;
bool* SecondaryLEDHandler::isActivePixel = nullptr;
uint16_t SecondaryLEDHandler::numLeds = 0;

void SecondaryLEDHandler::initialize(uint16_t n) {
    numLeds = n;
    leds = new CRGB[numLeds];
    isActivePixel = new bool[numLeds];
    FastLED.addLeds<WS2812B, SECONDARY_ARRAY_DATA_PIN, GRB>(leds, numLeds);
    clearLEDs();
    loadFromArray();
}

void SecondaryLEDHandler::updateLEDs() {
    FastLED.show();
}

void SecondaryLEDHandler::setPixel(uint16_t index, CRGB color, uint8_t brightness) {
    if (index < numLeds) {
        leds[index] = color;
        leds[index].nscale8(brightness);
    }
}

void SecondaryLEDHandler::setAllPixels(CRGB color, uint8_t brightness) {
    for (uint16_t i = 0; i < numLeds; i++) {
        setPixel(i, color, brightness);
        isActivePixel[i] = false;
    }
}

void SecondaryLEDHandler::clearLEDs() {
    setAllPixels(CRGB::Black, 255);
}

void SecondaryLEDHandler::loadFromArray() {
    for (uint16_t i = 0; i < numLeds; i++) {
        CRGB color = CRGB(SECONDARY_BITMAP[i]);
        if (color == CRGB(MAP_WHITE)) {
            setPixel(i, CRGB(DISPLAY_WHITE), BRIGHTNESS_INACTIVE_WHITE);
        } else if (color == CRGB(MAP_BLUE)) {
            setPixel(i, CRGB(DISPLAY_BLUE), BRIGHTNESS_INACTIVE_BLUE);
        } else if (color == CRGB(MAP_YELLOW)) {
            setPixel(i, CRGB(DISPLAY_DEEP_BLUE), BRIGHTNESS_INACTIVE_DEEP_BLUE);
        } else {
            setPixel(i, color, 255);
        }
        isActivePixel[i] = false;
    }
}

bool SecondaryLEDHandler::isBluePixel(uint16_t index) {
    return SECONDARY_BITMAP[index] == MAP_BLUE;
}

bool SecondaryLEDHandler::isYellowPixel(uint16_t index) {
    return SECONDARY_BITMAP[index] == MAP_YELLOW;
}

void SecondaryLEDHandler::activateBluePixels() {
    for (uint16_t i = 0; i < numLeds; i++) {
        if (isBluePixel(i)) {
            setPixel(i, CRGB(DISPLAY_BLUE), BRIGHTNESS_ACTIVE_BLUE);
            isActivePixel[i] = true;
        }
    }
}

void SecondaryLEDHandler::activateYellowPixels() {
    for (uint16_t i = 0; i < numLeds; i++) {
        if (isYellowPixel(i)) {
            setPixel(i, CRGB(DISPLAY_DEEP_BLUE), BRIGHTNESS_ACTIVE_DEEP_BLUE);
            isActivePixel[i] = true;
        }
    }
}

void SecondaryLEDHandler::deactivateAllPixels() {
    for (uint16_t i = 0; i < numLeds; i++) {
        if (isActivePixel[i]) {
            CRGB color = CRGB(SECONDARY_BITMAP[i]);
            if (color == CRGB(MAP_BLUE)) {
                setPixel(i, CRGB(DISPLAY_BLUE), BRIGHTNESS_INACTIVE_BLUE);
            } else if (color == CRGB(MAP_YELLOW)) {
                setPixel(i, CRGB(DISPLAY_DEEP_BLUE), BRIGHTNESS_INACTIVE_DEEP_BLUE);
            } else if (color == CRGB(MAP_WHITE)) {
                setPixel(i, CRGB(DISPLAY_WHITE), BRIGHTNESS_INACTIVE_WHITE);
            }
            isActivePixel[i] = false;
        }
    }
}

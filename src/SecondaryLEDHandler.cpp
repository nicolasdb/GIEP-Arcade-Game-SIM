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
        if (color == CRGB(COLOR_WHITE)) {
            setPixel(i, color, BRIGHTNESS_INACTIVE_WHITE);
        } else if (color == CRGB(COLOR_BLUE)) {
            setPixel(i, color, BRIGHTNESS_INACTIVE_BLUE);
        } else if (color == CRGB(COLOR_YELLOW)) {
            setPixel(i, color, BRIGHTNESS_INACTIVE_YELLOW);
        } else {
            setPixel(i, color, 255);
        }
        isActivePixel[i] = false;
    }
}

bool SecondaryLEDHandler::isBluePixel(uint16_t index) {
    return SECONDARY_BITMAP[index] == COLOR_BLUE;
}

bool SecondaryLEDHandler::isYellowPixel(uint16_t index) {
    return SECONDARY_BITMAP[index] == COLOR_YELLOW;
}

void SecondaryLEDHandler::activateBluePixels() {
    for (uint16_t i = 0; i < numLeds; i++) {
        if (isBluePixel(i)) {
            setPixel(i, CRGB(COLOR_WHITE), BRIGHTNESS_ACTIVE_WHITE);
            isActivePixel[i] = true;
        }
    }
}

void SecondaryLEDHandler::activateYellowPixels() {
    for (uint16_t i = 0; i < numLeds; i++) {
        if (isYellowPixel(i)) {
            setPixel(i, CRGB(COLOR_WHITE), BRIGHTNESS_ACTIVE_WHITE);
            isActivePixel[i] = true;
        }
    }
}

void SecondaryLEDHandler::activateBlueAndYellowPixels() {
    for (uint16_t i = 0; i < numLeds; i++) {
        if (isBluePixel(i) || isYellowPixel(i)) {
            setPixel(i, CRGB(COLOR_WHITE), BRIGHTNESS_ACTIVE_WHITE);
            isActivePixel[i] = true;
        }
    }
}

void SecondaryLEDHandler::deactivateAllPixels() {
    for (uint16_t i = 0; i < numLeds; i++) {
        if (isActivePixel[i]) {
            CRGB color = CRGB(SECONDARY_BITMAP[i]);
            if (color == CRGB(COLOR_BLUE)) {
                setPixel(i, color, BRIGHTNESS_INACTIVE_BLUE);
            } else if (color == CRGB(COLOR_YELLOW)) {
                setPixel(i, color, BRIGHTNESS_INACTIVE_YELLOW);
            } else if (color == CRGB(COLOR_WHITE)) {
                setPixel(i, color, BRIGHTNESS_INACTIVE_WHITE);
            }
            isActivePixel[i] = false;
        }
    }
}
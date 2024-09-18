#include "MatrixConfig.h"

MatrixConfig::MatrixConfig(int pin, CRGB* ledsArray, int w, int h)
    : leds(ledsArray), width(w), height(h), numLEDs(w * h), dataPin(pin) {
}

void MatrixConfig::init() {
    FastLED.addLeds<WS2813, RGB>(leds, numLEDs).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(MATRIX_BRIGHTNESS);
    clear();
    show();
}

void MatrixConfig::clear() {
    FastLED.clear(true);
}

void MatrixConfig::show() {
    FastLED.show();
}

void MatrixConfig::setPixel(int x, int y, CRGB color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        leds[XY(x, y)] = color;
    }
}

CRGB MatrixConfig::getPixel(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return leds[XY(x, y)];
    }
    return CRGB::Black;
}

void MatrixConfig::drawBitmap(const uint8_t* bitmap, int w, int h, CRGB color) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (bitmap[y * w + x]) {
                setPixel(x, y, color);
            }
        }
    }
}

void MatrixConfig::loadArray(const uint32_t* sourceArray) {
    for (int i = 0; i < numLEDs; i++) {
        leds[i] = CRGB(sourceArray[i]);
    }
}

uint16_t MatrixConfig::XY(uint8_t x, uint8_t y) {
    uint16_t i;
    
    if (width & 0x01) {
        // Odd-width matrix, like 25x25
        uint8_t reverseY = (height - 1) - y;
        if (reverseY & 0x01) {
            // Odd rows run backwards
            uint8_t reverseX = (width - 1) - x;
            i = (reverseY * width) + reverseX;
        } else {
            // Even rows run forwards
            i = (reverseY * width) + x;
        }
    } else {
        // Even-width matrix
        if (y & 0x01) {
            // Odd rows run backwards
            uint8_t reverseX = (width - 1) - x;
            i = (y * width) + reverseX;
        } else {
            // Even rows run forwards
            i = (y * width) + x;
        }
    }
    
    return i;
}
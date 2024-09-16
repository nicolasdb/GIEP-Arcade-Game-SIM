#include "MatrixConfig.h"

CRGB MatrixConfig::leds[625];
bool MatrixConfig::isActivePixel[625];
uint8_t MatrixConfig::width;
uint8_t MatrixConfig::height;
MatrixOrientation MatrixConfig::orientation;
bool MatrixConfig::zigzag;

void MatrixConfig::initialize(uint8_t w, uint8_t h, MatrixOrientation o, bool z) {
    width = w;
    height = h;
    orientation = o;
    zigzag = z;
    FastLED.addLeds<WS2812B, MATRIX_DATA_PIN, GRB>(leds, width * height);
    clearMatrix();
    loadFromArray();
}

void MatrixConfig::updateMatrix() {
    FastLED.show();
}

void MatrixConfig::setPixel(uint8_t x, uint8_t y, CRGB color, uint8_t brightness) {
    uint16_t index = XY(x, y);
    if (index < width * height) {
        leds[index] = color;
        leds[index].nscale8(brightness);
    }
}

void MatrixConfig::setAllPixels(CRGB color, uint8_t brightness) {
    for (uint16_t i = 0; i < width * height; i++) {
        leds[i] = color;
        leds[i].nscale8(brightness);
        isActivePixel[i] = false;
    }
}

void MatrixConfig::clearMatrix() {
    setAllPixels(CRGB::Black, 255);
}

void MatrixConfig::loadFromArray() {
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t index = XY(x, y);
            CRGB color = CRGB(MAIN_BITMAP[y * width + x]);
            if (color == CRGB(COLOR_WHITE) || color == CRGB(COLOR_BLUE) || color == CRGB(COLOR_YELLOW)) {
                setPixel(x, y, CRGB(COLOR_YELLOW_TINTED_WHITE), BRIGHTNESS_INACTIVE_YELLOW_TINTED_WHITE);
            } else {
                setPixel(x, y, color, 255);
            }
            isActivePixel[index] = false;
        }
    }
}

bool MatrixConfig::isBluePixel(uint8_t x, uint8_t y) {
    return MAIN_BITMAP[y * width + x] == COLOR_BLUE;
}

bool MatrixConfig::isYellowPixel(uint8_t x, uint8_t y) {
    return MAIN_BITMAP[y * width + x] == COLOR_YELLOW;
}

void MatrixConfig::activateBluePixels() {
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            if (isBluePixel(x, y)) {
                setPixel(x, y, CRGB(COLOR_BLUE), BRIGHTNESS_ACTIVE_BLUE);
                isActivePixel[XY(x, y)] = true;
            }
        }
    }
}

void MatrixConfig::activateYellowPixels() {
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            if (isYellowPixel(x, y)) {
                setPixel(x, y, CRGB(COLOR_YELLOW), BRIGHTNESS_ACTIVE_YELLOW);
                isActivePixel[XY(x, y)] = true;
            }
        }
    }
}

void MatrixConfig::activateBlueAndYellowPixels() {
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            if (isBluePixel(x, y) || isYellowPixel(x, y)) {
                setPixel(x, y, CRGB(COLOR_BLUE), BRIGHTNESS_ACTIVE_BLUE);
                isActivePixel[XY(x, y)] = true;
            }
        }
    }
}

void MatrixConfig::deactivateAllPixels() {
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t index = XY(x, y);
            if (isActivePixel[index]) {
                CRGB originalColor = CRGB(MAIN_BITMAP[y * width + x]);
                if (originalColor == CRGB(COLOR_BLUE) || originalColor == CRGB(COLOR_YELLOW) || originalColor == CRGB(COLOR_WHITE)) {
                    setPixel(x, y, CRGB(COLOR_YELLOW_TINTED_WHITE), BRIGHTNESS_INACTIVE_YELLOW_TINTED_WHITE);
                } else {
                    setPixel(x, y, originalColor, 255);
                }
                isActivePixel[index] = false;
            }
        }
    }
}

uint16_t MatrixConfig::XY(uint8_t x, uint8_t y) {
    if (x >= width || y >= height) {
        return 0;
    }

    uint16_t i;

    switch (orientation) {
        case MatrixOrientation::BOTTOM_LEFT_VERTICAL:
            if (zigzag) {
                if (x & 0x01) {
                    // Odd columns run bottom to top
                    i = (x * height) + y;
                } else {
                    // Even columns run top to bottom
                    i = (x * height) + (height - 1 - y);
                }
            } else {
                // Non-zigzag pattern
                i = (x * height) + (height - 1 - y);
            }
            break;
        
        // Add cases for other orientations if needed
        
        default:
            return 0;
    }

    return i;
}

const char* MatrixConfig::orientationToString(MatrixOrientation o) {
    switch (o) {
        case MatrixOrientation::TOP_LEFT_HORIZONTAL: return "TOP_LEFT_HORIZONTAL";
        case MatrixOrientation::TOP_LEFT_VERTICAL: return "TOP_LEFT_VERTICAL";
        case MatrixOrientation::BOTTOM_LEFT_HORIZONTAL: return "BOTTOM_LEFT_HORIZONTAL";
        case MatrixOrientation::BOTTOM_LEFT_VERTICAL: return "BOTTOM_LEFT_VERTICAL";
        case MatrixOrientation::BOTTOM_RIGHT_VERTICAL: return "BOTTOM_RIGHT_VERTICAL";
        default: return "UNKNOWN";
    }
}
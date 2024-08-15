#include "MatrixConfig.h"

MatrixConfig::MatrixConfig(uint8_t width, uint8_t height, MatrixOrientation orientation, bool zigzag)
    : width(width), height(height), orientation(orientation), zigzag(zigzag) {
    DebugLogger::info("MatrixConfig created: %dx%d, %s, %s",
                      width, height, 
                      orientationToString(orientation),
                      zigzag ? "zigzag" : "normal");
    validate();
}

uint8_t MatrixConfig::getWidth() const { return width; }
uint8_t MatrixConfig::getHeight() const { return height; }
MatrixOrientation MatrixConfig::getOrientation() const { return orientation; }
bool MatrixConfig::isZigzag() const { return zigzag; }
uint16_t MatrixConfig::getNumLeds() const { return width * height; }

uint16_t MatrixConfig::XY(uint8_t x, uint8_t y) const {
    DebugLogger::debug("XY input: x=%u, y=%u", static_cast<unsigned int>(x), static_cast<unsigned int>(y));
    
    if (x >= width || y >= height) {
        DebugLogger::error("Invalid coordinates: (%u, %u)", static_cast<unsigned int>(x), static_cast<unsigned int>(y));
        return 0; // Return first LED as a fallback
    }
    
    uint16_t i;
    switch(orientation) {
        case MatrixOrientation::TOP_LEFT_HORIZONTAL:
            if (zigzag && y % 2 == 1) {
                x = (width - 1) - x;
            }
            i = (y * width) + x;
            break;
        case MatrixOrientation::TOP_LEFT_VERTICAL:
            if (zigzag && x % 2 == 1) {
                y = (height - 1) - y;
            }
            i = (x * height) + y;
            break;
        case MatrixOrientation::BOTTOM_LEFT_HORIZONTAL:
            y = (height - 1) - y;
            if (zigzag && y % 2 == 1) {
                x = (width - 1) - x;
            }
            i = (y * width) + x;
            break;
        case MatrixOrientation::BOTTOM_LEFT_VERTICAL:
            x = (width - 1) - x;
            if (zigzag && x % 2 == 1) {
                y = (height - 1) - y;
            }
            i = (x * height) + y;
            break;
        case MatrixOrientation::BOTTOM_RIGHT_VERTICAL:
            x = (width - 1) - x;
            y = (height - 1) - y;
            if (zigzag && x % 2 == 1) {
                y = (height - 1) - y;
            }
            i = (x * height) + y;
            break;
    }
    DebugLogger::debug("XY mapping result: (%u, %u) -> %u", static_cast<unsigned int>(x), static_cast<unsigned int>(y), i);
    return i;
}

void MatrixConfig::validate() const {
    if (width == 0 || height == 0) {
        DebugLogger::error("Invalid matrix dimensions: %dx%d", width, height);
    }
    if (width * height > MAX_LEDS) {
        DebugLogger::error("Matrix size exceeds MAX_LEDS: %d > %d", width * height, MAX_LEDS);
    }
    DebugLogger::info("MatrixConfig validated successfully");
}

const char* MatrixConfig::orientationToString(MatrixOrientation o) {
    switch(o) {
        case MatrixOrientation::TOP_LEFT_HORIZONTAL: return "TOP_LEFT_HORIZONTAL";
        case MatrixOrientation::TOP_LEFT_VERTICAL: return "TOP_LEFT_VERTICAL";
        case MatrixOrientation::BOTTOM_LEFT_HORIZONTAL: return "BOTTOM_LEFT_HORIZONTAL";
        case MatrixOrientation::BOTTOM_LEFT_VERTICAL: return "BOTTOM_LEFT_VERTICAL";
        case MatrixOrientation::BOTTOM_RIGHT_VERTICAL: return "BOTTOM_RIGHT_VERTICAL";
        default: return "UNKNOWN";
    }
}
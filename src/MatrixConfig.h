#pragma once
#include <stdint.h>
#include "DebugLogger.h"

enum class MatrixOrientation {
    TOP_LEFT_HORIZONTAL,
    TOP_LEFT_VERTICAL,
    BOTTOM_LEFT_HORIZONTAL,
    BOTTOM_LEFT_VERTICAL
};

class MatrixConfig {
public:
    MatrixConfig(uint8_t width, uint8_t height, MatrixOrientation orientation, bool zigzag);
    
    uint8_t getWidth() const;
    uint8_t getHeight() const;
    MatrixOrientation getOrientation() const;
    bool isZigzag() const;
    uint16_t getNumLeds() const;
    
    uint16_t XY(uint8_t x, uint8_t y) const;
    void validate() const;

private:
    uint8_t width;
    uint8_t height;
    MatrixOrientation orientation;
    bool zigzag;

    static constexpr uint16_t MAX_LEDS = 1024; // Adjust as needed

    static const char* orientationToString(MatrixOrientation o);
};
#pragma once

#include <Arduino.h>
#include <FastLED.h>

class BitmapConfig {
public:
    static bool loadPrimaryBitmap(const char* filename);
    static bool loadSecondaryBitmap(const char* filename);

    static CRGB getPrimaryLEDColor(uint16_t index);
    static CRGB getSecondaryLEDColor(uint16_t index);

private:
    static uint8_t* primaryBitmap;
    static uint8_t* secondaryBitmap;
    static uint16_t primaryBitmapSize;
    static uint16_t secondaryBitmapSize;

    static bool loadBitmap(const char* filename, uint8_t*& bitmap, uint16_t& size);
};
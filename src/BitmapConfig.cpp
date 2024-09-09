#include "BitmapConfig.h"
#include <SPIFFS.h>
#include "DebugLogger.h"

uint8_t* BitmapConfig::primaryBitmap = nullptr;
uint8_t* BitmapConfig::secondaryBitmap = nullptr;
uint16_t BitmapConfig::primaryBitmapSize = 0;
uint16_t BitmapConfig::secondaryBitmapSize = 0;

bool BitmapConfig::loadPrimaryBitmap(const char* filename) {
    return loadBitmap(filename, primaryBitmap, primaryBitmapSize);
}

bool BitmapConfig::loadSecondaryBitmap(const char* filename) {
    return loadBitmap(filename, secondaryBitmap, secondaryBitmapSize);
}

CRGB BitmapConfig::getPrimaryLEDColor(uint16_t index) {
    if (index < primaryBitmapSize) {
        return CRGB(primaryBitmap[index * 3], primaryBitmap[index * 3 + 1], primaryBitmap[index * 3 + 2]);
    }
    return CRGB::Black;
}

CRGB BitmapConfig::getSecondaryLEDColor(uint16_t index) {
    if (index < secondaryBitmapSize) {
        return CRGB(secondaryBitmap[index * 3], secondaryBitmap[index * 3 + 1], secondaryBitmap[index * 3 + 2]);
    }
    return CRGB::Black;
}

bool BitmapConfig::loadBitmap(const char* filename, uint8_t*& bitmap, uint16_t& size) {
    if (!SPIFFS.begin(true)) {
        DebugLogger::error("An error occurred while mounting SPIFFS");
        return false;
    }

    File file = SPIFFS.open(filename, "r");
    if (!file) {
        DebugLogger::error("Failed to open bitmap file: %s", filename);
        return false;
    }

    size = file.size() / 3; // Assuming 3 bytes per pixel (RGB)
    if (bitmap != nullptr) {
        delete[] bitmap;
    }
    bitmap = new uint8_t[file.size()];

    if (file.read(bitmap, file.size()) != file.size()) {
        delete[] bitmap;
        bitmap = nullptr;
        size = 0;
        file.close();
        DebugLogger::error("Failed to read bitmap file: %s", filename);
        return false;
    }

    file.close();
    DebugLogger::info("Bitmap file loaded successfully: %s", filename);
    return true;
}
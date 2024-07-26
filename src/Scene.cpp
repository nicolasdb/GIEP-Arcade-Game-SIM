#include "Scene.h"

Scene::Scene(const MatrixConfig& config) : matrixConfig(config), width(config.getWidth()), height(config.getHeight()) {
    initializePixelMap();
}

void Scene::loadBitmap(const uint32_t* bitmap, uint8_t bitmapWidth, uint8_t bitmapHeight) {
    if (bitmapWidth != width || bitmapHeight != height) {
        DebugLogger::error("Bitmap dimensions do not match matrix dimensions");
        return;
    }

    for (uint16_t i = 0; i < width * height; i++) {
        switch (bitmap[i]) {
            case 0xFFFFFF: pixelMap[i] = PixelType::ACTIVE; break;
            case 0x000000: pixelMap[i] = PixelType::BUILDING; break;
            case 0xFFFF00: pixelMap[i] = PixelType::STREET_LIGHT; break;
            case 0x00FF00: pixelMap[i] = PixelType::INTERACTIVE; break;
            default: pixelMap[i] = PixelType::ACTIVE; break;
        }
    }
    DebugLogger::info("Bitmap loaded successfully");
}

PixelType Scene::getPixelType(uint8_t x, uint8_t y) const {
    if (x >= width || y >= height) {
        DebugLogger::error("Invalid coordinates: (%u, %u)", static_cast<unsigned int>(x), static_cast<unsigned int>(y));
        return PixelType::ACTIVE;
    }
    return pixelMap[y * width + x];
}

void Scene::setPixelType(uint8_t x, uint8_t y, PixelType type) {
    if (x >= width || y >= height) {
        DebugLogger::error("Invalid coordinates: (%u, %u)", static_cast<unsigned int>(x), static_cast<unsigned int>(y));
        return;
    }
    pixelMap[y * width + x] = type;
}

void Scene::draw(CRGB* leds) const {
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t index = matrixConfig.XY(x, y);
            leds[index] = getColorForPixelType(pixelMap[y * width + x]);
        }
    }
}

void Scene::initializePixelMap() {
    pixelMap = new PixelType[width * height];
    for (uint16_t i = 0; i < width * height; i++) {
        pixelMap[i] = PixelType::ACTIVE;
    }
    DebugLogger::info("PixelMap initialized: %ux%u", static_cast<unsigned int>(width), static_cast<unsigned int>(height));
}

void Scene::cleanupPixelMap() {
    delete[] pixelMap;
    DebugLogger::info("PixelMap cleaned up");
}

uint32_t Scene::getColorForPixelType(PixelType type) const {
    switch (type) {
        case PixelType::ACTIVE: return 0xFFFFFF;
        case PixelType::BUILDING: return 0x000000;
        case PixelType::STREET_LIGHT: return 0xFFFF00;
        case PixelType::INTERACTIVE: return 0x00FF00;
        default: return 0xFFFFFF;
    }
}
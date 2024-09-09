#include "RainSystem.h"
#include <algorithm>

RainSystem::RainSystem(const MatrixConfig& config)
    : matrixConfig(config), width(config.getWidth()), height(config.getHeight()),
      intensity(0), isVisible(true), mode(RainMode::NORMAL) {
    initializeRain();
}

RainSystem::~RainSystem() {
    cleanupRain();
}

void RainSystem::initializeRain() {
    rainDrops = new RainDrop[width];
    for (uint8_t x = 0; x < width; x++) {
        rainDrops[x] = {height, 0}; // Start with no raindrops and no trail
    }
}

void RainSystem::cleanupRain() {
    delete[] rainDrops;
}

void RainSystem::update(const bool* buildingMap) {
    if (!isVisible) {
        for (uint8_t x = 0; x < width; x++) {
            rainDrops[x] = {height, 0};
        }
        return;
    }

    float dropChance = intensity;
    int8_t windOffset = 0;
    uint8_t maxTrailLength = RAIN_MAX_TRAIL_LENGTH;

    switch (mode) {
        case RainMode::HEAVY:
            dropChance *= RAIN_HEAVY_MULTIPLIER;
            maxTrailLength = RAIN_MAX_TRAIL_LENGTH * 2;
            break;
        case RainMode::STORM:
            dropChance *= RAIN_STORM_MULTIPLIER;
            maxTrailLength = RAIN_MAX_TRAIL_LENGTH * 3;
            windOffset = random8(3) - 1; // -1, 0, or 1
            break;
        default:
            break;
    }

    for (uint8_t x = 0; x < width; x++) {
        if (rainDrops[x].y < height) {
            uint16_t nextIndex = (rainDrops[x].y + 1) * width + x;
            if (rainDrops[x].y + 1 < height && buildingMap[nextIndex]) {
                // If the next position is a building, make the raindrop disappear
                rainDrops[x] = {height, 0};
            } else {
                rainDrops[x].y++;
                rainDrops[x].trailLength = std::min<uint8_t>(rainDrops[x].trailLength + 1, maxTrailLength);

                if (mode == RainMode::STORM && random8() < RAIN_STORM_WIND_CHANCE) {
                    int8_t newX = (x + windOffset + width) % width;
                    if (rainDrops[newX].y >= height) {
                        std::swap(rainDrops[x], rainDrops[newX]);
                    }
                }
            }
        }
        
        if (rainDrops[x].y >= height && random8() < dropChance * 255) {
            rainDrops[x] = {0, 0}; // New raindrop at the top with no trail
        }
    }
}

void RainSystem::draw(CRGB* leds) const {
    if (!isVisible) return;

    uint8_t rainBrightness = RAIN_BRIGHTNESS;
    switch (mode) {
        case RainMode::HEAVY:
            rainBrightness = RAIN_BRIGHTNESS * 1.5;
            break;
        case RainMode::STORM:
            rainBrightness = RAIN_BRIGHTNESS * 2;
            break;
        default:
            break;
    }

    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height; y++) {
            uint16_t index = matrixConfig.XY(x, y);
            if (rainDrops[x].y == y) {
                leds[index] = blend(leds[index], CRGB(0, 0, rainBrightness), 128);
            } else if (y < rainDrops[x].y && y >= rainDrops[x].y - rainDrops[x].trailLength) {
                uint8_t trailBrightness = map(rainDrops[x].y - y, 0, rainDrops[x].trailLength, rainBrightness, 0);
                leds[index] = blend(leds[index], CRGB(0, 0, trailBrightness), 64);
            }
        }
    }
}

void RainSystem::setIntensity(float newIntensity) {
    intensity = newIntensity;
}

float RainSystem::getIntensity() const {
    return intensity;
}

void RainSystem::setVisible(bool visible) {
    isVisible = visible;
}

void RainSystem::setMode(RainMode newMode) {
    mode = newMode;
}
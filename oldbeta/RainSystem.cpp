#include "RainSystem.h"
#include <algorithm>

RainSystem::RainSystem(const MatrixConfig& config)
    : matrixConfig(config), width(config.getWidth()), height(config.getHeight()),
      intensity(0), isVisible(true), mode(RainMode::NORMAL), windDirection(0) {
    initializeRain();
}

RainSystem::~RainSystem() {
    cleanupRain();
}

void RainSystem::initializeRain() {
    rainDrops = new RainDrop[width];
    for (uint8_t x = 0; x < width; x++) {
        rainDrops[x] = {height, 0, static_cast<int8_t>(x)}; // Start with no raindrops and no trail, set initial x
    }
}

void RainSystem::cleanupRain() {
    delete[] rainDrops;
}

void RainSystem::update(const bool* buildingMap) {
    if (!isVisible) {
        for (uint8_t x = 0; x < width; x++) {
            rainDrops[x] = {height, 0, static_cast<int8_t>(x)};
        }
        return;
    }

    float dropChance = intensity;
    int8_t windOffset = 0;
    uint8_t maxTrailLength = RAIN_MAX_TRAIL_LENGTH;

    switch (mode) {
        case RainMode::HEAVY:
            dropChance *= RAIN_HEAVY_MULTIPLIER * 1.5;  // Increased multiplier
            maxTrailLength = RAIN_MAX_TRAIL_LENGTH * 2;
            break;
        case RainMode::STORM:
            dropChance *= RAIN_STORM_MULTIPLIER * 2;  // Increased multiplier
            maxTrailLength = RAIN_MAX_TRAIL_LENGTH * 3;
            // Update wind direction every 50 frames (adjust as needed)
            if (random8() < 5) { // 5/256 chance to change wind direction
                windDirection = random8(3) - 1; // -1, 0, or 1
            }
            windOffset = windDirection * 2; // Amplify wind effect
            break;
        default:
            break;
    }

    for (uint8_t x = 0; x < width; x++) {
        if (rainDrops[x].y < height) {
            uint16_t nextIndex = (rainDrops[x].y + 1) * width + rainDrops[x].x;
            if (rainDrops[x].y + 1 < height && buildingMap[nextIndex]) {
                // If the next position is a building, make the raindrop disappear
                rainDrops[x] = {height, 0, static_cast<int8_t>(x)};
            } else {
                rainDrops[x].y++;
                rainDrops[x].trailLength = std::min<uint8_t>(rainDrops[x].trailLength + 1, maxTrailLength);

                if (mode == RainMode::STORM) {
                    // Apply wind effect to all raindrops in STORM mode
                    int8_t newX = (rainDrops[x].x + windOffset + width) % width;
                    rainDrops[x].x = newX;
                }
            }
        }
        
        if (rainDrops[x].y >= height && random8() < dropChance * 255) {
            rainDrops[x] = {0, 0, static_cast<int8_t>(x)}; // New raindrop at the top with no trail, storing original x position
        }
    }
}

void RainSystem::draw(CRGB* leds) const {
    if (!isVisible) return;

    uint8_t rainBrightness = std::min(static_cast<uint8_t>(RAIN_BRIGHTNESS), static_cast<uint8_t>(255));

    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height; y++) {
            uint16_t index = matrixConfig.XY(rainDrops[x].x, y); // Use the potentially wind-affected x position
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
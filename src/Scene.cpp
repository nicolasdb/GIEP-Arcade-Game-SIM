#include "Scene.h"
#include "config.h"

Scene::Scene(const MatrixConfig& config) : matrixConfig(config), width(config.getWidth()), height(config.getHeight()), 
    rainIntensity(0), isRainVisible(true), sewerLevel(0), basinLevel(0), basinGateActive(false) {
    initializePixelMap();
    initializeRain();
    memset(giepStates, 0, sizeof(giepStates));
}

Scene::~Scene() {
    cleanupPixelMap();
    cleanupRain();
}

void Scene::loadBitmap(const uint32_t* bitmap, uint8_t bitmapWidth, uint8_t bitmapHeight) {
    if (bitmapWidth != width || bitmapHeight != height) {
        DebugLogger::error("Bitmap dimensions do not match matrix dimensions");
        return;
    }

    for (uint16_t i = 0; i < width * height; i++) {
        uint32_t color = bitmap[i];
        switch (color) {
            case COLOR_WHITE:
                pixelMap[i] = PixelType::ACTIVE;
                break;
            case COLOR_BLACK:
                pixelMap[i] = PixelType::BUILDING;
                break;
            case COLOR_YELLOW:
                pixelMap[i] = PixelType::SEWER;
                break;
            case COLOR_BLUE:
                pixelMap[i] = PixelType::BASIN;
                break;
            case COLOR_GREEN_1:
                pixelMap[i] = PixelType::GIEP_1;
                break;
            case COLOR_GREEN_2:
                pixelMap[i] = PixelType::GIEP_2;
                break;
            case COLOR_GREEN_3:
                pixelMap[i] = PixelType::GIEP_3;
                break;
            case COLOR_GREEN_4:
                pixelMap[i] = PixelType::GIEP_4;
                break;
            case COLOR_GREEN_5:
                pixelMap[i] = PixelType::GIEP_5;
                break;
            case COLOR_GREEN_6:
                pixelMap[i] = PixelType::GIEP_6;
                break;
            case COLOR_GREEN_7:
                pixelMap[i] = PixelType::GIEP_7;
                break;
            case COLOR_GREEN_8:
                pixelMap[i] = PixelType::GIEP_8;
                break;
            case COLOR_RED:
                pixelMap[i] = PixelType::BASIN_GATE;
                break;
            case COLOR_MAGENTA:
                pixelMap[i] = PixelType::BASIN_OVERFLOW;
                break;
            case COLOR_PURPLE:
                pixelMap[i] = PixelType::RIVER;
                break;
            default:
                pixelMap[i] = PixelType::ACTIVE;
                break;
        }
    }
    DebugLogger::info("Bitmap loaded successfully");
}

void Scene::loadDefaultScene() {
    loadBitmap(DEFAULT_BITMAP, width, height);
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

void Scene::update() {
    updateRain();
}

void Scene::draw(CRGB* leds) const {
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t index = matrixConfig.XY(x, y);
            PixelType pixelType = pixelMap[y * width + x];
            CRGB pixelColor = getColorForPixelType(pixelType);
            
            // If there's a raindrop here and rain is visible, blend with blue
            if (isRainVisible && rainDrops[x] == y) {
                pixelColor = blend(pixelColor, CRGB(0, 0, RAIN_BRIGHTNESS), 128);
            }
            
            // Handle basin gate specifically
            if (pixelType == PixelType::BASIN_GATE) {
                pixelColor = basinGateActive ? CRGB(BASIN_GATE_BRIGHTNESS, 0, 0) : CRGB(BASIN_GATE_INACTIVE_BRIGHTNESS, 0, 0);
                DebugLogger::debug("Basin Gate Pixel at (%d, %d): Active=%d, Color=(R:%d, G:%d, B:%d)", 
                    x, y, basinGateActive, pixelColor.r, pixelColor.g, pixelColor.b);
            }
            
            leds[index] = pixelColor;
        }
    }

    // Draw sewer level
    drawWaterLevel(leds, SEWER_START_X, SEWER_START_Y, SEWER_WIDTH, SEWER_HEIGHT, sewerLevel, 
                   CRGB(SEWER_BRIGHTNESS, SEWER_BRIGHTNESS, 0), CRGB(SEWER_EMPTY_BRIGHTNESS, SEWER_EMPTY_BRIGHTNESS, 0));

    // Draw basin level
    drawWaterLevel(leds, BASIN_START_X, BASIN_START_Y, BASIN_WIDTH, BASIN_HEIGHT, basinLevel, 
                   CRGB(0, 0, BASIN_BRIGHTNESS), CRGB(0, 0, BASIN_EMPTY_BRIGHTNESS));
    
    DebugLogger::debug("Basin Gate Active: %d", basinGateActive);
}

void Scene::setGIEPState(uint8_t giepIndex, bool state) {
    if (giepIndex < 8) {
        giepStates[giepIndex] = state;
    }
}

void Scene::setBasinGateState(bool state) {
    basinGateActive = state;
    DebugLogger::info("Basin Gate State set to: %d", state);
}

void Scene::setSewerLevel(float level) {
    sewerLevel = constrain(level, 0, 1);
}

void Scene::setBasinLevel(float level) {
    basinLevel = constrain(level, 0, 1);
}

void Scene::drawWaterLevel(CRGB* leds, uint8_t startX, uint8_t startY, uint8_t width, uint8_t height, float level, CRGB fullColor, CRGB emptyColor) const {
    uint8_t filledPixels = round(level * height);

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t index = matrixConfig.XY(startX + x, startY + (height - 1 - y));
            if (y < filledPixels) {
                leds[index] = fullColor;
            } else {
                leds[index] = emptyColor;
            }
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

void Scene::initializeRain() {
    rainDrops = new uint8_t[width];
    for (uint8_t x = 0; x < width; x++) {
        rainDrops[x] = height; // Start with no raindrops
    }
}

void Scene::setRainIntensity(float intensity) {
    rainIntensity = intensity;
}

float Scene::getRainIntensity() const {
    return rainIntensity;
}

void Scene::cleanupRain() {
    delete[] rainDrops;
}

void Scene::setRainVisible(bool visible) {
    isRainVisible = visible;
}

void Scene::updateRain() {
    if (!isRainVisible) {
        // Clear all raindrops when rain is not visible
        for (uint8_t x = 0; x < width; x++) {
            rainDrops[x] = height;
        }
        return;
    }

    for (uint8_t x = 0; x < width; x++) {
        // Move existing raindrops down
        if (rainDrops[x] < height) {
            rainDrops[x]++;
        }
        
        // Randomly create new raindrops
        if (rainDrops[x] >= height && random8() < rainIntensity * 255) {
            rainDrops[x] = 0;
        }
        
        // Check if raindrop has hit a building
        if (rainDrops[x] < height && getPixelType(x, rainDrops[x]) == PixelType::BUILDING) {
            rainDrops[x] = height; // Remove raindrop
        }
    }
}

CRGB Scene::getColorForPixelType(PixelType type) const {
    switch (type) {
        case PixelType::ACTIVE:
            return CRGB(ACTIVE_BRIGHTNESS, ACTIVE_BRIGHTNESS, ACTIVE_BRIGHTNESS);  // Dim white for active areas
        case PixelType::BUILDING:
            return CRGB::Black;
        case PixelType::SEWER:
        case PixelType::BASIN:
            return CRGB::Black;  // These are now handled in the draw method
        case PixelType::GIEP_1:
        case PixelType::GIEP_2:
        case PixelType::GIEP_3:
        case PixelType::GIEP_4:
        case PixelType::GIEP_5:
        case PixelType::GIEP_6:
        case PixelType::GIEP_7:
        case PixelType::GIEP_8: {
            int index = static_cast<int>(type) - static_cast<int>(PixelType::GIEP_1);
            return giepStates[index] ? CRGB(0, GIEP_ACTIVE_BRIGHTNESS, 0) : CRGB(0, GIEP_INACTIVE_BRIGHTNESS, 0);
        }
        case PixelType::BASIN_GATE:
            return basinGateActive ? CRGB(BASIN_GATE_BRIGHTNESS, 0, 0) : CRGB(BASIN_GATE_INACTIVE_BRIGHTNESS, 0, 0);
        case PixelType::BASIN_OVERFLOW:
            return CRGB(BASIN_OVERFLOW_BRIGHTNESS, 0, BASIN_OVERFLOW_BRIGHTNESS);
        case PixelType::RIVER:
            return CRGB(RIVER_BRIGHTNESS, 0, RIVER_BRIGHTNESS);
        default:
            return CRGB::Black;
    }
}
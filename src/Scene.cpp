#include "Scene.h"

Scene::Scene(const MatrixConfig& config) : matrixConfig(config), width(config.getWidth()), height(config.getHeight()), sewerLevel(0), basinLevel(0) {
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
        if ((color & 0x00FF00) == 0x00FF00) {
            // It's a shade of green, determine which GIEP group it belongs to
            uint8_t shade = (color >> 16) & 0xFF;  // Extract the red component
            if (shade >= 0 && shade <= 7) {  // Changed to 0-7 for 8 GIEP types
                pixelMap[i] = static_cast<PixelType>(static_cast<int>(PixelType::GIEP_1) + shade);
            } else {
                pixelMap[i] = PixelType::ACTIVE;  // Default to ACTIVE if it's an unknown shade
            }
        } else {
           switch (color) {
                case 0xFFFFFF: pixelMap[i] = PixelType::ACTIVE; break;
                case 0x000000: pixelMap[i] = PixelType::BUILDING; break;
                case 0xFFFF00: pixelMap[i] = PixelType::SEWER; break;
                case 0x0000FF: pixelMap[i] = PixelType::BASIN; break;
                case 0xFF0000: pixelMap[i] = PixelType::RESERVED_1; break;
                case 0xFF00FF: pixelMap[i] = PixelType::RESERVED_2; break;
                default: pixelMap[i] = PixelType::ACTIVE; break;
            }
        }
    }
    DebugLogger::info("Bitmap loaded successfully");
}


void Scene::loadDefaultScene() {
    // Define your default bitmap here
    const uint32_t DEFAULT_BITMAP[64] = {
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
        0xFFFFFF, 0xFFFFFF, 0x00FF00, 0xFFFFFF, 0x01FF00, 0xFFFFFF, 0x02FF00, 0xFFFFFF,
        0x03FF00, 0xFFFFFF, 0x00FF00, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000,
        0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0xFFFF00, 0x0000FF, 0x000000,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0xFFFF00, 0x0000FF, 0x000000,
        0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0xFFFF00, 0x0000FF, 0x000000,
        0xFFFFFF, 0x04FF00, 0xFFFFFF, 0xFFFFFF, 0x000000, 0xFFFF00, 0x0000FF, 0x000000
    };
    loadBitmap(DEFAULT_BITMAP, width, height);
}

void Scene::setGIEPState(uint8_t giepIndex, bool state) {
    if (giepIndex < 8) {
        giepStates[giepIndex] = state;
    }
}

void Scene::setSewerLevel(float level) {
    sewerLevel = constrain(level, 0, 1);
}

void Scene::setBasinLevel(float level) {
    basinLevel = constrain(level, 0, 1);
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
            CRGB pixelColor = getColorForPixelType(pixelMap[y * width + x]);
            
            // If there's a raindrop here, blend with blue
            if (rainDrops[x] == y) {
                pixelColor = blend(pixelColor, CRGB::Blue, 128);
            }
            
            leds[index] = pixelColor;
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

void Scene::initializeRain() {
    rainDrops = new uint8_t[width];
    for (uint8_t x = 0; x < width; x++) {
        rainDrops[x] = height; // Start with no raindrops
    }
}

void Scene::cleanupPixelMap() {
    delete[] pixelMap;
    DebugLogger::info("PixelMap cleaned up");
}

void Scene::cleanupRain() {
    delete[] rainDrops;
}

void Scene::updateRain() {
    for (uint8_t x = 0; x < width; x++) {
        // Move existing raindrops down
        if (rainDrops[x] < height) {
            rainDrops[x]++;
        }
        
        // Randomly create new raindrops
        if (rainDrops[x] >= height && random8() < 20) { // 20/255 chance of new raindrop
            rainDrops[x] = 0;
        }
        
        // Check if raindrop has hit a building
        if (rainDrops[x] < height && getPixelType(x, rainDrops[x]) == PixelType::BUILDING) {
            rainDrops[x] = height; // Remove raindrop
        }
    }
}

// Update the getColorForPixelType method to use the interactiveGroupState
CRGB Scene::getColorForPixelType(PixelType type) const {
    switch (type) {
        case PixelType::ACTIVE:
            return CRGB(10, 10, 10);  // Dim white for active areas
        case PixelType::BUILDING:
            return CRGB::Black;
        case PixelType::SEWER:
            return CRGB(255 * sewerLevel, 255 * sewerLevel, 0);  // Yellow, intensity based on level
        case PixelType::BASIN:
            return CRGB(0, 0, 255 * basinLevel);  // Blue, intensity based on level
        case PixelType::GIEP_1:
        case PixelType::GIEP_2:
        case PixelType::GIEP_3:
        case PixelType::GIEP_4:
        case PixelType::GIEP_5:
        case PixelType::GIEP_6:
        case PixelType::GIEP_7:
        case PixelType::GIEP_8: {
            int index = static_cast<int>(type) - static_cast<int>(PixelType::GIEP_1);
            return giepStates[index] ? CRGB(0, 255, 0) : CRGB(0, 32, 0);  // Bright green (0x00FF00) when active, dim green when inactive
        }
        case PixelType::RESERVED_1:
            return CRGB::Red;
        case PixelType::RESERVED_2:
            return CRGB::Magenta;
        default:
            return CRGB::Black;
    }
}


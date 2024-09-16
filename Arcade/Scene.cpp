#include "Scene.h"
#include "config.h"
#include <stack>

Scene::Scene(const MatrixConfig& config)
    : matrixConfig(config), width(config.getWidth()), height(config.getHeight()),
      sewerLevel(0), basinLevel(0), basinGateActive(false), isBasinOverflow(false),
      riverFlowOffset(0), isPolluted(false), rainSystem(config) {
    initializePixelMap();
    initializeBuildingMap();
    memset(giepStates, 0, sizeof(giepStates));
}

Scene::~Scene() {
    cleanupPixelMap();
    cleanupBuildingMap();
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
                buildingMap[i] = false;
                break;
            case COLOR_BLACK:
                pixelMap[i] = PixelType::BUILDING;
                buildingMap[i] = true;
                break;
            case COLOR_YELLOW:
                pixelMap[i] = PixelType::SEWER;
                buildingMap[i] = false;
                break;
            case COLOR_BLUE:
                pixelMap[i] = PixelType::BASIN;
                buildingMap[i] = false;
                break;
            case COLOR_GREEN_1:
                pixelMap[i] = PixelType::GIEP_1;
                buildingMap[i] = false;
                break;
            case COLOR_GREEN_2:
                pixelMap[i] = PixelType::GIEP_2;
                buildingMap[i] = false;
                break;
            case COLOR_GREEN_3:
                pixelMap[i] = PixelType::GIEP_3;
                buildingMap[i] = false;
                break;
            case COLOR_GREEN_4:
                pixelMap[i] = PixelType::GIEP_4;
                buildingMap[i] = false;
                break;
            case COLOR_GREEN_5:
                pixelMap[i] = PixelType::GIEP_5;
                buildingMap[i] = false;
                break;
            case COLOR_GREEN_6:
                pixelMap[i] = PixelType::GIEP_6;
                buildingMap[i] = false;
                break;
            case COLOR_GREEN_7:
                pixelMap[i] = PixelType::GIEP_7;
                buildingMap[i] = false;
                break;
            case COLOR_GREEN_8:
                pixelMap[i] = PixelType::GIEP_8;
                buildingMap[i] = false;
                break;
            case COLOR_RED:
                pixelMap[i] = PixelType::BASIN_GATE;
                buildingMap[i] = false;
                break;
            case COLOR_MAGENTA:
                pixelMap[i] = PixelType::BASIN_OVERFLOW;
                buildingMap[i] = false;
                break;
            case COLOR_PURPLE:
                pixelMap[i] = PixelType::RIVER;
                buildingMap[i] = false;
                break;
            default:
                pixelMap[i] = PixelType::ACTIVE;
                buildingMap[i] = false;
                break;
        }
    }
    detectShapes();
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
    buildingMap[y * width + x] = (type == PixelType::BUILDING);
}

void Scene::update() {
    rainSystem.update(buildingMap);
    updateOverflowState();
    updateRiverFlow();
}

void Scene::draw(CRGB* leds) const {
    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint16_t index = matrixConfig.XY(x, y);
            PixelType pixelType = pixelMap[y * width + x];
            leds[index] = getColorForPixelType(pixelType);
        }
    }

    rainSystem.draw(leds);

    // Draw sewer level
    drawWaterLevel(leds, sewerShape, sewerLevel, CRGB(SEWER_BRIGHTNESS, SEWER_BRIGHTNESS, 0), CRGB(SEWER_EMPTY_BRIGHTNESS, SEWER_EMPTY_BRIGHTNESS, 0));

    // Draw basin level
    drawWaterLevel(leds, basinShape, basinLevel, CRGB(0, 0, BASIN_BRIGHTNESS), CRGB(0, 0, BASIN_EMPTY_BRIGHTNESS));
    
    // Draw basin gate
    for (const auto& point : basinGateShape) {
        uint16_t index = matrixConfig.XY(point.x, point.y);
        leds[index] = basinGateActive ? CRGB(BASIN_GATE_BRIGHTNESS, 0, 0) : CRGB(BASIN_GATE_INACTIVE_BRIGHTNESS, 0, 0);
    }

    // Draw basin overflow and river
    if (isBasinOverflow) {
        for (const auto& point : basinOverflowShape) {
            uint16_t index = matrixConfig.XY(point.x, point.y);
            leds[index] = CRGB(BASIN_OVERFLOW_BRIGHTNESS, 0, BASIN_OVERFLOW_BRIGHTNESS);
        }
    }

    // Draw river with flowing effect
    drawRiver(leds);
    
    DebugLogger::debug("Basin Gate Active: %d, Basin Overflow: %d", basinGateActive, isBasinOverflow);
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

void Scene::drawWaterLevel(CRGB* leds, const std::vector<Point>& shape, float level, CRGB fullColor, CRGB emptyColor) const {
    if (shape.empty()) return;

    uint8_t minY = height;
    uint8_t maxY = 0;
    for (const auto& point : shape) {
        if (point.y < minY) minY = point.y;
        if (point.y > maxY) maxY = point.y;
    }

    uint8_t totalHeight = maxY - minY + 1;
    uint8_t filledPixels = round(level * totalHeight);

    for (const auto& point : shape) {
        uint16_t index = matrixConfig.XY(point.x, point.y);
        if (point.y >= maxY - filledPixels) {
            leds[index] = fullColor;
        } else {
            leds[index] = emptyColor;
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

void Scene::initializeBuildingMap() {
    buildingMap = new bool[width * height];
    for (uint16_t i = 0; i < width * height; i++) {
        buildingMap[i] = false;
    }
    DebugLogger::info("BuildingMap initialized: %ux%u", static_cast<unsigned int>(width), static_cast<unsigned int>(height));
}

void Scene::cleanupPixelMap() {
    delete[] pixelMap;
    DebugLogger::info("PixelMap cleaned up");
}

void Scene::cleanupBuildingMap() {
    delete[] buildingMap;
    DebugLogger::info("BuildingMap cleaned up");
}

void Scene::setRainIntensity(float intensity) {
    rainSystem.setIntensity(intensity);
}

float Scene::getRainIntensity() const {
    return rainSystem.getIntensity();
}

void Scene::setRainVisible(bool visible) {
    rainSystem.setVisible(visible);
}

void Scene::setRainMode(RainMode mode) {
    rainSystem.setMode(mode);
}

const bool* Scene::getBuildingMap() const {
    return buildingMap;
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
        case PixelType::RIVER:
            return CRGB::Black;  // These are now handled in the draw method
        default:
            return CRGB::Black;
    }
}

void Scene::detectShapes() {
    sewerShape.clear();
    basinShape.clear();
    basinGateShape.clear();
    basinOverflowShape.clear();
    riverShape.clear();

    std::vector<bool> visited(width * height, false);

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            if (!visited[y * width + x]) {
                PixelType type = getPixelType(x, y);
                switch (type) {
                    case PixelType::SEWER:
                        floodFill(x, y, PixelType::SEWER, sewerShape, visited);
                        break;
                    case PixelType::BASIN:
                        floodFill(x, y, PixelType::BASIN, basinShape, visited);
                        break;
                    case PixelType::BASIN_GATE:
                        floodFill(x, y, PixelType::BASIN_GATE, basinGateShape, visited);
                        break;
                    case PixelType::BASIN_OVERFLOW:
                        floodFill(x, y, PixelType::BASIN_OVERFLOW, basinOverflowShape, visited);
                        break;
                    case PixelType::RIVER:
                        floodFill(x, y, PixelType::RIVER, riverShape, visited);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    DebugLogger::info("Shapes detected: Sewer(%d), Basin(%d), Basin Gate(%d), Basin Overflow(%d), River(%d)",
                      sewerShape.size(), basinShape.size(), basinGateShape.size(), basinOverflowShape.size(), riverShape.size());
}

void Scene::floodFill(uint8_t startX, uint8_t startY, PixelType targetType, std::vector<Point>& shape, std::vector<bool>& visited) {
    std::stack<Point> stack;
    stack.push(Point(startX, startY));

    while (!stack.empty()) {
        Point p = stack.top();
        stack.pop();

        if (p.x >= width || p.y >= height || visited[p.y * width + p.x] || getPixelType(p.x, p.y) != targetType) {
            continue;
        }

        shape.emplace_back(p.x, p.y);
        visited[p.y * width + p.x] = true;

        stack.push(Point(p.x + 1, p.y));
        stack.push(Point(p.x - 1, p.y));
        stack.push(Point(p.x, p.y + 1));
        stack.push(Point(p.x, p.y - 1));
    }
}

void Scene::updateOverflowState() {
    isBasinOverflow = (basinLevel >= 1.0f);
}

CRGB Scene::getSewerColor() const {
    return CRGB(SEWER_BRIGHTNESS, SEWER_BRIGHTNESS, 0);
}

void Scene::updateRiverFlow() {
    riverFlowOffset = (riverFlowOffset + 1); // Changed from 16 to 20 steps for a complete cycle
}

void Scene::drawRiver(CRGB* leds) const {
    if (riverShape.empty()) return;

    uint8_t minY = height;
    uint8_t maxY = 0;
    for (const auto& point : riverShape) {
        if (point.y < minY) minY = point.y;
        if (point.y > maxY) maxY = point.y;
    }

    uint8_t totalHeight = maxY - minY + 1;
    uint8_t animatedLevels = std::min(totalHeight, static_cast<uint8_t>(3)); // Animate only the first 3 levels

    for (const auto& point : riverShape) {
        uint16_t index = matrixConfig.XY(point.x, point.y);
        if (isPolluted) {
            leds[index] = CRGB(RIVER_BRIGHTNESS, 0, RIVER_BRIGHTNESS); // Magenta for pollution
        } else if (point.y >= maxY - animatedLevels + 1) { // Animate the bottom 3 levels (or less if river is smaller)
            // Create a flowing effect from left to right
            // CHANGED: Adjusted the multiplier to control flow speed
            uint8_t brightness = sin8((width - point.x) * 25 + riverFlowOffset * 5);
            brightness = map(brightness, 0, 255, 70, 255); // Adjust the range of blue shades
            leds[index] = CRGB(0, 0, brightness); // Only blue shades
        } else {
            // Use a dimmer blue color for the upper part of the river
            leds[index] = CRGB(0, 0, 20); // Dimmer blue for non-animated part
        }
    }
}

void Scene::setPollutionState(bool polluted) {
    isPolluted = polluted;
}
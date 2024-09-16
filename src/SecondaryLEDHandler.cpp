#include "SecondaryLEDHandler.h"
#include "DebugLogger.h"

SecondaryLEDHandler::SecondaryLEDHandler() : endGameState(SecondaryLEDZone::NONE), rainLevel(0), lastBlinkTime(0) {
    memset(zoneStates, 0, sizeof(zoneStates));
    DebugLogger::debug("SecondaryLEDHandler initialized");
}

void SecondaryLEDHandler::begin() {
    FastLED.addLeds<WS2813, SECONDARY_LED_PIN, GRB>(leds, TOTAL_SECONDARY_LEDS);
    FastLED.clear();
    FastLED.show();
    DebugLogger::debug("SecondaryLEDHandler begun");
}

void SecondaryLEDHandler::update() {
    if (endGameState != SecondaryLEDZone::NONE) {
        updateEndGameState();
    } else {
        updateNormalState();
    }
    FastLED.show();
    DebugLogger::debug("SecondaryLEDHandler updated");
}

void SecondaryLEDHandler::setZoneState(SecondaryLEDZone zone, bool state) {
    uint8_t index = getZoneIndexFromBitmap(zone);
    if (index < SECONDARY_NUM_ZONES) {
        zoneStates[index] = state;
        DebugLogger::debug("Zone %d state set to %d", index, state);
    } else {
        DebugLogger::error("Invalid zone index: %d", index);
    }
}

void SecondaryLEDHandler::setRainLevel(uint8_t level) {
    DebugLogger::debug("Setting rain level to %d", level);
    rainLevel = level;
    updateRainLevelIndicators();
}

void SecondaryLEDHandler::setEndGameState(SecondaryLEDZone state) {
    endGameState = state;
    lastBlinkTime = millis();
    DebugLogger::debug("Set end game state to %d, lastBlinkTime: %lu", static_cast<int>(state), lastBlinkTime);
}

void SecondaryLEDHandler::updateNormalState() {
    for (int zone = 0; zone < SECONDARY_NUM_ZONES; zone++) {
        if (zoneStates[zone]) {
            for (int i = 0; i < LEDS_PER_ZONE; i++) {
                leds[zone * LEDS_PER_ZONE + i] = CRGB(GET_SECONDARY_LED_COLOR(zone, i));
            }
            DebugLogger::debug("Zone %d LEDs turned on", zone);
        } else {
            for (int i = 0; i < LEDS_PER_ZONE; i++) {
                leds[zone * LEDS_PER_ZONE + i] = CRGB::Black;
            }
            DebugLogger::debug("Zone %d LEDs turned off", zone);
        }
    }
    updateRainLevelIndicators();
}

void SecondaryLEDHandler::updateEndGameState() {
    unsigned long currentTime = millis();
    bool blinkOn = ((currentTime - lastBlinkTime) / 500) % 2 == 0;  // 0.5-second blink interval
    lastBlinkTime = currentTime;  // Update lastBlinkTime every call

    DebugLogger::debug("Updating end game state. EndGameState: %d, BlinkOn: %d, CurrentTime: %lu, LastBlinkTime: %lu", 
                       static_cast<int>(endGameState), blinkOn, currentTime, lastBlinkTime);

    CRGB endGameColor;
    switch (endGameState) {
        case SecondaryLEDZone::WIN:
            endGameColor = CRGB::Cyan;
            break;
        case SecondaryLEDZone::FLOOD_DEATH:
            endGameColor = CRGB::Blue;
            break;
        case SecondaryLEDZone::POLLUTION_DEATH:
            endGameColor = CRGB::Red;
            break;
        default:
            DebugLogger::error("Invalid end game state: %d", static_cast<int>(endGameState));
            return;
    }

    if (endGameState == SecondaryLEDZone::WIN) {
        // Blink WIN_ZONE and all GIEP zones
        uint8_t winIndex = getZoneIndexFromBitmap(SecondaryLEDZone::WIN);

        for (int zone = 0; zone < SECONDARY_NUM_ZONES; zone++) {
            CRGB zoneColor;
            SecondaryLEDZone currentZone = static_cast<SecondaryLEDZone>(zone);
            if (zone == winIndex) {
                zoneColor = endGameColor;
            } else if (zone < 9 && zone != 4) {  // GIEP zones (0-3, 5-8), excluding basin gate (4)
                zoneColor = CRGB(GET_SECONDARY_LED_COLOR(zone, 0));
            } else {
                continue;  // Skip other zones
            }

            for (int i = 0; i < LEDS_PER_ZONE; i++) {
                leds[zone * LEDS_PER_ZONE + i] = blinkOn ? zoneColor : CRGB::Black;
            }
            DebugLogger::debug("Zone %d (%s) set to %s, Color: (%d, %d, %d)", 
                               zone, getZoneName(currentZone), 
                               blinkOn ? "ON" : "OFF", zoneColor.r, zoneColor.g, zoneColor.b);
        }
    } else {  // FLOOD_DEATH or POLLUTION_DEATH
        for (int zone = 0; zone < SECONDARY_NUM_ZONES; zone++) {
            if (zone != getZoneIndexFromBitmap(SecondaryLEDZone::WIN)) {
                for (int i = 0; i < LEDS_PER_ZONE; i++) {
                    leds[zone * LEDS_PER_ZONE + i] = blinkOn ? endGameColor : CRGB::Black;
                }
            }
        }
    }

    DebugLogger::debug("End game state updated");
}

CRGB SecondaryLEDHandler::getColorForZone(SecondaryLEDZone zone) {
    uint8_t index = getZoneIndexFromBitmap(zone);
    if (index < SECONDARY_NUM_ZONES) {
        CRGB color = CRGB(GET_SECONDARY_LED_COLOR(index, 0));
        DebugLogger::debug("Color for zone %d: (%d, %d, %d)", index, color.r, color.g, color.b);
        return color;
    }
    DebugLogger::error("Invalid zone index: %d", index);
    return CRGB::Black;
}

void SecondaryLEDHandler::updateRainLevelIndicators() {
    DebugLogger::debug("Updating rain level indicators. Current level: %d", rainLevel);
    
    // Turn off all rain level indicators
    for (int i = 0; i < 3; i++) {
        SecondaryLEDZone zone = static_cast<SecondaryLEDZone>(static_cast<int>(SecondaryLEDZone::RAIN_LEVEL_1) + i);
        uint8_t zoneIndex = getZoneIndexFromBitmap(zone);
        for (int j = 0; j < LEDS_PER_ZONE; j++) {
            leds[zoneIndex * LEDS_PER_ZONE + j] = CRGB::Black;
        }
    }

    // Turn on the appropriate rain level indicator based on the current rain level
    for (int i = 0; i < rainLevel; i++) {
        SecondaryLEDZone zone = static_cast<SecondaryLEDZone>(static_cast<int>(SecondaryLEDZone::RAIN_LEVEL_1) + i);
        uint8_t zoneIndex = getZoneIndexFromBitmap(zone);
        for (int j = 0; j < LEDS_PER_ZONE; j++) {
            leds[zoneIndex * LEDS_PER_ZONE + j] = CRGB(GET_SECONDARY_LED_COLOR(zoneIndex, j));
        }
    }

    DebugLogger::debug("Rain level indicators updated");
}

uint8_t SecondaryLEDHandler::getZoneIndexFromBitmap(SecondaryLEDZone zone) {
    // This function maps the SecondaryLEDZone enum to the index in the SECONDARY_LED_BITMAP
    switch (zone) {
        case SecondaryLEDZone::GIEP_1: return 0;
        case SecondaryLEDZone::GIEP_2: return 1;
        case SecondaryLEDZone::GIEP_3: return 2;
        case SecondaryLEDZone::GIEP_4: return 3;
        case SecondaryLEDZone::BASIN_GATE: return 4;
        case SecondaryLEDZone::GIEP_8: return 5;
        case SecondaryLEDZone::GIEP_7: return 6;
        case SecondaryLEDZone::GIEP_6: return 7;
        case SecondaryLEDZone::GIEP_5: return 8;
        case SecondaryLEDZone::WIN: return 9;
        case SecondaryLEDZone::POLLUTION_DEATH: return 10;
        case SecondaryLEDZone::RAIN_LEVEL_3: return 11;
        case SecondaryLEDZone::RAIN_LEVEL_2: return 12;
        case SecondaryLEDZone::RAIN_LEVEL_1: return 13;
        case SecondaryLEDZone::FLOOD_DEATH: return 10; // Same as POLLUTION_DEATH
        default: return SECONDARY_NUM_ZONES; // Invalid index
    }
}

const char* SecondaryLEDHandler::getZoneName(SecondaryLEDZone zone) {
    switch (zone) {
        case SecondaryLEDZone::GIEP_1: return "GIEP_1";
        case SecondaryLEDZone::GIEP_2: return "GIEP_2";
        case SecondaryLEDZone::GIEP_3: return "GIEP_3";
        case SecondaryLEDZone::GIEP_4: return "GIEP_4";
        case SecondaryLEDZone::GIEP_5: return "GIEP_5";
        case SecondaryLEDZone::GIEP_6: return "GIEP_6";
        case SecondaryLEDZone::GIEP_7: return "GIEP_7";
        case SecondaryLEDZone::GIEP_8: return "GIEP_8";
        case SecondaryLEDZone::BASIN_GATE: return "BASIN_GATE";
        case SecondaryLEDZone::WIN: return "WIN";
        case SecondaryLEDZone::POLLUTION_DEATH: return "POLLUTION_DEATH";
        case SecondaryLEDZone::FLOOD_DEATH: return "FLOOD_DEATH";
        case SecondaryLEDZone::RAIN_LEVEL_1: return "RAIN_LEVEL_1";
        case SecondaryLEDZone::RAIN_LEVEL_2: return "RAIN_LEVEL_2";
        case SecondaryLEDZone::RAIN_LEVEL_3: return "RAIN_LEVEL_3";
        default: return "UNKNOWN";
    }
}
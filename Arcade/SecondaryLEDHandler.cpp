#include "SecondaryLEDHandler.h"
#include "DebugLogger.h"

SecondaryLEDHandler::SecondaryLEDHandler() : endGameState(SecondaryLEDZone::GIEP_1), rainLevel(0), isBlinking(false), lastBlinkTime(0) {
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
    updateLEDs();
    updateBlinkEffect();
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

void SecondaryLEDHandler::setZoneState(SecondaryLEDZone zone, bool state, CRGB color) {
    uint8_t index = getZoneIndexFromBitmap(zone);
    if (index < SECONDARY_NUM_ZONES) {
        zoneStates[index] = state;
        for (int i = 0; i < LEDS_PER_ZONE; i++) {
            leds[index * LEDS_PER_ZONE + i] = state ? color : CRGB::Black;
        }
        DebugLogger::debug("Zone %d state set to %d with color (%d, %d, %d)", index, state, color.r, color.g, color.b);
    } else {
        DebugLogger::error("Invalid zone index: %d", index);
    }
}

void SecondaryLEDHandler::setRainLevel(uint8_t level) {
    DebugLogger::debug("Setting rain level to %d", level);
    rainLevel = level;
    updateRainLevelIndicators();
}

void SecondaryLEDHandler::blinkAll(CRGB color) {
    isBlinking = true;
    blinkColor = color;
    DebugLogger::debug("Set all LEDs to blink with color (%d, %d, %d)", color.r, color.g, color.b);
}

void SecondaryLEDHandler::stopBlinking() {
    isBlinking = false;
    DebugLogger::debug("Stopped blinking");
}

void SecondaryLEDHandler::setEndGameState(SecondaryLEDZone state) {
    endGameState = state;
    DebugLogger::debug("Set end game state to %d", static_cast<int>(state));
}

void SecondaryLEDHandler::updateLEDs() {
    if (isBlinking) {
        DebugLogger::debug("Skipping LED update due to blinking");
        return; // Don't update LEDs if we're in blink mode
    }

    // Handle end game state
    if (endGameState == SecondaryLEDZone::FLOOD_DEATH ||
        endGameState == SecondaryLEDZone::POLLUTION_DEATH ||
        endGameState == SecondaryLEDZone::WIN) {
        CRGB endGameColor = getColorForZone(endGameState);
        for (int i = 0; i < TOTAL_SECONDARY_LEDS; i++) {
            leds[i] = endGameColor;
        }
        DebugLogger::debug("Updated LEDs for end game state %d", static_cast<int>(endGameState));
    } else {
        // Update all other zones
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
    }
}

void SecondaryLEDHandler::updateBlinkEffect() {
    if (!isBlinking) {
        return;
    }

    unsigned long currentTime = millis();
    if (currentTime - lastBlinkTime >= BLINK_DURATION) {
        lastBlinkTime = currentTime;
        for (int i = 0; i < TOTAL_SECONDARY_LEDS; i++) {
            leds[i] = (leds[i] == CRGB::Black) ? blinkColor : CRGB::Black;
        }
        DebugLogger::debug("Blink effect updated");
    }
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
    setZoneState(SecondaryLEDZone::RAIN_LEVEL_1, false);
    setZoneState(SecondaryLEDZone::RAIN_LEVEL_2, false);
    setZoneState(SecondaryLEDZone::RAIN_LEVEL_3, false);

    // Turn on the appropriate rain level indicator based on the current rain level
    switch (rainLevel) {
        case 1:
            setZoneState(SecondaryLEDZone::RAIN_LEVEL_1, true);
            DebugLogger::debug("Rain level 1 indicator turned on");
            break;
        case 2:
            setZoneState(SecondaryLEDZone::RAIN_LEVEL_2, true);
            DebugLogger::debug("Rain level 2 indicator turned on");
            break;
        case 3:
            setZoneState(SecondaryLEDZone::RAIN_LEVEL_3, true);
            DebugLogger::debug("Rain level 3 indicator turned on");
            break;
        default:
            DebugLogger::debug("No rain level indicators turned on");
            break;
    }
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
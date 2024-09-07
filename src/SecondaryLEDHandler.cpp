#include "SecondaryLEDHandler.h"

SecondaryLEDHandler::SecondaryLEDHandler() : endGameState(SecondaryLEDZone::GIEP_1), rainLevel(0) {
    memset(zoneStates, 0, sizeof(zoneStates));
}

void SecondaryLEDHandler::begin() {
    FastLED.addLeds<WS2813, SECONDARY_LED_PIN, GRB>(leds, TOTAL_SECONDARY_LEDS);
    FastLED.clear();
    FastLED.show();
}

void SecondaryLEDHandler::update() {
    updateLEDs();
    FastLED.show();
}

void SecondaryLEDHandler::setZoneState(SecondaryLEDZone zone, bool state) {
    uint8_t index = getZoneIndexFromBitmap(zone);
    if (index < SECONDARY_NUM_ZONES) {
        zoneStates[index] = state;
    }
}

void SecondaryLEDHandler::setRainLevel(uint8_t level) {
    rainLevel = level;
    setZoneState(SecondaryLEDZone::RAIN_LEVEL_1, level >= 1);
    setZoneState(SecondaryLEDZone::RAIN_LEVEL_2, level >= 2);
    setZoneState(SecondaryLEDZone::RAIN_LEVEL_3, level >= 3);
}

void SecondaryLEDHandler::setEndGameState(SecondaryLEDZone state) {
    endGameState = state;
}

void SecondaryLEDHandler::updateLEDs() {
    // Handle end game state
    if (endGameState == SecondaryLEDZone::FLOOD_DEATH ||
        endGameState == SecondaryLEDZone::POLLUTION_DEATH ||
        endGameState == SecondaryLEDZone::WIN) {
        CRGB endGameColor = getColorForZone(endGameState);
        for (int i = 0; i < TOTAL_SECONDARY_LEDS; i++) {
            leds[i] = endGameColor;
        }
    } else {
        // Update all other zones
        for (int zone = 0; zone < SECONDARY_NUM_ZONES; zone++) {
            if (zoneStates[zone]) {
                for (int i = 0; i < LEDS_PER_ZONE; i++) {
                    leds[zone * LEDS_PER_ZONE + i] = CRGB(GET_SECONDARY_LED_COLOR(zone, i));
                }
            } else {
                for (int i = 0; i < LEDS_PER_ZONE; i++) {
                    leds[zone * LEDS_PER_ZONE + i] = CRGB::Black;
                }
            }
        }
    }
}

CRGB SecondaryLEDHandler::getColorForZone(SecondaryLEDZone zone) {
    uint8_t index = getZoneIndexFromBitmap(zone);
    if (index < SECONDARY_NUM_ZONES) {
        return CRGB(GET_SECONDARY_LED_COLOR(index, 0));
    }
    return CRGB::Black;
}

void SecondaryLEDHandler::updateRainLevelIndicators() {
    // This method is called by update() and handles the rain level indicators
    for (int i = 0; i < 3; i++) {
        SecondaryLEDZone rainZone = static_cast<SecondaryLEDZone>(static_cast<int>(SecondaryLEDZone::RAIN_LEVEL_1) + i);
        bool isActive = i < rainLevel;
        setZoneState(rainZone, isActive);
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
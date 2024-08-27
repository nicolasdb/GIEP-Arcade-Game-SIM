#include "SecondaryLEDHandler.h"

SecondaryLEDHandler::SecondaryLEDHandler() : endGameState(SecondaryLEDZone::GIEP_1) {
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
    if (static_cast<int>(zone) < SECONDARY_NUM_ZONES) {
        zoneStates[static_cast<int>(zone)] = state;
    }
}

void SecondaryLEDHandler::setRainLevel(uint8_t level) {
    for (int i = 0; i < 3; i++) {
        setZoneState(static_cast<SecondaryLEDZone>(static_cast<int>(SecondaryLEDZone::RAIN_LEVEL_1) + i), i < level);
    }
}

void SecondaryLEDHandler::setEndGameState(SecondaryLEDZone state) {
    endGameState = state;
}

void SecondaryLEDHandler::updateLEDs() {
    for (int zone = 0; zone < SECONDARY_NUM_ZONES; zone++) {
        CRGB color = zoneStates[zone] ? getColorForZone(static_cast<SecondaryLEDZone>(zone)) : CRGB::Black;
        for (int i = 0; i < LEDS_PER_ZONE; i++) {
            leds[zone * LEDS_PER_ZONE + i] = color;
        }
    }

    // Handle end game state
    if (endGameState == SecondaryLEDZone::FLOOD_DEATH ||
        endGameState == SecondaryLEDZone::POLLUTION_DEATH ||
        endGameState == SecondaryLEDZone::WIN) {
        CRGB endGameColor = getColorForZone(endGameState);
        for (int i = 0; i < TOTAL_SECONDARY_LEDS; i++) {
            leds[i] = endGameColor;
        }
    }
}

CRGB SecondaryLEDHandler::getColorForZone(SecondaryLEDZone zone) {
    switch (zone) {
        case SecondaryLEDZone::GIEP_1:
            return CRGB(COLOR_GREEN_1);
        case SecondaryLEDZone::GIEP_2:
            return CRGB(COLOR_GREEN_2);
        case SecondaryLEDZone::GIEP_3:
            return CRGB(COLOR_GREEN_3);
        case SecondaryLEDZone::GIEP_4:
            return CRGB(COLOR_GREEN_4);
        case SecondaryLEDZone::GIEP_5:
            return CRGB(COLOR_GREEN_5);
        case SecondaryLEDZone::GIEP_6:
            return CRGB(COLOR_GREEN_6);
        case SecondaryLEDZone::GIEP_7:
            return CRGB(COLOR_GREEN_7);
        case SecondaryLEDZone::GIEP_8:
            return CRGB(COLOR_GREEN_8);
        case SecondaryLEDZone::BASIN_GATE:
            return BASIN_GATE_COLOR;
        case SecondaryLEDZone::RAIN_LEVEL_1:
            return CRGB::Yellow;
        case SecondaryLEDZone::RAIN_LEVEL_2:
            return CRGB::Orange;
        case SecondaryLEDZone::RAIN_LEVEL_3:
            return CRGB::Red;
        case SecondaryLEDZone::FLOOD_DEATH:
            return SEWER_COLOR;
        case SecondaryLEDZone::POLLUTION_DEATH:
            return RIVER_COLOR;
        case SecondaryLEDZone::WIN:
            return CRGB::Green;
        default:
            return CRGB::Black;
    }
}
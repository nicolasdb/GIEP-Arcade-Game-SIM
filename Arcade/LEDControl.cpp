#include "config.h"

void setFloodDeathZone(CRGB* leds, bool active) {
    if (active) {
        leds[SECONDARY_LED_ZONE_MAPPING[FLOOD_DEATH_ZONE].start_index] = GET_SECONDARY_LED_COLOR(FLOOD_DEATH_ZONE, 0);
        leds[SECONDARY_LED_ZONE_MAPPING[FLOOD_DEATH_ZONE].start_index + 1] = GET_SECONDARY_LED_COLOR(FLOOD_DEATH_ZONE, 1);
    } else {
        leds[SECONDARY_LED_ZONE_MAPPING[FLOOD_DEATH_ZONE].start_index] = CRGB::Black;
        leds[SECONDARY_LED_ZONE_MAPPING[FLOOD_DEATH_ZONE].start_index + 1] = CRGB::Black;
    }
}

void setPollutionDeathZone(CRGB* leds, bool active) {
    if (active) {
        leds[SECONDARY_LED_ZONE_MAPPING[POLLUTION_DEATH_ZONE].start_index] = GET_SECONDARY_LED_COLOR(POLLUTION_DEATH_ZONE, 0);
        leds[SECONDARY_LED_ZONE_MAPPING[POLLUTION_DEATH_ZONE].start_index + 1] = GET_SECONDARY_LED_COLOR(POLLUTION_DEATH_ZONE, 1);
    } else {
        leds[SECONDARY_LED_ZONE_MAPPING[POLLUTION_DEATH_ZONE].start_index] = CRGB::Black;
        leds[SECONDARY_LED_ZONE_MAPPING[POLLUTION_DEATH_ZONE].start_index + 1] = CRGB::Black;
    }
}

void updateDeathZones(CRGB* leds, bool isGameOver, bool isFloodDeath, bool isPollutionDeath) {
    if (isGameOver) {
        // Game over condition: light up all 4 LEDs
        if (isFloodDeath) {
            // Use flood death color for all 4 LEDs
            CRGB floodColor = GET_SECONDARY_LED_COLOR(FLOOD_DEATH_ZONE, 0);
            leds[SECONDARY_LED_ZONE_MAPPING[FLOOD_DEATH_ZONE].start_index] = floodColor;
            leds[SECONDARY_LED_ZONE_MAPPING[FLOOD_DEATH_ZONE].start_index + 1] = floodColor;
            leds[SECONDARY_LED_ZONE_MAPPING[POLLUTION_DEATH_ZONE].start_index] = floodColor;
            leds[SECONDARY_LED_ZONE_MAPPING[POLLUTION_DEATH_ZONE].start_index + 1] = floodColor;
        } else if (isPollutionDeath) {
            // Use pollution death color for all 4 LEDs
            CRGB pollutionColor = GET_SECONDARY_LED_COLOR(POLLUTION_DEATH_ZONE, 0);
            leds[SECONDARY_LED_ZONE_MAPPING[FLOOD_DEATH_ZONE].start_index] = pollutionColor;
            leds[SECONDARY_LED_ZONE_MAPPING[FLOOD_DEATH_ZONE].start_index + 1] = pollutionColor;
            leds[SECONDARY_LED_ZONE_MAPPING[POLLUTION_DEATH_ZONE].start_index] = pollutionColor;
            leds[SECONDARY_LED_ZONE_MAPPING[POLLUTION_DEATH_ZONE].start_index + 1] = pollutionColor;
        } else {
            // If neither flood nor pollution death, turn off all LEDs (shouldn't happen in normal gameplay)
            setFloodDeathZone(leds, false);
            setPollutionDeathZone(leds, false);
        }
    } else {
        // Not game over: update zones separately based on current game state
        setFloodDeathZone(leds, isFloodDeath);
        setPollutionDeathZone(leds, isPollutionDeath);
    }
}
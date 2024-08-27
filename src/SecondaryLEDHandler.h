#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

/**
 * @enum SecondaryLEDZone
 * @brief Represents the different zones in the secondary LED array.
 * 
 * This enum defines the various zones that can be controlled in the secondary LED array.
 * Each zone corresponds to a specific game element or state.
 */
enum class SecondaryLEDZone {
    GIEP_1,          ///< GIEP device 1
    GIEP_2,          ///< GIEP device 2
    GIEP_3,          ///< GIEP device 3
    GIEP_4,          ///< GIEP device 4
    GIEP_5,          ///< GIEP device 5
    GIEP_6,          ///< GIEP device 6
    GIEP_7,          ///< GIEP device 7
    GIEP_8,          ///< GIEP device 8
    BASIN_GATE,      ///< Basin gate status
    RAIN_LEVEL_1,    ///< Rain intensity level 1
    RAIN_LEVEL_2,    ///< Rain intensity level 2
    RAIN_LEVEL_3,    ///< Rain intensity level 3
    FLOOD_DEATH,     ///< End game state: death from flood
    POLLUTION_DEATH, ///< End game state: death from pollution
    WIN              ///< End game state: win
};

/**
 * @class SecondaryLEDHandler
 * @brief Manages the secondary LED array for additional visual feedback.
 * 
 * This class is responsible for controlling the secondary LED array, which provides
 * visual feedback for various game elements and states. It manages the state of
 * different zones and updates the LEDs accordingly.
 */
class SecondaryLEDHandler {
public:
    /**
     * @brief Construct a new SecondaryLEDHandler object.
     */
    SecondaryLEDHandler();

    /**
     * @brief Initialize the secondary LED array.
     */
    void begin();

    /**
     * @brief Update the LED array based on current states.
     * 
     * This method should be called regularly to refresh the LED display.
     */
    void update();

    /**
     * @brief Set the state of a specific zone.
     * 
     * @param zone The zone to set.
     * @param state The state to set the zone to (true for on, false for off).
     */
    void setZoneState(SecondaryLEDZone zone, bool state);

    /**
     * @brief Set the current rain level.
     * 
     * @param level The rain level (0-3).
     */
    void setRainLevel(uint8_t level);

    /**
     * @brief Set the end game state.
     * 
     * @param state The end game state to display.
     */
    void setEndGameState(SecondaryLEDZone state);

private:
    CRGB leds[TOTAL_SECONDARY_LEDS];
    bool zoneStates[SECONDARY_NUM_ZONES];
    SecondaryLEDZone endGameState;

    /**
     * @brief Update the LED colors based on current states.
     */
    void updateLEDs();

    /**
     * @brief Get the color for a specific zone.
     * 
     * @param zone The zone to get the color for.
     * @return CRGB The color for the specified zone.
     */
    CRGB getColorForZone(SecondaryLEDZone zone);
};
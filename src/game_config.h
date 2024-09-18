#pragma once
#include <cstdint>

namespace GameConfig {
    namespace Timing {
        constexpr uint32_t WAITING_RAINING_DURATION = 10000;
        constexpr uint32_t WAITING_DRY_DURATION = 10000;
        constexpr uint32_t RAINING_DURATION = 10000;
        constexpr uint32_t HEAVY_DURATION = 15000;
        constexpr uint32_t STORM_DURATION = 15000;
        constexpr uint32_t END_STATE_DURATION = 5000;
    }

    namespace SewerMechanics {
        constexpr float SEWER_INCREASE_RATE_RAINING = 0.013f;
        constexpr float SEWER_INCREASE_RATE_HEAVY = 0.02f;
        constexpr float SEWER_INCREASE_RATE_STORM = 0.025f;
        constexpr float GIEP_EFFECT_STRENGTH = 0.0045f;
        constexpr float SEWER_DRAIN_RATE = 0.01f;
        constexpr float BASIN_GATE_TRANSFER_RATE = 0.007f;
    }

    namespace GameBalance {
        constexpr float SEWER_OVERFLOW_THRESHOLD = 0.65f;
        constexpr float BASIN_OVERFLOW_THRESHOLD = 0.95f;
        constexpr float WIN_THRESHOLD = 0.5f;
    }

    namespace RainVisuals {
        constexpr float RAIN_INTENSITY_RAINING = 0.005f;
        constexpr float RAIN_INTENSITY_HEAVY = 0.03f;
        constexpr float RAIN_INTENSITY_STORM = 0.10f;
        constexpr uint8_t RAIN_MAX_TRAIL_LENGTH = 4;
        constexpr float RAIN_HEAVY_MULTIPLIER = 10.0f;
        constexpr float RAIN_STORM_MULTIPLIER = 30.0f;
        constexpr uint8_t RAIN_STORM_WIND_CHANCE = 10; // 90% chance (out of 255)
    }

    namespace Brightness {
        constexpr uint8_t GLOBAL_BRIGHTNESS = 30;
        constexpr uint8_t ACTIVE_BRIGHTNESS = 16;
        constexpr uint8_t GIEP_ACTIVE_BRIGHTNESS = 255;
        constexpr uint8_t GIEP_INACTIVE_BRIGHTNESS = 30;
        constexpr uint8_t RAIN_BRIGHTNESS = 190;
        constexpr uint8_t SEWER_BRIGHTNESS = 255;
        constexpr uint8_t SEWER_EMPTY_BRIGHTNESS = ACTIVE_BRIGHTNESS;
        constexpr uint8_t BASIN_BRIGHTNESS = 255;
        constexpr uint8_t BASIN_EMPTY_BRIGHTNESS = 12;
        constexpr uint8_t BASIN_GATE_BRIGHTNESS = 255;
        constexpr uint8_t BASIN_GATE_INACTIVE_BRIGHTNESS = 40;
        constexpr uint8_t BASIN_OVERFLOW_BRIGHTNESS = 255;
        constexpr uint8_t RIVER_BRIGHTNESS = 255;
    }

    namespace Animation {
        constexpr uint32_t BLINK_DURATION = 500; // milliseconds
    }

    namespace TaskConfig {
        constexpr uint32_t BUTTON_TASK_STACK_SIZE = 2048;
        constexpr uint32_t GAME_UPDATE_TASK_STACK_SIZE = 4096;
        constexpr uint32_t LED_UPDATE_TASK_STACK_SIZE = 2048;
        constexpr uint8_t BUTTON_TASK_PRIORITY = 3;
        constexpr uint8_t GAME_UPDATE_TASK_PRIORITY = 2;
        constexpr uint8_t LED_UPDATE_TASK_PRIORITY = 1;
    }
}
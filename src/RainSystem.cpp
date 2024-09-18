#include "RainSystem.h"
#include <Arduino.h>

RainSystem::RainSystem(MatrixConfig& matrix) : matrix(matrix) {}

void RainSystem::init() {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        raindrops[i] = {-1, -1, false};
    }
}

void RainSystem::update(GameState currentState) {
    CRGB rainColor;
    int spawnChance;
    int fallSpeed;

    switch (currentState) {
        case GameState::WAITING_RAIN1:
            rainColor = CRGB::Blue;
            spawnChance = RAIN1_SPAWN_CHANCE / 2;
            fallSpeed = RAIN1_FALL_SPEED;
            break;
        case GameState::RAIN1:
            rainColor = CRGB::Blue;
            spawnChance = RAIN1_SPAWN_CHANCE;
            fallSpeed = RAIN1_FALL_SPEED;
            break;
        case GameState::RAIN2:
            rainColor = CRGB::Cyan;
            spawnChance = RAIN2_SPAWN_CHANCE;
            fallSpeed = RAIN2_FALL_SPEED;
            break;
        case GameState::RAIN3:
            rainColor = CRGB::White;
            spawnChance = RAIN3_SPAWN_CHANCE;
            fallSpeed = RAIN3_FALL_SPEED;
            break;
        default:
            return; // No rain in other states
    }

    updateRaindrops(rainColor, fallSpeed, spawnChance);
}

void RainSystem::spawnRaindrop(CRGB color) {
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        if (!raindrops[i].active) {
            raindrops[i].x = random(MAIN_MATRIX_WIDTH);
            raindrops[i].y = 0;
            raindrops[i].active = true;
            matrix.setPixel(raindrops[i].x, raindrops[i].y, color);
            break;
        }
    }
}

bool RainSystem::isBlockZone(int x, int y) {
    // TODO: Implement logic to check if the given coordinate is in a BLOCK zone
    // This will depend on how you've defined your zones in the bitmap/array
    return false;
}

void RainSystem::updateRaindrops(CRGB color, int fallSpeed, int spawnChance) {
    // Update existing raindrops
    for (int i = 0; i < MAX_RAINDROPS; i++) {
        if (raindrops[i].active) {
            matrix.setPixel(raindrops[i].x, raindrops[i].y, CRGB::Black); // Clear old position
            raindrops[i].y += fallSpeed;

            if (raindrops[i].y >= MAIN_MATRIX_HEIGHT || isBlockZone(raindrops[i].x, raindrops[i].y)) {
                raindrops[i].active = false;
            } else {
                matrix.setPixel(raindrops[i].x, raindrops[i].y, color);
            }
        }
    }

    // Spawn new raindrops
    if (random(100) < spawnChance) {
        spawnRaindrop(color);
    }
}
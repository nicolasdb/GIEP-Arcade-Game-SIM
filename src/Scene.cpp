#include "Scene.h"
#include "config.h"
#include "arrays.h"

Scene::Scene(MatrixConfig& main, MatrixConfig& sec) : mainMatrix(main), secMatrix(sec) {}

void Scene::init() {
    // Initialize both matrices with their default states
    mainMatrix.loadArray(array1);
    secMatrix.loadArray(array2);
}

void Scene::update(GameState currentState) {
    switch (currentState) {
        case GameState::WAITING_DRY:
        case GameState::WAITING_RAIN1:
            drawWaitingScene(currentState == GameState::WAITING_RAIN1);
            break;
        case GameState::RAIN1:
            drawRainScene(RAIN1_FALL_SPEED);
            break;
        case GameState::RAIN2:
            drawRainScene(RAIN2_FALL_SPEED);
            break;
        case GameState::RAIN3:
            drawRainScene(RAIN3_FALL_SPEED);
            break;
        case GameState::WIN:
            drawWinScene();
            break;
        case GameState::FLOOD:
            drawFloodScene();
            break;
        case GameState::POLLUTION:
            drawPollutionScene();
            break;
    }

    updateSecondaryMatrix(currentState);
}

void Scene::drawWaitingScene(bool isRaining) {
    // Clear the main matrix
    mainMatrix.clear();

    // Draw a stable pattern for the waiting state
    for (int i = 0; i < MAIN_MATRIX_WIDTH; i++) {
        for (int j = 0; j < MAIN_MATRIX_HEIGHT; j++) {
            if ((i + j) % 2 == 0) {
                mainMatrix.setPixel(i, j, isRaining ? CRGB::Blue : CRGB::Green);
            }
        }
    }
}

void Scene::drawRainScene(int fallSpeed) {
    // The main rain animation is handled by RainSystem
    // Here we can add additional visual elements if needed
}

void Scene::drawWinScene() {
    mainMatrix.clear();
    // Draw a celebratory pattern, e.g., a smiley face or a checkmark
    // This is a simple example, you may want to create a more elaborate animation
    for (int i = 0; i < MAIN_MATRIX_WIDTH; i++) {
        for (int j = 0; j < MAIN_MATRIX_HEIGHT; j++) {
            if ((i - MAIN_MATRIX_WIDTH/2) * (i - MAIN_MATRIX_WIDTH/2) + 
                (j - MAIN_MATRIX_HEIGHT/2) * (j - MAIN_MATRIX_HEIGHT/2) < 100) {
                mainMatrix.setPixel(i, j, CRGB::Green);
            }
        }
    }
}

void Scene::drawFloodScene() {
    mainMatrix.clear();
    // Fill the matrix with blue color from bottom to top
    static int floodLevel = 0;
    floodLevel = (floodLevel + 1) % MAIN_MATRIX_HEIGHT;
    
    for (int i = 0; i < MAIN_MATRIX_WIDTH; i++) {
        for (int j = MAIN_MATRIX_HEIGHT - 1; j >= MAIN_MATRIX_HEIGHT - floodLevel; j--) {
            mainMatrix.setPixel(i, j, CRGB::Blue);
        }
    }
}

void Scene::drawPollutionScene() {
    mainMatrix.clear();
    // Fill the matrix with brown color patterns
    for (int i = 0; i < MAIN_MATRIX_WIDTH; i++) {
        for (int j = 0; j < MAIN_MATRIX_HEIGHT; j++) {
            if ((i + j) % 3 == 0) {
                mainMatrix.setPixel(i, j, CRGB::Brown);
            }
        }
    }
}

void Scene::updateSecondaryMatrix(GameState state) {
    secMatrix.clear();
    
    switch (state) {
        case GameState::WAITING_DRY:
        case GameState::WAITING_RAIN1:
            // Display a stable pattern
            for (int i = 0; i < SEC_MATRIX_WIDTH; i++) {
                for (int j = 0; j < SEC_MATRIX_HEIGHT; j++) {
                    if ((i + j) % 2 == 0) {
                        secMatrix.setPixel(i, j, state == GameState::WAITING_RAIN1 ? CRGB::Blue : CRGB::Green);
                    }
                }
            }
            break;
        case GameState::RAIN1:
        case GameState::RAIN2:
        case GameState::RAIN3:
            // Display increasing intensity of blue
            CRGB rainColor;
            if (state == GameState::RAIN1) rainColor = CRGB(0, 0, 100);
            else if (state == GameState::RAIN2) rainColor = CRGB(0, 0, 180);
            else rainColor = CRGB(0, 0, 255);
            
            for (int i = 0; i < SEC_MATRIX_WIDTH; i++) {
                for (int j = 0; j < SEC_MATRIX_HEIGHT; j++) {
                    secMatrix.setPixel(i, j, rainColor);
                }
            }
            break;
        case GameState::WIN:
            // Display green for win
            for (int i = 0; i < SEC_MATRIX_WIDTH; i++) {
                for (int j = 0; j < SEC_MATRIX_HEIGHT; j++) {
                    secMatrix.setPixel(i, j, CRGB::Green);
                }
            }
            break;
        case GameState::FLOOD:
            // Display blue for flood
            for (int i = 0; i < SEC_MATRIX_WIDTH; i++) {
                for (int j = 0; j < SEC_MATRIX_HEIGHT; j++) {
                    secMatrix.setPixel(i, j, CRGB::Blue);
                }
            }
            break;
        case GameState::POLLUTION:
            // Display brown for pollution
            for (int i = 0; i < SEC_MATRIX_WIDTH; i++) {
                for (int j = 0; j < SEC_MATRIX_HEIGHT; j++) {
                    secMatrix.setPixel(i, j, CRGB::Brown);
                }
            }
            break;
    }
}
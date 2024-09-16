#include <Arduino.h>
#include <FastLED.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MatrixConfig.h"
#include "SecondaryLEDHandler.h"
#include "ButtonHandler.h"
#include "config.h"

unsigned long lastButtonReleaseTime = 0;
bool button1WasPressed = false;
bool button2WasPressed = false;

void buttonTask(void *pvParameters) {
    for (;;) {
        ButtonHandler::checkButtons();
        bool button1Pressed = ButtonHandler::isButton1Pressed();
        bool button2Pressed = ButtonHandler::isButton2Pressed();

        if (!button1Pressed && !button2Pressed) {
            if (button1WasPressed || button2WasPressed) {
                lastButtonReleaseTime = millis();
            }
        }

        button1WasPressed = button1Pressed;
        button2WasPressed = button2Pressed;

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void displayTask(void *pvParameters) {
    for (;;) {
        if (ButtonHandler::isButton1Pressed()) {
            MatrixConfig::activateBluePixels();
            SecondaryLEDHandler::activateBluePixels();
        } else if (ButtonHandler::isButton2Pressed()) {
            MatrixConfig::activateBlueAndYellowPixels();
            SecondaryLEDHandler::activateBlueAndYellowPixels();
        } else if (millis() - lastButtonReleaseTime > BUTTON_RELEASE_DELAY) {
            MatrixConfig::deactivateAllPixels();
            SecondaryLEDHandler::deactivateAllPixels();
        }
        
        MatrixConfig::updateMatrix();
        SecondaryLEDHandler::updateLEDs();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void setup() {
    // Initialize MatrixConfig with 25x25 size, BOTTOM_LEFT_VERTICAL orientation, and zigzag mode
    MatrixConfig::initialize(25, 25, MatrixOrientation::BOTTOM_LEFT_VERTICAL, true);

    // Initialize SecondaryLEDHandler with 56 LEDs (4x14 array)
    SecondaryLEDHandler::initialize(56);

    ButtonHandler::initialize();

    xTaskCreatePinnedToCore(buttonTask, "ButtonTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(displayTask, "DisplayTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
    // Empty. Tasks are handled by FreeRTOS
}
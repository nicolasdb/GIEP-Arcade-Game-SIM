#include <Arduino.h>
#include <FastLED.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "MatrixConfig.h"
#include "SecondaryLEDHandler.h"
#include "ButtonHandler.h"
#include "config.h"

unsigned long lastButtonReleaseTime = 0;

void buttonTask(void *pvParameters) {
    for (;;) {
        ButtonHandler::checkButtons();
        if (!ButtonHandler::isButton1Pressed() && !ButtonHandler::isButton2Pressed()) {
            lastButtonReleaseTime = millis();
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void displayTask(void *pvParameters) {
    for (;;) {
        if (ButtonHandler::isButton1Pressed()) {
            MatrixConfig::setAllPixels(CRGB(COLOR_BRIGHT_WHITE));
            SecondaryLEDHandler::setAllPixels(CRGB(COLOR_BRIGHT_WHITE));
        } else if (ButtonHandler::isButton2Pressed()) {
            MatrixConfig::setAllPixels(CRGB(COLOR_BRIGHT_WHITE));
            SecondaryLEDHandler::setAllPixels(CRGB(COLOR_BRIGHT_WHITE));
        } else if (millis() - lastButtonReleaseTime > BUTTON_RELEASE_DELAY) {
            MatrixConfig::clearMatrix();
            SecondaryLEDHandler::clearLEDs();
        } else {
            if (lastButtonReleaseTime > 0 && millis() - lastButtonReleaseTime <= BUTTON_RELEASE_DELAY) {
                CRGB color = ButtonHandler::isButton1Pressed() ? CRGB(COLOR_BLUE) : CRGB(COLOR_BLUE) | CRGB(COLOR_YELLOW);
                MatrixConfig::setAllPixels(color);
                SecondaryLEDHandler::setAllPixels(color);
            }
        }
        
        MatrixConfig::updateMatrix();
        SecondaryLEDHandler::updateLEDs();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void setup() {
    MatrixConfig::initialize();
    SecondaryLEDHandler::initialize();
    ButtonHandler::initialize();

    xTaskCreatePinnedToCore(buttonTask, "ButtonTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(displayTask, "DisplayTask", 2048, NULL, 1, NULL, 1);
}

void loop() {
    // Empty. Tasks are handled by FreeRTOS
}
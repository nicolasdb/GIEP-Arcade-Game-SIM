#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <FastLED.h>
#include <Wire.h>
#include "GameLogic.h"
#include "config.h"

CRGB mainLeds[MAIN_MATRIX_WIDTH * MAIN_MATRIX_HEIGHT];
CRGB secLeds[SEC_MATRIX_WIDTH * SEC_MATRIX_HEIGHT];
GameLogic gameLogic(mainLeds, secLeds);

void gameTask(void *pvParameters) {
    for (;;) {
        gameLogic.update();
        vTaskDelay(pdMS_TO_TICKS(10)); // 10ms delay, adjust as needed for smooth gameplay
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // Wait for serial port to connect. Needed for native USB port only
    }
    
    Serial.println("Initializing game...");

    // Initialize I2C for MCP23017
    Wire.begin(MCP23017_SDA_PIN, MCP23017_SCL_PIN);

    // Initialize FastLED
    FastLED.addLeds<WS2813, MAIN_MATRIX_PIN, RGB>(mainLeds, MAIN_MATRIX_WIDTH * MAIN_MATRIX_HEIGHT);
    FastLED.addLeds<WS2813, SEC_MATRIX_PIN, RGB>(secLeds, SEC_MATRIX_WIDTH * SEC_MATRIX_HEIGHT);
    FastLED.setBrightness(MATRIX_BRIGHTNESS);

    gameLogic.init();

    xTaskCreatePinnedToCore(
        gameTask,
        "GameTask",
        8192,  // Increased stack size
        NULL,
        1,
        NULL,
        1  // Run on core 1
    );

    Serial.println("Game initialized and running.");
}

void loop() {
    // The main loop is empty as we're using FreeRTOS tasks
    vTaskDelete(NULL);
}
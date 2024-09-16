#include <Arduino.h>
#include <FastLED.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"
#include "game_config.h"
#include "ButtonHandler.h"
#include "MCP23017Handler.h"
#include "DebugLogger.h"
#include "../include/array1.h"
#include "../include/array2.h"

CRGB leds[NUM_LEDS];
CRGB secondaryLeds[TOTAL_SECONDARY_LEDS];
MCP23017Handler mcpHandler(MCP23017_ADDRESS);

void handleButtonPress(uint8_t button) {
    DebugLogger::info("Button %d pressed", button);
    // Blink the corresponding LED
    if (button < NUM_MCP_BUTTONS) {
        // Toggle the LED on
        mcpHandler.setLED(button, true);
        vTaskDelay(pdMS_TO_TICKS(DEBUG_BUTTON_BLINK_DURATION));
        // Toggle the LED off
        mcpHandler.setLED(button, false);
    } else if (button == NUM_MCP_BUTTONS) {
        // Handle 9th button (BUTTON9_PIN)
        digitalWrite(LED9_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(DEBUG_BUTTON_BLINK_DURATION));
        digitalWrite(LED9_PIN, LOW);
    }
}

void handleButtonRelease(uint8_t button) {
    DebugLogger::info("Button %d released", button);
}

void updateMainLEDs() {
    // Draw the array1.h bitmap on the main LED matrix
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(MAIN_LED_BITMAP[i]);
    }
    FastLED.show();
}

void updateSecondaryLEDs() {
    // Draw the array2.h bitmap on the secondary LED array
    for (int i = 0; i < TOTAL_SECONDARY_LEDS; i++) {
        secondaryLeds[i] = CRGB(SECONDARY_LED_BITMAP[i]);
    }
    FastLED.show();
}

void buttonTask(void* parameter) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(10);  // Check every 10ms

    while (true) {
        // Check each button on the MCP23017
        for (int i = 0; i < NUM_MCP_BUTTONS; i++) {
            if (mcpHandler.readButton(i)) {
                handleButtonPress(i);
            }
        }

        // Check the 9th button (BUTTON9_PIN)
        if (digitalRead(BUTTON9_PIN) == LOW) {
            handleButtonPress(NUM_MCP_BUTTONS);
        }

        vTaskDelayUntil(&lastWakeTime, frequency);
    }
}

void ledUpdateTask(void* parameter) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(33);  // ~30fps

    while (true) {
        updateMainLEDs();
        updateSecondaryLEDs();
        vTaskDelayUntil(&lastWakeTime, frequency);
    }
}

void setup() {
    Serial.begin(115200);
    
    // Wait for serial or timeout after 3 seconds
    unsigned long startTime = millis();
    while (!Serial && millis() - startTime < 3000) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    DebugLogger::init(Serial, LogLevel::INFO);
    DebugLogger::info("System initializing");

    mcpHandler.begin();

    // Setup 9th button and LED
    pinMode(BUTTON9_PIN, INPUT_PULLUP);
    pinMode(LED9_PIN, OUTPUT);

    FastLED.addLeds<LED_TYPE, MATRIX_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, SECONDARY_LED_PIN, COLOR_ORDER>(secondaryLeds, TOTAL_SECONDARY_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();

    xTaskCreatePinnedToCore(buttonTask, "ButtonTask", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, NULL, 0);
    xTaskCreatePinnedToCore(ledUpdateTask, "LEDUpdateTask", LED_UPDATE_TASK_STACK_SIZE, NULL, LED_UPDATE_TASK_PRIORITY, NULL, 1);

    DebugLogger::info("Setup complete");
}

void loop() {
    // Empty, as tasks are handling everything
    vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1 second
}
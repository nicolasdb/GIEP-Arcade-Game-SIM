#include <Arduino.h>
#include <FastLED.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DebugLogger.h"
#include "StateTracker.h"
#include "MatrixConfig.h"
#include "Scene.h"
#include "GameLogic.h"

#define LED_PIN     9
#define BRIGHTNESS  30
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define DEBUG_BUTTON  0  // Built-in button pin

const uint8_t MATRIX_WIDTH = 8;
const uint8_t MATRIX_HEIGHT = 8;
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

const uint8_t BUTTON_PINS[] = {1, 3, 5, 7, 10, 11, 12, 13, 14};  // Added more pins for 9 buttons
const uint8_t NUM_BUTTONS = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

CRGB leds[NUM_LEDS];
MatrixConfig matrixConfig(MATRIX_WIDTH, MATRIX_HEIGHT, MatrixOrientation::TOP_LEFT_HORIZONTAL, false);
Scene scene(matrixConfig);
GameLogic gameLogic(scene);

// Debounce variables for debug button
bool lastDebugButtonState = HIGH;
unsigned long lastDebugButtonDebounceTime = 0;
unsigned long debugButtonDebounceDelay = 50;

void buttonTask(void* parameter) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(10);  // Check every 10ms
    bool lastDebugButtonState = HIGH;

    while (true) {
        // Handle game buttons
        for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
            bool buttonState = digitalRead(BUTTON_PINS[i]) == LOW;
            gameLogic.handleButton(i, buttonState);
        }

        // Handle debug button
        bool debugButtonState = digitalRead(DEBUG_BUTTON) == LOW;
        if (debugButtonState != lastDebugButtonState) {
            if (debugButtonState == LOW) {
                DebugLogger::info("Debug button pressed");
                DebugLogger::info("Current game state: %s", gameLogic.getStateString());
                // Add more debug information as needed
            }
            lastDebugButtonState = debugButtonState;
        }

        vTaskDelayUntil(&lastWakeTime, frequency);
    }
}


void gameUpdateTask(void* parameter) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(100);  // Update every 100ms

    while (true) {
        gameLogic.update();
        vTaskDelayUntil(&lastWakeTime, frequency);
    }
}

void ledUpdateTask(void* parameter) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(33);  // ~30fps

    while (true) {
        scene.update();
        scene.draw(leds);
        FastLED.show();
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

    // while (!Serial) { ; }
    // delay(1000);

    DebugLogger::init(Serial, LogLevel::INFO);
    DebugLogger::info("Debug logger initialized");

    StateTracker::setState(SystemState::INITIALIZING);
    DebugLogger::info("Initial system state: %s", StateTracker::getCurrentStateString());

    pinMode(DEBUG_BUTTON, INPUT_PULLUP);
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    }

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
        // Clear all LEDs
    FastLED.clear();
    FastLED.show();
    DebugLogger::info("FastLED initialized");

    StateTracker::setState(SystemState::MATRIX_READY);
    DebugLogger::info("Matrix ready. System state: %s", StateTracker::getCurrentStateString());

    scene.loadDefaultScene();
    DebugLogger::info("Default scene loaded");

    StateTracker::setState(SystemState::SCENE_LOADED);
    DebugLogger::info("Setup complete. Final system state: %s", StateTracker::getCurrentStateString());

    xTaskCreatePinnedToCore(buttonTask, "ButtonTask", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(gameUpdateTask, "GameUpdateTask", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(ledUpdateTask, "LEDUpdateTask", 2048, NULL, 1, NULL, 1);

    DebugLogger::info("FreeRTOS tasks created");
}

void loop() {
    // Empty, as tasks are handling everything
}
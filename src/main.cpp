#include <Arduino.h>
#include <FastLED.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DebugLogger.h"
#include "StateTracker.h"
#include "MatrixConfig.h"
#include "Scene.h"
#include "GameLogic.h"
#include "config.h"
#include "ButtonHandler.h"
#include "MCP23017Handler.h"
#include "esp_task_wdt.h"

#define LED_PIN     9
#define BRIGHTNESS  30
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

CRGB leds[NUM_LEDS];
MatrixConfig matrixConfig(MATRIX_WIDTH, MATRIX_HEIGHT, MatrixOrientation::BOTTOM_RIGHT_VERTICAL, true);
Scene scene(matrixConfig);
GameLogic gameLogic(scene);
MCP23017Handler mcpHandler(MCP23017_ADDRESS);
ButtonHandler buttonHandler(mcpHandler, gameLogic);

void setupWatchdog() {
    esp_task_wdt_init(WDT_TIMEOUT, true);  // Enable panic so ESP32 restarts
    esp_task_wdt_add(NULL);  // Add current thread to WDT watch
}

void feedWatchdog() {
    esp_task_wdt_reset();
}

void buttonTask(void* parameter) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(10);  // Check every 10ms

    while (true) {
        buttonHandler.update();
        feedWatchdog();
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

    DebugLogger::init(Serial, LogLevel::INFO);
    DebugLogger::info("Debug logger initialized");

    StateTracker::setState(SystemState::INITIALIZING);
    DebugLogger::info("Initial system state: %s", StateTracker::getCurrentStateString());

    pinMode(DEBUG_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BASIN_GATE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BASIN_GATE_LED_PIN, OUTPUT);

    mcpHandler.begin();
    DebugLogger::info("MCP23017 initialized");

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();
    
    DebugLogger::info("FastLED initialized");

    StateTracker::setState(SystemState::MATRIX_READY);
    DebugLogger::info("Matrix ready. System state: %s", StateTracker::getCurrentStateString());

    scene.loadDefaultScene();
    DebugLogger::info("Default scene loaded");

    StateTracker::setState(SystemState::SCENE_LOADED);
    DebugLogger::info("Setup complete. Final system state: %s", StateTracker::getCurrentStateString());

    setupWatchdog();
    DebugLogger::info("Watchdog initialized");

    xTaskCreatePinnedToCore(buttonTask, "ButtonTask", BUTTON_TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL, 0);
    xTaskCreatePinnedToCore(gameUpdateTask, "GameUpdateTask", GAME_UPDATE_TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL, 1);
    xTaskCreatePinnedToCore(ledUpdateTask, "LEDUpdateTask", LED_UPDATE_TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL, 1);

    DebugLogger::info("FreeRTOS tasks created");
}

void loop() {
    // Empty, as tasks are handling everything
    feedWatchdog();
    vTaskDelay(pdMS_TO_TICKS(100));  // Small delay to prevent watchdog from triggering unnecessarily
}
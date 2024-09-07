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
#include "SecondaryLEDHandler.h"
#include "esp_task_wdt.h"

CRGB leds[NUM_LEDS];
MatrixConfig matrixConfig(MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_ORIENTATION, true);
Scene scene(matrixConfig);
SecondaryLEDHandler secondaryLEDs;
GameLogic gameLogic(scene, secondaryLEDs);
MCP23017Handler mcpHandler(MCP23017_ADDRESS);
ButtonHandler buttonHandler(mcpHandler, gameLogic);

void setupWatchdog() {
    esp_err_t err = esp_task_wdt_init(WDT_TIMEOUT, false);  // Disable panic so ESP32 doesn't restart
    if (err != ESP_OK) {
        DebugLogger::error("Failed to initialize watchdog timer: %d", err);
        return;
    }
    DebugLogger::info("Watchdog timer initialized successfully");
}

void feedWatchdog() {
    esp_err_t err = esp_task_wdt_reset();
    if (err != ESP_OK) {
        DebugLogger::warn("Failed to feed watchdog timer: %d", err);
    }
}

void buttonTask(void* parameter) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(10);  // Check every 10ms

    while (true) {
        buttonHandler.update();
        vTaskDelayUntil(&lastWakeTime, frequency);
    }
}

void gameUpdateTask(void* parameter) {
    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(33);  // Update every ~30fps

    while (true) {
        TickType_t startTime = xTaskGetTickCount();
        
        gameLogic.update();
        feedWatchdog();
        
        TickType_t endTime = xTaskGetTickCount();
        TickType_t executionTime = endTime - startTime;
        
        DebugLogger::debug("gameUpdateTask execution time: %dms", pdTICKS_TO_MS(executionTime));
        
        if (executionTime > frequency) {
            DebugLogger::warn("gameUpdateTask exceeded update frequency by %dms", 
                              pdTICKS_TO_MS(executionTime - frequency));
        }
        
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
        secondaryLEDs.update();
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

    secondaryLEDs.begin();
    DebugLogger::info("Secondary LEDs initialized");

    StateTracker::setState(SystemState::MATRIX_READY);
    DebugLogger::info("Matrix ready. System state: %s", StateTracker::getCurrentStateString());

    scene.loadDefaultScene();
    DebugLogger::info("Default scene loaded");

    StateTracker::setState(SystemState::SCENE_LOADED);
    DebugLogger::info("Setup complete. Final system state: %s", StateTracker::getCurrentStateString());

    setupWatchdog();

    BaseType_t result;
    result = xTaskCreatePinnedToCore(buttonTask, "ButtonTask", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, NULL, 0);
    if (result != pdPASS) {
        DebugLogger::error("Failed to create ButtonTask: %d", result);
    }
    result = xTaskCreatePinnedToCore(gameUpdateTask, "GameUpdateTask", GAME_UPDATE_TASK_STACK_SIZE, NULL, GAME_UPDATE_TASK_PRIORITY, NULL, 1);
    if (result != pdPASS) {
        DebugLogger::error("Failed to create GameUpdateTask: %d", result);
    }
    result = xTaskCreatePinnedToCore(ledUpdateTask, "LEDUpdateTask", LED_UPDATE_TASK_STACK_SIZE, NULL, LED_UPDATE_TASK_PRIORITY, NULL, 1);
    if (result != pdPASS) {
        DebugLogger::error("Failed to create LEDUpdateTask: %d", result);
    }

    DebugLogger::info("FreeRTOS tasks created");
}

void loop() {
    // Empty, as tasks are handling everything
    vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1 second
}
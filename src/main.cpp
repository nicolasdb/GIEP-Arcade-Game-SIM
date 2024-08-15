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
#include "esp_task_wdt.h"

#define LED_PIN     9
#define BRIGHTNESS  30
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB


// const uint8_t MATRIX_WIDTH = 8;
// const uint8_t MATRIX_HEIGHT = 8;
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

// const uint8_t DEBUG_BUTTON_PIN = 0;
const uint8_t BUTTON_PINS[] = {1, 3, 5, 7, 10, 11, 12, 13, 14};  // 9 game buttons
const uint8_t NUM_BUTTONS = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

CRGB leds[NUM_LEDS];
MatrixConfig matrixConfig(MATRIX_WIDTH, MATRIX_HEIGHT, MatrixOrientation::BOTTOM_RIGHT_VERTICAL, true);
Scene scene(matrixConfig);
GameLogic gameLogic(scene);
ButtonHandler buttonHandler(BUTTON_PINS, DEBUG_BUTTON_PIN, gameLogic); 

void smoothStartup() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show();
    delay(50);  // Short delay to ensure LEDs are off

    for (int brightness = 0; brightness <= BRIGHTNESS; brightness++) {
        FastLED.setBrightness(brightness);
        FastLED.show();
        delay(10);  // Gradual increase in brightness
    }
}

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
    for (uint8_t i = 0; i < NUM_GAME_BUTTONS; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    }

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(0);  // Start with brightness 0
    smoothStartup();  // Implement smooth startup
    
    DebugLogger::info("FastLED initialized");

    StateTracker::setState(SystemState::MATRIX_READY);
    DebugLogger::info("Matrix ready. System state: %s", StateTracker::getCurrentStateString());

    scene.loadDefaultScene();
    DebugLogger::info("Default scene loaded");

    StateTracker::setState(SystemState::SCENE_LOADED);
    DebugLogger::info("Setup complete. Final system state: %s", StateTracker::getCurrentStateString());

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
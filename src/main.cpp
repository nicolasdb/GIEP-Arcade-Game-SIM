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
#include "ConfigLoader.h"
#include "BitmapConfig.h"
#include <SPIFFS.h>

CRGB leds[NUM_LEDS];
MatrixConfig matrixConfig(MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_ORIENTATION, true);
Scene scene(matrixConfig);
SecondaryLEDHandler secondaryLEDs;
GameLogic gameLogic(scene, secondaryLEDs);
MCP23017Handler mcpHandler(MCP23017_ADDRESS);
ButtonHandler buttonHandler(mcpHandler, gameLogic);

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

    DebugLogger::init(Serial, LogLevel::CRITICAL);
    DebugLogger::critical("System initialized");

    if (!SPIFFS.begin(true)) {
        DebugLogger::critical("An error occurred while mounting SPIFFS");
        return;
    }

    // Load configuration
    if (!ConfigLoader::loadConfig("/game_config.json")) {
        DebugLogger::critical("Failed to load game configuration");
    }

    // Load bitmaps
    if (!matrixConfig.loadBitmap("/primary_bitmap.bin")) {
        DebugLogger::critical("Failed to load primary bitmap");
    }
    if (!secondaryLEDs.loadBitmap("/secondary_bitmap.bin")) {
        DebugLogger::critical("Failed to load secondary bitmap");
    }

    pinMode(DEBUG_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BASIN_GATE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BASIN_GATE_LED_PIN, OUTPUT);

    mcpHandler.begin();
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(ConfigLoader::BRIGHTNESS());
    FastLED.clear();
    FastLED.show();
    secondaryLEDs.begin();
    scene.loadDefaultScene();

    BaseType_t result;
    result = xTaskCreatePinnedToCore(buttonTask, "ButtonTask", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, NULL, 0);
    if (result != pdPASS) {
        DebugLogger::critical("Failed to create ButtonTask: %d", result);
    }
    result = xTaskCreatePinnedToCore(gameUpdateTask, "GameUpdateTask", GAME_UPDATE_TASK_STACK_SIZE, NULL, GAME_UPDATE_TASK_PRIORITY, NULL, 1);
    if (result != pdPASS) {
        DebugLogger::critical("Failed to create GameUpdateTask: %d", result);
    }
    result = xTaskCreatePinnedToCore(ledUpdateTask, "LEDUpdateTask", LED_UPDATE_TASK_STACK_SIZE, NULL, LED_UPDATE_TASK_PRIORITY, NULL, 1);
    if (result != pdPASS) {
        DebugLogger::critical("Failed to create LEDUpdateTask: %d", result);
    }

    DebugLogger::critical("Setup complete");
}

void loop() {
    // Empty, as tasks are handling everything
    vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1 second
}
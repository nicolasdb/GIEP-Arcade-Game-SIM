#include <Arduino.h>
#include <FastLED.h>
#include "DebugLogger.h"
#include "StateTracker.h"
#include "MatrixConfig.h"
#include "Scene.h"


#define LED_PIN     9
#define BRIGHTNESS  30
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define DEBUG_BUTTON  0  // Built-in button pin

// Define matrix dimensions
const uint8_t MATRIX_WIDTH = 8;
const uint8_t MATRIX_HEIGHT = 8;

// Calculate total number of LEDs
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

// Define button pins
const uint8_t BUTTON_PINS[] = {1, 3, 5, 7};
const uint8_t NUM_BUTTONS = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);
int lastDebugButtonState = HIGH;
unsigned long lastDebugDebounceTime = 0;


// Debounce parameters
const unsigned long DEBOUNCE_DELAY = 50; // milliseconds
unsigned long lastDebounceTime[NUM_BUTTONS] = {0};
bool buttonState[NUM_BUTTONS] = {HIGH, HIGH, HIGH, HIGH};
bool lastButtonState[NUM_BUTTONS] = {HIGH, HIGH, HIGH, HIGH};


CRGB leds[NUM_LEDS];
MatrixConfig config(MATRIX_WIDTH, MATRIX_HEIGHT, MatrixOrientation::TOP_LEFT_HORIZONTAL, false);
Scene scene(config);

// Define the bitmap for our scene
const uint32_t BITMAP_ARRAY[NUM_LEDS] = {
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
  0xFFFFFF, 0xFFFFFF, 0x00FF00, 0xFFFFFF, 0x00FF00, 0xFFFFFF, 0x00FF00, 0xFFFFFF,
  0x00FF00, 0xFFFFFF, 0x00FF00, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0x000000,
  0xFFFFFF, 0xFFFFFF, 0x000000, 0x000000, 0x000000, 0xFFFF00, 0x0000FF, 0x000000,
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0xFFFF00, 0x0000FF, 0x000000,
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0xFFFF00, 0x0000FF, 0x000000,
  0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0x000000, 0xFFFF00, 0x0000FF, 0x000000
};

void logSystemState() {
    DebugLogger::info("Current system state: %s", StateTracker::getCurrentStateString());
    DebugLogger::info("Matrix config: %dx%d, %s, %s",
                      config.getWidth(), config.getHeight(),
                      config.getOrientation() == MatrixOrientation::TOP_LEFT_HORIZONTAL ? "TOP_LEFT_HORIZONTAL" : "Other",
                      config.isZigzag() ? "zigzag" : "normal");
    DebugLogger::info("Total LEDs: %d", NUM_LEDS);
    DebugLogger::info("Scene loaded: Yes");
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    delay(1000);

    pinMode(DEBUG_BUTTON, INPUT_PULLUP);

    // Initialize button pins
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    }

    DebugLogger::init(Serial, LogLevel::INFO);
    DebugLogger::info("Debug logger initialized");

    StateTracker::setState(SystemState::INITIALIZING);
    DebugLogger::info("Initial system state: %s", StateTracker::getCurrentStateString());

    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    DebugLogger::info("FastLED initialized");

    StateTracker::setState(SystemState::MATRIX_READY);
    DebugLogger::info("Matrix ready. System state: %s", StateTracker::getCurrentStateString());

    scene.loadBitmap(BITMAP_ARRAY, MATRIX_WIDTH, MATRIX_HEIGHT);
    DebugLogger::info("Scene bitmap loaded");

    StateTracker::setState(SystemState::SCENE_LOADED);
    DebugLogger::info("Setup complete. Final system state: %s", StateTracker::getCurrentStateString());


}


void handleButtons() {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        int reading = digitalRead(BUTTON_PINS[i]);
        if (reading != lastButtonState[i]) {
            lastDebounceTime[i] = millis();
        }

        if ((millis() - lastDebounceTime[i]) > DEBOUNCE_DELAY) {
            if (reading != buttonState[i]) {
                buttonState[i] = reading;
                if (buttonState[i] == LOW) {
                    // Button is pressed
                    DebugLogger::info("Button %d pressed", i + 1);
                    scene.setInteractiveGroupState(static_cast<PixelType>(static_cast<int>(PixelType::INTERACTIVE_1) + i), true);
                } else {
                    // Button is released
                    DebugLogger::info("Button %d released", i + 1);
                    scene.setInteractiveGroupState(static_cast<PixelType>(static_cast<int>(PixelType::INTERACTIVE_1) + i), false);
                }
            }
        }
        lastButtonState[i] = reading;
    }
}

void handleDebugButton() {
    int reading = digitalRead(DEBUG_BUTTON);
    if (reading != lastDebugButtonState) {
        lastDebugDebounceTime = millis();
    }

    if ((millis() - lastDebugDebounceTime) > DEBOUNCE_DELAY) {
        if (reading == LOW && lastDebugButtonState == HIGH) {
            // Debug button is pressed (falling edge)
            logSystemState();
        }
    }

    lastDebugButtonState = reading;
}


void loop() {
    static unsigned long lastUpdate = 0;
    unsigned long currentMillis = millis();

    // Check debug button state
    if (digitalRead(DEBUG_BUTTON) == LOW) {
        logSystemState();
        delay(50);  // Simple debounce for debug button
    }

    // Handle interactive buttons
    handleButtons();

    if (currentMillis - lastUpdate >= 100) {
        scene.update(); // Update rain
        scene.draw(leds);
        FastLED.show();
        lastUpdate = currentMillis;
    }
}
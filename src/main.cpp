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
#define PIN_BUTTON  0  // Built-in button pin

// Define matrix dimensions
const uint8_t MATRIX_WIDTH = 8;
const uint8_t MATRIX_HEIGHT = 8;

// Calculate total number of LEDs
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

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

void testPattern() {
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Red;
    }
    FastLED.show();
    delay(1000);
    
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Green;
    }
    FastLED.show();
    delay(1000);
    
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Blue;
    }
    FastLED.show();
    delay(1000);
}

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    delay(1000);

    pinMode(PIN_BUTTON, INPUT_PULLUP);

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


    // Run test pattern
    // DebugLogger::info("Running test pattern");
    // testPattern();
    // DebugLogger::info("Test pattern complete");
}

void loop() {
    static unsigned long lastUpdate = 0;
    static bool lastButtonState = HIGH;
    unsigned long currentMillis = millis();

    // Check button 0 state
    bool currentButtonState = digitalRead(PIN_BUTTON);
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        // Button was just pressed
        logSystemState();
        delay(50);  // Simple debounce
    }
    lastButtonState = currentButtonState;

    if (currentMillis - lastUpdate >= 100) {
        DebugLogger::debug("Updating LED pattern");
        scene.update(); // Update rain
        scene.draw(leds);
        FastLED.show();
        DebugLogger::info("LEDs updated");
        lastUpdate = currentMillis;
    }
}
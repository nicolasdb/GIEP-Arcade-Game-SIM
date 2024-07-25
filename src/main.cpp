#include <Arduino.h>
#include <FastLED.h>
#include "DebugLogger.h"
#include "StateTracker.h"
#include "MatrixConfig.h"


#define LED_PIN     9  // Change this to the pin your LED matrix is connected to
#define NUM_LEDS    64 // 8x8 matrix
#define BRIGHTNESS  30
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
MatrixConfig config(8, 8, MatrixOrientation::TOP_LEFT_HORIZONTAL, false);


void setup() {
    Serial.begin(115200);
    while (!Serial) { ; } // Wait for serial port to connect
    delay(1000);  // Add a delay to ensure serial is ready

    DebugLogger::init(Serial, LogLevel::DEBUG);
    DebugLogger::info("Debug logger initialized");

    StateTracker::setState(SystemState::INITIALIZING);
    DebugLogger::info("System state: %s", StateTracker::getCurrentStateString());

    // Initialize FastLED
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    DebugLogger::info("FastLED initialized");

    StateTracker::setState(SystemState::RUNNING);
    DebugLogger::info("Setup complete. System state: %s", StateTracker::getCurrentStateString());
    
    // Test DebugLogger
    uint8_t testX = 5;
    uint8_t testY = 6;
    DebugLogger::debug("Test debug output: x=%u, y=%u", static_cast<unsigned int>(testX), static_cast<unsigned int>(testY));

    // Test XY function
    for (uint8_t x = 0; x < 8; x++) {
        for (uint8_t y = 0; y < 8; y++) {
            uint16_t index = config.XY(x, y);
            DebugLogger::debug("XY(%u, %u) = %u", static_cast<unsigned int>(x), static_cast<unsigned int>(y), index);
        }
    }
}


void drawPattern() {
    // Clear all LEDs
    FastLED.clear();

    // Draw a simple pattern - a diagonal line
    for (uint8_t i = 0; i < 8; i++) {
        DebugLogger::debug("Drawing pixel at (%u, %u)", static_cast<unsigned int>(i), static_cast<unsigned int>(i));
        uint16_t ledIndex = config.XY(i, i);
        DebugLogger::debug("LED index for (%u, %u): %u", static_cast<unsigned int>(i), static_cast<unsigned int>(i), ledIndex);
        if (ledIndex < NUM_LEDS) {
            leds[ledIndex] = CRGB::Red;
        } else {
            DebugLogger::error("LED index out of bounds: %u", ledIndex);
        }
    }

    FastLED.show();
}

void loop() {
    static unsigned long lastUpdate = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= 1000) {
        DebugLogger::debug("Updating LED pattern");
        drawPattern();
        lastUpdate = currentMillis;
    }
}
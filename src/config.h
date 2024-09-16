#ifndef CONFIG_H
#define CONFIG_H

// Define ESP32 for FastLED
#define FASTLED_ESP32_I2S true

// Hardware configuration
#define MATRIX_DATA_PIN 9
#define SECONDARY_ARRAY_DATA_PIN 7
#define MCP_SDA_PIN 13
#define MCP_SCL_PIN 15

// Button release delay (in milliseconds)
#define BUTTON_RELEASE_DELAY 1000

// Colors
#define COLOR_BLUE 0x0000c7
#define COLOR_YELLOW 0xffff00
#define COLOR_WHITE 0xffffff

// Brightness levels
#define BRIGHTNESS_ACTIVE_WHITE 200
#define BRIGHTNESS_INACTIVE_WHITE 5
#define BRIGHTNESS_ACTIVE_BLUE 200
#define BRIGHTNESS_INACTIVE_BLUE 5
#define BRIGHTNESS_ACTIVE_YELLOW 200
#define BRIGHTNESS_INACTIVE_YELLOW 5

#endif // CONFIG_H
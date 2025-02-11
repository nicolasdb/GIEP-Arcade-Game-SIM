#ifndef CONFIG_H
#define CONFIG_H


// Hardware configuration
#define MATRIX_DATA_PIN 9
#define SECONDARY_ARRAY_DATA_PIN 7
#define MCP_SDA_PIN 13
#define MCP_SCL_PIN 15

// Button release delay (in milliseconds)
#define BUTTON_RELEASE_DELAY 5000

// Map colors (for identifying pixels)
#define MAP_BLUE 0x0000c7
#define MAP_YELLOW 0xffff00
#define MAP_WHITE 0xffffff

// Display colors (actual LED colors)
#define DISPLAY_BLUE 0x0000c7
#define DISPLAY_DEEP_BLUE 0x0000c7
#define DISPLAY_WHITE 0xffffff
#define DISPLAY_BLUE_TINTED_WHITE 0xffffa0  // Renamed from yellow-tinted white

// Brightness levels
#define BRIGHTNESS_INACTIVE_WHITE 10
#define BRIGHTNESS_ACTIVE_BLUE_TINTED_WHITE 100  // Renamed from yellow-tinted white
#define BRIGHTNESS_INACTIVE_BLUE_TINTED_WHITE 10  // Renamed from yellow-tinted white
#define BRIGHTNESS_ACTIVE_BLUE 100
#define BRIGHTNESS_INACTIVE_BLUE 5
#define BRIGHTNESS_ACTIVE_DEEP_BLUE 100  // Renamed from yellow
#define BRIGHTNESS_INACTIVE_DEEP_BLUE 5  // Renamed from yellow

#endif // CONFIG_H

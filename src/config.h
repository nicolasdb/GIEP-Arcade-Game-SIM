#pragma once

// MCP23017 Configuration
#define MCP23017_ADDRESS 0x20

// Button pins
#define BUTTON1_PIN mcp.GPIOA0
#define BUTTON2_PIN mcp.GPIOA1 
#define BUTTON3_PIN mcp.GPIOA2
#define BUTTON4_PIN mcp.GPIOA3
#define BUTTON5_PIN mcp.GPIOA4
#define BUTTON6_PIN mcp.GPIOA5
#define BUTTON7_PIN mcp.GPIOA6
#define BUTTON8_PIN mcp.GPIOA7
#define BUTTON9_PIN 1

// LED pins
#define LED1_PIN mcp.GPIOB0
#define LED2_PIN mcp.GPIOB1
#define LED3_PIN mcp.GPIOB2
#define LED4_PIN mcp.GPIOB3
#define LED5_PIN mcp.GPIOB4
#define LED6_PIN mcp.GPIOB5
#define LED7_PIN mcp.GPIOB6
#define LED8_PIN mcp.GPIOB7
#define LED9_PIN 3

// LED Configuration
#define NUM_LEDS (sizeof(MAIN_LED_BITMAP))
#define NUM_SECONDARY_LEDS (sizeof(SECONDARY_LED_BITMAP) / sizeof(SECONDARY_LED_BITMAP[0]))

// Pin for main LED array (matrix)
#define MAIN_LED_DATA_PIN 9

// Pin for secondary LED array  
#define SECONDARY_LED_DATA_PIN 7

// Default LED brightness
#define DEFAULT_BRIGHTNESS 50

// Debug settings
#define DEBUG_MODE true 
#define DEBUG_LED_PIN 2
#define DEBUG_BUTTON_BLINK_DURATION 500 // ms

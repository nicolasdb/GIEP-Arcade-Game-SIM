#pragma once
#include <Arduino.h>

// Bitmap for the secondary LED array (4x15)
const uint32_t SECONDARY_LED_BITMAP[60] = {
    // Simple alternating pattern
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    0xFF00FF, 0x00FFFF, 0xFFFFFF, 0x000000,
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    0xFF00FF, 0x00FFFF, 0xFFFFFF, 0x000000,
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    0xFF00FF, 0x00FFFF, 0xFFFFFF, 0x000000,
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    0xFF00FF, 0x00FFFF, 0xFFFFFF, 0x000000,
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    0xFF00FF, 0x00FFFF, 0xFFFFFF, 0x000000,
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    0xFF00FF, 0x00FFFF, 0xFFFFFF, 0x000000,
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00,
    0xFF00FF, 0x00FFFF, 0xFFFFFF, 0x000000,
    0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00
};
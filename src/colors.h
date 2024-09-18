#include <Arduino.h>

// Consolidated color definitions
enum ZoneColors {
    AIR = 0xFFFFFF,             // asigned for Air on MAIN_MATRIX
    BLOCK = 0x000000,           // asigned to block rain on MAIN_MATRIX
    GIEP_1 = 0x00FF00,          // asigned for GIEP1 zones MAIN_MATRIX and SEC_MATRIX, and button1
    GIEP_2 = 0x00E300,          // asigned for GIEP2 zones MAIN_MATRIX and SEC_MATRIX, and button2
    GIEP_3 = 0x00C700,          // asigned for GIEP3 zones MAIN_MATRIX and SEC_MATRIX, and button3
    GIEP_4 = 0x00AB00,          // asigned for GIEP4 zones MAIN_MATRIX and SEC_MATRIX, and button4
    GIEP_5 = 0x008F00,          // asigned for GIEP5 zones MAIN_MATRIX and SEC_MATRIX, and button5
    GIEP_6 = 0x007300,          // asigned for GIEP6 zones MAIN_MATRIX and SEC_MATRIX, and button6
    GIEP_7 = 0x005700,          // asigned for GIEP7 zones MAIN_MATRIX and SEC_MATRIX, and button7
    GIEP_8 = 0x003200,          // asigned for GIEP8 zones MAIN_MATRIX and SEC_MATRIX, and button8
    SEWER = 0xFFFF00,           // asigned for Sewer on MAIN_MATRIX
    BASIN = 0x0000FF,           // asigned for Basin on MAIN_MATRIX
    BASIN_GATE = 0xFF0000,      // asigned for Basin_Gate zones MAIN_MATRIX and SEC_MATRIX, and button9
    OVERFLOW = 0xFF00FF,        // asigned for OverFlow on MAIN_MATRIX
    RIVER = 0x6400FF,           // asigned for River on MAIN_MATRIX
    RAIN1 = 0x0000E3,           // asigned for Rain Level 1 on SEC_MATRIX
    RAIN2 = 0x0000C7,           // asigned for Rain Level 2 on SEC_MATRIX
    RAIN3 = 0x0000AB,           // asigned for Rain Level 3 on SEC_MATRIX
    D_FLOOD = 0xC70000,         // asigned for Flood Death on SEC_MATRIX
    D_POLLUTION = 0xAB0000,     // asigned for Pollution Death on SEC_MATRIX
    WIN = 0x00FFFF,             // asigned for Win condition on SEC_MATRIX
};
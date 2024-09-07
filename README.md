# GIEP Arcade Game SIM

This project is a simulation of an arcade game for GIEP (Green Infrastructure and Educational Program). The game simulates a city's water management system during various rainfall intensities, showcasing the importance of green infrastructure in managing urban water flow.

## Features

- Main LED matrix display for visualizing the city and water flow
- Secondary LED array for additional visual feedback
- Interactive buttons for controlling GIEP devices
- Simulation of different rainfall intensities and water levels
- Game states including win and loss conditions
- Enhanced debugging capabilities
- Improved error handling and watchdog implementation

## Hardware Components

- ESP32 microcontroller
- WS2812B LED matrix (main display)
- WS2813 LED strip (secondary array)
- MCP23017 I/O expander for button inputs
- Various buttons for user interaction

## Software Components

- `main.cpp`: Main program entry point and task management
- `GameLogic`: Manages the game state and logic
- `Scene`: Handles the visual representation on the LED matrix
- `ButtonHandler`: Manages button inputs
- `MCP23017Handler`: Interfaces with the MCP23017 I/O expander
- `SecondaryLEDHandler`: Manages the secondary LED array
- `MatrixConfig`: Configures the LED matrix layout
- `StateTracker`: Tracks the overall system state
- `DebugLogger`: Provides logging functionality for debugging
- `config.h`: Contains hardware-specific configurations
- `game_config.h`: Contains game-specific configurations for easy adjustment

## Game Mechanics

1. The game simulates different rainfall intensities: idle, raining, heavy, and storm.
2. Players can activate GIEP devices using buttons to manage water flow.
3. Water levels in the sewer and basin are simulated and visualized.
4. Players can open/close the basin gate to transfer water between the sewer and basin.
5. The game ends in a win state if the player survives until the end of the storm, or in a loss state if the sewer overflows or the basin pollutes the river.

## Secondary LED Array

The secondary LED array provides additional visual feedback:

- 8 zones for GIEP device status
- 1 zone for basin gate status
- 3 zones for rainfall intensity levels
- 3 zones for end game states (flood death, pollution death, win)

## Building and Running

1. Ensure you have PlatformIO installed.
2. Clone this repository.
3. Open the project in PlatformIO.
4. Build and upload the project to your ESP32 board.

## Debugging

The project now includes enhanced debugging capabilities:
- Use the `DebugLogger` class to log messages at different levels (ERROR, WARN, INFO, DEBUG).
- Debug logs can be enabled or disabled using the DEBUG flag in the build configuration.
- The `config.h` file includes a DEBUG_PRINT macro for conditional debug output.

## Configuration

- Hardware-specific configurations are located in `config.h`.
- Game-specific parameters (e.g., timing, water levels, rain intensities) are now separated into `game_config.h` for easier adjustment.

## Failsafe Mechanisms

- Watchdog timer implementation to detect and recover from system hangs.
- Improved error handling in task creation and hardware initialization.

## Contributing

Contributions to improve the game or add new features are welcome. Please submit a pull request with your changes.

## License

This project is licensed under the MIT License. See the LICENSE file for details.
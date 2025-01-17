# GIEP Arcade Game SIM

This arcade game is designed to raise awareness about the importance of effective rainwater management in urban areas, specifically in the context of the Integrated Management of Rainwater ([GIEP: Gestion intégrée des eaux pluviales](https://environnement.brussels/pro/agenda/gestion-integree-des-eaux-pluviales-et-vegetalisation-des-giep)) program in Brussels. 
GIEP aims to manage rainwater in a sustainable and integrated way, reducing flooding, improving water quality, and promoting urban biodiversity. The game highlights the challenges of conventional rainwater management, where urbanization and climate change lead to increased flooding, water pollution, and soil degradation. By simulating the impact of different rainwater management strategies, the game encourages players to think creatively about solutions that prioritize infiltration, reuse, and natural drainage, ultimately promoting a more ecological approach to urban water management.

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

## Game Logic

The game progresses through the following states:

1. Waiting Phase:
   - The game cycles between WAITING_RAINING and WAITING_DRY states.
   - This phase serves as an attract mode to draw players' attention.

2. Active Gameplay:
   - Starts when a player presses any button during the waiting phase.
   - Progresses through RAINING, HEAVY, and STORM weather conditions.
   - Each weather condition has a specific duration and rain intensity.

3. Player Interactions:
   - Players can activate/deactivate 8 GIEP (Green Infrastructure Element) buttons.
   - Each active GIEP reduces the rate at which the sewer level increases.
   - Players can also control a basin gate to transfer water from the sewer to the basin.

4. Water Management:
   - The sewer level increases with rainfall and decreases with GIEP effects and natural drainage.
   - The basin level increases when the basin gate is open, transferring water from the sewer.
   - Players must balance the use of GIEPs and the basin gate to prevent overflow.

5. Win Condition:
   - Achieved when both sewer and basin levels are below a certain threshold after a storm.

6. Lose Conditions:
   - FLOOD: Occurs when the sewer level reaches its overflow threshold.
   - CANAL: Occurs when the basin level reaches its overflow threshold, polluting the river.

7. Game Over:
   - After a win or lose condition is met, the game displays the result for a set duration.
   - The game then automatically returns to the waiting phase, ready for the next player.

The game's difficulty progressively increases with each weather state, challenging players to use strategy and quick decision-making to manage the water levels effectively.

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
# GIEP Arcade Game Framework

This project provides a framework for creating arcade-style games using a custom hardware setup with LED matrices and buttons.

## Project Structure

- `src/`: Contains the main source code files
  - `main.cpp`: Main program logic
  - `config.h`: Hardware and general configuration
  - `game_config.h`: Game-specific configuration
  - Other source files for various components (ButtonHandler, Scene, etc.)
- `include/`: Header files and additional resources
  - `array1.h`: Bitmap array for the main LED matrix
  - `array2.h`: Bitmap array for the secondary LED array
- `lib/`: External libraries
- `platformio.ini`: PlatformIO configuration file

## Key Components

- `ButtonHandler`: Manages button inputs
- `MCP23017Handler`: Handles communication with the MCP23017 I/O expander
- `MatrixConfig`: Configures the LED matrix
- `Scene`: Manages the game scene and rendering
- `SecondaryLEDHandler`: Controls secondary LED arrays

## Creating a New Game

1. Update the bitmap arrays in `include/array1.h` and `include/array2.h` with your game-specific LED configurations.
2. Modify `src/game_config.h` with game-specific settings and parameters.
3. Implement game logic in the `updateGameLogic()` function in `src/main.cpp`.
4. Update LED displays in the `updateLEDDisplay()` function in `src/main.cpp`.
5. Customize button handling in the `handleButtonPress()` and `handleButtonRelease()` functions.

## Setup Instructions

1. Install PlatformIO in your development environment.
2. Clone this repository.
3. Open the project in PlatformIO.
4. Connect your hardware setup to your computer.
5. Build and upload the project to your board using PlatformIO.

## Building and Flashing

1. Open the project in PlatformIO.
2. Click the "Build" button to compile the project.
3. Click the "Upload" button to flash the firmware to your board.

## Customizing LED Configurations

- Edit `include/array1.h` to change the main LED matrix configuration.
- Edit `include/array2.h` to change the secondary LED array configuration.
- Use the GIMP image editor and the h2array.py script to generate new bitmap arrays.

## Contributing

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Make your changes and commit them with clear, concise commit messages.
4. Push your changes and create a pull request.

## License

[Specify your license here]

## Contact

[Your contact information or project maintainer's contact]
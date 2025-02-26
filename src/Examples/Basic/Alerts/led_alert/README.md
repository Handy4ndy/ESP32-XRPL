# Basic LED Alert Example

This script demonstrates how to use a three-color LED for different alerts.

## Features

- Cycles through various colors.
- Flashes SOS in Morse code.
- Creates a rainbow effect for 20 seconds.

## Hardware Requirements

- ESP32 or compatible microcontroller.
- Three-color LED (RGB LED).

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.

## Installation

1. Open the `led_alert.ino` file in the Arduino IDE.
2. Install the required libraries:
   - None (uses built-in Arduino functions).
3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Connect the three-color LED to the specified pins on your ESP32.
2. Power on your ESP32 or compatible microcontroller.
3. The device will cycle through various colors, flash SOS in Morse code, and create a rainbow effect.

## Functions

- `setColor(int red, int green, int blue)`: Sets the color of the three-color LED.
- `cycleColors()`: Cycles through various colors.
- `flashSOS()`: Flashes SOS in Morse code.
- `rainbowEffect(int duration)`: Creates a rainbow effect for the specified duration.

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

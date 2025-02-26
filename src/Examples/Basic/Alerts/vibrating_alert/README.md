# Basic Vibrating Alert Example

This script demonstrates how to use vibrating alerts for different events using a vibration motor.

## Features

- Plays different vibration patterns for various events.
- Uses a vibration motor connected to the ESP32.

## Hardware Requirements

- ESP32 or compatible microcontroller.
- Vibration motor.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.

## Installation

1. Open the `vibrating_alert.ino` file in the Arduino IDE.
2. Install the required libraries:
   - None (uses built-in Arduino functions).
3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Connect a vibration motor to the specified pin on your ESP32.
2. Power on your ESP32 or compatible microcontroller.
3. The device will play different vibration patterns for various events.

## Functions

- `vibratePattern1()`: Plays the first vibration pattern.
- `vibratePattern2()`: Plays the second vibration pattern.
- `vibrateHighScorePattern()`: Plays the high score vibration pattern.
- `playVibrationPattern(int* pattern, int size)`: Plays a vibration pattern based on the provided durations and size.

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

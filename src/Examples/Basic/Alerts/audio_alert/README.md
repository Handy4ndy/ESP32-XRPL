# Basic Audio Alert Example

This script demonstrates how to use audio alerts for different events using a speaker or buzzer.

## Features

- Plays different tunes for various events.
- Uses a speaker or buzzer connected to the ESP32.

## Hardware Requirements

- ESP32 or compatible microcontroller.
- Speaker or buzzer.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.

## Installation

1. Open the `audio_alert.ino` file in the Arduino IDE.
2. Install the required libraries:
   - None (uses built-in Arduino functions).
3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Connect a speaker or buzzer to the specified pin on your ESP32. 
   (The Micro_ledger speaker is assigned to pin 3)
2. Power on your ESP32 or compatible microcontroller.
3. The device will play different tunes for various events.

## Functions

- `playTune1()`: Plays the first tune.
- `playTune2()`: Plays the second tune.
- `playHighScoreTune()`: Plays the high score tune.
- `playMelody(int* melody, int size)`: Plays a melody based on the provided notes and size.

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

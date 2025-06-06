# Display Loader Example

This script demonstrates how to display a loading screen with a glitch effect on an OLED display using the Adafruit SSD1306 library. It switches between displaying the XRPL and Xahau logos, which are stored in separate header files.

## Features

- Displays a loading screen with a glitch effect.
- Switches between displaying the XRPL and Xahau logos.
- Uses separate header files for the logo bitmaps.

## Hardware Requirements

- ESP32 or compatible microcontroller with I2C capability.
- OLED display (SSD1306).

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- Adafruit GFX library.
- Adafruit SSD1306 library.

## Installation

1. Open the `display_loader_header.ino` file in the Arduino IDE.
2. Install the required libraries:
   - Adafruit GFX
   - Adafruit SSD1306
3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Power on your ESP32 or compatible microcontroller.
2. The OLED display will show a loading screen with a glitch effect.
3. The display will switch between the XRPL and Xahau logos every 2 seconds.

## Header Files

- `XRPL_Logo.h`: Contains the bitmap data for the XRPL logo.
- `Xahau_Logo.h`: Contains the bitmap data for the Xahau logo.

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

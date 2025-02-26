# ESP32 XRPL Project

This project consists of multiple scripts to connect to XRPL / Xahau WebSocket's and retrieve information. Each script has its own functionality, ranging from monitoring transactions to displaying data on an OLED screen.

## Features

- Connects to a specified WiFi network.
- Monitors XRPL / Xahau transactions and account activities.
- Displays data on an OLED screen.
- Provides audio-visual feedback using an LED and a speaker.
- Handles WebSocket connections and reconnections.

## Hardware Requirements

- ESP32 or compatible microcontroller with WiFi capability.
- OLED display (SSD1306).
- LED.
- Speaker or buzzer.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- ArduinoJson library.
- WebSockets library.
- Adafruit GFX and SSD1306 libraries.

## Installation

1. Clone this repository or download the ZIP file and extract it.
2. Open the desired `.ino` file in the Arduino IDE.
3. Install the required libraries:
   - ArduinoJson
   - WebSockets
   - Adafruit GFX
   - Adafruit SSD1306
4. Configure your WiFi credentials and other parameters in the `.ino` file.
5. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Power on your ESP32 or compatible microcontroller.
2. The device will connect to the specified WiFi network.
3. Once connected, it will start performing its designated function based on the script.
4. The OLED display, LED, and speaker will provide feedback based on the operations performed.

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

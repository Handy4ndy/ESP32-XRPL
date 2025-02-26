# Ledger Stream Example

This script demonstrates how to connect to the XRPL WebSocket server, subscribe to the ledger stream, and display the ledger sequence and transaction count on an OLED screen.

## Features

- Connects to a specified WiFi network using WiFiManager.
- Subscribes to the XRPL ledger stream.
- Displays the ledger sequence and transaction count on an OLED screen.
- Handles WebSocket reconnections.

## Hardware Requirements

- ESP32 or compatible microcontroller with WiFi capability.
- OLED display (SSD1306).

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- ArduinoJson library.
- WebSockets library.
- Adafruit GFX and SSD1306 libraries.
- WiFiManager library.

## Installation

1. Open the `ledger_stream.ino` file in the Arduino IDE.
2. Install the required libraries:
   - ArduinoJson
   - WebSockets
   - Adafruit GFX
   - Adafruit SSD1306
   - WiFiManager
3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Power on your ESP32 or compatible microcontroller.
2. The device will attempt to connect to the specified WiFi network using WiFiManager.
3. Once connected, it will connect to the XRPL WebSocket server and subscribe to the ledger stream.
4. The OLED display will show the ledger sequence and transaction count.

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

# WiFi Setup

This script demonstrates how to connect an ESP32 to a specified WiFi network using hardcoded credentials. It also includes functionality to simulate a disconnection and handle reconnections.

## Features

- Connects to a specified WiFi network using hardcoded SSID and password.
- Simulates a disconnection after 5 seconds.
- Automatically attempts to reconnect to the WiFi network if disconnected.

## Hardware Requirements

- ESP32 or compatible microcontroller with WiFi capability.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.

## Installation

1. Open the `wifi_connect.ino` file in the Arduino IDE.
2. Configure your WiFi credentials in the `wifi_connect.ino` file:

   ```cpp
   const char* ssid = "Your_Network_SSID";
   const char* password = "Your_Network_Password";
   ```
3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Open the Serial Monitor in the Arduino IDE.
2. Power on your ESP32 or compatible microcontroller.
3. The device will attempt to connect to the specified WiFi network.
4. Once connected, it will simulate a disconnection after 5 seconds and then attempt to reconnect.
5. The Serial Monitor will display the connection status:

   ```
   Connecting to WiFi...
   Connected to WiFi
   Simulating disconnection in 5 seconds
   WiFi disconnected. Reconnecting...
   Reconnecting to WiFi...
   Reconnected to WiFi
   Simulating disconnection in 5 seconds
   WiFi disconnected. Reconnecting...
   Reconnecting to WiFi...
   Reconnected to WiFi
   ```

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

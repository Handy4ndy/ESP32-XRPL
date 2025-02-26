# XRPL WebSocket Connection Example

This script demonstrates how to connect to the XRPL WebSocket server and log ledger updates in the serial monitor.

## Features

- Connects to a specified WiFi network.
- Connects to the XRPL WebSocket server.
- Subscribes to the ledger stream and logs ledger updates in the serial monitor.
- Handles WebSocket reconnections.

## Hardware Requirements

- ESP32 or compatible microcontroller with WiFi capability.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- ArduinoJson library.
- WebSockets library.

## Installation

1. Open the `xrpl_websocket.ino` file in the Arduino IDE.

2. Install the required libraries:
   - ArduinoJson
   - WebSockets

3. Configue the hardcoded websocket_host and enter your network details

```ccp

// XRPL WebSocket server address

const char* websocket_host = "s2.ripple.com"; // XRPL - "s2.ripple.com", "xrplcluster.com" || Xahau - "xahau.network", "xahau.org"
const uint16_t websocket_port = 443;
const char* websocket_url = "/";

// Replace with your network credentials
const char* ssid = "Your_Network_SSID";
const char* password = "Your_Network_Password";

```

4. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Open the Serial Monitor in the Arduino IDE.
2. Power on your ESP32 or compatible microcontroller.
3. The device will attempt to connect to the specified WiFi network.
4. Once connected, it will connect to the XRPL WebSocket server and subscribe to the ledger stream.
5. The Serial Monitor will display the connection status and ledger updates:

```
Connecting to WiFi...
Connected to WiFi
WebSocket Connected
Update received from: s2.ripple.com
Ledger Sequence: 12345678
Total Transactions: 123
```

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

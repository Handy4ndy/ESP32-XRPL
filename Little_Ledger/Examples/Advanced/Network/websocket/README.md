# XRPL WebSocket Connection Example with WiFiManager

This script demonstrates how to connect to the XRPL WebSocket server using WiFiManager and a custom WebSocket host parameter. It logs ledger updates in the serial monitor.

## Features

- Connects to a specified WiFi network using WiFiManager.
- Allows entry of a custom WebSocket host parameter.
- Connect to XRPL or Xahau WebSocket servers.
- Subscribes to the ledger stream and logs ledger updates in the serial monitor.
- Handles WebSocket reconnections.

## Hardware Requirements

- ESP32 or compatible microcontroller with WiFi capability.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- ArduinoJson library.
- WebSockets library.
- WiFiManager library.

## Installation

1. Open the `custom_websocket.ino` file in the Arduino IDE.

2. Install the required libraries:
  - WebSocketsClient - WebSockets_Generic by khoih-prog
  - ArduinoJson - ArduinoJson by Benoit Blanchon
  - WiFiManager - WiFiManager by tzapu

3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Open the Serial Monitor in the Arduino IDE.
2. Power on your ESP32 or compatible microcontroller.
3. The device will attempt to connect to the specified WiFi network using WiFiManager.
4. If the connection fails, it will start an access point with the SSID "WiFi_Manager" and a web portal for configuration.
5. Enter the custom WebSocket host parameter if needed.
6. Once connected, it will connect to the XRPL WebSocket server and subscribe to the ledger stream.
7. The Serial Monitor will display the connection status and ledger updates.

### Custom Parameter Entry

- If a custom WebSocket host parameter is entered, it will be used for the WebSocket connection.
- If no custom WebSocket host parameter is entered, a hardcoded default host (`s2.ripple.com`) will be used.
- This custom host parameter allows switching between XRPL and Xahau with ease.

#### Example Logs

**With Custom Host:**

```
Connected to WiFi
Custom host was entered, custom host used: xahau.org
WebSocket Connected
Update received from: xahau.org
Ledger Sequence: 12311895
Total Transactions: 212
```

**Without Custom Host:**

```
Connected to WiFi
No custom host was added, hardcoded parameter used: s2.ripple.com
WebSocket Connected
Update received from: s2.ripple.com
Ledger Sequence: 94371956
Total Transactions: 45
```

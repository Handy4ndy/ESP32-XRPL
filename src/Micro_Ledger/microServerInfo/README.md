# Micro Ledger - Real-Time Blockchain Monitor

The Micro Ledger is a portable, wearable IoT device designed to provide real-time monitoring of blockchain networks, specifically the Xahau network and XRP Ledger. It uses an ESP32 microcontroller (Seeed Xiao ESP32 S3) to connect to a WiFi network and establish a WebSocket connection to a server, where it subscribes to the "ledger" stream to receive updates on new ledgers. Upon receiving a ledger update, it requests server information and displays key metrics on a 0.96" OLED screen. The device also features alerts for high server latency using a haptic motor, visual feedback, and optional audio notifications.

## How It Works

### Ledger Stream and Server_info Requests

The device establishes a WebSocket connection to the specified server (default: `xahau.network`) and subscribes to the "ledger" stream by sending:

```json
{"command": "subscribe", "streams": ["ledger"]}
```

When a new ledger is published (indicated by a message containing `"ledger_index"`), the device sends a `{"command": "server_info"}` request to fetch the current server status. The server responds with a JSON object containing metrics such as:

- `hostid`: A unique identifier for the server.
- `io_latency_ms`: The current I/O latency in milliseconds.
- `uptime`: The server's uptime in seconds.
- `build_version`: The software version of the server.
- `complete_ledgers`: The range or status of complete ledgers.
- `load_factor`: A floating-point value indicating the server's load.
- `peers`: The number of connected peers.

These metrics are parsed using the `ArduinoJson` library and displayed on the OLED screen.

### Displayed Data

The OLED display (SSD1306) alternates between two pages every 8 seconds, showing:

**Page 0:**
- WebSocket Host
- Host ID
- Build Version
- Complete Ledgers

**Page 1:**
- WebSocket Host
- Uptime
- I/O Latency
- Load Factor
- Peers

### Alerts

If the I/O latency (`io_latency_ms`) exceeds the `LATENCY_THRESHOLD` (default: 5 ms), the device triggers an alert:
- The haptic motor on GPIO8 is activated.
- The OLED display flashes "HIGH LATENCY" for 5 seconds (`ALERT_DURATION`).
- Alerts are rate-limited to once every 60 seconds (`ALERT_TIMEOUT`).

## Setup Requirements

- **Hardware**: Seeed Xiao ESP32 S3, SSD1306 0.96" OLED display, haptic motor (GPIO 09), button (GPIO 10), optional buzzer (e.g., GPIO 08). 
- **Software**: Arduino IDE with ESP32 board support, libraries (WiFi, WebSocketsClient, ArduinoJson, Adafruit_GFX, Adafruit_SSD1306).
- **Tools**: USB cable for programming, 2.4GHz WiFi network.

## Getting Started

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) and add ESP32 board support.
2. Install required libraries via the Arduino Library Manager.
3. Connect the Micro Ledger to your computer and upload the code from the project repository.
4. Configure settings as described below.

## Configuration

### Adjusting WiFi Settings

To connect the device to your WiFi network, modify the `connection.cpp` file:

1. Open `connection.cpp`.
2. Locate the following lines:
   ```cpp
   namespace {
     const char* ssid = "YOUR_NETWORK_SSID";
     const char* password = "YOUR_NETWORK_SSID";
   }
   ```
3. Replace `"YOUR_NETWORK_SSID"` with your WiFi SSID and `"YOUR_NETWORK_SSID"` with your WiFi password.
4. Save and re-upload the code to the device.

**Example**:
```cpp
namespace {
  const char* ssid = "MyHomeWiFi";
  const char* password = "MyPassword123";
}
```

### Adjusting WebSocket Settings

To connect to a different WebSocket server, modify the `microServerInfo.ino` file:

1. Open `microServerInfo.ino`.
2. Locate the following lines:
   ```cpp
   const char* websocket_host = "xahau.network";
   const uint16_t websocket_port = 443;
   const char* websocket_url = "/";
   ```
3. Update `websocket_host`, `websocket_port`, and `websocket_url` as needed.
4. Save and re-upload the code to the device.

**Example**:
```cpp
const char* websocket_host = "xahau.org"; // XRPL - "s2.ripple.com", "xrplcluster.com" || Xahau - "xahau.network", "xahau.org"
const uint16_t websocket_port = 443;
const char* websocket_url = "/";
```

### Adjusting Alert Settings

To customize the alert behavior, modify the following constants in `microServerInfo.ino`:

1. Locate the constants:
   ```cpp
   const int LATENCY_THRESHOLD = 5; // milliseconds
   const unsigned long ALERT_DURATION = 5000; // 5 seconds
   const unsigned long ALERT_TIMEOUT = 60000; // 60 seconds
   ```
2. Adjust:
   - `LATENCY_THRESHOLD`: Set the latency trigger (e.g., `10` for 10ms).
   - `ALERT_DURATION`: Set alert duration (e.g., `3000` for 3 seconds).
   - `ALERT_TIMEOUT`: Set minimum time between alerts (e.g., `30000` for 30 seconds).
3. Save and re-upload the code.

**Example**:
```cpp
const int LATENCY_THRESHOLD = 10; // milliseconds
const unsigned long ALERT_DURATION = 3000; // 3 seconds
const unsigned long ALERT_TIMEOUT = 30000; // 30 seconds
```

## Adding Audio Notifications

To add audio notifications, connect a buzzer to a GPIO pin (e.g., GPIO10, as GPIO8 is used for the haptic motor):

1. Connect the buzzer’s positive leg to GPIO10 and the negative leg to GND.
2. In `microServerInfo.ino`, define a new constant:
   ```cpp
   const int AUDIO_PIN = 10;
   ```
3. In the `setup` function, configure the pin:
   ```cpp
   pinMode(AUDIO_PIN, OUTPUT);
   ```
4. In the `alertSimulation` function, add:
   ```cpp
   digitalWrite(AUDIO_PIN, HIGH);
   delay(100);
   digitalWrite(AUDIO_PIN, LOW);
   ```
   Adjust the delay for different sound patterns.
5. Save and re-upload the code.

## Additional Notes

- The device uses deep sleep mode to conserve power, triggered by a long press on the button (GPIO9).
- Ensure your WiFi network is 2.4GHz, as many ESP32 devices do not support 5GHz networks.
- For debugging, open the Serial Monitor in the Arduino IDE at 9600 baud to view connection and alert logs.
- Verify pin assignments (haptic motor on GPIO 09, button on GPIO 10, buzzer on GPIO 08) if alerts or inputs fail.
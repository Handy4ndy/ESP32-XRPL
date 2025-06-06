# Micro Ledger - Real-Time XRP Price Monitor

The **Micro Ledger** is a portable IoT device built on the Seeed Xiao ESP32 S3 microcontroller. It connects to the Xahau network via a WebSocket server to monitor the XRP price in RLUSD in real-time. The device subscribes to ledger updates, fetches book offers to determine the current price, and displays key metrics on a 0.96" OLED screen. It tracks high and low prices over 5-minute windows, calculates percentage moves, and retains an all-time high score in non-volatile memory. Additional features include button controls for reset and power management, plus haptic feedback.

---

## How It Works

### Ledger Stream and Price Fetching

The device establishes a WebSocket connection to a server (default: `xrplcluster.com`) and subscribes to the "ledger" stream with:

```json
{"command": "subscribe", "streams": ["ledger"]}
```

Upon receiving a ledger update (identified by a `"ledger_index"` in the response), it requests the current XRP/RLUSD price via a `book_offers` command:

```json
{"command": "book_offers", "taker_gets": {"currency": "XRP"}, "taker_pays": {"currency": "524C555344000000000000000000000000000000", "issuer": "rMxCKbEDwqr76QuheSUMdEGf4B9xJ8m5De"}, "limit": 1}
```

The response provides the order book, from which the device extracts the `quality` of the first offer, converts it to RLUSD per XRP, and updates the display using the `ArduinoJson` library.

### Price Tracking

The device monitors:
- **Current Price**: Updated with each ledger.
- **5-Minute High/Low**: Tracks the highest and lowest prices within a 5-minute window.
- **Percentage Move**: Calculated at the end of each 5-minute window as `((high - low) / low) * 100`, rounded to two decimal places.
- **All-Time High**: Stored persistently and updated when surpassed.

### Displayed Data

The OLED screen (SSD1306) shows:
- **Current Price**: Latest XRP price in RLUSD (e.g., `$0.123456`).
- **Previous 5-Minute High**: Highest price from the last 5-minute window.
- **Previous 5-Minute Low**: Lowest price from the last 5-minute window.
- **Previous 5-Minute % Move**: Percentage change from the last window.
- **High Score**: All-time high price.

### Button Controls

- **Short Press (< 2s)**: Resets the device via `ESP.restart()`.
- **Long Press (≥ 2s)**: Toggles power state turns off WiFi and display to save power, or restarts if already off.

---

## Setup Requirements

### Hardware
- Seeed Xiao ESP32 S3
- SSD1306 0.96" OLED display (I2C, address 0x3C)
- Haptic motor (GPIO 8)
- Button (GPIO 9, RTC-capable)
- USB cable for programming
- 2.4GHz WiFi network

### Software
- Arduino IDE with ESP32 board support
- Libraries:
  - `WiFi`
  - `WebSocketsClient`
  - `ArduinoJson`
  - `Adafruit_GFX`
  - `Adafruit_SSD1306`
  - `Preferences`
  - `esp_sleep`
  - `WiFiManager`

---

## Getting Started

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) and add ESP32 board support.
2. Install the required libraries via the Arduino Library Manager.
3. Connect the ESP32 to your computer via USB.
4. Open the code in the Arduino IDE, configure settings (see below), and upload it to the device.

---

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

### WebSocket Settings
To change the WebSocket server, edit `microPriceInfo.ino`:
```cpp
const char* websocket_host = "xrplcluster.com";
const uint16_t websocket_port = 443;
const char* websocket_url = "/";
```
Modify these values (e.g., to `s2.ripple.com`), save, and re-upload the code.

---

## Additional Notes

- **Power Management**: Long-press the button (GPIO 9) to enter a low-power state (WiFi and display off). Another long press restarts the device.
- **Debugging**: Use the Serial Monitor at 9600 baud to view connection status, price updates, and errors.
- **Pin Assignments**: Haptic motor on GPIO 8, button on GPIO 9. Verify these match your wiring.
- **Boot Screen**: Displays a loading animation with the Xahau logo on startup.
- **Reconnection**: Automatically retries WiFi and WebSocket connections every 5 seconds if disconnected.

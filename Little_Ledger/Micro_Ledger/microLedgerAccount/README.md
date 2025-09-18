# Micro Ledger - Real-Time XRPL Account Monitor

The **Micro Ledger** is a portable IoT device powered by the Seeed Xiao ESP32 S3 microcontroller, designed to monitor a specific Xahau or XRP Ledger (XRPL) account in real-time. It connects to a WebSocket server (default: `xahau.org`) to track ledger updates and account transactions, displaying key metrics on a 0.96" OLED screen. The device provides visual alerts via an RGB LED, audio feedback through a speaker, and haptic feedback for specific transaction types like payments, trust lines, and offers. It also tracks the highest transaction count per ledger and supports button controls for resetting, viewing high scores, or entering sleep mode.

---

## How It Works

### Ledger and Account Subscription
The device establishes a secure WebSocket connection and subscribes to:
- **Ledger Stream**:
  ```json
  {"command": "subscribe", "streams": ["ledger"]}
  ```
- **Account Transactions** for a specified wallet address (e.g., `r3VekZUS7MfnkWiJ131TsGqmKZwvqAyisX`):
  ```json
  {"command": "subscribe", "accounts": ["r3VekZUS7MfnkWiJ131TsGqmKZwvqAyisX"]}
  ```

When a ledger closes (indicated by `"type": "ledgerClosed"`), it captures the ledger sequence and transaction count. For account-specific transactions, it processes details like transaction type, amount, and destination.

### Transaction Monitoring
The device monitors transactions involving the specified account, including:
- **Payments**: Detects incoming (positive alert) or outgoing (negative alert) payments, displaying the amount and destination/source.
- **TrustSet**: Identifies trust line additions (positive) or removals (negative, limit = 0).
- **OfferCreate**: Tracks new offer creation (positive).
- **OfferCancel**: Tracks offer cancellations (negative).

**Alerts**:
- **Visual**: RGB LED changes color based on transaction count:
  - Blue: ≤150 transactions
  - Green: <151–600 transactions
  - Red: ≥600 transactions
- **Audio**: Plays unique melodies for each transaction type (positive/negative) via a speaker on GPIO 7.
- **Haptic**: Activates feedback on GPIO 8 (configurable).

### Displayed Data
The OLED screen (SSD1306) shows:
- **Ledger Sequence**: Current ledger index.
- **Transaction Count**: Number of transactions in the current ledger.
- **High Score**: Highest transaction count recorded and its ledger index.
- **Transaction Visualization**: 2x2 pixel squares at the bottom, scaled by transaction count (smaller for >500 transactions).
- **Transaction Details**: For account events (e.g., payments), shows type, amount for 10 seconds before reverting to the main display.

### High Score Tracking
- Tracks the highest transaction count per ledger, stored in `Preferences` for non-volatile memory.
- Updates and saves the high score with its ledger sequence when surpassed, triggering a unique tune.

### Button Controls
- **Short Press (<2s)**: Displays the high score and its ledger index for 5 seconds, accompanied by a tune.
- **Medium Press (2–10s)**: Resets the device via `ESP.restart()`.
- **Long Press (>10s)**: Enters sleep mode, waking on button press (GPIO 9).

---

## Setup Requirements

### Hardware
- Seeed Xiao ESP32 S3
- SSD1306 0.96" OLED display (I2C, address 0x3C)
- Haptic motor (GPIO 8)
- Button (GPIO 9, RTC-capable)
- RGB LED (Red: GPIO 5, Green: GPIO 19, Blue: GPIO 18)
- Speaker (GPIO 7)
- USB cable for programming
- 2.4GHz WiFi network

### Software
- Arduino IDE with ESP32 board support
- Libraries:
  - `Adafruit_GFX`
  - `Adafruit_SSD1306`
  - `ArduinoJson`
  - `WebSocketsClient`
  - `WiFi`
  - `Preferences`
  - `pitches.h` (included for musical notes)

---

## Getting Started

1. Install the [Arduino IDE](https://www.arduino.cc/en/software) and add ESP32 board support.
2. Install the required libraries via the Arduino Library Manager.
3. Connect the ESP32 to your computer via USB.
4. Open the code in the Arduino IDE, configure settings (see below), and upload it to the device.

---

## Configuration

### WiFi Settings
Edit the WiFi credentials in the Connection.cpp file:
```cpp
const char* ssid = "YOUR_NETWORK_SSID";
const char* password = "YOUR_NETWORK_PASSWORD";
```
Replace with your 2.4GHz WiFi SSID and password, save, and re-upload.

**Example**:
```cpp
const char* ssid = "MyHomeWiFi";
const char* password = "MyPassword123";
```

### WebSocket Settings
To change the WebSocket server, modify in the main .ino file:
```cpp
const char* websocket_host = "xahau.org";
const uint16_t websocket_port = 443;
const char* websocket_url = "/";
```
Update to another server (e.g., `s2.ripple.com`), save, and re-upload.

**Example**:
```cpp
const char* websocket_host = "xrplcluster.com";
const uint16_t websocket_port = 443;
const char* websocket_url = "/";
```

### Wallet Address
Set the XRPL account to monitor:
```cpp
const char* walletAddress = "r3VekZUS7MfnkWiJ131TsGqmKZwvqAyisX";
```
Replace with your desired account address, save, and re-upload.

---

## Additional Notes

- **Power Management**: Long-press the button (>10s) to enter sleep mode, reducing power consumption. Press the button to wake.
- **Debugging**: Monitor logs in the Serial Monitor at 9600 baud for connection status, transaction details, and errors.
- **Pin Assignments**:
  - Haptic motor: GPIO 8
  - Button: GPIO 9
  - Speaker: GPIO 7
- **Reconnection**: Automatically retries WiFi and WebSocket connections every 4 seconds if disconnected.
- **High Score Persistence**: Stored in non-volatile memory, preserved across power cycles.

---

Enjoy real-time XRPL account monitoring with your Micro Ledger!

*Created by Handy_4ndy, licensed under the GNU Lesser General Public License v2.1 or later.*

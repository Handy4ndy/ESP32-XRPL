# XRPL Account Monitor

This project connects to a specified WiFi network and monitors an XRPL (XRP Ledger) account using WebSockets. It processes different types of transactions such as payments, trust lines, and offers, and provides audio-visual feedback using an LED and a speaker.

## Features

- Connects to a specified WiFi network using WiFiManager.
- Monitors an XRPL account for transactions.
- Processes different types of transactions: payments, trust lines, and offers.
- Provides audio-visual feedback using an LED and a speaker.
- Handles WebSocket connections and reconnections.

## Hardware Requirements

- ESP32 or compatible microcontroller with WiFi capability.
- LED.
- Speaker or buzzer.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- ArduinoJson library.
- WebSockets library.
- WiFiManager library.

## Installation

1. Open the `account_stream.ino` file in the Arduino IDE.
2. Install the required libraries:
   - ArduinoJson
   - WebSockets
   - WiFiManager
3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Power on your ESP32 or compatible microcontroller.
2. The device will attempt to connect to the specified WiFi network using WiFiManager.
3. Once connected, it will connect to the XRPL WebSocket server and subscribe to the specified account.
4. The LED and speaker will provide feedback based on the transactions processed.

## Functions

- `subscribeToAccount(const char* account)`: Subscribes to the specified XRPL account.
- `processTransaction(uint8_t* payload, size_t length)`: Processes the incoming transaction data.
- `getAmountValue(JsonVariant transaction)`: Extracts the amount value from the transaction.
- `performAction(const char* transactionType, bool positive)`: Performs an action based on the transaction type and positivity.
- `playMelody(int* melody, int size)`: Plays a melody based on the provided notes and size.

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

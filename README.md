# XRPL Account Monitor

XRPL Account Monitor is an Arduino project that connects to a specified WiFi network and monitors an XRPL (XRP Ledger) account using WebSockets. It processes different types of transactions such as payments, trust lines, and offers, and provides audio-visual feedback using an LED and a speaker.

## Features

- Connects to a specified WiFi network.
- Monitors an XRPL account using WebSockets.
- Processes different types of transactions:
  - Payments
  - Trust lines (TrustSet)
  - Offers (OfferCreate and OfferCancel)
- Provides audio-visual feedback using an LED and a speaker.

## Hardware Requirements

- ESP32 or compatible microcontroller with WiFi capability.
- LED.
- Speaker or buzzer.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- ArduinoJson library.
- WebSockets library.

## Wiring

- Connect the LED to pin 5.
- Connect the speaker to pin 4.

## Installation

1. Clone this repository or download the ZIP file and extract it.

2. Open the `xrplaccountmonitor.ino` file in the Arduino IDE.

3. Install the required libraries:
   - ArduinoJson
   - WebSockets

4. Configure your WiFi credentials and XRPL wallet address in the `xrplaccountmonitor.ino` file:

   ```cpp
   const char* ssid = "Your_SSID";
   const char* password = "Your_PASSWORD";
   const char* walletAddress = "Your_XRPL_Wallet_Address";
   ```

5. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Power on your ESP32 or compatible microcontroller.
2. The device will connect to the specified WiFi network.
3. Once connected, it will start monitoring the specified XRPL account.
4. The LED and speaker will provide feedback based on the transactions detected.

## Licence

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

# XRPL Account Monitor

XRPL Account Monitor is an ESP32 project that connects to a specified WiFi network and monitors an XRPL (XRP Ledger) rAddress using WebSockets. It processes different types of transactions such as payments, trust lines, and offers, providing audio-visual feedback using an LED and a speaker.

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
- Speaker.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- ArduinoJson library.
- WebSockets library.
- WiFiManager library.

## Wiring

- Connect the LED to pin 5.
- Connect the speaker to pin 4.
- Ground both connections.

## Installation

1. Clone this repository or download the ZIP file and extract it.

2. Open the `ledger_account_monitor.ino` file in the Arduino IDE.

3. Extract the files from the Library zip folder and add them to the arduino\libraries or Install the required libraries from the IDE:
   - WiFI
   - ArduinoJson
   - WebSockets
   - WiFiManager

4. Upload the code to your ESP32 or compatible microcontroller.
   ** Comment wifiManager.resetSettings(); to disable reset on reboot

5. To configure the Network Connection and rAddress, turn on the device and search for the Micro_Ledger wifi signal.
    - Configure WiFi
      - Network SSID
      - Network PASSWORD
      - rAddress 

## Usage

1. Power on your ESP32 or compatible microcontroller.
2. Connect the device to the specified WiFi network and assign the rAddress. ( Micro_Ledger )
3. Once connected, it will start monitoring the specified XRPL account.
4. The LED and speaker will provide feedback based on the transactions detected.

## Licence

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

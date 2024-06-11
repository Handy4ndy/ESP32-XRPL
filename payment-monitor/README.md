# XRPL Payment Monitor

The XRPL Payment Monitor is an Arduino project that connects an ESP32/ESP8266 device to a WiFi network and establishes a WebSocket connection to an XRPL (XRP Ledger) server. The primary goal is to monitor payment transactions for a specific account and provide feedback through an LED and a speaker.

## Dependencies

- [WiFi.h](https://www.arduino.cc/en/Reference/WiFi) - Arduino library for ESP32/ESP8266 WiFi support.
- [WebSocketsClient.h](https://github.com/Links2004/arduinoWebSockets) - Arduino library for WebSocket client support.
- [ArduinoJson.h](https://arduinojson.org/) - Arduino library for parsing and generating JSON data.

## Components

- **WiFi Connection**: The device connects to a specified WiFi network.
- **WebSocket Client**: Establishes a WebSocket connection to the XRPL server to receive real-time transaction updates.
- **JSON Parsing**: Uses the ArduinoJson library to parse transaction data received via WebSocket.
- **Payment Monitoring**: Subscribes to a specific XRPL account to monitor payment transactions.
- **Feedback Mechanism**:
  - If a payment is sent or received, it plays a positive melody and toggles an LED.

## Functions

- **setup()**: Initializes serial communication, connects to WiFi, sets up WebSocket, and configures pins.
- **loop()**: Keeps the WebSocket connection alive.
- **webSocketEvent()**: Handles different WebSocket events like connection, disconnection, receiving messages, etc.
- **subscribeToAccount()**: Subscribes to a specified XRPL account to receive transaction updates.
- **processTransaction()**: Parses and processes transaction data to identify payment transactions.
- **performAction()**: Toggles the LED and plays a positive melody when a payment is sent or received.

## Setup Instructions

1. Clone the repository.
2. Open the project in your Arduino IDE.
3. Install the following dependencies:
   - [WiFi.h](https://www.arduino.cc/en/Reference/WiFi)
   - [WebSocketsClient.h](https://github.com/Links2004/arduinoWebSockets)
   - [ArduinoJson.h](https://arduinojson.org/)
4. Update the following lines in the code with your network and XRPL account details:

    ```cpp
    const char* ssid = "your_network_SSID";      // Insert the network SSID you want to connect to
    const char* password = "your_network_password";  // Provide the password for the network
    const char* walletAddress = "your_account_address"; // Set the wallet address you wish to monitor
    ```

5. Upload the code to your ESP32/ESP8266 device.
6. Open the Serial Monitor to view the device's output.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

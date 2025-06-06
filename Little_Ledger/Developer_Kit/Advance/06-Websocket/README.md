# Tutorial 06: WebSocket

## Overview

This tutorial is part of the ESP32-XRPL Developer Kit and demonstrates how to connect to a WebSocket server using hardcoded WiFi credentials. The script subscribes to the XRPL ledger stream, logs updates in the Serial Monitor, and displays connection status and received data on an OLED screen. A button is included to reset the device.

---

## Objectives

- Learn how to connect to a WebSocket server using the ESP32.
- Understand how to parse JSON messages from the WebSocket server.
- Display connection status and received data on an OLED screen.
- Use a button to reset the ESP32.
- Practice modular programming by handling WebSocket events and reconnections.

---

## Components Required

- ESP32 Microcontroller (NodeMCU-32S)
- 0.96" OLED Display (SSD1306)
- Tactile Button
- Breadboard
- Jumper Wires

---

## Circuit Diagram

1. Connect the OLED display to the I2C pins:
   - **SDA** to GPIO21.
   - **SCL** to GPIO22.
2. Connect one leg of the button to GPIO5 and the other leg to GND.
3. Ensure the ESP32 is powered via USB or an external power source.

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/Developer_Kit.png)

---

## How to Use

1. **Upload the Code**:
   - Open the `06-Websocket.ino` file in the Arduino IDE.
   - Replace the placeholder WiFi credentials (`Your_Network_SSID` and `Your_Network_Password`) with your actual WiFi network credentials.
   - Select the correct board and port under `Tools`.
   - Click the upload button.

2. **Observe the OLED Display**:
   - The OLED display will show the WiFi and WebSocket connection status.
   - Once connected, it will display updates from the WebSocket server, including the ledger sequence and transaction count.

3. **Monitor the Serial Output**:
   - Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`).
   - Set the baud rate to `9600`.
   - Observe the messages indicating the WebSocket connection status and ledger updates.

4. **Test the Reset Button**:
   - Press the button connected to GPIO5 to reset the ESP32.
   - The OLED display will show a "Resetting device..." message before restarting.

---

## Code Explanation

### Key Features

- **WiFi Connection**:
  - The script uses the `WiFi.h` library to connect to a specified WiFi network.
  - It handles disconnections and attempts to reconnect automatically.

- **WebSocket Connection**:
  - The script uses the `WebSocketsClient` library to connect to a WebSocket server.
  - It subscribes to the XRPL ledger stream and logs updates in the Serial Monitor.

- **OLED Display**:
  - The OLED display is initialized using the `Adafruit_SSD1306` library.
  - It shows the WiFi and WebSocket connection status and received data.

- **Button for Reset**:
  - A button connected to GPIO5 is used to reset the ESP32.
  - The OLED display shows a "Resetting device..." message before restarting.

- **Non-Blocking Logic**:
  - The script uses `millis()` to periodically check the WiFi and WebSocket connection status without blocking other tasks.

---

## Challenge Section

- **Integrate WiFiManager**:
  - Replace the hardcoded WiFi credentials with the `WiFiManager` library.
  - Add a custom parameter for the WebSocket host using `WiFiManagerParameter`.
  - Save the custom parameter and use it to connect to the WebSocket server.

- **Enhance the Display**:
  - Modify the code to display additional information from the ledger stream, such as:
    - Ledger close time.
    - Total fees collected.
    - Other relevant fields available in the JSON response.
  - Format the display to show this information clearly.

- **Customize the Transaction Count Display**:
  - Change how the transaction count is displayed on the OLED screen.
  - For example, use a bar graph or percentage to represent the transaction count visually.

These challenges will help you practice using WiFiManager, improve your understanding of JSON parsing, and enhance the user experience with visual feedback.

---

## Next Steps

- Once you complete this tutorial, explore the Advanced tutorials to learn about integrating WebSocket communication with alerts and dynamic displays.
- Experiment with combining WebSocket data and OLED graphics to create interactive IoT applications.

---

Happy coding! 🚀

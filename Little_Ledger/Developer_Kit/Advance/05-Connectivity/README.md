# Tutorial 05: Connectivity

## Overview

This tutorial is part of the ESP32-XRPL Developer Kit and demonstrates how to connect the ESP32 to a WiFi network. The script includes functionality to handle disconnections and reconnections, and it displays the connection status on an OLED screen.

---

## Objectives

- Learn how to connect the ESP32 to a WiFi network.
- Understand how to handle WiFi disconnections and reconnections.
- Display connection status and IP address on an OLED screen.
- Use the Serial Monitor to debug and monitor the connection process.

---

## Components Required

- ESP32 Microcontroller (NodeMCU-32S)
- 0.96" OLED Display (SSD1306)
- Breadboard
- Jumper Wires

---

## Circuit Diagram

1. Connect the OLED display to the I2C pins:
   - **SDA** to GPIO21.
   - **SCL** to GPIO22.
2. Ensure the ESP32 is powered via USB or an external power source.

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/Developer_Kit.png)

---

## How to Use

1. **Upload the Code**:
   - Open the `05-Connectivity.ino` file in the Arduino IDE.
   - Replace the placeholder WiFi credentials (`Your_Network_SSID` and `Your_Network_Password`) with your actual WiFi network credentials.
   - Select the correct board and port under `Tools`.
   - Click the upload button.

2. **Observe the OLED Display**:
   - The OLED display will show the connection status:
     - "Attempting Connection" while connecting.
     - "Connected to [SSID]" and the IP address upon successful connection.

3. **Monitor the Serial Output**:
   - Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`).
   - Set the baud rate to `9600`.
   - Observe the messages indicating the connection status and IP address.

4. **Test Disconnection Handling**:
   - Disconnect the WiFi network or turn off the router.
   - Observe the reconnection attempts on the OLED display and Serial Monitor.

---

## Code Explanation

### Key Features

- **WiFi Connection**:
  - The script uses the `WiFi.h` library to connect to a specified WiFi network.
  - It handles disconnections and attempts to reconnect automatically.

- **OLED Display**:
  - The OLED display is initialized using the `Adafruit_SSD1306` library.
  - It shows the connection status and IP address.

- **Non-Blocking Logic**:
  - The script uses `millis()` to periodically check the WiFi status without blocking other tasks.

- **Serial Monitoring**:
  - The Serial Monitor displays connection status and debugging messages.

---

## Challenge Section

- **Use WiFiManager to Add a Custom Variable**:
  - Modify the code to use the `WiFiManager` library for managing WiFi connections.
  - Add a custom variable (e.g., a device name or custom parameter) using `WiFiManagerParameter`.
  - Save the custom variable and print it to the Serial Monitor after a successful connection.

- **Incorporate Connection Display Logic into WiFiManager**:
  - Update the OLED display to show the connection status during the WiFiManager setup process.
  - Display messages such as "Connecting to WiFi" and "Connected to [SSID]" along with the IP address.

These challenges will help you practice using WiFiManager, improve your understanding of dynamic WiFi configuration, and enhance the user experience with visual feedback.

---

## Next Steps

- Once you complete this tutorial, proceed to the next Advanced tutorial to explore WebSocket communication.
- Experiment with combining connectivity, display, and alerts to create interactive IoT applications.

---

Happy coding! 🚀

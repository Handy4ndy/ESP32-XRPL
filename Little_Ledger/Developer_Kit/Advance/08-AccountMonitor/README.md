# Tutorial 08: Account Monitor

## Overview

This tutorial demonstrates how to monitor an XRPL account using a WebSocket connection. The program logs account-specific transactions in the Serial Monitor and displays relevant data on an OLED screen. Alerts are provided via an RGB LED and speaker for specific transaction types.

---

## Objectives

- Learn how to monitor an XRPL account using a WebSocket connection.
- Display account-specific transactions on an OLED screen.
- Provide visual and audio alerts for transactions such as payments, trust lines, and offers.
- Track and display the highest transaction count (high score).

---

## Components Required

- ESP32 Microcontroller (NodeMCU-32S)
- 0.96" OLED Display (SSD1306)
- RGB LED (HW-479, with built-in resistors)
- Tactile Button
- Speaker or Buzzer
- Breadboard
- Jumper Wires

---

## Circuit Diagram

1. Connect the OLED display to the I2C pins:
   - **SDA** to GPIO21.
   - **SCL** to GPIO22.
2. Connect the RGB LED:
   - **RED** to GPIO5.
   - **GREEN** to GPIO19.
   - **BLUE** to GPIO18.
3. Connect the speaker or buzzer to GPIO17 and GND.
4. Connect one leg of the button to GPIO4 and the other leg to GND.

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/Developer_Kit.png)

---

## How to Use

1. **Upload the Code**:
   - Open the `08-AccountMonitor.ino` file in the Arduino IDE.
   - Replace `YOUR_NETWORK_SSID` and `YOUR_NETWORK_PASSWORD` with your WiFi credentials.
   - Replace `rADDRESSrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr` with your XRPL wallet address.
   - Select the correct board and port under `Tools`.
   - Click the upload button.

2. **Observe the OLED Display**:
   - The OLED display will show account-specific transactions and alerts.

3. **Monitor the Serial Output**:
   - Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`).
   - Set the baud rate to `9600`.
   - Observe the messages indicating account-specific transactions and alerts.

---

## Code Explanation

### Key Features

- **Account Monitoring**:
  - The ESP32 subscribes to a specific XRPL account and logs transactions.

- **OLED Display**:
  - The OLED display shows account-specific transactions and alerts.

- **RGB LED and Speaker**:
  - The RGB LED and speaker provide visual and audio alerts for specific transaction types.

---

## Challenge Section

- **Add Multi-Account Monitoring**:
  - Modify the code to monitor multiple XRPL accounts simultaneously.
  - Display the account name or identifier along with the transaction details on the OLED screen.

- **Enhance Alerts**:
  - Customize the RGB LED and speaker alerts for different transaction types (e.g., payments, trust lines, offers).
  - Incorperate alerts for other transaction types.

---

## Next Steps

- Experiment with modifying the code to monitor additional transaction types or XRPL features.
- Explore integrating this functionality into a larger IoT project.

---

Happy coding! 🚀

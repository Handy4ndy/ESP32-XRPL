# Tutorial 07: Ledger Stream

## Overview

This tutorial demonstrates how to track XRPL ledger updates and transaction counts using a WebSocket connection. The program logs updates in the Serial Monitor and displays relevant data on an OLED screen. It also tracks the highest transaction count (high score) and its ledger sequence.

---

## Objectives

- Learn how to track XRPL ledger updates and transaction counts.
- Display ledger updates and transaction counts on an OLED screen.
- Track and display the highest transaction count (high score).
- Use RGB LEDs to indicate transaction count thresholds.

---

## Components Required

- ESP32 Microcontroller (NodeMCU-32S)
- 0.96" OLED Display (SSD1306)
- RGB LED (HW-479, with built-in resistors)
- Tactile Button
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
3. Connect one leg of the button to GPIO4 and the other leg to GND.

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/04_Display.png)
---

## How to Use

1. **Upload the Code**:
   - Open the `07-LedgerStream.ino` file in the Arduino IDE.
   - Replace `YOUR_NETWORK_SSID` and `YOUR_NETWORK_PASSWORD` with your WiFi credentials.
   - Select the correct board and port under `Tools`.
   - Click the upload button.

2. **Observe the OLED Display**:
   - The OLED display will show ledger updates and transaction counts.

3. **Monitor the Serial Output**:
   - Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`).
   - Set the baud rate to `9600`.
   - Observe the messages indicating ledger updates and transaction counts.

---

## Code Explanation

### Key Features

- **Ledger Updates**:
  - The ESP32 subscribes to the XRPL ledger stream and logs updates.

- **OLED Display**:
  - The OLED display shows ledger updates, transaction counts, and the high score.

- **RGB LED**:
  - The RGB LED changes color based on transaction count thresholds.

---

## Challenge Section

- **Add a Transaction Threshold Alert**:
  - Modify the code to play a sound or flash the RGB LED when the transaction count exceeds a specific threshold (e.g., 1000).
  - Use the speaker or buzzer to play a unique melody for high transaction counts.

- **Customize the Display**:
  - Add a graphical representation of the transaction count (e.g., a bar graph or progress bar).
  - Experiment with different fonts and graphics using the `Adafruit_GFX` library.

---

## Next Steps

- Proceed to **Tutorial 08: Account Monitor** to learn how to monitor specific XRPL accounts.
- Experiment with modifying the code to track additional ledger metrics.

---

Happy coding! 🚀

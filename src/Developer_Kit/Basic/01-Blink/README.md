# Tutorial 01: Blink

## Overview

This tutorial is part of the ESP32-XRPL Developer Kit and introduces you to controlling an RGB LED using the ESP32. The script demonstrates three different modes of operation for the RGB LED:
1. **Slow Blink**: Cycles through red, green, and blue LEDs with a slow blink.
2. **Fast Fade**: Smoothly fades a random color in and out.
3. **Fast Blink**: Randomly blinks one or more LEDs at a fast rate.

The tutorial also includes a challenge to refactor the code using a `switch` case structure for better modularity.

---

## Objectives

- Learn how to control an RGB LED using the ESP32.
- Understand how to use `millis()` for non-blocking timing.
- Explore different LED control patterns.
- Practice refactoring code into modular functions.
- Use the Serial Monitor as a debugging tool.

---

## Components Required

- ESP32 Microcontroller (NodeMCU-32S)
- RGB LED (HW-479, with built-in resistors)
- Breadboard
- Jumper Wires

---

## Circuit Diagram

1. Connect the **RED** pin of the RGB LED to GPIO5.
2. Connect the **GREEN** pin of the RGB LED to GPIO19.
3. Connect the **BLUE** pin of the RGB LED to GPIO18.
4. Connect the **common cathode** (or anode, depending on your LED type) to GND (or 3.3V for common anode).

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/01-Blink.png)

---

## How to Use

1. **Upload the Code**:
   - Open the `01-Blink.ino` file in the Arduino IDE.
   - Select the correct board and port under `Tools`.
   - Click the upload button.

2. **Observe the Modes**:
   - The RGB LED will cycle through three modes:
     - **Slow Blink**: Each color blinks for 1 second.
     - **Fast Fade**: A random color fades in and out smoothly.
     - **Fast Blink**: Random LEDs blink rapidly.

3. **Monitor the Serial Output**:
   - Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`).
   - Set the baud rate to `9600`.
   - Observe the messages indicating the current mode and debugging information.

4. **Challenge**:
   - Uncomment the `switch` case logic in the code.
   - Refactor the `loop` function by moving the mode logic into separate functions (`slowBlink`, `fastFade`, `fastBlink`).
   - Add a button connected in parallel with the ground to control the on/off state of the RGB LED.

---

## Code Explanation

### Key Features

- **Non-blocking Timing**:
  - The script uses `millis()` to manage timing without blocking the execution of other code.

- **Mode Switching**:
  - The script cycles through three modes every 5 seconds.

- **Randomized Behavior**:
  - The `Fast Fade` and `Fast Blink` modes use the `random()` function to introduce variability.

- **Serial Monitoring**:
  - The script uses the Serial Monitor to print debugging messages, such as the current mode and status updates. This is a powerful tool for understanding how the code operates and troubleshooting issues.

### Challenge Section

The commented-out `switch` case logic in the code provides a template for refactoring. Users are encouraged to:
- Move the mode logic into separate functions.
- Use the `switch` case structure to call the appropriate function based on the current mode.
- Add a button connected in parallel with the ground to toggle the LED on/off.

---

### Emulate

[Emulate](https://github.com](https://wokwi.com/projects/429706170250877953)

## Next Steps

- Once you complete this tutorial, proceed to **Tutorial 02: Button** to learn how to use a button to control the RGB LED modes.
- Explore the `src/Examples` directory for additional example code.

---

Happy coding! 🚀

# Tutorial 02: Button

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/Button.png)

## Overview

This tutorial is part of the ESP32-XRPL Developer Kit and introduces you to using a button to control an RGB LED. The script demonstrates three different modes of operation for the RGB LED:
1. **Slow Blink**: Cycles through red, green, and blue LEDs with a slow blink.
2. **Fast Fade**: Smoothly fades a random color in and out.
3. **Fast Blink**: Randomly blinks one or more LEDs at a fast rate.

The tutorial also includes a challenge to extend the functionality by adding a new RGB mode or implementing a long-press feature.

---

## Objectives

- Learn how to use a button to control an RGB LED.
- Understand how to implement debouncing for reliable button presses.
- Explore different LED control patterns.
- Practice extending functionality with modular programming.
- Use the Serial Monitor as a debugging tool.

---

## Components Required

- ESP32 Microcontroller (NodeMCU-32S)
- RGB LED (HW-479, with built-in resistors)
- Tactile Button
- Breadboard
- Jumper Wires

---

## Circuit Diagram

1. Connect the **RED** pin of the RGB LED to GPIO5.
2. Connect the **GREEN** pin of the RGB LED to GPIO19.
3. Connect the **BLUE** pin of the RGB LED to GPIO18.
4. Connect one leg of the button to GPIO4 and the other leg to GND.
5. Enable the internal pull-up resistor for GPIO4 in the code.

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/02-Button.png)

---

## How to Use

1. **Upload the Code**:
   - Open the `02-Button.ino` file in the Arduino IDE.
   - Select the correct board and port under `Tools`.
   - Click the upload button.

2. **Observe the Modes**:
   - Press the button to cycle through the three modes:
     - **Slow Blink**: Each color blinks for 1 second.
     - **Fast Fade**: A random color fades in and out smoothly.
     - **Fast Blink**: Random LEDs blink rapidly.

3. **Monitor the Serial Output**:
   - Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`).
   - Set the baud rate to `9600`.
   - Observe the messages indicating the current mode and debugging information.

---

## Challenge Section

- **Add a New RGB Mode**:
  - Create a new mode, such as a **Rainbow Cycle**, that smoothly transitions through all colors of the RGB spectrum.
  - Use the `analogWrite` function to adjust the brightness of each color channel over time.
  - Incorporate this new mode into the `switch` case structure.
  - Test your new mode using the Serial Monitor for debugging.

- **Implement a Long Button Press**:
  - Modify the button press logic to detect a long button press (e.g., holding the button for 2 seconds).
  - Use the long press to toggle between enabling and disabling the RGB LED entirely, or to reset the mode to **Slow Blink**.
  - Hint: Use `millis()` to measure how long the button is held down.

---

## Next Steps

- Once you complete this tutorial, proceed to **Tutorial 03: Alerts** to learn how to generate audio alerts using a buzzer and LED.
- Explore the `src/Examples` directory for additional example code.

---

Happy coding! 🚀

# Tutorial 03: Alerts

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/Speaker.png)

## Overview

This tutorial is part of the ESP32-XRPL Developer Kit and demonstrates how to generate audio and visual alerts using a button, an RGB LED, and a speaker. The script includes three different modes of operation for the RGB LED:

1. **Slow Blink**: Cycles through red, green, and blue LEDs with a slow blink.
2. **Fast Fade**: Smoothly fades a random color in and out.
3. **Fast Blink**: Randomly blinks one or more LEDs at a fast rate.

Additional Features:
- A short button press cycles through the three modes and plays a short alert sound.
- A long button press (5 seconds) plays a melody using the notes defined in `pitches.h`.

---

## Objectives

- Learn how to generate audio alerts using a speaker.
- Understand how to implement long button press detection.
- Explore different LED control patterns.
- Practice extending functionality with modular programming.
- Use the Serial Monitor as a debugging tool.

---

## Components Required

- ESP32 Microcontroller (NodeMCU-32S)
- RGB LED (HW-479, with built-in resistors)
- Tactile Button
- Speaker or Buzzer
- Breadboard
- Jumper Wires

---

## Circuit Diagram

1. Connect the **RED** pin of the RGB LED to GPIO5.
2. Connect the **GREEN** pin of the RGB LED to GPIO19.
3. Connect the **BLUE** pin of the RGB LED to GPIO18.
4. Connect one leg of the button to GPIO4 and the other leg to GND.
5. Connect the speaker or buzzer to GPIO17 and GND.
6. Enable the internal pull-up resistor for GPIO4 in the code.

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/03-Alerts.png)

---

## How to Use

1. **Upload the Code**:
   - Open the `03-Alerts.ino` file in the Arduino IDE.
   - Select the correct board and port under `Tools`.
   - Click the upload button.

2. **Observe the Modes**:
   - Press the button to cycle through the three modes:
     - **Slow Blink**: Each color blinks for 1 second.
     - **Fast Fade**: A random color fades in and out smoothly.
     - **Fast Blink**: Random LEDs blink rapidly.
   - A short alert sound will play each time the mode changes.

3. **Test the Long Press**:
   - Hold the button for 5 seconds to trigger a long press.
   - A melody will play through the speaker or buzzer.

4. **Monitor the Serial Output**:
   - Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`).
   - Set the baud rate to `9600`.
   - Observe the messages indicating the current mode and debugging information.

---

## Code Explanation

### Key Features

- **Debouncing**:
  - The script uses a debounce delay of 200 milliseconds to ensure reliable button presses.

- **Mode Switching**:
  - Each short button press cycles through the three modes: Slow Blink, Fast Fade, and Fast Blink.

- **Audio Alerts**:
  - A short alert sound plays each time the mode changes.
  - A long press triggers a melody using the notes defined in `pitches.h`.

- **Serial Monitoring**:
  - The script uses the Serial Monitor to print debugging messages, such as the current mode and status updates. This is a powerful tool for understanding how the code operates and troubleshooting issues.

---

## Challenge Section

- **Add a Different Melody for Each Mode**:
  - Modify the code to play a unique melody for each mode (Slow Blink, Fast Fade, Fast Blink) when the mode changes.
  - Use the `tone()` function to generate the melodies.

- **Change the Long Press Melody**:
  - Replace the existing long press melody with a new one.
  - Experiment with different note sequences and durations to create a custom melody.

These challenges will help you practice modular programming, improve your understanding of audio generation, and expand the functionality of your project.

---

## Emulate

[Emulate](https://wokwi.com/projects/429707546029863937)
---

## Next Steps

- Once you complete this tutorial, proceed to **Tutorial 04: Display** to learn how to display text and graphics on a 0.96" LCD screen.
- Explore the `src/Examples` directory for additional example code.

---

Happy coding! 🚀

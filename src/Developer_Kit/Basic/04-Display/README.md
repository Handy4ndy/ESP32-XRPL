# Tutorial 04: Display
 
![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/DisplayScreen.png)

## Overview

This tutorial is part of the ESP32-XRPL Developer Kit and demonstrates how to use an OLED display to show a loading image and display the current mode of operation for the RGB LED. The script includes three different modes of operation for the RGB LED:

1. **Slow Blink**: Cycles through red, green, and blue LEDs with a slow blink.
2. **Fast Fade**: Smoothly fades a random color in and out.
3. **Fast Blink**: Randomly blinks one or more LEDs at a fast rate.

Additional Features:
- An OLED display shows a loading image (XRPL logo) at startup.
- The display updates to show the current mode when the button is pressed.

---

## Objectives

- Learn how to use an OLED display with the ESP32.
- Understand how to display text and graphics on the screen.
- Explore different LED control patterns.
- Practice extending functionality with modular programming.
- Use the Serial Monitor as a debugging tool.

---

## Components Required

- ESP32 Microcontroller (NodeMCU-32S)
- RGB LED (HW-479, with built-in resistors)
- Tactile Button
- Speaker or Buzzer
- 0.96" OLED Display (SSD1306)
- Breadboard
- Jumper Wires

---

## Circuit Diagram

1. Connect the **RED** pin of the RGB LED to GPIO5.
2. Connect the **GREEN** pin of the RGB LED to GPIO19.
3. Connect the **BLUE** pin of the RGB LED to GPIO18.
4. Connect one leg of the button to GPIO4 and the other leg to GND.
5. Connect the speaker or buzzer to GPIO17 and GND.
6. Connect the OLED display to the I2C pins:
   - **SDA** to GPIO21.
   - **SCL** to GPIO22.
   - **GND** to GND.
   - **VCC** to 3v3.
7. Enable the internal pull-up resistor for GPIO4 in the code.

![Circuit Diagram](https://github.com/Handy4ndy/ESP32-XRPL/blob/main/src/Developer_Kit/GettingStarted/devKitRef/04_Display.png)

---

## How to Use

1. **Upload the Code**:
   - Open the `04-Display.ino` file in the Arduino IDE.
   - Select the correct board and port under `Tools`.
   - Click the upload button.

2. **Observe the OLED Display**:
   - At startup, the OLED display will show a loading image (XRPL logo).
   - When the button is pressed, the display will update to show the current mode:
     - **Slow Blink**: Displays "Slow Blink Active".
     - **Fast Fade**: Displays "Fast Fade Active".
     - **Fast Blink**: Displays "Fast Blink Active".

3. **Test the Long Press**:
   - Hold the button for 5 seconds to trigger a long press.
   - The display will show "Long press detected" and "Playing melody" while the melody plays.

4. **Monitor the Serial Output**:
   - Open the Serial Monitor in the Arduino IDE (`Tools > Serial Monitor`).
   - Set the baud rate to `9600`.
   - Observe the messages indicating the current mode and debugging information.

---

## Code Explanation

### Key Features

- **OLED Display**:
  - The OLED display is initialized using the `Adafruit_SSD1306` library.
  - A loading image (XRPL logo) is displayed at startup.
  - The display updates to show the current mode when the button is pressed.

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

- **Customize the Display**:
  - Modify the code to display additional information, such as the RGB LED's current color or brightness level.
  - Experiment with different fonts and graphics using the `Adafruit_GFX` library.

- **Replace the Bitmap Image**:
  - Use the [Bitmap Generator](https://github.com/Handy4ndy/Bitmap-Generator) tool to create a new bitmap image.
  - Replace the existing XRPL logo with your custom image.
  - Update the code to display the new image during the startup sequence.

These challenges will help you practice working with displays, improve your understanding of graphics programming, and expand the functionality of your project.

---

## Next Steps

- Once you complete this tutorial, explore the Advanced tutorials to learn about connectivity and IoT concepts.
- Experiment with combining the OLED display, audio alerts, and RGB LED to create interactive projects.

---

Happy coding! 🚀

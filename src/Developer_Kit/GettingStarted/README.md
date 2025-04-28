# ESP32-XRPL Developer Kit SDK

Welcome to the ESP32-XRPL Developer Kit SDK! This SDK is designed to help you assemble your developer kit, learn the basics of programming and circuits, and build a strong foundation for advanced IoT projects. By following the tutorials, you will gain hands-on experience with the ESP32 microcontroller, breadboard prototyping, and coding in C++.

---

## What is the Developer Kit SDK?

The Developer Kit SDK is a step-by-step guide that helps users construct their developer kit while learning essential programming and electronics concepts. It includes tutorials that cover basic and advanced topics, enabling you to set up your board correctly and prepare for more complex projects.

---

## Kit Contents

To build the Developer Kit and follow the tutorials, you will need the following components:

- **ESP32 Microcontroller**
- **Tactile Button**
- **3-Color LED (RGB)**
- **Buzzer**
- **Diode**
- **0.96" LCD Screen** 
- **Breadboard**
- **Jumper Wires**
- **Resistors** 
- **USB Cable** 

---

## Tutorials Overview

The tutorials are divided into **Basic** and **Advanced** levels. Start with the Basic tutorials to assemble your kit and learn the fundamentals before moving on to Advanced tutorials that introduce connectivity and IoT concepts.

### Basic Tutorials
1. **01-Blink**: Learn how to blink an LED using the ESP32.
2. **02-Button**: Use a button to control an RGB LED.
3. **03-Alerts**: Generate sound and visual alerts using a buzzer and LED.
4. **04-Display**: Display text and graphics on a 0.96" LCD screen.

Each tutorial has its own `README.md` file in the corresponding directory to explain the contents and guide you through the steps.

---

## Advanced Tutorials

Once you complete the Basic tutorials, you can explore Advanced tutorials that cover:
- **Connectivity**: Connect the ESP32 to a WiFi network.
- **WebSocket Communication**: Establish a WebSocket connection to send and receive data.
- **Dynamic Display**: Display real-time information on the LCD screen.
- **Integrated Alerts**: Combine connectivity and alerts for interactive IoT applications.

---

## How to Use This SDK

1. **Start with the Basics**:
   - Follow the Basic tutorials in sequence to assemble your kit and learn the fundamentals.
   - Each tutorial includes example code located in the `src/Examples` directory.

2. **Progress to Advanced Topics**:
   - Once your kit is fully assembled and functional, move on to the Advanced tutorials to explore IoT concepts.

3. **Experiment and Innovate**:
   - Use the knowledge gained from the tutorials to create your own projects and expand the capabilities of your Developer Kit.

---

## Getting Started

### Step 1: Set Up Your Environment

1. **Install Arduino IDE**:
   - Download and install the Arduino IDE from [here](https://www.arduino.cc/en/software).

2. **Add ESP32 Board Support**:
   - Open Arduino IDE.
   - Go to `File > Preferences`.
   - In the "Additional Board Manager URLs" field, add:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Go to `Tools > Board > Boards Manager`, search for "ESP32", and install it.

3. **Install Required Libraries**:
   - Go to `Sketch > Include Library > Manage Libraries`.
   - Search for and install the following libraries:
     - `WiFi`
     - `WebSocketsClient`
     - `ArduinoJson`
     - `Adafruit_GFX`
     - `Adafruit_SSD1306`

### Step 2: Install USB-to-Serial Drivers

The ESP32 (NodeMCU-32S) requires a USB-to-serial driver to communicate with your computer via USB. The driver depends on the operating system and the USB-to-serial chip (commonly CP2102 or CH340/CH341) on your ESP32 board.

- **Windows**:
  - Download and install the driver for your ESP32's USB-to-serial chip:
    - CP2102: [Silicon Labs CP2102 Driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
    - CH340/CH341: [WCH CH340 Driver](http://www.wch.cn/engine/downloads/CH341SER_EXE.html)
  - After installation, the ESP32 will appear as a COM port in Arduino IDE under `Tools > Port`.

- **macOS**:
  - For CP2102, macOS (Catalina and later) usually includes built-in support, so no driver is needed.
  - For CH340/CH341, download and install the driver from [here](https://github.com/adrianmihalko/ch340g-ch34g-ch34x-mac-os-x-driver).
  - After installation, allow the driver in `System Settings > Security & Privacy` if prompted.
  - The ESP32 will appear as a port (e.g., `/dev/cu.usbserial-*`) in Arduino IDE under `Tools > Port`.

- **Linux**:
  - Most Linux distributions (e.g., Ubuntu, Debian) include built-in support for CP2102 and CH340/CH341 chips.
  - Ensure your user has permission to access the serial port by running:
    ```
    sudo usermod -a -G dialout $USER
    ```
    Then log out and back in.
  - The ESP32 will appear as a port (e.g., `/dev/ttyUSB0`) in Arduino IDE under `Tools > Port`.
  - If the port doesn’t appear, check USB detection with `dmesg` or ensure the kernel modules (`usbserial`, `cp210x`, or `ch341`) are loaded.

**Notes**:
- Use a high-quality USB cable with data lines (not power-only).
- If the port doesn’t appear in Arduino IDE, verify driver installation, check cable quality, or consult your ESP32 board’s documentation for the specific USB-to-serial chip.

---

## Next Steps

- Begin with **01-Blink** to learn how to blink an LED using the ESP32.
- Follow the tutorials in sequence to assemble your kit and build your skills.
- Explore the `src/Examples` directory for example code to accompany each tutorial.

---

Happy learning and building! 🚀
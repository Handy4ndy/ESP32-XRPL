# Little Ledger - Real-Time Blockchain Monitoring

## Introduction

Little Ledger is an open-source project by [Xspence](https://xspence.co.uk) that merges Internet of Things (IoT) devices with the Internet of Value (IoV) to deliver real-time monitoring and notifications for the XRP Ledger and Xahau Networks. By subscribing to ledger streams, Little Ledger devices provide instant alerts for on-chain events such as transactions, price changes, and network status, making blockchain activity more accessible and engaging for users. All profits from Little Ledger are reinvested into supporting the ecosystem's growth and infrastructure.

> **Educational Focus**
> Little Ledger have been designed as an educational project to inspire others to develop and increase their C++ programming skills. By working with real-world blockchain data and embedded hardware, users gain practical experience that can be transferred to core protocol development for the XRP Ledger, Xahau, and other blockchain networks. It's a fun and hands-on way to build foundational C++ knowledge while exploring the world of decentralized technology.

## About Xspence

[Xspence](https://xspence.co.uk) is a forward-thinking fintech startup focused on creating innovative blockchain solutions utilizing the XRP Ledger and Xahau Networks. As the driving force behind Little Ledger, XRMine, and Lambriini Art, Xspence is committed to fostering innovation and growth in the blockchain community. The company provides comprehensive documentation, unified support and feedback channels, and maintains the infrastructure necessary for its projects to thrive. A key goal for 2025 is to establish a validator node for the Xahau Network, further solidifying Xspence’s role in advancing decentralized technology.

## Little Ledger Devices

Little Ledger offers a range of devices tailored to diverse users:

- **Developer_Kit**: Ideal for hobbyists and developers, this kit includes an ESP32 device and detailed documentation to inspire creativity and enable hands-on blockchain monitoring solutions. The Developer Kit features step-by-step tutorials, example code, and modular hardware to help users learn about connectivity, display, alerts, and real-time blockchain data streaming.

- **Micro_Ledger**: A wearable device for real-time XRP Ledger monitoring. It features a Seeed Xiao ESP32 S3 microcontroller, a 0.96" OLED display, speaker, LEDs, vibration motor, and a 900mAh battery offering up to 12 hours of use. Designed for portability, it includes a 3D printed case and lanyard. The Micro_Ledger provides real-time updates on ledger status, server health, and price information, with visual, haptic, and audio alerts for critical events.

These devices are user-friendly, low-power, and designed for both casual users and blockchain enthusiasts.

## How It Works

Little Ledger devices connect to blockchain networks via WiFi and use WebSocket technology to receive real-time ledger updates. When a new ledger is published, the device fetches server information and displays key metrics—such as host ID, I/O latency, uptime, and peer count—on its OLED screen. If server performance metrics, like latency, exceed predefined thresholds, the device alerts the user through vibrations and visual cues, ensuring they stay informed about critical network changes.

## Project Structure

The repository is organized as follows:

- **Little_Ledger/Developer_Kit/**: Step-by-step tutorials and example code for learning and prototyping.
- **Little_Ledger/Micro_Ledger/**: Wearable device firmware for real-time blockchain monitoring.
- **Little_Ledger/Examples/**: Additional example projects for display, network, and alert features.
- **Little_Ledger/Lamp/**: Fun and experimental LED-based projects.
- **Little_Ledger/Priceaction/**: Price monitoring and analytics modules.

Each directory contains detailed README files and code samples to help you get started.

## Community and Support

Xspence encourages community participation through an open-source codebase and tutorials for applications like Account Monitor. Support is available via Xspence’s unified channels, fostering collaboration and learning.

## Getting Started

1. **Install the Arduino IDE** and add ESP32 board support.
2. **Install required libraries**: WiFi, WebSocketsClient, ArduinoJson, Adafruit_GFX, Adafruit_SSD1306, and others as specified in each tutorial.
3. **Assemble your hardware** following the provided circuit diagrams.
4. **Upload the example code** from the relevant directory to your ESP32 device.
5. **Monitor the device** via OLED display and Serial Monitor for real-time blockchain updates.

For detailed setup instructions, see the `GettingStarted` and individual project README files.

## Future Plans

Xspence will launch Little Ledger products on [xMerch.app](https://www.xmerch.app/) soon, with plans for direct sales and a validator node by 2025, reinforcing its commitment to the XRP Ledger and Xahau ecosystems.

---

*Created by Xspence. Open source and community-driven. A bootstap project where profits will be reinvested into blockchain infrastructure and ecosystem growth.*
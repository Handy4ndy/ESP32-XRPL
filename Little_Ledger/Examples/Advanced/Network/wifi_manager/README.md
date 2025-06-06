# WiFi Manager Setup

This script demonstrates how to use the WiFiManager library to connect an ESP32 to a specified WiFi network. It includes functionality to handle reconnections, simulate disconnections, and enter custom parameters.

## Features

- Connects to a specified WiFi network using the WiFiManager library.
- Automatically attempts to reconnect to the WiFi network if disconnected.
- Simulates a disconnection for testing purposes.
- Allows entry of custom parameters.

## Hardware Requirements

- ESP32 or compatible microcontroller with WiFi capability.

## Software Requirements

- Arduino IDE.
- ESP32 board support in Arduino IDE.
- WiFiManager library.

## Installation

1. Open the `wifi_manager.ino` file in the Arduino IDE.
2. Install the required libraries:
   - WiFiManager by tzapu: [WiFiManager GitHub Repository](https://github.com/tzapu/WiFiManager)
3. Upload the code to your ESP32 or compatible microcontroller.

## Usage

1. Open the Serial Monitor in the Arduino IDE.

2. Power on your ESP32 or compatible microcontroller.

3. The device will attempt to connect to the specified WiFi network using the WiFiManager library.

4. If the connection fails, it will start an access point with the SSID "WiFi_Manager" and a web portal for configuration.

```
    *wm:resetSettings 
    *wm:SETTINGS ERASED 
    *wm:AutoConnect 
    *wm:No wifi saved, skipping 
    *wm:AutoConnect: FAILED for  21 ms
    *wm:StartAP with SSID:  WiFi_Manager
    *wm:AP IP address: 192.168.4.1
    *wm:Starting Web Portal 
```
5. Once connected, it will print the connection status to the Serial Monitor.

```
    *wm:Connecting to NEW AP: Your_Network_SSID
    *wm:connectTimeout not set, ESP waitForConnectResult... 
    *wm:Connect to new AP [SUCCESS] 
    *wm:Got IP Address: 
    *wm:192.168.4.81 
    *wm:config portal exiting 
    Connected to WiFi
```

6. The script will handle reconnections if the WiFi connection is lost.

7. The Serial Monitor will display the connection status:

```
    WiFi disconnected. Reconnecting...
    Failed to connect to WiFi. Retrying...
    Reconnected to WiFi
    Connected...
```

## Advanced

### Disconnection Test

To simulate a disconnection for testing purposes, uncomment the following lines in the `loop` function:

```cpp
// Uncomment the following lines to simulate a disconnection
if (WiFi.status() == WL_CONNECTED) {
  Serial.println("Simulating disconnection in 5 seconds");
  delay(5000); // Wait for 5 seconds
  WiFi.disconnect(); // Disconnect from WiFi
}
```

This will disconnect the WiFi after 5 seconds and trigger the reconnection process.

### Custom Parameter

To use a custom parameter with WiFiManager, uncomment the following lines:

```cpp

// Declare the WiFi Manager parameter
char customparam[40] = "";

// Custom parameter for WiFiManager
WiFiManagerParameter custom_text("customparam", "Custom Parameter", customparam, 40);
wifiManager.addParameter(&custom_text);

// Save custom parameter value
strcpy(customparam, custom_text.getValue());

// If custom parameter is found, print it to the serial monitor
if (strlen(customparam) > 0) {
  Serial.print("Custom parameter was entered, custom parameter used: ");
  Serial.println(customparam);
} else {
  // Use the hardcoded parameter if blank
  strcpy(customparam, "hardcodedParam");
  Serial.print("No custom parameter was added, hardcoded parameter used: ");
  Serial.println(customparam);
}
```

## License

This project is licensed under the GNU Lesser General Public License - see the [LICENSE](LICENSE) file for details.

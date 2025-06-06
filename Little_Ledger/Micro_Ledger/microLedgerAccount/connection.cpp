#include "connection.h"
#include <WiFi.h>

namespace {
  const char* ssid = "XRPL_wifi";
  const char* password = "Qwerty123";
}

void WiFiConnection::connect(Adafruit_SSD1306& display) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(3, 0);
  display.print("Wifi/Websocket Setup");
  display.setCursor(5, 18);
  display.print("Connecting to:");
  display.setCursor(5, 27);
  display.print(ssid);
  display.display();

  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() - wifiStart > 30000) {
      ESP.restart();
    }
  }
  display.fillRect(0, 16, 128, 48, BLACK);
  display.setCursor(5, 18);
  display.print("Connected to:");
  display.setCursor(5, 27);
  display.print(ssid);
  display.setCursor(5, 38);
  display.print("IP Address: ");
  display.setCursor(5, 47);
  display.print(WiFi.localIP());
  display.display();
  delay(1000);
  Serial.print("wifi is connected..");
}

bool WiFiConnection::isConnected() {
  return WiFi.status() == WL_CONNECTED;
  
}

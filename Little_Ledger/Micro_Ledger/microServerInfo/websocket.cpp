#include "websocket.h"
#include <WebSocketsClient.h>
#include <Adafruit_SSD1306.h>

namespace {
  WebSocketsClient webSocket;
  bool connected = false;
  unsigned long lastReconnectAttempt = 0;
  const unsigned long reconnectInterval = 5000;
  Adafruit_SSD1306* disp = nullptr;
  const char* ws_host = nullptr;
  uint16_t ws_port = 0;
  const char* ws_url = nullptr;
  WebSocketMessageCallback messageCallback = nullptr;

  void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
      case WStype_DISCONNECTED:
        connected = false;
        if (disp) {
          disp->clearDisplay();
          disp->setCursor(3, 0);
          disp->print("Wifi/Websocket Setup");
          disp->setCursor(5, 18);
          disp->print("Connecting to:");
          disp->setCursor(5, 27);
          disp->print(ws_host);
          disp->display();
        }
        if (ws_host && ws_url) {
          webSocket.beginSSL(ws_host, ws_port, ws_url);
        }
        break;
      case WStype_CONNECTED:
        connected = true;
        if (disp) {
          disp->setCursor(5, 38);
          disp->print("WebSocket Connected");
          disp->display();
        }
        webSocket.sendTXT("{\"command\": \"subscribe\", \"streams\": [\"ledger\"]}");
        break;
      case WStype_TEXT:
        if (messageCallback) {
          messageCallback(payload, length);
        } else {
          Serial.println("Received WebSocket message (no callback set)");
        }
        break;
    }
  }
}

void WebSocketConnection::begin(Adafruit_SSD1306& display, const char* host, uint16_t port, const char* url, WebSocketMessageCallback callback) {
  disp = &display;
  ws_host = host;
  ws_port = port;
  ws_url = url;
  messageCallback = callback;
  webSocket.beginSSL(host, port, url);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(reconnectInterval);
}

void WebSocketConnection::loop() {
  webSocket.loop();
}

bool WebSocketConnection::isConnected() {
  return connected;
}

void WebSocketConnection::send(const char* message) {
  if (connected) {
    webSocket.sendTXT(message);
  } else {
    Serial.println("WebSocket not connected, cannot send message");
  }
}

void WebSocketConnection::disconnect() {
  webSocket.disconnect();
  connected = false;
  Serial.println("WebSocket connection closed");
}
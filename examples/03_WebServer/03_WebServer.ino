#include <ESP_NIC.h>
EspNICServer server(80);
EspNICClient peer;
String request;
uint32_t acceptedAt;
void setup() {
  Serial.begin(115200); delay(1000);
  if (!EspNIC.begin() || !EspNIC.connectWiFi("YOUR_SSID","YOUR_PASSWORD")) { Serial.println(EspNIC.lastError()); return; }
  Serial.println(EspNIC.getLocalIP());
  if (!server.begin(80)) Serial.println("Host lwIP required for TCP server.");
}
void loop() {
  if (!EspNIC.isReady()) { delay(1); return; }
  EspNIC.poll();
  if (!peer) { peer=server.accept(); request=""; acceptedAt=millis(); }
  unsigned budget=128;
  while (peer.available() && budget--) {
    int c=peer.read(); if (c<0) break; request+=char(c);
    if (request.endsWith("\r\n\r\n")) {
      peer.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 18\r\nConnection: close\r\n\r\nESP-NIC host ready");
      peer.stop(); break;
    }
    if (request.length()>1024) { peer.stop(); break; }
  }
  if (peer && uint32_t(millis()-acceptedAt)>3000) peer.stop();
  delay(1);
}

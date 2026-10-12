#include <ESP_NIC.h>
// Default HTTP works without additional certificates. For verified HTTPS,
// provide your server's current CA PEM below, then set USE_HTTPS to 1.
#ifndef USE_HTTPS
#define USE_HTTPS 0
#endif
#if USE_HTTPS
EspNICSecureClient client;
static const char rootCA[]=""; // Replace with a real trusted PEM, never setInsecure.
#else
EspNICClient client;
#endif
bool started;
uint32_t lastData;
void setup() {
  Serial.begin(115200); delay(1000);
  if (!EspNIC.begin() || !EspNIC.connectWiFi("YOUR_SSID","YOUR_PASSWORD")) { Serial.println(EspNIC.lastError()); return; }
#if USE_HTTPS
  // A nonempty PEM selects your trust root; otherwise ESP32's built-in CA
  // bundle is used when the installed SDK enables it. No insecure fallback.
  if (rootCA[0]) client.setCACert(rootCA);
#if defined(ARDUINO_ARCH_ESP32)
  configTime(0,0,"pool.ntp.org");
  uint32_t waitStart=millis();
  while (time(nullptr)<1700000000 && uint32_t(millis()-waitStart)<15000) { EspNIC.poll(); delay(1); }
#endif
  started=client.connect("example.com",443);
#else
  started=client.connect("example.com",80);
#endif
  if (started) client.print("GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n");
  else Serial.println("Connection failed (TLS requires a valid trust root and clock).");
  lastData=millis();
}
void loop() {
  if (EspNIC.isReady()) EspNIC.poll();
  unsigned budget=256;
  while (started && client.available() && budget--) { int c=client.read(); if (c>=0) { Serial.write(c); lastData=millis(); } }
  if (started && (!client.connected() || uint32_t(millis()-lastData)>10000)) { client.stop(); started=false; }
  delay(1);
}

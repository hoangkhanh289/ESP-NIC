#include <ESP_NIC.h>
uint8_t frame[1514];
void setup() {
  Serial.begin(115200); delay(1000);
#if defined(ARDUINO_ARCH_STM32)
  if (!EspNIC.beginSPI(PA5,PA6,PA7,PB6,PB0,PB1,PB10,10000000)) { Serial.println(EspNIC.lastError()); return; }
  // Association only; install an external IP stack if TCP/IP is required.
  if (!EspNIC.connectWiFi("YOUR_SSID","YOUR_PASSWORD")) Serial.println(EspNIC.lastError());
#else
  Serial.println("Select STM32 F446/G474/F767/H743.");
#endif
}
void loop() {
  if (EspNIC.isReady()) {
    int n=EspNIC.readPacket(frame,sizeof(frame));
    if (n>0) { Serial.print("Ethernet RX bytes: "); Serial.println(n); }
    // Supply a valid frame to sendPacket. Do not echo broadcast packets.
  }
  delay(1);
}

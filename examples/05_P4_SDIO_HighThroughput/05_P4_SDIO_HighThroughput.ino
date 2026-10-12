#include <ESP_NIC.h>
uint8_t frame[1514];
uint32_t receivedBytes,intervalStart;
void setup() {
  Serial.begin(115200); delay(1000);
  // Check schematic: P4 CLK18/CMD19/D0=14/D1=15/D2=16/D3=17/RESET54.
  // SDIO slave pins are fixed; add external pull-ups. No fictitious rate claim.
  nic_sdio_config_t config={18,19,14,15,16,17,54,40000000,4,1};
  if (!EspNIC.beginSDIO(config) || !EspNIC.connectWiFi("YOUR_SSID","YOUR_PASSWORD")) { Serial.println(EspNIC.lastError()); return; }
  Serial.println(EspNIC.getLocalIP()); intervalStart=millis();
}
void loop() {
  if (!EspNIC.isReady()) { delay(1); return; }
  int n=EspNIC.readPacket(frame,sizeof(frame)); if (n>0) receivedBytes+=n;
  uint32_t elapsed=millis()-intervalStart;
  if (elapsed>=1000) { Serial.print("Observed RX bytes/s: "); Serial.println((uint64_t(receivedBytes)*1000)/elapsed); receivedBytes=0; intervalStart=millis(); }
  delay(1);
}

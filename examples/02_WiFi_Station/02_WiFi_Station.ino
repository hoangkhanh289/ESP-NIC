#include <ESP_NIC.h>
void setup() {
  Serial.begin(115200); delay(1000);
  if (!EspNIC.begin()) { Serial.println(EspNIC.lastError()); return; }
  if (!EspNIC.connectWiFi("YOUR_SSID","YOUR_PASSWORD")) { Serial.println(EspNIC.lastError()); return; }
  Serial.println(EspNIC.getLocalIP());
  Serial.println("Association/DHCP complete. Keep poll running.");
}
void loop() { if (EspNIC.isReady()) EspNIC.poll(); delay(1); }

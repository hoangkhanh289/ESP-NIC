#include <ESP_NIC.h>
void setup() {
  Serial.begin(115200); delay(1000);
  if (!EspNIC.begin()) { Serial.println(EspNIC.lastError()); return; }
  nic_ap_record_t networks[20];
  int count=EspNIC.scanNetworks(networks,20);
  if (count<0) { Serial.println(count); return; }
  for (int i=0;i<count;++i) {
    const auto &ap=networks[i];
    Serial.print(ap.ssid); Serial.print("  RSSI="); Serial.print(ap.rssi);
    Serial.print("  CH="); Serial.print(ap.channel); Serial.print("  AUTH="); Serial.print(ap.authmode);
    Serial.print("  BSSID=");
    for (unsigned b=0;b<6;++b) { if (b) Serial.print(':'); if (ap.bssid[b]<16) Serial.print('0'); Serial.print(ap.bssid[b],HEX); }
    Serial.println();
  }
  // 5 GHz results require an ESP32-C5 slave and a compatible country/channel.
}
void loop() { if (EspNIC.isReady()) EspNIC.poll(); delay(1); }

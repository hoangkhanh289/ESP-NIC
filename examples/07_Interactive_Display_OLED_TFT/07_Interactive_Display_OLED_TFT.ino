#include <ESP_NIC.h>
uint32_t lastUpdate;
void eventReceived(uint32_t id,const uint8_t *data,size_t length,void *) {
  Serial.print("Slave event "); Serial.print(id); Serial.print(" bytes="); Serial.println(length);
  if (id==1 && data) Serial.write(data,length);
}
void setup() {
  Serial.begin(115200); delay(1000); EspNIC.onEvent(eventReceived);
  // Flash the nested slave sketch, not an upstream firmware lacking display callbacks.
  if (!EspNIC.beginSPI()) { Serial.println(EspNIC.lastError()); return; }
  EspNIC.displayText(0,0,"ESP-NIC ready");
}
void loop() {
  if (EspNIC.isReady()) {
    EspNIC.poll();
    if (uint32_t(millis()-lastUpdate)>=1000) { lastUpdate=millis(); EspNIC.displayProgress((millis()/1000)%101,"bus active"); }
  }
  delay(1);
}

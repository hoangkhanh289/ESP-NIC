#include <ESP_NIC_Slave.h>
void setup() {
  Serial.begin(115200); delay(1000);
  if (!EspNICSlave.beginSPI()) Serial.println(EspNICSlave.lastError());
}
void loop() { EspNICSlave.poll(); delay(1); }

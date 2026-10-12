#include <ESP_NIC.h>
void setup() {
  Serial.begin(115200); delay(1000);
  // Generic wiring example. Choose free GPIOs on your actual T-Display board;
  // GPIO17/18 may conflict with a display and are not a board recommendation.
#if defined(ARDUINO_ARCH_STM32)
  const int32_t tx=PA9,rx=PA10;
#else
  const int32_t tx=17,rx=18;
#endif
  if (!EspNIC.beginUART(tx,rx,921600,-1,1)) { Serial.println(EspNIC.lastError()); return; }
  if (!EspNIC.connectWiFi("YOUR_SSID","YOUR_PASSWORD")) Serial.println(EspNIC.lastError());
}
void loop() { if (EspNIC.isReady()) EspNIC.poll(); delay(1); }

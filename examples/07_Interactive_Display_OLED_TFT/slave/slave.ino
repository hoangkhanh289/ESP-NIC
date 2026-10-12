#include <ESP_NIC_Slave.h>
// Set DISPLAY_KIND=1 for SSD1306, 2 for ST7789 after installing Adafruit libs.
// Default serial rendering has no optional dependencies; it tests real RPC.
#ifndef DISPLAY_KIND
#define DISPLAY_KIND 0
#endif
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#if DISPLAY_KIND==1
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128,64,&Wire,-1);
#elif DISPLAY_KIND==2
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
// Software SPI on separate GPIOs avoids reusing the NIC's SPI2 peripheral.
// CS14/DC15/MOSI16/SCLK17 are examples: check actual board/PSRAM reservations.
Adafruit_ST7789 display(14,15,16,17,-1);
#endif
struct DisplayUpdate { uint8_t kind,percent; int16_t x,y; char text[1401]; };
QueueHandle_t displayQueue;
void enqueueDisplay(uint8_t kind,int16_t x,int16_t y,uint8_t percent,const String &text) {
  // One NIC producer. Queue copies bounded data; rendering runs in another task.
  static DisplayUpdate update;
  update.kind=kind; update.x=x; update.y=y; update.percent=percent;
  size_t n=text.length(); if (n>1400) return;
  memcpy(update.text,text.c_str(),n); update.text[n]=0;
  // A full display queue drops this update, never stalls the network poll task.
  xQueueSend(displayQueue,&update,0);
}
void renderDisplay(void *) {
  DisplayUpdate update;
  for (;;) {
    if (xQueueReceive(displayQueue,&update,portMAX_DELAY)!=pdTRUE) continue;
    if (update.kind==1) {
      Serial.print(update.x); Serial.print(','); Serial.print(update.y); Serial.print(": "); Serial.println(update.text);
#if DISPLAY_KIND
      display.setCursor(update.x,update.y); display.print(update.text);
#endif
    } else {
      Serial.print(update.text); Serial.print(' '); Serial.println(update.percent);
#if DISPLAY_KIND
      display.fillRect(0,48,128,12,0); display.drawRect(0,48,128,12,0xFFFF); display.fillRect(2,50,124*update.percent/100,8,0xFFFF);
#endif
    }
#if DISPLAY_KIND==1
    display.display();
#endif
  }
}
void setup() {
  Serial.begin(115200); delay(1000);
#if DISPLAY_KIND==1
#if CONFIG_IDF_TARGET_ESP32
  Wire.begin(21,22); // Avoid classic ESP32 SPI18/19.
#else
  Wire.begin(18,19); // Check USB/display conflicts on the actual board.
#endif
  if (!display.begin(SSD1306_SWITCHCAPVCC,0x3C)) { Serial.println("OLED initialization failed"); return; }
  display.clearDisplay(); display.setTextColor(SSD1306_WHITE);
#elif DISPLAY_KIND==2
  display.init(240,240); display.setTextColor(ST77XX_WHITE);
#endif
  displayQueue=xQueueCreate(2,sizeof(DisplayUpdate));
  if (!displayQueue) { Serial.println("Display queue allocation failed"); return; }
  if (xTaskCreate(renderDisplay,"display",4096,nullptr,1,nullptr)!=pdPASS) {
    vQueueDelete(displayQueue); displayQueue=nullptr; Serial.println("Display task allocation failed"); return;
  }
  EspNICSlave.onDisplayText([](int16_t x,int16_t y,const String &text) {
    enqueueDisplay(1,x,y,0,text);
  });
  EspNICSlave.onDisplayProgress([](uint8_t percent,const String &label) {
    enqueueDisplay(2,0,0,percent,label);
  });
  if (!EspNICSlave.beginSPI()) Serial.println(EspNICSlave.lastError());
}
void loop() {
  EspNICSlave.poll();
  static uint32_t lastEvent;
  if (EspNICSlave.isMasterConnected() && uint32_t(millis()-lastEvent)>=2000) {
    lastEvent=millis(); const uint8_t sensor[]={42}; EspNICSlave.sendEventToMaster(100,sensor,sizeof(sensor));
  }
  delay(1);
}

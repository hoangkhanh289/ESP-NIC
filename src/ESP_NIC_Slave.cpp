// SPDX-License-Identifier: Apache-2.0
// Public slave ABI forwarding only.
#include "ESP_NIC_Slave.h"
#if defined(ARDUINO_ARCH_STM32)
#if defined(STM32F4xx)
#define NIC_ENTRY esp_nic_v1_f4
#elif defined(STM32G4xx)
#define NIC_ENTRY esp_nic_v1_g4
#elif defined(STM32F7xx)
#define NIC_ENTRY esp_nic_v1_f7
#else
#define NIC_ENTRY esp_nic_v1_h7
#endif
#else
#define NIC_ENTRY esp_nic_v1
#endif
extern "C" int NIC_ENTRY(uint32_t,void *,void *);
namespace {
struct WifiArgs { const char *ssid,*password; uint32_t timeout; };
struct CommandArgs { uint32_t id; const uint8_t *data; size_t size; };
template<class T> struct FunctionArgs { T callback; };
}
EspNICSlaveClass EspNICSlave;
EspNICSlaveClass::EspNICSlaveClass():_ctx(nullptr) {}
EspNICSlaveClass::~EspNICSlaveClass() { end(); }
bool EspNICSlaveClass::context() { return _ctx || !NIC_ENTRY(128,nullptr,&_ctx); }
bool EspNICSlaveClass::beginSPI(int32_t clk,int32_t mi,int32_t mo,int32_t cs,int32_t hs,int32_t dr) {
    nic_spi_config_t c={clk,mi,mo,cs,hs,dr,-1,20000000,2}; return context() && !NIC_ENTRY(130,_ctx,&c);
}
bool EspNICSlaveClass::beginSDIO() { return context() && !NIC_ENTRY(131,_ctx,nullptr); }
bool EspNICSlaveClass::beginUART(int32_t tx,int32_t rx,uint32_t baud,uint8_t port) { nic_uart_config_t c={tx,rx,-1,baud,port}; return context() && !NIC_ENTRY(132,_ctx,&c); }
void EspNICSlaveClass::end() { if (_ctx) NIC_ENTRY(129,_ctx,nullptr); _ctx=nullptr; }
int EspNICSlaveClass::poll() { return NIC_ENTRY(133,_ctx,nullptr); }
int EspNICSlaveClass::lastError() { return NIC_ENTRY(145,_ctx,nullptr); }
bool EspNICSlaveClass::isMasterConnected() const { return NIC_ENTRY(134,_ctx,nullptr)>0; }
bool EspNICSlaveClass::isWiFiConnected() const { return NIC_ENTRY(135,_ctx,nullptr)>0; }
String EspNICSlaveClass::getSSID() const { char name[33]={}; NIC_ENTRY(136,_ctx,name); return String(name); }
int8_t EspNICSlaveClass::getRSSI() { return NIC_ENTRY(137,_ctx,nullptr); }
IPAddress EspNICSlaveClass::getIP() const { return IPAddress(); }
nic_slave_stats_t EspNICSlaveClass::statistics() const { nic_slave_stats_t s={}; NIC_ENTRY(138,_ctx,&s); return s; }
uint32_t EspNICSlaveClass::getTxPackets() const { return statistics().tx_packets; }
uint32_t EspNICSlaveClass::getRxPackets() const { return statistics().rx_packets; }
uint32_t EspNICSlaveClass::getTxBytes() const { return statistics().tx_bytes; }
uint32_t EspNICSlaveClass::getRxBytes() const { return statistics().rx_bytes; }
bool EspNICSlaveClass::setLocalWiFi(const char *s,const char *p) { WifiArgs a={s,p,0}; return !NIC_ENTRY(139,_ctx,&a); }
void EspNICSlaveClass::onCommand(CustomCommandCallback cb) { if (context()) { FunctionArgs<CustomCommandCallback> a={cb}; NIC_ENTRY(140,_ctx,&a); } }
void EspNICSlaveClass::onMessage(CustomMessageCallback cb) { if (context()) { FunctionArgs<CustomMessageCallback> a={cb}; NIC_ENTRY(141,_ctx,&a); } }
void EspNICSlaveClass::onDisplayText(DisplayTextCallback cb) { if (context()) { FunctionArgs<DisplayTextCallback> a={cb}; NIC_ENTRY(142,_ctx,&a); } }
void EspNICSlaveClass::onDisplayProgress(DisplayProgressCallback cb) { if (context()) { FunctionArgs<DisplayProgressCallback> a={cb}; NIC_ENTRY(143,_ctx,&a); } }
int EspNICSlaveClass::sendEventToMaster(uint32_t id,const uint8_t *p,size_t n) { CommandArgs a={id,p,n}; return NIC_ENTRY(144,_ctx,&a); }
int EspNICSlaveClass::sendMessageToMaster(const String &s) { return sendEventToMaster(1,reinterpret_cast<const uint8_t *>(s.c_str()),s.length()); }

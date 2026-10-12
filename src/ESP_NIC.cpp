// SPDX-License-Identifier: Apache-2.0
// ABI forwarding only. Hardware, codecs and state live in libesp_nic.a.
#include "ESP_NIC.h"
#if defined(ARDUINO_ARCH_STM32)
#include <SPI.h>
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
struct PacketArgs { const uint8_t *data; size_t size; };
struct ReadArgs { uint8_t *data; size_t size; };
struct IPArgs { uint8_t bytes[4]; uint16_t port; };
struct HostArgs { const char *name; uint16_t port; };
struct ScanArgs { nic_ap_record_t *records; size_t capacity; uint32_t timeout; };
struct APArgs { const char *ssid,*password; uint8_t channel,clients; bool hidden; };
struct CustomArgs { uint32_t id; const uint8_t *data; size_t size; uint8_t *reply; size_t *reply_size; uint32_t timeout; };
struct CallbackArgs { nic_event_callback_t callback; void *context; };
struct FlashArgs { nic_firmware_callback_t callback; void *context; const char *ssid,*password; };
struct TextArgs { int16_t x,y; const char *text; };
struct ProgressArgs { uint8_t percent; const char *label; };
struct CommandArgs { uint32_t id; const uint8_t *data; size_t size; };
#if defined(ARDUINO_ARCH_STM32)
struct SDArgs { uint32_t clock; int (*setup)(const nic_sdio_config_t *); void (*close)(); bool dma; };
struct MemoryArgs { void *(*allocate)(size_t); void (*release)(void *); };
struct DMAArgs { int (*setup)(SPIClass &); };
#endif
}
EspNICClass EspNIC;
EspNICClass::EspNICClass():_ctx(nullptr) {}
EspNICClass::~EspNICClass() { end(); }
bool EspNICClass::context() { return _ctx || !NIC_ENTRY(0,nullptr,&_ctx); }
bool EspNICClass::begin(nic_mode_t mode) { return mode==NIC_MODE_SPI ? beginSPI():mode==NIC_MODE_SDIO ? beginSDIO():false; }
bool EspNICClass::beginSPI(int32_t clk,int32_t mi,int32_t mo,int32_t cs,int32_t hs,int32_t dr,int32_t reset,uint32_t hz) {
    nic_spi_config_t c={clk,mi,mo,cs,hs,dr,reset,hz,ESP_NIC_SPI_MODE}; return beginSPI(c);
}
bool EspNICClass::beginSPI(const nic_spi_config_t &c) { return context() && !NIC_ENTRY(2,_ctx,const_cast<nic_spi_config_t *>(&c)); }
bool EspNICClass::beginSDIO(uint8_t width,uint32_t hz) {
    nic_sdio_config_t c={ESP_NIC_SDIO_CLK,ESP_NIC_SDIO_CMD,ESP_NIC_SDIO_D0,ESP_NIC_SDIO_D1,ESP_NIC_SDIO_D2,ESP_NIC_SDIO_D3,ESP_NIC_SDIO_RESET,hz,width,1}; return beginSDIO(c);
}
bool EspNICClass::beginSDIO(const nic_sdio_config_t &c) { return context() && !NIC_ENTRY(3,_ctx,const_cast<nic_sdio_config_t *>(&c)); }
bool EspNICClass::beginUART(int32_t tx,int32_t rx,uint32_t baud,int32_t reset,uint8_t port) { nic_uart_config_t c={tx,rx,reset,baud,port}; return beginUART(c); }
bool EspNICClass::beginUART(const nic_uart_config_t &c) { return context() && !NIC_ENTRY(4,_ctx,const_cast<nic_uart_config_t *>(&c)); }
void EspNICClass::end() { if (_ctx) NIC_ENTRY(1,_ctx,nullptr); _ctx=nullptr; }
int EspNICClass::poll() { return NIC_ENTRY(7,_ctx,nullptr); }
bool EspNICClass::isReady() { return NIC_ENTRY(5,_ctx,nullptr)>0; }
nic_status_t EspNICClass::getStatus() { return static_cast<nic_status_t>(NIC_ENTRY(6,_ctx,nullptr)); }
int EspNICClass::lastError() { return NIC_ENTRY(13,_ctx,nullptr); }
const char *EspNICClass::firmwareVersion() { const char *version=""; NIC_ENTRY(14,_ctx,&version); return version; }
bool EspNICClass::connectWiFi(const char *s,const char *p,uint32_t timeout) { WifiArgs a={s,p,timeout}; return !NIC_ENTRY(8,_ctx,&a); }
bool EspNICClass::disconnectWiFi() { return !NIC_ENTRY(9,_ctx,nullptr); }
IPAddress EspNICClass::getLocalIP() { IPArgs a={}; NIC_ENTRY(10,_ctx,&a); return IPAddress(a.bytes[0],a.bytes[1],a.bytes[2],a.bytes[3]); }
int EspNICClass::sendPacket(const uint8_t *p,size_t n) { PacketArgs a={p,n}; return NIC_ENTRY(11,_ctx,&a); }
int EspNICClass::readPacket(uint8_t *p,size_t n) { ReadArgs a={p,n}; return NIC_ENTRY(12,_ctx,&a); }
int EspNICClass::scanNetworks(nic_ap_record_t *p,size_t n,uint32_t timeout) { ScanArgs a={p,n,timeout}; return NIC_ENTRY(15,_ctx,&a); }
bool EspNICClass::softAP(const char *s,const char *p,uint8_t ch,uint8_t n,bool hidden) { APArgs a={s,p,ch,n,hidden}; return !NIC_ENTRY(16,_ctx,&a); }
bool EspNICClass::stopSoftAP() { return !NIC_ENTRY(17,_ctx,nullptr); }
IPAddress EspNICClass::getAPIP() { IPArgs a={}; NIC_ENTRY(29,_ctx,&a); return IPAddress(a.bytes[0],a.bytes[1],a.bytes[2],a.bytes[3]); }
bool EspNICClass::enableBLE() { return !NIC_ENTRY(18,_ctx,nullptr); }
int EspNICClass::sendHCI(const uint8_t *p,size_t n) { PacketArgs a={p,n}; return NIC_ENTRY(19,_ctx,&a); }
int EspNICClass::customRPC(uint32_t id,const uint8_t *p,size_t n,uint8_t *reply,size_t &size,uint32_t timeout) { CustomArgs a={id,p,n,reply,&size,timeout}; return NIC_ENTRY(20,_ctx,&a); }
void EspNICClass::onEvent(nic_event_callback_t cb,void *ctx) { if (context()) { CallbackArgs a={cb,ctx}; NIC_ENTRY(21,_ctx,&a); } }
void EspNICClass::onHCI(nic_event_callback_t cb,void *ctx) { if (context()) { CallbackArgs a={cb,ctx}; NIC_ENTRY(22,_ctx,&a); } }
bool EspNICClass::updateFirmware(nic_firmware_callback_t cb,void *ctx,const char *s,const char *p) { FlashArgs a={cb,ctx,s,p}; return !NIC_ENTRY(23,_ctx,&a); }
int EspNICClass::sendCustomCommand(uint8_t id,const uint8_t *p,size_t n) { CommandArgs a={id,p,n}; return NIC_ENTRY(24,_ctx,&a); }
int EspNICClass::sendCustomMessage(const String &s) { return NIC_ENTRY(25,_ctx,const_cast<String *>(&s)); }
int EspNICClass::displayText(int16_t x,int16_t y,const char *s) { TextArgs a={x,y,s}; return NIC_ENTRY(26,_ctx,&a); }
int EspNICClass::displayProgress(uint8_t percent,const char *s) { ProgressArgs a={percent,s}; return NIC_ENTRY(27,_ctx,&a); }
#if defined(ARDUINO_ARCH_STM32)
bool EspNICClass::configureSDIO(uint32_t clk,int (*setup)(const nic_sdio_config_t *),void (*close)(),bool dma) { SDArgs a={clk,setup,close,dma}; return context() && !NIC_ENTRY(28,_ctx,&a); }
bool EspNICClass::configureMemory(void *(*allocate)(size_t),void (*release)(void *)) { MemoryArgs a={allocate,release}; return context() && !NIC_ENTRY(30,_ctx,&a); }
bool EspNICClass::configureSPIDMA(int (*setup)(SPIClass &)) { DMAArgs a={setup}; return context() && !NIC_ENTRY(31,_ctx,&a); }
#endif
EspNICClient::EspNICClient():EspNICClient(false) {}
EspNICClient::EspNICClient(bool secure):_impl(nullptr),secure_(secure) {}
EspNICClient::EspNICClient(nic_client_ctx_t *p):_impl(p),secure_(false) {}
EspNICClient::~EspNICClient() { if (_impl) NIC_ENTRY(33,_impl,nullptr); }
EspNICClient::EspNICClient(EspNICClient &&c) noexcept:_impl(c._impl),secure_(c.secure_) { c._impl=nullptr; }
EspNICClient &EspNICClient::operator=(EspNICClient &&c) noexcept { if (this!=&c) { if (_impl) NIC_ENTRY(33,_impl,nullptr); _impl=c._impl; secure_=c.secure_; c._impl=nullptr; } return *this; }
bool EspNICClient::context() { return _impl || !NIC_ENTRY(secure_ ? 44:32,nullptr,&_impl); }
void EspNICClient::setCertificate(const char *pem) { if (!_impl) NIC_ENTRY(44,nullptr,&_impl); if (_impl) NIC_ENTRY(43,_impl,const_cast<char *>(pem)); }
int EspNICClient::connect(IPAddress ip,uint16_t port) { IPArgs a={{ip[0],ip[1],ip[2],ip[3]},port}; if (secure_ && !_impl) NIC_ENTRY(44,nullptr,&_impl); return context() ? NIC_ENTRY(34,_impl,&a):0; }
int EspNICClient::connect(const char *name,uint16_t port) { HostArgs a={name,port}; if (secure_ && !_impl) NIC_ENTRY(44,nullptr,&_impl); return context() ? NIC_ENTRY(35,_impl,&a):0; }
size_t EspNICClient::write(uint8_t v) { return write(&v,1); }
size_t EspNICClient::write(const uint8_t *p,size_t n) { PacketArgs a={p,n}; int rc=_impl ? NIC_ENTRY(36,_impl,&a):0; return rc>0 ? rc:0; }
int EspNICClient::available() { return _impl ? NIC_ENTRY(37,_impl,nullptr):0; }
int EspNICClient::read() { uint8_t v; return read(&v,1)==1 ? v:-1; }
int EspNICClient::read(uint8_t *p,size_t n) { ReadArgs a={p,n}; return _impl ? NIC_ENTRY(38,_impl,&a):-1; }
int EspNICClient::peek() { return _impl ? NIC_ENTRY(39,_impl,nullptr):-1; }
void EspNICClient::flush() { if (_impl) NIC_ENTRY(40,_impl,nullptr); }
void EspNICClient::stop() { if (_impl) NIC_ENTRY(41,_impl,nullptr); }
uint8_t EspNICClient::connected() { return _impl ? NIC_ENTRY(42,_impl,nullptr):0; }
EspNICClient::operator bool() { return connected(); }
EspNICServer::EspNICServer(uint16_t port):_impl(nullptr),port_(port) {}
EspNICServer::~EspNICServer() { if (_impl) NIC_ENTRY(65,_impl,nullptr); }
void EspNICServer::begin() { begin(port_); }
bool EspNICServer::begin(uint16_t port) { port_=port; if (!_impl) NIC_ENTRY(64,nullptr,&_impl); return _impl && NIC_ENTRY(66,_impl,&port)>0; }
EspNICClient EspNICServer::accept() { nic_client_ctx_t *c=nullptr; if (_impl) NIC_ENTRY(67,_impl,&c); return EspNICClient(c); }
EspNICClient EspNICServer::available() { return accept(); }
void EspNICServer::end() { if (_impl) NIC_ENTRY(68,_impl,nullptr); }
size_t EspNICServer::write(uint8_t) { return 0; }
size_t EspNICServer::write(const uint8_t *,size_t) { return 0; }
EspNICUDP::EspNICUDP():_impl(nullptr) {}
EspNICUDP::~EspNICUDP() { if (_impl) NIC_ENTRY(97,_impl,nullptr); }
bool EspNICUDP::context() { return _impl || !NIC_ENTRY(96,nullptr,&_impl); }
uint8_t EspNICUDP::begin(uint16_t port) { return context() ? NIC_ENTRY(98,_impl,&port):0; }
void EspNICUDP::stop() { if (_impl) NIC_ENTRY(99,_impl,nullptr); }
int EspNICUDP::beginPacket(IPAddress ip,uint16_t port) { IPArgs a={{ip[0],ip[1],ip[2],ip[3]},port}; return context() ? NIC_ENTRY(100,_impl,&a):0; }
int EspNICUDP::beginPacket(const char *name,uint16_t port) { HostArgs a={name,port}; return context() ? NIC_ENTRY(101,_impl,&a):0; }
int EspNICUDP::endPacket() { return _impl ? NIC_ENTRY(102,_impl,nullptr):0; }
size_t EspNICUDP::write(uint8_t v) { return write(&v,1); }
size_t EspNICUDP::write(const uint8_t *p,size_t n) { PacketArgs a={p,n}; return _impl ? NIC_ENTRY(103,_impl,&a):0; }
int EspNICUDP::parsePacket() { return _impl ? NIC_ENTRY(104,_impl,nullptr):0; }
int EspNICUDP::available() { return _impl ? NIC_ENTRY(105,_impl,nullptr):0; }
int EspNICUDP::read() { uint8_t v; return read(&v,1)==1 ? v:-1; }
int EspNICUDP::read(uint8_t *p,size_t n) { ReadArgs a={p,n}; return _impl ? NIC_ENTRY(106,_impl,&a):-1; }
int EspNICUDP::read(char *p,size_t n) { return read(reinterpret_cast<uint8_t *>(p),n); }
int EspNICUDP::peek() { return _impl ? NIC_ENTRY(107,_impl,nullptr):-1; }
void EspNICUDP::flush() { if (_impl) NIC_ENTRY(108,_impl,nullptr); }
IPAddress EspNICUDP::remoteIP() { IPArgs a={}; if (_impl) NIC_ENTRY(109,_impl,&a); return IPAddress(a.bytes[0],a.bytes[1],a.bytes[2],a.bytes[3]); }
uint16_t EspNICUDP::remotePort() { return _impl ? NIC_ENTRY(110,_impl,nullptr):0; }

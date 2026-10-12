#pragma once
#include <Arduino.h>
#include <IPAddress.h>
#include <Client.h>
#include <Server.h>
#include <Udp.h>
#include "esp_nic_types.h"
#include "esp_nic_config.h"
struct nic_internal_ctx_t;
struct nic_client_ctx_t;
struct nic_server_ctx_t;
struct nic_udp_ctx_t;
#if defined(ARDUINO_ARCH_STM32)
class SPIClass;
#endif
/** One coprocessor session. Call begin/poll/RPC/end from one task. Network
 * callbacks enqueue data internally. SPI/SDIO config is passed at runtime. */
class EspNICClass {
public:
    EspNICClass();
    ~EspNICClass();
    EspNICClass(const EspNICClass &)=delete;
    EspNICClass &operator=(const EspNICClass &)=delete;
    /** Select the board default bus. UART needs explicit pins via beginUART(). */
    bool begin(nic_mode_t mode=ESP_NIC_DEFAULT_MODE);
    /** @brief Initialize the master SPI bus and negotiate with the slave.
     * @param sclk Master clock output GPIO.
     * @param miso Master input GPIO, connected to slave MISO.
     * @param mosi Master output GPIO, connected to slave MOSI.
     * @param cs Active-low chip-select GPIO.
     * @param handshake Input GPIO driven by the slave when a transaction is armed.
     * @param dataready Input GPIO driven by the slave when data is queued.
     * @param reset Output connected to slave EN; -1 disables reset control.
     * @param freq_hz SPI clock in Hz, initially use 10000000 or 20000000.
     * @return True on initialization/negotiation success; otherwise lastError(). */
    bool beginSPI(int32_t sclk=ESP_NIC_SPI_SCLK,int32_t miso=ESP_NIC_SPI_MISO,int32_t mosi=ESP_NIC_SPI_MOSI,
        int32_t cs=ESP_NIC_SPI_CS,int32_t handshake=ESP_NIC_SPI_HANDSHAKE,int32_t dataready=ESP_NIC_SPI_DATAREADY,
        int32_t reset=ESP_NIC_SPI_RESET,uint32_t freq_hz=ESP_NIC_SPI_HZ);
    /** @param config Runtime pins, mode 2 and clock in Hz.
     * @return True on success; otherwise lastError(). */
    bool beginSPI(const nic_spi_config_t &config);
    /** @brief Initialize function-1 packet SDIO; external pull-ups are required.
     * @param width Bus width, 1 or 4.
     * @param freq_hz SDIO clock in Hz, 20000000 or 40000000.
     * @return True on success; unsupported silicon returns false. */
    bool beginSDIO(uint8_t width=ESP_NIC_SDIO_WIDTH,uint32_t freq_hz=ESP_NIC_SDIO_HZ);
    /** @param config Runtime SDIO GPIOs, width, slot and Hz.
     * @return True on success; STM32 requires board setup registration first. */
    bool beginSDIO(const nic_sdio_config_t &config);
    /** @brief Reserve a hardware UART; console port 0 is rejected.
     * @param tx Master output GPIO connected to slave RX.
     * @param rx Master input GPIO connected to slave TX.
     * @param baud Matching slave baud rate, 115200..3000000.
     * @param reset Output connected to slave EN, or -1.
     * @param port Available hardware UART index other than console 0.
     * @return True on success, false with lastError() on failure. */
    bool beginUART(int32_t tx,int32_t rx,uint32_t baud=ESP_NIC_UART_BAUD,int32_t reset=-1,uint8_t port=1);
    /** @param config Runtime GPIOs, baud and UART index.
     * @return True on success, false on invalid pins/unsupported hardware. */
    bool beginUART(const nic_uart_config_t &config);
    /** @brief Release interfaces and transport; stop clients beforehand. */
    void end();
    /** Pump transport and network traffic frequently; negative return is error. */
    int poll();
    /** @return True after protocol negotiation, independent of Wi-Fi/IP. */
    bool isReady();
    /** @return Public link/association/IP state. */
    nic_status_t getStatus();
    /** @return Latest numeric transport/RPC error; 0 if no error recorded. */
    int lastError();
    /** @return Borrowed version text, empty before the slave announces it. */
    const char *firmwareVersion();
    /** @brief Wait for association + DHCP on ESP32; association only on STM32.
     * @param ssid NUL-terminated network name, at most 32 bytes.
     * @param password NUL-terminated credential; empty for an open network.
     * @param timeout_ms Maximum association/DHCP wait in milliseconds.
     * @return True when ready, false with lastError() otherwise. */
    bool connectWiFi(const char *ssid,const char *password,uint32_t timeout_ms=30000);
    /** @return True when the STA disconnect RPC succeeds. */
    bool disconnectWiFi();
    /** @return Host IPv4 address, or zero when unavailable/raw-only. */
    IPAddress getLocalIP();
    /** @brief Scan into caller-owned storage; 5 GHz requires a C5 slave.
     * @param records Output array; strings are terminated inside each record.
     * @param capacity Number of record slots, 1..32.
     * @param timeout_ms Maximum scan wait in milliseconds.
     * @return Stored record count, or a negative nic_error_t/vendor error. */
    int scanNetworks(nic_ap_record_t *records,size_t capacity,uint32_t timeout_ms=15000);
    /** Start radio AP and ESP32 host DHCP server at 192.168.4.1. STM32 exposes
     * the radio AP; an external host IP/DHCP stack is required there.
     * @param ssid Nonempty NUL-terminated AP name, at most 32 bytes.
     * @param password Empty for open AP, or 8..63 bytes for WPA2.
     * @param channel Radio channel; slave SDK/regulatory domain validates it.
     * @param clients Maximum clients, 1..10.
     * @param hidden Suppress SSID broadcast if true.
     * @return True on success; otherwise lastError(). */
    bool softAP(const char *ssid,const char *password="",uint8_t channel=1,uint8_t clients=4,bool hidden=false);
    /** @return True when AP stop succeeds; STA operation is retained. */
    bool stopSoftAP();
    /** @return ESP32 host AP address, or zero without an AP IP stack. */
    IPAddress getAPIP();
    /** @return True when a BT-enabled slave enables its HCI controller. */
    bool enableBLE();
    /** @param packet H4 packet including the leading HCI packet-type byte.
     * @param length Complete H4 byte count.
     * @return Queued byte count or negative error. */
    int sendHCI(const uint8_t *packet,size_t length);
    /** @param data Complete Ethernet frame.
     * @param length Frame bytes, at most 1514.
     * @return Queued bytes or negative error; poll() completes transmission. */
    int sendPacket(const uint8_t *data,size_t length);
    /** @param buffer Caller-owned output storage.
     * @param capacity Available bytes; a short buffer preserves the next frame.
     * @return Frame length, 0 when empty, or negative error. */
    int readPacket(uint8_t *buffer,size_t capacity);
    /** Coprocessor CustomRpc; data <=1400 bytes. Never sent over the Wi-Fi LAN.
     * @param id Application command identifier.
     * @param data Borrowed payload, null only when length is zero.
     * @param length Payload byte count, at most 1400.
     * @param reply Caller-owned reply storage; null allowed for an empty reply.
     * @param reply_length Capacity on entry, actual/required bytes on return.
     * @param timeout_ms Maximum response wait in milliseconds.
     * @return 0 on success, negative error otherwise; no silent truncation. */
    int customRPC(uint32_t id,const uint8_t *data,size_t length,uint8_t *reply,size_t &reply_length,uint32_t timeout_ms=5000);
    /** @param id Application ID; 1/2/3 reserved for display convenience messages.
     * @param data Borrowed bytes, null only for length 0.
     * @param length Payload bytes, at most 1400.
     * @return Byte count or negative error. */
    int sendCustomCommand(uint8_t id,const uint8_t *data,size_t length);
    /** @param message UTF-8 message, at most 1400 bytes.
     * @return Message length or negative error. */
    int sendCustomMessage(const String &message);
    /** @param x Signed display pixel X coordinate.
     * @param y Signed display pixel Y coordinate.
     * @param text NUL-terminated UTF-8, copied into a bounded command.
     * @return Encoded command bytes or negative error. */
    int displayText(int16_t x,int16_t y,const char *text);
    /** @param percent Progress from 0 to 100.
     * @param label Optional NUL-terminated UTF-8 label.
     * @return Encoded command bytes or negative error. */
    int displayProgress(uint8_t percent,const char *label=nullptr);
    /** @param callback Receives event ID/payload inside poll; null unregisters.
     * @param context Borrowed user context passed unchanged to callback. */
    void onEvent(nic_event_callback_t callback,void *context=nullptr);
    /** @param callback Receives HCI type/payload; null unregisters.
     * @param context Borrowed user context passed unchanged to callback. */
    void onHCI(nic_event_callback_t callback,void *context=nullptr);
    /** Release the bus, run a user flasher callback, reopen the previous bus.
     * @param callback Programs, verifies, resets the slave and releases flasher UART.
     * @param context Borrowed callback context.
     * @param ssid Optional STA name for recovery; paired with password.
     * @param password Optional paired credential, including empty/open network.
     * @return True only if callback, bus restore and requested DHCP succeed. */
    bool updateFirmware(nic_firmware_callback_t callback,void *context,const char *ssid=nullptr,const char *password=nullptr);
#if defined(ARDUINO_ARCH_STM32)
    /** @param kernel_clock Actual SDMMC kernel clock in Hz.
     * @param setup Board clocks/pin AF setup; returns 0 or negative error.
     * @param close Optional hardware cleanup callback.
     * @param dma Opt in to F4/F7 DMA2 Stream3 Channel4 or H7 IDMA.
     * @return True when registered before begin, false if transport is active. */
    bool configureSDIO(uint32_t kernel_clock,int (*setup)(const nic_sdio_config_t *),void (*close)()=nullptr,bool dma=false);
    /** Called with the selected SPIClass; link both HAL DMA channels/IRQs.
     * @param setup Board DMA/IRQ setup returning 0; null restores polling.
     * @return True when registered before begin; false when transport is active. */
    bool configureSPIDMA(int (*setup)(SPIClass &));
    /** Paired 32-byte aligned DMA SRAM allocator, installed before begin.
     * @param allocate Returns 32-byte aligned DMA SRAM; null with release null resets.
     * @param release Frees the same allocation; must be paired with allocate.
     * @return True before begin; false on a mismatched pair or active transport. */
    bool configureMemory(void *(*allocate)(size_t),void (*release)(void *));
#endif
private:
    nic_internal_ctx_t *_ctx;
    bool context();
};
extern EspNICClass EspNIC;
/** Arduino TCP Client. Socket/context ownership is unique and movable. */
class EspNICClient : public Client {
public:
    EspNICClient();
    virtual ~EspNICClient();
    EspNICClient(const EspNICClient &)=delete;
    EspNICClient &operator=(const EspNICClient &)=delete;
    EspNICClient(EspNICClient &&) noexcept;
    EspNICClient &operator=(EspNICClient &&) noexcept;
    int connect(IPAddress,uint16_t) override;
    int connect(const char *,uint16_t) override;
    size_t write(uint8_t) override;
    size_t write(const uint8_t *,size_t) override;
    int available() override;
    int read() override;
    int read(uint8_t *,size_t) override;
    int peek() override;
    void flush() override;
    void stop() override;
    uint8_t connected() override;
    operator bool() override;
    using Print::write;
protected:
    explicit EspNICClient(bool secure);
    void setCertificate(const char *);
private:
    friend class EspNICServer;
    explicit EspNICClient(nic_client_ctx_t *);
    nic_client_ctx_t *_impl;
    bool secure_;
    bool context();
};
/** Verified TLS on ESP32. PEM buffer must outlive the connection. */
class EspNICSecureClient : public EspNICClient {
public:
    EspNICSecureClient():EspNICClient(true) {}
    void setCACert(const char *pem) { setCertificate(pem); }
};
class EspNICServer : public Server {
public:
    explicit EspNICServer(uint16_t port=80);
    ~EspNICServer();
    EspNICServer(const EspNICServer &)=delete;
    EspNICServer &operator=(const EspNICServer &)=delete;
    void begin() override;
    bool begin(uint16_t);
    EspNICClient accept();
    EspNICClient available();
    void end();
    size_t write(uint8_t) override;
    size_t write(const uint8_t *,size_t) override;
    using Print::write;
private:
    nic_server_ctx_t *_impl; uint16_t port_;
};
/** UDP datagrams <=1472 bytes; oversized writes fail without truncation. */
class EspNICUDP : public UDP {
public:
    EspNICUDP();
    ~EspNICUDP();
    EspNICUDP(const EspNICUDP &)=delete;
    EspNICUDP &operator=(const EspNICUDP &)=delete;
    uint8_t begin(uint16_t) override;
    void stop() override;
    int beginPacket(IPAddress,uint16_t) override;
    int beginPacket(const char *,uint16_t) override;
    int endPacket() override;
    size_t write(uint8_t) override;
    size_t write(const uint8_t *,size_t) override;
    int parsePacket() override;
    int available() override;
    int read() override;
    int read(unsigned char *,size_t) override;
    int read(char *,size_t) override;
    int peek() override;
    void flush() override;
    IPAddress remoteIP() override;
    uint16_t remotePort() override;
    using Print::write;
private:
    nic_udp_ctx_t *_impl;
    bool context();
};

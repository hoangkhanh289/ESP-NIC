#pragma once
#include <Arduino.h>
#include <IPAddress.h>
#include "esp_nic_types.h"
#include "esp_nic_config.h"
struct nic_slave_ctx_t;
/** Arduino ESP coprocessor endpoint. begin() arms real DMA/UART hardware;
 * isMasterConnected() becomes true only after a valid Hosted negotiation.
 * One owner task calls poll(). Wi-Fi callbacks only enqueue bounded packets.
 * A wireless ESP is required; P4 and STM32 return NIC_UNSUPPORTED. */
class EspNICSlaveClass {
public:
    EspNICSlaveClass();
    ~EspNICSlaveClass();
    EspNICSlaveClass(const EspNICSlaveClass &)=delete;
    EspNICSlaveClass &operator=(const EspNICSlaveClass &)=delete;
    /** @brief Arm the slave Mode-2 DMA SPI peripheral.
     * @param sclk Slave clock input GPIO; -1 selects chip default.
     * @param miso Slave output GPIO; -1 selects chip default.
     * @param mosi Slave input GPIO; -1 selects chip default.
     * @param cs Slave active-low select input; -1 selects default.
     * @param handshake Output to master HS, high when a transaction is armed.
     * @param dataready Output to master DR, high when slave data is queued.
     * @return True when armed, not proof of a connected master; otherwise lastError(). */
    bool beginSPI(int32_t sclk=-1,int32_t miso=-1,int32_t mosi=-1,int32_t cs=-1,int32_t handshake=-1,int32_t dataready=-1);
    /** @brief Arm packet SDIO using fixed silicon pin routing and external pull-ups.
     * @return True on supported wireless silicon, false/unsupported otherwise. */
    bool beginSDIO();
    /** @param tx Slave TX GPIO connected to master RX.
     * @param rx Slave RX GPIO connected to master TX.
     * @param baud Same baud as master, 115200..3000000.
     * @param port Available hardware UART other than console 0.
     * @return True when armed, false with lastError() otherwise. */
    bool beginUART(int32_t tx,int32_t rx,uint32_t baud=921600,uint8_t port=1);
    /** @brief Stop bus/radio callbacks and release bounded DMA buffers. */
    void end();
    /** @return Nonnegative work result, or negative error. Single owner task only. */
    int poll();
    /** @return Last numeric slave error; 0 when no error is recorded. */
    int lastError();
    /** @return True only after valid Hosted negotiation/ACK. */
    bool isMasterConnected() const;
    /** @return True after radio STA association. */
    bool isWiFiConnected() const;
    /** @return A copied SSID String, empty when unavailable. */
    String getSSID() const;
    /** @return RSSI in dBm, or 0 when unavailable. */
    int8_t getRSSI();
    /** Host owns the IP stack; the slave has no separate DHCP address. */
    IPAddress getIP() const;
    /** @return Snapshot of bus packet/byte counters and dropped queue entries. */
    nic_slave_stats_t statistics() const;
    uint32_t getTxPackets() const;
    uint32_t getRxPackets() const;
    uint32_t getTxBytes() const;
    uint32_t getRxBytes() const;
    /** @param ssid NUL-terminated STA name, at most 32 bytes.
     * @param password Borrowed NUL-terminated credential, empty for open STA.
     * @return True when radio setup succeeds; does not wait for association. */
    bool setLocalWiFi(const char *ssid,const char *password);
    typedef void (*CustomCommandCallback)(uint8_t,const uint8_t *,size_t);
    typedef void (*CustomMessageCallback)(const String &);
    typedef void (*DisplayTextCallback)(int16_t,int16_t,const String &);
    typedef void (*DisplayProgressCallback)(uint8_t,const String &);
    /** @param callback Receives ID/payload inside poll; null unregisters.
     * Copy borrowed bytes to defer slow work. IDs 1/2/3 also dispatch display helpers. */
    void onCommand(CustomCommandCallback callback);
    /** @param callback Receives UTF-8 message inside poll; null unregisters. */
    void onMessage(CustomMessageCallback callback);
    /** @param callback Receives signed X/Y pixel coordinates and UTF-8 text.
     * Null unregisters. Keep rendering short so bus/network queues keep draining. */
    void onDisplayText(DisplayTextCallback callback);
    /** @param callback Receives percent 0..100 and label inside poll; null unregisters. */
    void onDisplayProgress(DisplayProgressCallback callback);
    /** @param id Application event ID.
     * @param data Borrowed payload; null allowed only with length 0.
     * @param length Bytes, at most 1400.
     * @return Payload length or negative error; requires negotiated master. */
    int sendEventToMaster(uint32_t id,const uint8_t *data,size_t length);
    /** @param message UTF-8 event message, at most 1400 bytes.
     * @return Message bytes or negative error; requires negotiated master. */
    int sendMessageToMaster(const String &message);
private:
    nic_slave_ctx_t *_ctx;
    bool context();
};
extern EspNICSlaveClass EspNICSlave;

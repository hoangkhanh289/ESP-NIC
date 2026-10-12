#pragma once
#include <stdint.h>
#include <stddef.h>
/** Link, association and host DHCP are distinct states. */
enum nic_status_t : uint8_t { NIC_DISCONNECTED=0,NIC_CONNECTED=1,NIC_WIFI_CONNECTED=2,NIC_IP_READY=3,NIC_ERROR=4 };
enum nic_mode_t : uint8_t { NIC_MODE_SPI=0,NIC_MODE_SDIO=1,NIC_MODE_UART=2 };
/** Negative transport errors; zero is success. Vendor RPC errors may be other
 * negative ESP-IDF values. No positive error is reported as success. */
enum nic_error_t : int { NIC_OK=0,NIC_IO=-1,NIC_INVALID=-2,NIC_NO_MEMORY=-3,NIC_AGAIN=-4,
    NIC_UNSUPPORTED=-5,NIC_TIMEOUT=-6,NIC_PROTOCOL=-7,NIC_BUSY=-8,NIC_NOT_CONNECTED=-9 };
/** All pins are master GPIO numbers, not slave GPIO labels. -1 disables reset. */
struct nic_spi_config_t {
    int32_t sclk,miso,mosi,cs; ///< Master SPI GPIOs, active-low CS.
    int32_t handshake,dataready; ///< Master inputs driven by slave HS/DR outputs.
    int32_t reset; ///< Master output to slave EN, or -1 when absent.
    uint32_t frequency; ///< SPI clock in Hz.
    uint8_t mode; ///< Hosted SPI mode, must be 2 for supplied firmware.
};
struct nic_sdio_config_t {
    int32_t clk,cmd,d0,d1,d2,d3; ///< Master SDIO GPIOs; slave silicon pins are fixed.
    int32_t reset; ///< Master output to slave EN, or -1.
    uint32_t frequency; ///< SDMMC clock in Hz, normally 20000000 or 40000000.
    uint8_t width; ///< Bus width, 1 or 4; external pull-ups are required.
    uint8_t slot; ///< Host SDMMC slot, normally 1.
};
struct nic_uart_config_t {
    int32_t tx,rx; ///< Master GPIOs; cross-connect to slave RX/TX.
    int32_t reset; ///< Master output to slave EN, or -1.
    uint32_t baud; ///< Baud 115200..3000000, matching the slave configuration.
    uint8_t port; ///< Hardware UART index other than console 0.
};
struct nic_ap_record_t {
    char ssid[33]; ///< NUL-terminated network name, at most 32 bytes.
    uint8_t bssid[6]; ///< AP MAC address in network byte order.
    uint8_t channel; ///< Primary radio channel.
    uint8_t authmode; ///< Slave ESP-IDF wifi_auth_mode_t numeric value.
    int32_t rssi; ///< Received signal strength in dBm.
};
struct nic_slave_stats_t {
    uint32_t tx_packets,rx_packets; ///< Bus packets sent/received.
    uint32_t tx_bytes,rx_bytes; ///< Bus payload bytes sent/received.
    uint32_t dropped; ///< Queue/validation drops, cumulative since begin.
};
/** Data belongs to the library and is valid only during the callback. */
typedef void (*nic_event_callback_t)(uint32_t id,const uint8_t *data,size_t length,void *context);
/** Return true only after flashing, verifying and resetting into application. */
typedef bool (*nic_firmware_callback_t)(void *context);

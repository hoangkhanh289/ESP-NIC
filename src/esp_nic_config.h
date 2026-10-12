#pragma once
/* These defaults are compiled at the call site. Explicit configs are runtime
 * values; changing this header cannot change options baked into an archive. */
#if defined(ARDUINO_ARCH_ESP32)
#include <esp_arduino_version.h>
#if ESP_ARDUINO_VERSION_MAJOR!=3 || ESP_ARDUINO_VERSION_MINOR!=3 || ESP_ARDUINO_VERSION_PATCH!=12
#error "ESP-NIC binaries require Arduino-ESP32 3.3.12. Rebuild archives for another SDK."
#endif
#if defined(CONFIG_IDF_TARGET_ESP32)
#define NIC_DEFAULT_CLK 18
#define NIC_DEFAULT_MISO 19
#define NIC_DEFAULT_MOSI 23
#define NIC_DEFAULT_CS 5
#define NIC_DEFAULT_HS 26
#define NIC_DEFAULT_DR 27
#define NIC_DEFAULT_RESET 25
#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define NIC_DEFAULT_CLK 12
#define NIC_DEFAULT_MISO 13
#define NIC_DEFAULT_MOSI 11
#define NIC_DEFAULT_CS 10
#define NIC_DEFAULT_HS 4
#define NIC_DEFAULT_DR 5
#define NIC_DEFAULT_RESET 6
#elif defined(CONFIG_IDF_TARGET_ESP32P4)
#define NIC_DEFAULT_CLK 24
#define NIC_DEFAULT_MISO 25
#define NIC_DEFAULT_MOSI 26
#define NIC_DEFAULT_CS 27
#define NIC_DEFAULT_HS 28
#define NIC_DEFAULT_DR 29
#define NIC_DEFAULT_RESET 30
#else
#define NIC_DEFAULT_CLK 6
#define NIC_DEFAULT_MISO 5
#define NIC_DEFAULT_MOSI 7
#define NIC_DEFAULT_CS 10
#define NIC_DEFAULT_HS 3
#define NIC_DEFAULT_DR 4
#define NIC_DEFAULT_RESET 2
#endif
#elif defined(ARDUINO_ARCH_STM32)
#if !defined(STM32F446xx) && !defined(STM32G474xx) && !defined(STM32F767xx) && !defined(STM32H743xx)
#error "ESP-NIC STM32 binaries currently cover F446/G474/F767/H743 with STM32duino 3.0.0."
#endif
#define NIC_DEFAULT_CLK PA5
#define NIC_DEFAULT_MISO PA6
#define NIC_DEFAULT_MOSI PA7
#define NIC_DEFAULT_CS PB6
#define NIC_DEFAULT_HS PB0
#define NIC_DEFAULT_DR PB1
#define NIC_DEFAULT_RESET PB10
#else
#error "No ESP-NIC binary for this architecture."
#endif
#ifndef ESP_NIC_SPI_SCLK
#define ESP_NIC_SPI_SCLK NIC_DEFAULT_CLK
#endif
#ifndef ESP_NIC_SPI_MISO
#define ESP_NIC_SPI_MISO NIC_DEFAULT_MISO
#endif
#ifndef ESP_NIC_SPI_MOSI
#define ESP_NIC_SPI_MOSI NIC_DEFAULT_MOSI
#endif
#ifndef ESP_NIC_SPI_CS
#define ESP_NIC_SPI_CS NIC_DEFAULT_CS
#endif
#ifndef ESP_NIC_SPI_HANDSHAKE
#define ESP_NIC_SPI_HANDSHAKE NIC_DEFAULT_HS
#endif
#ifndef ESP_NIC_SPI_DATAREADY
#define ESP_NIC_SPI_DATAREADY NIC_DEFAULT_DR
#endif
#ifndef ESP_NIC_SPI_RESET
#define ESP_NIC_SPI_RESET NIC_DEFAULT_RESET
#endif
#ifndef ESP_NIC_SPI_HZ
#define ESP_NIC_SPI_HZ 20000000UL
#endif
#ifndef ESP_NIC_SPI_MODE
#define ESP_NIC_SPI_MODE 2
#endif
#ifndef ESP_NIC_DEFAULT_MODE
#if defined(CONFIG_IDF_TARGET_ESP32P4)
#define ESP_NIC_DEFAULT_MODE NIC_MODE_SDIO
#else
#define ESP_NIC_DEFAULT_MODE NIC_MODE_SPI
#endif
#endif
#if defined(CONFIG_IDF_TARGET_ESP32)
#define NIC_SD_CLK 14
#define NIC_SD_CMD 15
#define NIC_SD_D0 2
#define NIC_SD_D1 4
#define NIC_SD_D2 12
#define NIC_SD_D3 13
#else
#define NIC_SD_CLK 18
#define NIC_SD_CMD 19
#define NIC_SD_D0 14
#define NIC_SD_D1 15
#define NIC_SD_D2 16
#define NIC_SD_D3 17
#endif
#ifndef ESP_NIC_SDIO_CLK
#define ESP_NIC_SDIO_CLK NIC_SD_CLK
#endif
#ifndef ESP_NIC_SDIO_CMD
#define ESP_NIC_SDIO_CMD NIC_SD_CMD
#endif
#ifndef ESP_NIC_SDIO_D0
#define ESP_NIC_SDIO_D0 NIC_SD_D0
#endif
#ifndef ESP_NIC_SDIO_D1
#define ESP_NIC_SDIO_D1 NIC_SD_D1
#endif
#ifndef ESP_NIC_SDIO_D2
#define ESP_NIC_SDIO_D2 NIC_SD_D2
#endif
#ifndef ESP_NIC_SDIO_D3
#define ESP_NIC_SDIO_D3 NIC_SD_D3
#endif
#ifndef ESP_NIC_SDIO_RESET
#if defined(CONFIG_IDF_TARGET_ESP32P4)
#define ESP_NIC_SDIO_RESET 54
#else
#define ESP_NIC_SDIO_RESET NIC_DEFAULT_RESET
#endif
#endif
#ifndef ESP_NIC_SDIO_HZ
#define ESP_NIC_SDIO_HZ 40000000UL
#endif
#ifndef ESP_NIC_SDIO_WIDTH
#define ESP_NIC_SDIO_WIDTH 4
#endif
#ifndef ESP_NIC_UART_BAUD
#define ESP_NIC_UART_BAUD 921600UL
#endif

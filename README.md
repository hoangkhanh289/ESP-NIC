# ESP-NIC: Universal High-Performance Network Interface Card (Bilingual VI / EN)

[![Author](https://img.shields.io/badge/Author-Hoang%20Khanh-informational.svg)](https://github.com/hoangkhanh289)
[![GitHub Repo](https://img.shields.io/badge/GitHub-ESP--NIC-blue.svg)](https://github.com/hoangkhanh289/ESP-NIC)
[![Arduino](https://img.shields.io/badge/Arduino-1.5%2B%20Precompiled-blue.svg)](https://docs.arduino.cc/arduino-cli/library-specification/)
[![License](https://img.shields.io/badge/License-Apache%202.0-green.svg)](LICENSE)
[![Transport](https://img.shields.io/badge/Transport-SPI%20%7C%20SDIO%20%7C%20UART-orange.svg)](#4-sơ-đồ-đấu-nối-phần-cứng--hardware-wiring)
[![Supported Masters](https://img.shields.io/badge/Masters-ESP32%20%7C%20ESP32--P4%20%7C%20ESP32--S3%20%7C%20STM32-blueviolet.svg)](#2-ma-trận-tính-năng--capability-matrix)
[![Supported Slaves](https://img.shields.io/badge/Slaves-ESP32--C6%20%7C%20ESP32--C5%20%7C%20ESP32--C3%20%7C%20ESP32--S3%20%7C%20ESP32-success.svg)](#2-ma-trận-tính-năng--capability-matrix)

---

## Giới thiệu & Tổng quan / Overview

### 🇻🇳 Tiếng Việt
**ESP-NIC (ESP Network Interface Card)** là giải pháp card mạng không dây công nghiệp hiệu năng cao dành cho hệ sinh thái Arduino. Thư viện cho phép vi điều khiển chủ (Master MCU như **ESP32-P4, ESP32-S3, ESP32, STM32F4/F7/G4/H7**) kết nối và điều khiển trực tiếp vi điều khiển phụ không dây (Slave Coprocessor như **ESP32-C6, ESP32-C5, ESP32-C3, ESP32-S3, ESP32**) thông qua các giao tiếp bus tốc độ cao: **SPI, SDIO (Packet Mode)** hoặc **UART**:

* 🌐 **Kết nối Mạng Toàn diện**: Cung cấp đầy đủ tính năng Wi-Fi Station (STA), SoftAP, quét mạng (hỗ trợ Wi-Fi 6 2.4GHz trên C6 và Wi-Fi băng tần kép 2.4GHz / 5GHz trên C5), máy khách/máy chủ TCP, UDP và kết nối bảo mật HTTPS/TLS với xác thực chứng chỉ số X.509.
* ⚡ **Giao tiếp Bus Đa dạng & Tốc độ Cao**:
  * **SDIO Host (Packet Mode)**: Hỗ trợ truyền nhận gói tin 4-bit tần số 20–40 MHz trên ESP32-P4 / ESP32-S3 cho thông lượng cực lớn.
  * **SPI Bus (DMA)**: Chế độ Mode 2 với 2 chân tín hiệu phần cứng Handshake (HS) và Data Ready (DR), hỗ trợ tần số lên đến 10–40 MHz.
  * **UART Port**: Tối ưu với cơ chế checksum và resync tự động ở tốc độ baud cao (115,200 – 3,000,000 bps).
* 📱 **Hỗ trợ Bluetooth HCI Passthrough**: Chuyển tiếp luồng gói tin Bluetooth H4 Controller qua bus đến vi điều khiển chủ để tích hợp BLE Host Stack.
* 🖥️ **Kênh RPC & Hiển thị Tương tác Độc lập**: Cung cấp kênh truyền thông tin tùy biến (Custom RPC, chuỗi thông điệp, thanh tiến trình, hiển thị văn bản OLED/TFT trên slave và sự kiện phản hồi ngược từ Slave về Master) chạy song song và độc lập với luồng Ethernet mạng.
* 🔄 **Phối hợp Nạp Firmware Trên Mạch (Firmware Handoff)**: Tích hợp API đóng mở bus mượt mà để phối hợp với thư viện [EspSerialFlasher](https://github.com/hoangkhanh289/esp-serial-flasher) hoặc [ESP-ISP](https://github.com/hoangkhanh289/ESP-ISP) tự động nạp hoặc cập nhật firmware cho Slave mà không cần phần cứng can thiệp thủ công.
* 📦 **Lõi Nhị phân Tối ưu (Precompiled Hardened Core)**: Bộ phân giải giao thức, đóng khung SLIP/Hosted, quản lý hàng đợi bộ nhớ và driver phần cứng được biên dịch tối ưu sẵn trong các thư viện tĩnh `.a`, giúp sketch biên dịch tức thì và ngăn ngừa xung đột biểu tượng.

### 🇬🇧 English
**ESP-NIC (ESP Network Interface Card)** is a high-performance, industrial-grade wireless network interface solution designed for the Arduino ecosystem. It enables a host microcontroller (Master MCU such as **ESP32-P4, ESP32-S3, ESP32, STM32F4/F7/G4/H7**) to interface with an external wireless coprocessor (Slave MCU such as **ESP32-C6, ESP32-C5, ESP32-C3, ESP32-S3, ESP32**) over high-speed buses: **SPI, SDIO (Packet Mode)**, or **UART**:

* 🌐 **Comprehensive Networking**: Full Wi-Fi Station (STA), SoftAP, network scanning (supporting Wi-Fi 6 2.4GHz on C6 and dual-band 2.4GHz / 5GHz on C5), TCP/UDP Client & Server, and verified TLS/HTTPS secure connections with X.509 certificates.
* ⚡ **High-Throughput Bus Backends**:
  * **SDIO Host (Packet Mode)**: High-speed 4-bit bus at 20–40 MHz on ESP32-P4 / S3 for maximum data throughput.
  * **SPI Bus (DMA)**: Mode 2 SPI with dedicated hardware Handshake (HS) and Data Ready (DR) flow-control pins at 10–40 MHz.
  * **UART Port**: Bounded FIFO transmission with automatic packet checksums and stream resynchronization (115,200 – 3,000,000 bps).
* 📱 **Bluetooth HCI Passthrough**: Bridges raw H4 Bluetooth controller packets over the coprocessor bus to power host-side BLE stacks.
* 🖥️ **Interactive Display & Custom RPC Channel**: Dedicated sub-channel for custom application RPC, text/progress rendering on optional coprocessor displays (OLED/TFT), and reverse event callbacks from slave to master without congesting LAN traffic.
* 🔄 **Seamless Firmware Handoff**: Dedicated API to release the hardware bus, hand control over to [EspSerialFlasher](https://github.com/hoangkhanh289/esp-serial-flasher) / [ESP-ISP](https://github.com/hoangkhanh289/ESP-ISP) for in-system firmware updates, and cleanly restore network operation.
* 📦 **Precompiled Hardened Core**: Wire framing, serializers, buffer pools, and hardware drivers are encapsulated inside precompiled static archives (`.a`) for fast builds and zero symbol collision.

---

## Kiến trúc Hệ thống / System Architecture

```text
+-----------------------------------------------------+                 +-----------------------------------------------------+
|                  MASTER MCU (HOST)                  |                 |               SLAVE COPROCESSOR (NIC)               |
|      ESP32-P4 / ESP32-S3 / ESP32 / STM32 Master     |                 |          ESP32-C6 / ESP32-C5 / ESP32-C3 / S3        |
|                                                     |                 |                                                     |
|  - Arduino Application Sketch                       |                 |  - ESP-Hosted-MCU Firmware (RPC V2 Protocol)        |
|  - #include <ESP_NIC.h>                             |  Hardware Bus   |  - Wi-Fi 6 (C6) / Dual-Band 2.4G & 5G (C5) Radio    |
|  - EspNIC.beginSPI() / beginSDIO() / beginUART()    | <=============> |  - Bluetooth 5 (LE) HCI Controller                  |
|  - EspNICClient / EspNICServer / EspNICUDP / TLS    |                 |  - SPI / SDIO Packet Slave / UART Peripheral        |
|  - Custom RPC & Interactive Display API             |                 |  - Optional Slave Display (SSD1306 / ST7789)        |
|  - lwIP Network Stack (ESP32) / Raw Ethernet (STM32)|                 |  - Reset & Boot Strapping Lines                     |
+-----------------------------------------------------+                 +-----------------------------------------------------+
```

---

## 1. Cài đặt & Khởi động Nhanh / Installation & Quickstart

### A. Cài đặt Thư viện / Installation
1. Cài đặt gói bo mạch **Arduino-ESP32 (v3.3.12)** hoặc **STM32duino (v3.0.0)** qua Boards Manager của Arduino IDE.
2. Tải bản phát hành `dist/ESP_NIC-1.0.0.zip` và chọn **Sketch → Include Library → Add .ZIP Library** trong Arduino IDE. Hoặc giải nén kho mã nguồn vào thư mục `<Sketchbook>/libraries/ESP_NIC`.
3. Mở **File → Examples → ESP-NIC**, chọn ví dụ phù hợp, cấu hình chân GPIO/SSID/Mật khẩu và nạp cho Master.

### B. Ví dụ Kết nối Wi-Fi Cơ bản / Quickstart Code Example
```cpp
#include <ESP_NIC.h>

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println("Khoi tao ESP-NIC...");
  // Khoi tao mac dinh (SPI mode 2 hoac SDIO tuy thuoc cau hinh phan cung)
  if (!EspNIC.begin()) {
    Serial.printf("Khoi tao that bai! Loi: %s\n", EspNIC.lastError());
    return;
  }

  Serial.println("Dang ket noi Wi-Fi...");
  if (EspNIC.connectWiFi("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD", 30000)) {
    Serial.print("Da ket noi Wi-Fi thanh cong! IP: ");
    Serial.println(EspNIC.getLocalIP());
  } else {
    Serial.printf("Ket noi Wi-Fi that bai: %s\n", EspNIC.lastError());
  }
}

void loop() {
  // Bat buoc goi poll() thuong xuyen trong vong lap loop() de xu ly goi tin
  EspNIC.poll();
  delay(1);
}
```

> [!IMPORTANT]
> **Quy tắc Vận hành Quan trọng / Operational Rules**:
> 1. Luôn gọi hàm `EspNIC.poll()` thường xuyên từ **một luồng tác vụ (single task)** để bơm hàng đợi dữ liệu. Không gọi các hàm giao dịch của thư viện từ ngắt cứng (ISR).
> 2. `EspNIC.isReady()` xác nhận đàm phán bắt tay giao thức bus thành công; trạng thái liên kết này tách biệt với trạng thái cấp phát IP/DHCP của Wi-Fi.
> 3. Cấu hình chân phần cứng runtime thông qua các struct cấu hình công khai (`beginSPI`, `beginSDIO`, `beginUART`); tuyệt đối không cần can thiệp hay sửa đổi file `.a`.

---

## 2. Ma trận Tính năng / Capability Matrix

| Phân loại MCU | Giao tiếp SPI (Mode 2) | Giao tiếp UART | Giao tiếp SDIO (Packet Mode) | Tính năng Mạng & Giao thức |
|---|---|---|---|---|
| **ESP32 Master** | IDF DMA, 10–40 MHz | Native UART (115.2k – 3M) | Khe Slot-1 cố định (1/4-bit) | IPv4 lwIP đầy đủ, DHCP STA/AP, TCP/UDP, TLS X.509 |
| **ESP32-S3 / P4 Master** | IDF DMA, 10–40 MHz | Native UART (115.2k – 3M) | Tùy biến GPIO, 4-bit, 20–40 MHz | lwIP đầy đủ, thông lượng cao, TLS X.509 |
| **ESP32-S2 / C3 / C5 / C6 Master** | IDF DMA, 10–40 MHz | Native UART (115.2k – 3M) | Không hỗ trợ phần cứng SDIO Host | lwIP đầy đủ, DHCP STA/AP, TCP/UDP, TLS X.509 |
| **STM32F446 / G474 Master** | `SPIClass`; tùy chọn HAL DMA | HAL RX Ring Buffer | F446 SDIO; G474 không có SDMMC | Raw Ethernet frames; ứng dụng tự quản lý stack IP |
| **STM32F767 / H743 Master** | `SPIClass`; tùy chọn HAL DMA | HAL RX Ring Buffer | SDMMC Packet Mode, DMA/IDMA | Raw Ethernet frames; ứng dụng tự quản lý stack IP |
| **ESP32 / C5 / C6 Slave** | SPI DMA Slave | Native UART Slave | Chân silicon cố định, Packet Mode | Wi-Fi 6 (C6), Dual-Band (C5), Bluetooth HCI |
| **ESP32-S3 / C3 Slave** | SPI DMA Slave | Native UART Slave | Không có ngoại vi SDIO Slave | Wi-Fi 4 / BLE 5 Controller |

> [!NOTE]
> **Lưu ý riêng cho Master STM32**: Môi trường STM32duino mặc định không tích hợp sẵn stack lwIP. Vì vậy, `connectWiFi()` chỉ đóng vai trò liên kết vô tuyến với Access Point và `getLocalIP()` sẽ trả về `0.0.0.0`. Dữ liệu mạng được đưa ra dưới dạng khung Raw Ethernet (`sendPacket` / `readPacket`) để người dùng cấp cho lwIP ngoài.

---

## 3. Tương thích Nhị phân & ABI / Binary Compatibility & ABI

Thư viện bao gồm **11 file archive tĩnh / 11 cấu hình build** tương ứng với các bộ vi điều khiển:
* **ESP32 Processors**: `src/esp32/`, `src/esp32s2/`, `src/esp32s3/`, `src/esp32c3/`, `src/esp32c5/`, `src/esp32c6/`, `src/esp32p4/`.
* **ARM Cortex-M Processors**:
  * `src/cortex-m4/`: Bao gồm các thành phần dành riêng cho STM32F446 và STM32G474 (Hard-float ABI `fpv4-sp-d16`).
  * `src/cortex-m7/`: Bao gồm các thành phần dành riêng cho STM32F767 và STM32H743 (Hard-float ABI `fpv5-sp-d16`).

Khung facade C++ mỏng tự động chọn symbol dispatcher tương ứng theo target phần cứng khi biên dịch. Xem chi tiết báo cáo mã băm SHA-256 và kích thước tại [`docs/VALIDATION.md`](docs/VALIDATION.md).

---

## 4. Sơ đồ Đấu nối Phần cứng / Hardware Wiring

### A. Đấu nối Bus SPI (Khuyến nghị Mode 2) / SPI Wiring
Bus SPI sử dụng cơ chế bắt tay 2 chiều thông qua chân **Handshake (HS)** và **Data Ready (DR)** để đạt tốc độ cao và chống nghẽn bộ đệm:

```text
  Master SCLK ------------> Slave SCLK
  Master MOSI ------------> Slave MOSI
  Master MISO <------------ Slave MISO
  Master CS   ------------> Slave CS
  Master HS   <------------ Slave HS (GPIO Out thông báo sẵn sàng giao dịch)
  Master DR   <------------ Slave DR (GPIO Out thông báo có dữ liệu đợi đọc)
  Master Reset -----------> Slave EN
  Master GND  ------------> Slave GND (Bắt buộc nối chung mass)
```

| Tín hiệu / Signal | ESP32-S3 Master Ví dụ | STM32 Master Ví dụ | ESP32-C6 Slave Profile | Mô tả Kỹ thuật / Notes |
|---|:---:|:---:|:---:|---|
| **SCLK** | `GPIO 12` | `PA5` | `GPIO 6` | Xung nhịp SPI (Khởi đầu 10MHz, tối đa 40MHz) |
| **MISO** | `GPIO 13` | `PA6` | `GPIO 2` | Master In Slave Out |
| **MOSI** | `GPIO 11` | `PA7` | `GPIO 7` | Master Out Slave In |
| **CS** | `GPIO 10` | `PB6` | `GPIO 10` | Chip Select (Active LOW) |
| **HS (Slave → Master)** | `GPIO 4 (In)` | `PB0 (In)` | `GPIO 3 (Out)` | Bắt tay: Slave báo đã kích hoạt phiên SPI |
| **DR (Slave → Master)** | `GPIO 5 (In)` | `PB1 (In)` | `GPIO 4 (Out)` | Data Ready: Slave báo đang có dữ liệu trong hàng đợi |
| **Reset Control** | `GPIO 6 (Out)` | `PB10 (Out)` | `EN` | Master điều khiển Reset/EN của Slave |
| **GND** | `GND` | `GND` | `GND` | **Bắt buộc nối chung Mass / Common Ground** |

---

### B. Đấu nối Bus SDIO (Packet Mode 4-bit) / SDIO Wiring
Bus SDIO cung cấp băng thông mạng cao nhất, đặc biệt tối ưu cho cặp đôi **ESP32-P4 Master** và **ESP32-C6 / C5 Slave**:

| Tín hiệu / Signal | ESP32-P4 Master (Ví dụ) | ESP32-C6 Slave | ESP32-C5 Slave | Ghi chú Trở kéo & Phần cứng |
|---|:---:|:---:|:---:|---|
| **CLK** | `GPIO 18` | `GPIO 19` | `GPIO 9` | Xung nhịp SDIO (20 – 40 MHz) |
| **CMD** | `GPIO 19` | `GPIO 18` | `GPIO 10` | Lệnh SDIO (Cần trở kéo lên 10 kΩ lên 3.3V) |
| **D0** | `GPIO 14` | `GPIO 20` | `GPIO 8` | Đường dữ liệu 0 (Trở kéo lên 10 kΩ) |
| **D1** | `GPIO 15` | `GPIO 21` | `GPIO 7` | Đường dữ liệu 1 (Trở kéo lên 10 kΩ) |
| **D2** | `GPIO 16` | `GPIO 22` | `GPIO 14` | Đường dữ liệu 2 (Trở kéo lên 10 kΩ) |
| **D3** | `GPIO 17` | `GPIO 23` | `GPIO 13` | Đường dữ liệu 3 (Trở kéo lên 10 kΩ) |
| **Reset** | `GPIO 54` | `EN` | `EN` | Reset Slave từ Master |

> [!WARNING]
> * Các đường `CMD`, `D0..D3` bắt buộc phải có điện trở kéo lên ngoài **10 kΩ** nối nguồn 3.3V theo đúng khuyến cáo trong Datasheet của Espressif.
> * Chú ý chân của ESP32-C5 khác biệt hoàn toàn với ESP32-C6. Không đấu chéo nhầm chân giữa hai dòng chip.

---

### C. Đấu nối Bus UART (Giao tiếp Tiết kiệm Chân) / UART Wiring
Dành cho các ứng dụng tiết kiệm chân hoặc bo mạch không có ngoại vi SPI/SDIO rảnh:
* Nối chéo: **Master TX → Slave RX** và **Master RX ← Slave TX**.
* Bắt buộc chung nguồn Mass (**GND**).
* Tùy chọn nối chân **Master Reset → Slave EN** để phục vụ reset hoặc nạp lại firmware.
* Cấu hình baudrate mặc định trong firmware mẫu đi kèm: **921,600 bps, 8N1** (hỗ trợ dải baud 115,200 – 3,000,000 bps; hai bên phải cùng cấu hình baud giống nhau).

---

## 5. Mạng không dây, Socket & Kênh RPC Tương tác / Networking, Sockets & Interactive RPC

### A. Wi-Fi & Quét Mạng / Wi-Fi & Scan
* Hỗ trợ quét mạng đầy đủ thông tin: SSID, BSSID, RSSI, Mode bảo mật (WPA2/WPA3), Kênh truyền (Channel).
* Hỗ trợ băng tần 5GHz khi kết hợp với Slave **ESP32-C5**.
* `EspNIC.softAP(ssid, password)` tạo điểm phát sóng AP. Trên ESP32 host, thư viện tự động kích hoạt DHCP Server cấp dải mạng `192.168.4.1/24`.

### B. Client / Server / UDP & Bảo mật TLS
* Cung cấp các lớp hướng đối tượng chuẩn Arduino: `EspNICClient`, `EspNICServer`, `EspNICUDP`.
* Lớp bảo mật `EspNICSecureClient`: Hỗ trợ kiểm tra chứng chỉ máy chủ bằng mảng chứng chỉ PEM (`setCACert`) hoặc Bundle CA có sẵn trong SDK. Bắt buộc đồng bộ thời gian thực (NTP/RTC) trước khi mở kết nối TLS/HTTPS.

### C. Bluetooth HCI Passthrough
* Hàm `EspNIC.enableBLE()`, `EspNIC.sendHCI()` và callback `EspNIC.onHCI()` cho phép truyền nhận trực tiếp các gói tin chuẩn H4 Controller với vi điều khiển phụ có hỗ trợ Bluetooth LE.

### D. Màn hình Tương tác & Lệnh Tùy biến / Interactive Display & Custom RPC
Kênh truyền thông tin phụ chạy độc lập với luồng gói tin Ethernet mạng LAN:
* `EspNIC.displayText(x, y, "Text")`: Hiển thị văn bản tại tọa độ màn hình gắn trên slave.
* `EspNIC.displayProgress(percent, "Label")`: Cập nhật thanh tiến trình phần trăm nạp/tải.
* `EspNIC.sendCustomMessage("Hello")`: Gửi chuỗi tin nhắn tùy chỉnh (ID = 1).
* `EspNIC.customRPC(cmd_id, tx_data, tx_len, rx_buf, rx_len)`: Thực thi thủ tục gọi hàm từ xa có phản hồi hai chiều.
* `EspNIC.onEvent(callback)`: Lắng nghe sự kiện bất đồng bộ từ Slave gửi ngược lên Master (nút bấm, cảm biến, trạng thái ngoại vi).

---

## 6. Chuyển giao Nạp Firmware / Firmware Handoff (`esp-serial-flasher`)

Thư viện cung cấp hàm tích hợp chuyên dụng `EspNIC.updateFirmware(...)` để phối hợp nhịp nhàng với bộ nạp trên mạch:

```cpp
#include <ESP_NIC.h>
#include <EspSerialFlasher.h> // Thu vien nap firmware tuy chon

bool myFlasherCallback(void *context) {
  // Tai day, ESP-NIC da giai phong hoan toan bus SPI/SDIO/UART.
  // Tien hanh su dung EspSerialFlasher de nap firmware moi cho Slave:
  // 1. Ket noi ROM bootloader cua Slave
  // 2. Nap phan vung firmware va xac minh ma bam MD5
  // 3. Reset Slave de khoi dong vao app moi
  // 4. Giai phong cong UART cua Flasher
  return true; // Tra ve true neu nap thanh cong
}

void triggerUpdate() {
  // Goi updateFirmware: ESP-NIC se tam dong mang, nhan callback nap,
  // sau do tu dong khoi phuc lai bus va ket noi lai Wi-Fi STA.
  bool ok = EspNIC.updateFirmware(myFlasherCallback, nullptr, "YOUR_SSID", "YOUR_PASSWORD");
}
```

Xem hướng dẫn và mã mẫu chi tiết tại [`docs/API.md`](docs/API.md).

---

## 7. Danh mục 8 Ví dụ Mẫu / Examples Index

Tất cả các ví dụ nằm trong thư mục [`examples/`](examples/):

| Thư mục Ví dụ | Mục đích Kỹ thuật / Description | Ứng dụng Thực tế / Real-world Use Case |
|---|---|---|
| [`01_WiFi_Scan`](examples/01_WiFi_Scan/) | Quét mạng không dây | Hiển thị danh sách AP, cường độ sóng RSSI, kênh truyền, xác thực 2.4G & 5G (C5). |
| [`02_WiFi_Station`](examples/02_WiFi_Station/) | Kết nối Wi-Fi Client | Kết nối vào Router Wi-Fi, nhận IP qua DHCP (ESP32) hoặc lấy trạng thái vô tuyến (STM32). |
| [`03_WebServer`](examples/03_WebServer/) | Máy chủ Web HTTP | Khởi tạo Web Server điều khiển GPIO hoặc xem thông số qua giao diện web trực quan. |
| [`04_WebClient`](examples/04_WebClient/) | Máy khách HTTP / HTTPS | Gửi yêu cầu GET/POST qua HTTP và HTTPS bảo mật có kiểm tra chứng chỉ X.509. |
| [`05_P4_SDIO_HighThroughput`](examples/05_P4_SDIO_HighThroughput/) | Băng thông cao SDIO 4-bit | Kiểm tra truyền nhận dữ liệu gói tin SDIO tốc độ 40MHz giữa ESP32-P4 và ESP32-C6. |
| [`06_STM32_SPI_RawEthernet`](examples/06_STM32_SPI_RawEthernet/) | Khung mạng Raw Ethernet | Dành cho Master STM32 truyền nhận khung Ethernet thô để gắn vào lwIP bên ngoài. |
| [`07_Interactive_Display_OLED_TFT`](examples/07_Interactive_Display_OLED_TFT/) | Màn hình tương tác & Custom RPC | Điều khiển màn hình OLED/TFT gắn trên Slave và nhận sự kiện nút bấm gửi ngược về Master. |
| [`08_UART_LowPinCount`](examples/08_UART_LowPinCount/) | Giao tiếp UART rút gọn | Kết nối Master và Slave chỉ với 2 chân UART TX/RX ở tốc độ cao 921,600 bps. |

Mỗi thư mục ví dụ đều có sẵn mã nguồn đồng hành dành cho chip phụ tại thư mục con `slave/slave.ino`.

---

## 8. Firmware Coprocessor & Bản quyền / Firmware & Licensing

* **Firmware nạp sẵn**: Có sẵn 7 profile firmware nhị phân được biên dịch hoàn chỉnh cho các chip Slave (ESP32-C6, C5, S3) qua các giao tiếp SPI, SDIO, UART, BLE tại thư mục [`firmware/`](firmware/). Xem hướng dẫn chi tiết tại [`firmware/README_FLASHING.md`](firmware/README_FLASHING.md).
* **Bảo vệ Nhị phân (Hardening)**: Xem tài liệu kỹ thuật tại [`docs/HARDENING.md`](docs/HARDENING.md).
* **Kiểm chứng (Validation)**: Xem báo cáo kiểm thử tại [`docs/VALIDATION.md`](docs/VALIDATION.md).
* **Bản quyền & Giấy phép**: Phát hành theo chuẩn mã nguồn mở **Apache License 2.0** — Xem chi tiết tại [LICENSE](LICENSE), [NOTICE](NOTICE) và [`LICENSES/`](LICENSES/).
* **Tác giả**: **Hoàng Khánh** ([@hoangkhanh289](https://github.com/hoangkhanh289)). Kho lưu trữ chính thức: [https://github.com/hoangkhanh289/ESP-NIC](https://github.com/hoangkhanh289/ESP-NIC).

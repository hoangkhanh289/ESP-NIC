# ESP-NIC API Reference / Tài liệu Tham chiếu API (Bilingual VI / EN)

Tài liệu này định nghĩa chi tiết các hàm API công khai, kiểu dữ liệu, tham số, quyền sở hữu bộ đệm và quy ước xử lý lỗi của thư viện **ESP-NIC** cho cả hai vai trò: **Master (Host)** và **Slave (Coprocessor)**.

This guide provides a comprehensive technical reference for public API methods, data structures, buffer ownership, and error handling conventions of the **ESP-NIC** library for both **Master (Host)** and **Slave (Coprocessor)** roles.

---

## 1. Vòng đời & Cấu hình Giao tiếp / Lifecycle & Bus Configuration

### A. Bảng tra cứu Phương thức / Method Reference

| Phương thức / Method | 🇻🇳 Mô tả Tham số & Kết quả (Tiếng Việt) | 🇬🇧 Parameters & Return Value (English) |
|---|---|---|
| `begin(mode)` | Khởi tạo với giao tiếp mặc định của bo mạch (SPI hoặc SDIO). Chế độ UART yêu cầu gọi `beginUART`. Trả về `true` khi đàm phán bắt tay bus thành công. | Initializes default board bus (SPI or SDIO); UART requires explicit `beginUART`. Returns `true` only after successful bus negotiation. |
| `beginSPI(config)` | Cấu hình SPI Master: chân GPIO `sclk, miso, mosi, cs, handshake, dataready, reset`; tần số `frequency` (Hz); chế độ `mode=2`. Chân reset để `-1` nếu không đấu nối. | Configures Master SPI: GPIO pins `sclk, miso, mosi, cs, handshake, dataready, reset`; clock `frequency` (Hz); `mode=2`. Reset `-1` indicates absent. |
| `beginSDIO(config)` | Cấu hình SDIO Master: chân GPIO `clk, cmd, d0..d3, reset`; tần số `frequency` (Hz); độ rộng 1 hoặc 4-bit; slot 1. STM32 yêu cầu gọi `configureSDIO` trước. | Configures Master SDIO: GPIO pins `clk, cmd, d0..d3, reset`; clock `frequency` (Hz); bus width 1 or 4; slot 1. STM32 requires `configureSDIO` first. |
| `beginUART(config)` | Cấu hình UART Master: chân TX/RX, reset tùy chọn (-1), tốc độ baud khớp với slave (115,200 – 3,000,000 bps), cổng phần cứng 1 hoặc 2. | Configures Master UART: TX/RX pins, optional reset (-1), matching slave baud (115,200 – 3,000,000 bps), hardware port 1 or 2 if available. |
| `configureSDIO(clk, setup, close, dma)` | **(Dành riêng cho STM32)**: Thiết lập xung nhịp SDMMC kernel (Hz); hàm callback `setup` cấu hình clock/chân Alternate Function và trả về 0; `close` giải phóng ngoại vi. | **(STM32 Only)**: Sets actual SDMMC kernel clock in Hz; `setup` callback configures clocks/AF pins and returns 0; optional `close` releases them. |
| `configureSPIDMA(setup)` | **(Dành riêng cho STM32)**: Nhận đối tượng `SPIClass` được chọn; liên kết DMA TX/RX qua HAL và vector ngắt IRQ, trả về 0. Truyền `nullptr` để dùng chế độ Polling. | **(STM32 Only)**: Receives selected `SPIClass`; links HAL TX/RX DMA and IRQs, returns 0. Passing `nullptr` restores polling mode. |
| `configureMemory(alloc, free)` | **(Dành riêng cho STM32)**: Cặp callback cấp phát vùng nhớ SRAM DMA căn lề 32-byte và giải phóng con trỏ. Truyền `nullptr` để dùng bộ cấp phát mặc định. | **(STM32 Only)**: Paired callbacks allocate 32-byte aligned DMA SRAM and free the pointer. Passing `nullptr` restores default allocator. |
| `poll()` | **Bắt buộc gọi thường xuyên**: Bơm dữ liệu hàng đợi, quét bus và mạng. Trả về số byte/gói tin đã xử lý (>=0), hoặc mã lỗi âm nếu thất bại. | **Mandatory routine**: Pumps queues, bus, and network. Returns non-negative count of processed items, or negative error code. |
| `end()` | Dừng hoạt động, giải phóng quyền sở hữu bus, ngoại vi và ngắt. An toàn ngay cả khi chưa khởi tạo. Cần đóng các socket client trước. | Releases role, bus, and interfaces; safe when uninitialized. Stop active network clients before calling this. |
| `isReady()` / `getStatus()` | Kiểm tra liên kết đã sẵn sàng / trạng thái công khai. Trạng thái đàm phán bus độc lập với trạng thái nhận IP từ Wi-Fi DHCP. | Checks if link is negotiated / returns public state. Bus negotiation and Wi-Fi IP readiness are distinct states. |
| `lastError()` | Trả về chuỗi thông báo lỗi gần nhất gặp phải của lớp truyền tải hoặc RPC. | Returns descriptive string of the most recent transport or RPC failure. |
| `firmwareVersion()` | Chuỗi phiên bản firmware của Slave (nhận được sau khi Slave gửi gói tin chào hỏi). Cần sao chép chuỗi nếu muốn lưu trữ qua lần reset. | Slave firmware version string announced during handshake. Copy the string if retaining it across end/reset calls. |

---

## 2. Quản lý Mạng Wi-Fi & Gói tin / Wi-Fi & Packet Management

| Phương thức / Method | 🇻🇳 Mô tả Tham số & Kết quả (Tiếng Việt) | 🇬🇧 Parameters & Return Value (English) |
|---|---|---|
| `connectWiFi(ssid, pass, timeout)` | Kết nối vào Access Point. SSID ≤ 32 byte; password để trống (Open) hoặc mã khóa WPA/WPA2/WPA3; timeout tính bằng ms. Trên ESP32 đợi cấp DHCP; trên STM32 chỉ đợi kết nối vô tuyến. | Associates with an AP. SSID ≤ 32 bytes; password empty (Open) or WPA credential; timeout in ms. ESP32 waits for DHCP; STM32 waits for radio association. |
| `disconnectWiFi()` | Ngắt kết nối trạm Wi-Fi STA hiện tại. Trả về `true` khi gửi lệnh RPC thành công. | Disconnects current Wi-Fi STA. Returns `true` on successful RPC execution. |
| `getLocalIP()` / `getAPIP()` | Trả về đối tượng `IPAddress` IPv4. Trả về `0.0.0.0` nếu chưa được cấp IP hoặc không hỗ trợ. | Returns IPv4 `IPAddress`. Returns `0.0.0.0` if IP is unavailable or unsupported. |
| `scanNetworks(records, cap, timeout)` | Quét danh sách Wi-Fi xung quanh. Mảng `records` do người dùng cung cấp (sức chứa 1..32); timeout tính bằng ms. Trả về số lượng AP tìm thấy hoặc mã lỗi âm. Mỗi bản ghi gồm: SSID[33], BSSID[6], kênh (channel), kiểu bảo mật authmode, RSSI (dBm). | Scans nearby networks into caller-provided array (capacity 1..32); bounded wait in ms. Returns count of APs found or negative error. Each record includes SSID[33], BSSID[6], channel, authmode, RSSI in dBm. |
| `softAP(ssid, pass, ch, max, hidden)` | Khởi tạo điểm phát sóng SoftAP: SSID 1..32 byte, password rỗng hoặc 8..63 byte, kênh hợp lệ theo quy định quốc gia, số lượng client 1..10. Trên ESP32 host sẽ tự động khởi động DHCP Server cấp dải `192.168.4.1/24`. | Starts SoftAP: SSID 1..32 bytes, password empty or 8..63 bytes, valid regulatory channel, 1..10 clients. ESP32 host also launches AP DHCP server at `192.168.4.1/24`. |
| `stopSoftAP()` | Tắt điểm phát sóng SoftAP và hủy interface AP; giữ nguyên chế độ kết nối STA nếu đang chạy. | Stops SoftAP and destroys host AP interface; preserves STA connection. |
| `sendPacket(data, length)` | Gửi khung Ethernet hoàn chỉnh (độ dài ≤ 1514 byte). Trả về số byte đã xếp vào hàng đợi hoặc mã lỗi âm; cần gọi `poll()` để truyền ra bus. | Transmits raw Ethernet frame (length ≤ 1514 bytes). Returns queued byte count or negative error; call `poll()` to flush. |
| `readPacket(buffer, capacity)` | Đọc một khung Ethernet từ hàng đợi đến. Trả về chiều dài khung (byte), trả về 0 nếu hàng đợi trống, hoặc mã lỗi âm. Hàng đợi đệm tối đa 4 khung; khung vượt quá sẽ bị hủy. | Reads one received Ethernet frame. Returns frame length, 0 if empty, or negative error. Up to 4 raw frames are queued; excess frames are dropped. |
| `enableBLE()` | Yêu cầu firmware Slave kích hoạt controller Bluetooth LE; trả về lỗi nếu firmware Slave không hỗ trợ BLE. | Requests BT-enabled slave to activate BLE controller; returns error if slave firmware lacks Bluetooth support. |
| `sendHCI(packet, length)` | Gửi gói tin chuẩn H4 Bluetooth HCI bao gồm byte loại gói tin (Type byte). Trả về số byte đã nhận hoặc mã lỗi âm. | Transmits raw H4 Bluetooth HCI packet including type byte. Returns queued bytes or negative error. |
| `onHCI(callback, context)` | Đăng ký hàm nhận phản hồi gói tin HCI từ Slave (loại gói tin, con trỏ dữ liệu, độ dài và context). Dữ liệu chỉ hợp lệ trong phạm vi thực thi callback. | Registers callback to receive incoming HCI packets from slave (packet type, payload, length, and user context). Borrowed data valid only within callback. |

---

## 3. RPC Tùy biến, Hiển thị & Sự kiện / Custom RPC, Display & Events

| Phương thức / Method | 🇻🇳 Mô tả Tham số & Kết quả (Tiếng Việt) | 🇬🇧 Parameters & Return Value (English) |
|---|---|---|
| `customRPC(id, tx, tx_len, rx, rx_len, timeout)` | Lệnh RPC 32-bit hai chiều: mã lệnh `id`, dữ liệu gửi `tx` ≤ 1400 byte. `rx_len` khi truyền vào là sức chứa bộ đệm nhận, khi trả về là chiều dài dữ liệu thực tế. Trả về 0 nếu thành công hoặc mã lỗi âm. | 32-bit bidirectional RPC: command `id`, payload `tx` ≤ 1400 bytes. `rx_len` represents buffer capacity on input and actual returned bytes on exit. Returns 0 on success or negative error. |
| `sendCustomCommand(id, data, length)` | Gửi lệnh 1 chiều với mã lệnh 8-bit `id` và dữ liệu ≤ 1400 byte. Các ID 1, 2, 3 được dành riêng cho các lệnh hiển thị văn bản và tiến trình. | Sends 1-way command with 8-bit `id` and payload ≤ 1400 bytes. IDs 1, 2, 3 are reserved for display convenience helpers. |
| `sendCustomMessage(String)` | Gửi chuỗi tin nhắn UTF-8 (độ dài ≤ 1400 byte) với mã ID = 1. Trả về số byte gửi hoặc mã lỗi âm. | Sends UTF-8 text message (length ≤ 1400 bytes) with ID = 1. Returns sent length or negative error. |
| `displayText(x, y, text)` | Hiển thị chuỗi văn bản tại tọa độ pixel `(x, y)` trên màn hình của Slave (tổng kích thước lệnh ≤ 1400 byte). Trả về số byte đã mã hóa hoặc mã lỗi âm. | Displays text at pixel coordinates `(x, y)` on slave display (total command ≤ 1400 bytes). Returns encoded bytes or negative error. |
| `displayProgress(percent, label)` | Cập nhật thanh tiến trình: phần trăm `percent` (0..100) và chuỗi nhãn `label` tùy chọn. Trả về số byte đã mã hóa hoặc mã lỗi âm. | Updates progress bar: `percent` (0..100) and optional `label` string. Returns encoded bytes or negative error. |
| `onEvent(callback, context)` | Đăng ký callback nhận sự kiện từ Slave gửi lên: sự kiện ứng dụng tùy biến hoặc sự kiện mạng Wi-Fi từ SDK. Dữ liệu chỉ có hiệu lực trong hàm callback. | Registers callback to process asynchronous events from slave (custom app events or SDK Wi-Fi notifications). Borrowed payload valid only during callback. |
| `updateFirmware(callback, ctx, ssid, pass)` | Chuyển giao nạp firmware: tạm đóng mạng và nhả bus phần cứng để gọi hàm nạp `callback`, sau đó tự khôi phục bus và kết nối lại Wi-Fi. Trả về `true` khi cả hai bước thành công. | Firmware handoff routine: closes network and releases hardware bus to run flashing `callback`, then restores bus and reconnects Wi-Fi. Returns `true` only if both succeed. |

---

## 4. Bảng Mã Lỗi Chuẩn / Error Code Reference

Các hàm API trả về mã lỗi số nguyên âm theo quy chuẩn:

| Mã Lỗi / Error Code | Giá trị / Value | 🇻🇳 Ý nghĩa Kỹ thuật | 🇬🇧 Technical Description |
|---|:---:|---|---|
| `NIC_OK` | `0` | Thành công | Operation completed successfully |
| `NIC_IO` | `-1` | Lỗi truyền thông phần cứng I/O | Hardware bus communication failure |
| `NIC_INVALID` | `-2` | Tham số hoặc trạng thái không hợp lệ | Invalid configuration, state, or argument |
| `NIC_NO_MEMORY` | `-3` | Hết bộ nhớ đệm / cấp phát thất bại | Out of heap memory or buffer allocation failed |
| `NIC_AGAIN` | `-4` | Tài nguyên bận, thử lại sau | Resource temporarily unavailable; retry later |
| `NIC_UNSUPPORTED` | `-5` | Tính năng không được phần cứng hỗ trợ | Feature unsupported by silicon or firmware |
| `NIC_TIMEOUT` | `-6` | Quá thời gian phản hồi (Timeout) | Bus, stream, or RPC response timed out |
| `NIC_PROTOCOL` | `-7` | Lỗi sai định dạng gói tin hoặc checksum | Protocol framing or checksum validation failed |
| `NIC_BUSY` | `-8` | Ngoại vi đang thực thi tác vụ khác | Subsystem or hardware bus is currently busy |
| `NIC_NOT_CONNECTED` | `-9` | Chưa kết nối bus hoặc chưa đàm phán | Hardware link not connected or not negotiated |

---

## 5. Các Lớp Socket TCP / UDP / TLS / Socket Networking Classes

Tất cả các lớp socket kế thừa từ các giao diện tiêu chuẩn của Arduino, đảm bảo tương thích với các thư viện mạng phổ biến:

### A. `EspNICClient` (TCP Client)
* Quản lý một phiên socket TCP hướng kết nối; chỉ hỗ trợ ngữ nghĩa di chuyển (**move-only**).
* `connect(host, port)`: Phân giải tên miền DNS thông qua NIC và mở kết nối.
* `connect(IPAddress, port)`: Mở kết nối trực tiếp đến địa chỉ IP đích.
* Triển khai đầy đủ: `available()`, `read()`, `peek()`, `write()`, `connected()`, `flush()`, `stop()`.

### B. `EspNICServer` (TCP Server)
* Khởi tạo máy chủ lắng nghe tại cổng xác định: `EspNICServer server(80);`.
* Kích hoạt máy chủ: `server.begin();`.
* Tiếp nhận kết nối: `server.accept()` hoặc `server.available()` trả về đối tượng `EspNICClient`.
* `server.end()`: Đóng socket máy chủ lắng nghe.

### C. `EspNICUDP` (UDP Datagram)
* `begin(local_port)`: Gắn (bind) vào cổng cục bộ để nhận gói tin.
* `beginPacket(dest_ip, port)`: Thiết lập địa chỉ đích gửi datagram.
* `write(data, size)`: Ghi dữ liệu vào datagram (tối đa 1472 byte).
* `endPacket()`: Phát gói tin ra mạng.
* `parsePacket()`: Kiểm tra gói tin đến, trả về kích thước gói tin đang chờ đọc.
* `remoteIP()` / `remotePort()`: Lấy thông tin nguồn gửi của gói tin vừa nhận.

### D. `EspNICSecureClient` (Bảo mật TLS / HTTPS)
* Hỗ trợ nạp chứng chỉ xác thực máy chủ: `client.setCACert(root_ca_pem)`.
* Bắt buộc cài đặt thời gian thực chính xác cho Master (qua NTP hoặc RTC) trước khi thực hiện handshake HTTPS.
* Mặc định xác thực chứng chỉ CA và Hostname; không hỗ trợ chế độ kết nối thiếu an toàn (`setInsecure`).

---

## 6. Giao tiếp Phía Slave Coprocessor / Slave Endpoint (`EspNICSlave`)

Đối tượng toàn cục `EspNICSlave` (khai báo trong `#include <ESP_NIC_Slave.h>`) được sử dụng trong sketch Arduino chạy trực tiếp trên vi điều khiển phụ:

* `beginSPI(sclk, miso, mosi, cs, handshake, dataready)`: Khởi tạo bus SPI Slave. Truyền `-1` để sử dụng các chân mặc định của dòng chip.
* `beginSDIO()`: Khởi tạo bus SDIO Slave trên các chân silicon cố định.
* `beginUART(tx, rx, baud, port)`: Khởi tạo giao tiếp UART Slave tốc độ cao.
* `poll()`: Bắt buộc gọi định kỳ trong `loop()` của Slave để xử lý luồng gói tin và sự kiện.
* `isMasterConnected()`: Trả về `true` khi Master đã hoàn tất bắt tay giao thức.
* `onCommand(callback)` / `onMessage(callback)` / `onDisplayText(callback)` / `onDisplayProgress(callback)`: Đăng ký các hàm xử lý lệnh và thông điệp nhận từ Master.
* `sendEventToMaster(event_id, data, length)`: Gửi sự kiện từ Slave ngược về Master (tối đa 1400 byte).

---

## 7. Phối hợp Nạp Firmware Trên Mạch / In-System Firmware Flashing Handoff

Đoạn mã mẫu hoàn chỉnh thể hiện cách phối hợp nhịp nhàng giữa **ESP-NIC** và **EspSerialFlasher** / **ESP-ISP**:

```cpp
#include <ESP_NIC.h>
#include <EspSerialFlasher.h> // Thư viện nạp firmware tùy chọn

struct FlasherContext {
  EspFlasherConfig config;
  uint32_t bootloader_offset;
  const uint8_t *bootloader_bin;
  size_t bootloader_size;
  const uint8_t *bootloader_md5;

  uint32_t app_offset;
  const uint8_t *app_bin;
  size_t app_size;
  const uint8_t *app_md5;
};

// Hàm callback nạp firmware được thực thi khi NIC đã giải phóng hoàn toàn bus
bool executeFlashing(void *opaque) {
  auto *ctx = static_cast<FlasherContext *>(opaque);
  
  Serial.println("[Flasher] Khoi tao bo nap Serial Flasher...");
  if (!EspSerialFlasher.begin(ctx->config)) {
    Serial.printf("[Flasher] Khoi tao loi: %s\n", EspSerialFlasher.getLastError());
    return false;
  }

  Serial.println("[Flasher] Ket noi ROM Bootloader cua Slave...");
  if (!EspSerialFlasher.connect(ESP_TARGET_AUTO, 921600)) {
    Serial.printf("[Flasher] Ket noi that bai: %s\n", EspSerialFlasher.getLastError());
    return false;
  }

  // 1. Nap Bootloader
  Serial.println("[Flasher] Dang nap Bootloader...");
  if (!EspSerialFlasher.flashBuffer(ctx->bootloader_offset, ctx->bootloader_bin, ctx->bootloader_size) ||
      !EspSerialFlasher.verifyMD5(ctx->bootloader_offset, ctx->bootloader_size, ctx->bootloader_md5)) {
    Serial.printf("[Flasher] Loi nap Bootloader: %s\n", EspSerialFlasher.getLastError());
    return false;
  }

  // 2. Nap Ung dung chinh (Application)
  Serial.println("[Flasher] Dang nap Application Firmware...");
  if (!EspSerialFlasher.flashBuffer(ctx->app_offset, ctx->app_bin, ctx->app_size) ||
      !EspSerialFlasher.verifyMD5(ctx->app_offset, ctx->app_size, ctx->app_md5)) {
    Serial.printf("[Flasher] Loi nap App: %s\n", EspSerialFlasher.getLastError());
    return false;
  }

  // 3. Reset Slave ve che do chay App binh thuong
  Serial.println("[Flasher] Nap hoan tat! Reset Slave vao Firmware moi...");
  EspSerialFlasher.resetTarget();

  // 4. Bat buoc giai phong cong UART cua Flasher truoc khi return
  EspSerialFlasher.end();
  return true;
}

void performFullFirmwareUpdate() {
  FlasherContext ctx;
  ctx.config.rx_pin = 18;       // Master RX noi sang Slave TX
  ctx.config.tx_pin = 17;       // Master TX noi sang Slave RX
  ctx.config.reset_pin = 6;     // Master GPIO noi sang Slave EN
  ctx.config.boot_pin = 4;      // Master GPIO noi sang Slave BOOT Strap
  ctx.config.flash_baud = 921600;

  // Offset quy dinh boi datasheet/manifest cua tung chip:
  // ESP32 Classic: 0x1000 | ESP32-C5 & P4: 0x2000 | ESP32-C6 & S3: 0x0
  ctx.bootloader_offset = 0x0; 
  ctx.app_offset = 0x10000;

  // Tien hanh goi ham updateFirmware cua ESP-NIC:
  // ESP-NIC se:
  // - Dong cac ket noi mang va giai phong bus SPI/SDIO/UART.
  // - Goi executeFlashing(&ctx).
  // - Khi executeFlashing tra ve true: khoi phuc lai bus truoc do va tu dong ket noi lai Wi-Fi.
  bool success = EspNIC.updateFirmware(executeFlashing, &ctx, "YOUR_SSID", "YOUR_PASSWORD");
  if (success) {
    Serial.println("[ESP-NIC] Cap nhat firmware va khoi phuc mang thanh cong!");
  } else {
    Serial.printf("[ESP-NIC] Cap nhat firmware that bai: %s\n", EspNIC.lastError());
  }
}
```

> [!IMPORTANT]
> **Quy ước Chân BOOT Strap của các dòng chip Slave**:
> * **ESP32 Classic / ESP32-S2 / ESP32-S3**: Chân BOOT là **`GPIO 0`**.
> * **ESP32-C2 / ESP32-C3 / ESP32-C6 / ESP32-H2**: Chân BOOT là **`GPIO 9`** (chân `GPIO 8` cần giữ mức HIGH khi nạp).
> * **ESP32-C5**: Chân BOOT là **`GPIO 28`** (chân `GPIO 27` cần giữ mức HIGH khi nạp).
> * **ESP32-P4**: Chân BOOT là **`GPIO 35`** (chân `GPIO 36` cần giữ mức HIGH khi nạp).

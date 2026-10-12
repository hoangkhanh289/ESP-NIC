# Flashing the ESP-NIC Coprocessor / Hướng dẫn Nạp Firmware Chip Phụ (Bilingual VI / EN)

Tài liệu này cung cấp hướng dẫn kỹ thuật chi tiết về việc nạp các bộ firmware biên dịch sẵn cho vi điều khiển phụ (**ESP32-C6, ESP32-C5, ESP32-S3**) để biến chip thành Card mạng không dây hiệu năng cao cho Master.

This guide provides technical instructions for flashing precompiled coprocessor firmware onto slave microcontrollers (**ESP32-C6, ESP32-C5, ESP32-S3**) to serve as high-performance Network Interface Cards for the host MCU.

---

## 1. Danh mục Cấu hình Firmware Hỗ trợ / Available Profiles

| Chip Slave | Cấu hình Truyền tải / Profiles | Băng tần Wi-Fi / Radio | Giao thức Hỗ trợ / Features |
|---|---|---|---|
| **ESP32-C6** | `spi`, `sdio`, `uart`, `spi-ble` | 2.4 GHz (Wi-Fi 6 802.11ax) | SPI Mode 2, SDIO 4-bit Packet, UART 921.6k, BLE 5 HCI |
| **ESP32-C5** | `spi`, `sdio` | Băng tần kép 2.4 GHz & 5 GHz | SPI Mode 2, SDIO 4-bit Packet (Wi-Fi 6 Dual-Band) |
| **ESP32-S3** | `spi` | 2.4 GHz (Wi-Fi 4 802.11b/g/n) | SPI Mode 2 DMA |

Toàn bộ các file nhị phân trong thư mục này được biên dịch bằng **ESP-IDF v5.5.5**, **ESP-Hosted-MCU v3.0.9** và **protobuf-c v1.5.2**. Tất cả các mã băm SHA-256 đã được kiểm định và lưu giữ trong file `manifest.json` của từng thư mục.

All binaries in this directory were built with **ESP-IDF v5.5.5**, **ESP-Hosted-MCU v3.0.9**, and **protobuf-c v1.5.2**. SHA-256 manifests have been cryptographically verified.

---

## 2. Các Bước Nạp Chi tiết / Step-by-Step Instructions

### 🇻🇳 Tiếng Việt
1. **Lựa chọn thư mục cấu hình**: Chọn đúng thư mục con tương ứng với chip và giao tiếp bạn sử dụng (ví dụ: `firmware/esp32c6/spi/`).
2. **Kết nối phần cứng**: Nối mạch nạp USB-UART vào chân UART0 của chip **Slave** (chân TX/RX, EN và BOOT). Cấp nguồn 3.3V ổn định.
3. **Kiểm tra tính toàn vẹn**: So khớp mã băm SHA-256 của các file `.bin` với thông tin trong `manifest.json`.
4. **Thực thi lệnh nạp**: Thay thế `COM_PORT` bằng cổng serial thực tế trên máy tính và chạy lệnh nạp `esptool`.
5. **Kiểm tra log khởi động**: Mở Serial Monitor ở tốc độ **115,200 baud**, reset chip Slave và kiểm tra thông báo khởi động RPC V2.

### 🇬🇧 English
1. **Select Profile Directory**: Navigate to the exact subfolder matching your target chip and bus transport (e.g., `firmware/esp32c6/spi/`).
2. **Connect Hardware**: Attach your USB-UART serial programmer to the **slave** chip's UART0 pins (TX/RX, EN, and BOOT). Ensure clean 3.3V power.
3. **Verify Integrity**: Match SHA-256 digests against entries recorded in `manifest.json`.
4. **Run Flash Command**: Replace `COM_PORT` with your physical serial port identifier and execute the `esptool` command.
5. **Verify Boot Console**: Open a serial terminal at **115,200 baud**, reset the slave MCU, and confirm RPC V2 initialization.

---

## 3. Lệnh Nạp Chuẩn esptool / esptool Command Syntax

### A. Dành cho ESP32-C6 (SPI / SDIO / UART)
```powershell
python -m esptool --chip esp32c6 --port COM_PORT write_flash `
  0x0000 bootloader.bin `
  0x8000 partition-table.bin `
  0xd000 ota_data_initial.bin `
  0x10000 eh_cp_wifi_sta.bin
```

### B. Dành cho ESP32-C5 (Băng tần kép 2.4G & 5G)
> [!CAUTION]
> **Offset Bootloader của ESP32-C5 là `0x2000`** (không phải `0x0000` như C6 hay S3).

```powershell
python -m esptool --chip esp32c5 --port COM_PORT write_flash `
  0x2000 bootloader.bin `
  0x8000 partition-table.bin `
  0xd000 ota_data_initial.bin `
  0x10000 eh_cp_wifi_sta.bin
```

### C. Dành cho ESP32-S3 (SPI)
```powershell
python -m esptool --chip esp32s3 --port COM_PORT write_flash `
  0x0000 bootloader.bin `
  0x8000 partition-table.bin `
  0xd000 ota_data_initial.bin `
  0x10000 eh_cp_wifi_sta.bin
```

---

## 4. Các Lưu ý Kỹ thuật Quan trọng / Important Technical Notes

1. **Lệnh `write_flash` vs `write-flash`**:
   * Các phiên bản `esptool` cũ sử dụng cú pháp dấu gạch dưới `write_flash`.
   * Các phiên bản `esptool` mới hơn (v4.x+) hỗ trợ cả `write-flash` và `write_flash`.
2. **File `.bin` độc lập ngoài thư mục gốc**:
   * Các file nhị phân có tên riêng ở thư mục ngoài (ví dụ `eh_cp_wifi_sta.bin`) chỉ là **App Image** (nạp tại offset `0x10000`).
   * Nếu nạp vào chip trắng chưa từng nạp bootloader, bắt buộc phải nạp trọn bộ cả 4 file (`bootloader`, `partition-table`, `ota_data_initial` và `app`).
3. **Thông số Bus Vật lý / Bus Operational Parameters**:
   * **SPI**: Chạy ở **Mode 2**, tần số mặc định 10 MHz (hỗ trợ nâng lên 20/40 MHz nếu đường mạch ngắn và có chống nhiễu).
   * **UART**: Tốc độ truyền tải mặc định là **921,600 bps** (8N1).
   * **SDIO**: Hoạt động ở chế độ gói tin (**Packet Mode**). Bản build firmware đã được áp dụng bản vá tối ưu hàng đợi (Queue Draining) giúp chống nghẽn gói tin và giới hạn khung Ethernet ở 1536 byte.

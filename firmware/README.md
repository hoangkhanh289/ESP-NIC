# Coprocessor (Slave) Firmware Guide / Hướng dẫn Firmware Vi điều khiển Phụ (Bilingual VI / EN)

Tài liệu này cung cấp hướng dẫn toàn diện về cấu trúc thư mục, quy trình nạp và cách tự biên dịch firmware chuẩn dành cho vi điều khiển phụ (**ESP32-C6, ESP32-C5, ESP32-S3, ESP32-C3, ESP32**) để hoạt động ăn khớp với thư viện **ESP-NIC**.

This guide outlines directory organization, flashing instructions, and compilation workflows for official coprocessor firmware (**ESP32-C6, ESP32-C5, ESP32-S3, ESP32-C3, ESP32**) designed to interface with the **ESP-NIC** library.

---

> [!IMPORTANT]
> **Tương thích Giao thức RPC V2 / RPC Protocol Compatibility**:
> * **🇻🇳 Tiếng Việt**: Thư viện ESP-NIC phía Master sử dụng giao thức **RPC V2** (dựa trên bản snapshot ESP-Hosted-MCU **3.0.9**). Tuyệt đối **không** sử dụng các bản firmware legacy 2.x (vốn đi kèm trong các gói Arduino ESP32-P4 cũ) vì cấu trúc gói tin không tương thích.
> * **🇬🇧 English**: The ESP-NIC host library implements the **RPC V2** wire protocol (grounded in the ESP-Hosted-MCU **3.0.9** snapshot). Do **not** flash older legacy 2.x firmware binaries, as their framing structures are fundamentally incompatible.

---

## 1. Cấu trúc Thư mục Firmware / Directory Structure

Mỗi cấu hình nạp được đóng gói trong một thư mục con chuyên biệt `firmware/<chip>/<transport>/`:

```text
firmware/
├── esp32c6/
│   ├── spi/              # ESP32-C6 qua bus SPI (Mode 2)
│   ├── sdio/             # ESP32-C6 qua bus SDIO (Packet Mode 4-bit)
│   ├── uart/             # ESP32-C6 qua cổng UART (921,600 baud)
│   └── spi-ble/          # ESP32-C6 SPI kèm tính năng Bluetooth LE HCI
├── esp32c5/
│   ├── spi/              # ESP32-C5 băng tần kép 2.4G & 5G (SPI)
│   └── sdio/             # ESP32-C5 băng tần kép 2.4G & 5G (SDIO)
└── esp32s3/
    └── spi/              # ESP32-S3 qua bus SPI
```

### Mỗi thư mục cấu hình bao gồm / Each profile directory includes:
* **`manifest.json`**: Chứa địa chỉ offset chính xác của từng phân vùng (sinh bởi ESP-IDF) và mã băm SHA-256 đối chiếu toàn vẹn dữ liệu.
* **`sdkconfig`**: File cấu hình chi tiết (định nghĩa chân GPIO thực tế, tần số xung nhịp, cấu hình DMA, bộ đệm).
* **Các file nhị phân hoàn chỉnh / Binary Flash Set**:
  * `bootloader.bin`: Bộ nạp khởi động thế hệ 2.
  * `partition-table.bin`: Bảng phân vùng Flash.
  * `ota_data_initial.bin`: Dữ liệu phân vùng lựa chọn khởi động OTA.
  * `eh_cp_wifi_sta.bin`: Chương trình ứng dụng chính của Coprocessor.

---

## 2. Quy trình Nạp Firmware bằng esptool / Flashing with esptool

### 🇻🇳 Các bước thực hiện:
1. Kết nối mạch nạp USB-to-UART (CP2102, CH340...) vào cổng UART0 của chip **Slave** (không nối vào Master).
2. Mở Device Manager để xác định cổng truyền thông `COM_PORT` tương ứng.
3. Chạy lệnh nạp flash (đối chiếu địa chỉ offset cụ thể trong file `manifest.json` của từng chip):

### 🇬🇧 Step-by-Step Procedure:
1. Connect a USB-to-UART serial adapter directly to the **slave** chip's ROM UART0 pins (do not connect to the master).
2. Check your system's Device Manager / device list to find the allocated serial port (`COM_PORT`).
3. Execute the flashing command using offsets specified in the profile's `manifest.json`:

```powershell
# Ví dụ nạp cho ESP32-C6 (SPI / SDIO) từ thư mục firmware/esp32c6/spi/
python -m esptool --chip esp32c6 --port COM_PORT write-flash `
  0x0 bootloader.bin `
  0x8000 partition-table.bin `
  0xd000 ota_data_initial.bin `
  0x10000 eh_cp_wifi_sta.bin
```

4. **Kiểm tra sau khi nạp / Post-Flash Verification**:
   * Mở Serial Monitor tại tốc độ **115,200 baud**.
   * Nhấn nút Reset (EN) trên chip Slave.
   * Quan sát log khởi động để đảm bảo chip báo cáo khởi tạo thành công giao thức RPC V2 và đúng chế độ truyền tải (SPI/SDIO/UART).

> [!CAUTION]
> **Lưu ý đặc biệt về địa chỉ nạp của ESP32-C5**:
> * ESP32-C5 sử dụng offset Bootloader là **`0x2000`** (trong khi C6, C3, S3 là `0x0`).
> * Luôn luôn đọc file `manifest.json` trong thư mục tương ứng trước khi nạp để tránh ghi sai địa chỉ làm hỏng bộ nhớ flash.

---

## 3. Tự Biên dịch Firmware từ Mã nguồn / Building from Source

Nếu có nhu cầu tùy chỉnh chân GPIO, thêm tính năng hoặc tối ưu xung nhịp, bạn có thể tự biên dịch lại firmware bằng ESP-IDF (yêu cầu **ESP-IDF v5.5** trở lên):

```powershell
# Build firmware cho các cấu hình thông dụng
python tools/build_firmware.py --profiles esp32c6-spi esp32c6-sdio
python tools/build_firmware.py --profiles esp32c5-spi esp32c5-sdio esp32s3-spi
python tools/build_firmware.py --profiles esp32c6-uart
python tools/build_firmware.py --profiles esp32c6-spi --ble
```

Xem thêm hướng dẫn chi tiết từng bước tại [`README_FLASHING.md`](README_FLASHING.md).

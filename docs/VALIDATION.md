# Validation Scope & Test Evidence / Phạm vi Kiểm chứng & Báo cáo Thử nghiệm (Bilingual VI / EN)

Tài liệu này xác định phạm vi kiểm chứng kỹ thuật, ma trận biên dịch, kết quả kiểm tra hồi quy giả lập (regression tests) và các giới hạn kiểm thử thực tế đối với gói phân phối thư viện **ESP-NIC**.

This document defines the validation scope, compilation matrix, simulated regression test results, and hardware testing limitations for the **ESP-NIC** library distribution.

---

## 1. Tóm tắt Kiểm chứng Phát hành / Release Validation Summary

| Hạng mục / Scope | Trạng thái / Status | 🇻🇳 Chi tiết Kỹ thuật | 🇬🇧 Technical Summary |
|---|:---:|---|---|
| **Biên dịch 11 Hồ sơ ABI** | **PASS** | 11 archive tĩnh (`libesp_nic.a`) cho ESP32 và ARM Cortex-M được biên dịch thành công. | All 11 precompiled static archives for ESP32 and ARM Cortex-M compiled cleanly. |
| **Kiểm định Ký hiệu & Chuỗi** | **PASS** | Vượt qua bài kiểm tra `nm` (chỉ xuất các hàm công khai) và `strings` (không lộ đường dẫn nhạy cảm). | Audited with `nm` (strict public exports) and `strings` (zero private paths). |
| **Tích hợp ESP-CARD-WIFI** | **PASS** | Ứng dụng SPI consumer liên kết thành công với thư viện tĩnh ESP32-S3 trên Arduino Core 3.3.12. | SPI consumer successfully linked against ESP32-S3 archive on Arduino Core 3.3.12. |
| **Mô phỏng Giao thức Slave** | **PASS** | Vượt qua các kịch bản bắt tay ACK, đánh thức SPI khi rảnh, bất biến bộ đệm DMA và phục hồi UART nhiễu. | Validated ACK negotiation, idle-SPI wakeup, DMA immutability, and noisy UART recovery. |
| **Kênh RPC & Màn hình** | **PASS** | Kiểm tra thành công hàng đợi hiển thị OLED/TFT, báo cáo IP máy chủ và adapter HCI Bluetooth. | Verified deferred display events, host IP reports, and Bluetooth HCI adapters. |
| **Đóng gói ZIP Phát hành** | **PASS** | Gói `dist/ESP_NIC-1.0.0.zip` được đóng gói với kiểm tra toàn vẹn mã băm SHA-256 và CRC. | Release ZIP packaged and validated with strict SHA-256 and CRC integrity checks. |

---

## 2. Chi tiết Kết quả Kiểm tra Giả lập / Simulated Test Results

### 🇻🇳 Tiếng Việt
* **Đàm phán Giao thức & Bắt tay Bus (Protocol Handshake)**:
  * Kiểm tra bắt tay Mode 2 SPI với 2 tín hiệu Handshake (HS) và Data Ready (DR).
  * Kiểm tra truyền nhận gói tin SDIO Packet Mode 4-bit với cơ chế chống tràn bộ đệm.
  * Kiểm tra tự động tính toán mã kiểm tra (checksum) và khả năng tái đồng bộ luồng UART khi xảy ra mất gói tin hoặc nhiễu đường truyền.
* **Bộ đệm DMA & Quyền Sở hữu Bộ nhớ (DMA & Memory Ownership)**:
  * Đảm bảo tính bất biến của bộ đệm DMA trong suốt chu kỳ truyền nhận.
  * Hàng đợi hiển thị (Display Queue) hoạt động trên tác vụ riêng biệt, tự động loại bỏ frame thừa khi quá tải thay vì chặn đứng tác vụ mạng chính.
* **Bluetooth HCI Passthrough**:
  * Mô phỏng chuyển tiếp các gói tin chuẩn Bluetooth H4 giữa Controller trên Slave và Host Stack trên Master qua bus truyền tải.

### 🇬🇧 English
* **Protocol Handshake & Bus Negotiation**:
  * Validated Mode 2 SPI handshake utilizing Handshake (HS) and Data Ready (DR) flow-control signals.
  * Tested SDIO 4-bit Packet Mode framing with bounded queue backpressure.
  * Validated UART frame checksums and automatic stream resynchronization over noisy physical channels.
* **DMA & Memory Buffer Ownership**:
  * Enforced DMA buffer immutability during active physical transmission.
  * Display RPC queue operates on a detached worker task; drops excess frames upon saturation rather than blocking core network polling.
* **Bluetooth HCI Passthrough**:
  * Simulated bidirectional routing of raw Bluetooth H4 packets between coprocessor controller and host stack.

---

## 3. Giới hạn Kiểm thử & Khuyến cáo Phần cứng / Testing Limits & Hardware Disclaimers

> [!WARNING]
> **Giới hạn Kiểm thử Phần cứng / Hardware Testing Disclaimer**:
> * **🇻🇳 Tiếng Việt**: Toàn bộ các kết quả kiểm chứng trong tài liệu này được thực hiện thông qua **biên dịch liên kết thực tế (build/link verification) và kịch bản mô phỏng phần mềm (mock driver simulation)**.
> * **Chưa đo đạc xung điện áp thực tế** trên máy hiện sóng (Oscilloscope) hay bộ phân tích logic (Logic Analyzer).
> * **Chưa đo đạc khoảng cách truyền sóng vô tuyến RF thực tế**, công suất phát Wi-Fi/Bluetooth hoặc thông lượng mạng tối đa trong môi trường nhiễu sóng vật lý.
> * Người dùng cần đối chiếu sơ đồ chân bo mạch, trở kéo pull-up và nguồn cấp 3.3V đủ dòng cho chip Slave trước khi triển khai thực tế.
>
> * **🇬🇧 English**: All verification results documented here are established through **compiler link checks and software mock driver regression suites**.
> * **Electrical signal waveforms have NOT been measured** via oscilloscope or logic analyzer.
> * **RF wireless range, actual throughput, and Wi-Fi/BLE radio sensitivity have NOT been tested in live physical environments**.
> * Developers must verify their physical board pinout, required pull-up resistors, and provide adequate 3.3V power decoupling for the coprocessor in production hardware.

---

## 4. Tài liệu Đối chiếu / Reference Documents

* Danh mục mã băm SHA-256 của toàn bộ các file archive tĩnh: [`binary-manifest.json`](binary-manifest.json)
* Chi tiết kết quả kiểm tra từng module: [`validation-report.json`](validation-report.json)
* Hướng dẫn bảo vệ nhị phân: [`HARDENING.md`](HARDENING.md)
* Hướng dẫn nạp firmware chip phụ: [`../firmware/README_FLASHING.md`](../firmware/README_FLASHING.md)

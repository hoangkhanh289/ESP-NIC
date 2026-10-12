# Binary Hardening & Packaging Security / Bảo vệ Bản Phân phối Nhị phân (Bilingual VI / EN)

Tài liệu này giải thích các kỹ thuật tối ưu hóa mã nhị phân, quy trình ẩn danh hóa biểu tượng (symbol anonymization), tiêu chuẩn đóng gói và các giới hạn kỹ thuật áp dụng cho các thư viện tĩnh `.a` trong gói phát hành **ESP-NIC**.

This document outlines the compiler optimizations, symbol anonymization procedures, packaging standards, and technical limitations applied to the precompiled `.a` static archives in the **ESP-NIC** distribution.

---

## 1. Cờ Biên dịch & Tối ưu hóa / Compiler Flags & Optimizations

### 🇻🇳 Tiếng Việt
Toàn bộ mã nguồn lõi trong các thư viện tĩnh được biên dịch với tập cờ tối ưu hóa kích thước và bảo mật nghiêm ngặt:
* **Tối ưu kích thước & tốc độ**: Sử dụng `-Os`, `-ffunction-sections`, `-fdata-sections` và `-fomit-frame-pointer` để loại bỏ các đoạn mã thừa khi liên kết.
* **Loại bỏ thông tin gỡ lỗi**: Biên dịch với `-g0`, `-fno-ident`, `-DNDEBUG`, tắt hoàn toàn RTTI (Run-Time Type Information) và cơ chế C++ Exceptions để tiết kiệm bộ nhớ RAM/Flash.
* **Ẩn danh hóa giao diện nội bộ**: Sử dụng `-fvisibility=hidden`. Mọi hàm nội bộ không công khai đều bị ẩn hoàn toàn khỏi bảng ký hiệu.
* **Ánh xạ đường dẫn phát triển**: Toàn bộ đường dẫn tuyệt đối của máy tính phát triển được ánh xạ lại bằng các cờ prefix map của GCC, ngăn ngừa rò rỉ cấu trúc thư mục và tên người dùng cục bộ.

### 🇬🇧 English
Core library units are compiled with strict size-optimization and hardening flags:
* **Size & Performance**: Compiled with `-Os`, `-ffunction-sections`, `-fdata-sections`, and `-fomit-frame-pointer` to facilitate dead-code elimination at link time.
* **Stripping Debug Telemetry**: Uses `-g0`, `-fno-ident`, `-DNDEBUG`, with C++ RTTI and Exceptions entirely disabled to minimize footprint.
* **Hiding Internal Interfaces**: Employs `-fvisibility=hidden` so non-public routines are excluded from the exported symbol table.
* **Path Mapping**: Host absolute build paths are neutralized via GCC prefix maps, preventing local directory and username leaks.

---

## 2. Quy trình Liên kết & Ẩn danh Biểu tượng / Symbol Anonymization Process

```text
  [Private Sources (.cpp/.c)]
              │
              ▼  (gcc -Os -g0 -fvisibility=hidden -DNDEBUG)
  [Intermediate Objects (.o)]
              │
              ▼  (ld -r: Partial Linking)
  [Single Aggregate Object]
              │
              ▼  (objcopy / strip: Anonymize internal symbols, strip debug/notes)
  [Hardened Static Archive (libesp_nic.a)]
              │
              ▼  (nm & strings audit against binary-manifest.json)
  [Verified Public Distribution]
```

### 🇻🇳 Tiếng Việt
1. **Liên kết từng phần (Partial Linking)**: Các đơn vị biên dịch của từng kiến trúc CPU được liên kết sơ bộ (`ld -r`) thành một khối hợp nhất.
2. **Ẩn danh hóa biểu tượng nội bộ**: Tất cả các hàm, biến tĩnh và chữ ký COMDAT nội bộ được chuyển thành dạng ẩn danh cục bộ. Các section siêu dữ liệu được đổi tên mà vẫn bảo toàn cấu trúc literal-pool của vi xử lý Xtensa và chỉ mục ngoại lệ của ARM Cortex-M.
3. **Loại bỏ Section rác**: Loại bỏ triệt để các section `.comment`, `.note.GNU-stack` và Build IDs.
4. **Điểm nhập duy nhất (Single Dispatcher Entrypoint)**: Mỗi archive chỉ công khai một hàm dispatcher duy nhất theo chuẩn C-ABI (`esp_nic_v1_*`), kết nối trực tiếp với lớp vỏ C++ `ESP_NIC.h` mà không làm lộ tên các lớp điều khiển nội bộ.

### 🇬🇧 English
1. **Partial Linking**: Translation units for each CPU architecture are partially linked (`ld -r`) into a single aggregate object.
2. **Symbol Localization**: All internal functions, static variables, and COMDAT signatures are localized and anonymized. Metadata sections are renamed while preserving Xtensa literal-pool pairings and ARM exception indices.
3. **Section Stripping**: Strips obsolete `.comment`, `.note.GNU-stack`, and build ID sections, ensuring deterministic build archives.
4. **Single Dispatcher Interface**: Each archive exposes exactly one public C-ABI dispatcher entry point (`esp_nic_v1_*`), interfacing cleanly with the `ESP_NIC.h` facade without exposing private internal class hierarchies.

---

## 3. Kiểm định & Xác thực Tự động / Verification & Auditing

### 🇻🇳 Tiếng Việt
Mỗi bản build nhị phân phát hành đều phải vượt qua bộ kịch bản kiểm tra tự động trước khi đóng gói:
* **Kiểm tra Ký hiệu Xuất (Export Audit)**: Sử dụng lệnh `nm` để đảm bảo archive CHỈ chứa đúng các entrypoint được cấp phép.
* **Kiểm tra Chuỗi Văn bản Nhạy cảm (Strings Audit)**: Dùng lệnh `strings` quét toàn bộ archive nhị phân để khẳng định không tồn tại:
  * Đường dẫn ổ đĩa tuyệt đối (`E:\GITHUB`, `C:\Users`).
  * Tên người dùng máy phát triển.
  * Tên các file mã nguồn nội bộ hoặc chuỗi diagnostic thô.
* **Đối chiếu Mã băm SHA-256**: Kích thước và mã băm SHA-256 của từng file `libesp_nic.a` được ghi nhận minh bạch tại [`binary-manifest.json`](binary-manifest.json).

### 🇬🇧 English
Every release archive is audited through automated verification scripts:
* **Symbol Audit**: Employs `nm` to guarantee the archive exports ONLY authorized public entry points.
* **Sensitive Strings Audit**: Scans binary text with `strings` to verify complete absence of:
  * Absolute developer filesystem paths (`E:\GITHUB`, `C:\Users`).
  * Host operating system usernames.
  * Private source filenames or verbose internal debug diagnostics.
* **SHA-256 Fingerprinting**: File sizes and SHA-256 checksums are documented in [`binary-manifest.json`](binary-manifest.json).

---

## 4. Giới hạn Kỹ thuật & Cảnh báo / Limitations & Disclaimers

> [!CAUTION]
> **Giới hạn Bảo vệ Kỹ thuật đảo ngược / Reverse Engineering Disclaimer**:
> * **🇻🇳 Tiếng Việt**: Quy trình Hardening giúp mã nguồn nhị phân gọn nhẹ, tối ưu bộ nhớ và bảo vệ tên hàm nội bộ; **đây không phải là giải pháp chống dịch ngược tuyệt đối**. Các công cụ dịch ngược dịch mã máy như Ghidra hoặc IDA Pro vẫn có thể phân tích luồng lệnh assembly và logic thuật toán.
> * **🇬🇧 English**: Hardening optimizes footprint and obfuscates symbol metadata; **it does not prevent reverse engineering**. Decompilers such as Ghidra or IDA Pro can still inspect machine instructions and recover control-flow graphs.

> [!NOTE]
> **Giữ nguyên Bản quyền Nguồn mở / Open-Source Rights**:
> Toàn bộ quyền phân phối mã nguồn mở theo giấy phép **Apache-2.0** và ghi nhận tác giả gốc của Espressif Systems đều được bảo toàn nguyên vẹn trong [LICENSE](../LICENSE) và [NOTICE](../NOTICE). Việc đóng gói nhị phân không làm phương hại đến quyền sử dụng của cộng đồng.

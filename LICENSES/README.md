# License Information & Inventory / Danh mục Giấy phép & Bản quyền (Bilingual VI / EN)

Tài liệu này cung cấp bản kiểm kê chi tiết về giấy phép phân phối, quyền sở hữu trí tuệ và nghĩa vụ pháp lý khi tái phân phối mã nguồn hoặc các file nhị phân trong dự án **ESP-NIC**.

This document provides a compliance inventory and licensing overview governing the distribution of source code and binary assets within the **ESP-NIC** repository.

---

> [!IMPORTANT]
> **Hiệu lực của Định danh SPDX / Authority of SPDX Identifiers**:
> * **🇻🇳 Tiếng Việt**: Định danh **SPDX identifier** được ghi rõ ở phần đầu của mỗi file mã nguồn là căn cứ pháp lý có giá trị cao nhất và mang tính ràng buộc. Văn bản này đóng vai trò bản tóm tắt kiểm kê phục vụ đóng gói và phát hành; không thay thế cho các thông báo bản quyền cụ thể trong từng file.
> * **🇬🇧 English**: The **SPDX identifier** declared inside individual source files is legally authoritative and binding. This document provides a compliance summary for packaging and release; it does not supersede file-level copyright headers.

---

## 1. Giấy phép Mã nguồn Dự án / Project Source Code Licensing

File [`LICENSE`](../LICENSE) tại thư mục gốc là giấy phép **Apache-2.0** chính của dự án:

| Hạng mục Mã nguồn / Source Category | Giấy phép / License | 🇻🇳 Phạm vi Áp dụng | 🇬🇧 Applicable Scope |
|---|---|---|---|
| **Mã nguồn Chính (Core Source)** | [Apache-2.0](Apache-2.0) | Toàn bộ mã nguồn thư viện Arduino, driver bus và facade C++. | Primary Arduino library code, bus drivers, and C++ facade. |
| **Module Nhân Linux (Kernel Module)** | [GPL-2.0-only](GPL-2.0) | Mã nguồn các thành phần giao tiếp tương thích nhân hệ điều hành Linux. | Linux kernel-module communication sources. |
| **Mã nguồn Kép (Dual-Licensed Files)** | `GPL-2.0 OR Apache-2.0` | Các file định nghĩa giao thức dùng chung; người nhận có quyền chọn một trong hai giấy phép. | Shared protocol definitions; recipient may select either license option. |
| **Ví dụ Mẫu & Test (Examples & Tests)** | [CC0-1.0](CC0-1.0) / [Unlicense](Unlicense) | Toàn bộ các sketch mẫu trong `examples/` được miễn trừ bản quyền để người dùng tự do sao chép. | Examples and test sketches dedicated to the public domain. |

---

## 2. Thành phần Bên thứ Ba Tích hợp / Bundled Third-Party Components

* **protobuf-c**: Được tích hợp tại thư mục `common/serializers/third_party/msg_codec/protobuf-c/`, tuân thủ theo giấy phép **BSD-3-Clause** đi kèm trong thư mục đó.
* **Các thành phần Vendored từ Espressif**: Giữ nguyên toàn bộ thông báo bản quyền gốc của Espressif Systems trong file [`NOTICE`](../NOTICE). Mọi thông báo này bắt buộc phải được duy trì khi phân phối lại.

---

## 3. Nghĩa vụ Khi Đóng gói & Phát hành / Distribution & Compliance Requirements

> [!NOTE]
> Khi tái phân phối thư viện **ESP-NIC** dưới dạng mã nguồn hoặc thư viện tĩnh nhị phân:
> 1. **Bảo toàn Thẻ định danh SPDX**: Tuyệt đối không xóa hoặc chỉnh sửa các thẻ SPDX và phần header bản quyền ở đầu các file.
> 2. **Đính kèm Bản sao Giấy phép**: Đảm bảo các file giấy phép `Apache-2.0`, `GPL-2.0`, `CC0-1.0` và `Unlicense` được đóng gói đầy đủ trong các bản phát hành release ZIP.
> 3. **Ghi nhận protobuf-c**: Luôn duy trì thông báo bản quyền BSD-3-Clause khi đóng gói các bộ phân giải tuần tự protobuf-c.
> 4. **Kiểm tra File Độc lập**: Không tự ý suy diễn một giấy phép chung cho toàn bộ dự án; luôn đối chiếu định danh SPDX cụ thể của từng file mã nguồn.

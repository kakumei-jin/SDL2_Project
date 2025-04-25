# 🍎 Apple Timer Game - SDL2

Một game nhỏ viết bằng **SDL2**, nơi bạn điều khiển nhân vật để **ăn táo trong vòng 5 giây**, nếu không sẽ **thua cuộc**!

---

## 🎮 Gameplay

- Điều khiển nhân vật bằng các phím **WASD**.
- Một **quả táo (hình vuông đỏ)** sẽ xuất hiện ngẫu nhiên trên màn hình.
- Bạn có **5 giây** để di chuyển đến và ăn được táo.
- Nếu **không ăn kịp trong thời gian quy định**, màn hình sẽ hiện **Game Over**.

---

## 🛠 Cài đặt & Chạy game

### 1. Yêu cầu:
- **SDL2** (tối thiểu)
- Trình biên dịch hỗ trợ C++17 trở lên

### 2. Cài SDL2 (nếu chưa có)

#### Trên Windows (Visual Studio):
- Tải SDL2 Development Library từ: https://www.libsdl.org/download-2.0.php
- Giải nén và thiết lập include/lib trong project

#### Trên Linux:
```bash
sudo apt update
sudo apt install libsdl2-dev


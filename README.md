🍏 Apple Catching
Apple Catching là một trò chơi platformer mini, nơi bạn phải nhanh tay ăn được quả táo trong vòng 8 giây. Nếu không kịp, bạn sẽ thua!
Game được lập trình bằng C++ sử dụng SDL2 cho đồ họa và xử lý input.

🕹️ Gameplay
Người chơi điều khiển nhân vật di chuyển qua lại, nhảy và ăn item táo.

Mỗi quả táo xuất hiện, bạn có 8 giây để ăn được nó.

Nếu ăn thành công: tiếp tục đến quả táo mới.

Nếu không kịp: You Lost.

🎨 Assets sử dụng
Pixel Frog - Pixel Adventure (sprites nhân vật, nền, tilemap)

Brackeys' Platformer Assets Bundle (vật phẩm, một số chi tiết trang trí)

📎 Link tải Asset Pixel Frog - Pixel Adventure (https://pixelfrog-assets.itch.io/pixel-adventure-1)
📎 Link tải Asset Brackeys' Platformer Bundle (https://brackeysgames.itch.io/brackeys-platformer-bundle)

 + Pixel Adventure: Những asset này được phát hành theo giấy phép Creative Commons Zero (CC0).
Bạn được phép phân phối, chỉnh sửa, biến tấu, và sử dụng chúng trong bất kỳ hình thức hoặc định dạng nào, kể cả cho mục đích thương mại.
Không bắt buộc ghi nguồn (attribution).
+ Brackeys' Platformer: Những asset này được phát hành theo giấy phép Creative Commons Zero (CC0).
Bạn được phép phân phối, chỉnh sửa, biến tấu, và sử dụng chúng trong bất kỳ hình thức hoặc định dạng nào, kể cả cho mục đích thương mại.
Không bắt buộc ghi nguồn (attribution).


🛠️ Công nghệ
Ngôn ngữ lập trình: C++

Thư viện chính: SDL2

Các thư viện hỗ trợ:

SDL2_image (load hình ảnh)

SDL2_mixer (âm thanh, nếu thêm sau này)

🎬 Demo Gameplay
📺 Xem video demo tại đây: https://youtu.be/Vq-ZJoa8Rq0

📦 Tải game
🔗 Link tải bản chơi thử (thêm link tải .zip hoặc file .exe ở đây)

📜 Hướng dẫn build
Cài đặt SDL2 và SDL2_image (nếu build local).

Build project bằng Visual Studio 2022 hoặc g++.

Ví dụ build với g++:

bash
Copy
Edit
g++ main.cpp game.cpp player.cpp map.cpp apple.cpp -o AppleCatching -lSDL2 -lSDL2_image

✅ Tiến độ hiện tại
 Điều khiển nhân vật di chuyển bằng phím W/A/S/D và nhảy bằng SPACE

 Sinh táo ngẫu nhiên trên map

 Timer 8 giây cho mỗi quả táo

 Game over nếu không ăn kịp

 Thêm âm thanh và hiệu ứng (đang phát triển)

💬 Góp ý hoặc báo lỗi
Mọi góp ý vui lòng gửi về email: katoriwork160306@gmail.com hoặc GitHub.

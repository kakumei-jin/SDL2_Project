#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Map.h"
#include "Player.h"
#include <vector>
#include "apple.h"

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int width, int height);
    void run();
    bool running() const;
    void incrementScore();

private:
    
    //  background scrolling
    std::vector<SDL_Texture*> bgLayers;
    std::vector<int>            bgTileW, bgTileH;   // kích thước mỗi texture
    std::vector<float>          bgOffsets;          // offset current
    std::vector<float>          bgSpeeds;           // pixels/frame 

    void handleEvents();
    void update();
    void render();
    void clean();
    void reset(); // Thêm hàm reset game


    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    bool gameOver = false;
    const Uint32 appleTimeout = 8000; // 8 giây để ăn táo

    Map map;
    Player player;
    Apple apple;

    SDL_Texture* backgroundTex;
    float backgroundScrollX; 

    Uint32 frameStart;
    int frameTime;
    const int frameDelay = 1000 / 60;

    // score và high score
    int score;
    int highScore;
    TTF_Font* font;           // Font cho score, high score, timer
    TTF_Font* gameOverFont;   // Font riêng cho "You Lost"
    SDL_Texture* scoreTexture;
    SDL_Texture* highScoreTexture;
    SDL_Texture* timerTexture;    // Texture cho thời gian đếm ngược
    SDL_Texture* gameOverTexture; // Texture cho "You Lost"
    SDL_Texture* restartTexture; // Texture cho nút Restart
    SDL_Rect scoreRect;
    SDL_Rect highScoreRect;
    SDL_Rect timerRect;
    SDL_Rect gameOverRect;
    SDL_Rect restartRect;       // Rect cho nút Restart

    void updateScoreDisplay(); // Cập nhật texture hiển thị điểm
    void updateTimerDisplay(Uint32 remainingTime); // Cập nhật thời gian đếm ngược
    void loadHighScore();                         // Đọc high score từ file
    void saveHighScore();                         // Lưu high score vào file
};

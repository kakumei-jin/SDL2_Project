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
    Apple apple;
    //  background scrolling
    std::vector<SDL_Texture*> bgLayers;
    std::vector<int>            bgTileW, bgTileH;   // kích thước mỗi texture
    std::vector<float>          bgOffsets;          // offset current
    std::vector<float>          bgSpeeds;           // pixels/frame 

    void handleEvents();
    void update();
    void render();
    void clean();
    bool init();


    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    bool gameOver = false;
    const Uint32 appleTimeout = 5000; // 5 giây

    Map map;
    Player player;

    SDL_Texture* backgroundTex;
    float backgroundScrollX; 

    Uint32 frameStart;
    int frameTime;
    const int frameDelay = 1000 / 60;

    // Score và high score
    int score;
    int highScore;
    TTF_Font* font;
    SDL_Texture* scoreTexture;
    SDL_Texture* highScoreTexture;
    SDL_Rect scoreRect;
    SDL_Rect highScoreRect;

    void updateScoreDisplay(); // Cập nhật texture hiển thị điểm
};

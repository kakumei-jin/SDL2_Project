#pragma once
#include <SDL.h>
#include "Map.h"
#include "Player.h"
#include <vector>

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int width, int height);
    void run();
    bool running() const;

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

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;

    Map map;
    Player player;

    SDL_Texture* backgroundTex;
    float backgroundScrollX; 

    Uint32 frameStart;
    int frameTime;
    const int frameDelay = 1000 / 60;
};

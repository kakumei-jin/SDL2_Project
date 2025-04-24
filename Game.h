#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Player.h"
#include "Map.h"
#include "Constants.h"

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int width, int height);
    void run();
    void clean();
    bool running() const;

private:
    void handleEvents();
    void update();
    void render();

    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTex; 

    Player player;
    Map map;

    Uint32 frameStart;
    int frameTime;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
};
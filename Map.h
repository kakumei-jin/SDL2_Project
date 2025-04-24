#pragma once
#include <SDL.h>
#include "Constants.h"

class Map {
public:
    Map();
    ~Map();

    void init(const char* tilesetPath, SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer);
    bool isColliding(int x, int y, int w, int h) const;

private:
    SDL_Rect getTileSrcRect(int tileID) const;

    SDL_Texture* tileset;
    int mapData[MAP_ROWS][MAP_COLS];
};
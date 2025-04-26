#pragma once
#include <SDL.h>
#include <string>
#include "Constants.h"
#include "Map.h"

class Apple {
public:
    Apple();
    ~Apple();

    void init(SDL_Renderer* renderer, const Map& map);
    void update(const Map& map);
    void render(SDL_Renderer* renderer);
    bool isCollected(const SDL_Rect& playerRect) const;
    void respawn(const Map& map); 

private:
    void spawn(const Map& map);
    void updateAnimation();

    SDL_Texture* texture;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int frame;
    int frameCount;
    int frameDelay;
    const int totalFrames = 17;
    bool active;
    Uint32 spawnTime;
    float x, y;
    int scale;
};
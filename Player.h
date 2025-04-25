#pragma once
#include <SDL.h>
#include <map>
#include <string>
#include "Constants.h"

class Map;

class Player {
public:
    Player();
    ~Player();

    void init(SDL_Renderer* renderer);
    void handleInput(const Uint8* keystate);
    void update(const Map& map);
    void render(SDL_Renderer* renderer);

private:
    struct Animation {
        SDL_Texture* texture = nullptr;
        int frames = 0;
        int frameW = 0;
        int frameH = 0;
    };

    void loadAnimations(SDL_Renderer* renderer);

    float x, y;
    float speed;
    float velX, velY;

    bool onGround;
    bool isMovingHorizontally;
    bool justJumped;

    int frame;
    int frameDelay;
    int frameCount;

    std::string currentAnim;
    SDL_RendererFlip flip;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    int playerScale;

    std::map<std::string, Animation> animations;
};
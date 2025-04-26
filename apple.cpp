#include "Apple.h"
#include "TextureManager.h"
#include <random>
#include <cstdio>

Apple::Apple() :
    texture(nullptr),
    frame(0),
    frameCount(0),
    frameDelay(6),
    active(false),
    spawnTime(0),
    x(0.0f),
    y(0.0f),
    scale(2)
{
    srcRect = { 0, 0, 32, 32 };
    dstRect = { 0, 0, 32 * scale, 32 * scale };
}

Apple::~Apple() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Apple::init(SDL_Renderer* renderer, const Map& map) {
    texture = TextureManager::loadTexture("assets/apple.png", renderer);
    if (!texture) {
        printf("Failed to load apple texture: %s\n", IMG_GetError());
        return;
    }
    spawn(map);
    printf("Apple initialized.\n");
}

void Apple::spawn(const Map& map) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distX(0, MAP_COLS - 1);
    std::uniform_int_distribution<int> distY(0, MAP_ROWS - 2);

    bool validPosition = false;
    int tilePixelW = TILE_WIDTH * TILE_SCALE;
    int tilePixelH = TILE_HEIGHT * TILE_SCALE;

    while (!validPosition) {
        int col = distX(rng);
        int row = distY(rng);
        x = static_cast<float>(col * tilePixelW);
        y = static_cast<float>(row * tilePixelH);
        dstRect = { static_cast<int>(x), static_cast<int>(y), 32 * scale, 32 * scale };
        if (!map.isColliding(dstRect.x, dstRect.y, dstRect.w, dstRect.h)) {
            validPosition = true;
        }
    }

    active = true;
    spawnTime = SDL_GetTicks();
    frame = 0;
    frameCount = 0;
}

void Apple::respawn(const Map& map) {
    spawn(map); 
}

void Apple::update(const Map& map) {
    if (!active) return;

    updateAnimation();

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - spawnTime >= 5000) {
        spawn(map);
    }

    dstRect.x = static_cast<int>(x);
    dstRect.y = static_cast<int>(y);
}

void Apple::updateAnimation() {
    frameCount++;
    if (frameCount >= frameDelay) {
        frame = (frame + 1) % totalFrames;
        frameCount = 0;
        srcRect.x = frame * 32;
        srcRect.y = 0;
        srcRect.w = 32;
        srcRect.h = 32;
    }
}

void Apple::render(SDL_Renderer* renderer) {
    if (!active || !texture) return;
    SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
}

bool Apple::isCollected(const SDL_Rect& playerRect) const {
    if (!active) return false;

    SDL_Rect appleRect = dstRect;
    return (playerRect.x < appleRect.x + appleRect.w &&
        playerRect.x + playerRect.w > appleRect.x &&
        playerRect.y < appleRect.y + appleRect.h &&
        playerRect.y + playerRect.h > appleRect.y);
}
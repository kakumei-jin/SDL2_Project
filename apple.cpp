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

    int tilePixelW = TILE_WIDTH * TILE_SCALE; 
    int tilePixelH = TILE_HEIGHT * TILE_SCALE; 

    int maxCol = (WINDOW_WIDTH - (32 * scale)) / tilePixelW; 
    int maxRow = (WINDOW_HEIGHT - (32 * scale)) / tilePixelH; 

    std::uniform_int_distribution<int> distX(0, maxCol); 
    std::uniform_int_distribution<int> distY(2, maxRow); 

    bool validPosition = false;
    while (!validPosition) {
        int col = distX(rng);
        int row = distY(rng);
        x = static_cast<float>(col * tilePixelW);
        y = static_cast<float>(row * tilePixelH);
        dstRect = { static_cast<int>(x), static_cast<int>(y), 32 * scale, 32 * scale };

        if (!map.isColliding(dstRect.x, dstRect.y, dstRect.w, dstRect.h)) {
            bool hasGroundBelow = false;
            int checkRow = row + 1;
            int maxJumpHeightRows = 9; 
            while (checkRow < MAP_ROWS && checkRow <= row + maxJumpHeightRows) {
                if (map.isColliding(dstRect.x, checkRow * tilePixelH, dstRect.w, dstRect.h)) {
                    hasGroundBelow = true;
                    break;
                }
                checkRow++;
            }

            if (hasGroundBelow) {
                validPosition = true;
            }
        }
    }

    active = true;
    spawnTime = SDL_GetTicks();
    frame = 0;
    frameCount = 0;
    printf("Apple spawned at x: %f, y: %f (row: %d, col: %d)\n", x, y, static_cast<int>(y / tilePixelH), static_cast<int>(x / tilePixelW));
}

void Apple::respawn(const Map& map) {
    spawn(map);
}

void Apple::update(const Map& map) {
    if (!active) return;

    updateAnimation();

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

Uint32 Apple::getSpawnTime() const {
    return spawnTime;
}
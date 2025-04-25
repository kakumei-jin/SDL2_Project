#include "Player.h"
#include "TextureManager.h"
#include "Map.h"
#include <SDL.h>
#include <string>
#include <cstdio>

Player::Player() :
    x(100.0f), y(500.0f),
    speed(MOVE_SPEED),
    velX(0.0f), velY(0.0f),
    onGround(false),
    isMovingHorizontally(false),
    justJumped(false),
    frame(0), frameDelay(6), frameCount(0),
    currentAnim("idle"),
    flip(SDL_FLIP_NONE),
    srcRect{ 0, 0, 32, 32 },
    playerScale(2)
{

}

Player::~Player() {
    animations.clear();
}

void Player::init(SDL_Renderer* renderer) {
    loadAnimations(renderer);
    if (!animations.empty() && animations.count(currentAnim)) {
        Animation& startAnim = animations[currentAnim];
        dstRect = { static_cast<int>(x), static_cast<int>(y),
                    startAnim.frameW * playerScale, startAnim.frameH * playerScale };
    }
    else {
        dstRect = { static_cast<int>(x), static_cast<int>(y),
                    32 * playerScale, 32 * playerScale };
    }
    printf("Player initialized.\n");
}

void Player::loadAnimations(SDL_Renderer* renderer) {
    printf("Loading player animations...\n");
    animations["idle"] = { TextureManager::loadTexture("assets/animation/idle32x32.png", renderer), 11, 32, 32 };
    animations["run"] = { TextureManager::loadTexture("assets/animation/run32x32.png", renderer), 12, 32, 32 };
    animations["jump"] = { TextureManager::loadTexture("assets/animation/jump32x32.png", renderer), 1, 32, 32 };
    animations["fall"] = { TextureManager::loadTexture("assets/animation/fall32x32.png", renderer), 1, 32, 32 };
    animations["hit"] = { TextureManager::loadTexture("assets/animation/hit32x32.png", renderer), 7, 32, 32 };
    animations["double_jump"] = { TextureManager::loadTexture("assets/animation/doublejump32x32.png", renderer), 6, 32, 32 };
    animations["wall_jump"] = { TextureManager::loadTexture("assets/animation/walljump32x32.png", renderer), 5, 32, 32 };
    printf("Player animations loaded.\n");
}

void Player::handleInput(const Uint8* keystate) {
    isMovingHorizontally = false;
    velX = 0;

    if (keystate[SDL_SCANCODE_A]) {
        velX = -speed;
        isMovingHorizontally = true;
        flip = SDL_FLIP_HORIZONTAL;
    }
    if (keystate[SDL_SCANCODE_D]) {
        velX = speed;
        isMovingHorizontally = true;
        flip = SDL_FLIP_NONE;
    }
    if (keystate[SDL_SCANCODE_SPACE] && onGround) {
        velY = JUMP_FORCE;
        onGround = false;
        justJumped = true;
    }
}

void Player::update(const Map& map) {
    float oldX = x;
    float oldY = y;

    if (!onGround) {
        velY += GRAVITY;
    }


    x += velX;
    y += velY;

    if (animations.count(currentAnim)) {
        dstRect.w = animations[currentAnim].frameW * playerScale;
        dstRect.h = animations[currentAnim].frameH * playerScale;
    }
    else {
        dstRect.w = 32 * playerScale;
        dstRect.h = 32 * playerScale;
    }
    dstRect.x = static_cast<int>(x);
    dstRect.y = static_cast<int>(y);


    dstRect.y = static_cast<int>(oldY);
    if (map.isColliding(dstRect.x, dstRect.y, dstRect.w, dstRect.h)) {
        x = oldX;
        velX = 0;
        dstRect.x = static_cast<int>(x);
    }


    dstRect.y = static_cast<int>(y);
    if (map.isColliding(dstRect.x, dstRect.y, dstRect.w, dstRect.h)) {
        if (velY > 0) {
            int tilePixelH = TILE_HEIGHT * TILE_SCALE;
            int collidedTileRow = static_cast<int>((y + dstRect.h - 1) / tilePixelH);
            y = static_cast<float>(collidedTileRow * tilePixelH - dstRect.h);
            velY = 0;
            onGround = true;
        }
        else if (velY < 0) { // Collided while jumping up
            int tilePixelH = TILE_HEIGHT * TILE_SCALE;
            int collidedTileRow = static_cast<int>(y / tilePixelH);
            y = static_cast<float>((collidedTileRow + 1) * tilePixelH);
            velY = 0;
        }
        dstRect.y = static_cast<int>(y); // Update rect Y
    }
    else {
        onGround = false; // No Y collision means in the air
    }

    // Screen Boundaries
    if (x < 0) { x = 0; velX = 0; }
    if (x + dstRect.w > WINDOW_WIDTH) { x = static_cast<float>(WINDOW_WIDTH - dstRect.w); velX = 0; }
    if (y < 0) { y = 0; velY = 0; }
    if (y > WINDOW_HEIGHT) {
        x = 100.0f; y = 500.0f; velX = 0.0f; velY = 0.0f; onGround = false; currentAnim = "fall";
    }

    // Animation State
    std::string previousAnim = currentAnim;
    if (onGround) {
        currentAnim = isMovingHorizontally ? "run" : "idle";
    }
    else {
        if (justJumped) {
            currentAnim = "jump";
        }
        else if (velY > GRAVITY * 1.1f) {
            currentAnim = "fall";
        }
        else if (velY < -GRAVITY * 1.1f && currentAnim != "jump") {
            currentAnim = "jump";
        }
    }
    justJumped = false;

    if (currentAnim != previousAnim) {
        frame = 0;
        frameCount = 0;
        if (animations.count(currentAnim)) {
            Animation& newAnim = animations[currentAnim];
            dstRect.w = newAnim.frameW * playerScale;
            dstRect.h = newAnim.frameH * playerScale;
        }
    }

    // Animation Frame Update
    if (animations.count(currentAnim)) {
        Animation& anim = animations[currentAnim];
        if (anim.frames > 1) {
            frameCount++;
            if (frameCount >= frameDelay) {
                frame = (frame + 1) % anim.frames;
                frameCount = 0;
            }
        }
        else {
            frame = 0;
        }
        srcRect.x = frame * anim.frameW;
        srcRect.y = 0;
        srcRect.w = anim.frameW;
        srcRect.h = anim.frameH;
    }
    else {
        printf("Warning: Animation '%s' not found!\n", currentAnim.c_str());
        srcRect = { 0, 0, 32, 32 };
    }

    dstRect.x = static_cast<int>(x);
    dstRect.y = static_cast<int>(y);
}

void Player::render(SDL_Renderer* renderer) {
    if (!renderer) return;

    if (animations.count(currentAnim)) {
        Animation& anim = animations[currentAnim];
        if (anim.texture) {
            SDL_RenderCopyEx(renderer, anim.texture, &srcRect, &dstRect, 0, nullptr, flip);
        }
    }

}
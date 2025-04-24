#include "Game.h"
#include "TextureManager.h"
#include "Constants.h"
#include <cstdio>

Game::Game() : isRunning(false), window(nullptr), renderer(nullptr), backgroundTex(nullptr) {}

Game::~Game() {
    clean();
}

void Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return;
    }
    printf("SDL initialized.\n");

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        return;
    }
    printf("SDL_image initialized.\n");

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        IMG_Quit(); SDL_Quit(); return;
    }
    printf("Window created.\n");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window); IMG_Quit(); SDL_Quit(); return;
    }
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // Default clear color
    printf("Renderer created.\n");

    printf("Loading game resources...\n");
    backgroundTex = TextureManager::loadTexture("assets/nature.png", renderer); // Ensure this exists
    map.init("assets/terrain16x16.png", renderer); // Ensure this exists
    player.init(renderer);
    printf("Game resources loaded.\n");

    isRunning = true;
    printf("Game initialized successfully. Starting game loop...\n");
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        // Other event handling if needed
    }
}

void Game::update() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    player.handleInput(keystate);
    player.update(map);
    // Update other game objects here
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // Light blue background
    SDL_RenderClear(renderer);

    if (backgroundTex) {
        SDL_Rect bgDest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, backgroundTex, nullptr, &bgDest);
    }

    map.render(renderer);
    player.render(renderer);
    // Render other game objects here

    SDL_RenderPresent(renderer);
}

void Game::run() {
    if (!isRunning) { // Don't run if init failed
        printf("Game initialization failed. Cannot run game loop.\n");
        return;
    }
    while (running()) {
        frameStart = SDL_GetTicks();

        handleEvents();
        update();
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    printf("Exiting game loop.\n");
}

void Game::clean() {
    printf("Cleaning up game...\n");
    TextureManager::cleanUp(); // Cleans all cached textures

    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; printf("Renderer destroyed.\n"); }
    if (window) { SDL_DestroyWindow(window); window = nullptr; printf("Window destroyed.\n"); }

    IMG_Quit();
    SDL_Quit();
    printf("SDL subsystems quit.\n");
    printf("Cleanup complete.\n");
}

bool Game::running() const {
    return isRunning;
}
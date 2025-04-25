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
    // load các layer pattern
    const char* paths[] = {
        "assets/Yellow.png",
        "assets/Blue.png",
        "assets/Green.png",
        "assets/Purple.png",
        "assets/Gray.png"
    };
    const float speeds[] = { 0.2f, 0.4f, 0.6f, 0.8f, 1.0f };
    int n = sizeof(paths) / sizeof(paths[0]);

    bgLayers.reserve(n);
    bgTileW.reserve(n);
    bgTileH.reserve(n);
    bgOffsets.assign(n, 0.0f);
    bgSpeeds.assign(speeds, speeds + n);

    for (int i = 0; i < n; ++i) {
        SDL_Texture* tex = TextureManager::loadTexture(paths[i], renderer);
        bgLayers.push_back(tex);
        int w, h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
        bgTileW.push_back(w);
        bgTileH.push_back(h);
    }

    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    printf("Renderer created.\n");

    printf("Loading game resources...\n");
    backgroundTex = TextureManager::loadTexture("assets/nature.png", renderer); 
    map.init("assets/terrain16x16.png", renderer); 
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
    }
}

void Game::update() {
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    player.handleInput(keystate);
    player.update(map);

        for (size_t i = 0; i < bgLayers.size(); ++i) {
            bgOffsets[i] += bgSpeeds[i];
            if (bgOffsets[i] >= bgTileW[i]) {
                bgOffsets[i] -= bgTileW[i];
            }
        }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); 
    SDL_RenderClear(renderer);

    if (backgroundTex) {
        SDL_Rect bgDest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
        SDL_RenderCopy(renderer, backgroundTex, nullptr, &bgDest);
    }
    // render layer background 
    for (size_t i = 0; i < bgLayers.size(); ++i) {
        SDL_Texture* tex = bgLayers[i];
        int tw = bgTileW[i], th = bgTileH[i];
        int off = static_cast<int>(bgOffsets[i]);
 
        for (int y = 0; y < WINDOW_HEIGHT; y += th) {
            for (int x = -tw + off; x < WINDOW_WIDTH; x += tw) {
                SDL_Rect dst = { x, y, tw, th };
                SDL_RenderCopy(renderer, tex, nullptr, &dst);
            }
        }
    }

    map.render(renderer);
    player.render(renderer);

    SDL_RenderPresent(renderer);
}

void Game::run() {
    if (!isRunning) {
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
#include "Game.h"
#include "TextureManager.h"
#include "Constants.h"
#include <cstdio>
#include <sstream>

Game::Game() :
    isRunning(false),
    window(nullptr),
    renderer(nullptr),
    backgroundTex(nullptr),
    backgroundScrollX(0.0f),
    frameStart(0),
    frameTime(0),
    score(0),              // Khởi tạo điểm
    highScore(0),          // Khởi tạo high score
    font(nullptr),
    scoreTexture(nullptr),
    highScoreTexture(nullptr)
{
    scoreRect = { 10, 10, 0, 0 };      // Góc trên bên trái
    highScoreRect = { 10, 40, 0, 0 };  // Dưới score một chút
}

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

    if (TTF_Init() == -1) { 
        printf("SDL_ttf could not initialize! TTF Error: %s\n", TTF_GetError());
        IMG_Quit(); SDL_Quit(); return;
    }
    printf("SDL_ttf initialized.\n");

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
    apple.init(renderer, map);

    font = TTF_OpenFont("assets/font.ttf", 24); 
    if (!font) {
        printf("Failed to load font! TTF Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); TTF_Quit(); IMG_Quit(); SDL_Quit(); return;
    }
    updateScoreDisplay(); // Tạo texture ban đầu cho điểm

    printf("Game resources loaded.\n");

    isRunning = true;
    printf("Game initialized successfully. Starting game loop...\n");
}

void Game::incrementScore() {
    score += 1; // Tăng 10 điểm mỗi táo
    if (score > highScore) {
        highScore = score;
    }
    updateScoreDisplay(); 
}

void Game::updateScoreDisplay() {
    if (!font || !renderer) return;

    // Hủy texture cũ
    if (scoreTexture) SDL_DestroyTexture(scoreTexture);
    if (highScoreTexture) SDL_DestroyTexture(highScoreTexture);

    // Tạo chuỗi điểm
    std::stringstream ss;
    ss << "Score: " << score;
    std::string scoreStr = ss.str();
    ss.str(""); // Xóa buffer
    ss << "High Score: " << highScore;
    std::string highScoreStr = ss.str();

    // Tạo surface và texture cho điểm
    SDL_Color textColor = { 255, 255, 255, 255 }; // text màu trắng
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreStr.c_str(), textColor);
    SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreStr.c_str(), textColor);

    if (scoreSurface) {
        scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        scoreRect.w = scoreSurface->w;
        scoreRect.h = scoreSurface->h;
        SDL_FreeSurface(scoreSurface);
    }
    if (highScoreSurface) {
        highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
        highScoreRect.w = highScoreSurface->w;
        highScoreRect.h = highScoreSurface->h;
        SDL_FreeSurface(highScoreSurface);
    }
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
    apple.update(map);

    SDL_Rect playerRect;
    playerRect.x = static_cast<int>(player.getX());
    playerRect.y = static_cast<int>(player.getY());
    playerRect.w = player.getDstRect().w;
    playerRect.h = player.getDstRect().h;
    if (apple.isCollected(playerRect)) {
        incrementScore();
        apple.respawn(map); // Tái tạo táo ở vị trí mới
    }

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
    apple.render(renderer);

    // vẽ điểm và high score
    if (scoreTexture) {
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    }
    if (highScoreTexture) {
        SDL_RenderCopy(renderer, highScoreTexture, nullptr, &highScoreRect);
    }

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

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    printf("SDL subsystems quit.\n");
    printf("Cleanup complete.\n");
}

bool Game::running() const {
    return isRunning;
}
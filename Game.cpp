#include "Game.h"
#include "TextureManager.h"
#include "Constants.h"
#include <cstdio>
#include <sstream>
#include <fstream>
#include <string>

// thêm các header cần thiết để lấy đường dẫn file thực thi
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

Game::Game() :
    isRunning(false),
    window(nullptr),
    renderer(nullptr),
    backgroundTex(nullptr),
    backgroundScrollX(0.0f),
    frameStart(0),
    frameTime(0),
    score(0),
    highScore(0),
    font(nullptr),
    scoreTexture(nullptr),
    highScoreTexture(nullptr),
    timerTexture(nullptr),
    gameOverTexture(nullptr),
    gameOver(false),
    backgroundMusic(nullptr)
{
    scoreRect = { 10, 10, 0, 0 };      // góc trên bên trái
    highScoreRect = { 10, 40, 0, 0 };  // dưới score một chút
    timerRect = { 10, 70, 0 , 0 }; // dưới high score
    gameOverRect = { WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, 0, 0 }; // điều chỉnh vị trí "You Lost" lên trên một chút
    restartRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 20, 100, 40 }; // dưới "You Lost"
}

// hàm lấy đường dẫn thư mục chứa file thực thi
std::string getExecutableDirectory() {
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string pathStr(path);
    size_t lastSlash = pathStr.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        pathStr = pathStr.substr(0, lastSlash);
    }
    return pathStr + "\\";
#else
    char path[1024];
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (count != -1) {
        path[count] = '\0';
        std::string pathStr(path);
        size_t lastSlash = pathStr.find_last_of("/");
        if (lastSlash != std::string::npos) {
            pathStr = pathStr.substr(0, lastSlash);
        }
        return pathStr + "/";
    }
    return "./";
#endif
}

void Game::loadHighScore() {
    std::string highScorePath = getExecutableDirectory() + "highscore.txt";
    std::ifstream inFile(highScorePath);
    if (inFile.is_open()) {
        inFile >> highScore;
        inFile.close();
        printf("Loaded high score: %d from %s\n", highScore, highScorePath.c_str());
    }
    else {
        highScore = 0;
        printf("No high score file found at %s, starting with 0\n", highScorePath.c_str());
    }
}

void Game::saveHighScore() {
    std::string highScorePath = getExecutableDirectory() + "highscore.txt";
    std::ofstream outFile(highScorePath);
    if (outFile.is_open()) {
        outFile << highScore;
        outFile.close();
        printf("Saved high score: %d to %s\n", highScore, highScorePath.c_str());
    }
    else {
        printf("Failed to save high score to %s\n", highScorePath.c_str());
    }
}

Game::~Game() {
    clean();
}

void Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        SDL_Delay(5000);
        return;
    }
    printf("SDL initialized.\n");

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        SDL_Delay(5000);
        return;
    }
    printf("SDL_image initialized.\n");

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF Error: %s\n", TTF_GetError());
        IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }
    printf("SDL_ttf initialized.\n");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }
    printf("SDL_mixer initialized.\n");

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }
    printf("Window created.\n");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window); TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }

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
        if (!tex) {
            printf("Failed to load background layer %d: %s\n", i, paths[i]);
            SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); TTF_Quit(); IMG_Quit(); SDL_Quit();
            SDL_Delay(5000);
            return;
        }
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
    if (!backgroundTex) {
        printf("Failed to load background texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }
    map.init("assets/terrain16x16.png", renderer);
    player.init(renderer);
    apple.init(renderer, map);

    font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        printf("Failed to load font! TTF Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }

    gameOverFont = TTF_OpenFont("assets/font.ttf", 60); //kích cỡ "You Lost"
    if (!gameOverFont) {
        printf("Failed to load game over font! TTF Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }

    loadHighScore();
    updateScoreDisplay();

    SDL_Color textColor = { 255, 0, 0, 255 };
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(gameOverFont, "You Lost", textColor);
    if (gameOverSurface) {
        gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        gameOverRect.w = gameOverSurface->w;
        gameOverRect.h = gameOverSurface->h;
        gameOverRect.x = WINDOW_WIDTH / 2 - gameOverRect.w / 2;
        gameOverRect.y = WINDOW_HEIGHT / 2 - gameOverRect.h / 2 - 30;
        SDL_FreeSurface(gameOverSurface);
    }
    else {
        printf("Failed to render game over text! TTF Error: %s\n", TTF_GetError());
    }

    // tạo texture cho nút Restart
    textColor = { 0, 255, 0, 255 }; // màu xanh lá
    SDL_Surface* restartSurface = TTF_RenderText_Solid(font, "Restart", textColor);
    if (restartSurface) {
        restartTexture = SDL_CreateTextureFromSurface(renderer, restartSurface);
        restartRect.w = restartSurface->w;
        restartRect.h = restartSurface->h;
        restartRect.x = WINDOW_WIDTH / 2 - restartRect.w / 2;
        restartRect.y = gameOverRect.y + gameOverRect.h + 20; // dưới "You Lost" 20 pixel
        SDL_FreeSurface(restartSurface);
        printf("Restart button created successfully.\n");
    }
    else {
        printf("Failed to render restart text! TTF Error: %s\n", TTF_GetError());
    }

    // Tải và phát nhạc nền
    std::string musicPath = getExecutableDirectory() + "assets/music/backhome.mp3";
    backgroundMusic = Mix_LoadMUS(musicPath.c_str());
    if (!backgroundMusic) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    else {
        // Phát nhạc nền, lặp vô hạn (-1)
        if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
            printf("Failed to play background music! SDL_mixer Error: %s\n", Mix_GetError());
        }
        else {
            printf("Background music playing.\n");
        }
    }

    printf("Game resources loaded.\n");

    isRunning = true;
    printf("Game initialized successfully. Starting game loop...\n");
}


void Game::incrementScore() {
    score += 1; // tăng 1 điểm mỗi táo
    if (score > highScore) {
        highScore = score;
    }
    updateScoreDisplay();
}

void Game::updateScoreDisplay() {
    if (!font || !renderer) return;

    // hủy texture cũ
    if (scoreTexture) SDL_DestroyTexture(scoreTexture);
    if (highScoreTexture) SDL_DestroyTexture(highScoreTexture);

    // tạo chuỗi điểm
    std::stringstream ss;
    ss << "Score: " << score;
    std::string scoreStr = ss.str();
    ss.str(""); // xóa buffer
    ss << "High Score: " << highScore;
    std::string highScoreStr = ss.str();

    // tạo surface và texture cho điểm
    SDL_Color textColor = { 255, 255, 255, 255 }; // text màu trắng
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreStr.c_str(), textColor);
    SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreStr.c_str(), textColor);

    if (scoreSurface) {
        scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        scoreRect.w = scoreSurface->w;
        scoreRect.h = scoreSurface->h;
        SDL_FreeSurface(scoreSurface);
    }
    else {
        printf("Failed to render score text! TTF Error: %s\n", TTF_GetError());
    }
    if (highScoreSurface) {
        highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);
        highScoreRect.w = highScoreSurface->w;
        highScoreRect.h = highScoreSurface->h;
        SDL_FreeSurface(highScoreSurface);
    }
    else {
        printf("Failed to render high score text! TTF Error: %s\n", TTF_GetError());
    }
}

void Game::updateTimerDisplay(Uint32 remainingTime) {
    if (!font || !renderer) return;

    if (timerTexture) SDL_DestroyTexture(timerTexture);

    std::stringstream ss;
    ss << "Time Left: " << (remainingTime / 1000.0f) << "s";
    std::string timerStr = ss.str();

    SDL_Color textColor = { 255, 255, 0, 255 }; // màu vàng
    SDL_Surface* timerSurface = TTF_RenderText_Solid(font, timerStr.c_str(), textColor);
    if (timerSurface) {
        timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
        timerRect.w = timerSurface->w;
        timerRect.h = timerSurface->h;
        SDL_FreeSurface(timerSurface);
    }
    else {
        printf("Failed to render timer text! TTF Error: %s\n", TTF_GetError());
    }
}

void Game::reset() {
    score = 0;
    gameOver = false;
    apple.respawn(map); // táo xuất hiện lại
    updateScoreDisplay();
}


void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        if (gameOver && event.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if (x >= restartRect.x && x <= restartRect.x + restartRect.w &&
                y >= restartRect.y && y <= restartRect.y + restartRect.h) {
                printf("Restart button clicked!\n");
                reset(); // reset game khi nhấn nút Restart
            }
        }
    }
}

void Game::update() {
    if (gameOver) return; // dừng cập nhật nếu game over

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
        apple.respawn(map); // tái tạo táo ở vị trí mới
    }

    // kiểm tra thời gian táo
    Uint32 currentTime = SDL_GetTicks();
    Uint32 spawnTime = apple.getSpawnTime();
    Uint32 timeElapsed = currentTime - spawnTime;
    if (timeElapsed >= appleTimeout) {
        gameOver = true;
    }
    else {
        Uint32 remainingTime = appleTimeout - timeElapsed;
        updateTimerDisplay(remainingTime); // cập nhật thời gian đếm ngược
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
    if (timerTexture && !gameOver) {
        SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);
    }
    if (gameOver && gameOverTexture) {
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
        if (restartTexture) {
            SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);
        }
        else {
            printf("restartTexture is null, cannot render Restart button!\n");
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::run() {
    if (!isRunning) {
        printf("Game initialization failed. Cannot run game loop.\n");
        SDL_Delay(5000);
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
    saveHighScore();

    TextureManager::cleanUp();

    if (scoreTexture) SDL_DestroyTexture(scoreTexture);
    if (highScoreTexture) SDL_DestroyTexture(highScoreTexture);
    if (timerTexture) SDL_DestroyTexture(timerTexture);
    if (gameOverTexture) SDL_DestroyTexture(gameOverTexture);
    if (restartTexture) SDL_DestroyTexture(restartTexture);
    if (font) TTF_CloseFont(font);
    if (gameOverFont) TTF_CloseFont(gameOverFont);


    // Giải phóng nhạc nền
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
        printf("Background music freed.\n");
    }

    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; printf("Renderer destroyed.\n"); }
    if (window) { SDL_DestroyWindow(window); window = nullptr; printf("Window destroyed.\n"); }

    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    printf("SDL subsystems quit.\n");
    printf("Cleanup complete.\n");
    SDL_Delay(5000);
}

bool Game::running() const {
    return isRunning;
}
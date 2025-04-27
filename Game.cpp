#include "Game.h"
#include "TextureManager.h"
#include "Constants.h"
#include <cstdio>
#include <sstream>
#include <fstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

Game::Game() :
    state(GameState::MENU),
    isRunning(false),
    window(nullptr),
    renderer(nullptr),
    frameStart(0),
    frameTime(0),
    score(0),
    highScore(0),
    font(nullptr),
    menuFont(nullptr),
    gameOverFont(nullptr),
    scoreTexture(nullptr),
    highScoreTexture(nullptr),
    timerTexture(nullptr),
    gameOverTexture(nullptr),
    restartTexture(nullptr),
    playTexture(nullptr),
    settingsTexture(nullptr),
    backTexture(nullptr),
    volumeUpTexture(nullptr),
    volumeDownTexture(nullptr),
    volumeDisplayTexture(nullptr),
    pauseTexture(nullptr),
    resumeTexture(nullptr),
    backgroundMusic(nullptr),
    musicVolume(64)
{
    scoreRect = { 10, 10, 0, 0 };
    highScoreRect = { 10, 40, 0, 0 };
    timerRect = { 10, 70, 0, 0 };
    gameOverRect = { WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, 0, 0 };
    restartRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 20, 100, 40 };
    playRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 40, 100, 40 };
    settingsRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 20, 100, 40 };
    backRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 80, 100, 40 };
    volumeUpRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 40, 100, 40 };
    volumeDownRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 20, 100, 40 };
    volumeDisplayRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 100, 100, 40 };
    pauseRect = { WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 50, 0, 0 };
    resumeRect = { WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 20, 100, 40 };
}

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
        printf("Loaded high score: %d\n", highScore);
    }
    else {
        highScore = 0;
        printf("No high score file found, starting with 0\n");
    }
}

void Game::saveHighScore() {
    std::string highScorePath = getExecutableDirectory() + "highscore.txt";
    std::ofstream outFile(highScorePath);
    if (outFile.is_open()) {
        outFile << highScore;
        outFile.close();
        printf("Saved high score: %d\n", highScore);
    }
    else {
        printf("Failed to save high score\n");
    }
}

Game::~Game() {
    clean();
}

void Game::updateVolumeDisplay() {
    if (!font || !renderer) return;

    if (volumeDisplayTexture) SDL_DestroyTexture(volumeDisplayTexture);

    std::stringstream ss;
    ss << "Volume: " << musicVolume;
    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* volumeSurface = TTF_RenderText_Solid(font, ss.str().c_str(), textColor);
    if (volumeSurface) {
        volumeDisplayTexture = SDL_CreateTextureFromSurface(renderer, volumeSurface);
        volumeDisplayRect.w = volumeSurface->w;
        volumeDisplayRect.h = volumeSurface->h;
        volumeDisplayRect.x = WINDOW_WIDTH / 2 - volumeDisplayRect.w / 2;
        SDL_FreeSurface(volumeSurface);
    }
}

void Game::pauseMusic() {
    if (Mix_PlayingMusic()) {
        Mix_PauseMusic();
        printf("Music paused.\n");
    }
}

void Game::resumeMusic() {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        printf("Music resumed.\n");
    }
}

void Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        SDL_Delay(5000);
        return;
    }
    printf("SDL initialized.\n");

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
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
        Mix_CloseAudio(); TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }
    printf("Window created.\n");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window); Mix_CloseAudio(); TTF_Quit(); IMG_Quit(); SDL_Quit();
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
            SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); Mix_CloseAudio(); TTF_Quit(); IMG_Quit(); SDL_Quit();
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
    map.init("assets/terrain16x16.png", renderer);
    player.init(renderer);
    apple.init(renderer, map);

    font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        printf("Failed to load font! TTF Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); Mix_CloseAudio(); TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }

    menuFont = TTF_OpenFont("assets/font.ttf", 48);
    if (!menuFont) {
        printf("Failed to load menu font! TTF Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); Mix_CloseAudio(); TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }

    gameOverFont = TTF_OpenFont("assets/font.ttf", 60);
    if (!gameOverFont) {
        printf("Failed to load game over font! TTF Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); Mix_CloseAudio(); TTF_Quit(); IMG_Quit(); SDL_Quit();
        SDL_Delay(5000);
        return;
    }

    loadHighScore();
    updateScoreDisplay();

    // Initialize menu elements
    SDL_Color textColor = { 0, 255, 0, 255 };
    SDL_Surface* playSurface = TTF_RenderText_Solid(menuFont, "Play", textColor);
    if (playSurface) {
        playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
        playRect.w = playSurface->w;
        playRect.h = playSurface->h;
        playRect.x = WINDOW_WIDTH / 2 - playRect.w / 2;
        playRect.y = WINDOW_HEIGHT / 2 - playRect.h / 2 - 30;
        SDL_FreeSurface(playSurface);
        printf("Play button created.\n");
    }

    SDL_Surface* settingsSurface = TTF_RenderText_Solid(menuFont, "Settings", textColor);
    if (settingsSurface) {
        settingsTexture = SDL_CreateTextureFromSurface(renderer, settingsSurface);
        settingsRect.w = settingsSurface->w;
        settingsRect.h = settingsSurface->h;
        settingsRect.x = WINDOW_WIDTH / 2 - settingsRect.w / 2;
        settingsRect.y = playRect.y + playRect.h + 20;
        SDL_FreeSurface(settingsSurface);
        printf("Settings button created.\n");
    }

    // Initialize settings screen elements
    SDL_Surface* backSurface = TTF_RenderText_Solid(menuFont, "Back", textColor);
    if (backSurface) {
        backTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
        backRect.w = backSurface->w;
        backRect.h = backSurface->h;
        backRect.x = WINDOW_WIDTH / 2 - backRect.w / 2;
        backRect.y = WINDOW_HEIGHT / 2 + 80;
        SDL_FreeSurface(backSurface);
        printf("Back button created.\n");
    }

    SDL_Surface* volumeUpSurface = TTF_RenderText_Solid(menuFont, "Press to raise volume", textColor);
    if (volumeUpSurface) {
        volumeUpTexture = SDL_CreateTextureFromSurface(renderer, volumeUpSurface);
        volumeUpRect.w = volumeUpSurface->w;
        volumeUpRect.h = volumeUpSurface->h;
        volumeUpRect.x = WINDOW_WIDTH / 2 - volumeUpRect.w / 2;
        volumeUpRect.y = WINDOW_HEIGHT / 2 - volumeUpRect.h / 2 - 30;
        SDL_FreeSurface(volumeUpSurface);
        printf("Volume Up button created.\n");
    }

    SDL_Surface* volumeDownSurface = TTF_RenderText_Solid(menuFont, "Press to lower volume", textColor);
    if (volumeDownSurface) {
        volumeDownTexture = SDL_CreateTextureFromSurface(renderer, volumeDownSurface);
        volumeDownRect.w = volumeDownSurface->w;
        volumeDownRect.h = volumeDownSurface->h;
        volumeDownRect.x = WINDOW_WIDTH / 2 - volumeDownRect.w / 2;
        volumeDownRect.y = volumeUpRect.y + volumeUpRect.h + 20;
        SDL_FreeSurface(volumeDownSurface);
        printf("Volume Down button created.\n");
    }

    updateVolumeDisplay(); // Initialize volume display

    // Initialize game over elements
    textColor = { 255, 0, 0, 255 };
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(gameOverFont, "You Lost", textColor);
    if (gameOverSurface) {
        gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        gameOverRect.w = gameOverSurface->w;
        gameOverRect.h = gameOverSurface->h;
        gameOverRect.x = WINDOW_WIDTH / 2 - gameOverRect.w / 2;
        gameOverRect.y = WINDOW_HEIGHT / 2 - gameOverRect.h / 2 - 30;
        SDL_FreeSurface(gameOverSurface);
    }

    textColor = { 0, 255, 0, 255 };
    SDL_Surface* restartSurface = TTF_RenderText_Solid(font, "Restart", textColor);
    if (restartSurface) {
        restartTexture = SDL_CreateTextureFromSurface(renderer, restartSurface);
        restartRect.w = restartSurface->w;
        restartRect.h = restartSurface->h;
        restartRect.x = WINDOW_WIDTH / 2 - restartRect.w / 2;
        restartRect.y = gameOverRect.y + gameOverRect.h + 20;
        SDL_FreeSurface(restartSurface);
        printf("Restart button created.\n");
    }

    // Initialize pause elements
    textColor = { 255, 255, 255, 255 };
    SDL_Surface* pauseSurface = TTF_RenderText_Solid(gameOverFont, "Paused", textColor);
    if (pauseSurface) {
        pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
        pauseRect.w = pauseSurface->w;
        pauseRect.h = pauseSurface->h;
        pauseRect.x = WINDOW_WIDTH / 2 - pauseRect.w / 2;
        pauseRect.y = WINDOW_HEIGHT / 2 - pauseRect.h / 2 - 30;
        SDL_FreeSurface(pauseSurface);
        printf("Pause text created.\n");
    }

    textColor = { 0, 255, 0, 255 };
    SDL_Surface* resumeSurface = TTF_RenderText_Solid(font, "Resume", textColor);
    if (resumeSurface) {
        resumeTexture = SDL_CreateTextureFromSurface(renderer, resumeSurface);
        resumeRect.w = resumeSurface->w;
        resumeRect.h = resumeSurface->h;
        resumeRect.x = WINDOW_WIDTH / 2 - resumeRect.w / 2;
        resumeRect.y = pauseRect.y + pauseRect.h + 20;
        SDL_FreeSurface(resumeSurface);
        printf("Resume button created.\n");
    }

    backgroundMusic = Mix_LoadMUS("assets/music/time_for_adventure.mp3");
    if (!backgroundMusic) {
        printf("Failed to load background music! SDL_mixer Error: %s\n", Mix_GetError());
    }
    else {
        Mix_VolumeMusic(musicVolume);
        if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
            printf("Failed to play background music! SDL_mixer Error: %s\n", Mix_GetError());
        }
        else {
            printf("Background music playing.\n");
        }
    }

    printf("Game resources loaded.\n");
    isRunning = true;
    printf("Game initialized successfully.\n");
}

void Game::incrementScore() {
    score += 1;
    if (score > highScore) {
        highScore = score;
    }
    updateScoreDisplay();
}

void Game::updateScoreDisplay() {
    if (!font || !renderer) return;

    if (scoreTexture) SDL_DestroyTexture(scoreTexture);
    if (highScoreTexture) SDL_DestroyTexture(highScoreTexture);

    std::stringstream ss;
    ss << "Score: " << score;
    std::string scoreStr = ss.str();
    ss.str("");
    ss << "High Score: " << highScore;
    std::string highScoreStr = ss.str();

    SDL_Color textColor = { 255, 255, 255, 255 };
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

void Game::updateTimerDisplay(Uint32 remainingTime) {
    if (!font || !renderer) return;

    if (timerTexture) SDL_DestroyTexture(timerTexture);

    std::stringstream ss;
    ss << "Time Left: " << (remainingTime / 1000.0f) << "s";
    SDL_Color textColor = { 255, 255, 0, 255 };
    SDL_Surface* timerSurface = TTF_RenderText_Solid(font, ss.str().c_str(), textColor);
    if (timerSurface) {
        timerTexture = SDL_CreateTextureFromSurface(renderer, timerSurface);
        timerRect.w = timerSurface->w;
        timerRect.h = timerSurface->h;
        SDL_FreeSurface(timerSurface);
    }
}

void Game::reset() {
    score = 0;
    state = GameState::PLAYING;
    apple.respawn(map);
    updateScoreDisplay();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
            return;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (state == GameState::MENU) {
                if (x >= playRect.x && x <= playRect.x + playRect.w &&
                    y >= playRect.y && y <= playRect.y + playRect.h) {
                    state = GameState::PLAYING;
                    reset();
                    printf("Play button clicked!\n");
                }
                else if (x >= settingsRect.x && x <= settingsRect.x + settingsRect.w &&
                    y >= settingsRect.y && y <= settingsRect.y + settingsRect.h) {
                    state = GameState::SETTINGS;
                    printf("Settings button clicked!\n");
                }
            }
            else if (state == GameState::SETTINGS) {
                if (x >= backRect.x && x <= backRect.x + backRect.w &&
                    y >= backRect.y && y <= backRect.y + backRect.h) {
                    state = GameState::MENU;
                    printf("Back button clicked!\n");
                }
                else if (x >= volumeUpRect.x && x <= volumeUpRect.x + volumeUpRect.w &&
                    y >= volumeUpRect.y && y <= volumeUpRect.y + volumeUpRect.h) {
                    musicVolume = (musicVolume + 16 <= 128) ? musicVolume + 16 : 128;
                    Mix_VolumeMusic(musicVolume);
                    updateVolumeDisplay();
                    printf("Volume increased to %d\n", musicVolume);
                }
                else if (x >= volumeDownRect.x && x <= volumeDownRect.x + volumeDownRect.w &&
                    y >= volumeDownRect.y && y <= volumeDownRect.y + volumeDownRect.h) {
                    musicVolume = (musicVolume - 16 >= 0) ? musicVolume - 16 : 0;
                    Mix_VolumeMusic(musicVolume);
                    updateVolumeDisplay();
                    printf("Volume decreased to %d\n", musicVolume);
                }
            }
            else if (state == GameState::GAME_OVER &&
                x >= restartRect.x && x <= restartRect.x + restartRect.w &&
                y >= restartRect.y && y <= restartRect.y + restartRect.h) {
                printf("Restart button clicked!\n");
                reset();
            }
            else if (state == GameState::PAUSED &&
                x >= resumeRect.x && x <= resumeRect.x + resumeRect.w &&
                y >= resumeRect.y && y <= resumeRect.y + resumeRect.h) {
                state = GameState::PLAYING;
                resumeMusic();
                printf("Resume button clicked!\n");
            }
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            if (state == GameState::PLAYING) {
                state = GameState::PAUSED;
                pauseMusic();
                printf("Game paused.\n");
            }
            else if (state == GameState::PAUSED) {
                state = GameState::PLAYING;
                resumeMusic();
                printf("Game resumed.\n");
            }
        }
    }
}

void Game::update() {
    if (state != GameState::PLAYING) return;

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
        apple.respawn(map);
    }

    Uint32 currentTime = SDL_GetTicks();
    Uint32 spawnTime = apple.getSpawnTime();
    Uint32 timeElapsed = currentTime - spawnTime;
    if (timeElapsed >= appleTimeout) {
        state = GameState::GAME_OVER;
    }
    else {
        updateTimerDisplay(appleTimeout - timeElapsed);
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

    if (state == GameState::MENU) {
        if (playTexture) SDL_RenderCopy(renderer, playTexture, nullptr, &playRect);
        if (settingsTexture) SDL_RenderCopy(renderer, settingsTexture, nullptr, &settingsRect);
    }
    else if (state == GameState::SETTINGS) {
        if (volumeDisplayTexture) SDL_RenderCopy(renderer, volumeDisplayTexture, nullptr, &volumeDisplayRect);
        if (volumeUpTexture) SDL_RenderCopy(renderer, volumeUpTexture, nullptr, &volumeUpRect);
        if (volumeDownTexture) SDL_RenderCopy(renderer, volumeDownTexture, nullptr, &volumeDownRect);
        if (backTexture) SDL_RenderCopy(renderer, backTexture, nullptr, &backRect);
    }
    else {
        map.render(renderer);
        player.render(renderer);
        apple.render(renderer);

        if (scoreTexture) SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
        if (highScoreTexture) SDL_RenderCopy(renderer, highScoreTexture, nullptr, &highScoreRect);
        if (timerTexture && state == GameState::PLAYING) SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);

        if (state == GameState::GAME_OVER && gameOverTexture) {
            SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
            if (restartTexture) SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);
        }
        else if (state == GameState::PAUSED) {
            if (pauseTexture) SDL_RenderCopy(renderer, pauseTexture, nullptr, &pauseRect);
            if (resumeTexture) SDL_RenderCopy(renderer, resumeTexture, nullptr, &resumeRect);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::run() {
    if (!isRunning) {
        printf("Game initialization failed.\n");
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
    if (playTexture) SDL_DestroyTexture(playTexture);
    if (settingsTexture) SDL_DestroyTexture(settingsTexture);
    if (backTexture) SDL_DestroyTexture(backTexture);
    if (volumeUpTexture) SDL_DestroyTexture(volumeUpTexture);
    if (volumeDownTexture) SDL_DestroyTexture(volumeDownTexture);
    if (volumeDisplayTexture) SDL_DestroyTexture(volumeDisplayTexture);
    if (pauseTexture) SDL_DestroyTexture(pauseTexture);
    if (resumeTexture) SDL_DestroyTexture(resumeTexture);
    if (font) TTF_CloseFont(font);
    if (menuFont) TTF_CloseFont(menuFont);
    if (gameOverFont) TTF_CloseFont(gameOverFont);
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        printf("Background music freed.\n");
    }

    Mix_CloseAudio();
    printf("SDL_mixer closed.\n");
    if (renderer) { SDL_DestroyRenderer(renderer); printf("Renderer destroyed.\n"); }
    if (window) { SDL_DestroyWindow(window); printf("Window destroyed.\n"); }

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
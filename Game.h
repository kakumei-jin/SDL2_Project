#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "Map.h"
#include "Player.h"
#include <vector>
#include "apple.h"

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int width, int height);
    void run();
    bool running() const;
    void incrementScore();

private:
    // Game states
    enum class GameState {
        MENU,
        SETTINGS,
        PLAYING,
        PAUSED,
        GAME_OVER
    };
    GameState state;

    // Background scrolling
    std::vector<SDL_Texture*> bgLayers;
    std::vector<int> bgTileW, bgTileH;
    std::vector<float> bgOffsets;
    std::vector<float> bgSpeeds;

    void handleEvents();
    void update();
    void render();
    void clean();
    void reset();
    void updateVolumeDisplay(); // new method to update volume display
    void pauseMusic(); // new method to pause music
    void resumeMusic(); // new method to resume music

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    const Uint32 appleTimeout = 8000;

    Map map;
    Player player;
    Apple apple;

    Uint32 frameStart;
    int frameTime;
    const int frameDelay = 1000 / 60;

    //score and high score
    int score;
    int highScore;
    TTF_Font* font;
    TTF_Font* menuFont;
    TTF_Font* gameOverFont;
    SDL_Texture* scoreTexture;
    SDL_Texture* highScoreTexture;
    SDL_Texture* timerTexture;
    SDL_Texture* gameOverTexture;
    SDL_Texture* restartTexture;
    SDL_Rect scoreRect;
    SDL_Rect highScoreRect;
    SDL_Rect timerRect;
    SDL_Rect gameOverRect;
    SDL_Rect restartRect;

    //  menu elements
    SDL_Texture* playTexture;
    SDL_Rect playRect;
    SDL_Texture* settingsTexture;
    SDL_Rect settingsRect;

    // settings screen elements
    SDL_Texture* backTexture;
    SDL_Rect backRect;
    SDL_Texture* volumeUpTexture;
    SDL_Rect volumeUpRect;
    SDL_Texture* volumeDownTexture;
    SDL_Rect volumeDownRect;
    SDL_Texture* volumeDisplayTexture; // new texture for volume display
    SDL_Rect volumeDisplayRect; // new rect for volume display
    int musicVolume;

    // pause elements
    SDL_Texture* pauseTexture;
    SDL_Texture* resumeTexture;
    SDL_Rect pauseRect;
    SDL_Rect resumeRect;

    Mix_Music* backgroundMusic;

    void updateScoreDisplay();
    void updateTimerDisplay(Uint32 remainingTime);
    void loadHighScore();
    void saveHighScore();
};
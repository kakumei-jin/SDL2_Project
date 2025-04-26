#include "Game.h"
#include "Constants.h"

int main(int argc, char* argv[]) {
    Game game;
    game.init("SDL2 Game", WINDOW_WIDTH, WINDOW_HEIGHT);
    game.run();

    return 0;
}
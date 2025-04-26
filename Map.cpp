#include "Map.h"
#include "TextureManager.h"
#include <cstdio>

Map::Map() : tileset(nullptr) {
    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int col = 0; col < MAP_COLS; ++col) {
            mapData[row][col] = 0;
        }
    }
}

Map::~Map() {
    // Textures are cleaned up by TextureManager::cleanUp()
}

void Map::init(const char* tilesetPath, SDL_Renderer* renderer) {
    tileset = TextureManager::loadTexture(tilesetPath, renderer);
    if (!tileset) {
        printf("Failed to load tileset texture in Map::init: %s\n", tilesetPath);
        return;
    }


    const int EMPTY_TILE = 0;
    const int GREY_BRICK_1 = 1;
    const int GROUND_TILE_GREEN = (1 * TILESET_COLS) + 0;
    const int DIRT_BLOCK = (1 * TILESET_COLS) + 2;
    const int GOLD_BLOCK = (10 * TILESET_COLS) + 7;

    int initialMap[MAP_ROWS][MAP_COLS] = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, GOLD_BLOCK, GOLD_BLOCK, GOLD_BLOCK, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, GOLD_BLOCK, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, GOLD_BLOCK, 0, GOLD_BLOCK, GOLD_BLOCK,GOLD_BLOCK, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, GOLD_BLOCK, GOLD_BLOCK, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, GOLD_BLOCK, GOLD_BLOCK, GOLD_BLOCK ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, GOLD_BLOCK,GOLD_BLOCK ,  0, 0, 0, GOLD_BLOCK, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN, GROUND_TILE_GREEN }
    };


    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int col = 0; col < MAP_COLS; ++col) {
            mapData[row][col] = initialMap[row][col];
        }
    }
    printf("Map initialized with tileset: %s\n", tilesetPath);
}

SDL_Rect Map::getTileSrcRect(int tileID) const {
    if (tileID < 0) tileID = 0;

    SDL_Rect src;
    src.w = TILE_WIDTH;
    src.h = TILE_HEIGHT;
    src.x = (tileID % TILESET_COLS) * TILE_WIDTH;
    src.y = (tileID / TILESET_COLS) * TILE_HEIGHT;
    return src;
}

void Map::render(SDL_Renderer* renderer) {
    if (!tileset || !renderer) return;

    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int col = 0; col < MAP_COLS; ++col) {
            int tileID = mapData[row][col];
            if (tileID == 0) continue;

            SDL_Rect src = getTileSrcRect(tileID);
            SDL_Rect dst = {
                col * TILE_WIDTH * TILE_SCALE,
                row * TILE_HEIGHT * TILE_SCALE,
                TILE_WIDTH * TILE_SCALE,
                TILE_HEIGHT * TILE_SCALE
            };
            SDL_RenderCopy(renderer, tileset, &src, &dst);
        }
    }
}

bool Map::isColliding(int x, int y, int w, int h) const {
    int tilePixelW = TILE_WIDTH * TILE_SCALE;
    int tilePixelH = TILE_HEIGHT * TILE_SCALE;

    if (tilePixelW <= 0 || tilePixelH <= 0) return false;

    int leftTile = x / tilePixelW;
    int rightTile = (x + w - 1) / tilePixelW;
    int topTile = y / tilePixelH;
    int bottomTile = (y + h - 1) / tilePixelH;

    leftTile = (leftTile < 0) ? 0 : leftTile;
    rightTile = (rightTile >= MAP_COLS) ? MAP_COLS - 1 : rightTile;
    topTile = (topTile < 0) ? 0 : topTile;
    bottomTile = (bottomTile >= MAP_ROWS) ? MAP_ROWS - 1 : bottomTile;

    for (int row = topTile; row <= bottomTile; ++row) {
        for (int col = leftTile; col <= rightTile; ++col) {
            if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
                if (mapData[row][col] != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}
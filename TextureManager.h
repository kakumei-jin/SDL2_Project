#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <map>
#include <cstdio>

class TextureManager {
private:
    static std::map<std::string, SDL_Texture*> textureCache;

public:
    static SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);
    static void Draw(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip = SDL_FLIP_NONE);
    static void cleanUp();
};
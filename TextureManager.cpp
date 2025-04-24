#include "TextureManager.h"

std::map<std::string, SDL_Texture*> TextureManager::textureCache;

SDL_Texture* TextureManager::loadTexture(const std::string& path, SDL_Renderer* renderer) {
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        return it->second;
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (texture == nullptr) {
        printf("Unable to load texture '%s'! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else {
        printf("Loaded texture: %s\n", path.c_str());
        textureCache[path] = texture;
    }
    return texture;
}

void TextureManager::Draw(SDL_Renderer* renderer, SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip) {
    if (!tex || !renderer) return;
    SDL_RenderCopyEx(renderer, tex, &src, &dest, 0, nullptr, flip);
}

void TextureManager::cleanUp() {
    printf("Cleaning up textures...\n");
    for (auto& pair : textureCache) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textureCache.clear();
    printf("Texture cleanup complete.\n");
}
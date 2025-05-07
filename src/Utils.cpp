#include "C:\SDLproject_vscode\include\Utils.h"
#include "C:\SDLproject_vscode\include\SDL_image.h"
#include "C:\SDLproject_vscode\include\SDL_render.h"
#include "C:\SDLproject_vscode\include\SDL_surface.h"
#include "C:\SDLproject_vscode\include\SDL_log.h"


SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        SDL_Log("Failed to load image: %s, %s", path, IMG_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}
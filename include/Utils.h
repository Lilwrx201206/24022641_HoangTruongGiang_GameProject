#ifndef UTILS_H
#define UTILS_H

#include "SDL.h"
#include "SDL_image.h"
#include <string>

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer);
bool checkCollision(SDL_Rect a, SDL_Rect b);

#endif
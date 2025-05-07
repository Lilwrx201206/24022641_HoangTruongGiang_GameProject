#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <vector>
#include <string>
#include "Utils.h"
#include "Constants.h"


struct Obstacle {
    int x, y;
};
void runGame(SDL_Renderer* renderer, TTF_Font* font);
void renderScore(SDL_Renderer* renderer, TTF_Font* font, int score);
void renderLives(SDL_Renderer* renderer, TTF_Font* font, int lives);
void renderPlayer(SDL_Renderer* renderer, SDL_Texture* playerTexture, SDL_Rect& player, int frame);
void renderPlayerJump(SDL_Renderer* renderer, SDL_Texture* jumpTexture, SDL_Rect& player, int frame);
void renderObstacle(SDL_Renderer* renderer, SDL_Texture* obstacleTexture, SDL_Rect& obstacle, int frame);
void renderInfiniteMap(SDL_Renderer* renderer, SDL_Texture* mapTexture, int& mapOffset);

#endif
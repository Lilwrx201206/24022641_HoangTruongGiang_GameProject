#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include "SDL.h"
#include "SDL_ttf.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "Constants.h"
extern std::vector<int> highScores; // Declare highScores as extern

void loadHighScores();
void saveHighScores();
void showHighScores(SDL_Renderer* renderer, TTF_Font* font);

#endif
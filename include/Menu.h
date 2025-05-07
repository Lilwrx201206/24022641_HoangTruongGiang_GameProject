#ifndef MENU_H
#define MENU_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <string>
#include "Utils.h"

enum MenuOption { MENU_PLAY, MENU_HIGHSCORE, MENU_SETTING, MENU_EXIT, MENU_NONE };

MenuOption showMainMenu(SDL_Renderer* renderer, TTF_Font* font);
void showSettings(SDL_Renderer* renderer);

#endif
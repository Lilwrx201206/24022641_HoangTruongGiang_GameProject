#include "C:\SDLproject_vscode\include\Utils.h"
#include "C:\SDLproject_vscode\include\Game.h"
#include "C:\SDLproject_vscode\include\Menu.h"
#include "C:\SDLproject_vscode\include\HighScores.h"
#include "C:\SDLproject_vscode\include\Settings.h"
#include "C:\SDLproject_vscode\include\Constants.h"  // Include the constants header
#include "C:\SDLproject_vscode\include\SDL.h"
#include "C:\SDLproject_vscode\include\SDL_ttf.h"
#include "C:\SDLproject_vscode\include\SDL_image.h"
#include "C:\SDLproject_vscode\include\SDL_mixer.h"

#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#undef main
using namespace std;
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0 || IMG_Init(IMG_INIT_PNG) == 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "Failed to initialize SDL, TTF, IMG, or Mix: " << SDL_GetError() << endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Endless Runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Failed to create window: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Failed to create renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("C:\\SDLproject_vscode\\resource\\game -_-\\font.otf", 16);
    if (!font) {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    loadHighScores(); // Load high scores at the start

    bool running = true;
    while (running) {
        MenuOption option = showMainMenu(renderer, font);

        if (option == MENU_PLAY) {
            runGame(renderer, font);
        } else if (option == MENU_HIGHSCORE) {
            showHighScores(renderer, font);
        } else if (option == MENU_SETTING) {
            showSettings(renderer);
        } else if (option == MENU_EXIT) {
            running = false;
        }
    }

    saveHighScores(); // Save high scores before exiting
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    SDL_Delay(1000000);
    return 0;
}
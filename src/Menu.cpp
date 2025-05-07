#include "C:\SDLproject_vscode - Copy\include\Menu.h"
#include "C:\SDLproject_vscode - Copy\include\Constants.h" // Use Constants.h for screen dimensions
#include "C:\SDLproject_vscode - Copy\include\Utils.h"   // Use Utils.h for loadTexture
#include "C:\SDLproject_vscode - Copy\include\SDL_render.h"
#include "C:\SDLproject_vscode - Copy\include\SDL_rect.h"
#include "C:\SDLproject_vscode - Copy\include\SDL_mouse.h"
#include "C:\SDLproject_vscode - Copy\include\SDL_events.h"
#include "C:\SDLproject_vscode - Copy\include\SDL_log.h"
#include "C:\SDLproject_vscode - Copy\include\SDL_ttf.h"
#include "C:\SDLproject_vscode - Copy\include\SDL_mixer.h"
#include <iostream> // For error logging

// --- Helper Functions ---
// (These could potentially be moved to Utils.h/cpp if used elsewhere)

// Function to render text
// --- Main Menu Function ---

MenuOption showMainMenu(SDL_Renderer* renderer, TTF_Font* font) {
    const char* options[] = { " ", " ", " ", " " };
    SDL_Rect playButton = { SCREEN_WIDTH / 2 - 50, 126, 100, 28 };
    SDL_Rect highscoreButton = { SCREEN_WIDTH / 2 - 65, 160, 130, 28 };
    SDL_Rect settingButton = { SCREEN_WIDTH / 2 - 50, 220, 100, 28 };
    SDL_Rect exitButton = { SCREEN_WIDTH / 2 - 50, 280, 100, 40 };

    SDL_Texture* backgroundTexture = loadTexture("C:\\SDLproject_vscode\\resource\\mainmenu-_-\\mainmenu.jpg", renderer);
    if (!backgroundTexture) {
        SDL_Log("Failed to load background texture");
        return MENU_EXIT;
    }

    SDL_Texture* playHoverImage = loadTexture("C:\\SDLproject_vscode\\resource\\mainmenu-_-\\mainmenuplay.png", renderer);
    if (!playHoverImage) {
        SDL_Log("Failed to load play hover image");
        SDL_DestroyTexture(backgroundTexture);
        return MENU_EXIT;
    }

    SDL_Texture* highscoreHoverImage = loadTexture("C:\\SDLproject_vscode\\resource\\mainmenu-_-\\mainmenuhighscore.png", renderer);
    if (!highscoreHoverImage) {
        SDL_Log("Failed to load highscore hover image");
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(playHoverImage);
        return MENU_EXIT;
    }

    SDL_Texture* settingHoverImage = loadTexture("C:\\SDLproject_vscode\\resource\\mainmenu-_-\\mainmenusetting.png", renderer);
    if (!settingHoverImage) {
        SDL_Log("Failed to load setting hover image");
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(playHoverImage);
        SDL_DestroyTexture(highscoreHoverImage);
        return MENU_EXIT;
    }

    TTF_Font* titleFont = TTF_OpenFont("C:\\SDLproject_vscode\\resource\\game -_-\\font.otf", 32); // Title font size is twice the menu font size
    if (!titleFont) {
        SDL_Log("Failed to load title font: %s", TTF_GetError());
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(playHoverImage);
        SDL_DestroyTexture(highscoreHoverImage);
        SDL_DestroyTexture(settingHoverImage);
        return MENU_EXIT;
    }

    Mix_Music* menuMusic = Mix_LoadMUS("C:\\SDLproject_vscode\\resource\\mainmenu-_-\\mainmenumusic.mp3");
    if (!menuMusic) {
        SDL_Log("Failed to load menu music: %s", Mix_GetError());
        TTF_CloseFont(titleFont);
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(playHoverImage);
        SDL_DestroyTexture(highscoreHoverImage);
        SDL_DestroyTexture(settingHoverImage);
        return MENU_EXIT;
    }

    Mix_PlayMusic(menuMusic, -1); // Play music in a loop

    bool running = true;
    MenuOption selectedOption = MENU_NONE;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
                selectedOption = MENU_EXIT;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                SDL_Point point = { x, y };

                if (SDL_PointInRect(&point, &playButton)) {
                    selectedOption = MENU_PLAY;
                    running = false;
                } else if (SDL_PointInRect(&point, &highscoreButton)) {
                    selectedOption = MENU_HIGHSCORE;
                    running = false;
                } else if (SDL_PointInRect(&point, &settingButton)) {
                    selectedOption = MENU_SETTING;
                    running = false;
                } else if (SDL_PointInRect(&point, &exitButton)) {
                    selectedOption = MENU_EXIT;
                    running = false;
                }
            }
        }

        // Get mouse position
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point mousePoint = { mouseX, mouseY };

        // Render menu
        SDL_RenderClear(renderer);

        // Render background
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

        // Render hover images based on mouse position
        if (SDL_PointInRect(&mousePoint, &playButton)) {
            SDL_RenderCopy(renderer, playHoverImage, nullptr, nullptr);
        } else if (SDL_PointInRect(&mousePoint, &highscoreButton)) {
            SDL_RenderCopy(renderer, highscoreHoverImage, nullptr, nullptr);
        } else if (SDL_PointInRect(&mousePoint, &settingButton)) {
            SDL_RenderCopy(renderer, settingHoverImage, nullptr, nullptr);
        }

        // Render game title
        SDL_Color white = { 255, 255, 255, 255 };
        SDL_Surface* titleSurface = TTF_RenderText_Solid(titleFont, " ", white);
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect = { SCREEN_WIDTH / 2 - titleSurface->w / 2, 40, titleSurface->w, titleSurface->h };
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);

        // Render menu options
        for (int i = 0; i < 4; ++i) {
            SDL_Surface* surface = TTF_RenderText_Solid(font, options[i], white);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            SDL_Rect textRect = { SCREEN_WIDTH / 2 - surface->w / 2, 110 + i * 60, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, nullptr, &textRect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        SDL_RenderPresent(renderer);
    }

    Mix_HaltMusic(); // Stop the music when leaving the menu
    Mix_FreeMusic(menuMusic);

    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(playHoverImage);
    SDL_DestroyTexture(highscoreHoverImage);
    SDL_DestroyTexture(settingHoverImage);
    TTF_CloseFont(titleFont);
    return selectedOption;
}
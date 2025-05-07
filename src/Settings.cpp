#include "C:\SDLproject_vscode\include\Settings.h"
#include "C:\SDLproject_vscode\include\SDL_render.h"
#include "C:\SDLproject_vscode\include\SDL_mixer.h"
#include "C:\SDLproject_vscode\include\SDL_image.h"
#include "C:\SDLproject_vscode\include\Utils.h"
using namespace std;
void showSettings(SDL_Renderer* renderer) {
    SDL_Texture* settingsTexture = loadTexture("C:\\SDLproject_vscode\\resource\\setting-_-\\setting.jpg", renderer);
    SDL_Texture* muteHoverTexture = loadTexture("C:\\SDLproject_vscode\\resource\\setting-_-\\settingoff.png", renderer);
    SDL_Texture* unmuteHoverTexture = loadTexture("C:\\SDLproject_vscode\\resource\\setting-_-\\settingon.png", renderer);

    if (!settingsTexture || !muteHoverTexture || !unmuteHoverTexture) {
        SDL_Log("Failed to load textures for settings");
        SDL_DestroyTexture(settingsTexture);
        SDL_DestroyTexture(muteHoverTexture);
        SDL_DestroyTexture(unmuteHoverTexture);
        return;
    }

    Mix_Music* settingsMusic = Mix_LoadMUS("C:\\SDLproject_vscode\\resource\\setting-_-\\settingmusic.mp3");
    if (!settingsMusic) {
        SDL_Log("Failed to load settings music: %s", Mix_GetError());
        SDL_DestroyTexture(settingsTexture);
        SDL_DestroyTexture(muteHoverTexture);
        SDL_DestroyTexture(unmuteHoverTexture);
        return;
    }

    Mix_PlayMusic(settingsMusic, -1); // Play music in a loop

    SDL_Rect muteButton = {202, 127, 33, 33}; // Mute button rectangle
    SDL_Rect unmuteButton = {163, 127, 33, 33}; // Unmute button rectangle

    bool running = true;
    bool isMuted = false; // Track mute state

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                SDL_Point point = {x, y};

                if (SDL_PointInRect(&point, &muteButton)) {
                    Mix_VolumeMusic(0); // Mute music
                    isMuted = true;
                    running = false; // Return to main menu
                } else if (SDL_PointInRect(&point, &unmuteButton)) {
                    Mix_VolumeMusic(MIX_MAX_VOLUME); // Unmute music
                    isMuted = false;
                    running = false; // Return to main menu
                }
            }
        }

        // Get mouse position
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point mousePoint = {mouseX, mouseY};

        SDL_RenderClear(renderer);

        // Render settings background
        SDL_RenderCopy(renderer, settingsTexture, nullptr, nullptr);

        // Render hover images based on mouse position
        if (SDL_PointInRect(&mousePoint, &muteButton)) {
            SDL_RenderCopy(renderer, muteHoverTexture, nullptr, nullptr);
        } else if (SDL_PointInRect(&mousePoint, &unmuteButton)) {
            SDL_RenderCopy(renderer, unmuteHoverTexture, nullptr, nullptr);
        }

        // Render mute and unmute buttons
        

        SDL_RenderPresent(renderer);
    }

    Mix_HaltMusic(); // Stop the music when leaving the settings section
    Mix_FreeMusic(settingsMusic);
    SDL_DestroyTexture(settingsTexture);
    SDL_DestroyTexture(muteHoverTexture);
    SDL_DestroyTexture(unmuteHoverTexture);
}

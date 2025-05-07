#include "C:\SDLproject_vscode\include\HighScores.h"
#include "C:\SDLproject_vscode\include\SDL_ttf.h"
#include "C:\SDLproject_vscode\include\SDL_image.h"
#include "C:\SDLproject_vscode\include\SDL_mixer.h"
#include "C:\SDLproject_vscode\include\Constants.h"
#include "C:\SDLproject_vscode\include\Utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;
vector<int> highScores; // Define highScores here

void loadHighScores() {
    highScores.clear();
    ifstream file("C:\\SDLproject_vscode\\resource\\highscore-_-\\highscores.txt");
    int score;
    while (file >> score) {
        highScores.push_back(score);
    }
    file.close();
    sort(highScores.rbegin(), highScores.rend()); // Sort in descending order
}

void saveHighScores() {
    ofstream file("C:\\SDLproject_vscode\\resource\\highscore-_-\\highscores.txt");
    for (size_t i = 0; i < min(highScores.size(), static_cast<size_t>(MAX_HIGHSCORES)); ++i) {
        file << highScores[i] << endl;
    }
    file.close();
}

void renderHighScores(SDL_Renderer* renderer, TTF_Font* font, int scrollOffset) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Rect boxRect = {54, 131, 209, 300}; // Define the box dimensions for all scores

    // Render the border for the high scores box
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White border
    SDL_RenderDrawRect(renderer, &boxRect);

    // Set the clipping area to restrict rendering to the box
    SDL_RenderSetClipRect(renderer, &boxRect);

    // Render all high scores inside the box
    int lineHeight = 25; // Height of each line for a score
    int maxScoresToShow = min(static_cast<int>(highScores.size()), 10); // Limit to 10 scores

    for (int i = 0; i < maxScoresToShow; ++i) {
        string scoreText = to_string(i + 1) + ". " + to_string(highScores[i]);
        SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect textRect = {
            boxRect.x + 10, // Add padding from the left
            boxRect.y + 10 + i * lineHeight - scrollOffset, // Adjust position with scroll offset
            surface->w,
            surface->h
        };

        // Render only if the text is within the visible box
        if (textRect.y + textRect.h > boxRect.y && textRect.y < boxRect.y + boxRect.h) {
            SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        }

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Reset the clipping area
    SDL_RenderSetClipRect(renderer, nullptr);
}

void showHighScores(SDL_Renderer* renderer, TTF_Font* font) {
    loadHighScores();

    SDL_Texture* backgroundTexture = loadTexture("C:\\SDLproject_vscode\\resource\\highscore-_-\\highscore.png", renderer);
    if (!backgroundTexture) {
        SDL_Log("Failed to load highscore background texture");
        return;
    }

    Mix_Music* highscoreMusic = Mix_LoadMUS("C:\\SDLproject_vscode\\resource\\highscore-_-\\highscore.mp3");
    if (!highscoreMusic) {
        SDL_Log("Failed to load highscore music: %s", Mix_GetError());
        SDL_DestroyTexture(backgroundTexture);
        return;
    }

    Mix_PlayMusic(highscoreMusic, -1); // Play music in a loop

    bool running = true;
    int scrollOffset = 0; // Track the scroll offset
    const int maxScroll = 250; // Maximum scroll offset (10 scores * 25 line height - box height)

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                running = false;
            } else if (e.type == SDL_MOUSEWHEEL) {
                // Adjust scroll offset based on mouse wheel movement
                scrollOffset -= e.wheel.y * 25; // Scroll by one line per wheel step
                if (scrollOffset < 0) scrollOffset = 0;
                if (scrollOffset > maxScroll) scrollOffset = maxScroll;
            }
        }

        SDL_RenderClear(renderer);

        // Render background
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

        // Render high scores with scrolling
        renderHighScores(renderer, font, scrollOffset);

        SDL_RenderPresent(renderer);
    }

    Mix_HaltMusic(); // Stop the music when leaving the high scores section
    Mix_FreeMusic(highscoreMusic);
    SDL_DestroyTexture(backgroundTexture);
}
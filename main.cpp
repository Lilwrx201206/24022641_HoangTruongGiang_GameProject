#include "include/SDL.h"
#include "include/SDL_ttf.h"
#include "include/SDL_image.h"
#include "include/SDL_mixer.h"
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#undef main
using namespace std;

const int SCREEN_WIDTH = 320; // Updated width
const int SCREEN_HEIGHT = 256; // Updated height
const int PLAYER_WIDTH = 24; // Updated width
const int PLAYER_HEIGHT = 20; // Updated height
const int OBSTACLE_WIDTH = 24; // Updated width
const int OBSTACLE_HEIGHT = 20; // Updated height
const int GRAVITY = 1;
const int JUMP_STRENGTH = -15; // Jump strength remains unchanged
float obstacleSpeed = 5.0f; // Changed to float for speed adjustments
const float SPEED_INCREMENT = 1.05f; // 5% speed increase
const int OBSTACLES_TO_INCREMENT = 10; // Number of obstacles to trigger speed increase

const int PLAYER_FRAME_WIDTH = 24; // Width of each frame
const int PLAYER_FRAME_HEIGHT = 20; // Height of each frame
const int PLAYER_FRAME_COUNT = 6; // Total number of frames
const int PLAYER_ANIMATION_FPS = 20; // Updated to 20 FPS

const int PLAYER_JUMP_FRAME_WIDTH = 24; // Width of each jump frame
const int PLAYER_JUMP_FRAME_HEIGHT = 24; // Height of each jump frame
const int PLAYER_JUMP_FRAME_COUNT = 5; // Updated to 5 frames
const int PLAYER_JUMP_ANIMATION_FPS = 10; // Updated to 10 FPS

const int OBSTACLE_FRAME_WIDTH = 32; // Width of each obstacle frame
const int OBSTACLE_FRAME_HEIGHT = 24; // Height of each obstacle frame
const int OBSTACLE_FRAME_COUNT = 6; // Total number of frames for obstacle animation
const int OBSTACLE_ANIMATION_FPS = 10; // Animation FPS for obstacles

const int MAX_HIGHSCORES = 5; // Maximum number of high scores to store
vector<int> highScores; // Vector to store high scores

const int MAP_WIDTH = 448; // Updated map width
const int MAP_HEIGHT = 256; // Updated map height

int lives = 3; // Initial lives
int score = 0; // Player's score

struct Obstacle {
    int x, y;
};

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

void renderScore(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color white = {255, 255, 255, 255};
    string scoreText = "Score: " + to_string(score);

    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = {10, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderLives(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color white = {255, 255, 255, 255};
    string livesText = "Lives: " + to_string(lives);

    SDL_Surface* surface = TTF_RenderText_Solid(font, livesText.c_str(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = {SCREEN_WIDTH - surface->w - 10, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderPlayer(SDL_Renderer* renderer, SDL_Texture* playerTexture, SDL_Rect& player, int frame) {
    SDL_Rect srcRect = {
        frame * PLAYER_FRAME_WIDTH, // X position of the frame in the sprite sheet
        0, // Y position (top of the sprite sheet)
        PLAYER_FRAME_WIDTH,
        PLAYER_FRAME_HEIGHT
    };
    SDL_RenderCopy(renderer, playerTexture, &srcRect, &player);
}

void renderPlayerJump(SDL_Renderer* renderer, SDL_Texture* jumpTexture, SDL_Rect& player, int frame) {
    SDL_Rect srcRect = {
        frame * PLAYER_JUMP_FRAME_WIDTH, // X position of the frame in the sprite sheet
        0, // Y position (top of the sprite sheet)
        PLAYER_JUMP_FRAME_WIDTH,
        PLAYER_JUMP_FRAME_HEIGHT
    };
    SDL_RenderCopy(renderer, jumpTexture, &srcRect, &player);
}

void renderObstacle(SDL_Renderer* renderer, SDL_Texture* obstacleTexture, SDL_Rect& obstacle, int frame) {
    SDL_Rect srcRect = {
        frame * OBSTACLE_FRAME_WIDTH, // X position of the frame in the sprite sheet
        0, // Y position (top of the sprite sheet)
        OBSTACLE_FRAME_WIDTH,
        OBSTACLE_FRAME_HEIGHT
    };
    SDL_RenderCopy(renderer, obstacleTexture, &srcRect, &obstacle);
}

void renderInfiniteMap(SDL_Renderer* renderer, SDL_Texture* mapTexture, int& mapOffset) {
    // Use the updated map dimensions
    SDL_Rect srcRect = { 0, 0, MAP_WIDTH, MAP_HEIGHT };
    SDL_Rect dstRect1 = { -mapOffset, 0, MAP_WIDTH, SCREEN_HEIGHT };
    SDL_Rect dstRect2 = { MAP_WIDTH - mapOffset, 0, MAP_WIDTH, SCREEN_HEIGHT };

    // Render the two segments
    SDL_RenderCopy(renderer, mapTexture, &srcRect, &dstRect1);
    SDL_RenderCopy(renderer, mapTexture, &srcRect, &dstRect2);

    // Update the map offset
    mapOffset += 2; // Adjust the speed of scrolling here
    if (mapOffset >= MAP_WIDTH) {
        mapOffset = 0; // Reset offset when the first segment fully scrolls out
    }
}

void loadHighScores() {
    highScores.clear();
    ifstream file("C:\\SDLproject_vscode\\highscores.txt");
    int score;
    while (file >> score) {
        highScores.push_back(score);
    }
    file.close();
    sort(highScores.rbegin(), highScores.rend()); // Sort in descending order
}

void saveHighScores() {
    ofstream file("C:\\SDLproject_vscode\\highscores.txt");
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

    SDL_Texture* backgroundTexture = loadTexture("C:\\SDLproject_vscode\\resource\\ChatGPT Image 15_55_57 17 thg 4, 2025.png", renderer);
    if (!backgroundTexture) {
        SDL_Log("Failed to load highscore background texture");
        return;
    }

    Mix_Music* highscoreMusic = Mix_LoadMUS("C:\\SDLproject_vscode\\epic-gamer-music.mp3");
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

void showSettings(SDL_Renderer* renderer) {
    SDL_Texture* settingsTexture = loadTexture("C:\\SDLproject_vscode\\z6515153283334_66ad447253a5351df3a0050ccbb208c9.jpg", renderer);
    SDL_Texture* muteHoverTexture = loadTexture("C:\\SDLproject_vscode\\settingoff.png", renderer);
    SDL_Texture* unmuteHoverTexture = loadTexture("C:\\SDLproject_vscode\\settingon.png", renderer);

    if (!settingsTexture || !muteHoverTexture || !unmuteHoverTexture) {
        SDL_Log("Failed to load textures for settings");
        SDL_DestroyTexture(settingsTexture);
        SDL_DestroyTexture(muteHoverTexture);
        SDL_DestroyTexture(unmuteHoverTexture);
        return;
    }

    Mix_Music* settingsMusic = Mix_LoadMUS("C:\\SDLproject_vscode\\Trường Đại học Công nghệ mến yêu.mp3");
    if (!settingsMusic) {
        SDL_Log("Failed to load settings music: %s", Mix_GetError());
        SDL_DestroyTexture(settingsTexture);
        SDL_DestroyTexture(muteHoverTexture);
        SDL_DestroyTexture(unmuteHoverTexture);
        return;
    }

    Mix_PlayMusic(settingsMusic, -1); // Play music in a loop

    SDL_Rect muteButton = {163, 127, 33, 33}; // Mute button rectangle
    SDL_Rect unmuteButton = {202, 127, 33, 33}; // Unmute button rectangle

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

enum MenuOption { MENU_PLAY, MENU_HIGHSCORE, MENU_SETTING, MENU_EXIT, MENU_NONE };

MenuOption showMainMenu(SDL_Renderer* renderer, TTF_Font* font) {
    const char* options[] = { " ", " ", " ", " " };
    SDL_Rect playButton = { SCREEN_WIDTH / 2 - 50, 126, 100, 28 };
    SDL_Rect highscoreButton = { SCREEN_WIDTH / 2 - 65, 160, 130, 28 };
    SDL_Rect settingButton = { SCREEN_WIDTH / 2 - 50, 220, 100, 28 };
    SDL_Rect exitButton = { SCREEN_WIDTH / 2 - 50, 280, 100, 40 };

    SDL_Texture* backgroundTexture = loadTexture("C:\\SDLproject_vscode\\z6514644732983_d6b6e70e0a497a65a4cd7d8a23410e34.jpg", renderer);
    if (!backgroundTexture) {
        SDL_Log("Failed to load background texture");
        return MENU_EXIT;
    }

    SDL_Texture* playHoverImage = loadTexture("C:\\SDLproject_vscode\\Screenshot 2025-04-29 020514.png", renderer);
    if (!playHoverImage) {
        SDL_Log("Failed to load play hover image");
        SDL_DestroyTexture(backgroundTexture);
        return MENU_EXIT;
    }

    SDL_Texture* highscoreHoverImage = loadTexture("C:\\SDLproject_vscode\\Screenshot 2025-04-29 020620.png", renderer);
    if (!highscoreHoverImage) {
        SDL_Log("Failed to load highscore hover image");
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(playHoverImage);
        return MENU_EXIT;
    }

    SDL_Texture* settingHoverImage = loadTexture("C:\\SDLproject_vscode\\Screenshot 2025-04-29 021600.png", renderer);
    if (!settingHoverImage) {
        SDL_Log("Failed to load setting hover image");
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(playHoverImage);
        SDL_DestroyTexture(highscoreHoverImage);
        return MENU_EXIT;
    }

    TTF_Font* titleFont = TTF_OpenFont("C:\\SDLproject_vscode\\resource\\font.otf", 32); // Title font size is twice the menu font size
    if (!titleFont) {
        SDL_Log("Failed to load title font: %s", TTF_GetError());
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(playHoverImage);
        SDL_DestroyTexture(highscoreHoverImage);
        SDL_DestroyTexture(settingHoverImage);
        return MENU_EXIT;
    }

    Mix_Music* menuMusic = Mix_LoadMUS("C:\\SDLproject_vscode\\squid-game-season-2-music.mp3");
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

void showGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, bool& restartGame, bool& returnToMenu) {
    SDL_Texture* defaultBackgroundTexture = loadTexture("C:\\SDLproject_vscode\\1000_F_516305253_LIJ19QqUYkB1Mmk4oZVsQJnb7P7WNXOe.jpg", renderer);
    SDL_Texture* hoverRestartBackgroundTexture = loadTexture("C:\\SDLproject_vscode\\Thiết kế chưa có tên (1).png", renderer);
    SDL_Texture* hoverMenuBackgroundTexture = loadTexture("C:\\SDLproject_vscode\\Thiết kế chưa có tên (2).png", renderer);

    if (!defaultBackgroundTexture || !hoverRestartBackgroundTexture || !hoverMenuBackgroundTexture) {
        SDL_Log("Failed to load game over background textures");
        return;
    }

    Mix_Music* gameOverMusic = Mix_LoadMUS("C:\\SDLproject_vscode\\dumb-ways-to-die.mp3");
    if (!gameOverMusic) {
        SDL_Log("Failed to load game over music: %s", Mix_GetError());
        SDL_DestroyTexture(defaultBackgroundTexture);
        SDL_DestroyTexture(hoverRestartBackgroundTexture);
        SDL_DestroyTexture(hoverMenuBackgroundTexture);
        return;
    }

    Mix_PlayMusic(gameOverMusic, -1); // Play music in a loop

    SDL_Rect restartButton = {100, 143, 56, 23}; // Restart button rectangle
    SDL_Rect mainMenuButton = {163, 143, 56, 23}; // Main menu button rectangle

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
                returnToMenu = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                SDL_Point point = {x, y};

                if (SDL_PointInRect(&point, &restartButton)) {
                    restartGame = true;
                    running = false;
                } else if (SDL_PointInRect(&point, &mainMenuButton)) {
                    returnToMenu = true;
                    running = false;
                }
            }
        }

        // Get mouse position
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point mousePoint = {mouseX, mouseY};

        SDL_RenderClear(renderer);

        // Render background based on hover state
        if (SDL_PointInRect(&mousePoint, &restartButton)) {
            SDL_RenderCopy(renderer, hoverRestartBackgroundTexture, nullptr, nullptr);
        } else if (SDL_PointInRect(&mousePoint, &mainMenuButton)) {
            SDL_RenderCopy(renderer, hoverMenuBackgroundTexture, nullptr, nullptr);
        } else {
            SDL_RenderCopy(renderer, defaultBackgroundTexture, nullptr, nullptr);
        }

        SDL_RenderPresent(renderer);
    }

    Mix_HaltMusic(); // Stop the music when leaving the game over screen
    Mix_FreeMusic(gameOverMusic);
    SDL_DestroyTexture(defaultBackgroundTexture);
    SDL_DestroyTexture(hoverRestartBackgroundTexture);
    SDL_DestroyTexture(hoverMenuBackgroundTexture);
}

void runGame(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Rect player = {100, SCREEN_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT}; // Updated size
    int playerVelocityY = 0;
    int jumpCount = 0; // Track the number of jumps
    const int MAX_JUMPS = 2; // Allow up to two jumps (double jump)

    vector<Obstacle> obstacles;
    int spawnTimer = 0;
    int obstaclesPassed = 0; // Track the number of obstacles passed

    lives = 3; // Reset lives to 3 when the game starts
    score = 0; // Reset score

    SDL_Texture* playerTexture = loadTexture("C:\\SDLproject_vscode\\resource\\playerRun.png", renderer);
    if (!playerTexture) {
        SDL_Log("Failed to load player texture");
        return;
    }

    SDL_Texture* playerJumpTexture = loadTexture("C:\\SDLproject_vscode\\resource\\playerJump.png", renderer);
    if (!playerJumpTexture) {
        SDL_Log("Failed to load player jump texture");
        SDL_DestroyTexture(playerTexture);
        return;
    }

    SDL_Texture* obstacleTexture = loadTexture("C:\\SDLproject_vscode\\resource\\Alien\\Alien_run.png", renderer);
    if (!obstacleTexture) {
        SDL_Log("Failed to load obstacle texture");
        SDL_DestroyTexture(playerTexture);
        SDL_DestroyTexture(playerJumpTexture);
        return;
    }

    SDL_Texture* mapTexture = loadTexture("C:\\SDLproject_vscode\\resource\\map.png", renderer);
    if (!mapTexture) {
        SDL_Log("Failed to load map texture");
        SDL_DestroyTexture(playerTexture);
        SDL_DestroyTexture(playerJumpTexture);
        SDL_DestroyTexture(obstacleTexture);
        return;
    }

    Mix_Chunk* hurtSound = Mix_LoadWAV("C:\\SDLproject_vscode\\classic_hurt.mp3");
    Mix_Chunk* jumpSound = Mix_LoadWAV("C:\\SDLproject_vscode\\super-mario-bros_e5hRRa6.mp3");
    if (!hurtSound || !jumpSound) {
        SDL_Log("Failed to load sounds: %s", Mix_GetError());
        SDL_DestroyTexture(playerTexture);
        SDL_DestroyTexture(playerJumpTexture);
        SDL_DestroyTexture(obstacleTexture);
        SDL_DestroyTexture(mapTexture);
        return;
    }

    int mapOffset = 0; // Offset for scrolling the map

    int playerFrame = 0; // Current animation frame for the player
    int jumpFrame = 0; // Current animation frame for the jump
    int obstacleFrame = 0; // Current animation frame for obstacles
    Uint32 frameStart = SDL_GetTicks(); // Track time for animation

    bool running = true;
    bool restartGame = false;
    bool returnToMenu = false;
    SDL_Event e;

    srand(static_cast<unsigned>(time(0)));

    while (running) {
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE && jumpCount < MAX_JUMPS) {
                playerVelocityY = JUMP_STRENGTH;
                jumpCount++;
                Mix_PlayChannel(-1, jumpSound, 0); // Play jump sound
            }
        }

        // Update player position
        playerVelocityY += GRAVITY;
        player.y += playerVelocityY;

        if (player.y + PLAYER_HEIGHT >= SCREEN_HEIGHT) {
            player.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
            jumpCount = 0; // Reset jump count when the player lands
        }

        // Spawn obstacles
        spawnTimer++;
        if (spawnTimer > 100) {
            obstacles.push_back({SCREEN_WIDTH, SCREEN_HEIGHT - OBSTACLE_HEIGHT}); // Updated size
            spawnTimer = 0;
        }

        // Update obstacles
        for (auto& obs : obstacles) {
            obs.x -= static_cast<int>(obstacleSpeed);
        }

        // Remove off-screen obstacles and count passed obstacles
        obstacles.erase(remove_if(obstacles.begin(), obstacles.end(), [&](Obstacle& obs) {
            if (obs.x + OBSTACLE_WIDTH < 0) {
                obstaclesPassed++;
                score++; // Increment score for each obstacle passed
                if (obstaclesPassed % OBSTACLES_TO_INCREMENT == 0) {
                    obstacleSpeed *= SPEED_INCREMENT; // Increase speed by 5%
                    lives++; // Gain 1 life for every 10 obstacles passed
                }
                return true;
            }
            return false;
        }), obstacles.end());

        // Check collisions
        SDL_Rect playerRect = {player.x, player.y, player.w, player.h};
        for (auto& obs : obstacles) {
            SDL_Rect obstacleRect = {obs.x, obs.y, OBSTACLE_FRAME_WIDTH, OBSTACLE_FRAME_HEIGHT}; // Updated size
            if (checkCollision(playerRect, obstacleRect)) {
                lives--; // Lose 1 life on collision
                Mix_PlayChannel(-1, hurtSound, 0); // Play hurt sound
                obs.x = -OBSTACLE_WIDTH; // Move the obstacle off-screen to avoid multiple collisions
                if (lives <= 0) {
                    highScores.push_back(score); // Add the score to the high scores
                    saveHighScores(); // Save the updated high scores
                    running = false; // Game over
                    showGameOverScreen(renderer, font, restartGame, returnToMenu);
                }
                break;
            }
        }

        // Update animation frames
        if (SDL_GetTicks() - frameStart >= 1000 / PLAYER_ANIMATION_FPS) {
            playerFrame = (playerFrame + 1) % PLAYER_FRAME_COUNT;
            obstacleFrame = (obstacleFrame + 1) % OBSTACLE_FRAME_COUNT;
            if (jumpCount > 0) {
                jumpFrame = (jumpFrame + 1) % PLAYER_JUMP_FRAME_COUNT;
            }
            frameStart = SDL_GetTicks();
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // Sky blue
        SDL_RenderClear(renderer);

        // Render infinite map
        renderInfiniteMap(renderer, mapTexture, mapOffset);

        // Draw player with animation
        if (jumpCount > 0) {
            renderPlayerJump(renderer, playerJumpTexture, player, jumpFrame);
        } else {
            renderPlayer(renderer, playerTexture, player, playerFrame);
        }

        // Draw obstacles with animation
        for (const auto& obs : obstacles) {
            SDL_Rect obstacleRect = {obs.x, obs.y, OBSTACLE_FRAME_WIDTH, OBSTACLE_FRAME_HEIGHT}; // Updated size
            renderObstacle(renderer, obstacleTexture, obstacleRect, obstacleFrame);
        }

        renderScore(renderer, font); // Render score in the top-left corner
        renderLives(renderer, font); // Render lives in the top-right corner

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    if (restartGame) {
        runGame(renderer, font);
    }

    Mix_FreeChunk(hurtSound);
    Mix_FreeChunk(jumpSound);
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(playerJumpTexture);
    SDL_DestroyTexture(obstacleTexture);
}

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

    TTF_Font* font = TTF_OpenFont("C:\\SDLproject_vscode\\resource\\font.otf", 16);
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
            showHighScores(renderer, font); // Navigate to high scores
        } else if (option == MENU_SETTING) {
            showSettings(renderer); // Navigate to settings
        } else if (option == MENU_EXIT) {
            running = false;
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

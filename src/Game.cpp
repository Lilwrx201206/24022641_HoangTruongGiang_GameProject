#include "C:\SDLproject_vscode\include\Game.h"
#include "C:\SDLproject_vscode\include\SDL_ttf.h"
#include "C:\SDLproject_vscode\include\SDL_image.h"
#include "C:\SDLproject_vscode\include\SDL_mixer.h"
#include "C:\SDLproject_vscode\include\HighScores.h"
#include "C:\SDLproject_vscode\include\Utils.h"

using namespace std;
int score = 0;
int lives = 3;
float obstacleSpeed = 5.0f; // Initial speed of obstacles
void showGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, bool& restartGame, bool& returnToMenu) {
    SDL_Texture* defaultBackgroundTexture = loadTexture("C:\\SDLproject_vscode\\resource\\gameover-_-\\gameover.jpg", renderer);
    SDL_Texture* hoverRestartBackgroundTexture = loadTexture("C:\\SDLproject_vscode\\resource\\gameover-_-\\gameoveryes.png", renderer);
    SDL_Texture* hoverMenuBackgroundTexture = loadTexture("C:\\SDLproject_vscode\\resource\\gameover-_-\\gameoverno.png", renderer);

    if (!defaultBackgroundTexture || !hoverRestartBackgroundTexture || !hoverMenuBackgroundTexture) {
        SDL_Log("Failed to load game over background textures");
        return;
    }

    Mix_Music* gameOverMusic = Mix_LoadMUS("C:\\SDLproject_vscode\resource\\gameover-_-\\gameovermusic.mp3");
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

    SDL_Texture* playerTexture = loadTexture("C:\\SDLproject_vscode\\resource\\game -_-\\playerRun.png", renderer);
    if (!playerTexture) {
        SDL_Log("Failed to load player texture");
        return;
    }

    SDL_Texture* playerJumpTexture = loadTexture("C:\\SDLproject_vscode\\resource\\game -_-\\playerJump.png", renderer);
    if (!playerJumpTexture) {
        SDL_Log("Failed to load player jump texture");
        SDL_DestroyTexture(playerTexture);
        return;
    }

    SDL_Texture* obstacleTexture = loadTexture("C:\\SDLproject_vscode\\resource\\game -_-\\AlienRun.png", renderer);
    if (!obstacleTexture) {
        SDL_Log("Failed to load obstacle texture");
        SDL_DestroyTexture(playerTexture);
        SDL_DestroyTexture(playerJumpTexture);
        return;
    }

    SDL_Texture* mapTexture = loadTexture("C:\\SDLproject_vscode\\resource\\game -_-\\map.png", renderer);
    if (!mapTexture) {
        SDL_Log("Failed to load map texture");
        SDL_DestroyTexture(playerTexture);
        SDL_DestroyTexture(playerJumpTexture);
        SDL_DestroyTexture(obstacleTexture);
        return;
    }

    Mix_Chunk* hurtSound = Mix_LoadWAV("C:\\SDLproject_vscode\\resource\\game -_-\\bithuong.mp3");
    Mix_Chunk* jumpSound = Mix_LoadWAV("C:\\SDLproject_vscode\\resource\\game -_-\\jumpsound.mp3");
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

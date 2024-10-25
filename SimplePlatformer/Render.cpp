#include "Render.h"
#include "Config.h"
#include <SDL.h>
#include <iostream>
#include <vector>

int Render(int argc, char* argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << '\n';
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("2D Platformer with SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN);
    SDL_SetWindowFullscreen(window, true);

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    // Load player sprite
    Config config;
    SDL_Texture* playerTexture = nullptr;
    SDL_Surface* tempSurface = SDL_LoadBMP(config.playerSettings.spritePath);
    if (!tempSurface) {
        std::cerr << "SDL_LoadBMP Error: " << SDL_GetError() << '\n';
    }
    else {
        playerTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
        SDL_FreeSurface(tempSurface);
    }

    // Game loop control
    bool isRunning = true;

    // Blocks creation
    std::vector<SDL_Rect> blocks = config.blockSettings.positions;

    // Player properties
    SDL_Rect player = config.playerSettings.startPos;
    int playerSpeed = 5;
    float gravity = 0.5f;
    float verticalVelocity = 0.0f;
    bool isJumping = false;
    SDL_Rect lastSafePosition = player;

    // Camera offset to follow player
    int cameraOffsetX = 0;

    // Define maximum camera offset
    const int maxCameraOffsetX = 3000;

    // Add this before the main loop, to track player direction
    bool isFacingRight = true;

    // Variables to track the timing
    Uint32 conditionStartTime = 0; // Start time when condition first became true
    bool conditionWasTrue = false;  // Indicates if the condition was true

    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                isRunning = false;
            }
        }

        // Player movement handling
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_A]) {
            player.x -= playerSpeed;
            isFacingRight = false; // Set to false when moving left
        }
        if (keystate[SDL_SCANCODE_D]) {
            player.x += playerSpeed;
            isFacingRight = true; // Set to true when moving right
        }
        if (keystate[SDL_SCANCODE_SPACE] && !isJumping) {
            verticalVelocity = -10.0f;  // Jump strength
            isJumping = true;
        }

        // Apply gravity
        verticalVelocity += gravity;
        player.y += static_cast<int>(verticalVelocity);

        // Collision detection with blocks
        bool onBlock = false;
        for (const auto& block : blocks) {
            if (SDL_HasIntersection(&player, &block)) {
                // Check for ground collision (top of the block)
                if (player.y + player.h <= block.y + 10) {
                    player.y = block.y - player.h;
                    verticalVelocity = 0.0f;
                    isJumping = false;
                    lastSafePosition = player;
                    onBlock = true;
                }
                else {
                    // Handle side collisions
                    if (player.x < block.x) { // Colliding with the left side
                        player.x = block.x - player.w; // Move player to the left of the block
                    }
                    else if (player.x > block.x + block.w) { // Colliding with the right side
                        player.x = block.x + block.w; // Move player to the right of the block
                    }
                }
            }
        }
        bool canJumpMidAir = !onBlock && !isJumping;

        // Check if the example condition is true
        if (canJumpMidAir) {
            if (!conditionWasTrue) {
                // If condition just became true, record the start time
                conditionStartTime = SDL_GetTicks();
                conditionWasTrue = true; // Mark the condition as true
            }
        }
        else {
            // Reset the condition if it is no longer true
            conditionWasTrue = false;
        }

        // Check if the condition has been true for at least 100 milliseconds
        if (canJumpMidAir && (SDL_GetTicks() - conditionStartTime >= 100)) {
            // This block will execute only if the condition has been true for 100 milliseconds
            //std::cout << "Condition has been true for at least 100 milliseconds!" << std::endl;
            isJumping = true;
        }

        // Check if the player falls off the screen
        if (player.y > 1080) {
            player = lastSafePosition;  // Respawn at last safe position
            verticalVelocity = 0.0f;
            isJumping = false;
        }

        // Update camera offset to keep the player centered
        cameraOffsetX = player.x - 500; // Offset to keep the player roughly in the center

        // Limit camera offset to a maximum of 4250
        if (cameraOffsetX > maxCameraOffsetX) {
            cameraOffsetX = maxCameraOffsetX;
        }

        if (cameraOffsetX < 0) {
            cameraOffsetX = 0; // Prevent negative space
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 34, 35, 35, 255);  // Clear to dark gray
        SDL_RenderClear(renderer);

        // Render blocks
        SDL_SetRenderDrawColor(renderer, config.blockSettings.color.r, config.blockSettings.color.g, config.blockSettings.color.b, config.blockSettings.color.a);  // Block color
        for (const auto& block : blocks) {
            SDL_Rect renderRect = { block.x - cameraOffsetX, block.y, block.w, block.h };
            SDL_RenderFillRect(renderer, &renderRect);
        }

        // Rendering the player with flip based on direction
        SDL_Rect playerRenderRect = { player.x - cameraOffsetX, player.y, player.w, player.h };
        if (playerTexture) {
            SDL_RendererFlip flip = isFacingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL; // Flip conditionally
            SDL_RenderCopyEx(renderer, playerTexture, NULL, &playerRenderRect, 0, NULL, flip);
        }
        else {
            SDL_SetRenderDrawColor(renderer, config.playerSettings.color.r, config.playerSettings.color.g, config.playerSettings.color.b, config.playerSettings.color.a);
            SDL_RenderFillRect(renderer, &playerRenderRect);
        }

        // Present rendered frame
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};
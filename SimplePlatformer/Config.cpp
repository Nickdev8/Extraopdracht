#include "Config.h"
#include <SDL.h>
#include <vector>

// Configuration structure to hold all customizable parameters
struct Config{
    struct Color {
        Uint8 r, g, b, a;
    };

    struct PlayerSettings {
        SDL_Rect startPos = { 100, 450 - 64, 64, 64 }; // Starting position and size
        Color color = { 0, 255, 0, 255 }; // Color (used for debugging if no sprite is available)
        const char* spritePath = "player.bmp"; // Path to player sprite
    };

    struct BlockSettings {
        std::vector<SDL_Rect> positions = {
            { 0, 500, 300, 600},    // Big block 1
            { 450, 500, 100, 20},   // Platform 1
            { 700, 500, 100, 20},   // Platform 2
            { 1000, 500, 200, 600}, // Big block 2
            { 1200, 500, 200, 100}, // Platform 3
            { 1550, 500, 100, 20},  // Platform 4
            { 1850, 500, 100, 20},  // Platform 5
            { 2000, 800, 100, 20},  // Platform 6 (big drop)
            { 2250, 725, 100, 20},  // Platform 7
            { 2500, 650, 100, 20},  // Platform 8
            { 2750, 575, 100, 20},  // Platform 9
            { 2950, 500, 10, 20},   // Platform 10
            { 3050, 495, 10, 20},   // Platform 11
            { 3150, 490, 10, 20},   // Platform 12
            { 3250, 485, 10, 20},   // Platform 13
            { 3350, 480, 10, 20},   // Platform 14
            { 3450, 475, 10, 20},   // Platform 15
            { 3550, 470, 10, 20},   // Platform 16
            { 3750, 500, 1000, 600}, // Big block 3
        };
        Color color = { 240, 246, 240, 255 }; // Block color
    };

    PlayerSettings playerSettings;
    BlockSettings blockSettings;
};

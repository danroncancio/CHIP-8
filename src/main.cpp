#include "chip8.h"
#include "raylib.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

const size_t PIXEL_SCALE = 16;

int main (int argc, char *argv[]) {

    std::ifstream rom(argv[1], std::ios::binary);
    if (!rom) {
        std::cerr << "ERROR: Failed to open the rom file" << std::endl;
        return -1;
    }

    Chip8 chip8;
    chip8.load_rom(rom);

    // Display
    InitWindow(chip8.WIDTH * PIXEL_SCALE, chip8.HEIGHT * PIXEL_SCALE, "Chip8");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Simulate a 500hz clock
        for (auto i = 0; i <= 8; i++) {
            chip8.tick();
        }

        chip8.update_timers();
        
        // Input
        // First row
        if (IsKeyPressed(KEY_ONE))
            chip8.keys[1] = true;
        else if (IsKeyReleased(KEY_ONE))
            chip8.keys[1] = false;

        if (IsKeyPressed(KEY_TWO))
            chip8.keys[2] = true;
        else if (IsKeyReleased(KEY_TWO))
            chip8.keys[2] = false;

        if (IsKeyPressed(KEY_THREE))
            chip8.keys[3] = true;
        else if (IsKeyReleased(KEY_THREE))
            chip8.keys[3] = false;

        if (IsKeyPressed(KEY_FOUR))
            chip8.keys[0xC] = true;
        else if (IsKeyReleased(KEY_FOUR))
            chip8.keys[0xC] = false;

        // Second Row
        if (IsKeyPressed(KEY_Q))
            chip8.keys[4] = true;
        else if (IsKeyReleased(KEY_Q))
            chip8.keys[4] = false;

        if (IsKeyPressed(KEY_W))
            chip8.keys[5] = true;
        else if (IsKeyReleased(KEY_W))
            chip8.keys[5] = false;

        if (IsKeyPressed(KEY_E))
            chip8.keys[6] = true;
        else if (IsKeyReleased(KEY_E))
            chip8.keys[6] = false;

        if (IsKeyPressed(KEY_R))
            chip8.keys[0xD] = true;
        else if (IsKeyReleased(KEY_R))
            chip8.keys[0xD] = false;

        // Third Row
        if (IsKeyPressed(KEY_A))
            chip8.keys[7] = true;
        else if (IsKeyReleased(KEY_A))
            chip8.keys[7] = false;

        if (IsKeyPressed(KEY_S))
            chip8.keys[8] = true;
        else if (IsKeyReleased(KEY_S))
            chip8.keys[8] = false;

        if (IsKeyPressed(KEY_D))
            chip8.keys[9] = true;
        else if (IsKeyReleased(KEY_D))
            chip8.keys[9] = false;

        if (IsKeyPressed(KEY_F))
            chip8.keys[0xE] = true;
        else if (IsKeyReleased(KEY_F))
            chip8.keys[0xE] = false;

        // Third Row
        if (IsKeyPressed(KEY_Z))
            chip8.keys[0xA] = true;
        else if (IsKeyReleased(KEY_Z))
            chip8.keys[0xA] = false;

        if (IsKeyPressed(KEY_X))
            chip8.keys[0] = true;
        else if (IsKeyReleased(KEY_X))
            chip8.keys[0] = false;

        if (IsKeyPressed(KEY_C))
            chip8.keys[0xB] = true;
        else if (IsKeyReleased(KEY_C))
            chip8.keys[0xB] = false;

        if (IsKeyPressed(KEY_V))
            chip8.keys[0xF] = true;
        else if (IsKeyReleased(KEY_V))
            chip8.keys[0xF] = false;
        

        BeginDrawing();
        ClearBackground(BLACK);

        for (auto y = 0; y < chip8.HEIGHT; y++) {
            for (auto x = 0; x < chip8.WIDTH; x++) {
                if (chip8.display_buffer[y * chip8.WIDTH + x] == 0x1) {
                    DrawRectangle(x * PIXEL_SCALE, y * PIXEL_SCALE, PIXEL_SCALE - 2, PIXEL_SCALE -2, WHITE);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

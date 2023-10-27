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
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_SPACE)) {
            chip8.tick();
        }

        // Simulate a 500hz clock
        for (auto i = 0; i <= 8; i++) {
            chip8.tick();
        }

        chip8.update_timers();

        // Drawing
        for (auto y = 0; y < chip8.HEIGHT; y++) {
            for (auto x = 0; x < chip8.WIDTH; x++) {
                if (chip8.display_buffer[y * chip8.WIDTH + x] == 0x1) {
                    DrawRectangle(x * PIXEL_SCALE, y * PIXEL_SCALE, PIXEL_SCALE, PIXEL_SCALE, WHITE);
                }
            }
        }

        EndDrawing();
    }

    return 0;
}

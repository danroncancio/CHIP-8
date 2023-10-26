#ifndef CHIP8
#define CHIP8

#include <fstream>
#include <iostream>
#include <cstdint>
#include <vector>
#include <stack>
#include <string>

struct Nibbles {
    uint16_t first{0};
    uint16_t second{0};
    uint16_t third{0};
    uint16_t n{0};
    uint16_t nn{0};
    uint16_t nnn{0};
};

class Chip8 {
private:
    uint8_t m_memory[4*1024]{0};        // 4 kilobytes of RAM
    uint8_t m_registers[16]{0};         // General-purpose variables from V0 to VF 
    std::stack<uint16_t> m_stack[16];   // Used to call subroutines and return from them
    uint8_t m_delay_timer{0};           // Timer that decrements at a rate of 60hz
    uint8_t m_sound_timer{0};           // Similar to the delay timer
    uint16_t m_pc{0x200};                   // Program counter, points at the current intruction in mem
    uint16_t m_I{0};                    // Index register, points at locations in mem
    uint16_t m_opcode{0};               // Variable for holding the 2byte operation code
    Nibbles m_nibbles;

    void fetch();
    void decode();
    void execute();

public:
    static const size_t WIDTH{64};
    static const size_t HEIGHT{32};
    uint8_t display_buffer[WIDTH*HEIGHT]{0};

    Chip8();
    ~Chip8();

    void load_rom(std::ifstream &rom_file);
    void tick();
};

#endif // !CHIP8

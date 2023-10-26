#include "chip8.h"
#include <cstdint>
#include <cstring>

Chip8::Chip8() {
    // Load built-in font in memory.
    size_t font_mem_offset = 0x50;
    uint8_t font[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80 // F
    };

    for (auto &b : font) {
        m_memory[font_mem_offset] = b;
        font_mem_offset++;
    }
}

Chip8::~Chip8() {}

void Chip8::load_rom(std::ifstream &rom_file) {
    size_t rom_mem_offset = 0x200;

    char byte;
    while(rom_file.read(&byte, sizeof(byte))) {
        m_memory[rom_mem_offset] = static_cast<uint8_t>(byte);
        rom_mem_offset++;
    }
}

void Chip8::tick() {
    fetch();
    decode();
    execute();
}

void Chip8::fetch() {
    m_opcode = 0;
    m_opcode |= m_memory[m_pc] << 8;
    m_opcode |= m_memory[m_pc + 1];
    m_pc += 2;

    printf("Opcode: 0x%02X \n", m_opcode);
}

void Chip8::decode() {
    m_nibbles.first = (m_opcode & 0xF000) >> 12;
    m_nibbles.second = (m_opcode & 0x0F00) >> 8;
    m_nibbles.third = (m_opcode & 0x00F0) >> 4;
    m_nibbles.n= m_opcode & 0x000F;
    m_nibbles.nn= m_opcode & 0x00FF;
    m_nibbles.nnn= m_opcode & 0x0FFF;
}

void Chip8::execute() {
    switch (m_nibbles.first) {
        case 0x0:
            switch (m_nibbles.nn) {
                case 0xE0:
                    // Clear screen
                    memset(display_buffer, 0, sizeof(display_buffer));
                    break;
            }
            break;
        case 0x1:
            // Jump to address NNN
            m_pc = m_nibbles.nnn;
            break;
        case 0x6:
            // Set register VX to NN
            m_registers[m_nibbles.second] = m_nibbles.nn;
            break;
        case 0x7:
            // Add the value NN to the register VX
            m_registers[m_nibbles.second] += m_nibbles.nn;
            break;
        case 0xA:
            // Set index register to NNN
            m_I = m_nibbles.nnn;
            break;
        case 0xD:
            // Draw
            uint8_t x, y;
            x = m_registers[m_nibbles.second] % WIDTH;
            y = m_registers[m_nibbles.third] % HEIGHT;
            m_registers[0xF] = 0;

            for (auto row = 0; row < m_nibbles.n; row++) {
                uint8_t sprite_data = m_memory[m_I + row];
                uint8_t shift = 0x7;
                uint8_t mask = 0x80;

                for (auto col = 0; col < 8; col++) {
                    uint8_t pixel_bit = (sprite_data & mask) >> shift;
                    if (shift >= 0)
                        shift -= 1;
                    
                    size_t idx = x + WIDTH * y;
                    if (pixel_bit == 0x1 && display_buffer[idx] == 0x1) {
                        display_buffer[idx] = 0;
                        m_registers[0xF] = 0x1;
                    } else if (pixel_bit == 0x1 && display_buffer[idx] != 0x1) {
                        display_buffer[idx] = 0x1;
                    }

                    x += 1;
                    mask >>= 1;
                }

                x = m_registers[m_nibbles.second] % WIDTH;
                y += 1;

            }
            break;
    }
}

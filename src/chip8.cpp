#include "chip8.h"

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

void Chip8::update_timers() {
    if (m_delay_timer > 0) {
        m_delay_timer--;
    }

    if (m_sound_timer > 0) {
        m_sound_timer--;
    }
}

void Chip8::fetch() {
    m_opcode = 0;
    m_opcode |= m_memory[m_pc] << 8;
    m_opcode |= m_memory[m_pc + 1];
    m_pc += 2;

    //printf("Opcode: 0x%02X \n", m_opcode);
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
                case 0xEE:
                    // Return from subroutine
                    m_pc = m_stack.top();
                    m_stack.pop();
                    break;
            }
            break;
        case 0x1:
            // Jump to address NNN
            m_pc = m_nibbles.nnn;
            break;
        case 0x2:
            // Call subroutine ad NNN
            m_stack.push(m_pc);
            m_pc = m_nibbles.nnn;
            break;
        case 0x3:
            // Skips next instruction if VX == NN
            if (m_registers[m_nibbles.second] == m_nibbles.nn)
                m_pc += 2;
            break;
        case 0x4:
            // Skips next instruction if VX != NN
            if (m_registers[m_nibbles.second] != m_nibbles.nn)
                m_pc += 2;
            break;
        case 0x5:
            // Skips next instruction if VX == VY
            if (m_registers[m_nibbles.second] == m_registers[m_nibbles.third])
                m_pc += 2;
            break;
        case 0x6:
            // Set register VX to NN
            m_registers[m_nibbles.second] = m_nibbles.nn;
            break;
        case 0x7:
            // Add the value NN to the register VX
            m_registers[m_nibbles.second] += m_nibbles.nn;
            break;
        case 0x8:
            switch (m_nibbles.n) {
                case 0x0:
                    // Set VX = VY
                    m_registers[m_nibbles.second] = m_registers[m_nibbles.third];
                    break;
                case 0x1:
                    // Set VX = VX OR VY
                    m_registers[m_nibbles.second] |= m_registers[m_nibbles.third];
                    break;
                case 0x2:
                    // Set VX = VX AND VY
                    m_registers[m_nibbles.second] &= m_registers[m_nibbles.third];
                    break;
                case 0x3:
                    // Set VX = VX XOR VY
                    m_registers[m_nibbles.second] ^= m_registers[m_nibbles.third];
                    break;
                case 0x4:
                    // Set VX = VX + VY, Set VF = carry
                    {
                        uint16_t result = m_registers[m_nibbles.second] + m_registers[m_nibbles.third];
                        m_registers[m_nibbles.second] = (result & 0xFF);

                        if (result > 0xFF)
                            m_registers[0xF] = 0x1;
                        else
                            m_registers[0xF] = 0x0;

                        break;
                    }
                case 0x5:
                    // Set VX = VX - VY, Set VF = NOT borrow
                    if (m_registers[m_nibbles.second] > m_registers[m_nibbles.third])
                        m_registers[0xF] = 1;
                    else
                        m_registers[0xF] = 0;

                    m_registers[m_nibbles.second] -= m_registers[m_nibbles.third];

                    break;
                case 0x6:
                    // Set VX = VX SHR 1
                    // TODO: Add config for supporting old behavior of this instruction
                    {
                        uint8_t lsb = m_registers[m_nibbles.second] & 1;
                        if (lsb == 1)
                            m_registers[0xF] = 1;
                        else
                            m_registers[0xF] = 0;

                        m_registers[m_nibbles.second] >>= 1;

                        break;
                    }
                case 0x7:
                    // Set VX = VY - VX, Set VF = NOT borrow
                    if (m_registers[m_nibbles.third] > m_registers[m_nibbles.second])
                        m_registers[0xF] = 1;
                    else
                        m_registers[0xF] = 0;

                    m_registers[m_nibbles.second] = m_registers[m_nibbles.third] - m_registers[m_nibbles.second];
                    
                    break;
                case 0xE:
                    // Set VX = VX SHL 1
                    {
                        uint8_t msb = 0;
                        uint8_t vx = m_registers[m_nibbles.second];
                        while (vx) {
                            msb = vx;
                            vx >>= 1;
                        }

                        if (msb == 0x1)
                            m_registers[0xF] = 1;
                        else
                            m_registers[0xF] = 0;

                        m_registers[m_nibbles.second] <<= 1;

                        break;
                    }
            }
            break;
        case 0x9:
            // Skips next instruction if VX != VY
            if (m_registers[m_nibbles.second] != m_registers[m_nibbles.third])
                m_pc += 2;
            break;
        case 0xA:
            // Set index register to NNN
            m_I = m_nibbles.nnn;
            break;
        case 0xB:
            // Jump to location NNN + V0
            m_pc = m_nibbles.nnn + m_registers[0x0];
            break;
        case 0xC:
            // Set VX = random byte AND NN
            m_registers[m_nibbles.second] = (rand() % 256) & m_nibbles.nn;
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
        case 0xF:
            switch (m_nibbles.nn) {
                case 0x07:
                    // Set VX = Delay timer value
                    m_registers[m_nibbles.second] = m_delay_timer;
                    break;
                case 0x0A:
                    // Wait for key press and store the key value in VX
                    // TODO: Implement instruction
                    break;
                case 0x15:
                    // Set delay timer = VX
                    m_delay_timer = m_registers[m_nibbles.second];
                    break;
                case 0x18:
                    // Set sound timer = VX
                    m_sound_timer = m_registers[m_nibbles.second];
                    break;
                case 0x1E:
                    // Set I = I + VX
                    m_I += m_registers[m_nibbles.second];
                    break;
                case 0x29:
                    // Set I = location of sprite for digit VX
                    // FIXME: Not sure this is the correct implementation of this instruction
                    m_I = m_registers[m_nibbles.second];
                    break;
                case 0x33:
                    // Store BCD representation of VX in memory locations I, I+1 and I+2
                    // TODO: Implement instruction
                    break;
                case 0x55:
                    printf("Opcode => %X \n", m_opcode);
                    printf("X value: %x \n", m_nibbles.second);
                    for (auto i = 0; i <= m_nibbles.second; i++) {
                        m_memory[m_I + i] = m_registers[i];
                        printf("Mem at %X is %X \n", m_I + i, m_memory[m_I + i]);
                        printf("Register value: %X \n", m_registers[i]);
                    }
                    break;
                case 0x65:
                    // Read registers
                    for (auto i = 0; i <= m_nibbles.second; i++) {
                        m_registers[i] = m_memory[m_I + i];
                    }
                    break;
            }
            break;
    }
}

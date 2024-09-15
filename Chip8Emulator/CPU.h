#pragma once
#include <cstdint>
#include <vector>

class chip8Cpu
{

private:
    std::uint16_t index_r; //index register, only 12 bits used
    std::uint16_t stack_r; //stack register
    std::uint8_t delay_r; //delay timer register
    std::uint8_t sound_r; //sound timer register
    std::uint8_t Vx_r[16] = {}; //general purpose registers

    std::vector<std::uint8_t>& chip8_ram;
    std::uint32_t display[64] = { 0 }; //display pixel array
    //pointer to RAM
public:
    void execute() {

    }

    chip8Cpu(std::vector<std::uint8_t>& ram)
        : chip8_ram{ ram }
    {
    }
};
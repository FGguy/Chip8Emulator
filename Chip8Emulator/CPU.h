#pragma once
#include <cstdint>
#include <vector>
#include <SDL.h>
#include <windows.h>
class CPU
{
private:
    std::uint16_t pc_r; //program counter register, points at current instruction in ram
    std::uint16_t index_r; //index register, only 12 bits used
    std::uint16_t stack_r[16]; //stack register
    std::uint8_t delay_r; //delay timer register
    std::uint8_t sound_r; //sound timer register
    std::uint8_t Vx_r[16]; //general purpose registers

    std::vector<std::uint8_t>& chip8_ram;
    std::uint64_t display[32]; //display pixel array
    //pointer to RAM
    SDL_Window* c8Window;
    SDL_Renderer* c8Renderer;
public:
    int execute();
    void loadFonts();
    void renderScreenBuffer();
    CPU(std::vector<std::uint8_t>& ram);
};


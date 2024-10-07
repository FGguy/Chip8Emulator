#include "CPU.h"
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <SDL.h>
#include <windows.h>

    //need to implement some kind of clock mechanism for timed register, display refresh
    //and instruction execution throttling.

    int CPU::execute() {
        loadFonts();

        //set pc to first instruction in ram


        //start window setup
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) { //add error message if fails
            return 1;
        }

        c8Window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 640, SDL_WINDOW_SHOWN);
        if (!c8Window) {
            return 1;
        }

        c8Renderer = SDL_CreateRenderer(c8Window, -1, SDL_RENDERER_ACCELERATED);
        if (!c8Renderer) {
            return 1;
        }

        //Fetch / decode / execute loop
        pc_r = 200;
        std::uint16_t instruction{ 0 };
        // around 700 instructions per second
        // every second refresh screen and decrement timers
        while (true) {
            // fetch
            instruction = 0;
            instruction = (static_cast<std::uint16_t>(chip8_ram[pc_r + 1]) << 8) | static_cast<std::uint16_t>(chip8_ram[pc_r]);
            pc_r += 2;
            decodeExecuteInstruction(instruction);
        }

        //window cleanup
        SDL_DestroyRenderer(c8Renderer);
        SDL_DestroyWindow(c8Window);
        SDL_Quit();

        return 0;
     }

    void CPU::renderScreenBuffer() {
        //clear screen
        SDL_SetRenderDrawColor(c8Renderer, 0, 0, 0, 255); //black
        SDL_RenderClear(c8Renderer);

        SDL_SetRenderDrawColor(c8Renderer, 255, 255, 255, 255); //white
        SDL_Rect pixel;
        pixel.x = 0;
        pixel.y = 0;
        pixel.h = 20;
        pixel.w = 20;

        //iterate over each bit in buffer
        //if 0 black if 1 then white
        for (int i = 0; i < 32; i++) {
            std::uint64_t pixelRow = display[i];
            for (int j = 0; j < 64; j++) {
                std::uint64_t bit = pixelRow & 1;
                if (bit == 1)
                {
                    SDL_RenderFillRect(c8Renderer, &pixel);
                }
                pixelRow >>= 1;
                pixel.x += 20;
            }
            pixel.x = 0;
            pixel.y += 20;
        }
        SDL_RenderPresent(c8Renderer);
    }

    void CPU::decodeExecuteInstruction(std::uint16_t instruction) {

    }

    void CPU::loadFonts() {
        //0
        chip8_ram[0] = 0xF0;
        chip8_ram[1] = 0x90;
        chip8_ram[2] = 0x90;
        chip8_ram[3] = 0x90;
        chip8_ram[4] = 0xF0;

        //1
        chip8_ram[5] = 0x20;
        chip8_ram[6] = 0x60;
        chip8_ram[7] = 0x20;
        chip8_ram[8] = 0x20;
        chip8_ram[9] = 0x70;

        //2
        chip8_ram[10] = 0xF0;
        chip8_ram[11] = 0x10;
        chip8_ram[12] = 0xF0;
        chip8_ram[13] = 0x80;
        chip8_ram[14] = 0xF0;

        //3
        chip8_ram[15] = 0xF0;
        chip8_ram[16] = 0x10;
        chip8_ram[17] = 0xF0;
        chip8_ram[18] = 0x10;
        chip8_ram[19] = 0xF0;

        //4
        chip8_ram[20] = 0x90;
        chip8_ram[21] = 0x90;
        chip8_ram[22] = 0xF0;
        chip8_ram[23] = 0x10;
        chip8_ram[24] = 0x10;

        //5
        chip8_ram[25] = 0xF0;
        chip8_ram[26] = 0x80;
        chip8_ram[27] = 0xF0;
        chip8_ram[28] = 0x10;
        chip8_ram[29] = 0xF0;

        //6
        chip8_ram[30] = 0xF0;
        chip8_ram[31] = 0x80;
        chip8_ram[32] = 0xF0;
        chip8_ram[33] = 0x90;
        chip8_ram[34] = 0xF0;

        //7
        chip8_ram[35] = 0xF0;
        chip8_ram[36] = 0x10;
        chip8_ram[37] = 0x20;
        chip8_ram[38] = 0x40;
        chip8_ram[39] = 0x40;

        //8
        chip8_ram[40] = 0xF0;
        chip8_ram[41] = 0x90;
        chip8_ram[42] = 0xF0;
        chip8_ram[43] = 0x90;
        chip8_ram[44] = 0xF0;

        //9
        chip8_ram[45] = 0xF0;
        chip8_ram[46] = 0x90;
        chip8_ram[47] = 0xF0;
        chip8_ram[48] = 0x10;
        chip8_ram[49] = 0xF0;

        //A
        chip8_ram[50] = 0xF0;
        chip8_ram[51] = 0x90;
        chip8_ram[52] = 0xF0;
        chip8_ram[53] = 0x90;
        chip8_ram[54] = 0x90;

        //B
        chip8_ram[55] = 0xE0;
        chip8_ram[56] = 0x90;
        chip8_ram[57] = 0xE0;
        chip8_ram[58] = 0x90;
        chip8_ram[59] = 0xE0;

        //C
        chip8_ram[60] = 0xF0;
        chip8_ram[61] = 0x80;
        chip8_ram[62] = 0x80;
        chip8_ram[63] = 0x80;
        chip8_ram[64] = 0xF0;

        //D -
        chip8_ram[65] = 0xE0;
        chip8_ram[66] = 0x90;
        chip8_ram[67] = 0x90;
        chip8_ram[68] = 0x90;
        chip8_ram[69] = 0xE0;

        //E
        chip8_ram[70] = 0xF0;
        chip8_ram[71] = 0x80;
        chip8_ram[72] = 0xF0;
        chip8_ram[73] = 0x80;
        chip8_ram[74] = 0xF0;

        //F
        chip8_ram[75] = 0xF0;
        chip8_ram[76] = 0x80;
        chip8_ram[77] = 0xF0;
        chip8_ram[78] = 0x80;
        chip8_ram[79] = 0x80;
    }

    CPU::CPU(std::vector<std::uint8_t> &ram)
        :
        chip8_ram{ ram }, 
        pc_r{ 0 }, 
        index_r{ 0 }, 
        stack_r{}, 
        delay_r{ 0 }, 
        sound_r{ 0 }, 
        Vx_r{}, 
        display{},
        c8Window{nullptr},
        c8Renderer{nullptr}
    {
    }

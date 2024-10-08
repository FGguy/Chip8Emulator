#include "CPU.h"
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <SDL.h>
#include <windows.h>
#include <chrono>

    //need to implement some kind of clock mechanism for timed register, display refresh
    //and instruction execution throttling.

    int CPU::execute() {
        loadFonts();

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

        //might be buggy
        const int targetFPS = 60; // Target frames per second
        const std::chrono::milliseconds frameDuration(1000 / targetFPS);
        const int targetIPS = 700; 
        const std::chrono::milliseconds InstructionDuration(2);

        auto lastCallTimeFrames = std::chrono::steady_clock::now(); 
        auto lastCallTimeInstructions = std::chrono::steady_clock::now();
        while (true) {
            // fetch
            instruction = 0;
            instruction = (static_cast<std::uint16_t>(chip8_ram[pc_r + 1]) << 8) | static_cast<std::uint16_t>(chip8_ram[pc_r]);
            pc_r += 2;

            auto currentTime = std::chrono::steady_clock::now(); // Get current time
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCallTimeInstructions);

            if (elapsed >= InstructionDuration) {
                decodeExecuteInstruction(instruction);
                lastCallTimeInstructions = currentTime;
            }

            currentTime = std::chrono::steady_clock::now(); // Get current time
            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCallTimeFrames);

            if (elapsed >= frameDuration) {
                renderScreenBuffer();
                if( delay_r > 0 ){
                    //do a thing
                    delay_r--;
                }
                if (sound_r > 0) {
                    //do a thing
                    sound_r--;
                }
                lastCallTimeFrames = currentTime;
            }
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
        std::uint8_t nibble4 = instruction & 0b1111;
        std::uint8_t nibble3 = (instruction >> 4) & 0b1111;
        std::uint8_t nibble2 = (instruction >> 8) & 0b1111;
        std::uint8_t nibble1 = (instruction >> 12) & 0b1111;

        switch (instruction) {
        case 0x00E0: //clear screen
            return;
            break;
        case 0x00EE: //return from subroutine
            return;
            break;
        }

        switch (nibble1) {
        case 0x1: //jump to
            break;
        case 0x2: //call subroutine
            break;
        case 0x3: //conditional skip
            break;
        case 0x4: //conditional skip
            break;
        case 0x5: //conditional skip
            break;
        case 0x9: //conditional skip
            break;
        case 0x6: //Set
            break;
        case 0x7: //Add
            break;
        case 0x8:
            switch (nibble4) {
            case 0x0: //Set
                break;
            case 0x1: //OR
                break;
            case 0x2: //AND
                break;
            case 0x3: //XOR
                break;
            case 0x4: //Add
                break;
            case 0x5: //Subtract
                break;
            case 0x7: //Subtract
                break;
            case 0x6: //Shift
                break;
            case 0xE: //Shift
                break;
            }
            break;
        case 0xA: //Set index
            break;
        case 0xB: //jump with offset
            break;
        case 0xC: //Random
            break;
        case 0xD: //Display
            break;
        case 0xE: 
            if (nibble3 == 0x9 && nibble4 == 0xE) { //skip if key

            }
            else if (nibble3 == 0xA && nibble4 == 0x1) { //skip if key

            }
            break;
        case 0xF:
            if (nibble3 == 0x0 && nibble4 == 0x7) { //Timer

            }
            else if (nibble3 == 0x1 && nibble4 == 0x5) { //Timer

            }
            else if (nibble3 == 0x1 && nibble4 == 0x8) { //Timer

            }
            else if (nibble3 == 0x1 && nibble4 == 0xE) { //Add to index

            }
            else if (nibble3 == 0x0 && nibble4 == 0xA) { //Get key

            }
            else if (nibble3 == 0x2 && nibble4 == 0x9) { //Font Character

            }
            else if (nibble3 == 0x3 && nibble4 == 0x3) { //Binary-coded decimal conversion

            }
            else if (nibble3 == 0x5 && nibble4 == 0x5) { //Store and load memory

            }
            else if (nibble3 == 0x6 && nibble4 == 0x5) { //Store and load memory

            }
            break;
        }
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

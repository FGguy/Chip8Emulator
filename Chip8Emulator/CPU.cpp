#include "CPU.h"
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <SDL.h>
#include <windows.h>
#include <chrono>
#include <limits>
#include <iomanip>
#include <fstream>

    int CPU::execute() {
        loadFonts();

        //std::ofstream debugfile;
        //debugfile.open("C:/Users/Yan/Desktop/output.txt");

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
        pc_r = 0x200;
        std::uint16_t instruction{ 0 };

        //might be buggy
        const int targetFPS = 60; // Target frames per second
        const std::chrono::milliseconds frameDuration(1000 / targetFPS);
        const int targetIPS = 700; 
        const std::chrono::milliseconds InstructionDuration(2);

        auto lastCallTimeFrames = std::chrono::steady_clock::now(); 
        auto lastCallTimeInstructions = std::chrono::steady_clock::now();
        int balls{ 0 };

        //for (int i = 0x200; i < 4094; i+=2 ) {
        //    debugfile << "address: " << i << ": " << std::endl;
        //    debugfile << " - " << std::setfill('0') << std::setw(sizeof(std::uint16_t) * 2) << std::hex << ((static_cast<std::uint16_t>(chip8_ram[i]) << 8) | static_cast<std::uint16_t>(chip8_ram[i + 1])) << std::endl;
        //}

        while (pc_r < 4094) {
            // fetch
            instruction = 0;
            instruction = (static_cast<std::uint16_t>(chip8_ram[pc_r]) << 8) | static_cast<std::uint16_t>(chip8_ram[pc_r + 1]);
            pc_r += 2;

            //debugfile << "address: " << pc_r - 2 << ": " << std::endl;
            //debugfile << " - " << std::setfill('0') << std::setw(sizeof(std::uint16_t) * 2) << std::hex << instruction << std::endl;

            if (instruction == 0) {
                break;
            }

            //std::cout << std::hex << instruction << std::endl;

            //auto currentTime = std::chrono::steady_clock::now(); // Get current time
            //auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCallTimeInstructions);

            //if (elapsed >= InstructionDuration) {
               decodeExecuteInstruction(instruction);
                //lastCallTimeInstructions = currentTime;
            //}

            //currentTime = std::chrono::steady_clock::now(); // Get current time
           // elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCallTimeFrames);

            //if (elapsed >= frameDuration) {
                renderScreenBuffer();
            //    if( delay_r > 0 ){
            //        delay_r--;
            //    }
            //    if (sound_r > 0) {
            //        sound_r--;
            //    }
            //    lastCallTimeFrames = currentTime;
            //}
                //Sleep(500);
        }

        Sleep(10000);

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

        uint64_t bitmask = 0x8000000000000000u;

        for (int i = 0; i < 32; i++) {
            std::uint64_t pixelRow { display[i] };
            for (int j = 0; j < 64; j++) {
                std::uint64_t bit = pixelRow & bitmask;
                if (bit != 0)
                {
                    SDL_RenderFillRect(c8Renderer, &pixel);
                }
                pixelRow <<= 1;
                pixel.x += 20;
            }
            pixel.x = 0;
            pixel.y += 20;
        }
        SDL_RenderPresent(c8Renderer);
    }

    void CPU::decodeExecuteInstruction(std::uint16_t instruction) {
        std::uint8_t nibble4 = instruction & 0b1111u;
        std::uint8_t nibble3 = (instruction >> 4) & 0b1111u;
        std::uint8_t nibble2 = (instruction >> 8) & 0b1111u;
        std::uint8_t nibble1 = (instruction >> 12) & 0b1111u;

        switch (instruction) {
        case 0x00E0: //clear screen 
            memset(display, 0, sizeof(display));
            return;
            break;
        case 0x00EE: //return from subroutine 
            pc_r = stack_r->top();
            stack_r->pop();
            return;
            break;
        }

        std::uint8_t Vx = 0;
        std::uint8_t Vy = 0;
        std::uint8_t Nn = 0;
        std::uint8_t x_coordinate = 0;
        std::uint8_t y_coordinate = 0;

        switch (nibble1) {

        case 0x1: //jump to 
            pc_r = instruction & 0x0FFFu;
            break;

        case 0x2: //call subroutine 
            stack_r->push(pc_r);
            pc_r = instruction & 0x0FFFu;
            break;

        case 0x3: //conditional skip 
            Vx = Vx_r[nibble2];
            Nn = instruction & 0x00FFu;
            if (Vx == Nn){
                pc_r += 2;
            }
            break;

        case 0x4: //conditional skip 
            Vx = Vx_r[nibble2];
            Nn = instruction & 0x00FFu;
            if (Vx != Nn) {
                pc_r += 2;
            }
            break;

        case 0x5: //conditional skip 
            Vx = Vx_r[nibble2];
            Vy = Vx_r[nibble3];
            if (Vx == Vy) {
                pc_r += 2;
            }
            break;

        case 0x9: //conditional skip 
            Vx = Vx_r[nibble2];
            Vy = Vx_r[nibble3];
            if (Vx != Vy) {
                pc_r += 2;
            }
            break;

        case 0x6: //Set 
            Vx_r[nibble2] = instruction & 0x00FFu;
            break;

        case 0x7: //Add 
            Vx_r[nibble2] += instruction & 0x00FFu;
            break;

        case 0x8:
            switch (nibble4) {
            case 0x0: //Set
                Vx_r[nibble2] = Vx_r[nibble3];
                break;

            case 0x1: //OR
                Vx_r[nibble2] = Vx_r[nibble2] | Vx_r[nibble3];
                break;

            case 0x2: //AND
                Vx_r[nibble2] = Vx_r[nibble2] & Vx_r[nibble3];
                break;

            case 0x3: //XOR
                Vx_r[nibble2] = Vx_r[nibble2] ^ Vx_r[nibble3];
                break;

            case 0x4: //Add
                if ((0xFF - Vx_r[nibble2]) < Vx_r[nibble3]) {
                    Vx_r[0xF] = 1;
                }
                Vx_r[nibble2] = Vx_r[nibble2] + Vx_r[nibble3];
                break;

            case 0x5: //Subtract
                if (Vx_r[nibble2] > Vx_r[nibble3]) {
                    Vx_r[0xF] = 1;
                }
                else {
                    Vx_r[0xF] = 0;
                }
                Vx_r[nibble2] = Vx_r[nibble2] - Vx_r[nibble3];
                break;

            case 0x7: //Subtract
                if (Vx_r[nibble2] > Vx_r[nibble3]) {
                    Vx_r[0xF] = 0;
                }
                else {
                    Vx_r[0xF] = 1;
                }
                Vx_r[nibble2] = Vx_r[nibble3] - Vx_r[nibble2];
                break;

            case 0x6: //Shift
                if ((Vx_r[nibble3] & 1) == 1) {
                    Vx_r[0xF] = 1;
                }
                else {
                    Vx_r[0xF] = 0;
                }
                Vx_r[nibble2] = Vx_r[nibble3] >> 1;
                break;

            case 0xE: //Shift
                if ((Vx_r[nibble3] & 0x80) == 0x80) {
                    Vx_r[0xF] = 1;
                }
                else {
                    Vx_r[0xF] = 0;
                }
                Vx_r[nibble2] = Vx_r[nibble3] << 1;
                break;

            }
            break;

        case 0xA: //Set index
            index_r = instruction & 0x0FFFu;
            break;

        case 0xB: //jump with offset
            pc_r = (instruction & 0x0FFFu) + Vx_r[0x0];
            break;

        case 0xC: //Random
            Vx_r[nibble2] = (rand() % 256) & (instruction & 0x00FFu);
            break;

        case 0xD: //Display
            x_coordinate = Vx_r[nibble2] % 64;
            y_coordinate = Vx_r[nibble3] % 32;
            Vx_r[0xF] = 0;
            for (unsigned int row = 0; row < nibble4; ++row) {
                std::uint8_t row_pixels = chip8_ram[index_r + row];
                //std::cout << static_cast<int>(row_pixels) << std::endl;
                for(int col = 0; col < 8; col++) {
                    //from left to right, xor with each pixel in display, if both bits are 1 set to 0 and set Vf to 1
                    std::uint64_t pixel = 0;
                    pixel = (row_pixels & 0b10000000u) ? 1u : 0u;
                    row_pixels = row_pixels << 1;

                    //bitshift by x coordinate
                    pixel = pixel << (63 - x_coordinate);

                    //check for Vf register 
                    if ((display[y_coordinate] & pixel) != 0) {
                        Vx_r[0xF] = 1;
                    }

                    display[y_coordinate] = display[y_coordinate] ^ pixel;

                    x_coordinate++;
                    if (x_coordinate > 63) {
                        break;
                    }
                }
                x_coordinate = Vx_r[nibble2] % 64;
                y_coordinate++;
                if (y_coordinate > 31) {
                    break;
                }
            }
            break;

        case 0xE: 
            if (nibble3 == 0x9 && nibble4 == 0xE) { //skip if key IO TODO
                std::uint8_t keypress = checkInput();
                if (keypress != 16u) {
                    if (Vx_r[nibble2] == keypress) {
                        pc_r += 2;
                    }
                }
            }
            else if (nibble3 == 0xA && nibble4 == 0x1) { //skip if key IO TODO
                std::uint8_t keypress = checkInput();
                if (keypress != 16u) {
                    if (Vx_r[nibble2] != keypress) {
                        pc_r += 2;
                    }
                }
            }
            break;

        case 0xF:
            if (nibble3 == 0x0 && nibble4 == 0x7) { //Timer
                Vx_r[nibble2] = delay_r;
            }
            else if (nibble3 == 0x1 && nibble4 == 0x5) { //Timer
                delay_r = Vx_r[nibble2];
            }
            else if (nibble3 == 0x1 && nibble4 == 0x8) { //Timer
                sound_r = Vx_r[nibble2];
            }
            else if (nibble3 == 0x1 && nibble4 == 0xE) { //Add to index
                index_r += Vx_r[nibble2];
            }
            else if (nibble3 == 0x0 && nibble4 == 0xA) { //Get key IO 
                std::uint8_t keypress = checkInput();
                if (keypress == 16u) {
                    pc_r -= 2;
                }
                else {
                    Vx_r[nibble2] = keypress;
                }
            }
            else if (nibble3 == 0x2 && nibble4 == 0x9) { //Font Character
                index_r = nibble2 * 5;
            }
            else if (nibble3 == 0x3 && nibble4 == 0x3) { //Binary-coded decimal conversion 
                chip8_ram[index_r] = (Vx_r[nibble2]% 1000) / 100;
                chip8_ram[index_r + 1] = (Vx_r[nibble2] % 100) /10;
                chip8_ram[index_r + 2] = Vx_r[nibble2] % 10;
            }
            else if (nibble3 == 0x5 && nibble4 == 0x5) { //Store and load memory
                for (int reg = 0; reg < 15; reg++) {
                    chip8_ram[index_r + reg] = Vx_r[reg];
                }
            }
            else if (nibble3 == 0x6 && nibble4 == 0x5) { //Store and load memory
                for (int reg = 0; reg < 15; reg++) {
                    Vx_r[reg] = chip8_ram[index_r + reg];
                }
            }
            break;
        }
    }

    std::uint8_t CPU::checkInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_0:
                    return 0x0u;
                    break;
                case SDLK_1:
                    return 0x1u;
                    break;
                case SDLK_2:
                    return 0x2u;
                    break;
                case SDLK_3:
                    return 0x3u;
                    break;
                case SDLK_4:
                    return 0x4u;
                    break;
                case SDLK_5:
                    return 0x5u;
                    break;
                case SDLK_6:
                    return 0x6u;
                    break;
                case SDLK_7:
                    return 0x7u;
                    break;
                case SDLK_8:
                    return 0x8u;
                    break;
                case SDLK_9:
                    return 0x9u;
                    break;
                case SDLK_a:
                    return 0xAu;
                    break;
                case SDLK_b:
                    return 0xBu;
                    break;
                case SDLK_c:
                    return 0xCu;
                    break;
                case SDLK_d:
                    return 0xDu;
                    break;
                case SDLK_e:
                    return 0xEu;
                    break;
                case SDLK_f:
                    return 0xFu;
                    break;
                }
                break;
            default:
                break;
            }
            return 16u;
        }
    }

    void CPU::loadFonts() {
        //0
        chip8_ram[0] = 0xF0u;
        chip8_ram[1] = 0x90u;
        chip8_ram[2] = 0x90u;
        chip8_ram[3] = 0x90u;
        chip8_ram[4] = 0xF0u;

        //1
        chip8_ram[5] = 0x20u;
        chip8_ram[6] = 0x60u;
        chip8_ram[7] = 0x20u;
        chip8_ram[8] = 0x20u;
        chip8_ram[9] = 0x70u;

        //2
        chip8_ram[10] = 0xF0u;
        chip8_ram[11] = 0x10u;
        chip8_ram[12] = 0xF0u;
        chip8_ram[13] = 0x80u;
        chip8_ram[14] = 0xF0u;

        //3
        chip8_ram[15] = 0xF0u;
        chip8_ram[16] = 0x10u;
        chip8_ram[17] = 0xF0u;
        chip8_ram[18] = 0x10u;
        chip8_ram[19] = 0xF0u;

        //4
        chip8_ram[20] = 0x90u;
        chip8_ram[21] = 0x90u;
        chip8_ram[22] = 0xF0u;
        chip8_ram[23] = 0x10u;
        chip8_ram[24] = 0x10u;

        //5
        chip8_ram[25] = 0xF0u;
        chip8_ram[26] = 0x80u;
        chip8_ram[27] = 0xF0u;
        chip8_ram[28] = 0x10u;
        chip8_ram[29] = 0xF0u;

        //6
        chip8_ram[30] = 0xF0u;
        chip8_ram[31] = 0x80u;
        chip8_ram[32] = 0xF0u;
        chip8_ram[33] = 0x90u;
        chip8_ram[34] = 0xF0u;

        //7
        chip8_ram[35] = 0xF0u;
        chip8_ram[36] = 0x10u;
        chip8_ram[37] = 0x20u;
        chip8_ram[38] = 0x40u;
        chip8_ram[39] = 0x40u;

        //8
        chip8_ram[40] = 0xF0u;
        chip8_ram[41] = 0x90u;
        chip8_ram[42] = 0xF0u;
        chip8_ram[43] = 0x90u;
        chip8_ram[44] = 0xF0u;

        //9
        chip8_ram[45] = 0xF0u;
        chip8_ram[46] = 0x90u;
        chip8_ram[47] = 0xF0u;
        chip8_ram[48] = 0x10u;
        chip8_ram[49] = 0xF0u;

        //A
        chip8_ram[50] = 0xF0u;
        chip8_ram[51] = 0x90u;
        chip8_ram[52] = 0xF0u;
        chip8_ram[53] = 0x90u;
        chip8_ram[54] = 0x90u;

        //B
        chip8_ram[55] = 0xE0u;
        chip8_ram[56] = 0x90u;
        chip8_ram[57] = 0xE0u;
        chip8_ram[58] = 0x90u;
        chip8_ram[59] = 0xE0u;

        //C
        chip8_ram[60] = 0xF0u;
        chip8_ram[61] = 0x80u;
        chip8_ram[62] = 0x80u;
        chip8_ram[63] = 0x80u;
        chip8_ram[64] = 0xF0u;

        //D -
        chip8_ram[65] = 0xE0u;
        chip8_ram[66] = 0x90u;
        chip8_ram[67] = 0x90u;
        chip8_ram[68] = 0x90u;
        chip8_ram[69] = 0xE0u;

        //E
        chip8_ram[70] = 0xF0u;
        chip8_ram[71] = 0x80u;
        chip8_ram[72] = 0xF0u;
        chip8_ram[73] = 0x80u;
        chip8_ram[74] = 0xF0u;

        //F
        chip8_ram[75] = 0xF0u;
        chip8_ram[76] = 0x80u;
        chip8_ram[77] = 0xF0u;
        chip8_ram[78] = 0x80u;
        chip8_ram[79] = 0x80u;
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

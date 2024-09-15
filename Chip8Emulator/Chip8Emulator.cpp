// Chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include "CPU.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "No file path provided";
        return 1;
    }

    std::string filePath = argv[1];

    std::ifstream chip8ROM;
    chip8ROM.open(filePath, std::ios_base::binary);

    if (!chip8ROM.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    //initialize ram and load program
    std::vector<std::uint8_t> ram(4096);
    int ram_index{ 200 };

    while (!chip8ROM.eof()) {
        if (ram_index > 4096) {
            std::cerr << "Error: program is too big for memory. ";
            return 1;
        }
        ram[ram_index++] = chip8ROM.get(); //make sure conversion is not causing dataloss
    }
    chip8ROM.close();

    //run program
    chip8Cpu cpu{ ram };
    cpu.execute();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu


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
    int ram_size{ 4096 };
    int ram_index{ 200 };
    std::vector<std::uint8_t> ram(ram_size,0);

    while (!chip8ROM.eof()) {
        if (ram_index > ram_size - 1) {
            std::cerr << "Error: program is too big for memory. ";
            return 1;
        }
        chip8ROM.read(reinterpret_cast<char*>(&ram[ram_index++]),sizeof(std::uint8_t)); 
    }
    chip8ROM.close();

    //run program
    CPU cpu{ ram };
    return cpu.execute();
}


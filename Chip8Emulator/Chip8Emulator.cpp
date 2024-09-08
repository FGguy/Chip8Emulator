// Chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "No file path provided";
        return 1;
    }

    std::string filePath = argv[1];

    std::ifstream chip8ROM;
    chip8ROM.open(filePath);

    if (!chip8ROM.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(chip8ROM, line)) {
        std::cout << line << '\n';
    }
    std::cin >> line;

    chip8ROM.close();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu


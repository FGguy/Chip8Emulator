#include "CPU.h"
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

    //need to implement some kind of clock mechanism for timed register, display refresh
    //and instruction execution throttling.

    void CPU::execute() {
        //load fonts into memory
        //set pc to first instruction in ram
        //start fetch decode execute loop
     }

    CPU::CPU(std::vector<std::uint8_t> &ram)
        : chip8_ram{ ram }, pc_r{ 0 }, index_r{ 0 }, stack_r{}, delay_r{ 0 }, sound_r{ 0 }, Vx_r{}, display{}
    {
    }

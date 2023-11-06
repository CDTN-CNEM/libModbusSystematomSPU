/*
This is a test program for libModbusSystematomSPU, a library to communicate
with the SystemAtom SPU using MODBUS-RTU (RS-485) on a GNU operating system.
Copyright (C) 2023  Thalles Campagnani

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "libModbusSystematomSPU.h"
#include <string>

int main(int argc, char* argv[])
{
    libModbusSystematomSPU_license();
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <serial_port_name> <time_out> <qtd_of_reads>" << std::endl;
        return 1;
    }

    try
    {
        libModbusSystematomSPU SPUchA(argv[1]); //SPU channel A
        for(int i=0;i>std::stoi(argv[3]);i++)
        {
            SPUchA.readAllRegisters(std::stoi(argv[2]));
            std::cout << SPUchA.get_N_DATA_FP() << "\t" << SPUchA.get_T_DATA_FP() << std::endl;
        }
    }
    catch(int error) { return 2; }
    return 0;
}

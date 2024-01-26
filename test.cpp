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
#include <chrono>

std::string getTimeString()
{
    static std::chrono::time_point<std::chrono::high_resolution_clock> tempoInicial = std::chrono::high_resolution_clock::now();

    // Obtém o tempo atual
    std::time_t tempoAtualRaw = std::time(nullptr);
    struct tm* tempoInfo = std::localtime(&tempoAtualRaw);

    // Obtém horas, minutos, segundos e milissegundos
    int horas = tempoInfo->tm_hour;
    int minutos = tempoInfo->tm_min;
    int segundos = tempoInfo->tm_sec;
    std::string millisegundos = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000);

    // Calcula diferença de tempo em microssegundos
    auto tempoAtual = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
    static auto tempoAnterior = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
    auto diferencaMicros = tempoAtual - tempoAnterior;
    tempoAnterior = tempoAtual;

    // Formata a string
    std::string horaFormatada = std::to_string(horas) + ":" + std::to_string(minutos) + ":" + std::to_string(segundos) + "." + millisegundos + "\t [" + std::to_string(diferencaMicros.count()) + "]";

    return horaFormatada;
}

int main(int argc, char* argv[])
{
    libModbusSystematomSPU_license();
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <serial_port_name> <qtd_of_reads>" << std::endl;
        return 1;
    }

    try
    {
        libModbusSystematomSPU SPUchA(argv[1]); //SPU channel A
        for(int i=0;i<std::stoi(argv[2]);i++)
        {
            SPU_DATA data = SPUchA.get_all();
            std::cout << getTimeString() << "\t" << data.N_DATA_FP << std::endl;
        }
    }
    catch(int error) { return 2; }
    return 0;
}

/*
libModbusSystematomSPU is a library to communicate with the SystemAtom SPU
using MODBUS-RTU (RS-485) on a GNU operating system.
Copyright (C) 2023-2024  Thalles Campagnani

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

#ifndef LIB_MODBUS_SYSTEMATOM_SPU
#define LIB_MODBUS_SYSTEMATOM_SPU

#include <libserialport.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdint>

struct SPU_DATA
{
    bool    READ            = 0;
    float   N_DATA_FP       = 0;
    float   T_DATA_FP       = 0;
    float   F1_DATA_FP      = 0;
    float   F2_DATA_FP      = 0;
    float   F3_DATA_FP      = 0;
    float   EMR_N_THRESHOLD = 0;
    float   WRN_N_THRESHOLD = 0;
    float   EMR_T_THRESHOLD = 0;
    float   WRN_T_THRESHOLD = 0;
    bool    EMR_N           = 0;
    bool    WRN_N           = 0;
    bool    EMR_T           = 0;
    bool    WRN_T           = 0;
    bool    R1              = 0;
    bool    R2              = 0;
    bool    R3              = 0;
    bool    RDY             = 0;
    bool    TEST            = 0;
    bool    XXXX            = 0;
};


void libModbusSystematomSPU_license();

class libModbusSystematomSPU {
public:
    libModbusSystematomSPU(std::string portname);
    ~libModbusSystematomSPU();

    bool readAllRegisters(const int readTimeoutMillis);

    std::string  get_portname();

    float   get_N_DATA_FP();
    float   get_T_DATA_FP();
    float   get_F1_DATA_FP();
    float   get_F2_DATA_FP();
    float   get_F3_DATA_FP();
    float   get_EMR_N_THRESHOLD();
    float   get_WRN_N_THRESHOLD();
    float   get_EMR_T_THRESHOLD();
    float   get_WRN_T_THRESHOLD();
    uint8_t get_EMR_N();
    uint8_t get_WRN_N();
    uint8_t get_EMR_T();
    uint8_t get_WRN_T();
    uint8_t get_R1();
    uint8_t get_R2();
    uint8_t get_R3();
    uint8_t get_RDY();
    uint8_t get_TEST();
    uint8_t get_XXXX();
    
    
private:
    const char* portname;
    struct sp_port* port;
    sp_return result;

    float N_DATA_FP;
    float T_DATA_FP;
    float F1_DATA_FP;
    float F2_DATA_FP;
    float F3_DATA_FP;
    float EMR_N_THRESHOLD;
    float WRN_N_THRESHOLD;
    float EMR_T_THRESHOLD;
    float WRN_T_THRESHOLD;
    uint8_t EMR_N;
    uint8_t WRN_N;
    uint8_t EMR_T;
    uint8_t WRN_T;
    uint8_t R1;
    uint8_t R2;
    uint8_t R3;
    uint8_t RDY;
    uint8_t TEST;
    uint8_t XXXX;
    float conv4BytesToFloat(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4);
};

#endif // LIB_MODBUS_SYSTEMATOM_SPU

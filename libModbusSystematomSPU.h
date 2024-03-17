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

#include <modbus/modbus-rtu.h>
#include <modbus/modbus.h>

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdint>

struct SPU_DATA
{
    int   STATE             = -1;   //Status da SPU:
                                    //-1 = Nenhuma tentativa de conexão realizada ainda.
                                    // 0 = Valores lidos com sucesso.
                                    // 1 = Erro ao ler dados.
                                    // 2 = Erro de contexto modbus.
    float   N_DATA_FP       = -1;
    float   T_DATA_FP       = -1;
    float   F1_DATA_FP      = -1;
    float   F2_DATA_FP      = -1;
    float   F3_DATA_FP      = -1;
    float   EMR_N_THRESHOLD = -1;
    float   WRN_N_THRESHOLD = -1;
    float   EMR_T_THRESHOLD = -1;
    float   WRN_T_THRESHOLD = -1;
    bool    EMR_N           =  0;
    bool    WRN_N           =  0;
    bool    EMR_T           =  0;
    bool    WRN_T           =  0;
    bool    R1              =  0;
    bool    R2              =  0;
    bool    R3              =  0;
    bool    RDY             =  0;
    bool    TEST            =  0;
    bool    XXXX            =  0;
};


void libModbusSystematomSPU_license();

class libModbusSystematomSPU {
public:
    libModbusSystematomSPU(std::string portname);
    ~libModbusSystematomSPU();

    bool tryConnect();
    
    //Get the name of the port (who am I?)
    std::string  get_portname();

    //Get all the struct and update all (or part) of the variables
    SPU_DATA get_all             ();
    SPU_DATA get_all_update_NT   ();
    SPU_DATA get_all_update_F    ();
    SPU_DATA get_all_update_NTF  ();
    SPU_DATA get_all_update_bool ();

    //Get just variable
    float get_N_DATA_FP          ();
    float get_T_DATA_FP          ();
    float get_F1_DATA_FP         ();
    float get_F2_DATA_FP         ();
    float get_F3_DATA_FP         ();
    float get_EMR_N_THRESHOLD    ();
    float get_WRN_N_THRESHOLD    ();
    float get_EMR_T_THRESHOLD    ();
    float get_WRN_T_THRESHOLD    ();
    bool  get_EMR_N              ();
    bool  get_WRN_N              ();
    bool  get_EMR_T              ();
    bool  get_WRN_T              ();
    bool  get_R1                 ();
    bool  get_R2                 ();
    bool  get_R3                 ();
    bool  get_RDY                ();
    bool  get_TEST               ();
    bool  get_XXXX               ();    
    
private:
    std::string portname;
    modbus_t* ctx;
    SPU_DATA spuData;
    float conv2RegsToFloat(uint16_t data1, uint16_t data2);
    float get_1_DATA_FP(int start_address);
    uint16_t get_1_DATA(int address);
    std::string stdErrorMsg(std::string functionName, std::string errorMsg, std::string exptionMsg);
};

#endif // LIB_MODBUS_SYSTEMATOM_SPU

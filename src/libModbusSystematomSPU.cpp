/*
libModbusSystematomSPU is a library to communicate with the SystemAtom SPU
using MODBUS-RTU (RS-485) on a GNU operating system.
Copyright (C) 2023-2024 Thalles Campagnani

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

#include <libModbusSystematomSPU.h>

#include <modbus/modbus-rtu.h>
#include <modbus/modbus.h>

struct libModbusSystematomSPU_private {
    std::string portname;
    modbus_t* ctx;
    SPU_DATA spuData;
    bool flagNotConnected;//Devido a um erro na biblioteca ModBus na função modbus_free() que causa falha
    //de segmentação, fez-se necessário criar essa flag para as funções da categoria get_data...() consigam
    //saber que o dispositivo não existe para emitir STATE 2 (desconectado)
};

void libModbusSystematomSPU_license()
{
    std::cout << "libModbusSystematomSPU Copyright (C) 2023-2024 Thalles Campagnani" << std::endl;
    std::cout << "This program comes with ABSOLUTELY NO WARRANTY;" << std::endl;
    std::cout << "This is free software, and you are welcome to redistribute it" << std::endl;
    std::cout << "under certain conditions; For more details read the file LICENSE" << std::endl;
    std::cout << "that came together with the library." << std::endl << std::endl;
}

bool libModbusSystematomSPU::tryConnect()
{
    //Se estiver conectado e existir ctx feche a conexão
    if(!this->_p->flagNotConnected && this->_p->ctx) 
    {
        modbus_close(this->_p->ctx);
        this->_p->flagNotConnected=1;
    }
    
    // Create a new Modbus context
    this->_p->ctx = modbus_new_rtu(this->_p->portname.c_str(), 57600, 'N', 8, 1);

    // Set the slave address
    modbus_set_slave(this->_p->ctx, 0x01);

    if (this->_p->ctx == nullptr || modbus_connect(this->_p->ctx) == -1) 
    {
        std::cerr << stdErrorMsg("tryConnect()","Failed to connect to Modbus device:", modbus_strerror(errno));
        modbus_close(this->_p->ctx);
        //modbus_free(this->_p->ctx); //Bug na biblioteca causa falha de segmentação
        this->_p->flagNotConnected=1;
        return 1;
    }
    else
    {
        std::cout << "Connection successful to: " << get_portname() << std::endl;
        this->_p->flagNotConnected=0;
        return 0;
    } 
}

libModbusSystematomSPU::libModbusSystematomSPU(std::string portname)
{
    this->_p = new libModbusSystematomSPU_private;
    this->_p->portname = portname;
    tryConnect();
}

libModbusSystematomSPU::~libModbusSystematomSPU() {
    // Close the Modbus connection
    if (this->_p->ctx) {
        modbus_close(this->_p->ctx);
        //modbus_free(this->_p->ctx); //Bug na biblioteca causa falha de segmentação
    }
}

std::string  libModbusSystematomSPU::get_portname()    { return this->_p->portname; }

float libModbusSystematomSPU::conv2RegsToFloat(uint16_t data1, uint16_t data2)
{
    uint16_t data[2] = {data2, data1};
    float* floatValue = reinterpret_cast<float*>(data);
    return *floatValue;
}

/*
////// FLOAT32 ////////////////

0x0001 N_DATA_FP
0x0002

0x0003 T_DATA_FP
0x0004

0x0005 F1_DATA_FP
0x0006

0x0007 F2_DATA_FP
0x0008

0x0009 F3_DATA_FP
0x000A

0x000B EMR_N_THRESHOLD
0x000C

0x000D WRN_N_THRESHOLD
0x000E

0x000F EMR_T_THRESHOLD
0x0010

0x0011 WRN_T_THRESHOLD
0x0012

////// INT16 ////////////////

0x0064  EMR_N
0x0065  WRN_N
0x0066  EMR_T
0x0067  WRN_T
0x0068  R1
0x0069  R2
0x006A  R3
0x006B  RDY
0x006C  TEST
0x006D  ?
*/

std::string libModbusSystematomSPU::stdErrorMsg(std::string functionName, std::string errorMsg, std::string exptionMsg)
{
    std::string msg =   "ERROR in libModbusSystematomSPU::" + 
                        functionName +
                        " at " + 
                        get_portname() + 
                        "\n\tError type: " + 
                        errorMsg;
    if (exptionMsg!="") 
    {
        msg +=          "\n\tError code: " + 
                        exptionMsg;
    }
    msg += "\n";
    return msg;
}

SPU_DATA libModbusSystematomSPU::get_all()
{
    // Check if the Modbus context exists
    if (!this->_p->ctx || this->_p->flagNotConnected) {
        std::cerr << stdErrorMsg("get_all()","Modbus context does not exist","");
        this->_p->spuData.STATE = 2;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Read from the register 0x0001 to 0x0012
    int start_address =          0x0001; // Starting address
    int   end_address =          0x0012; // Ending   address
    int num_registers = 1 + end_address - start_address; // Number of registers to read
    uint16_t data[num_registers];
    int result = modbus_read_registers(this->_p->ctx, start_address, num_registers, data);

    if (result == -1) {
        std::cerr << stdErrorMsg("get_all()","Failed to read data",modbus_strerror(errno));
        this->_p->spuData.STATE = 1;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Convert data in floats variables
    this->_p->spuData.N_DATA_FP       = conv2RegsToFloat(data[ 0], data[ 1]);
    this->_p->spuData.T_DATA_FP       = conv2RegsToFloat(data[ 2], data[ 3]);
    this->_p->spuData.F1_DATA_FP      = conv2RegsToFloat(data[ 4], data[ 5]);
    this->_p->spuData.F2_DATA_FP      = conv2RegsToFloat(data[ 6], data[ 7]);
    this->_p->spuData.F3_DATA_FP      = conv2RegsToFloat(data[ 8], data[ 9]);
    this->_p->spuData.EMR_N_THRESHOLD = conv2RegsToFloat(data[10], data[11]);
    this->_p->spuData.WRN_N_THRESHOLD = conv2RegsToFloat(data[12], data[13]);
    this->_p->spuData.EMR_T_THRESHOLD = conv2RegsToFloat(data[14], data[15]);
    this->_p->spuData.WRN_T_THRESHOLD = conv2RegsToFloat(data[16], data[17]);


    // Read from the register 0x0064 to 0x006D
    start_address =          0x0064; // Starting address
    end_address   =          0x006D; // Ending   address
    num_registers = 1 + end_address - start_address; // Number of registers to read
    result = modbus_read_registers(this->_p->ctx, start_address, num_registers, data);

    if (result == -1) {
        std::cerr << stdErrorMsg("get_all()","Failed to read data",modbus_strerror(errno));
        this->_p->spuData.STATE = 1;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Convert data in floats variables
    this->_p->spuData.EMR_N           = data[0];
    this->_p->spuData.WRN_N           = data[1];
    this->_p->spuData.EMR_T           = data[2];
    this->_p->spuData.WRN_T           = data[3];
    this->_p->spuData.R1              = data[4];
    this->_p->spuData.R2              = data[5];
    this->_p->spuData.R3              = data[6];
    this->_p->spuData.RDY             = data[7];
    this->_p->spuData.TEST            = data[8];
    this->_p->spuData.XXXX            = data[9];

    this->_p->spuData.STATE           = 0;
    this->_p->spuData.TIME = std::chrono::system_clock::now();
    return this->_p->spuData;
}

SPU_DATA libModbusSystematomSPU::get_all_update_NT()
{
    // Check if the Modbus context exists
    if (!this->_p->ctx || this->_p->flagNotConnected) {
        std::cerr << stdErrorMsg("get_all_update_NT()","Modbus context does not exist","");
        this->_p->spuData.STATE = 2;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Read from the register 0x0001 to 0x006D
    int num_registers = 0x04; // Number of registers to read
    int start_address = 0x01; // Starting address
    const int dataSize = 1 + num_registers - start_address; // Total number of bytes to read
    uint16_t data[dataSize];

    int result = modbus_read_registers(this->_p->ctx, start_address, num_registers, data);

    if (result == -1) {
        std::cerr << stdErrorMsg("get_all_update_NT()","Failed to read data",modbus_strerror(errno));
        this->_p->spuData.STATE = 1;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Convert data in floats variables
    this->_p->spuData.N_DATA_FP       = conv2RegsToFloat(data[ 0], data[ 1]);
    this->_p->spuData.T_DATA_FP       = conv2RegsToFloat(data[ 2], data[ 3]);
    this->_p->spuData.STATE           = 0;
    this->_p->spuData.TIME = std::chrono::system_clock::now();
    return this->_p->spuData;
}

SPU_DATA libModbusSystematomSPU::get_all_update_NTF()
{
    // Check if the Modbus context exists
    if (!this->_p->ctx || this->_p->flagNotConnected) {
        std::cerr << stdErrorMsg("get_all_update_NTF()","Modbus context does not exist","");
        this->_p->spuData.STATE = 2;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Read from the register 0x0001 to 0x006D
    int num_registers = 0x0A; // Number of registers to read
    int start_address = 0x01; // Starting address
    const int dataSize = 1 + num_registers - start_address; // Total number of bytes to read
    uint16_t data[dataSize];

    int result = modbus_read_registers(this->_p->ctx, start_address, num_registers, data);

    if (result == -1) {
        std::cerr << stdErrorMsg("get_all_update_NTF()","Failed to read data",modbus_strerror(errno));
        this->_p->spuData.STATE = 1;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Convert data in floats variables
    this->_p->spuData.N_DATA_FP       = conv2RegsToFloat(data[ 0], data[ 1]);
    this->_p->spuData.T_DATA_FP       = conv2RegsToFloat(data[ 2], data[ 3]);
    this->_p->spuData.F1_DATA_FP      = conv2RegsToFloat(data[ 4], data[ 5]);
    this->_p->spuData.F2_DATA_FP      = conv2RegsToFloat(data[ 6], data[ 7]);
    this->_p->spuData.F3_DATA_FP      = conv2RegsToFloat(data[ 8], data[ 9]);
    this->_p->spuData.STATE           = 0;
    this->_p->spuData.TIME = std::chrono::system_clock::now();
    return this->_p->spuData;
}

SPU_DATA libModbusSystematomSPU::get_all_update_F()
{
    // Check if the Modbus context exists
    if (!this->_p->ctx || this->_p->flagNotConnected) {
        std::cerr << stdErrorMsg("get_all_update_F()","Modbus context does not exist","");
        this->_p->spuData.STATE = 2;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Read from the register 0x0001 to 0x006D
    int num_registers = 0x0A-0x04; // Number of registers to read
    int start_address = 0x05; // Starting address
    const int dataSize = 1 + num_registers - start_address; // Total number of bytes to read
    uint16_t data[dataSize];

    int result = modbus_read_registers(this->_p->ctx, start_address, num_registers, data);

    if (result == -1) {
        std::cerr << stdErrorMsg("get_all_update_F()","Failed to read data",modbus_strerror(errno));
        this->_p->spuData.STATE = 1;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Convert data in floats variables
    this->_p->spuData.F1_DATA_FP      = conv2RegsToFloat(data[ 4], data[ 5]);
    this->_p->spuData.F2_DATA_FP      = conv2RegsToFloat(data[ 6], data[ 7]);
    this->_p->spuData.F3_DATA_FP      = conv2RegsToFloat(data[ 8], data[ 9]);
    this->_p->spuData.STATE           = 0;
    this->_p->spuData.TIME = std::chrono::system_clock::now();
    return this->_p->spuData;
}

 SPU_DATA libModbusSystematomSPU::get_all_update_bool ()
 {
    // Check if the Modbus context exists
    if (!this->_p->ctx || this->_p->flagNotConnected) {
        std::cerr << stdErrorMsg("get_all_update_bool()","Modbus context does not exist","");
        this->_p->spuData.STATE = 2;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Read from the register 0x0001 to 0x006D
    int num_registers = 0x0A; // Number of registers to read
    int start_address = 0x64; // Starting address
    const int dataSize = 1 + num_registers - start_address; // Total number of bytes to read
    uint16_t data[dataSize];

    int result = modbus_read_registers(this->_p->ctx, start_address, num_registers, data);

    if (result == -1) {
        std::cerr << stdErrorMsg("get_all_update_bool()","Failed to read data",modbus_strerror(errno));
        this->_p->spuData.STATE = 1;
        this->_p->spuData.TIME = std::chrono::system_clock::now();
        return this->_p->spuData;
    }

    // Convert data in floats variables
    this->_p->spuData.EMR_N           = data[0];
    this->_p->spuData.WRN_N           = data[1];
    this->_p->spuData.EMR_T           = data[2];
    this->_p->spuData.WRN_T           = data[3];
    this->_p->spuData.R1              = data[4];
    this->_p->spuData.R2              = data[5];
    this->_p->spuData.R3              = data[6];
    this->_p->spuData.RDY             = data[7];
    this->_p->spuData.TEST            = data[8];
    this->_p->spuData.XXXX            = data[9];
    this->_p->spuData.STATE           = 0;
    this->_p->spuData.TIME = std::chrono::system_clock::now();
    return this->_p->spuData;
 }


float libModbusSystematomSPU::get_1_DATA_FP(int start_address)
{
    if (!this->_p->ctx || this->_p->flagNotConnected) {
        std::cerr << stdErrorMsg("get_1_DATA_FP()","Modbus context does not exist","");
        return -1;
    }
    int num_registers = 0x02; // Number of registers to read
    const int dataSize = 1 + num_registers - start_address; // Total number of bytes to read
    uint16_t data[dataSize];
    int result = modbus_read_registers(this->_p->ctx, start_address, num_registers, data);
    if (result == -1) {
        std::cerr << stdErrorMsg("get_1_DATA_FP()","Failed to read data",modbus_strerror(errno));
        return -1;
    }
    return conv2RegsToFloat(data[ 0], data[ 1]);
}

uint16_t libModbusSystematomSPU::get_1_DATA(int address)
{
    if (!this->_p->ctx || this->_p->flagNotConnected) {
        std::cerr << stdErrorMsg("get_1_DATA()","Modbus context does not exist","");
        return -1;
    }
    uint16_t data[1];
    int result = modbus_read_registers(this->_p->ctx, address, 0x01, data);
    if (result == -1) {
        std::cerr << stdErrorMsg("get_1_DATA()","Failed to read data",modbus_strerror(errno));
        return -1;
    }
    return *data;
}

float libModbusSystematomSPU::get_N_DATA_FP()       {return get_1_DATA_FP(0x01);}
float libModbusSystematomSPU::get_T_DATA_FP()       {return get_1_DATA_FP(0x03);}
float libModbusSystematomSPU::get_F1_DATA_FP()      {return get_1_DATA_FP(0x05);}
float libModbusSystematomSPU::get_F2_DATA_FP()      {return get_1_DATA_FP(0x07);}
float libModbusSystematomSPU::get_F3_DATA_FP()      {return get_1_DATA_FP(0x09);}
float libModbusSystematomSPU::get_EMR_N_THRESHOLD() {return get_1_DATA_FP(0x0B);}
float libModbusSystematomSPU::get_WRN_N_THRESHOLD() {return get_1_DATA_FP(0x0D);}
float libModbusSystematomSPU::get_EMR_T_THRESHOLD() {return get_1_DATA_FP(0x0F);}
float libModbusSystematomSPU::get_WRN_T_THRESHOLD() {return get_1_DATA_FP(0x11);}
bool  libModbusSystematomSPU::get_EMR_N()           {return get_1_DATA(0x64);}
bool  libModbusSystematomSPU::get_WRN_N()           {return get_1_DATA(0x65);}
bool  libModbusSystematomSPU::get_EMR_T()           {return get_1_DATA(0x66);}
bool  libModbusSystematomSPU::get_WRN_T()           {return get_1_DATA(0x67);}
bool  libModbusSystematomSPU::get_R1()              {return get_1_DATA(0x68);}
bool  libModbusSystematomSPU::get_R2()              {return get_1_DATA(0x69);}
bool  libModbusSystematomSPU::get_R3()              {return get_1_DATA(0x70);}
bool  libModbusSystematomSPU::get_RDY()             {return get_1_DATA(0x71);}
bool  libModbusSystematomSPU::get_TEST()            {return get_1_DATA(0x72);}
bool  libModbusSystematomSPU::get_XXXX()            {return get_1_DATA(0x73);}

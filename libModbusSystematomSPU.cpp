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

#include "libModbusSystematomSPU.hpp"

void libModbusSystematomSPU_license()
{
    std::cout << "libModbusSystematomSPU  Copyright (C) 2023-2024 Thalles Campagnani" << std::endl;
    std::cout << "This program comes with ABSOLUTELY NO WARRANTY;" << std::endl;
    std::cout << "This is free software, and you are welcome to redistribute it" << std::endl;
    std::cout << "under certain conditions; For more details read the file LICENSE" << std::endl;
    std::cout << "that came together with the library." << std::endl << std::endl;
}

libModbusSystematomSPU::libModbusSystematomSPU(const char* portname) : portname(portname), port(nullptr)
{
    // Create a serial port handle
    result = sp_get_port_by_name(portname, &port);
    //if (result != SP_OK) throw std::runtime_error("Error: Unable to get the serial port by name");
    
    // Open the serial port
    result = sp_open(port, SP_MODE_READ_WRITE); // Open in read-write mode
    //if (result != SP_OK) throw std::runtime_error("Error: Unable to open the serial port");

    // Set the baud rate, data bits, stop bits, and parity
    sp_set_baudrate (port, 57600);
    sp_set_bits     (port, 8);
    sp_set_stopbits (port, 1);
    sp_set_parity   (port, SP_PARITY_NONE);
}

libModbusSystematomSPU::~libModbusSystematomSPU() {
    if (port) {
        sp_close(port);
        sp_free_port(port);
    }
}

const char*  libModbusSystematomSPU::get_portname()    {return portname;}

float   libModbusSystematomSPU::get_N_DATA_FP()        { return N_DATA_FP; }
float   libModbusSystematomSPU::get_T_DATA_FP()        { return T_DATA_FP; }
float   libModbusSystematomSPU::get_F1_DATA_FP()       { return F1_DATA_FP; }
float   libModbusSystematomSPU::get_F2_DATA_FP()       { return F2_DATA_FP; }
float   libModbusSystematomSPU::get_F3_DATA_FP()       { return F3_DATA_FP; }
float   libModbusSystematomSPU::get_EMR_N_THRESHOLD()  { return EMR_N_THRESHOLD; }
float   libModbusSystematomSPU::get_WRN_N_THRESHOLD()  { return WRN_N_THRESHOLD; }
float   libModbusSystematomSPU::get_EMR_T_THRESHOLD()  { return EMR_T_THRESHOLD; }
float   libModbusSystematomSPU::get_WRN_T_THRESHOLD()  { return WRN_T_THRESHOLD; }
uint8_t libModbusSystematomSPU::get_EMR_N()            { return EMR_N; }
uint8_t libModbusSystematomSPU::get_WRN_N()            { return WRN_N; }
uint8_t libModbusSystematomSPU::get_EMR_T()            { return EMR_T; }
uint8_t libModbusSystematomSPU::get_WRN_T()            { return WRN_T; }
uint8_t libModbusSystematomSPU::get_R1()               { return R1; }
uint8_t libModbusSystematomSPU::get_R2()               { return R2; }
uint8_t libModbusSystematomSPU::get_R3()               { return R3; }
uint8_t libModbusSystematomSPU::get_RDY()              { return RDY; }
uint8_t libModbusSystematomSPU::get_TEST()             { return TEST; }
uint8_t libModbusSystematomSPU::get_XXXX()             { return XXXX; }


float libModbusSystematomSPU::conv4BytesToFloat(uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4)
{
    uint8_t data[4] = {data1, data2, data3, data4};
    float* floatValue = reinterpret_cast<float*>(data);
    return *floatValue;
}

bool libModbusSystematomSPU::readAllRegisters(const int readTimeoutMillis)
{
    /*
        PROTOCOL INFORMATION: 
            LOOP: 
                Sendto slave device: 01 03 00 00 00 6D 84 27 
                Receive back regs data.
            Explanation:
                01      - is slave adress; 
                03      - is command (read data regs); 
                00 00   - start adress of regs; 
                00 6D   - number of regs to read;
                84 27   - CRC;
    */
    if (!port)// throw std::runtime_error("Error: Can't readAllRegisters because the serial port isn't open");
    {
        //std::cerr << "Error: Failed to read data" << std::endl;
        return 0;//0=não leu
    }
    // Data to be sent to the slave device
    uint8_t send_data[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x6D, 0x84, 0x27};

    // Write the data to the serial port
    result = sp_blocking_write(port, send_data, sizeof(send_data), readTimeoutMillis);
    if (result < 0) //throw std::runtime_error("Error: Failed to send data");
    {
        //std::cerr << "Error: Failed to read data" << std::endl;
        return 0;//0=não leu
    }
    // Read data from the serial port with a timeout
    const int dataSize = 3+2*0x006E; // Total number of bytes to read
    uint8_t data[dataSize];

    result = sp_blocking_read(port, data, dataSize /*sizeof(data)*/, readTimeoutMillis);
    if (result <= dataSize-1) //throw std::runtime_error("Error: Failed to read data");
    {
        //std::cerr << "Error: Failed to read data" << std::endl;
        return 0;//0=não leu
    }
    // Convert data in floats variables
    N_DATA_FP       = conv4BytesToFloat(data[ 7],data[ 8],data[ 9],data[10]);
    T_DATA_FP       = conv4BytesToFloat(data[11],data[12],data[13],data[14]);
    F1_DATA_FP      = conv4BytesToFloat(data[15],data[16],data[17],data[18]);
    F2_DATA_FP      = conv4BytesToFloat(data[19],data[20],data[21],data[22]);
    F3_DATA_FP      = conv4BytesToFloat(data[23],data[24],data[25],data[26]);
    EMR_N_THRESHOLD = conv4BytesToFloat(data[27],data[28],data[29],data[30]);
    WRN_N_THRESHOLD = conv4BytesToFloat(data[31],data[32],data[33],data[34]);
    EMR_T_THRESHOLD = conv4BytesToFloat(data[35],data[36],data[37],data[38]);
    WRN_T_THRESHOLD = conv4BytesToFloat(data[39],data[40],data[41],data[42]);
    EMR_N           = data[201];
    WRN_N           = data[203];
    EMR_T           = data[205];
    WRN_T           = data[207];
    R1              = data[209];
    R2              = data[211];
    R3              = data[213];
    RDY             = data[215];
    TEST            = data[217];
    XXXX            = data[219];
    return 1;//1=leu
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

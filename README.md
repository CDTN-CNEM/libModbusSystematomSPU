# libModbusSystematomSPU

libModbusSystematomSPU is a library to communicate with the SystemAtom SPU using MODBUS-RTU (RS-485) on a GNU operating system.

## Installing dependencies

This library use libserialport, so it should be install in your system before compilation process.

Using ArchLinux family:

```Bash
sudo pacman -Sy libmodbus
```

Using Debian family:

```Bash
sudo apt update && sudo apt install libmodbus-dev
```

## Accessing serialPorts

To make sure your user can acess serial ports, add your user to group uucp:

```Bash
sudo usermod -a -G uucp <your_user>
```

## Compiling just the Lib

```Bash
g++ -c libModbusSystematomSPU.cpp -lmodbus -o libModbusSystematomSPU.o
```

## Compiling the lib with test and Running

For compiling:

```Bash
g++ test.cpp libModbusSystematomSPU.cpp -lmodbus -o testlibModbusSystematomSPU.elf
```

For run the test you should specify the <serial_port_name> and <qtd_of_reads>. Example:

```Bash
./testlibModbusSystematomSPU.elf /dev/ttyUSB0 10
```

Tip: clear screen, compiling and running in just one command

```Bash
clear ; g++ test.cpp libModbusSystematomSPU.cpp  -lmodbus -o testlibModbusSystematomSPU.elf && ./testlibModbusSystematomSPU.elf /dev/ttyUSB0 10
```

## License, Warranty and Copyright

This library comes with ABSOLUTELY NO WARRANTY; Is under GNU GENERAL PUBLIC LICENSE version 3 (GPL3), that means you ONLY can use in free software projects.
For more details read the file LICENSE.
Copyright (C) 2023-2024  Thalles Campagnani
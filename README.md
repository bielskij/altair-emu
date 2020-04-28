# altair-emu

This project is an another Altair 8800 machine emulator implementation in C/C++. 

Most publicly available emulators contain the same issues in some opcodes execution and do not provide real time execution. According that I decided to write my own version of the emulator basing on original MITS and Intel documentation and some reverse engineering researches how the 8080 CPU internals works.

I tried to cover all the CPU internals with a 'tick' resolution. Core class implements original 8080 CPU, it covers:
 - CPU stating (FETCH, MEMR/W, STACKR/W, IN/OUT, INT)
 - CPU cycling
 - all CPU address, data and control signals.

TODO:
 - HALT, HALT+INT, WAIT states
 - HALT support
 
To make an emulator usable, some peripherals were implemented. Set of them allow running CPM on the emulator using original BROM and floppy images. The following peripherals are already implemented:
 - 88-SIO  (without VI-RTC interrupt support)
 - 88-2SIO (without VI-RTC interrupt support)  
 - 88-PMC
 - 88-MCS-16
 - 88-DCDD + FD400

### Building and running emulator application

##### Build
```bash
mkdir build
cd build
cmake ..
make
```

##### Run
```bash
./application ../examples/configs/cpm22-dcdd.ini
```

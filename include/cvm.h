#ifndef CVM_H
#define CVM_H

#define MEMSIZE 4096-1
#define STACKSIZE 16-1
#define REGISTERCOUNT 16-1
#define SCREENSIZEX 64-1
#define SCREENSIZEY 32-1
#define INDEX(x,y) (x + (SCREENSIZEX * y))

//For standardized data types
#include <cstdint>
#include <stdlib.h>

class CVM{
    public:
    CVM();
    ~CVM();
    void Step();
    void Run();
    void Pause();

    private:
    //Memory
    uint16_t* vmStack;//32 bytes stack memory
    uint8_t* vmMemory;//4096 bytes addressible memory
    uint8_t* frameBuffer;

    //Registers
    uint8_t* vmRegisters;//16 general purpose registers
    //Special purpose registers
    uint16_t I;//Address register
    uint8_t DT,ST;//Timer and sound registers
    //Pseudo-registers
    uint16_t PC; //Program counter
    uint8_t SP; //Stack pointer

};

#endif

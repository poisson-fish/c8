#ifndef CVM_H
#define CVM_H

//For standardized data types
#include <cstdint>
#include <stdlib.h>

//Register definitions
#define V0 0x0
#define V1 0x1
#define V2 0x2
#define V3 0x3
#define V4 0x4
#define V5 0x5
#define V6 0x6
#define V7 0x7
#define V8 0x8 
#define V9 0x9
#define VA 0xA
#define VB 0xB
#define VC 0xC
#define VD 0xD
#define VE 0xE
#define VF 0xF

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
    
    //Registers
    uint8_t* vmRegisters;//16 general purpose registers
    //Special purpose registers
    uint16_t I;//Address register
    uint8_t DT,ST;//Timer and sound registers
    //Pseudo-registers
    uint16_t PC; //Program counter
    uint8_t SP; //Stack pointer

    //Opcodes
    int op_SYS(); //Jump to machine code
    int op_CLS(); //Clear display
    int op_RET(); //Return from subroutine
    int op_JP();  //Jump to 12 bit location
    int op_CALL(); //Call to 12 bit subroutine address
    int op_SEC(); //Conditional constant instruction skip equality
    int op_SNEB(); //Conditional instruction skip inequality
    int op_SE(); //Conditional register instruction skip equality
    int op_LDB(); //Load byte register
    int op_ADDR(); //Add byte register
    int op_LDR(); //Load register register
    int op_OR(); //OR register
    int op_AND(); //AND register
    int op_XOR(); //XOR register
    int op_ADDC(); //Add register with carry
    int op_SUB(); //Subtract register borrow flag
    int op_SHR(); //Shift register right
    int op_SUBN(); //Subtract register reverse
    int op_SHL(); //Shift register left
    int op_SNER(); //Conditional instruction skip equality register
    int op_LDI(); //Load 12 bits to I register
    int op_JPR(); //Jump to 12 bit offset from register 0
    int op_RND(); //Random AND byte
    int op_DRW(); //Draw at register value
    int op_SKP(); //Keyboard register skip
    int op_SKNP(); //Keyboard not register skip
    int op_LDT(); //Load DT into register
    int op_LDK(); //Wait and load key press into register
    int op_DTL(); //Load DT from register
    int op_STL(); //Load ST from register
    int op_ADDI(); //Add into I register
    int op_LDF(); //Load sprite location into I register
    int op_LDIB(); //Load BCD into I, I+1, I+2
    int op_STIX(); //Load multiple registers into memory
    int op_LDIX(); //Read memory into multiple registers

};

#endif

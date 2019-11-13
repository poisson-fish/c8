#include "cvm.h"
#include "util.h"

#define ADR (((opcode & 0xF) << 4) & lobyte)
#define X (opcode & 0xF)
#define Y (lobyte >> 4)
#define KK (lobyte)
#define LONIB (lobyte & 0xF)


void CVM::Step(){
    uint8_t opcode = vmMemory[PC];
    uint8_t lobyte = vmMemory[PC+1];
    switch(opcode >> 4){
        case 0x0:
            switch(lobyte){
                case 0xE0: //CLS
                    //clearDisplay();
                    break;
                case 0xEE: //RET
                    PC = vmStack[SP];
                    SP -= 1;
                    break;
                default: //SYS
                    PC++;
                    break; //Ignored
            }
            break;
        case 0x1: //JP addr
            PC = ADR;
            break;
        case 0x2: //CALL addr
            SP++;
            vmStack[SP] = PC;
            PC = ADR;
            break;
        case 0x3: //SE Vx, byte
            if(vmRegisters[X] == KK){
                PC = PC + 2;
            }
            else{
                PC++;
            }
            break;
        case 0x4: //SNE Vx, byte
            if(vmRegisters[X] != KK){
                PC = PC + 2;
            }
            else{
                PC++;
            }
            break;
        case 0x5: //SE Vx, Vy
            if(vmRegisters[X] == vmRegisters[Y]){
                PC = PC + 2;
            }
            else{
                PC++;
            }
            break;
        case 0x6: //LD Vx, byte
            vmRegisters[X] = KK;
            PC++;
            break;
        case 0x7: //ADD Vx, byte
            vmRegisters[X] = vmRegisters[X] + KK;
            PC++;
            break;
        case 0x8:
            switch(LONIB){
                case 0x0: //LD Vx, Vy
                    vmRegisters[X] = vmRegisters[Y];
                    PC++;
                    break;
                case 0x1: //OR Vx, Vy
                    vmRegisters[X] = vmRegisters[X] | vmRegisters[Y];
                    PC++;
                    break;
                case 0x2: //AND Vx, Vy
                    vmRegisters[X] = vmRegisters[X] & vmRegisters[Y];
                    PC++;
                    break;
                case 0x3: //XOR Vx, Vy
                    vmRegisters[X] = vmRegisters[X] ^ vmRegisters[Y];
                    PC++;
                    break;
                case 0x4: //ADD Vx, Vy
                    {
                        uint8_t result = vmRegisters[X] + vmRegisters[Y]; //Do addition into a 16 bit bucket
                        if((result >> 8) == 0x0){ //Check if overflows 8 bits
                            vmRegisters[0xF] = 0x1; //Set F flag for overflow
                        }
                        else {
                            vmRegisters[0xF] = 0x0;
                        }
                        vmRegisters[X] = (result & 0xFF); //Assign lower byte to register
                        PC++;
                    }
                    break;
                case 0x5: //SUB Vx, Vy
                    {
                        uint8_t result = (0xFFFF & vmRegisters[X]) - vmRegisters[Y]; //Borrow canary bits AND'd
                        if((result >> 8) != 0xFF){ //Are canary bits untouched?
                            vmRegisters[0xF] = 0x0; //Naw, we had to borrow, set flag to 0
                        }
                        else{
                            vmRegisters[0xF] = 0x1; //Clean as a whistle
                        }
                        vmRegisters[X] = (result & 0xFF); //Assign lower byte
                        PC++;
                    }
                    break;
                case 0x6: //SHR Vx {, VY} Note: Originally this opcode meant set VX equal to VY bitshifted right 1 but emulators and software seem to ignore VY now.
                    if((vmRegisters[X] & 0x1) == 0x1){
                        vmRegisters[0xF] = 0x1;
                    }
                    else{
                        vmRegisters[0xF] = 0x0;
                    }
                    vmRegisters[X] = vmRegisters[X] >> 1;
                    PC++;
                    break;
                case 0x7: //SUBN Vx, Vy
                    {
                        uint8_t result = (0xFFFF & vmRegisters[Y]) - vmRegisters[X]; //Borrow canary bits AND'd
                        if((result >> 8) != 0xFF){ //Are canary bits untouched?
                            vmRegisters[0xF] = 0x0; //Naw, we had to borrow, set flag to 0
                        }
                        else{
                            vmRegisters[0xF] = 0x1; //Clean as a whistle
                        }
                        vmRegisters[X] = (result & 0xFF); //Assign lower byte
                        PC++;
                    }
                    break;
                case 0xE: //SHL Vx, {Vy} (See earlier note on SHR)
                    if((vmRegisters[X] & 0x80) == 0x1){
                        vmRegisters[0xF] = 0x1;
                    }
                    else{
                        vmRegisters[0xF] = 0x0;
                    }
                    vmRegisters[X] = vmRegisters[X] << 0x1;
                    PC++;
                    break;
                default:
                    PC++;
                    //Invalid Opcode
                    break;
            }
        case 0x9: //SNE Vx, Vy
            if(vmRegisters[X] != vmRegisters[Y]){
                PC = PC + 0x2;
            }
            else{
                PC++;
            }
            break;
        case 0xA: //LD I, addr
            I = ADR;
            PC++;
            break;
        case 0xB: //JP V0, addr
            PC = vmRegisters[0x0] + ADR;
            break;
        case 0xC: //RND Vx, byte
            vmRegisters[X] = ((uint16_t)(rand() % 256)) & KK;
            PC++;
            break;
        case 0xD: //DRW Vx, Vy, nibble
            //Not implemented
            PC++;
            break;
        case 0xE:
            switch(lobyte){
                case 0x9E: //SKP Vx
                    //Not implemented
                    PC++;
                    break;
                case 0xA1: //SKNP Vx
                    //Not implemented
                    PC++;
                    break;
            }
            break;
        case 0xF:
            switch(lobyte){
                case 0x07: //LD Vx, DT
                    vmRegisters[X] = DT;
                    PC++;
                    break;
                case 0x0A: //LD Vx, K
                    //Not implemented 
                    PC++;
                    break;
                case 0x15: //LD DT,Vx
                    DT = vmRegisters[X];
                    PC++;
                    break;
                case 0x18: //LD ST, Vx
                    ST = vmRegisters[X];
                    PC++;
                    break;
                case 0x1E: //ADD I, Vx
                    I = I + vmRegisters[X];
                    PC++;
                    break;
                case 0x29: //LD F, Vx
                    //Not implemented
                    PC++;
                    break;
                case 0x33: //LD B, Vx
                    {
                        std::vector<uint8_t> digits;
                        collect_digits(digits,vmRegisters[X]);
                        int i = 0;
                        for(std::vector<uint8_t>::iterator it = digits.begin(); it != digits.end(); it++,i++ ) vmMemory[I + i] = *it;
                        PC++;
                    }
                    break;
                case 0x55: //LD [I], Vx
                    {
                        for(int i = 0; i <= X;i++) vmMemory[I + i] = vmRegisters[i];
                        PC++;
                    }
                    break;
                case 0x65: //LD Vx, [I]
                    {
                        for(int i = 0; i <= X;i++)
                        {
                            vmRegisters[i] = vmMemory[I + i];
                        }
                        PC++;
                    }
                    break;
                default:
                    PC++;
                    //Invalid Opcode
                    break;
            }
        
    }

}
CVM::CVM(){
    vmStack = new uint16_t[15];
    vmMemory = new uint8_t[4095];
    vmRegisters = new uint8_t[15];
}
CVM::~CVM(){
    delete[] vmStack;
    delete[] vmMemory;
    delete[] vmRegisters;
}

#include "cvm.h"
#include "util.h"

#define OPCODE (vmMemory[PC])
#define LOBYTE (vmMemory[PC+1])
#define ADR (((OPCODE & 0xF) << 8) ^ LOBYTE)
#define X (OPCODE & 0xF)
#define Y (LOBYTE >> 4)
#define KK (LOBYTE)
#define LONIB (LOBYTE & 0xF)

void CVM::Step(){
    drawFlag = false;
    switch(OPCODE >> 4){
        case 0x0:
            switch(LOBYTE){
                case 0xE0: //CLS
                    for(int i = 0;i<=SCREENSIZEX*SCREENSIZEY;i++){
                        frameBuffer[i] = 0x0;
                    }
                    drawFlag = true;
                    PC+=2;
                    break;
                case 0xEE: //RET
                    PC = vmStack[SP];
                    SP -= 1;
                    break;
                default: //SYS
                    PC+=2;
                    break; //Ignored
            }
            break;
        case 0x1: //JP addr
            PC = ADR;
            std::cout << "Jump to " << std::hex << +ADR << std::endl;
            break;
        case 0x2: //CALL addr
            SP++;
            vmStack[SP] = PC;
            PC = ADR;
            break;
        case 0x3: //SE Vx, byte
            if(vmRegisters[X] == KK){
                PC = PC + 4;
            }
            else{
                PC+=2;
            }
            break;
        case 0x4: //SNE Vx, byte
            if(vmRegisters[X] != KK){
                PC = PC + 4;
            }
            else{
                PC+=2;
            }
            break;
        case 0x5: //SE Vx, Vy
            if(vmRegisters[X] == vmRegisters[Y]){
                PC = PC + 4;
            }
            else{
                PC+=2;
            }
            break;
        case 0x6: //LD Vx, byte
            vmRegisters[X] = KK;
            PC+=2;
            break;
        case 0x7: //ADD Vx, byte
            vmRegisters[X] = vmRegisters[X] + KK;
            PC+=2;
            break;
        case 0x8:
            switch(LONIB){
                case 0x0: //LD Vx, Vy
                    vmRegisters[X] = vmRegisters[Y];
                    PC+=2;
                    break;
                case 0x1: //OR Vx, Vy
                    vmRegisters[X] = vmRegisters[X] | vmRegisters[Y];
                    PC+=2;
                    break;
                case 0x2: //AND Vx, Vy
                    vmRegisters[X] = vmRegisters[X] & vmRegisters[Y];
                    PC+=2;
                    break;
                case 0x3: //XOR Vx, Vy
                    vmRegisters[X] = vmRegisters[X] ^ vmRegisters[Y];
                    PC+=2;
                    break;
                case 0x4: //ADD Vx, Vy
                    {
                        uint16_t result = vmRegisters[X] + vmRegisters[Y]; //Do addition into a 16 bit bucket
                        if((result >> 8) == 0x0){ //Check if overflows 8 bits
                            vmRegisters[0xF] = 0x1; //Set F flag for overflow
                        }
                        else {
                            vmRegisters[0xF] = 0x0;
                        }
                        vmRegisters[X] = (result & 0xFF); //Assign lower byte to register
                        PC+=2;
                    }
                    break;
                case 0x5: //SUB Vx, Vy
                    {
                        uint16_t result = (0xFFFF & vmRegisters[X]) - vmRegisters[Y]; //Borrow canary bits AND'd
                        if((result >> 8) != 0xFF){ //Are canary bits untouched?
                            vmRegisters[0xF] = 0x0; //Naw, we had to borrow, set flag to 0
                        }
                        else{
                            vmRegisters[0xF] = 0x1; //Clean as a whistle
                        }
                        vmRegisters[X] = (result & 0xFF); //Assign lower byte
                        PC+=2;
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
                    PC+=2;
                    break;
                case 0x7: //SUBN Vx, Vy
                    {
                        uint16_t result = (0xFFFF & vmRegisters[Y]) - vmRegisters[X]; //Borrow canary bits AND'd
                        if((result >> 8) != 0xFF){ //Are canary bits untouched?
                            vmRegisters[0xF] = 0x0; //Naw, we had to borrow, set flag to 0
                        }
                        else{
                            vmRegisters[0xF] = 0x1; //Clean as a whistle
                        }
                        vmRegisters[X] = (result & 0xFF); //Assign lower byte
                        PC+=2;
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
                    PC+=2;
                    break;
                default:
                    PC+=2;
                    //Invalid Opcode
                    break;
            }
        case 0x9: //SNE Vx, Vy
            if(vmRegisters[X] != vmRegisters[Y]){
                PC = PC + 4;
            }
            else{
                PC=PC + 2;
            }
            break;
        case 0xA: //LD I, addr
            I = ADR;
            PC+=2;
            break;
        case 0xB: //JP V0, addr
            PC = vmRegisters[0x0] + ADR;
            break;
        case 0xC: //RND Vx, byte
            vmRegisters[X] = ((uint16_t)(rand() % 256)) & KK;
            PC+=2;
            break;
        case 0xD: //DRW Vx, Vy, nibble
            for(int y = 0;y<=LONIB;y++){
                uint8_t toWrite = vmMemory[I + y];
                uint8_t xcoord = vmRegisters[X];
                uint8_t index_y = vmRegisters[Y]+y;
                uint8_t index_x = (uint8_t) (xcoord / sizeof(uint8_t));
                uint8_t index_x_mod = xcoord % sizeof(uint8_t);
                if(index_x_mod > 0){
                    uint8_t toWriteShiftStart = toWrite >> index_x_mod;
                    uint8_t toWriteShiftEnd = toWrite << (sizeof(uint8_t) - index_x_mod);
                    vmRegisters[0xF] = ((frameBuffer[INDEX(index_x,index_y)] & toWriteShiftStart) != 0)
                                        | ((frameBuffer[INDEX(index_x+1,index_y)] & toWriteShiftEnd) != 0);
                    frameBuffer[INDEX(index_x,index_y)] ^= toWriteShiftStart;
                    frameBuffer[INDEX(index_x+1,index_y)] ^= toWriteShiftEnd;
                }
                else {
                    vmRegisters[0xF] = (frameBuffer[INDEX(index_x,index_y)] & toWrite) != 0;
                    frameBuffer[INDEX(index_x,index_y)] ^= toWrite;
                }   
                drawFlag = true;
                    
            } 
            PC+=2;
            break;
        case 0xE:
            switch(LOBYTE){
                case 0x9E: //SKP Vx
                    //Not implemented
                    PC+=2;
                    break;
                case 0xA1: //SKNP Vx
                    //Not implemented
                    PC+=2;
                    break;
            }
            break;
        case 0xF:
            switch(LOBYTE){
                case 0x07: //LD Vx, DT
                    vmRegisters[X] = DT;
                    break;
                case 0x0A: //LD Vx, K
                    //Not implemented 
                    break;
                case 0x15: //LD DT,Vx
                    DT = vmRegisters[X];
                    break;
                case 0x18: //LD ST, Vx
                    ST = vmRegisters[X];
                    break;
                case 0x1E: //ADD I, Vx
                    I = I + vmRegisters[X];
                    break;
                case 0x29: //LD F, Vx
                    //Not implemented
                    break;
                case 0x33: //LD B, Vx
                    {
                        std::vector<uint8_t> digits;
                        collect_digits(digits,vmRegisters[X]);
                        int i = 0;
                        for(std::vector<uint8_t>::iterator it = digits.begin(); it != digits.end(); it++,i++ ) vmMemory[I + i] = *it;
                    }
                    break;
                case 0x55: //LD [I], Vx
                    {
                        for(int i = 0; i <= X;i++) vmMemory[I + i] = vmRegisters[i];
                    }
                    break;
                case 0x65: //LD Vx, [I]
                    {
                        for(int i = 0; i <= X;i++) vmRegisters[i] = vmMemory[I + i];
                    }
                    break;
                default:
                    //Invalid Opcode
                    break;
            }
            PC+=2;
            break;
    }

}
void CVM::LoadTestProgram(){
    uint8_t tank[] = {0x12,0x34,0x10,0x54,0x7C,0x6C,0x7C,0x7C,0x44,0x7C,0x7C,0x6C,0x7C,0x54,0x10,0x00,0xFC,0x78,0x6E,0x78,0xFC,0x00,0x3F,0x1E,0x76,0x1E,0x3F,0x00,0x72,0xFF,0xA2,0x02,0x00,0xEE,0x72,0x01,0xA2,0x08,0x00,0xEE,0x71,0xFF,0xA2,0x15,0x00,0xEE,0x71,0x01,0xA2,0x0F,0x00,0xEE,0x61,0x20,0x62,0x10,0xA2,0x02,0xD1,0x27,0xF0,0x0A,0xD1,0x27,0x40,0x02,0x22,0x1C,0x40,0x04,0x22,0x28,0x40,0x06,0x22,0x2E,0x40,0x08,0x22,0x22,0x12,0x3A};
    Reset();
    std::copy(std::begin(tank),std::end(tank),vmMemory+PC);
}
void CVM::Run(bool step){
    for(int j = PC; j<= PC+50;j++){
        std::cout << ":" << std::hex << +vmMemory[j] << std::endl;
    }
    while(true){
        std::cout << "Executing bytecode: " << std::hex << +vmMemory[PC] << ":" << +vmMemory[PC+1] << std::endl;
        Step();
        for(int i = 0;i<=REGISTERCOUNT;i++){
            std::cout << "Vx" << i <<": " << std::hex << +vmRegisters[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "PC: " << PC << std::endl;
        if(std::getchar() == 'd'){
            //Print framebuffer
            for(int y = 0; y<SCREENSIZEY;y++){
        
            for(int x = 0; x<SCREENSIZEX;x++){
                std::bitset<8> bitfield(frameBuffer[INDEX(x,y)]);
                std::cout << bitfield;
            }
            std::cout << std::endl;
         }   
            
        }
    }
}
void CVM::Reset(){
    PC = 0x200;
    int i = 0;
    for(i = 0;i<=STACKSIZE;i++){
        vmStack[i] = 0x0;
    }
    for(i = 0;i<=MEMSIZE;i++){
        vmMemory[i] = 0x0;
    }
    for(i = 0;i<=REGISTERCOUNT;i++){
        vmRegisters[i] = 0x0;
    }
    for(i = 0;i<=SCREENSIZEX*SCREENSIZEY;i++){
        frameBuffer[i] = 0x0;
    }
}
CVM::CVM(){
    vmStack = new uint16_t[STACKSIZE];
    vmMemory = new uint8_t[MEMSIZE];
    vmRegisters = new uint8_t[REGISTERCOUNT];
    frameBuffer = new uint8_t[SCREENSIZEX*SCREENSIZEY];
}
CVM::~CVM(){
    delete[] vmStack;
    delete[] vmMemory;
    delete[] vmRegisters;
    delete[] frameBuffer;
}

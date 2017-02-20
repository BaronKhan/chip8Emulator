#include <stdio.h>
#include "chip8.h"
#include "chip8Func.h"


int main() {
    chip8_h myChip8 = chip8Create();
    setPc(myChip8, 0x200);

    //-------------------------------------------------------------------------------------------------

    printf("Testing:    1NNN    Flow    goto NNN;   Jumps to address NNN.\n");
    for (int i=1; i <= 10; i++) {
        uint16_t expectedAddr = 0x200+i;
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x12, (uint8_t)i};
        writeMem(myChip8, 0x200, 2, opcode);
        uint8_t memValue[2];
        readMem(myChip8, getPc(myChip8), 2, memValue);
        emulateCycle(myChip8);
        printf("Opcode is %04x and PC contains %04x, PC jumps to address %04x, expected address %04x\n",
            (opcode[0]<<8) + opcode[1], (memValue[0]<<8) + memValue[1], getPc(myChip8), expectedAddr);
    }
    uint16_t expectedAddr = 0;
    setPc(myChip8, 0x200);
    uint8_t opcode[] = {0x10, 0};
    writeMem(myChip8, 0x200, 2, opcode);
    uint8_t memValue[2];
    readMem(myChip8, getPc(myChip8), 2, memValue);
    emulateCycle(myChip8);
    printf("Opcode is %04x and PC contains %04x, PC jumps to address %04x, expected address %04x\n",
        (opcode[0]<<8) + opcode[1], (memValue[0]<<8) + memValue[1], getPc(myChip8), expectedAddr);

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  2NNN    Flow    *(0xNNN)()  Calls subroutine at NNN.\n");
    printf("And:        00EE    Flow    return;     Returns from a subroutine.\n");
    for (int i=0; i <= 10; i++) {
        uint16_t expectedAddr = 0x200+i;
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x22, (uint8_t)i};
        writeMem(myChip8, 0x200, 2, opcode);
        uint8_t returnOpCode[] = {0x00, 0xEE};
        emulateCycle(myChip8);
        writeMem(myChip8, getPc(myChip8), 2, returnOpCode);
        uint8_t memValue[2];
        readMem(myChip8, getPc(myChip8), 2, memValue);
        printf("Opcode is %04x, PC jumping to address %04x, expected address %04x\n",
            (opcode[0]<<8) + opcode[1], getPc(myChip8), expectedAddr);
        emulateCycle(myChip8);
        printf("PC returned to address %04x, expected address %04x\n",
            getPc(myChip8), 0x202);
    }

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  3XNN    Cond    if(Vx==NN)  Skips the next instruction if VX equals NN.\n");
    for (int i = 0; i < 10; i++) {
        int registerTest = i;
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x30 | registerTest, registerTest*10};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, registerTest, registerTest*10);
        emulateCycle(myChip8);
        printf("Register %d is has value %d, expected value %d, skipping to address %04x, expected 204\n",
            registerTest, getRegister(myChip8, registerTest), registerTest*10, getPc(myChip8));
    }
    for (int i = 8; i < 14; i++) {
        int registerTest = i;
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x30 | registerTest, registerTest*10};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, registerTest, registerTest*10+1);
        emulateCycle(myChip8);
        printf("Register %d is has value %d, expected value %d, not skipping, now at %04x, expected 202\n",
            registerTest, getRegister(myChip8, registerTest), registerTest*10+1, getPc(myChip8));
    }

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  4XNN    Cond    if(Vx!=NN)  Skips the next instruction if VX doesn't equal NN.\n");
    for (int i = 0; i < 10; i++) {
        int registerTest = i;
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x40 | registerTest, 50};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, registerTest, registerTest*10);
        emulateCycle(myChip8);
        printf("Register %d is has value %d, not expecting value %d, skipping to address %04x\n",
            registerTest, getRegister(myChip8, registerTest), 50, getPc(myChip8));
    }

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  5XY0    Cond    if(Vx==Vy)  Skips the next instruction if VX equals VY.\n");
    for (int i = 0; i < 10; i++) {
        int registerTest = i;
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x50 | 0x1, (i<<4) | 0x0};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, registerTest, registerTest*10);
        setRegister(myChip8, 1, 50);
        emulateCycle(myChip8);
        printf("Register %d is has value %d, Register 1 is has value %d, skipping to address %04x\n",
            registerTest, getRegister(myChip8, registerTest), getRegister(myChip8, 1), getPc(myChip8));
    }

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  6XNN    Const   Vx = NN     Sets VX to NN.\n");
    for (int i = 0; i < 10; i++) {
        int registerTest = i;
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x60 | registerTest, registerTest*10};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, registerTest, 0);
        int oldValue = getRegister(myChip8, registerTest);
        emulateCycle(myChip8);
        printf("Register %d had value %d, setting to %d, expected %d\n",
            registerTest, oldValue, getRegister(myChip8, registerTest), registerTest*10);
    }

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  7XNN    Const   Vx += NN    Adds NN to VX.\n");
    for (int i = 0; i < 10; i++) {
        int registerTest = i;
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x70 | registerTest, registerTest*10};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, registerTest, registerTest);
        int oldValue = getRegister(myChip8, registerTest);
        emulateCycle(myChip8);
        printf("Register %d had value %d, setting to %d, expected %d\n",
            registerTest, oldValue, getRegister(myChip8, registerTest), registerTest+registerTest*10);
    }

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  8XY0    Assign  Vx=Vy   Sets VX to the value of VY.\n");
    int passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80, i<<4};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, i, i*10);
        emulateCycle(myChip8);
        printf("Register %d has value %d, opcode is %04x setting V0 to %d, expected %d\n",
            i, getRegister(myChip8, i), (opcode[0]<<8) + opcode[1], getRegister(myChip8, 0), i*10);
        if (getRegister(myChip8, 0) == getRegister(myChip8, i)) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  8XY1    BitOp   Vx=Vx|Vy    Sets VX to VX OR VY. (Bitwise OR operation)\n");
    passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80, (i<<4) | 1};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, 0, 40);
        setRegister(myChip8, i, i*10);
        int reg0 = getRegister(myChip8, 0);
        emulateCycle(myChip8);
        printf("Register %d has value %d, opcode is %04x setting V0 to %d, expected %d\n",
            i, getRegister(myChip8, i), (opcode[0]<<8) + opcode[1], getRegister(myChip8, 0), 40 | (i*10));
        if (getRegister(myChip8, 0) == reg0 | getRegister(myChip8, i)) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  8XY2    BitOp   Vx=Vx&Vy    Sets VX to VX and VY. (Bitwise AND operation)\n");
    passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80, (i<<4) | 2};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, 0, 40);
        setRegister(myChip8, i, i*10);
        int reg0 = getRegister(myChip8, 0);
        emulateCycle(myChip8);
        printf("Register %d has value %04x, opcode is %04x setting V0 to %04x, expected %04x\n",
            i, getRegister(myChip8, i), (opcode[0]<<8) + opcode[1], getRegister(myChip8, 0), 40 & (i*10));
        if (getRegister(myChip8, 0) == (reg0 & getRegister(myChip8, i))) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  8XY3    BitOp   Vx=Vx^Vy    Sets VX to VX xor VY.\n");
    passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80, (i<<4) | 3};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, 0, 40);
        setRegister(myChip8, i, i*10);
        int reg0 = getRegister(myChip8, 0);
        emulateCycle(myChip8);
        printf("Register %d has value %04x, opcode is %04x setting V0 to %04x, expected %04x\n",
            i, getRegister(myChip8, i), (opcode[0]<<8) + opcode[1], getRegister(myChip8, 0), 40 ^ (i*10));
        if (getRegister(myChip8, 0) == (reg0 ^ getRegister(myChip8, i))) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  8XY4    Math    Vx += Vy    Adds VY to VX. VF is set to 1 when there's a carry\n");
    passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80, (i<<4) | 4};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, 0, 40);
        setRegister(myChip8, i, i*25);
        int reg0 = getRegister(myChip8, 0);
        emulateCycle(myChip8);
        printf("Register %d has value %04x, setting V0 to %04x, expected %04x, VF is %d\n",
            i, getRegister(myChip8, i), getRegister(myChip8, 0), (uint8_t)(reg0 + (i*25)), getRegister(myChip8, 0xF));
        if (getRegister(myChip8, 0) == (uint8_t)(reg0 + getRegister(myChip8, i))) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  8XY5   Math    Vx -= Vy    VY is subtracted from VX. VF is set to 0 when there's a borrow,\n");
    passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80, (i<<4) | 5};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, 0, 40);
        setRegister(myChip8, i, i*10);
        int reg0 = getRegister(myChip8, 0);
        emulateCycle(myChip8);
        printf("Register %d has value %d, setting V0 to %d, expected %d, VF is %d\n",
            i, getRegister(myChip8, i), getRegister(myChip8, 0), (uint8_t)(reg0 - (i*10)), getRegister(myChip8, 0xF));
        if (getRegister(myChip8, 0) == (uint8_t)(reg0 - getRegister(myChip8, i))) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

    //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  8XY6    BitOp   Vx >> 1 Shifts VX right by one. VF is set to the value of the least significant bit of VX\n");
    passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80 + i, 6};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, i, i*11);
        uint8_t regVal = getRegister(myChip8, i);
        emulateCycle(myChip8);
        printf("Register %d had value %d, setting it to %d, expected %d, VF is %d\n",
            i, regVal, getRegister(myChip8, i), regVal >> 1, getRegister(myChip8, 0xF));
        if (getRegister(myChip8, i) == (uint8_t)(regVal >> 1)) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

    printf("\nTesting:  8XY7    Math    Vx=Vy-Vx    Sets VX to VY minus VX. VF is set to 0 when there's a borrow, a\n");
    passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80, (i<<4) | 7};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, 0, 40);
        setRegister(myChip8, i, i*10);
        int reg0 = getRegister(myChip8, 0);
        emulateCycle(myChip8);
        printf("Register %d has value %d, setting V0 to %d, expected %d, VF is %d\n",
            i, getRegister(myChip8, i), getRegister(myChip8, 0), (uint8_t)((i*10) - reg0), getRegister(myChip8, 0xF));
        if (getRegister(myChip8, 0) == (uint8_t)((i*10) - reg0)) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

        //-------------------------------------------------------------------------------------------------

    printf("\nTesting:  8XYE    BitOp   Vx << 1 Shifts VX left by one. VF is set to the value of the most significant bit of VX\n");
    passCount = 0;
    for (int i = 0; i < 10; i++) {
        setPc(myChip8, 0x200);
        uint8_t opcode[] = {0x80 + i, 0xE};
        writeMem(myChip8, 0x200, 2, opcode);
        setRegister(myChip8, i, i*25);
        uint8_t regVal = getRegister(myChip8, i);
        emulateCycle(myChip8);
        printf("Register %d had value %d, setting it to %d, expected %d, VF is %d\n",
            i, regVal, getRegister(myChip8, i), regVal << 1, getRegister(myChip8, 0xF));
        if (getRegister(myChip8, i) == (uint8_t)(regVal << 1)) { passCount++; }
    }
    printf("Number of tests passed: %d/10\n", passCount);

    return 0;
}
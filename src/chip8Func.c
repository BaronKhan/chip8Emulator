#include <stdio.h>
#include "chip8Func.h"
#include "chip8.h"

void (*opcodeTable[17])(chip8_h) = 
{
    cpuSpecial, cpuGoto, cpuCallSub, cpuSkipEqualsImmediate,
    cpuSkipNotEqualsImmediate, cpuSkipEqualsReg, cpuSetRegImmediate,
    cpuIncRegImmediate, cpuArithmetic, cpuNULL, cpuNULL,
    cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL
};

void (*arithmeticTable[15])(chip8_h) = 
{
    cpuMOV, cpuOR, cpuAND, cpuXOR,
    cpuADD, cpuSUB, cpuSHR, cpuRSB,
    cpuNULL, cpuNULL, cpuNULL, cpuNULL,
    cpuNULL, cpuNULL, cpuSHL
};

void cpuArithmetic(chip8_h state) {
    arithmeticTable[getOpcode(state) & 0xF](state);
}

void cpuKeyOp(chip8_h state) {}

void cpuOther(chip8_h state) {}

void cpuNULL(chip8_h state) {}

void cpuSpecial(chip8_h state) {
    switch(getOpcode(state)) {
        case 0x0E:
            cpuClearScreen(state);
            break;
        case 0xEE:
            cpuReturn(state);
            break;
        default:
            break;
    }
}

void cpuClearScreen(chip8_h state) {
    for (int i=0; i<64*32; i++) {
        setPixel(state, i, 0);
    }
}

void cpuReturn(chip8_h state) {
    uint16_t addr = stackPop(state);
    setPc(state, addr - 2);
}

void cpuGoto(chip8_h state) {
    uint16_t addr = (getOpcode(state) & 0x0FFF);
    setPc(state, addr - 2);
}

void cpuCallSub(chip8_h state) {
    uint16_t addr = (getOpcode(state) & 0x0FFF);
    //store address of next instruction
    stackPush(state, getPc(state)+2);
    setPc(state, addr - 2);
}

void cpuSkipEqualsImmediate(chip8_h state) {
    uint8_t regIndex = (getOpcode(state) >> 8) & 0xF;
    uint8_t value = getOpcode(state) & 0xFF;
    if (getRegister(state, regIndex) == value) {
        setPc(state, getPc(state)+2);
    }
}

void cpuSkipNotEqualsImmediate(chip8_h state) {
    uint8_t regIndex = (getOpcode(state) >> 8) & 0xF;
    uint8_t value = getOpcode(state) & 0xFF;
    if (getRegister(state, regIndex) != value) {
        setPc(state, getPc(state)+2);
    }
}

void cpuSkipEqualsReg(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t reg2 = (getOpcode(state) >> 4) & 0xF;
    if (getRegister(state, reg1) == getRegister(state, reg2)) {
        setPc(state, getPc(state)+2);
    }
}

void cpuSetRegImmediate(chip8_h state) {
    uint8_t regIndex = (getOpcode(state) >> 8) & 0xF;
    uint8_t value = getOpcode(state) & 0xFF;
    setRegister(state, regIndex, value);
}

void cpuIncRegImmediate(chip8_h state) {
    uint8_t regIndex = (getOpcode(state) >> 8) & 0xF;
    uint8_t value = getOpcode(state) & 0xFF;
    setRegister(state, regIndex, getRegister(state, regIndex) + value);
}

void cpuMOV(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t reg2 = (getOpcode(state) >> 4) & 0xF;
    setRegister(state, reg1, getRegister(state, reg2));
}

void cpuOR(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t reg2 = (getOpcode(state) >> 4) & 0xF;
    setRegister(state, reg1, getRegister(state, reg1) | getRegister(state, reg2));
}

void cpuAND(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t reg2 = (getOpcode(state) >> 4) & 0xF;
    setRegister(state, reg1, getRegister(state, reg1) & getRegister(state, reg2));
}

void cpuXOR(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t reg2 = (getOpcode(state) >> 4) & 0xF;
    setRegister(state, reg1, getRegister(state, reg1) ^ getRegister(state, reg2));
}

void cpuADD(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t reg2 = (getOpcode(state) >> 4) & 0xF;
    uint8_t x = getRegister(state, reg1);
    uint8_t y = getRegister(state, reg2);
    setRegister(state, reg1, (uint8_t)(x + y));
    setRegister(state, 0xF, !((uint8_t)(x + y) >= x && (uint8_t)(x + y) >= y));
}

void cpuSUB(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t reg2 = (getOpcode(state) >> 4) & 0xF;
    uint8_t x = (uint8_t)getRegister(state, reg1);
    uint8_t y = (uint8_t)getRegister(state, reg2);
    y = (~y) + 1;
    setRegister(state, reg1, (uint8_t)(x + y));
    setRegister(state, 0xF, ((uint8_t)(x + y) >= x && (uint8_t)(x + y) >= y));
}

void cpuSHR(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t x = getRegister(state, reg1);
    setRegister(state, 0xF, x & 1);
    setRegister(state, reg1, x >> 1);
}

void cpuRSB(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t reg2 = (getOpcode(state) >> 4) & 0xF;
    uint8_t x = (uint8_t)getRegister(state, reg1);
    uint8_t y = (uint8_t)getRegister(state, reg2);
    x = (~x) + 1;
    setRegister(state, reg1, (uint8_t)(y + x));
    setRegister(state, 0xF, ((uint8_t)(y + x) >= x && (uint8_t)(y + x) >= y));
}

void cpuSHL(chip8_h state) {
    uint8_t reg1 = (getOpcode(state) >> 8) & 0xF;
    uint8_t x = getRegister(state, reg1);
    setRegister(state, 0xF, (x >> 7) & 1);
    setRegister(state, reg1, x << 1);
}
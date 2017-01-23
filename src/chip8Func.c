#include <stdio.h>
#include "chip8Func.h"
#include "chip8.h"

void (*opcodeTable[17])(chip8_h) = 
{
	cpuSpecial, cpuGoto, cpuCallSub, cpuSkipEqualsImmediate,
	cpuSkipNotEqualsImmediate, cpuSkipEqualsReg, cpuSetRegImmediate,
	cpuIncRegImmediate, cpuNULL, cpuNULL, cpuNULL,
	cpuNULL, cpuNULL, cpuNULL, cpuNULL, cpuNULL,cpuNULL
};

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
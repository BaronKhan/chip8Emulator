#include <stdio.h>
#include "chip8.h"
#include "chip8Func.h"


int main() {
	chip8_h myChip8 = chip8Create();
	setPc(myChip8, 0x200);

	//-------------------------------------------------------------------------------------------------

	printf("Testing:	1NNN	Flow	goto NNN;	Jumps to address NNN.\n");
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

	printf("\nTesting: 	2NNN	Flow	*(0xNNN)()	Calls subroutine at NNN.\n");
	printf("And: 		00EE	Flow	return;	Returns from a subroutine.\n");
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

	printf("\nTesting: 	3XNN	Cond	if(Vx==NN)	Skips the next instruction if VX equals NN.\n");
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

	printf("\nTesting: 	4XNN	Cond	if(Vx!=NN)	Skips the next instruction if VX doesn't equal NN.\n");
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

	printf("\nTesting: 	5XY0	Cond	if(Vx==Vy)	Skips the next instruction if VX equals VY.\n");
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

	printf("\nTesting: 	6XNN	Const	Vx = NN 	Sets VX to NN.\n");
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

	printf("\nTesting: 	7XNN	Const	Vx += NN	Adds NN to VX.\n");
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

	return 0;
}
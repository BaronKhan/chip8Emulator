#ifndef chip8Func_h_header
#define chip8Func_h_header

#include <stdint.h>
#include "chip8.h"

#ifdef __cplusplus
extern "C"{
#endif

///////////////////////////
///FUNCTION ARRAY TABLES///
///////////////////////////

void (*opcodeTable[17])(chip8_h);			//use first hex digit
//void (*specialTable[3])(chip8_h);			//0XXX: use switch
void (*arithmeticTable[16])(chip8_h);		//8XXX: use last hex digit
void (*keyOpTable[11])(chip8_h);			//EXXX: use second last hex digit
void (*fTable[16])(chip8_h);				//FXXX: use switch

//////////////////////
///OPCODE FUNCTIONS///
//////////////////////

void cpuNULL(chip8_h state);
void cpuSpecial(chip8_h state);
void cpuClearScreen(chip8_h state);
void cpuReturn(chip8_h state);
void cpuGoto(chip8_h state);
void cpuCallSub(chip8_h state);
void cpuSkipEqualsImmediate(chip8_h state);
void cpuSkipNotEqualsImmediate(chip8_h state);
void cpuSkipEqualsReg(chip8_h state);
void cpuSetRegImmediate(chip8_h state);
void cpuIncRegImmediate(chip8_h state);

#ifdef __cplusplus
};
#endif

#endif
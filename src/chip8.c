#include "chip8.h"
#include "chip8Func.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////
//                      MEMORY ADDRESSES                        //
// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)  //
// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F) //
// 0x200-0xFFF - Program ROM and work RAM                       //
//                                                              //
// opcodes stored as big-endian - MSB first                     //
//////////////////////////////////////////////////////////////////

struct chip8 {
    uint16_t opcode;            //instruction opcode
    uint8_t mem[4096];          //memory
    uint8_t V[16];              //registers
    uint16_t I;                 //16-bit address register
    uint16_t pc;                //12-bit program counter
    uint8_t gfx[64 * 32];       //64x32 screen
    unsigned int delayTimer;    //60Hz timer counting to 0
    unsigned int soundTimer;    //60Hz timer counting to 0
    uint16_t stack[16];         //stack for saving program counter
    uint8_t sp;                 //stack pointer
};

chip8_h chip8Create() {
    chip8_h newChip8 = (struct chip8*)malloc(sizeof(struct chip8));
    initialise(newChip8);
    return newChip8;
}

void initialise(chip8_h state) {
    state->I = 0;
    state->pc = 0;
    state->delayTimer = 0;
    state->soundTimer = 0;
    state->sp = 0;

    for (int i = 0; i < 16; i++) {
        state->V[i] = 0;
    }

    for (int i = 0; i < 4096; i++) {
        state->mem[i] = 0;
    }
}

void destroy(chip8_h state) {
    free(state);
}

void loadGame(chip8_h state, char* path) {
    FILE *src=fopen(path,"rb");
    uint8_t v;
    uint32_t offset = 0x200;
    while (fread(&v, 1, 1, src) == 1) {
        if (!writeMem(state, offset, 1, (uint8_t*)&v)) {
            fprintf(stderr, "Memory error while loading game.");
        }
        offset += 1;
    }
}

uint8_t getRegister(chip8_h state, unsigned int reg) {
    return state->V[reg];
}

void setRegister(chip8_h state, unsigned int reg, uint8_t value) {
    state->V[reg] = value;
}

uint16_t getPc(chip8_h state) {
    return state->pc;
}

void setPc(chip8_h state, uint16_t value) {
    state->pc = value;
}

unsigned int getDelayTimer(chip8_h state) {
    return state->delayTimer;
}

void setDelayTimer(chip8_h state, unsigned int value) {
    state->delayTimer = value;
}

unsigned int getSoundTimer(chip8_h state) {
    return state->soundTimer;
}

void setSoundTimer(chip8_h state, unsigned int value) {
    state->soundTimer = value;
}

uint16_t getI(chip8_h state) {
    return state->I;
}

void setI(chip8_h state, uint16_t value) {
    state->I = value;
}

uint16_t getStack(chip8_h state, int index) {
    return state->stack[index];
}

void setStack(chip8_h state, int index, uint16_t value) {
    state->stack[index] = value;
}

uint8_t getStackPointer(chip8_h state) {
    return state->sp;
}

void setStackPointer(chip8_h state, uint8_t value) {
    state->sp = value;
}

void stackPush(chip8_h state, uint16_t value) {
    state->stack[state->sp] = value;
    if (state->sp < 15) {
        state->sp++;
    }
}

uint16_t stackPop(chip8_h state) {
    if (state->sp > 0) {
        state->sp--;
    }
    return state->stack[state->sp];
}

uint8_t getPixel(chip8_h state, int index) {
    return state->gfx[index];
}

void setPixel(chip8_h state, int index, uint8_t value) {
    state->gfx[index] = value;
}


int readWriteMem(chip8_h state, int write, uint32_t address, uint32_t length, uint8_t* dataInOut) {
    if((state == 0)) {
        return 0;
    }
    if (write) {
        for(unsigned i=0; i<length; i++){
            state->mem[address+i] = dataInOut[i];
        }
    }
    else {
        for(unsigned i=0; i<length; i++){
            dataInOut[i]=state->mem[address+i];
        }
    }
    return 1;
}

int readMem(chip8_h state, uint32_t address, uint32_t length, uint8_t* dataOut) {
    return readWriteMem(state, 0, address, length, dataOut);
}

int writeMem(chip8_h state, uint32_t address, uint32_t length, uint8_t* dataIn) {
    return readWriteMem(state, 1, address, length, dataIn);
}

uint16_t getOpcode(chip8_h state) {
    return state->opcode;
}

void setOpcode(chip8_h state, uint16_t value) {
    state->opcode = value;
}

void emulateCycle(chip8_h state) {
    
    //Fetch
    uint8_t opcodeLsb, opcodeMsb;
    if (readMem(state, state->pc, 1, &opcodeMsb) && readMem(state, state->pc+1, 1, &opcodeLsb)) {
        state->opcode = (((uint16_t)opcodeMsb) << 8) | ((uint16_t)opcodeLsb);
    }
    else {
        fprintf(stderr, "Error occurred fetching opcode at address &d", state->pc);
        return;
    }

    //Decode and execute
    opcodeTable[(state->opcode & 0xF000) >> 12](state);

    setPc(state, getPc(state)+2);
}
#ifndef chip8_h_header
#define chip8_h_header

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

struct chip8;

typedef struct chip8* chip8_h;

chip8_h chip8Create();
void initialise(chip8_h state);
void destroy(chip8_h state);
void loadGame(chip8_h state, char* path);
uint8_t getRegister(chip8_h state, unsigned int reg);
void setRegister(chip8_h state, unsigned int reg, uint8_t value);
uint16_t getPc(chip8_h state);
void setPc(chip8_h state, uint16_t value);
unsigned int getDelayTimer(chip8_h state);
void setDelayTimer(chip8_h state, unsigned int value);
unsigned int getSoundTimer(chip8_h state);
void setSoundTimer(chip8_h state, unsigned int value);
uint16_t getI(chip8_h state);
void setI(chip8_h state, uint16_t value);
uint16_t getStack(chip8_h state, int index);
void setStack(chip8_h state, int index, uint16_t value);
uint8_t getStackPointer(chip8_h state);
void setStackPointer(chip8_h state, uint8_t value);
void stackPush(chip8_h state, uint16_t value);
uint16_t stackPop(chip8_h state);
uint8_t getPixel(chip8_h state, int index);
void setPixel(chip8_h state, int index, uint8_t value);
int readWriteMem(chip8_h state, int write, uint32_t address, uint32_t length, uint8_t* dataInOut);
int readMem(chip8_h state, uint32_t address, uint32_t length, uint8_t* dataOut);
int writeMem(chip8_h state, uint32_t address, uint32_t length, uint8_t* dataIn);
uint16_t getOpcode(chip8_h state);
void setOpcode(chip8_h state, uint16_t value);
void emulateCycle(chip8_h state);
int drawFlag(chip8_h state);
void setKeys(chip8_h state);


#ifdef __cplusplus
};
#endif

#endif
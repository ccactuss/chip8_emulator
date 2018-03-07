#ifndef _CHIP_H
#define _CHIP_H

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <cmath>
#include <iomanip>
#include <stdint.h>


#define MEM_SIZE 4096
#define REG_SIZE 16
#define PIXELS 2048 //64*32
#define STACK_SIZE 16
#define KEY_SIZE 16


class Chip
{
	private:
		uint8_t _mem[MEM_SIZE]; //memory table
		uint8_t _V[REG_SIZE]; //registers

		uint16_t _I; //index register
		uint16_t _pc; //program-counter
		uint16_t _opcode;

		uint8_t _delayTimer;
		uint8_t _soundTimer;

		uint16_t _stack[STACK_SIZE];
		uint16_t _sp;

		void clearScreen(bool changeDF = true);
		void clearStack();
		void clearRegisters();
		void clearMem();
		void clearKeys();

		void call(uint16_t where);
		void ret();
		void jmp(uint16_t where);

		void renderSprite(uint8_t x , uint8_t y , uint8_t height);


		uint8_t getNumFromOpcode(int index);
		uint16_t getNumFromOpcode(int index , int len);

	public:

		uint8_t _gfx[64*32]; //graphics table
		uint8_t _key[KEY_SIZE];
		bool _drawFlag;


		Chip();
		~Chip();

		void init();
		void emulateCycle();

		bool loadFile(const char* path);

		void setKeys();
		int getKey();

};


#endif //_CHIP_H

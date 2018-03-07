#ifndef _CHIP_H
#define _CHIP_H

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <cmath>
#include <iomanip>


#define MEM_SIZE 4096
#define REG_SIZE 16
#define PIXELS 2048 //64*32
#define STACK_SIZE 16
#define KEY_SIZE 16


class Chip
{
	private:
		unsigned short _opcode;
		unsigned char _mem[MEM_SIZE]; //memory table
		unsigned short _V[REG_SIZE]; //registers

		unsigned short _I; //index register
		unsigned short _pc; //program-counter

		unsigned char _delayTimer;
		unsigned char _soundTimer;

		unsigned short _stack[STACK_SIZE];
		unsigned short _sp;

		void clearScreen();
		void clearStack();
		void clearRegisters();
		void clearMem();


		void call(unsigned short where);
		void ret();
		void jmp(unsigned short where);

		void renderSprite(unsigned short x , unsigned short y , unsigned short height);


		unsigned short getNumFromOpcode(int index);
		unsigned int getNumFromOpcode(int index , int len);

	public:

		unsigned char _gfx[64*32]; //graphics table
		unsigned char _key[KEY_SIZE];
		bool _drawFlag;

		int count;

		Chip();
		~Chip();

		void init();
		void emulateCycle();

		bool loadFile(const char* path);

		void setKeys();

};


#endif //_CHIP_H

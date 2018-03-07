#include "chip.h"


unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};


Chip::Chip()
{
	this->_pc = 0x200;
	this->_opcode = 0;
	this->_I = 0;
	this->_sp = 0;
}


Chip::~Chip(){}


/*
	Initializes and clears the system whole memory, sets fontset
*/
void Chip::init()
{
	this->clearScreen();
	this->clearStack();
	this->clearRegisters();
	this->clearMem();


	//set fontset
	for (int i = 0; i < 80; i++)
	{
		this->_mem[i] = chip8_fontset[i];
	}

}


bool Chip::loadFile(const char* path)
{
	FILE* f = fopen(path , "rb");
	long fSize;
	size_t readRes;
	char* fBuff = nullptr;

	if (!f)
	{
		std::cerr << "Failed to open file" << std::endl;
		return false;
	}

	fseek(f , 0 , SEEK_END);
	fSize = ftell(f);
	rewind(f);


	fBuff = new char[fSize];
	readRes = fread(fBuff , sizeof(char) , (size_t)fSize , f);

	if (readRes != fSize)
	{
		std::cerr << "Error in reading ROM file" << std::endl;
		return false;
	}

	if (fSize < (MEM_SIZE - 512))
	{
		for (int i = 0; i < fSize; i++)
		{
			this->_mem[i + 512] = fBuff[i];
		}
	}
	else
	{
		std::cerr << "File to large for emulator" << std::endl;
		return false;
	}

	fclose(f);
	delete[] fBuff;

	return true;

}


void Chip::emulateCycle()
{
	this->_opcode = this->_mem[this->_pc] << 8 | this->_mem[this->_pc + 1];

  std::cout << "Opcode = " << std::hex << this->_opcode << std::endl;
  //std::cin.get();

	switch(this->_opcode & 0xF000)
	{
		case 0x0000:
			switch(this->_opcode & 0x000F)
			{
				case 0x0000: //0x00E0
					this->clearScreen();
					this->_pc += 2;
					break;

				case 0x000E: //0x00EE ret from subroutine (function i guess)
          this->ret();
          this->_pc += 2;
					break;
			}

			break;


		case 0x1000: //jmp instuction (0x1NNN)
      this->jmp(getNumFromOpcode(1 , 3));
			break;


		case 0x2000: //call instruction (0x2NNN)
			this->call(getNumFromOpcode(1 , 3));
			break;


		case 0x3000: //if Vx == NN jmp to next instrucion (0x3XNN)

      std::cout << "\tthis->_V[" << std::hex << getNumFromOpcode(1) << "] = " << std::hex << _V[getNumFromOpcode(1)] << std::endl;
      std::cout << "\tgetNumFromOpcode(2,2) = " << std::hex << getNumFromOpcode(2,2) << std::endl;
      std::cout << (this->_V[getNumFromOpcode(1)] == getNumFromOpcode(2 , 2)) << std::endl;
      //std::cin.get();

			if (this->_V[getNumFromOpcode(1)] == getNumFromOpcode(2 , 2))
			{
				this->_pc += 4;
				break;
			}

			this->_pc += 2;
			break;



		case 0x4000: //if Vx != NN jmp to next instrucion (0x4XNN)
			if (this->_V[this->getNumFromOpcode(1)] != getNumFromOpcode(2 , 2))
			{
				this->_pc += 4;
				break;
			}

			this->_pc += 2;
			break;



		case 0x5000: //if Vx == Vy jmp to next instruction (0x5XY0)
			if (this->_V[this->getNumFromOpcode(1)] == this->_V[getNumFromOpcode(2)])
			{
				this->_pc += 4;
				break;
			}

			this->_pc += 2;
			break;


		case 0x6000: //Sets: Vx = NN (0x6XNN)
			this->_V[this->getNumFromOpcode(1)] = this->getNumFromOpcode(2 , 2);
      this->_pc += 2;
			break;

		case 0x7000: //Sets: Vx += NN (0x7XNN)
			this->_V[this->getNumFromOpcode(1)] += this->getNumFromOpcode(2 , 2);
      this->_pc += 2;
			break;


		case 0x8000:

			switch(this->_opcode & 0x000F)
			{
				case 0x0000: //sets: Vx = Vy (0x8XY0)
					this->_V[this->getNumFromOpcode(1)] = this->_V[getNumFromOpcode(2)];
          this->_pc += 2;
					break;

				case 0x0001: //Sets: Vx = Vx | Vy (0x8XY1)
					this->_V[this->getNumFromOpcode(1)] |= this->_V[getNumFromOpcode(2)];
          this->_pc += 2;
					break;

				case 0x0002: //Sets Vx = Vx & Vy (0x8XY2)
					this->_V[this->getNumFromOpcode(1)] &= this->_V[getNumFromOpcode(2)];
          this->_pc += 2;
					break;

				case 0x0003: //Sets Vx = Vx ^ Vy (0x8XY3)
					this->_V[getNumFromOpcode(1)] ^= this->_V[getNumFromOpcode(2)];
          this->_pc += 2;
					break;

          case 0x0004: //Sets: Vx += Vy (0x8XY4)
          this->_V[getNumFromOpcode(1)] += this->_V[getNumFromOpcode(2)];
          this->_V[0xF] = this->_V[getNumFromOpcode(2)] > (0xFF - this->_V[getNumFromOpcode(1)]) ? 1 : 0; //TODO check this statement
          this->_pc += 2;
					break;

				case 0x0005: //Sets Vx -= Vy (0x8XY5)

          this->_V[0xF] = this->_V[getNumFromOpcode(2)] > this->_V[getNumFromOpcode(1)] ? 0 : 1; //TODO know what borrow is

					this->_V[getNumFromOpcode(1)] -= this->_V[getNumFromOpcode(2)];
          this->_pc += 2;
					break;

				case 0x0006: //Sets Vf = Vy &  0x0001 (carry flag); Vy = Vy >> 1; Sets Vx = Vy (0x8XY6)
          this->_V[0xF] = this->_V[getNumFromOpcode(2)] & 0x0001;
          this->_V[getNumFromOpcode(2)] >>= 1;
          //this->_V[getNumFromOpcode(1)] = this->_V[getNumFromOpcode(2)];

          this->_pc += 2;
          break;

				case 0x0007: //Sets: Vx = Vy - Vx (0x8XY7)

          this->_V[0xF] = this->_V[getNumFromOpcode(1)] > this->_V[getNumFromOpcode(2)] ? 0 : 1;

					this->_V[this->getNumFromOpcode(1)] = this->_V[getNumFromOpcode(2)] - this->_V[getNumFromOpcode(1)];
          this->_pc += 2;
					break;

				case 0x000E: //Sets Vf = Vy &  0x1000 (carry flag); Vy = Vy << 1; Sets Vx = Vy (0x8XYE)
          this->_V[0xF] = this->_V[getNumFromOpcode(1)] >> 7;
          this->_V[getNumFromOpcode(1)] <<= 1;
          //this->_V[getNumFromOpcode(1)] = this->_V[getNumFromOpcode(2)];
          this->_pc += 2;
					break;

				default:
          std::cerr << "Unknown opcode: " << std::hex << this->_opcode << std::endl;
          exit(3);
					break;
			}

			break;


		case 0x9000: //if Vx != Vy jmp to next instrucion (0x9XY0)
			if (this->_V[this->getNumFromOpcode(1)] != this->_V[getNumFromOpcode(2)])
			{
				this->_pc += 4;
				break;
			}

			this->_pc += 2;
			break;


		case 0xA000: //set I (0xANNN)
			this->_I = this->_opcode & 0x0FFF;
			this->_pc += 2;
			break;


		case 0xB000: //jmp to V0 + NNN (0xBNNN)
      this->jmp(this->_V[0X0] + getNumFromOpcode(1 , 3));
			break;

		case 0xC000: //Sets Vx = random() & NN (0xCXNN)
			srand(time(NULL));
			this->_V[this->getNumFromOpcode(1)] = ((rand() % (0xFF + 1)) & this->getNumFromOpcode(2 , 2));
      this->_pc += 2;
			break;


		case 0xD000: //Renders a sprite at (Vx,Vy) with with of 8px and height of Npx (0xDXYN)
      this->renderSprite(getNumFromOpcode(1) , getNumFromOpcode(2) , getNumFromOpcode(3));
      this->_pc += 2;
			break;


		case 0xE000: //Keypress event

			switch(this->_opcode & 0x00FF)
			{
				case 0x009E: //if Keypress == Vx jmp to next instrucion (0xEX9E)
          if (this->_key[this->_V[getNumFromOpcode(1)]] != 0)
          {
            this->_pc += 4;
            break;
          }

          this->_pc += 2;
					break;

				case 0x00A1: //if Keypress != Vx jmp to next instrucion (0xEXA1)
          if (!this->_key[this->_V[getNumFromOpcode(1)]])
          {
            this->_pc += 4;
            break;
          }

          this->_pc += 2;
          break;

				default:
					break;
			}

			break;


		case 0xF000:

			switch(this->_opcode & 0x00FF)
			{
				case 0x0007: //Sets: Vx = delayTimer; (0xFX07)
          this->_V[getNumFromOpcode(1)] = this->_delayTimer;
          this->_pc += 2;
					break;

				case 0x000A: //Sets: Vx = getKeypress; (0xFX0A)
          this->_pc += 2;
					break;

				case 0x0015: //Sets: delayTimer = Vx (0xFX15)
          this->_delayTimer = this->_V[getNumFromOpcode(1)];
          this->_pc += 2;
					break;

				case 0x0018: //Sets: soundTimer = Vx (0xFX18)
          this->_soundTimer = this->_V[getNumFromOpcode(1)];
          this->_pc += 2;
					break;

				case 0x001E: //Sets: I += Vx (0xFX1E)
          if (this->_I + this->_V[getNumFromOpcode(1)] > 0xFFF ? this->_V[0xF] = 1 : this->_V[0xF] = 0);
          this->_I += this->_V[getNumFromOpcode(1)];
          this->_pc += 2;
					break;

				case 0x0029: //Sets: I = Vx * 0x5 (0xFX29) //TODO fix decription
          this->_I = this->_V[getNumFromOpcode(1)] * 0x5;
          this->_pc += 2;
					break;

				case 0x0033: //Store dec(Vx), then mem[I] = hunderds(Vx), mem[I + 1] = tens(Vx), mem[I + 2] = digit(Vx) (0xFX33)
          this->_mem[this->_I] = getNumFromOpcode(1) / 100;
          this->_mem[this->_I + 1] = (getNumFromOpcode(1) / 10) % 10;
          this->_mem[this->_I + 2] = (getNumFromOpcode(1) % 100) % 10;

          this->_pc += 2;
					break;

				case 0x0055: //stores V0 to Vx at mem[I] (0xFX55)
          for (int i = 0; i <= getNumFromOpcode(1); i++)
          {
            this->_mem[this->_I + i] = this->_V[i];
          }

          this->_I += getNumFromOpcode(1) + 1;
          this->_pc += 2;
					break;

				case 0x0065: //Sets: V0 to Vx with values from mem[I]
          for (int i = 0; i < getNumFromOpcode(1); i++)
          {
            this->_V[i] = this->_mem[i + this->_I];
          }

          this->_I += getNumFromOpcode(1) + 1; //the way of the original interpreter
          this->_pc += 2;
					break;

				default:
					break;
			}

			break;

		default:
			std::cout << "Unknown opcode: 0x" << std::hex << this->_opcode << std::endl;
			exit(3);

	}



	    // Update timers
    if (this->_delayTimer > 0)
        --this->_delayTimer;

    if (this->_soundTimer > 0)
        if(this->_soundTimer == 1);
            //std::cout << "BEEP!" << std::endl;
        --this->_soundTimer;

}



/////////Utils//////////
void Chip::clearScreen()
{
	for (int i = 0; i < PIXELS; i++)
	{
		this->_gfx[i] = 0;
	}

  this->_drawFlag = true;
}



void Chip::clearStack()
{
	for (int i = 0; i < STACK_SIZE; ++i)
	{
		this->_stack[i] = 0;
	}
}


void Chip::clearRegisters()
{
	for (int i = 0; i < REG_SIZE; ++i)
	{
		this->_V[i] = 0;
	}
}


void Chip::clearMem()
{
	for (int i = 0; i < MEM_SIZE; ++i)
	{
		this->_mem[i] = 0;
	}
}



void Chip::call(unsigned short where)
{
	this->_stack[this->_sp] = this->_pc;
	++this->_sp;

	this->_pc = where;
}


void Chip::ret()
{
	--this->_sp;
	this->_pc = this->_stack[this->_sp];
}


void Chip::jmp(unsigned short where)
{
  std::cout << "'\tJumped to " << std::hex << this->_pc << std::endl;
  this->_pc = where;
  //std::cin.get();
}


void Chip::renderSprite(unsigned short x , unsigned short y , unsigned short height)
{
    unsigned short pix;
    this->_V[0xF] = 0; //resetting carry flag. it can be used as collision detector


    for (int yLine = 0; yLine < height; yLine++)
    {
      pix = this->_mem[this->_I + yLine];

      for (int xLine = 0; xLine < 8; xLine++)
      {
        if ((pix & (0x80 >> xLine)) != 0)
        {
          if (this->_gfx[x + xLine + ((y + yLine) * 64)] == 1)
          {
            this->_V[0xF] = 1;
          }

          this->_gfx[x + xLine + ((y + yLine) * 64)] ^= 1;
        }
      }

    }

    this->_drawFlag = true;
}



/*
	@param index in the hex number
	@return hex value of one digit

	example: this->_opcode = 0x1234;
			 this->getNumFromOpcode(0) = 1
			 this->getNumFromOpcode(1) = 2

			 ect...
*/
unsigned short Chip::getNumFromOpcode(int index)
{
  int num = (this->_opcode & (0xF000 / (int)std::pow(0x10 , index))) >> (12 - index * 4);
	return (this->_opcode & (0xF000 / (int)std::pow(0x10 , index))) >> (12 - index * 4);
}



/*
  gets a number from the opcode according to length and index

  @param index the index in the hex number
  @param len the length of the number
*/
unsigned int Chip::getNumFromOpcode(int index , int len)
{
  unsigned int num = 0;

//  std::cout << "gnfOpcode(" << index << "," << len <<  "): has been called" << std::endl;

  for (int i = 0; i < len; i++)
  {
      num *= 0x10;
      num += (this->getNumFromOpcode(i + index));
  }

  //std::cout << "gnfOpcode(" << index << "," << len <<  "): " << std::hex << num << std::endl;
  //std::cin.get();


  return num;
}

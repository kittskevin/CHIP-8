
#include <string>
#include <iostream>

#include "Types.h"
#include "CPU.h"

using namespace std;

CPU::CPU(string game) {
	// Set the registers
	program_counter = GAME_START;
	mem = new BYTE[MEM]();
	registers = new BYTE[16]();
	keys = new int[16]();
	i_register = 0;
	delay_timer = 0;

	// Set up the game file, open the file, then read it into memory
	FILE *file;
	file = fopen(game.c_str(), "r");
	if (file == nullptr) {
		cout << "Failed to open game file" << endl;
		exit(EXIT_FAILURE);
	}

	fread(&mem[GAME_START], MEM, 1, file);
	fclose(file);
}

WORD CPU::getNextInstruction() {
	if (program_counter + 1 > MEM && program_counter > 0) {  // Insanity check the PC
		cout << "Program counter out of bounds..." << endl;
		exit(EXIT_FAILURE);
	}
	WORD next = mem[program_counter];  // Get the byte at the program counter
	next = next << 8;  // Shift by 1 byte to make room for the next byte
	BYTE temp = mem[program_counter + 1];
	next = next | temp;

	program_counter += 2;

	return next;
}

void CPU::decrementTimer() {
	if (delay_timer > 0) {  // Only if the timer is set
		delay_timer--;
	}
}

void CPU::keydown(int key) {
	keys[key] = 1;
}


void CPU::keyup(int key) {
	keys[key] = 0;
}

void CPU::execute(WORD opcode) {
	switch (opcode & 0xF000) {  // Split by first number
	case 0x0000: {
		switch (opcode & 0x00FF) {  // Differentiate last two bytes
		case 0x00E0: Opcode00E0(); break;
		case 0x00EE: Opcode00EE(); break;
		}
	} break;
	case 0x1000: Opcode1NNN(opcode); break;
	case 0x2000: Opcode2NNN(opcode); break;
	case 0x3000: Opcode3XNN(opcode); break;
	case 0x4000: Opcode4XNN(opcode); break;
	case 0x5000: Opcode5XY0(opcode); break;
	case 0x6000: Opcode6XNN(opcode); break;
	case 0x7000: Opcode7XNN(opcode); break;
	case 0x8000: {
		switch (opcode & 0xF) {  // Differentiate by last byte
		case 0x0: Opcode8XY0(opcode); break;
		case 0x1: Opcode8XY1(opcode); break;
		case 0x2: Opcode8XY2(opcode); break;
		case 0x3: Opcode8XY3(opcode); break;
		case 0x4: Opcode8XY4(opcode); break;
		case 0x5: Opcode8XY5(opcode); break;
		case 0x6: Opcode8XY6(opcode); break;
		case 0x7: Opcode8XY7(opcode); break;
		case 0xE: Opcode8XYE(opcode); break;
		}
	}
	case 0x9000: Opcode9XY0(opcode); break;
	case 0xA000: OpcodeANNN(opcode); break;
	case 0xB000: OpcodeBNNN(opcode); break;
	case 0xC000: OpcodeCXNN(opcode); break;
	case 0xD000: OpcodeDXYN(opcode); break;
	case 0xE000: {
		switch (opcode & 0x00FF) {  // Naming??
		case 0x009E: OpcodeEX9E(opcode); break;
		case 0x00A1: OpcodeEXA1(opcode); break;
		}
	}
	case 0xF000: {
		switch (opcode & 0x00FF) {
		case 0x07: OpcodeFX07(opcode); break;
		case 0x0A: OpcodeFX0A(opcode); break;
		case 0x15: OpcodeFX15(opcode); break;
		case 0x18: OpcodeFX18(opcode); break;
		case 0x1E: OpcodeFX1E(opcode); break;
		case 0x29: OpcodeFX29(opcode); break;
		case 0x33: OpcodeFX33(opcode); break;
		case 0x55: OpcodeFX55(opcode); break;
		case 0x65: OpcodeFX65(opcode); break;
		}
	}
	default: break;
	}
}

void CPU::Opcode00E0() {
	memset(pixels, 0, WIDTH * HEIGHT);  // Clear the screen
}

void CPU::Opcode00EE() {
	program_counter = stack.back();  // Set the pc to the last adress on the stack
	stack.pop_back();  // pop it off the stack
}

void CPU::Opcode1NNN(WORD opcode) {
	program_counter = opcode & 0xFFF;  // Jump to adress NNN
}

void CPU::Opcode2NNN(WORD opcode) {
	stack.push_back(program_counter);  // Push the return address (next instruction)
	program_counter = opcode & 0x0FFF;  // Call / jump? to adress NNN
}

void CPU::Opcode3XNN(WORD opcode) {
	WORD x = (0x0F00 & opcode) >> 8;
	if ((opcode & 0x00FF) == registers[x]) {
		program_counter += 2;  // Skip the next instruction
	}
}

void CPU::Opcode4XNN(WORD opcode) {
	WORD x = (0x0F00 & opcode) >> 8;
	if ((opcode & 0x00FF) != registers[x]) {
		program_counter += 2;  // Skip the next instruction
	}
}

void CPU::Opcode5XY0(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	if (registers[x] == registers[y]) {
		program_counter += 2;  // Skip the next instruction
	}
}

void CPU::Opcode6XNN(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	registers[x] = (opcode & 0x00FF);
}

void CPU::Opcode7XNN(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	registers[x] += (opcode & 0x00FF);
}

void CPU::Opcode8XY0(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	registers[x] = registers[y];
}

void CPU::Opcode8XY1(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	registers[x] |= registers[y];
}

void CPU::Opcode8XY2(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	registers[x] &= registers[y];
}

void CPU::Opcode8XY3(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	registers[x] ^= registers[y];
}

void CPU::Opcode8XY4(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	WORD old = registers[x];
	registers[x] += registers[y];
	if (old > registers[x]) {  // Check for cary bit / overflow
		registers[0xF] = 1;
	}
	else {
		registers[0xF] = 0;
	}
}

void CPU::Opcode8XY5(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	if (registers[y] > registers[x]) {  // Check for borrow
		registers[0xF] = 0;
	}
	else {
		registers[0xF] = 1;
	}
	registers[x] -= registers[y];
}

void CPU::Opcode8XY6(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	BYTE lsb = (registers[x] & 0x0001);  // Get the least significant bit of VX
	registers[x] = registers[x] >> 1;
	registers[0xF] = lsb;
}

void CPU::Opcode8XY7(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	if (registers[y] < registers[x]) {  // Check for borrow
		registers[0xF] = 0;
	}
	else {
		registers[0xF] = 1;
	}
	registers[x] = registers[y] - registers[x];
}

void CPU::Opcode8XYE(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	BYTE msb = (registers[x] >> 7);  // Get the most significant bit of VX
	registers[x] = registers[x] << 1;
	registers[0xF] = msb;
}

void CPU::Opcode9XY0(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	WORD y = (opcode & 0x00F0) >> 4;
	if (registers[x] != registers[y]) {
		program_counter += 2;
	}
}

void CPU::OpcodeANNN(WORD opcode) {
	i_register = opcode & 0x0FFF;
}

void CPU::OpcodeBNNN(WORD opcode) {
	WORD addr = opcode & 0x0FFF;
	program_counter = addr + registers[0];
}

void CPU::OpcodeCXNN(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	registers[x] = (opcode & 0x00FF) & (rand() % 0xFFFF);
}

void CPU::OpcodeDXYN(WORD opcode) {
	WORD regx = (opcode & 0x0F00) >> 8;
	WORD regy = (opcode & 0x00F0) >> 4;

	int height = opcode & 0x000F;
	int xcoord = registers[regx];
	int ycoord = registers[regy];

	registers[0xF] = 0;  // Collision flag

	for (int line = 0; line < height; line++) {
		BYTE data = mem[i_register + line];
		int pixelinv = 7;
		int pixel = 0;
		for (pixel = 0; pixel < 8; pixel++, pixelinv--) {
			int mask = 1 << pixelinv;
			if (data & mask) {
				int x = xcoord + pixel;
				int y = ycoord + line;
				if (pixels[x][y] == 1) {
					registers[0xF] = 1;  // Check for collision
				}
				pixels[x][y] ^= 1;
			}
		}
	}
}

void CPU::OpcodeEX9E(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	int key = registers[x];
	if (keys[key] == 1) {  // Is pressed
		program_counter += 2;
	}
}

void CPU::OpcodeEXA1(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	int key = registers[x];
	if (keys[key] == 0) {  // Isnt pressed
		program_counter += 2;
	}
}

void CPU::OpcodeFX07(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	registers[x] = delay_timer;
}

void CPU::OpcodeFX0A(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	
	// See if a key is pressed
	int key = -1;
	for (int i = 0; i < 16; i++) {
		if (keys[i] == 1) {
			key = i;
		}
	}

	if (key == -1) {  // repeat the instruction if no key is pressed, emulating a pause for key press
		program_counter -= 2;
	}
	else {
		registers[x] = key;
	}

}

void CPU::OpcodeFX15(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	delay_timer = registers[x];
}

void CPU::OpcodeFX18(WORD opcode) {
	// SOUND TIMER
}

void CPU::OpcodeFX1E(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	i_register += registers[x];
}

void CPU::OpcodeFX29(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	i_register = registers[x] * 5;
}

void CPU::OpcodeFX33(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;
	
	int val = registers[x];
	int hundreds = val / 100;
	int tens = (val / 10) % 10;
	int ones = val % 10;

	mem[i_register] = hundreds;
	mem[i_register + 1] = tens;
	mem[i_register + 2] = ones;
}

void CPU::OpcodeFX55(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;

	for (int i = 0; i <= x; i++) {
		mem[i_register] = registers[i];
		i_register++;
	}
}

void CPU::OpcodeFX65(WORD opcode) {
	WORD x = (opcode & 0x0F00) >> 8;

	for (int i = 0; i <= x; i++) {
		registers[i] = mem[i_register];
		i_register++;
	}
}
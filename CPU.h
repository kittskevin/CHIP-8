#ifndef _CPU_H_
#define _CPU_H_

#include <string>
#include <vector>

#include "Types.h"

using namespace std;

#define MEM 40960  // The size of main memory
#define GAME_START 0x200  // The location of the start of the game in main memory

// The display pixels
#define WIDTH 64
#define HEIGHT 32

class CPU {
public:
	// Sets up the CPU registers and loads the given game into memory
	CPU(string rom);

	// Gets the next opcode and advances PC
	WORD getNextInstruction();

	// Executes the given opcode
	void execute(WORD opcode);

	void decrementTimer();

	void keydown(int key);
	void keyup(int key);

	// Opcode specification from https://en.wikipedia.org/wiki/CHIP-8
	void Opcode00E0();  // Clears the screen
	void Opcode00EE();  // Returns from a subroutine
	void Opcode1NNN(WORD opcode);  // Jumps to adress NNN
	void Opcode2NNN(WORD opcode);  // Calls subroutine at NNN
	void Opcode3XNN(WORD opcode);  // Skips the next instruction if VX equals NN
	void Opcode4XNN(WORD opcode);  // Skips the next instruction if VX does not equal NN
	void Opcode5XY0(WORD opcode);  // Skips the next instruction if VX equals VY
	void Opcode6XNN(WORD opcode);  // Set VX to NN
	void Opcode7XNN(WORD opcode);  // Add NN to VX
	void Opcode8XY0(WORD opcode);  // Sets VX to value of VY
	void Opcode8XY1(WORD opcode);  // Sets VX to VX or VY
	void Opcode8XY2(WORD opcode);  // Sets VX to VX and VY
	void Opcode8XY3(WORD opcode);  // Sets VX to VX xor VY
	void Opcode8XY4(WORD opcode);  // Adds VY to VX, sets VF to 1 on carry otherwise 0
	void Opcode8XY5(WORD opcode);  // VY is subtracted from VX. VF set to 0 on borrow, else 1
	void Opcode8XY6(WORD opcode);  // Shifts VX right by one, VY is set to least significant bit of VX before shift
	void Opcode8XY7(WORD opcode);  // VX is set to VY minus VX, VF set to 0 on borrow 1 otherwise
	void Opcode8XYE(WORD opcode);  // Shifts VX left by one, VF set to most significant bit
	void Opcode9XY0(WORD opcode);  // Skips next instruction if VX doesn't equal VY
	void OpcodeANNN(WORD opcode);  // Sets I to the adress NNN
	void OpcodeBNNN(WORD opcode);  // Jumps to address NNN + V0
	void OpcodeCXNN(WORD opcode);  // Sets VX to the result of a bitwise operation and operation on a random number and NN
	// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is 
	// read as bit-coded starting from memory location I; I value doesn�t change after the execution of this instruction.
	// As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
	// and to 0 if that doesn�t happen
	void OpcodeDXYN(WORD opcode);
	void OpcodeEX9E(WORD opcode);  // Skips the next instruction if the key stored in VX is pressed.
	void OpcodeEXA1(WORD opcode);  // Skips the next instruction if the key stored in VX isn't pressed.
	void OpcodeFX07(WORD opcode);  // Sets VX to the value of the delay timer.
	void OpcodeFX0A(WORD opcode);  // A key press is awatied, and then stored in VX
	void OpcodeFX15(WORD opcode);  // Sets the delay timer to VX
	void OpcodeFX18(WORD opcode);  // Sets the sound timer to VX
	void OpcodeFX1E(WORD opcode);  // Adds VX to I
	void OpcodeFX29(WORD opcode);  // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
	void OpcodeFX33(WORD opcode);  // Stores binary representation of VX at I, I+1, and I+2
	void OpcodeFX55(WORD opcode);  // Stores V0 to VX (including VX) in memory starting at address I.
	void OpcodeFX65(WORD opcode);  // Fills V0 to VX (including VX) with values from memory starting at address I

	// Has to be a public
	BYTE pixels[WIDTH][HEIGHT];  // Represents the pixels for display
	WORD program_counter;  // The currently executed instruction
private:
	BYTE *mem;  // Main memory
	BYTE *registers;  // 16 CPU registers
	WORD i_register;  // The memory register used with some opcodes
	vector<WORD> stack;  // The stack to store memory addresses
	
	int *keys;  // The state of the HEX keyboard, 16 keys, 1 means pressed, 0 unpressed
	unsigned int delay_timer;
};

#endif  // _CPU_H_
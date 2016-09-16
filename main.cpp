#include <iostream>
#include <iomanip>

#include "SDL.h"
#include "SDL_opengl.h"

#include "CPU.h"
#include "Types.h"

// Handles key presses
void handleKeyPress(CPU cpu, SDL_Event e);
void handleKeyUp(CPU cpu, SDL_Event e);

bool initWindow(SDL_Window **window, SDL_Renderer **rend);
void drawPixels(CPU cpu, SDL_Renderer *rend);

#define GAME_FILE "games/MISSILE"
#define SCALE 6
#define WINDOW_WIDTH WIDTH*SCALE
#define WINDOW_HEIGHT HEIGHT*SCALE

using namespace std;

int main(int argc, char* args[]) {
	
	SDL_Window *window;
	SDL_Renderer *rend;
	if (initWindow(&window, &rend) == false) {
		exit(EXIT_FAILURE);
	}
	
	// Boot the game
	CPU cpu = CPU(GAME_FILE);

	unsigned int time = SDL_GetTicks();
	unsigned int clockSpeed = 1000 / 60;
	unsigned int perframe = 600 / 60;

	bool run = true;
	SDL_Event e;
	while (run) {

		// Handle any key presses on the event queue
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT)
				run = false;
			else if (e.type == SDL_KEYDOWN) {
				handleKeyPress(cpu, e);
			}
			else if (e.type == SDL_KEYUP) {
				handleKeyUp(cpu, e);
			}
		}

		unsigned int current = SDL_GetTicks();
		if (time + clockSpeed < current) {
				
			// Update the delay timer
			cpu.decrementTimer();

			// Get opcodes and execute them
			for (unsigned int i = 0; i < perframe; i++) {
				WORD next = cpu.getNextInstruction();
				cout << hex << cpu.program_counter << ": ";
				cout << hex << next << endl;
				
				
				cpu.execute(next);
			}
				
			drawPixels(cpu, rend);  // Update the display

			time = current;  // Set the time of the last exectued intstruction
		}	
	}
	
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

bool initWindow(SDL_Window **window, SDL_Renderer **rend) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return false;
	}
	*window = SDL_CreateWindow("Chip-8",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		cout << "Error creating window" << endl;
		return false;
	}

	*rend = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (rend == nullptr) {
		SDL_DestroyWindow(*window);
		cout << "Error creating renderer" << endl;
		return false;
	}
	// Clear the screen
	SDL_SetRenderDrawColor(*rend, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(*rend);

	return true;
}

void drawPixels(CPU cpu, SDL_Renderer *rend) {
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			if (cpu.pixels[i][j]) {
				SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);  // Black?
			}
			else {
				SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);  // White?
			}
			SDL_Rect pixel = {i * SCALE, j * SCALE, SCALE, SCALE};
			SDL_RenderFillRect(rend, &pixel);
		}
	}
	SDL_RenderPresent(rend);  // display
}

void handleKeyPress(CPU cpu, SDL_Event e) {
	int key = -1;
	switch (e.key.keysym.sym) {  // Key mapping to emulate HEX keypad
	case SDLK_x: key = 0; break;
	case SDLK_1: key = 1; break;
	case SDLK_2: key = 2; break;
	case SDLK_3: key = 3; break;
	case SDLK_q: key = 4; break;
	case SDLK_w: key = 5; break;
	case SDLK_e: key = 6; break;
	case SDLK_a: key = 7; break;
	case SDLK_s: key = 8; break;
	case SDLK_d: key = 9; break;
	case SDLK_z: key = 0xA; break;
	case SDLK_c: key = 0xB; break;
	case SDLK_4: key = 0xC; break;
	case SDLK_r: key = 0xD; break;
	case SDLK_f: key = 0xE; break;
	case SDLK_v: key = 0xF; break;
	default:break;
	}
	if (key > -1 && key < 16) {
		cpu.keydown(key);
	}
}

void handleKeyUp(CPU cpu, SDL_Event e) {
	int key = -1;
	switch (e.key.keysym.sym) {  // Key mapping to emulate HEX keypad
	case SDLK_x: cpu.keyup(0); break;
	case SDLK_1: cpu.keyup(1); break;
	case SDLK_2: cpu.keyup(2); break;
	case SDLK_3: cpu.keyup(3); break;
	case SDLK_q: cpu.keyup(4); break;
	case SDLK_w: cpu.keyup(5); break;
	case SDLK_e: cpu.keyup(6); break;
	case SDLK_a: cpu.keyup(7); break;
	case SDLK_s: cpu.keyup(8); break;
	case SDLK_d: cpu.keyup(9); break;
	case SDLK_z: cpu.keyup(0xA); break;
	case SDLK_c: cpu.keyup(0xB); break;
	case SDLK_4: cpu.keyup(0xC); break;
	case SDLK_r: cpu.keyup(0xD); break;
	case SDLK_f: cpu.keyup(0xE); break;
	case SDLK_v: cpu.keyup(0xF); break;
	default: break;
	}
}
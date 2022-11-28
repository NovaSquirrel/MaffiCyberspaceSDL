#include "puzzle.h"
#define FRAME_LENGTH (16.0+2.0/3.0)
int ScreenWidth, ScreenHeight, ScreenZoom = 2;

SDL_Window *window = NULL;
SDL_Renderer *ScreenRenderer = NULL;
int quit = 0;
int retraces = 0;

SDL_Texture *BlockSheet = NULL;
SDL_Texture *EnemiesSheet = NULL;
SDL_Texture *MaffiSheet = NULL;
SDL_Texture *MiscellaneousSheet = NULL;

void run_game();
void draw_game();
void init_game();

int main(int argc, char *argv[]) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	ScreenWidth = 160*1 * ScreenZoom;
	ScreenHeight = 144*1 * ScreenZoom;

	window = SDL_CreateWindow("Maffi cyberspace game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
	if(!window) {
		 SDL_MessageBox(SDL_MESSAGEBOX_ERROR, "Error", NULL, "Window could not be created! SDL_Error: %s", SDL_GetError());
		 return -1;
	}
	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
		SDL_MessageBox(SDL_MESSAGEBOX_ERROR, "Error", NULL, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
		return -1;
	}
	if( TTF_Init() == -1 ) {
		SDL_MessageBox(SDL_MESSAGEBOX_ERROR, "Error", NULL, "SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
		return -1;
	}
	ScreenRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetScale(ScreenRenderer, 2, 2);
	// ------------------------------------------------------

	BlockSheet = LoadTexture("data/blocks.png", 0);
	EnemiesSheet = LoadTexture("data/enemies.png", 0);
	MaffiSheet = LoadTexture("data/maffi.png", 0);
	MiscellaneousSheet = LoadTexture("data/miscellaneous.png", 0);

	init_game();

	double frametimer = 0;         // For counting milliseconds until a frame has happened
	unsigned int last_time = SDL_GetTicks();

	SDL_Event e;
	while(!quit) {
		while(SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT)
				quit = 1;
		}

		SDL_SetRenderDrawColor(ScreenRenderer, 64, 64, 64, 255);
		SDL_RenderClear(ScreenRenderer);

		draw_game();
		if(SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
			SDL_Delay(16);
		} else {
			SDL_RenderPresent(ScreenRenderer);
		}

		unsigned int this_time = SDL_GetTicks();
		frametimer += this_time - last_time;
		last_time = this_time;
		while(frametimer >= FRAME_LENGTH) {
			frametimer -= FRAME_LENGTH;
			run_game();
		}
	}
	SDL_Quit();

	return 0;
}

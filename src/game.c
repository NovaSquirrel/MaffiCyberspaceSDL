#include "puzzle.h"
struct entity entities[ENTITY_LEN];

int playfield[PLAYFIELD_W][PLAYFIELD_H];

int PlayerX = 40;
int PlayerY = 40;
double CameraX = 0;
double CameraY = 0;
const Uint8 *keyboard;
int framecounter = 0;

enum KeyCode {
  KEY_LEFT  = 0x0001,
  KEY_RIGHT = 0x0002,
  KEY_UP    = 0x0004,
  KEY_DOWN  = 0x0008,
  KEY_A     = 0x0010,
  KEY_B     = 0x0020,
  KEY_RESET = 0x0040,
};

uint16_t KeyDown = 0, KeyNew = 0, KeyLast = 0;
int KeyRepeat = 0;

enum EntityType {
	E_EMPTY,
	E_ARROW,
	E_SHORT_ARROW,
};

int create_entity(int type, int px, int py, int vx, int vy, int var1, int var2, int var3, int var4) {
	int i = 0;
	for(; i<ENTITY_LEN; i++) {
		if(!entities[i].type) {
			break;
		}
	}
	if(i == ENTITY_LEN)
		return -1;
	entities[i].state = STATE_NORMAL;
	entities[i].type = type;
	entities[i].xpos = px;
	entities[i].ypos = py;
	entities[i].xspeed = vx;
	entities[i].yspeed = vy;
	entities[i].var[0] = var1;
	entities[i].var[1] = var2;
	entities[i].var[2] = var3;
	entities[i].var[3] = var4;
	entities[i].timer = 0;
	return i;
}

uint8_t test_level[] = {
	LC_ISLAND, 10, 10, 
	LC_ISLAND, 15, 10,
	LC_ISLAND, 20, 10,
	LC_ISLAND, 20, 15,
	LC_ISLAND, 20, 20,
	LC_ISLAND, 20, 25,
	LC_ISLAND, 20, 30,
	LC_ISLAND, 25, 30,
	LC_ISLAND, 30, 30,
	LC_RECT,   30, 10, 20, 5,
	LC_END,    10, 10,
};

void read_level(uint8_t *level);
void init_game() {
	keyboard = SDL_GetKeyboardState(NULL);
	RandomSeed();

	memset(entities, 0, sizeof(entities));

	read_level(test_level);
}

int PlayerFlips = SDL_FLIP_NONE;
int PlayerSourceX = 0;
int PlayerSourceY = 0;
void run_player() {
	int speed = 1;
	if(KeyDown & KEY_A)
		speed = 2;
	if(KeyDown & KEY_DOWN) {
		PlayerY+=speed;
		PlayerFlips = SDL_FLIP_NONE;
		PlayerSourceX = 0;
		PlayerSourceY = 24;
	}
	if(KeyDown & KEY_UP) {
		PlayerY-=speed;
		PlayerFlips = SDL_FLIP_NONE;
		PlayerSourceX = 0;
		PlayerSourceY = 48;
	}
	if(KeyDown & KEY_LEFT) {
		PlayerX-=speed;
		PlayerFlips = SDL_FLIP_HORIZONTAL;
		PlayerSourceX = 0;
		PlayerSourceY = 0;
	}
	if(KeyDown & KEY_RIGHT) {
		PlayerX+=speed;
		PlayerFlips = SDL_FLIP_NONE;
		PlayerSourceX = 0;
		PlayerSourceY = 0;
	}
	if(KeyNew & KEY_A) {
	}
}

void run_entity() {
//	static const int dir_x[4] = {1, 0, -1, 0};
//	static const int dir_y[4] = {0, 1, 0, -1};

	for(int i=0; i<ENTITY_LEN; i++) {
		switch(entities[i].type) {
			case E_ARROW:
				break;
			case E_SHORT_ARROW:
				break;
		}
	}
}

void run_game() {
	KeyLast = KeyDown;
	KeyDown =   (keyboard[SDL_SCANCODE_LEFT]  << 0) |
				(keyboard[SDL_SCANCODE_RIGHT] << 1) |
				(keyboard[SDL_SCANCODE_UP]    << 2) |
				(keyboard[SDL_SCANCODE_DOWN]  << 3) |
				(keyboard[SDL_SCANCODE_X]     << 4) |
				(keyboard[SDL_SCANCODE_Z]     << 5) |
				(keyboard[SDL_SCANCODE_C]     << 6);
	KeyNew = KeyDown & (~KeyLast);

	if(KeyNew & KEY_RESET)
		init_game();

	int TargetCameraX = PlayerX - (ScreenWidth/ScreenZoom/2);
	if(TargetCameraX < 0)
		TargetCameraX = 0;
	if(TargetCameraX > (PLAYFIELD_W-10)*16)
		TargetCameraX = (PLAYFIELD_W-10)*16;
	int TargetCameraY = PlayerY - (ScreenHeight/ScreenZoom/2);
	if(TargetCameraY < 0)
		TargetCameraY = 0;
	if(TargetCameraY > (PLAYFIELD_H-9)*16)
		TargetCameraY = (PLAYFIELD_H-9)*16;

	CameraX += (TargetCameraX - CameraX)/4.0;
	CameraY += (TargetCameraY - CameraY)/4.0;

	run_player();
	run_entity();

	framecounter++;
}

void draw_game() {
	// Do a parallax effect
	int ParallaxCameraOffsetX = (int)CameraX/2 % 16;
	int ParallaxCameraOffsetY = (int)CameraY/2 % 16;
	for(int i=0; i<(ScreenWidth/ScreenZoom); i++) { // 11
		for(int j=0; j<(ScreenHeight/ScreenZoom); j++) { // 10
			blit(BlockSheet, ScreenRenderer, 0, 0, i*16-ParallaxCameraOffsetX, j*16-ParallaxCameraOffsetY, 16, 16);
		}
	}

	// Render the blocks on top of that
	int TopLeftX = round(CameraX) / 16;
	int TopLeftY = round(CameraY) / 16;
	int CameraOffsetX = (int)round(CameraX) % 16;
	int CameraOffsetY = (int)round(CameraY) % 16;
	for(int i=0; i<(ScreenWidth/ScreenZoom); i++) { // 11
		for(int j=0; j<(ScreenHeight/ScreenZoom); j++) { // 10
			int tile = playfield[i+TopLeftX][j+TopLeftY];
			if(tile != 0) {
				blit(BlockSheet, ScreenRenderer, (tile&7)*16, (tile>>3)*16, i*16-CameraOffsetX, j*16-CameraOffsetY, 16, 16);
			}
		}
	}

	blitf(GameSheet, ScreenRenderer, PlayerSourceX, PlayerSourceY, PlayerX-round(CameraX)-8, PlayerY-round(CameraY)-24, 16, 24, PlayerFlips);
}

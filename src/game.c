#include "puzzle.h"
#include <math.h>

// Prototypes

void run_entities();
void draw_entities();

void run_player();
void draw_player();

// External variables
extern int PlayerX;
extern int PlayerY;
extern uint8_t test_level[];

// Variables

int playfield[PLAYFIELD_W][PLAYFIELD_H];
extern int controller_keys;

double CameraX = 0;
double CameraY = 0;
const Uint8 *keyboard;
int framecounter = 0;

uint16_t KeyDown = 0, KeyNew = 0, KeyLast = 0;
int KeyRepeat = 0;

// Code

void read_level(uint8_t *level);
void init_game() {
	keyboard = SDL_GetKeyboardState(NULL);
	RandomSeed();

	memset(entities, 0, sizeof(entities));

	read_level(test_level);
}

int RandomCanBeNegative(uint32_t Bound) {
	int number = Random(Bound);
	return Random(2) ? number : -number;
}

void run_game() {
	KeyLast = KeyDown;
	KeyDown =   (keyboard[SDL_SCANCODE_LEFT]  << 0) |
				(keyboard[SDL_SCANCODE_RIGHT] << 1) |
				(keyboard[SDL_SCANCODE_UP]    << 2) |
				(keyboard[SDL_SCANCODE_DOWN]  << 3) |
				(keyboard[SDL_SCANCODE_X]     << 4) |
				(keyboard[SDL_SCANCODE_Z]     << 5) |
				(keyboard[SDL_SCANCODE_C]     << 6) |
				(keyboard[SDL_SCANCODE_V]     << 7);
	KeyDown |= controller_keys;
	KeyNew = KeyDown & (~KeyLast);

	if(KeyNew & KEY_RESET)
		init_game();

	int TargetCameraX = PlayerX - (ScreenWidth/ScreenZoom/2);
	if(TargetCameraX < 0)
		TargetCameraX = 0;
//	if(TargetCameraX > (PLAYFIELD_W-10)*16)
//		TargetCameraX = (PLAYFIELD_W-10)*16;
	if(TargetCameraX > (PLAYFIELD_W-(ScreenWidth/ScreenZoom/16))*16)
		TargetCameraX = (PLAYFIELD_W-(ScreenWidth/ScreenZoom/16))*16;

	int TargetCameraY = PlayerY - (ScreenHeight/ScreenZoom/2);
	if(TargetCameraY < 0)
		TargetCameraY = 0;
//	if(TargetCameraY > (PLAYFIELD_H-9)*16)
//		TargetCameraY = (PLAYFIELD_H-9)*16;
	if(TargetCameraY > (PLAYFIELD_H-(ScreenHeight/ScreenZoom/16))*16)
		TargetCameraY = (PLAYFIELD_H-(ScreenHeight/ScreenZoom/16))*16;

	CameraX += (TargetCameraX - CameraX)/4.0;
	CameraY += (TargetCameraY - CameraY)/4.0;

	run_player();
	run_entities();

	if((framecounter & 63) == 0 && count_enemies() < 20) {
		// Make an enemy
		int x, y, give_up = 0;
		for(int tries = 20; tries; tries--) {
			if(tries == 1) {
				give_up = 1;
				break;
			}
			double angle = Random(360) / 360.0 * 2 * M_PI;
//			int radius = Random(80-60)+60;
			int radius = Random(120-80)+80;
			x = cos(angle) * radius;
			y = sin(angle) * radius;
/*			
			x = RandomCanBeNegative(80);
			y = RandomCanBeNegative(80);
*/
			if(solid_at_xy(PlayerX+x, PlayerY+y))
				continue;
			if((abs(x) + abs(y)) > 40)
				break;
		}
		if(!give_up)
			create_entity(E_ENEMY_SPAWNING, PlayerX+x, PlayerY+y, 0, 0, 0, 0, 0, 0);
	}


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

	draw_entities();
	draw_player();
}

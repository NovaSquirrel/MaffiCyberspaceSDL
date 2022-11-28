#include "puzzle.h"

// External variables
extern uint16_t KeyDown, KeyNew, KeyLast;

// Variables

int PlayerX = 40;
int PlayerY = 40;
int PlayerOffsetX = 0;

int PlayerFlips = SDL_FLIP_NONE;
int PlayerSourceX = 0;
int PlayerSourceY = 0;

int PlayerShootX = 1;
int PlayerShootY = 0;

// When you press a diagonal, make sure the diagonal is locked in for a little bit?
int DiagonalLockX = 0;
int DiagonalLockY = 0;
int DiagonalLockTimer = 0;

// Code
void do_diagonal_lock() {
	DiagonalLockX = PlayerShootX;
	DiagonalLockY = PlayerShootY;
	DiagonalLockTimer = 4;
}

void run_player() {
	// Move in response to pressing keys
	int speed = 1;
	int MoveX = 0;
	int MoveY = 0;

	if(DiagonalLockTimer)
		DiagonalLockTimer--;
	switch(KeyDown & (KEY_LEFT | KEY_DOWN | KEY_UP | KEY_RIGHT)) {
		case KEY_RIGHT:
			PlayerShootX = 1;
			PlayerShootY = 0;
			break;
		case KEY_DOWN:
			PlayerShootX = 0;
			PlayerShootY = 1;
			break;
		case KEY_LEFT:
			PlayerShootX = -1;
			PlayerShootY = 0;
			break;
			break;
		case KEY_UP:
			PlayerShootX = 0;
			PlayerShootY = -1;
			break;

		case KEY_RIGHT | KEY_DOWN:
			PlayerShootX = 1;
			PlayerShootY = 1;
			do_diagonal_lock();
			break;
		case KEY_LEFT | KEY_DOWN:
			PlayerShootX = -1;
			PlayerShootY = 1;
			do_diagonal_lock();
			break;
		case KEY_LEFT | KEY_UP:
			PlayerShootX = -1;
			PlayerShootY = -1;
			do_diagonal_lock();
			break;
		case KEY_RIGHT | KEY_UP:
			PlayerShootX = 1;
			PlayerShootY = -1;
			do_diagonal_lock();
			break;
	}
	if(DiagonalLockTimer) {
		PlayerShootX = DiagonalLockX;
		PlayerShootY = DiagonalLockY;
	}

//	if(KeyDown & KEY_A)
//		speed = 2;
	if(KeyDown & KEY_DOWN) {
		MoveY = speed;
		PlayerY+=speed;
		PlayerFlips = SDL_FLIP_NONE;
		PlayerSourceX = 0;
		PlayerSourceY = 24;
		PlayerOffsetX = 2;

		if(solid_at_xy(PlayerX, PlayerY + 1) && (KeyDown & KEY_DEBUG) == 0) {
			PlayerY -= MoveY;
			PlayerY = (PlayerY & ~15) + 15;
		}
	}
	if(KeyDown & KEY_UP) {
		MoveY = -speed;
		PlayerY-=speed;
		PlayerFlips = SDL_FLIP_NONE;
		PlayerSourceX = 0;
		PlayerSourceY = 48;
		PlayerOffsetX = -2;

		if(solid_at_xy(PlayerX, PlayerY - 1) && (KeyDown & KEY_DEBUG) == 0) {
			PlayerY -= MoveY;
			PlayerY = (PlayerY & ~15) + 1;
		}
	}

	if(KeyDown & KEY_LEFT) {
		MoveX = -speed;
		PlayerX-=speed;
		PlayerFlips = SDL_FLIP_HORIZONTAL;
		PlayerSourceX = 0;
		PlayerSourceY = 0;
		PlayerOffsetX = 2;

		if(solid_at_xy(PlayerX - 5, PlayerY) && (KeyDown & KEY_DEBUG) == 0) {
			PlayerX = ((PlayerX - MoveX) & ~15) + 5;
		}
	}
	if(KeyDown & KEY_RIGHT) {
		MoveX = speed;
		PlayerX+=speed;
		PlayerFlips = SDL_FLIP_NONE;
		PlayerSourceX = 0;
		PlayerSourceY = 0;
		PlayerOffsetX = -2;

		if(solid_at_xy(PlayerX + 5, PlayerY) && (KeyDown & KEY_DEBUG) == 0) {
			PlayerX = ((PlayerX - MoveX) & ~15) + 16 - 5;
		}
	}

	if(KeyNew & KEY_A) {
		create_entity(E_PLAYER_SHOT, PlayerX+PlayerShootX*8, PlayerY+PlayerShootY*8, PlayerShootX*2, PlayerShootY*2, 0, 0, 0, 0);
//		create_entity(E_PLAYER_SHOT, PlayerX, PlayerY, PlayerShootX*2, PlayerShootY*2, 0, 0, 0, 0);
	}
}

int walk_animation_frames[4] = {0, 16, 0, 32};

void draw_player() {
	int SourceOffsetX = 0;
	int PlayerTempOffsetX = 0;
	if(KeyDown & (KEY_LEFT | KEY_DOWN | KEY_UP | KEY_RIGHT)) {
		SourceOffsetX = walk_animation_frames[(framecounter>>2)&3];
	}
	if(KeyDown & KEY_A) {
		SourceOffsetX = 48;
		if(PlayerSourceY == 0) {
			PlayerTempOffsetX = PlayerFlips ? -2 : 2;
		}
	}
	blitf(MaffiSheet, ScreenRenderer, PlayerSourceX+SourceOffsetX, PlayerSourceY, PlayerX-round(CameraX)-8+PlayerOffsetX+PlayerTempOffsetX, PlayerY-round(CameraY)-24, 16, 24, PlayerFlips);
}

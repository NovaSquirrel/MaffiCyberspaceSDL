#include "puzzle.h"
#include <math.h>

// External variables

extern int PlayerX;
extern int PlayerY;

// Variables

struct entity entities[ENTITY_LEN];

// Code

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
	entities[i].flip = 0;
	return i;
}

int sign(int i) {
	if(i > 0)
		return 1;
	if(i < 0)
		return -1;
	return 0;
}

int touching_entity(int i, int j, int w1, int h1, int w2, int h2) {
	return (abs(entities[i].xpos - entities[j].xpos) * 2 < (w1 + w2)) &&
		(abs(entities[i].ypos - entities[j].ypos) * 2 < (h1 + h2));
}

void entity_get_shot(int e) {
	for(int i=0; i<ENTITY_LEN; i++) {
		switch(entities[i].type) {
			case E_PLAYER_SHOT:
				if(touching_entity(e, i, 16, 12, 8, 4)) {
					entities[e].type = E_EMPTY;
					entities[i].type = E_EMPTY;
				}
				break;
		}
	}
}

int entity_is_enemy(enum EntityType type) {
	switch(type) {
		case E_EMPTY:
		case E_PLAYER_SHOT:
		case E_ENEMY_SHOT:
		case E_POWERPOINT:
		case E_GLOBE:
		case E_WATER:
			return 0;
		default:
			return 1;
	}
}

int count_enemies() {
	int count = 0;
	for(int i=0; i<ENTITY_LEN; i++) {
		if(entity_is_enemy(entities[i].type))
			count++;
	}
	return count;
}

int walk_entity(int i, int x, int y) {
	int bumped = 0;
	entities[i].xpos += x;
	if(x < 0) {
		if(solid_at_xy(entities[i].xpos - 5, entities[i].ypos)) {
			entities[i].xpos = ((int)round(entities[i].xpos - x) & ~15) + 5;
			bumped = 1;
		}

	}
	if(x > 0) {
		if(solid_at_xy(entities[i].xpos + 5, entities[i].ypos)) {
			entities[i].xpos = ((int)round(entities[i].xpos - x) & ~15) + 16 - 5;
			bumped = 1;
		}
	}

	entities[i].ypos += y;
	if(y < 0) {
		if(solid_at_xy(entities[i].xpos, entities[i].ypos - 1)) {
			entities[i].ypos -= y;
			entities[i].ypos = ((int)round(entities[i].ypos) & ~15) + 1;
			bumped = 1;
		}

	}
	if(y > 0) {
		if(solid_at_xy(entities[i].xpos, entities[i].ypos + 1)) {
			entities[i].ypos -= y;
			entities[i].ypos = ((int)round(entities[i].ypos) & ~15) + 15;
			bumped = 1;
		}
	}
	return bumped;
}

void run_entities() {
//	static const int dir_x[4] = {1, 0, -1, 0};
//	static const int dir_y[4] = {0, 1, 0, -1};

	int map_x, map_y;
	for(int i=0; i<ENTITY_LEN; i++) {
		if(abs(entities[i].xpos - PlayerX) > 220
		|| abs(entities[i].ypos - PlayerY) > 220) {
			entities[i].type = 0;
			continue;
		}

		switch(entities[i].type) {
			case E_PLAYER_SHOT:
				entities[i].xpos += entities[i].xspeed;
				entities[i].ypos += entities[i].yspeed;
				entities[i].timer++;
				if(entities[i].timer >= 30) {
					entities[i].type = 0;
				}
				map_x = round(entities[i].xpos)/16;
				map_y = round(entities[i].ypos)/16;
				if(playfield[map_x][map_y] == T_BRICKS) {
					playfield[map_x][map_y] = T_FLOOR;
					entities[i].type = 0;
				}
				break;

			case E_WALKER:
				if(framecounter & 1)
					walk_entity(i, sign(PlayerX - entities[i].xpos), sign(PlayerY - entities[i].ypos));
				entity_get_shot(i);
				break;
			case E_KITTY:
				break;
			case E_BALL:
				break;
			case E_SMASH:
				break;
			case E_GEORGE:
				break;
			case E_BURGER:
				break;
			case E_MINE:
				break;
			case E_BONZI:
				break;
			case E_CLIPPY:
				break;
			case E_CLIPPY_POWERFUL:
				break;
			case E_HACKER:
				break;
			case E_ROVER:
				break;

			case E_ENEMY_SHOT:
				break;
			case E_POWERPOINT:
				break;
			case E_GLOBE:
				break;
			case E_POOF:
				break;
			case E_WATER:
				break;
			case E_ENEMY_SPAWNING:
				entities[i].timer++;
				if(entities[i].timer == 60) {
					entities[i].timer = 0;
					entities[i].type = E_WALKER;
				}
				break;
		}
	}
}

void draw_sprite_at_xy(SDL_Texture* src, int source_x, int source_y, int x, int y, int w, int h, SDL_RendererFlip flip) {
	blitf(src, ScreenRenderer, source_x, source_y, x-round(CameraX)-w/2, y-round(CameraY)-h, w, h, flip);
}

void draw_entities() {
	int animate = (((framecounter>>3)&1)*16);
	for(int i=0; i<ENTITY_LEN; i++) {
		switch(entities[i].type) {
			case E_PLAYER_SHOT:
//				draw_sprite_at_xy(MiscellaneousSheet, 8, 0, entities[i].xpos, entities[i].ypos-4+2*sin((double)framecounter/8+i), 8, 8, SDL_FLIP_NONE);
				draw_sprite_at_xy(MiscellaneousSheet, 8, 0, entities[i].xpos, entities[i].ypos-4, 8, 8, SDL_FLIP_NONE);
				break;

			case E_WALKER:
				draw_sprite_at_xy(EnemiesSheet, 0+animate, 0, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;
			case E_KITTY:
				draw_sprite_at_xy(EnemiesSheet, 32+animate, 0, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;
			case E_BALL:
				draw_sprite_at_xy(EnemiesSheet, 64, 0, entities[i].xpos, entities[i].ypos, 16, 16, SDL_FLIP_NONE);
				break;
			case E_SMASH:
				draw_sprite_at_xy(EnemiesSheet, 80, 0, entities[i].xpos, entities[i].ypos, 16, 16, SDL_FLIP_NONE);
				break;
			case E_GEORGE:
				draw_sprite_at_xy(EnemiesSheet, 96, 0, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;
			case E_BURGER:
				draw_sprite_at_xy(EnemiesSheet, 0, 16, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;
			case E_MINE:
				draw_sprite_at_xy(EnemiesSheet, 16, 16, entities[i].xpos, entities[i].ypos, 16, 16, SDL_FLIP_NONE);
				break;
			case E_BONZI:
				draw_sprite_at_xy(EnemiesSheet, 0+animate, 32, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;
			case E_CLIPPY:
				draw_sprite_at_xy(EnemiesSheet, 48+animate, 32, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;
			case E_CLIPPY_POWERFUL:
				draw_sprite_at_xy(EnemiesSheet, 0+animate, 48, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;
			case E_HACKER:
				draw_sprite_at_xy(EnemiesSheet, 64, 48, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;
			case E_ROVER:
				draw_sprite_at_xy(EnemiesSheet, 32+animate, 48, entities[i].xpos, entities[i].ypos, 16, 16, entities[i].flip?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
				break;

			case E_ENEMY_SHOT:
				draw_sprite_at_xy(MiscellaneousSheet, 56, 0, entities[i].xpos, entities[i].ypos-4, 8, 8, SDL_FLIP_NONE);
				break;
			case E_POWERPOINT:
				break;
			case E_GLOBE:
				draw_sprite_at_xy(EnemiesSheet, 32, 32, entities[i].xpos, entities[i].ypos, 16, 16, SDL_FLIP_NONE);
				break;
			case E_POOF:
				break;
			case E_WATER:
				break;
			case E_ENEMY_SPAWNING:
				draw_sprite_at_xy(EnemiesSheet, 32, 16, entities[i].xpos, entities[i].ypos, 16, 16, SDL_FLIP_NONE);
				break;
		}
	}
}

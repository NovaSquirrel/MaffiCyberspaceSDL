#ifndef PLATFORMER_HEADER
#define PLATFORMER_HEADER
#define NO_STDIO_REDIRECT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <dirent.h>
#include <stdarg.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#define PLAYFIELD_W 64
#define PLAYFIELD_MASK_W 63
#define PLAYFIELD_H 64
#define PLAYFIELD_MASK_H 63

extern int playfield[PLAYFIELD_W][PLAYFIELD_H];
extern int block_flags[];
extern int framecounter;
extern double CameraX;
extern double CameraY;

enum KeyCode {
	KEY_LEFT  = 0x0001,
	KEY_RIGHT = 0x0002,
	KEY_UP    = 0x0004,
	KEY_DOWN  = 0x0008,
	KEY_A     = 0x0010,
	KEY_B     = 0x0020,
	KEY_RESET = 0x0040,
	KEY_DEBUG = 0x0080,
};

enum TileType {
	T_EMPTY,
	T_FLOOR,
	T_STAR,
	T_HEART,
	T_CIRCLE,

	T_PRIZE,
	T_PRIZE_ANIMATION_1,
	T_PRIZE_ANIMATION_2,
	T_USED_PRIZE,

	T_BRICKS,
	T_CRACKED_BRICKS,
	T_TELEPORT,
	T_EXIT,
	T_SPRING,
	T_FIRE,
	T_BOMB,
	T_KEY,
	T_LOCK,
	T_TOGGLE_BUTTON,
	T_TOGGLE_ON,
	T_TOGGLE_OFF,

	T_UNUSED1,
	T_UNUSED2,
	T_UNUSED3,

	T_WALL,
	T_WALL_U,
	T_WALL_D,
	T_WALL_DU,
	T_WALL_R,
	T_WALL_RU,
	T_WALL_RD,
	T_WALL_RDU,
	T_WALL_L,
	T_WALL_LU,
	T_WALL_LD,
	T_WALL_LDU,
	T_WALL_LR,
	T_WALL_LRU,
	T_WALL_LRD,
	T_WALL_LRDU,
};

enum TileFlag {
	TF_SOLID = 1
};

enum EntityType {
	E_EMPTY,
	E_PLAYER_SHOT,

	E_WALKER,
	E_KITTY,
	E_BALL,
	E_SMASH,
	E_GEORGE,
	E_BURGER,
	E_MINE,
	E_BONZI,
	E_CLIPPY,
	E_CLIPPY_POWERFUL,
	E_HACKER,
	E_ROVER,

	E_ENEMY_SHOT,
	E_POWERPOINT,
	E_GLOBE,
	E_POOF,
	E_WATER,
	E_ENEMY_SPAWNING,
};

enum LevelCommand {
	LC_END,
	LC_ISLAND,         // x y
	LC_ISLAND_SMALL,   // x y
	LC_ISLAND_TINY,    // x y
	LC_CUSTOM_ISLAND,  // x y w h offx offy tries
	LC_RECT,           // x y w h
	LC_TYPE,           // type
};

enum entity_state {
	STATE_NORMAL,
	STATE_PAUSE,
	STATE_STUNNED,
	STATE_ACTIVE,
	STATE_INIT,
};

struct entity {
	int type;
	enum entity_state state;

	double xpos;
	double ypos;

	double xspeed;
	double yspeed;

	int var[4]; // generic variables
	int timer;
	int flip;   // Flip horizontally?
};
#define ENTITY_LEN 64
extern struct entity entities[ENTITY_LEN];


extern int ScreenWidth, ScreenHeight, ScreenZoom;
extern SDL_Window *window;
extern SDL_Renderer *ScreenRenderer;
extern int retraces;

extern SDL_Texture *BlockSheet;
extern SDL_Texture *EnemiesSheet;
extern SDL_Texture *MaffiSheet;
extern SDL_Texture *MiscellaneousSheet;

void strlcpy(char *Destination, const char *Source, int MaxLength);

// Drawing
void SDL_MessageBox(int Type, const char *Title, SDL_Window *Window, const char *fmt, ...);
SDL_Surface *SDL_LoadImage(const char *FileName, int Flags);
SDL_Texture *LoadTexture(const char *FileName, int Flags);
void rectfill(SDL_Renderer *Bmp, int X1, int Y1, int Width, int Height);
void rect(SDL_Renderer *Bmp, int X1, int Y1, int X2, int Y2);
void sblit(SDL_Surface* SrcBmp, SDL_Surface* DstBmp, int SourceX, int SourceY, int DestX, int DestY, int Width, int Height);
void blit(SDL_Texture* SrcBmp, SDL_Renderer* DstBmp, int SourceX, int SourceY, int DestX, int DestY, int Width, int Height);
void blitf(SDL_Texture* SrcBmp, SDL_Renderer* DstBmp, int SourceX, int SourceY, int DestX, int DestY, int Width, int Height, SDL_RendererFlip Flip);
void blitz(SDL_Texture* SrcBmp, SDL_Renderer* DstBmp, int SourceX, int SourceY, int DestX, int DestY, int Width, int Height, int Width2, int Height2);
void blitfull(SDL_Texture* SrcBmp, SDL_Renderer* DstBmp, int DestX, int DestY);

// Random
void RandomSeed();
uint32_t RandomRaw();
uint32_t Random(uint32_t Bound);
uint32_t RandomMinMax(uint32_t Min, uint32_t Max);

// Game related
int solid_at_xy(int x, int y);
int type_at_xy(int x, int y);
int create_entity(int type, int px, int py, int vx, int vy, int var1, int var2, int var3, int var4);
int entity_is_enemy(enum EntityType type);
int count_enemies();

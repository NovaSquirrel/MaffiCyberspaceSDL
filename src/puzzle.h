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

enum TileType {
	T_EMPTY,
	T_FLOOR,
	T_WALL,
	T_BRICKS,
	T_STAR,
	T_HEART,
};

enum LevelCommand {
	LC_END,
	LC_ISLAND,         // x y
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

	int xpos;
	int ypos;

	int xspeed;
	int yspeed;

	int var[4]; // generic variables
	int timer;
};
#define ENTITY_LEN 64
extern struct entity entities[ENTITY_LEN];


extern int ScreenWidth, ScreenHeight, ScreenZoom;
extern SDL_Window *window;
extern SDL_Renderer *ScreenRenderer;
extern int retraces;
extern SDL_Texture *GameSheet;
extern SDL_Texture *BlockSheet;

void SDL_MessageBox(int Type, const char *Title, SDL_Window *Window, const char *fmt, ...);
void strlcpy(char *Destination, const char *Source, int MaxLength);
SDL_Surface *SDL_LoadImage(const char *FileName, int Flags);
SDL_Texture *LoadTexture(const char *FileName, int Flags);
void rectfill(SDL_Renderer *Bmp, int X1, int Y1, int Width, int Height);
void rect(SDL_Renderer *Bmp, int X1, int Y1, int X2, int Y2);
void sblit(SDL_Surface* SrcBmp, SDL_Surface* DstBmp, int SourceX, int SourceY, int DestX, int DestY, int Width, int Height);
void blit(SDL_Texture* SrcBmp, SDL_Renderer* DstBmp, int SourceX, int SourceY, int DestX, int DestY, int Width, int Height);
void blitf(SDL_Texture* SrcBmp, SDL_Renderer* DstBmp, int SourceX, int SourceY, int DestX, int DestY, int Width, int Height, SDL_RendererFlip Flip);
void blitz(SDL_Texture* SrcBmp, SDL_Renderer* DstBmp, int SourceX, int SourceY, int DestX, int DestY, int Width, int Height, int Width2, int Height2);
void blitfull(SDL_Texture* SrcBmp, SDL_Renderer* DstBmp, int DestX, int DestY);

void RandomSeed();
uint32_t RandomRaw();
uint32_t Random(uint32_t Bound);
uint32_t RandomMinMax(uint32_t Min, uint32_t Max);

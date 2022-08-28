#include "puzzle.h"

extern int PlayerX;
extern int PlayerY;

// For flood fill
uint8_t playfield_visited[PLAYFIELD_W][PLAYFIELD_H];

void levelrect(int x, int y, int w, int h, int type) {
	for(int i=0; i<w; i++) {
		for(int j=0; j<h; j++) {
			playfield[(i+x)&PLAYFIELD_MASK_W][(j+y)&PLAYFIELD_MASK_H] = type;
		}
	}
}

void island_at(int x, int y, int max_width, int max_height, int max_offset_x, int max_offset_y, int rects, int type) {
	for(int r=0; r<rects; r++) {
		int width = Random(max_width)+1;
		int height = Random(max_height)+1;
		int oX = Random(Random(max_offset_x)+1)-max_offset_x/2;
		int oY = Random(Random(max_offset_y)+1)-max_offset_y/2;
		levelrect(x+oX-width/2, y+oY-height/2, width, height, T_FLOOR);
	}
}

void floodfill_playfield(int x, int y) {
	if(playfield_visited[x][y] || playfield[x][y] == T_WALL || playfield[x][y] == T_EMPTY || x < 0 || y < 0 || x >= PLAYFIELD_W || y >= PLAYFIELD_H)
		return;
	playfield_visited[x][y] = 1;
	floodfill_playfield(x-1, y);
	floodfill_playfield(x+1, y);
	floodfill_playfield(x, y-1);
	floodfill_playfield(x, y+1);
}

void read_level(uint8_t *level_template) {
	/*
	for(int i=0; i<20; i++) {
		island_at(Random(PLAYFIELD_W), Random(PLAYFIELD_H), 8, 8, 5, 5, 8, T_FLOOR);
	}
	*/

	int level_is_good = 0;
	while(!level_is_good) {
		// Clear the playfield first
		for(int i=0; i<PLAYFIELD_W; i++) {
			for(int j=0; j<PLAYFIELD_H; j++) {
				playfield[i][j] = T_EMPTY;
			}
		}

		// Parse the level
		const uint8_t *level = level_template;
		int done = 0;
		int type = T_FLOOR;
		int player_start_x = 0;
		int player_start_y = 0;
		while(!done) {
			uint8_t x, y, w, h, ox, oy, tries;
			switch(*(level++)) {
				case LC_END:
					player_start_x = *(level++);
					player_start_y = *(level++);
					PlayerX = player_start_x * 16 + 8;
					PlayerY = player_start_y * 16 + 8;
					done = 1;
					break;
				case LC_ISLAND:
					x     = *(level++);
					y     = *(level++);
					island_at(x, y, 8, 8, 5, 5, 8, type);
					break;
				case LC_CUSTOM_ISLAND:
					x     = *(level++);
					y     = *(level++);
					w     = *(level++);
					h     = *(level++);
					ox    = *(level++);
					oy    = *(level++);
					tries = *(level++);
					island_at(x, y, w, h, ox, oy, tries, type);
					break;
				case LC_RECT:
					x     = *(level++);
					y     = *(level++);
					w     = *(level++);
					h     = *(level++);
					levelrect(x-w/2, y-h/2, w, h, type);
					break;
				case LC_TYPE:
					type  = *(level++);
					break;
			}
		}

		// Generate walls
		for(int i=0; i<PLAYFIELD_W/2; i++) {
			for(int j=0; j<PLAYFIELD_H/2; j++) {
				if(Random(4) == 0)
					continue;
				int dotX = i * 2 + 1;
				int dotY = j * 2 + 1;
				if(playfield[dotX][dotY] != T_FLOOR)
					continue;
				playfield[dotX][dotY] = T_WALL;
				int direction = Random(4);

				if(direction == 0)
					playfield[dotX-1][dotY] = T_WALL;
				else if(direction == 1)
					playfield[dotX+1][dotY] = T_WALL;
				else if(direction == 2)
					playfield[dotX][dotY-1] = T_WALL;
				else if(direction == 3)
					playfield[dotX][dotY+1] = T_WALL;
			}
		}

		// Make sure you can traverse the level
		playfield[player_start_x][player_start_y] = T_FLOOR;

		memset(&playfield_visited, 0, sizeof(playfield_visited));
		floodfill_playfield(player_start_x, player_start_y);

		for(int x=0; x<PLAYFIELD_W; x++) {
			for(int y=0; y<PLAYFIELD_H; y++) {
				if(playfield[x][y] == T_FLOOR && !playfield_visited[x][y]) {
					playfield[x][y] = T_HEART;

					int used_directions[4] = {0, 0, 0, 0};
					while(!used_directions[0] || !used_directions[1] || !used_directions[2] || !used_directions[3]) {
						int direction = Random(4);
						if(used_directions[direction])
							continue;
						used_directions[direction] = 1;
						if(direction == 0 && x >= 2 && playfield_visited[x-2][y]) {
							playfield[x-1][y] = T_FLOOR;
							floodfill_playfield(x-1, y);
							break;
						} else if(direction == 1 && y >= 2 && playfield_visited[x][y-2]) {
							playfield[x][y-1] = T_FLOOR;
							floodfill_playfield(x, y-1);
							break;
						} else if(direction == 2 && x <= PLAYFIELD_W-3 && playfield_visited[x+2][y]) {
							playfield[x+1][y] = T_FLOOR;
							floodfill_playfield(x+1, y);
							break;
						} else if(direction == 3 && y <= PLAYFIELD_H-3 && playfield_visited[x][y+2]) {
							playfield[x][y+1] = T_FLOOR;
							floodfill_playfield(x, y+1);
							break;
						}
					}


				}
			}
		}
		fflush(stdout);

		level_is_good = 1;
	}

/*
	for(int i=1; i<PLAYFIELD_W-1; i++) {
		for(int j=1; j<PLAYFIELD_H-1; j++) {
			if(playfield[i][j] == T_FLOOR) {
				if((rand()&3) == 0)
					playfield[i][j] = T_BRICKS;
			}
		}
	}
*/

/*
	for(int i=0; i<50; i++) {
		int x = rand()&PLAYFIELD_MASK_W;
		int y = rand()&PLAYFIELD_MASK_H;
		if(playfield[x][y] == T_EMPTY)
			continue;
		for(int j=0; j<10; j++) {
			int oX = (rand()%((rand()%5)+1))-5/2;
			int oY = (rand()%((rand()%5)+1))-5/2;
			oX = (x + oX) & PLAYFIELD_MASK_W;
			oY = (y + oY) & PLAYFIELD_MASK_H;
			if(playfield[oX][oY] == T_FLOOR)
				playfield[oX][oY] = T_BRICKS;
		}
	}
*/

/*
	// Put a border on everything that needs it
	for(int i=1; i<PLAYFIELD_W-1; i++) {
		for(int j=1; j<PLAYFIELD_H-1; j++) {
			if(playfield[i][j] == T_EMPTY) {
				if((playfield[i-1][j] == T_FLOOR) ||
					(playfield[i+1][j] == T_FLOOR) ||
					(playfield[i][j-1] == T_FLOOR) ||
					(playfield[i][j+1] == T_FLOOR))
				playfield[i][j] = T_WALL;
			}
		}
	}
*/

	// Add bricks?
	for(int x=1; x<PLAYFIELD_W-1; x++) {
		for(int y=1; y<PLAYFIELD_H-1; y++) {
			if(playfield[x][y] == T_FLOOR) {
				if((rand()&15) == 0) {
					for(int j=0; j<5; j++) {
						int oX = Random(Random(5)+1)-5/2;
						int oY = Random(Random(5)+1)-5/2;

						oX = (x + oX) & PLAYFIELD_MASK_W;
						oY = (y + oY) & PLAYFIELD_MASK_H;
						if(playfield[oX][oY] == T_FLOOR)
							playfield[oX][oY] = T_BRICKS;
					}
				}
			}
		}
	}

	// Add stars
	for(int i=1; i<PLAYFIELD_W-1; i++) {
		for(int j=1; j<PLAYFIELD_H-1; j++) {
			if(playfield[i][j] == T_FLOOR) {
				if((rand()&15) == 0)
					playfield[i][j] = T_STAR;
			}
		}
	}

}


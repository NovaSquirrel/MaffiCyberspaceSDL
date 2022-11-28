#include "puzzle.h"

extern int PlayerX;
extern int PlayerY;

// For flood fill
int tiles_to_visit;
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
		int oX = Random(Random(max_offset_x)+1);
		int oY = Random(Random(max_offset_y)+1);
		if(Random(2))
			oX = -oX;
		if(Random(2))
			oY = -oY;
		levelrect(x+oX-width/2, y+oY-height/2, width, height, T_FLOOR);
	}
}

int floodfill_level = 0;
int max_floodfill_level = 0;
/*
void floodfill_playfield(int x, int y) {
	if(playfield_visited[x][y] || playfield[x][y] == T_WALL || playfield[x][y] == T_EMPTY || x < 0 || y < 0 || x >= PLAYFIELD_W || y >= PLAYFIELD_H)
		return;
	floodfill_level++;
	if(floodfill_level > max_floodfill_level) {
		max_floodfill_level = floodfill_level;
	}
	playfield_visited[x][y] = 1;
	tiles_to_visit--;
	floodfill_playfield(x-1, y);
	floodfill_playfield(x+1, y);
	floodfill_playfield(x, y-1);
	floodfill_playfield(x, y+1);
	floodfill_level--;
}
*/

#define FLOODFILL_QUEUE_SIZE 256
#define FLOODFILL_QUEUE_MASK 255
int floodfill_queue_x[FLOODFILL_QUEUE_SIZE];
int floodfill_queue_y[FLOODFILL_QUEUE_SIZE];
int floodfill_write_index = 0;
int floodfill_read_index = 0;
void floodfill_add_to_queue(int x, int y) {
	if(playfield_visited[x][y] || playfield[x][y] == T_WALL || playfield[x][y] == T_EMPTY || x < 0 || y < 0 || x >= PLAYFIELD_W || y >= PLAYFIELD_H)
		return;
	playfield_visited[x][y] = 1;
	// Add to the queue
	floodfill_level++;
	if(floodfill_level > max_floodfill_level)
		max_floodfill_level = floodfill_level;
	floodfill_queue_x[floodfill_write_index] = x;
	floodfill_queue_y[floodfill_write_index] = y;
	floodfill_write_index = (floodfill_write_index+1) & FLOODFILL_QUEUE_MASK;
	if(floodfill_write_index == floodfill_read_index) {
		puts("Circular buffer is full!");
		fflush(stdout);
	}
}
void floodfill_playfield(int x, int y) {
	floodfill_write_index = 0;
	floodfill_read_index = 0;
	floodfill_level = 0;
	floodfill_add_to_queue(x, y);
	while(floodfill_read_index != floodfill_write_index) {
		floodfill_level--;
		//printf("%d -> %d\n", floodfill_read_index, floodfill_write_index);
		int read_x = floodfill_queue_x[floodfill_read_index];
		int read_y = floodfill_queue_y[floodfill_read_index];
		floodfill_add_to_queue(read_x-1, read_y);
		floodfill_add_to_queue(read_x+1, read_y);
		floodfill_add_to_queue(read_x, read_y-1);
		floodfill_add_to_queue(read_x, read_y+1);
		floodfill_read_index = (floodfill_read_index+1) & FLOODFILL_QUEUE_MASK;
	}
}

void fix_unvisited_wall(int x, int y) {
	if((playfield[x][y] == T_FLOOR /*|| playfield[x][y] == T_HEART*/) && !playfield_visited[x][y]) {
		//playfield[x][y] = T_HEART;
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

void read_level(uint8_t *level_template) {
	int level_is_good = 0;
	int player_start_x = 0;
	int player_start_y = 0;
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
				case LC_ISLAND_TINY:
					x     = *(level++);
					y     = *(level++);
					island_at(x, y, 3, 3, 3, 3, 5, type);
					break;
				case LC_ISLAND_SMALL:
					x     = *(level++);
					y     = *(level++);
					island_at(x, y, 5, 5, 3, 3, 5, type);
					break;
				case LC_ISLAND:
					x     = *(level++);
					y     = *(level++);
					island_at(x, y, 8, 8, 3, 3, 8, type);
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

		// Put walls on the very edge of the map
		for(int i=0;i<PLAYFIELD_W;i++) {
			playfield[i][0] = T_EMPTY;
			playfield[i][PLAYFIELD_W-1] = T_EMPTY;
			playfield[0][i] = T_EMPTY;
			playfield[PLAYFIELD_H-1][i] = T_EMPTY;
			/*
			if(playfield[i][0] == T_FLOOR)
				playfield[i][0] = T_WALL;
			if(playfield[i][PLAYFIELD_H-1] == T_FLOOR)
				playfield[i][PLAYFIELD_H-1] = T_WALL;
			if(playfield[0][i] == T_FLOOR)
				playfield[0][i] = T_WALL;
			if(playfield[PLAYFIELD_W-1][i] == T_FLOOR)
				playfield[PLAYFIELD_W-1][i] = T_WALL;
			*/
		}

		// Generate walls
		for(int x=0; x<PLAYFIELD_W/2; x++) {
			for(int y=0; y<PLAYFIELD_H/2; y++) {
				if(Random(4) == 0)
					continue;
				int dotX = x * 2 + 1;
				int dotY = y * 2 + 1;
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

		// Count the number of tiles to visit
		tiles_to_visit = 0;
		for(int x=0; x<PLAYFIELD_W; x++) {
			for(int y=0; y<PLAYFIELD_H; y++) {
				if(playfield[x][y] == T_FLOOR) {
					tiles_to_visit++;
				}
			}
		}
		printf("Want to visit %d tiles\n", tiles_to_visit);

		memset(&playfield_visited, 0, sizeof(playfield_visited));
		max_floodfill_level = 0;
		floodfill_playfield(player_start_x, player_start_y);

		printf("%d tiles left after flood fill\n", tiles_to_visit);
		printf("Max flood fill level %d\n", max_floodfill_level);

		for(int try=0; try<2; try++) {
			// Try to fix the level
			for(int x=0; x<PLAYFIELD_W; x++) {
				for(int y=0; y<PLAYFIELD_H; y++) {
					fix_unvisited_wall(x, y);
				}
			}

			tiles_to_visit = 0;
			for(int x=0; x<PLAYFIELD_W; x++) {
				for(int y=0; y<PLAYFIELD_H; y++) {
					if(playfield[x][y] == T_FLOOR && !playfield_visited[x][y]) {
						tiles_to_visit++;
					}
				}
			}
			printf("%d tiles left to visit\n", tiles_to_visit);

			for(int x=PLAYFIELD_W-1; x>=0; x--) {
				for(int y=PLAYFIELD_H-1; y>=0; y--) {
					fix_unvisited_wall(x, y);
				}
			}

			tiles_to_visit = 0;
			for(int x=0; x<PLAYFIELD_W; x++) {
				for(int y=0; y<PLAYFIELD_H; y++) {
					if(playfield[x][y] == T_FLOOR && !playfield_visited[x][y]) {
						tiles_to_visit++;
					}
				}
			}
			printf("%d tiles left to visit\n", tiles_to_visit);
		}

		fflush(stdout);

		level_is_good = 1;
	}

/*
	// Put a border on everything that needs it
	for(int i=0; i<PLAYFIELD_W; i++) {
		for(int j=0; j<PLAYFIELD_H; j++) {
			if(playfield[i][j] == T_EMPTY) {
				if((i != 0 && playfield[i-1][j] == T_FLOOR) ||
					(i != PLAYFIELD_W-1 && playfield[i+1][j] == T_FLOOR) ||
					(j != 0 && playfield[i][j-1] == T_FLOOR) ||
					(j != PLAYFIELD_H-1 && playfield[i][j+1] == T_FLOOR))
				playfield[i][j] = T_WALL;
			}
		}
	}
*/
	// Autotile the walls
	for(int i=0; i<PLAYFIELD_W; i++) {
		for(int j=0; j<PLAYFIELD_H; j++) {
			if(playfield[i][j] == T_WALL) {
				int autotile = 0;
				if(j != 0 && playfield[i][j-1] >= T_WALL && playfield[i][j-1] <= T_WALL_LRDU)
					autotile |= 1;
				if(j != PLAYFIELD_H-1 && playfield[i][j+1] >= T_WALL && playfield[i][j+1] <= T_WALL_LRDU)
					autotile |= 2;
				if(i != PLAYFIELD_W-1 && playfield[i+1][j] >= T_WALL && playfield[i+1][j] <= T_WALL_LRDU)
					autotile |= 4;
				if(i != 0 && playfield[i-1][j] >= T_WALL && playfield[i-1][j] <= T_WALL_LRDU)
					autotile |= 8;
				playfield[i][j] = T_WALL + autotile;
			}
		}
	}

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

	// Make sure you can traverse the level
	playfield[player_start_x][player_start_y] = T_FLOOR;

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


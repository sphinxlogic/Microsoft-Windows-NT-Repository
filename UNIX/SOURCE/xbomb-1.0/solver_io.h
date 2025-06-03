
/*
 * solver_io.h
 */

#define  MAXWIDTH	100
#define  MAXHEIGHT	100

enum status_t {
	empty, bomb, bomb_flag, empty_flag, edge, unknown, last_status
};

struct generic_cell {
	enum status_t status;
	int neighbors;
	int neighbor_bombs;
	int neighbor_unknowns;
	int ted;
};

extern struct generic_cell map[MAXWIDTH+2][MAXHEIGHT+2];

extern int numbombs;
extern int width, height;

extern int mouse_x, mouse_y;

extern int counts[];


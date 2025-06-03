
/*
 * map.c
 * @(#)map.c	1.27 (UCLA) 10/22/92
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include "xbomb.h"


struct cell map[MAXWIDTH+2][MAXHEIGHT+2];

void
create_cute_map()
{
	int x, y;
	int mode=0;
	int nb = numbombs;
	int ns = width * height;

	onlyprint = 1;   /* so they can't score */

	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++)
			switch (mode) {
			case 0:
				map[x][y].status = EMPTY;
				ns--;
				if (lrand48() % ns >= nb)
					break;
				mode = 1;
			case 1:
				map[x][y].status = BOMB;
				if (!--nb)
					mode = 2;
				break;
			case 2:
				map[x][y].status = EMPTY;
				break;
			};
}


create_map()
{
	int x, y, nb, nok;

	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			map[x][y].status = EMPTY;
			map[x][y].flag = 0;
			map[x][y].neighbors = 0;
			map[x][y].neighbor_kbombs = 0;
		}
	
	for (x = 0; x <= width+1; x++) {
		map[x][0].status = BORDER;
		map[x][height+1].status = BORDER;
	}
	
	for (y = 0; y <= height+1; y++) {
		map[0][y].status = BORDER;
		map[width+1][y].status = BORDER;
	}

	for (nb = numbombs; nb > 0; nb--) {
		do {
			x = lrand48()%width + 1;
			y = lrand48()%height + 1;
			nok = (map[x][y].status != EMPTY);
			if (!startwithzero) 
			  nok = nok || ((x == 1) && (y == 1));
		} while (nok);
		map[x][y].status = BOMB;
	}


	numempty = width*height - numbombs;
	numflags = numbombs;

	/* if (lrand48() % 20 == 0) create_cute_map(); */

	
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++)
			if (map[x][y].status == EMPTY) {
				map[x][y].neighbors = 0;
				apply_neighbor(x,y,apply_inc_if_bomb, &map[x][y].neighbors);
			};

	if (startwithzero) {
		for (x = 1; x <= width; x++)
			for (y = 1; y <= height; y++)
				if ((map[x][y].status == EMPTY) &&
				    (map[x][y].neighbors == 0)) {
					map[x][y].status = KEMPTY;
					follow(x,y);
					return(0);
					}
		for (x = 1; x <= width; x++)
			for (y = 1; y <= height; y++)
				if (map[x][y].status == EMPTY) {
					map[x][y].status = KEMPTY;
					return(0);
					}
		}
	
	if (map[1][1].status == EMPTY) {
		map[1][1].status = KEMPTY;
		if (map[1][1].neighbors == 0) follow(1,1);
	};
}


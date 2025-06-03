
/*
 * actions.c
 * @(#)actions.c	1.4 (UCLA) 10/15/92
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include "xbomb.h"


void
move_somewhere(column, row)
int column, row;
{
	if (map[column][row].status == EMPTY) {
		if (map[column][row].flag) numflags++;
		map[column][row].status = KEMPTY;
		numempty--;
		if (shouldfollow(column, row)) {
			if (follow(column,row))
				dead = 1;
		};
	}
	if (map[column][row].status == BOMB)
		dead = 1;
}

void
place_flag(column, row)
int column, row;
{
	if ((map[column][row].status == EMPTY) ||
	    (map[column][row].status == BOMB)) {
		if (map[column][row].flag) {
			numflags++;
			map[column][row].flag = 0;
		}
		else {
			numflags--;
			map[column][row].flag = 1;
		}
	}
}


int
place_em()
{
	int x, y, ret = 0;
	
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			map[x][y].neighbor_unknowns = 0;
			apply_neighbor(x,y,apply_inc_if_unknown,&map[x][y].neighbor_unknowns);
		};
	
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++)
			if (map[x][y].status==KEMPTY && 
			    map[x][y].neighbors - map[x][y].neighbor_kbombs ==
			    map[x][y].neighbor_unknowns) {
				ret |= apply_neighbor(x,y,apply_flag_if_unknown, NULL);
			};
	return(ret);
}

int scan_em()
{
	int c, r, ret = 0;
	
	for (c = 1; c <= width; c++)
		for (r = 1; r <= height; r++)
			if (map[c][r].flag && 
			    map[c][r].status == EMPTY) {
				dead = 1;
				return(0);
			}
	
	for (c = 1; c <= width; c++)
		for (r = 1; r <= height; r++)
			if (map[c][r].flag && map[c][r].status == BOMB) {
				map[c][r].status = KBOMB;
				ret |= apply_neighbor(c,r,apply_add_neighbor_kbombs, (void*)1);
				putpix(c,r);
			}
	return(ret);
}

shouldfollow(x, y)
	int x, y;
{
	return (map[x][y].neighbors == 0 ||
		(betterfollow && map[x][y].neighbors - map[x][y].neighbor_kbombs == 0));
}

follow(c,r)
	int c, r;
{
	int x, y;
	
	for (x = c-1; x <= c+1; x++)
		for (y = r-1; y <= r+1; y++) {
			if (map[x][y].status == EMPTY) {
				if (map[x][y].flag) numflags++;
				map[x][y].status = KEMPTY;
				numempty--;
				putpix(x, y);
				if (shouldfollow(x,y))
					follow(x,y);
			}
		}
	return 0;   /* OK */
}




int
place_random(what) 
	int what;
{
	int numct, row, column;

	numct = (numempty + numflags - 1);
	if (numct == 0)	/* already solved */
		return(1);

	numct = lrand48()%numct + 1;

	for (row = 1; row <= height; row++) {
		for (column = 1; column <= width; column++) {
		  	if (is_unknown(map[column][row].status)) numct--;
			if (numct == 0) {
				if (what == BOMB) place_flag(column, row);
				else if (what == EMPTY) move_somewhere(column, row);
				else bail("invalid what in place_random");

				putpix(column,row);
				return(0);
				}
			}
		}
	fprintf(stderr, "This message shouldn't happen (but it does)!\n");
	return(0);
}


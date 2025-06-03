
/*
 * pattern_solver
 * @(#)pattern_solver.c	1.7 (UCLA) 10/6/92
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include <stdio.h>
#include "solver_io.h"


#define KNOWN(s) ((s == empty) || (s == bomb) || (s == edge))

struct point {
	int x, y;
	};

struct neighbor_set {
	int size;
	struct point points[8];
	};

struct neighbor_set mymap[MAXWIDTH][MAXHEIGHT];
	
int
apply_empty_if_unknown(x, y, ptr)
	int x, y;
	void *ptr;
{
	if (map[x][y].status==unknown) {
		printf ("%d %d e\n", x, y);
	};
}

int 
apply_flag_if_unknown(x, y, ptr)
	int x, y;
	void *ptr;
{
	if (map[x][y].status==unknown) {
		map[x][y].status = bomb_flag;
		printf ("%d %d f\n", x, y);
		return(1);
	};
	return(0);
}

int
run4(x, y, dx, dy)
{
	int ct = 0;

	if (map[x][y].status == unknown) ct++;
	if (map[x+dx][y].status == unknown) ct++;
	if (map[x][y+dy].status == unknown) ct++;
	if (map[x+dx][y+dy].status == unknown) ct++;

	return(ct);
}

void
add_neighbor(x, y, dx, dy)
int x, y, dx, dy;
{
	int s;

	s = mymap[x][y].size;
	mymap[x][y].points[s].x = x+dx;
	mymap[x][y].points[s].y = y+dy;
	mymap[x][y].size ++;
	}

void
make_neighbors(status, x, y, dx, dy)
int status, x, y;
{
	int i, j, fix;

	for (j = 0; j < mymap[x+dx][y+dy].size; j++) {
		fix = 1;
		for (i = 0; fix && (i < mymap[x][y].size); i++) {
			if ((mymap[x][y].points[i].x == mymap[x+dx][y+dy].points[j].x) &&
			    (mymap[x][y].points[i].y == mymap[x+dx][y+dy].points[j].y)) {
				fix = 0;
				}
			}
		if (fix) {
			printf ("%d %d %c\n", mymap[x+dx][y+dy].points[j].x, mymap[x+dx][y+dy].points[j].y, (status == empty)?'m':'f');
			}
		}
}

int
subset_of(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	int i, j, found;

	for (i = 0; i < mymap[x1][y1].size; i++) {
		found = 0;
		for (j = 0; !found && j < mymap[x2][y2].size; j++) {
			if ((mymap[x1][y1].points[i].x == mymap[x2][y2].points[j].x) &&
			    (mymap[x1][y1].points[i].y == mymap[x2][y2].points[j].y))
				found = 1;
				}
		if (!found) return(0);
		}
	
	return(1);
	}

int
place_em()
{
	int x, y, ret = 0, dx, dy;

	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			mymap[x][y].size = 0;
			if (map[x][y].status == empty &&
			    map[x][y].ted >= 1)
				for (dx = -1; dx <= 1; dx++)
					for (dy = -1; dy <= 1; dy++)
						if (((dx != 0) || (dy != 0)) &&
						    (! KNOWN(map[x+dx][y+dy].status)))
							add_neighbor(x, y, dx, dy);
						}


	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {

/***
			if (map[x][y].status == empty &&
			    map[x][y].ted == 1)
			    {
				for (dx = -1; dx <= 1; dx += 2)
					if (map[x+dx][y].status == empty &&
					    map[x+dx][y].ted == 1 &&
					    KNOWN(map[x-dx][y-1].status) &&
					    KNOWN(map[x-dx][y].status) &&
					    KNOWN(map[x-dx][y+1].status)) {
						apply_empty_if_unknown(x+2*dx,
								       y+1,NULL);
						apply_empty_if_unknown(x+2*dx,
								       y,NULL);
						apply_empty_if_unknown(x+2*dx,
								       y-1,NULL);
					};

				for (dy = -1; dy <= 1; dy += 2)
					if (map[x][y+dy].status==empty &&
					    map[x][y+dy].ted == 1 &&
					    KNOWN(map[x-1][y-dy].status) &&
					    KNOWN(map[x][y-dy].status) &&
					    KNOWN(map[x+1][y-dy].status)) {
						apply_empty_if_unknown(x+1,y+2*dy,NULL);
						apply_empty_if_unknown(x,y+2*dy,NULL);
						apply_empty_if_unknown(x-1,y+2*dy,NULL);
					};
			};

***/

			if (map[x][y].status == empty && 
			    map[x][y].ted > 1 &&
			    map[x][y].neighbor_unknowns - map[x][y].ted == 1
			    )
			    {
				for (dx = -1; dx <= 1; dx += 2)
					if (map[x-dx][y].status == empty &&
					    map[x-dx][y].ted == 1 &&
					    run4(x-dx, y-1, dx, 2) == 2
					    ) 
					    {
						  apply_flag_if_unknown(x+dx,
								y-1,NULL);
						  apply_flag_if_unknown(x+dx,
								y,NULL);
						  apply_flag_if_unknown(x+dx,
								y+1,NULL);
						  }

				for (dy = -1; dy <= 1; dy += 2)
					if (map[x][y-dy].status == empty &&
					    map[x][y-dy].ted == 1 &&
					    run4(x-1, y-dy, 2, dy) == 2
					    )
					    {
						  apply_flag_if_unknown(x-1,
								y+dy,NULL);
						  apply_flag_if_unknown(x,
								y+dy,NULL);
						  apply_flag_if_unknown(x+1,
								y+dy,NULL);
						  }
				
			};
		};


	for (x = 1; x <= width; x++) for (y = 1; y <= height; y++)
		if (map[x][y].status == empty &&
		    map[x][y].ted >= 1)
			for (dx = -2; dx <= 2; dx++)
				for (dy = -2; dy <= 2; dy++)
					if (((dx != 0) || (dy != 0)) &&
					    (x+dx <= width) &&
					    (x+dx >= 1) &&
					    (y+dy <= height) &&
					    (y+dy >= 1) && 
					    (map[x+dx][y+dy].status == empty) &&
		    			    (map[x+dx][y+dy].ted >= 1) &&
					    subset_of(x, y, x+dx, y+dy)) {
						if (map[x][y].ted == map[x+dx][y+dy].ted)
							make_neighbors(empty, x, y, dx, dy);
						if (map[x][y].ted == map[x+dx][y+dy].ted + mymap[x][y].size - mymap[x+dx][y+dy].size)
							make_neighbors(bomb, x, y, dx, dy);
							}
	
	return(ret);
}
	
main()
{
	int ret; int x, y;

	while (!feof(stdin)) {
		read_data();
		if (counts[bomb_flag])
			exit (1);   /* we don't allow flags, yet */
		calc_em();
		ret = place_em();
		printf ("\n");   /* terminate output */
		fflush(stdout);
	};
	exit (0);
}

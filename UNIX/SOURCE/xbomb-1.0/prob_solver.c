
/*
 * probabalistic flag identifier
 * @(#)prob_solver.c	1.3 (UCLA) 10/6/92
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
	
struct cell {
	float prob, inc;
};

struct cell mymap[MAXWIDTH+2][MAXHEIGHT+2];

int
place_em()
{
	int x, y, ret = 0, dx, dy;
	int maxx, maxy, found;
	float inc, max, tot;
	
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			if ((map[x][y].status == empty) &&
			   (map[x][y].neighbor_unknowns > 0))
				mymap[x][y].inc = 
					((float) map[x][y].ted) / 
					((float) map[x][y].neighbor_unknowns);
			else mymap[x][y].inc = 0.0;
			}
			

	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			mymap[x][y].prob = 0.0;
			if (map[x][y].status == unknown) {
				for (dx = -1; dx <= 1; dx++)
					for (dy = -1; dy <= 1; dy++)
						mymap[x][y].prob += 
							mymap[x+dx][y+dy].inc;
				}
			}

	found = 0;

	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++)
			if (map[x][y].status == unknown) {

				if (!found || (mymap[x][y].prob > max)) {
					maxx = x; maxy = y;
					max = mymap[x][y].prob;
					found = 1;
					}

/*
				for (dx = -1; dx <= 1; dx++)
					for (dy = -1; dy <= 1; dy++)
						if (((dx != 0) || (dy != 0)) &&
						    map[x+dx][y+dy].status == unknown)
							tot -= mymap[x+dx][y+dy].prob;
						if (!found || (tot > max)) {
							maxx = x; maxy = y;
							max = tot;
							found = 1;
							}
*/

				}
	
	printf("%d %d f\n", maxx, maxy);

	fprintf(stderr, "%d %d f %0.4f\n", maxx, maxy, max);
	
	return(ret);
}

main()
{
	int ret;
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

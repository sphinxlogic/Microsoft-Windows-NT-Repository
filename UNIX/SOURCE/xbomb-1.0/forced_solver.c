
/*
 * forced_solver -
 * Do the corner rule.
 *
 * Improved 22-Sep-92 to also do the basic follow rule.
 *
 * forced_solver is Copyright (C) 1992 by John Heidemann, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include <stdio.h>
#include "solver_io.h"


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
apply_move_if_unknown(x, y, ptr)
	int x, y;
	void *ptr;
{
	if (map[x][y].status==unknown) {
		map[x][y].status = bomb_flag;
		printf ("%d %d m\n", x, y);
		return(1);
	};
	return(0);
}


int
place_em()
{
	int x, y, ret = 0;
	
	/*
	 * This is the basic follow rule.
	 * If ted==0, it's empty.
	 */
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++)
			if (map[x][y].status==empty && 
			    map[x][y].ted == 0) {
				ret |= apply_neighbor(x,y,apply_move_if_unknown, NULL);
			};

	/*
	 * This is the basic corner rule.
	 * If ted==empties, it's a bomb.
	 */
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++)
			if (map[x][y].status==empty && 
			    map[x][y].ted == map[x][y].neighbor_unknowns) {
				ret |= apply_neighbor(x,y,apply_flag_if_unknown, NULL);
			};
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

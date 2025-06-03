
/*
 * applications.c
 * @(#)applications.c	1.2 (UCLA) 10/6/92
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include "xbomb.h"


int
apply_add_neighbor_kbombs(x, y, ptr)
	int x, y;
	void *ptr;
{
	int delta = (int)ptr;
	map[x][y].neighbor_kbombs += delta;
	if (map[x][y].status == KEMPTY && ted) putpix(x,y);
	if (map[x][y].status == KEMPTY && shouldfollow(x,y)) {
		follow(x,y);
		return(1);
	}
	return(0);
}

int
apply_inc_if_unknown(x, y, ptr)
	int x, y;
	void *ptr;
{
	int *ip = (int*)ptr;
	if (is_unknown(map[x][y].status))
		(*ip)++;
	return(0);
}

int
apply_inc_if_bomb(x, y, ptr)
	int x, y;
	void *ptr;
{
	int *ip = (int*)ptr;
	if (is_bomb(map[x][y].status))
		(*ip)++;
	return(0);
}

int 
apply_flag_if_unknown(x, y, ptr)
	int x, y;
	void *ptr;
{
	if (is_unknown(map[x][y].status) && !map[x][y].flag) {
		numflags--;
		map[x][y].flag = 1;
		putpix(x, y);
		return(1);
	};
	return(0);
}

int
add_neighbor_kbombs(c,r, delta)
	int c,r, delta;
{
	int x, y;
	
	for (x = c-1; x <= c+1; x++)
		for (y = r-1; y <= r+1; y++) {
			if (x==c && y==r)
				continue;
			map[x][y].neighbor_kbombs += delta;
			if (map[x][y].status == KEMPTY && ted) putpix(x,y);
			if (map[x][y].status == KEMPTY && shouldfollow(x,y))
				follow(x,y);
		};
}

int
apply_neighbor(c,r, f, ptr)
	int c,r;
	int (*f)();
	void *ptr;
{
	int x, y, ret = 0;
	
	for (x = c-1; x <= c+1; x++)
		for (y = r-1; y <= r+1; y++) {
			if (x==c && y==r)
				continue;
			ret |= (*f)(x, y, ptr);
		};
	
	return(ret);
}



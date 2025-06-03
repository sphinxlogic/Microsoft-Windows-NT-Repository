
/*
 * solver_io.c
 * @(#)solver_io.c	1.3 (UCLA) 10/6/92
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

struct generic_cell map[MAXWIDTH+2][MAXHEIGHT+2];

int numbombs;
int width, height;

int mouse_x, mouse_y;


int counts[(int)last_status];


void
bail(s)
	char *s;
{
	perror (s);
	exit(1);
}
		

void
read_data()
{
	char s[MAXWIDTH+3];
	int x, y;
	char *p;
	static char *delimiters = " \t\n";
	extern char *strtok();

	for (x=0; x<=MAXWIDTH+1; x++)
		for (y=0; y<=MAXHEIGHT+1; y++)
			map[x][y].status = edge;

	for (;;) {
		if (NULL==fgets (s, MAXWIDTH+3, stdin))
			break;
		if (s[0] == '\n' || s[0]  == 0)
			break;
		s[strlen(s)-1] = 0;  /* chop newline */
		p = strtok(s, delimiters);  /* get & terminate first word */
		if (strcmp(s,"numbombs")==0) {
			sscanf(p, "%d", &numbombs);
		} else if (strcmp(s,"mouse")==0) {
			sscanf(p, "%d %d", &mouse_x, &mouse_y);
		} else if (strcmp(s,"map")==0) {
			break;
		};
	};
	for (y=1;;y++) {
		if (NULL==fgets (s, MAXWIDTH+3, stdin))
			break;
		/* emtpy line terminates input */
		if (s[0] == '\n' || s[0]  == 0)
			break;
		width = strlen(s) - 1;
		for (x=1, p=s; *p && *p != '\n'; p++, x++) {
			switch (*p) {
			case 'X':
				map[x][y].status = bomb;
				break;
			case 'f':
				map[x][y].status = bomb_flag;
				break;
			case 'e':
				bail("Known empty squares not yet handled.");
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				map[x][y].status = empty;
				map[x][y].neighbors = *p - '0';
				break;
			case '.':
				map[x][y].status = unknown;
				break;
			default:
				bail("Bad square value.");
			};
			counts[map[x][y].status]++;
		};
	};
	height = y-1;
}


int
apply_inc_if_unknown(x, y, ptr)
	int x, y;
	void *ptr;
{
	int *ip = (int*)ptr;
	if (map[x][y].status==unknown)
		(*ip)++;
	return(0);
}

int
apply_inc_if_bomb(x, y, ptr)
	int x, y;
	void *ptr;
{
	int *ip = (int*)ptr;
	if (map[x][y].status==bomb)
		(*ip)++;
	return(0);
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


int
calc_em()
{
	int x, y, ret = 0, dx, dy;
	
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			map[x][y].neighbor_unknowns = 0;
			apply_neighbor(x,y,apply_inc_if_unknown,&map[x][y].neighbor_unknowns);
		};
	
	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++) {
			map[x][y].neighbor_bombs = 0;
			apply_neighbor(x,y,apply_inc_if_bomb,&map[x][y].neighbor_bombs);
		};

	for (x = 1; x <= width; x++)
		for (y = 1; y <= height; y++)
			if (map[x][y].status == empty)
				map[x][y].ted = map[x][y].neighbors -
					map[x][y].neighbor_bombs;
	
}

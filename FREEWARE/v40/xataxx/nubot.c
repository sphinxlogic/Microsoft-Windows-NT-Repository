#include <stdio.h>
#include <X11/Xlib.h>
#include "struct.h"

#define MAXMOVES (700)

#ifdef VMS
#define random rand
#define srandom srand
#endif /* VMS */

typedef struct {
    int ox,oy;
    int nx,ny;
    char jump;
} zthing;

int *tboard[10];

#define square(bd, x, y) (bd[(x)*y_board_size+(y)])

extern int b_look2(); /* return EMPTY, BLACK, WHITE, or OBSTACLE */
extern int x_board_size, y_board_size;
extern int *board, *bl2;
extern int botlevel;

void init_nubot()
{
    register int i, x, y;
    int *bd;

    for (i=0; i<10; i++) {
	bd = (int *) malloc(sizeof(int)*x_board_size*y_board_size);

	for (x=0;x!=x_board_size;x++)
	    for (y=0;y!=y_board_size;y++)
		bd[x*y_board_size+y] = EMPTY;
	tboard[i] = bd;
    }
}

void fcopy_board(bd1, bd2)
int *bd1, *bd2;
{
    bcopy(bd1, bd2, sizeof(int)*x_board_size*y_board_size);  
}

int b_lookr(x, y, bd)
int x, y;
int *bd;
{
    if ((x>=0)&&(x<x_board_size)&&(y>=0)&&(y<y_board_size))
	return(bd[x*y_board_size+y]);
    else
	return(OBSTACLE);
}

void apply_move(player, bd, mv)
int player;
int *bd;
zthing *mv;
{
    register int ix, iy;
    int sq;

    square(bd, mv->nx, mv->ny) = player;
    if ((mv->nx-mv->ox < 2) && (mv->nx-mv->ox > -2) && (mv->ny-mv->oy < 2) && (mv->ny-mv->oy > -2)) {
	/* leave orig alone */
    }
    else {
	square(bd, mv->ox, mv->oy) = EMPTY;
    };

    for (ix=(-1); ix<=1; ix++)
	for (iy=(-1); iy<=1; iy++) {
	    sq = square(bd, mv->nx+ix, mv->ny+iy);
	    if (sq!=EMPTY && sq!=OBSTACLE) 
		square(bd, mv->nx+ix, mv->ny+iy) = player;
	}
}

int valueb(player, oplayer, bd) /* estimate the value of the board for player. oplayer should be the opposite color */
int player, oplayer;
int *bd;
{
    register int ix, iy;
    int good=0, bad=0, empty=0;
    int sq;

    for (ix=0; ix<x_board_size; ix++)
	for (iy=0; iy<y_board_size; iy++) {
	    sq = square(bd, ix, iy);
	    if (sq == player) good++;
	    else if (sq == oplayer) bad++;
	    else if (sq == EMPTY) empty++;
	}

    if (empty==0) {
	if (good>bad) return (10000);
	else return (-10000);
    };
    if (good==0) return (-10000);
    if (bad==0) return (10000);
    return (good-bad);
}

list_moves(player, bd, movel)
int player;
int *bd;
zthing *movel;
{
    register int posx, posy;
    int moves=0;

#define do_check(jflag, x, y, player) \
    if (b_lookr((x), (y), bd)==EMPTY) { \
    movel[moves].nx = (x); \
    movel[moves].ny = (y); \
    movel[moves].ox = (posx); \
    movel[moves].oy = (posy); \
    movel[moves].jump = (jflag); \
    moves++; \
    }

    for (posx=0; posx<x_board_size; posx++)
	for (posy=0; posy<y_board_size; posy++) 
	    if (b_lookr((posx), (posy), bd)==player) {
		do_check(0,posx-1,posy-1,player);
		do_check(0,posx,posy-1,player);
		do_check(0,posx+1,posy-1,player);
		do_check(0,posx+1,posy,player);
		do_check(0,posx+1,posy+1,player);
		do_check(0,posx,posy+1,player);
		do_check(0,posx-1,posy+1,player);
		do_check(0,posx-1,posy,player);

		do_check(1,posx-2,posy-2,player);
		do_check(1,posx-1,posy-2,player);
		do_check(1,posx,posy-2,player);
		do_check(1,posx+1,posy-2,player);
		do_check(1,posx+2,posy-2,player);
		do_check(1,posx+2,posy-1,player);
		do_check(1,posx+2,posy,player);
		do_check(1,posx+2,posy+1,player);
		do_check(1,posx+2,posy+2,player);
		do_check(1,posx+1,posy+2,player);
		do_check(1,posx,posy+2,player);
		do_check(1,posx-1,posy+2,player);
		do_check(1,posx-2,posy+2,player);
		do_check(1,posx-2,posy+1,player);
		do_check(1,posx-2,posy,player);
		do_check(1,posx-2,posy-1,player);
	    }

    movel[moves].ox = (-1);
}

/* Make a move for player. If no move possible, just return. */
robot_move(player)
int player;
{
    static int initted = 0;
    zthing movel[MAXMOVES];
    int vall[MAXMOVES];
    register int ix;
    int temp, bestval = (-10000);
    int oplayer;
    zthing *best;
    int numbest, numglides, gomove;

    if (!initted) {
	init_nubot();
	initted = 1;
    }

    oplayer = (player==WHITE) ? BLACK : WHITE;

    list_moves(player, board, movel);

    if (movel[0].ox==(-1)) return; /* there are no legal moves */

    if (botlevel==0) {
	for (ix=0; movel[ix].ox!=(-1); ix++);
	ix = random() % ix;
	best = &(movel[ix]);
	
	move_piece(best->ox,best->oy,best->nx,best->ny,player);
	return;
    };
    
    for (ix=0; movel[ix].ox!=(-1); ix++) {
	fcopy_board(board, tboard[0]);
	apply_move(player, tboard[0], &(movel[ix]));
	vall[ix] = -veb(oplayer, tboard[0], 1, 10000);
	if (vall[ix] > bestval) bestval = vall[ix];
    }

    do {
	for (ix=0; movel[ix].ox!=(-1) && (vall[ix]!=bestval || random()%2); ix++);
    } while (movel[ix].ox==(-1));

    numbest = 0;
    numglides = 0;
    for (ix=0; movel[ix].ox!=(-1); ix++) {
/*
	printf("%d: %d,%d -- %d,%d %c (val %d)\n", ix, movel[ix].ox, movel[ix].oy, movel[ix].nx, movel[ix].ny, movel[ix].jump ? 'J' : 'G', vall[ix]);
*/
	if (vall[ix]==bestval) {
	    numbest++;
	    if (!movel[ix].jump) numglides++;
	}
    }
    
    if (numglides) {
	gomove = (random() % numglides) + 1;
/*
	printf("%d moves (%d glides); took %d'th glide ", numbest, numglides, gomove);
*/
	for (ix=0; gomove && movel[ix].ox!=(-1); ix++)
	    if (vall[ix]==bestval && (!movel[ix].jump)) gomove--;

    }
    else {
	gomove = (random() % numbest) + 1;
/*
	printf("%d moves (%d glides); took %d'th move ", numbest, numglides, gomove);
*/
	for (ix=0; gomove && movel[ix].ox!=(-1); ix++)
	    if (vall[ix]==bestval) gomove--;

    }

    ix--;
/*
    printf("(ix=%d) \n", ix);
*/
    best = &(movel[ix]);
/*
    printf("Move is %d,%d -- %d,%d (val %d vs %d)\n", best->ox,best->oy,best->nx,best->ny, vall[ix], bestval);
*/
    move_piece(best->ox,best->oy,best->nx,best->ny,player);
}

int veb(player, given, depth, dee) /* return the value of given board for one player */
int player;
int *given;
int depth;
int dee;
{
    int x,y;
    int oplayer;
    zthing movel[MAXMOVES];
    int vall;
    int temp, bestval = (-10000);
    register int ix;

    oplayer = (player==WHITE) ? BLACK : WHITE;

    if (depth == botlevel) {
	int val = valueb(player, oplayer, given);
	return val;
    }

    list_moves(player, given, movel);
    
    if (movel[0].ox==(-1)) return -10000; /* there are no legal moves */

    for (ix=0; movel[ix].ox!=(-1); ix++) {
	fcopy_board(given, tboard[depth]);
	apply_move(player, tboard[depth], &(movel[ix]));
	vall = -veb(oplayer, tboard[depth], depth+1, -bestval);
	if (vall > bestval) bestval = vall;
	if (bestval >= dee) return bestval;
    }

    return bestval;
}

dump_board(bd)
int *bd;
{
    register int ix, iy;
    int sq;
    for (iy=0; iy<y_board_size; iy++) {
	for (ix=0; ix<x_board_size; ix++) {
	    sq = square(bd, ix, iy);
	    switch (sq) {
		case EMPTY:
		    printf("- ");
		    break;
		case WHITE:
		    printf("W ");
		    break;
		case BLACK:
		    printf("B ");
		    break;
		case OBSTACLE:
		    printf("# ");
		    break;
	    }
	};
	printf("\n");
    }
    printf("\n");
}

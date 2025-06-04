/* -*-c++-*-  */

#ifndef INCLUDE_PRESSUP_H
#define INCLUDE_PRESSUP_H

#define SIDE		9	/* Dimension of board	*/

#define HISFIRST (SIDE*2+1)	/* His best first move	*/
#define MYFIRST (SIDE+SIDE/2-1) /* My best first move	*/
#define BELL 0x07
#define BACKSP 0x08



int	Depth;		/* Search depth (default = 3)	*/
int	Helpflag;
char	FFlag,	    /* -f option: machine goes first */
	BFlag;	    /* Debugging flag		    */
char	Startflag;	/* True on first move only */

char *image;
int Adj[16] = { -1,-1,-1,0,-1,1,0,-1,0,1,1,-1,1,0,1,1};

int	BestMove;		/* Returned by search	*/

#define BBOARD struct bord

struct bord
  {	char board[SIDE*SIDE];
	int  star;
	char red;
	char blue;
  };

BBOARD initb;

BBOARD master, savebd;

char string[20];

int CheckWin(BBOARD *bp);
void asknew(void);
void pboard(BBOARD *bp);
void bcopy(BBOARD *p1,BBOARD *p2);
void dmove(int n);
void move(BBOARD *bp, int n);
int search (BBOARD *bp,int ddepth, int who, int alpha, int beta);
int Help(void);
int getmove(void);

#endif


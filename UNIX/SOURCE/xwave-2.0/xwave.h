/* xwave.h - include file for xwave
 *
 */

/* used scaled-up fixed point numbers for more resolution */
#define SCALE 1024

#define IX(r,c) ((r)*maxcols + (c))

/* structure used to store the data values in the grid of points */
typedef struct pinfo {
	int pos;	/* current vertical position */
	int vel;	/* current vertical velocity */
	int tx,ty;	/* transformed (x,y) point for plotting */
} Pinfo;

typedef struct ftab {
	char *name;	/* name of the function */
	int (*func)();	/* pointer to the function */
} Ftab;

extern int maxrows, maxcols;

extern double springconst;
extern double dampfactor;

extern Pinfo *Generation[];
extern Pinfo *CurGen;
extern Pinfo *NextGen;

/* end */









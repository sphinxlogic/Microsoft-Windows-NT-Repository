/* Wave propagation
 * relax prop func by Friedman and Riddle;
 * the rest by jimmc, Feb '91.
 */

#include <stdio.h>
#include "xwave.h"

int bound[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };
#define boundlrlc bound[0]
#define boundlr   bound[1]
#define boundlrhc bound[2]
#define boundhc   bound[3]
#define boundhrhc bound[4]
#define boundhr   bound[5]
#define boundhrlc bound[6]
#define boundlc   bound[7]

int WaveIndex;
int (*WaveFunc)();

CalcBoundedNext()
{
	int r, c;

	if (boundlr) {
		r = 0;
		for (c=1; c<maxcols-1; c++)
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos;
	}
	if (boundhr) {
		r = maxrows-1;
		for (c=1; c<maxcols-1; c++)
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos;
	}
	if (boundlc) {
		c = 0;
		for (r=1; r<maxrows-1; r++)
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos;
	}
	if (boundhc) {
		c = maxcols - 1;
		for (r=1; r<maxrows-1; r++)
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos;
	}
	if (boundlrlc)
		NextGen[IX(0,0)].pos = CurGen[IX(0,0)].pos;
	if (boundlrhc)
		NextGen[IX(0,maxcols-1)].pos = CurGen[IX(0,maxcols-1)].pos;
	if (boundhrlc)
		NextGen[IX(maxrows-1,0)].pos = CurGen[IX(maxrows-1,0)].pos;
	if (boundhrhc)
		NextGen[IX(maxrows-1,maxcols-1)].pos =
			CurGen[IX(maxrows-1,maxcols-1)].pos;
}

/* The trivial case - next generation is exactly like this one (useful for
 * examing the forcing function without having any propagation effects get
 * in the way)
 */
CalcNone()
{
	int r, c;

	for (r = 0; r < maxrows; r++)
		for (c = 0; c < maxcols; c++)
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos;
}

/* original propagation model written by Friedman and Riddle.
 * A relaxation model; no velocity component, so no wave propagation;
 * next generation position of each point is independent of current position.
 */
CalcRelax()
{
	int r, c;

	for (r = 1; r < maxrows - 1; r++)
		for (c = 1; c < maxcols - 1; c++)
			NextGen[IX(r,c)].pos = (int) (0.25 *
			    ( CurGen[IX(r - 1,c)].pos
			    + CurGen[IX(r,c - 1)].pos
			    + CurGen[IX(r,c + 1)].pos
			    + CurGen[IX(r + 1,c)].pos));

	/* boundary stuff added by jimmc */
	if (!boundlr) {
		r = 0;
		for (c=1; c<maxcols-1; c++) {
			NextGen[IX(r,c)].pos = (int) (
			    ( CurGen[IX(r,c - 1)].pos
			    + CurGen[IX(r,c + 1)].pos
			    + CurGen[IX(r + 1,c)].pos) / 3.0 );
		}
	}
	if (!boundhr) {
		r = maxrows-1;
		for (c=1; c<maxcols-1; c++) {
			NextGen[IX(r,c)].pos = (int) (
			    ( CurGen[IX(r,c - 1)].pos
			    + CurGen[IX(r,c + 1)].pos
			    + CurGen[IX(r - 1,c)].pos) / 3.0 );
		}
	}
	if (!boundlc) {
		c = 0;
		for (r=1; r<maxrows-1; r++) {
			NextGen[IX(r,c)].pos = (int) (
			    ( CurGen[IX(r - 1,c)].pos
			    + CurGen[IX(r,c + 1)].pos
			    + CurGen[IX(r + 1,c)].pos) / 3.0 );
		}
	}
	if (!boundhc) {
		c = maxcols - 1;
		for (r=1; r<maxrows-1; r++) {
			NextGen[IX(r,c)].pos = (int) (
			    ( CurGen[IX(r - 1,c)].pos
			    + CurGen[IX(r,c - 1)].pos
			    + CurGen[IX(r + 1,c)].pos) / 3.0 );
		}
	}
	if (!boundlrlc)
		NextGen[IX(0,0)].pos = (int) (
		    ( CurGen[IX(1,0)].pos
		    + CurGen[IX(0,1)].pos) / 2.0 );
	if (!boundlrhc)
		NextGen[IX(0,maxcols-1)].pos = (int) (
		    ( CurGen[IX(1,maxcols-2)].pos
		    + CurGen[IX(0,maxcols-1)].pos) / 2.0 );
	if (!boundhrlc)
		NextGen[IX(maxrows-1,0)].pos = (int) (
		    ( CurGen[IX(maxrows-2,0)].pos
		    + CurGen[IX(maxrows-1,1)].pos) / 2.0 );
	if (!boundhrhc)
		NextGen[IX(maxrows-1,maxcols-1)].pos = (int) (
		    ( CurGen[IX(maxrows-2,maxcols-1)].pos
		    + CurGen[IX(maxrows-1,maxcols-2)].pos) / 2.0 );
	CalcBoundedNext();
}

/* wave propagation using simple damped spring model.
 * Spring model is f = - k*x - b*v where k is spring constant, b is damping 
 * factor.
 * x is sum of offsets from four neighbors (to which other end of springs
 * are attached).
 */
CalcWave()
{
	int r, c;
	int f;

	for (r = 1; r < maxrows - 1; r++) {
		for (c = 1; c < maxcols - 1; c++) {
			f = (int) (springconst * (
			      CurGen[IX(r-1,c)].pos + CurGen[IX(r,c-1)].pos +
			      CurGen[IX(r,c+1)].pos + CurGen[IX(r+1,c)].pos -
			      4*CurGen[IX(r,c)].pos) -
			      dampfactor*CurGen[IX(r,c)].vel);
			/* a=f/m; assume m=1, so a=f */
			NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
						NextGen[IX(r,c)].vel;
		}
	}
	if (!boundlr) {
		r = 0;
		for (c=1; c<maxcols-1; c++) {
			f = (int) (springconst * (
			      CurGen[IX(r,c-1)].pos +
			      CurGen[IX(r,c+1)].pos + CurGen[IX(r+1,c)].pos -
			      3*CurGen[IX(r,c)].pos) -
			      dampfactor*CurGen[IX(r,c)].vel);
			/* a=f/m; assume m=1, so a=f */
			NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
						NextGen[IX(r,c)].vel;
		}
	}
	if (!boundhr) {
		r = maxrows-1;
		for (c=1; c<maxcols-1; c++) {
			f = (int) (springconst * (
			      CurGen[IX(r,c-1)].pos +
			      CurGen[IX(r,c+1)].pos + CurGen[IX(r-1,c)].pos -
			      3*CurGen[IX(r,c)].pos) -
			      dampfactor*CurGen[IX(r,c)].vel);
			/* a=f/m; assume m=1, so a=f */
			NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
						NextGen[IX(r,c)].vel;
		}
	}
	if (!boundlc) {
		c = 0;
		for (r=1; r<maxrows-1; r++) {
			f = (int) (springconst * (
			      CurGen[IX(r+1,c)].pos +
			      CurGen[IX(r,c+1)].pos + CurGen[IX(r-1,c)].pos -
			      3*CurGen[IX(r,c)].pos) -
			      dampfactor*CurGen[IX(r,c)].vel);
			/* a=f/m; assume m=1, so a=f */
			NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
						NextGen[IX(r,c)].vel;
		}
	}
	if (!boundhc) {
		c = maxcols - 1;
		for (r=1; r<maxrows-1; r++) {
			f = (int) (springconst * (
			      CurGen[IX(r+1,c)].pos +
			      CurGen[IX(r,c-1)].pos + CurGen[IX(r-1,c)].pos -
			      3*CurGen[IX(r,c)].pos) -
			      dampfactor*CurGen[IX(r,c)].vel);
			/* a=f/m; assume m=1, so a=f */
			NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
			NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
						NextGen[IX(r,c)].vel;
		}
	}
	if (!boundlrlc) {
		r = 0; c = 0;
		f = (int) (springconst * (
		      CurGen[IX(r,c+1)].pos + CurGen[IX(r+1,c)].pos -
		      2*CurGen[IX(r,c)].pos) -
		      dampfactor*CurGen[IX(r,c)].vel);
		/* a=f/m; assume m=1, so a=f */
		NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
		NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
					NextGen[IX(r,c)].vel;
	}
	if (!boundlrhc) {
		r = 0; c = maxcols-1;
		f = (int) (springconst * (
		      CurGen[IX(r,c-1)].pos + CurGen[IX(r+1,c)].pos -
		      2*CurGen[IX(r,c)].pos) -
		      dampfactor*CurGen[IX(r,c)].vel);
		/* a=f/m; assume m=1, so a=f */
		NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
		NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
					NextGen[IX(r,c)].vel;
	}
	if (!boundhrlc) {
		r = maxrows-1; c = 0;
		f = (int) (springconst * (
		      CurGen[IX(r,c+1)].pos + CurGen[IX(r-1,c)].pos -
		      2*CurGen[IX(r,c)].pos) -
		      dampfactor*CurGen[IX(r,c)].vel);
		/* a=f/m; assume m=1, so a=f */
		NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
		NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
					NextGen[IX(r,c)].vel;
	}
	if (!boundhrhc) {
		r = maxrows-1; c = maxcols-1;
		f = (int) (springconst * (
		      CurGen[IX(r,c-1)].pos + CurGen[IX(r-1,c)].pos -
		      2*CurGen[IX(r,c)].pos) -
		      dampfactor*CurGen[IX(r,c)].vel);
		/* a=f/m; assume m=1, so a=f */
		NextGen[IX(r,c)].vel = CurGen[IX(r,c)].vel + f;
		NextGen[IX(r,c)].pos = CurGen[IX(r,c)].pos +
					NextGen[IX(r,c)].vel;
	}
	CalcBoundedNext();
}

Ftab WaveTab[] = {
	{ "relax", CalcRelax },
	{ "wave", CalcWave },
	{ "none", CalcNone },
};
int WaveTabSize = sizeof(WaveTab)/sizeof(WaveTab[0]);

InitWaveFunc()
{
	WaveIndex = 0;
	WaveFunc = WaveTab[WaveIndex].func;
}

PrintWaveFuncs(fp)
FILE *fp;
{
	int i;

	fprintf(fp,"Wave propagation functions are:\n");
	for (i=0; i<WaveTabSize; i++) {
		fprintf(fp," %s",WaveTab[i].name);
	}
	fprintf(fp,"\n");
}

SetNextWaveFunc()
{
	WaveIndex++;
	if (WaveIndex>=WaveTabSize) WaveIndex = 0;
	WaveFunc = WaveTab[WaveIndex].func;
	printf("Wave propagation function is set to %s\n",
		WaveTab[WaveIndex].name);
}

SetNamedWaveFunc(s)
char *s;
{
	int i;

	for (i=0; i<WaveTabSize; i++) {
		if (strcmp(WaveTab[i].name,s)==0) {
			WaveIndex = i;
			WaveFunc = WaveTab[WaveIndex].func;
			return 1;
		}
	}
	return 0;	/* not found */
}

SetBound(s)
char *s;
{
	int i;
	int t;

	if (!s) return;
	if (strcmp(s,"0")==0) s="00000000";
	if (strcmp(s,"1")==0) s="11111111";
	for (i=0; *s && i<8; i++) {
		bound[i] = (*s++!='0');
	}
}

/* end */

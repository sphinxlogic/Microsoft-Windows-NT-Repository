/* force.c - forcing functions for xwave
 * multi-sine function by Friedman and Riddle; the rest by McBeath.
 */

#include <stdio.h>
#include "xwave.h"

extern int fftime;
extern double sinTab[];
extern int sinTabSize;
extern int omega;
extern int omega2;
extern int forcerow, forcecol;
extern int forceptr, forceptc;
extern int forcefull;
extern int *editedRowVals;
extern int *editedColVals;
extern int *amplitudes;
extern int *frequencies;
extern int didedit;
extern int numEditedPoints;


int ForceIndex;
int (*ForceFunc)();

int ampl = 100;

int ampl1 = 150*SCALE, ampl2 = 100*SCALE;
int inc1 = -1, inc2 = -1;

/* original forcing function by Friedman and Riddle */
ForceFuncMultiSine()
{
	int t;
	int row,col,i;

	t = (fftime++)%sinTabSize;
/* make the assignment only if row and col are in range */
#define CLOAD(r,c)  if (r<maxrows && c<maxcols) CurGen[IX(r,c)].pos

/* load freq. and amp. matrices */
#define FVAL(r,c)   frequencies[r*maxcols+c]
#define AVAL(r,c)   amplitudes[r*maxcols+c]

        if (!didedit) {
   	       /* Load some of the cells */
   	           CLOAD(1,2)   = (int)(ampl1*sinTab[(30*t)%sinTabSize]);
	           CLOAD(2,4)   = (int)(ampl2*sinTab[(25*t)%sinTabSize]);
	           CLOAD(1,6)   = (int)(ampl1*sinTab[19*(10+t)%sinTabSize]);
	           CLOAD(2,8)   = (int)(ampl2*sinTab[(20*t)%sinTabSize]);
	           CLOAD(15,8)  = (int)(ampl2*sinTab[(5*t)%sinTabSize]);
	           CLOAD(7,10)  = (int)(ampl2*sinTab[(30*t)%sinTabSize]);
	           CLOAD(9,14)  = (int)(ampl2*sinTab[(20*t)%sinTabSize]);
	           CLOAD(12,16) = (int)((ampl2+50)*sinTab[(3*t)%sinTabSize]);
	           CLOAD(7,3)   = (int)(ampl2*sinTab[(40*t)%sinTabSize]);

  	      /* Increment and/or decrement our amplitudes */
	           if (ampl2 == 0)
		        inc2 = 5*SCALE;
	           else if (ampl2 == 350*SCALE)
		        inc2 = -1*SCALE;
	           ampl2 += inc2;
	           if (ampl1 == 0)
		        inc1 = 5*SCALE;
	           else if (ampl1 == 200*SCALE)
		        inc1 = -1*SCALE;
	           ampl1 += inc1;
	}
        else {  /* wave the edited points instead of set points */
                  row = editedRowVals[0];
                  col = editedColVals[0];
                  for (i=0;i<numEditedPoints;i++) {
                     row = editedRowVals[i];
                     col = editedColVals[i];
                     CurGen[row*maxcols+col].pos = (int)(AVAL(row,col)*SCALE*
                                         sinTab[(FVAL(row,col)*t)%sinTabSize]);
		  }
	}
}

ForceX(x)
int x;
{
	int r,c;

	if (forcerow>=0) {
		for (c=0; c<maxcols; c++) {
			CurGen[IX(forcerow,c)].pos = x;
			CurGen[IX(forcerow,c)].vel = 0;
		}
	}
	if (forcecol>=0) {
		for (r=0; r<maxrows; r++) {
			CurGen[IX(r,forcecol)].pos = x;
			CurGen[IX(r,forcecol)].vel = 0;
		}
	}
	if (forceptr>=0) {
		CurGen[IX(forceptr,forceptc)].pos = x;
		CurGen[IX(forceptr,forceptc)].vel = 0;
	}
}

ForceSine()
{
	int t;
	int x;

	t = (omega*fftime++)%sinTabSize;
	x = (int)(ampl*SCALE*sinTab[t]);
	ForceX(x);
}

ForceDelta()
{
	int t;
	int x;

	t = (omega*fftime++)%sinTabSize;
	if (t==0)
		x = (int)(ampl*SCALE);
	else {
		if (!forcefull) return;
		x = 0;
	}
	ForceX(x);
}

ForcePulse()
{
	int t,t2;
	int x;

	t = (omega*fftime++)%sinTabSize;
	t2 = omega2*t;
	if (t2<=sinTabSize/2)
		x = ampl*SCALE;
	else {
		if (!forcefull) return;
		x = 0;
	}
	ForceX(x);
}

ForceOneHalfSine()
{
	int t,t2;
	int x;
	int r,c;

	t = (omega*fftime++)%sinTabSize;
	t2 = omega2*t;
	if (t2<sinTabSize/2)
		x = (int)(ampl*SCALE*sinTab[t2]);
	else {
		if (!forcefull) return;
		x = 0;
	}
	ForceX(x);
}

ForceOneHalfSineSquared()
{
	int t,t2;
	int x;
	int r,c;

	t = (omega*fftime++)%sinTabSize;
	t2 = omega2*t;
	if (t2<sinTabSize/2)
		x = (int)(ampl*SCALE*sinTab[t2]*sinTab[t2]);
	else {
		if (!forcefull) return;
		x = 0;
	}
	ForceX(x);
}

Ftab ForceTab[] = {
	{ "multisine", ForceFuncMultiSine },
	{ "delta", ForceDelta },
	{ "pulse", ForcePulse },
	{ "sine", ForceSine },
	{ "onehalfsine", ForceOneHalfSine },
	{ "onehalfsinesquared", ForceOneHalfSineSquared },
};
int ForceTabSize = sizeof(ForceTab)/sizeof(ForceTab[0]);

InitForceFunc()
{
	ForceIndex = 0;
	ForceFunc = ForceTab[ForceIndex].func;
}

PrintForceFuncs(fp)
FILE *fp;
{
	int i;

	fprintf(fp,"Forcing functions are:\n");
	for (i=0; i<ForceTabSize; i++) {
		fprintf(fp," %s",ForceTab[i].name);
	}
	fprintf(fp,"\n");
}

SetNextForceFunc()
{
	ForceIndex++;
	if (ForceIndex>=ForceTabSize) ForceIndex = 0;
	ForceFunc = ForceTab[ForceIndex].func;
	printf("Forcing function is set to %s\n",
		ForceTab[ForceIndex].name);
}

SetNamedForceFunc(s)
char *s;
{
	int i;

	for (i=0; i<ForceTabSize; i++) {
		if (strcmp(ForceTab[i].name,s)==0) {
			ForceIndex = i;
			ForceFunc = ForceTab[ForceIndex].func;
			return 1;
		}
	}
	return 0;	/* not found */
}

/* end */

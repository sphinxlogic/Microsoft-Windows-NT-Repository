/******************************************************************************
** xwave - A 3-d perspective mesh for X
**
** Original Authors: (Jan 1991)
** Mike Friedman (mikef@umbc3.umbc.edu)
** Paul Riddle (paulr@umbc3.umbc.edu)
**
** Additional Help:
** Joe  Poole  (joe@umbc3.umbc.edu)
**
** University of Maryland, Baltimore County
*
* Enhanced by Jim McBeath (jimmc@hisoft.uucp) Feb 1991
******************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <math.h>
#include "xwave.h"
#include "patchlevel.h"
#include <sys/types.h>
#include <sys/time.h>


#define   DFLTMAXROWS    18
#define   DFLTMAXCOLS    25
#define   GENS       2
#define   SINTABSIZE    400
#define   TWOPI      6.283

#define   NFRAMES    50

#define   ROOT       0x1

#define SPRINGCONST 0.01
#define DAMPFACTOR 0.001

extern double atof();

extern int (*ForceFunc)();
extern int (*WaveFunc)();

extern Display *dpy;
extern int ampl;

extern int numEditedPoints;

int maxrows = DFLTMAXROWS;
int maxcols = DFLTMAXCOLS;
int CurIndex = 0;
int NextIndex = 1;
struct pinfo *Generation[GENS];
struct pinfo NullPinfo = {0};
struct pinfo *CurGen;
struct pinfo *NextGen;

int *amplitudes;
int *frequencies;
int *editedRowVals;
int *editedColVals;

double sinTab[SINTABSIZE];
int sinTabSize = SINTABSIZE;

int running = 1;
int fftime;
int omega=5;
int omega2=10;
int forcerow = -1;
int forcecol = -1;
int forceptr = -1;
int forceptc = -1;
int forcefull = 0;

double springconst = SPRINGCONST;
double dampfactor = DAMPFACTOR;

int flagtime=0;
int flagstereo=0;
int flagroot=0;
int flagplotvel=0;
int flagedit=0;
int didedit=0;

int nf;
int start;


char *
smalloc(s)
int s;
{
	extern char *malloc();
	char *p;

	p = malloc(s);
	if (p) return p;
	fprintf(stderr,"No more memory\n");
	exit(2);
}

char *
scalloc(n,s)
int n,s;
{
	extern char *calloc();
	char *p;

	p = calloc(n,s);
	if (p) return p;
	fprintf(stderr,"No more memory\n");
	exit(2);
}

char *demoorig[] = { "original" };	/* the original - no args */
char *demodeltapoint[] = { "deltapoint", "-force", "delta", "-wave", "wave",
	"-omega", "2" };
char *demodeltarow[] = { "deltarow", "-force", "delta", "-wave", "wave",
	"-forcerow", "8", "-amp", "30", "-omega", "2", "-bound", "0" };
char *demosineh[] = { "sineh", "-force", "onehalfsinesquared", "-wave", "wave",
	"-rows", "2", "-cols", "50", "-forcecol", "0", "-bound", "00111000",
	"-omega", "1", "-omega2", "4" };
char *demosine[] = { "sine", "-force", "sine", "-wave", "wave",
	"-rows", "2", "-cols", "48", "-forcecol", "0", "-bound", "00111000",
	"-omega", "5", "-plotvel", "-springconst", "0.05" };
char *demopulse[] = { "pulse", "-force", "pulse", "-forcefull", "-wave", "wave",
	"-omega", "1", "-omega2", "4", "-rows", "2", "-cols", "46",
	"-forcecol", "0", "-bound", "00111000", "-springconst", "0.05",
	"-plotvel" };
char *demopulse2[] = { "pulse", "-force", "pulse", "-wave", "wave",
	"-omega", "2", "-omega2", "4" };
/* remember to add new demo to demoinfo table below */

struct {
	char **p;
	int n;
} demoinfo [] = {
	{ demoorig, sizeof(demoorig)/sizeof(demoorig[0]) },
	{ demodeltapoint, sizeof(demodeltapoint)/sizeof(demodeltapoint[0]) },
	{ demodeltarow, sizeof(demodeltarow)/sizeof(demodeltarow[0]) },
	{ demosineh, sizeof(demosineh)/sizeof(demosineh[0]) },
	{ demosine, sizeof(demosine)/sizeof(demosine[0]) },
	{ demopulse, sizeof(demopulse)/sizeof(demopulse[0]) },
	{ demopulse2, sizeof(demopulse2)/sizeof(demopulse2[0]) },
};
int demoinfocount = sizeof(demoinfo)/sizeof(demoinfo[0]);

int
findDemo(s)
char *s;
{
	int i;

	i = atoi(s);
	if (i>0 || strcmp(s,"0")==0) {
		if (i>=demoinfocount) {
			(void)fprintf(stderr,"Max demo number is %d\n",
				demoinfocount-1);
			exit(1);
		}
		return i;	/* he specified a number (index) */
	}
	for (i=0; i<demoinfocount; i++) {
		if (strcmp(demoinfo[i].p[0],s)==0)
			return i;
	}
	(void)fprintf(stderr,"No such demo switch %s\n",s);
	exit(1);
}

DemoSwitch(s)
char *s;
{
	int n;

	n = findDemo(s);
	ProcessArgs(demoinfo[n].n,demoinfo[n].p);
}

PrintDemoSwitch(s)
char *s;
{
	int i,n;

	n = findDemo(s);
	for (i=0; i<demoinfo[n].n; i++) {
		printf("%s ",demoinfo[n].p[i]);
	}
	printf("\n");
	exit(0);
}

ListDemoSwitches(fp)
FILE *fp;
{
	int i;

	fprintf(fp,"Demo switches are:\n");
	for (i=0; i<demoinfocount; i++) {
		fprintf(fp," %s", demoinfo[i].p[0]);
	}
	fprintf(fp,"\n");
}

ProcessArgs(argc,argv)
int argc;
char *argv[];
{
	int i;

	for (i=1; i<argc; i++) {
                if (strcmp (argv[i], "-edit")==0) {
                        flagedit = 1;
                        didedit = 1;
		}
		else if (strcmp (argv[i], "-root")==0)
			flagroot = 1;
		else if (strcmp (argv[i], "-stereo")==0)
			flagstereo = 1;
		else if (strcmp(argv[i],"-springconst")==0) {
			springconst = atof(argv[++i]);
		}
		else if (strcmp(argv[i],"-dampfactor")==0) {
			dampfactor = atof(argv[++i]);
		}
		else if (strcmp(argv[i],"-rows")==0) {
			maxrows = atoi(argv[++i]);
			if (maxrows<2) maxrows = 2;
                        else if (maxrows>25) maxrows = 25;
		}
		else if (strcmp(argv[i],"-cols")==0) {
			maxcols = atoi(argv[++i]);
			if (maxcols<2) maxcols = 2;
		}
		else if (strcmp(argv[i],"-omega")==0) {
			omega = atoi(argv[++i]);
			if (omega<=0) omega=1;	/* can't divide by 0 */
		}
		else if (strcmp(argv[i],"-omega2")==0) {
			omega2 = atoi(argv[++i]);
			if (omega2<=0) omega2=1;	/* can't divide by 0 */
		}
		else if (strcmp(argv[i],"-amp")==0) {	/* forcing amplitude */
			ampl = atoi(argv[++i]);
			if (ampl<=0) ampl = 100;
		}
		else if (strcmp(argv[i],"-force")==0) {
			if (!SetNamedForceFunc(argv[++i])) {
				usage();
				exit(1);
			}
		}
		else if (strcmp(argv[i],"-forcerow")==0) {
			forcerow = atoi(argv[++i]);
		}
		else if (strcmp(argv[i],"-forcecol")==0) {
			forcecol = atoi(argv[++i]);
		}
		else if (strcmp(argv[i],"-forcepoint")==0) {
			forceptr = atoi(argv[++i]);
			forceptc = atoi(argv[++i]);
		}
		else if (strcmp(argv[i],"-forcefull")==0) {
			forcefull = 1;
		}
		else if (strcmp(argv[i],"-bound")==0) {
			SetBound(argv[++i]);
		}
		else if (strcmp(argv[i],"-wave")==0) {
			if (!SetNamedWaveFunc(argv[++i])) {
				usage();
				exit(1);
			}
		}
		else if (strcmp(argv[i],"-demo")==0) {
			DemoSwitch(argv[++i]);
		}
		else if (strcmp(argv[i],"-printdemo")==0) {
			PrintDemoSwitch(argv[++i]);
		}
		else if (strcmp(argv[i],"-plotvel")==0) {
			flagplotvel = 1;
		}
		else if (strcmp(argv[i],"-version")==0) {
			(void)printf("xwave version %d.%d, %s\n",
				VERSION, PATCHLEVEL, VDATE);
			exit(0);
		}
		else {
			usage();
			exit (1);
		}
	}
}

CheckArgs()
{
	if (forcerow >= maxrows)
		forcerow = maxrows/2;
	if (forcecol >= maxcols)
		forcecol = maxcols/2;
	if (forceptr >= maxrows)
		forceptr = maxrows/2;
	if (forceptc >= maxcols)
		forceptc = maxcols/2;
	if (forcerow<0 && forcecol<0) {
		if (forceptr<0)
			forceptr = maxrows/2;
		if (forceptc<0)
			forceptc = maxcols/2;
	} else {
		forceptr = forceptc = -1;
	}
}

usage()
{
	(void)fprintf(stderr, "Usage: xwave [-edit] [-root] [-stereo] [-springconst K]\n");
	(void)fprintf(stderr, "   [-dampfactor B] [-rows r] [-cols c]\n");
	(void)fprintf(stderr, "   [-force func] [-wave func] [-bound bits]\n");
	(void)fprintf(stderr, "   [-omega n] [-omega2 n] [-amp n]\n");
	(void)fprintf(stderr, "   [-forcerow r] [-forcecol c] [-forcepoint r c]\n");
	(void)fprintf(stderr, "   [-forcefull]\n");
	(void)fprintf(stderr, "   [-demo D] [-printdemo D] [-plotvel] [-version]\n");
	PrintForceFuncs(stderr);
	PrintWaveFuncs(stderr);
	ListDemoSwitches(stderr);
}

AllocData()
{
	Generation[0] = (struct pinfo *)smalloc(sizeof(struct pinfo)*
					maxrows*maxcols);
	Generation[1] = (struct pinfo *)smalloc(sizeof(struct pinfo)*
					maxrows*maxcols);
        amplitudes    = (int *)scalloc(maxrows*maxcols,sizeof(int));
        frequencies   = (int *)scalloc(maxrows*maxcols,sizeof(int));
        editedRowVals = (int *)scalloc(maxrows,sizeof(int));
        editedColVals = (int *)scalloc(maxcols,sizeof(int));
}

ClearData()
{
	int r, c, g;

	/* clear the forcing function timer */
	fftime = 0;

	/* Zero out the generation matrix */
	for (r = 0; r < maxrows; r++)
		for (c = 0; c < maxcols; c++)
			for (g = 0; g<GENS; g++)
				Generation[g][IX(r,c)] = NullPinfo;

	CurIndex = 0;
	NextIndex = 1;
	CurGen = Generation[CurIndex];
	NextGen = Generation[NextIndex];
}

/* This routine initializes the sine table.  The table is accessed during
 * computation to speed things up somewhat. */
void setupSinTable (sinTab)
double *sinTab;
{
	int i;
	double t = 0.0;

	for (i = 0; i < SINTABSIZE; i++, t += TWOPI / SINTABSIZE)
		sinTab[i] = sin (t);
}

DumpCur()
{
	int r,c;
	int v,p;

	for (r = 0; r < maxrows; r++) {
		for (c = 0; c < maxcols; c++) {
			v = CurGen[IX(r,c)].vel;
			p = CurGen[IX(r,c)].pos;
			if (v!=0 || p!=0)
				printf("[%d,%d]=(%d,%d)\n",r,c,v,p);
		}
	}
}

ProcessKey(s)
char *s;
{
	if (strcmp(s,"t")==0) {		/* time # frames */
                nf=0;
                flagtime=!flagtime;
	}
	else if (strcmp(s,"c")==0) {	/* clear */
		ClearData();
		ShowData();
	}
	else if (strcmp(s,"d")==0) {	/* (load) data */
		(*ForceFunc)();
		ShowData();
	}
	else if (strcmp(s,"f")==0) {	/* forcing function */
		SetNextForceFunc();
	}
	else if (strcmp(s,"p")==0) {	/* print data */
		DumpCur();
	}
	else if (strcmp(s," ")==0) {	/* step */
		Step();
		ShowData();
	}
	else if (strcmp(s,"r")==0)	/* run mode */
		running = !running;
	else if (strcmp(s,"q")==0)	/* q to exit */
		exit(0);
	else if (strcmp(s,"w")==0) {	/* wave propagation function */
		SetNextWaveFunc();
	}
	else XBell(dpy,25);	/* unknown */
}

ProcessEvent()
{
	/*
	 * Handle resizes.  When a resize happens, we need to re-allocate
	 * the background pixmap.  Note that the bigger the window, the
	 * larger the area that needs to get copied, and therefore the
	 * slower the program gets.
	 */
	XEvent event;
	int t;
	char cbuf[20];

	XNextEvent (dpy, &event);
	switch (event.type) {
	case ConfigureNotify:
		HandleResize(&event);
		break;
	case KeyPress:
                if (!flagedit) {
		    t = XLookupString(&event,cbuf,sizeof(cbuf),
			(KeySym *)NULL, (XComposeStatus *)NULL);
		    if (t<=0) break;
                    flagedit = 0;
                    cbuf[t] = '\0';
		    ProcessKey(cbuf);
		    break;
	        }
	default:
		break;
	}
}

Step()
{
	(*WaveFunc)();	/* calculate values at the next time point */
	CurIndex ^= 1;	/* swap next and current buffers */
	NextIndex ^= 1;
	CurGen = Generation[CurIndex];
	NextGen = Generation[NextIndex];

	(*ForceFunc)();	/* apply forcing function */

}

main (argc, argv)
int argc;
char *argv[];
{
	int t;

	InitWaveFunc();
	InitForceFunc();

	ProcessArgs(argc,argv);
	CheckArgs();	/* check for consistency etc. */
	AllocData();
	ClearData();
	InitWindow ();
	setupSinTable (sinTab);
	if (flagroot)
		WaitForExpose();
	while (1) {
		if (running) {
                        if (flagtime) {
                               if (nf==0)
                                     start = time((time_t)0);
                               if (nf==NFRAMES) {
                                     printf("#frames/sec.~%f\n",
                                            (float)nf/((time((time_t)0)-start)));
                                     nf = 0;
                               }
                               else
                                     nf++;
		        }
			if (QLength (dpy)) {
				ProcessEvent();
			}
			DoQueryPointer();
                        if (flagedit)
                                edit();
			Step();
			ShowData();
		}
		else {
		        ProcessEvent();
                        if (flagedit)
                                edit();
		}
	}
	/* NOTREACHED */
}

/* end */








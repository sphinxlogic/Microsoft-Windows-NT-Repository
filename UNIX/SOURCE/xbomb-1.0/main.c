
/*
 * main.c
 * @(#)main.c	1.32 (UCLA) 10/22/92
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include "xbomb.h"
#include "patchlevel.h"

int	dead = 0,	        /* true if you won or lost */
	numflags, 	/* number of bombs left to place */
	emptylen, 	/* number of places reserved for printing numflags */
	numempty, 	/* number of empty spaces */
	button, 	/* current button pressed */
	shifted,	/* was the shift key down? */
	numbombs = -1, 	/* number of bombs placed */
	width = -1,	/* width of puzzle */
	height = -1, 	/* height of puzzle */
	ted = -1,	/* ted mode = reduce nums when bomb detected */
	onlyprint = -1,	/* print scores, but don't update 'em */
	randomE = -1,	/* random number of empties */
	randomB = -1,	/* random number of bombs */
	seed = -1,	/* seed to pass to srand48 */
	showscore = -1,	/* should the score be shown? */
	betterfollow = -1,	/* use advanced following algorithm */
	startwithzero = -1,	/* start with a (almost) guaranteed zero */
	playagain = -1;         /* should we automatically restart */

#define MAX_SOLVERS 10
char *solvers[MAX_SOLVERS];
FILE *solver_rfps[MAX_SOLVERS], *solver_wfps[MAX_SOLVERS];
int solver_pids[MAX_SOLVERS];


#define is_bomb(s)  ((s == BOMB)?1:0)
#define is_bomb_or_flag(s) ((s==BOMB || s==FLAG)?1:0)
#define is_unknown(s) ((s==BOMB || s==EMPTY)?1:0)

struct cell map[MAXWIDTH+2][MAXHEIGHT+2];


void
cleanup()
{
	cleanup_pixmaps();
	revoke_solvers();
}


void
finish()
{
	cleanup();
	log_score();

	exit(0);
}


void
bail(s)
	char *s;
{
	perror (s);
	cleanup();
	exit(1);
}


#ifdef LACKS_STRDUP
char *
strdup(s)
	char *s;
{
	char *ss = NULL;
	if (s) {
		if (ss = malloc(strlen(s)+1)) {
			strcpy(ss, s);
		};
	};
	return ss;
}
#endif



static char *help_message[] = {
	"where options include:",
	"    -d host:display        Specify which X server to use",
	"    -g +x+y                Upper left-hand corner of puzzle",
	"    -fg color              Color for bombs and numbers",
	"    -bg color              Color for background",
	"    -bd color              Color for border",
	"    -bw width              Width of border (in pixels)",
	" ",
	"    -A                     AutoPlay - default is to play again",
	"    -bitmaps bitdir        Use bitmaps found in bitdir subdirectory",
	"    -h height              Bomb layout height (default 20)",
	"    -w width               Bomb layout width (default 20)",
	"    -n numberBombs         Number of bombs (default h*w/4)",
	"    -name player           Play with a different name",
	"    -f                     Make obvious moves (follow)",
	"    -q                     Only print scores when changed (quiet)",
	"    -r seed                Specify the seed (no highscore)",
	"    -Rb b                  randomly select b bombed squares at start",
	"    -Re e                  randomly select e empty squares at start",
	"    -s                     show scorefile",
	"    -S N Solver            Assign Solver #N",
	"    -t                     Deduct numbers for placed bombs (ted mode)",
	"    -z                     Start with a zero, instead of upper left",
	" ",
	"The authors recommend: -h 24 -w 34 -f -t -q -A",
	NULL
};

void
usage(name)
	char *name;
{
	char **cpp;

	fprintf(stderr, "usage: %s [-options ...]\n(Version %d.%d)\n", name,
		VERSION, PATCHLEVEL);
	for (cpp = help_message; *cpp; cpp++) {
		fprintf(stderr, "%s\n", *cpp);
	}
	fprintf(stderr, "\n");
	exit(1);
}


ParseAndStart(argc, argv)
	int argc;
	char *argv[];
{
	char *ProgName, *server, *borderColor;
	char *foreGround, *backGround;
	int i, temp;
	int num_solvers = 0;
	char *def;
	char solve_str[80];
	char *solve_num = solve_str + 6;

	strcpy (solve_str, "Solver0");

	server = "";
	borderColor = "";
	foreGround = "";
	backGround = "";

	ProgName = argv[0];

	/* parse command line options */

	for (i=1; i<argc; i++) {
		char *arg = argv[i];

		if (arg[0] == '-') {
			switch (arg[1]) {
				case 'A':
					playagain = 1;
					continue;
				case 'b':
					if (arg[2] == 'g') {
						if (++i >= argc) usage(ProgName);
						backGround = argv[i];
						continue;
						}
					else if (arg[2] == 'w') {
						if (++i >= argc) usage(ProgName);
						borderWidth = atoi(argv[i]);
						continue;
						}
					else if (arg[2] == 'd') {
						if (++i >= argc) usage(ProgName);
						borderColor = argv[i];
						continue;
						}
					else if (arg[2] == 'i') {
						if (++i >= argc) usage(ProgName);
						bitdir = argv[i];
						continue;
					        }
					else usage(ProgName);
					break;
				case 'd':
					if (++i >= argc) usage(ProgName);
					server = argv[i];
					continue;
				case 'f':
					if (arg[2] == 'g') {
						if (++i >= argc) usage(ProgName);
						foreGround = argv[i];
						continue;
						}
					else if (arg[2] == 0) {
						betterfollow = 1;
						continue;
						}
					else usage(ProgName);
					break;
				case 'g':
					if (++i >= argc) usage(ProgName);
					geom = argv[i];
					continue;
				case 'h':
					if (++i >= argc) usage(ProgName);
					height = atoi(argv[i]);
					continue;
				case 'n':
					if (arg[2] == 0) {
						if (++i >= argc) usage(ProgName);
						numbombs = atoi(argv[i]);
						continue;
						}
					else if (arg[2] == 'a') {
						if (++i >= argc) usage(ProgName);
						name = argv[i];
						continue;
						}
				case 'q':
					showscore = 0;
					continue;
				case 'r':
					if (++i >= argc) usage(ProgName);
					seed = atoi(argv[i]);
					continue;
				case 'R':
					if (arg[2] == 'e') {
						if (++i >= argc) usage(ProgName);
						randomE = atoi(argv[i]);
						continue;
						}
					else if (arg[2] == 'b') {
						if (++i >= argc) usage(ProgName);
						randomB = atoi(argv[i]);
						continue;
						}
					else usage(ProgName);
					break;
				case 's':
					/* so score won't be updated */
					onlyprint = 1;   
					log_score();
					exit(0);
				case 'S':
					if (++i >= argc) usage(ProgName);
					if (isdigit(arg[2])) {
						temp = arg[2] - '0';
						temp = (temp + 9) % 10;
						solvers[temp] = strdup(argv[i]);
						num_solvers = temp + 1;
						}
					else {
						solvers[num_solvers++] = strdup(argv[i]);
						}
					continue;
				case 't':
					ted = 1;
					continue;
				case 'w':
					if (++i >= argc) usage(ProgName);
					width = atoi(argv[i]);
					continue;
				case 'z':
					startwithzero = 1;
					onlyprint = 1;
					continue;
				default:
					usage(ProgName);
					continue;
				}
			}
		else
			usage(ProgName);
		}

	dpy = XOpenDisplay(server);
	if (dpy == NULL) {
		fprintf(stderr, "can't open display \"%s\"\n",server);
		exit(1);
	}
	screen = DefaultScreen(dpy);

	if (backGround[0] == '\0') {
		def = XGetDefault(dpy, "xbomb", "Background");
	    	backGround = def ? def: "white";
		}
	if (foreGround[0] == '\0') {
		def = XGetDefault(dpy, "xbomb", "Foreground");
	    	foreGround = def ? def: "black";
		}
	if (borderWidth < 0) {
		def = XGetDefault(dpy, "xbomb", "BorderWidth");
	    	borderWidth = def ? atoi(def) : 1;
		}
	if (borderColor[0] == '\0') {
		def = XGetDefault(dpy, "xbomb", "BorderColor");
	    	borderColor = def ? def: "black";
		}
	if (server[0] == '\0') {
		def = XGetDefault(dpy, "xbomb", "Display");
	    	server = def ? def: "black";
		}
	if (betterfollow < 0) {
		def = XGetDefault(dpy, "xbomb", "Follow");
	    	betterfollow = def ? 1 : 0;
		}
	if (bitdir == NULL) {
		if (def = XGetDefault(dpy, "xbomb", "Bitmaps"))
	    		bitdir = def;
	}
	if (geom[0] == '\0') {
		if (def = XGetDefault(dpy, "xbomb", "Geometry"))
	    		geom = def;
		}
	if (width < 0) {
		def = XGetDefault(dpy, "xbomb", "Width");
	    	width = def ? atoi(def) : 20;
		}
	if (height < 0) {
		def = XGetDefault(dpy, "xbomb", "Height");
	    	height = def ? atoi(def) : 20;
		}
	if (numbombs < 0) {
		def = XGetDefault(dpy, "xbomb", "NumberBombs");
	    	numbombs = def ? atoi(def) : 0;
		}
	if (name == NULL) {
		def = XGetDefault(dpy, "xbomb", "Name");
		if (def) name = def;
		}
	if (randomE < 0) {
		def = XGetDefault(dpy, "xbomb", "RandomEmpty");
	    	randomE = def ? atoi(def) : 0;
		}
	if (randomB < 0) {
		def = XGetDefault(dpy, "xbomb", "RandomBombs");
	    	randomB = def ? atoi(def) : 0;
		}
	if (playagain < 0) {
		def = XGetDefault(dpy, "xbomb", "PlayAgain");
	    	playagain = def ? 1 : 0;
		}
	if (showscore < 0) {
		def = XGetDefault(dpy, "xbomb", "Quiet");
	    	showscore = def ? 0 : 1;
		}
	for (i = 0; i <= 9; i++) {
		if (solvers[i] == NULL) {
			solve_num[0] = (i+1)%10 + '0';
			if (def = XGetDefault(dpy, "xbomb", solve_str))
	    			solvers[i] = def;
			}
		}
	if (ted < 0) {
		def = XGetDefault(dpy, "xbomb", "TedMode");
	    	ted = def ? 1 : 0;
		}
	if (startwithzero < 0) {
		def = XGetDefault(dpy, "xbomb", "StartWithZero");
	    	onlyprint = startwithzero = def ? 1 : 0;
		}

	if (startwithzero && showscore) 
		printf("Warning: starting with zero means no high score\n");

	FgPixel = requestColor(foreGround);
	BgPixel = requestColor(backGround);
	BdPixel = requestColor(borderColor);
}



main(argc,argv)
	int argc;
	char **argv;
{
	int i, ret, temp;

	ParseAndStart(argc, argv);

	graphics_main(argc, argv);
	
	if (seed==-1) {
		seed=time(NULL) + getpid();
	} else {
		onlyprint=1;
        }

	/* 
	 * Pass an int instead of 48 bit number to srand48 so we can easily use
	 * atoi to read in the number if given as an argument
	 */
        srand48(seed);

	do {

		create_map();

		for (temp = numflags, emptylen = 1; temp > 0; 
		     temp/=10,emptylen++);


		if (dead) {
			dead = 0;
			repaint();
			
		}

		for (i = 0; i < randomB + randomE; i++)
			place_random((i < randomB)?BOMB:EMPTY);

		ret = eventloop();

		log_score();
	} while (ret == 1);

	cleanup();
	exit(0) ;
}



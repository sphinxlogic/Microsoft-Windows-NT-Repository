
#ifndef lint
static char sccsid[] = "@(#)life1d.c	3.10 96/07/20 xlockmore";

#endif

/*-
 * life1d.c - Stephen Wolfram's game of Life for xlock, the X Window
 *            System lockscreen.
 *
 * Copyright (c) 1995 by David Bagley.
 *
 * See xlock.c for copying information.
 *
 * Revision History:
 * Changes of David Bagley <bagleyd@hertz.njit.edu>
 * 27-Jul-95: written, used life.c as a basis, using totalistic rules
 *            (default).  Special thanks to Harold V. McIntosh
 *            <mcintosh@servidor.unam.mx> for providing me with the
 *            LCAU collection and references.
 *
 * References:
 * Dewdney, A.K., "The Armchair Universe, Computer Recreations from the
 *   Pages of Scientific American Magazine", W.H. Freedman and Company,
 *   New York, 1988 (May 1985).
 * Perry, Kenneth E., "Abstract Mathematical Art", BYTE, December, 1986
 *   pp. 181-192
 * Hayes, Brian, "Computer Recreations", Scientific American, March 1984,
 *   p. 12
 * Wolfram, Stephen, "Cellular automata as models of complexity", Nature,
 *   4 October 1984, pp. 419-424
 * Wolfram, Stephen, "Computer Software in Science and Mathematics",
 *   Scientific American, September 1984, pp. 188-203
 *
 */

#include "xlock.h"

/* aliases for vars defined in the bitmap file */
#define CELL_WIDTH   image_width
#define CELL_HEIGHT    image_height
#define CELL_BITS    image_bits

#include "life1d.xbm"

#if defined( TOTALISTIC ) && defined( LCAU )
#undef TOTALISTIC
#undef LCAU
#endif
#if !defined( TOTALISTIC ) && !defined( LCAU )
/* Switch the following from 0 to 1 for alternate life or compile with -DLCAU */
#if 0
#define LCAU
#else
#define TOTALISTIC
#endif
#endif

#ifdef TOTALISTIC
#define STATES 5
#define RADIUS 4
#define MAXSUM_SIZE ((STATES-1)*(RADIUS*2+1)+1)
#else
#define STATES 4
#define RADIUS 1
#define MAXSUM_SIZE 64		/* power(STATES,RADIUS*2+1) */
#endif

ModeSpecOpt life1d_opts =
{0, NULL, NULL, NULL};

extern int  gridsize;

static XImage logo =
{
	0, 0,			/* width, height */
	0, XYBitmap, 0,		/* xoffset, format, data */
	LSBFirst, 8,		/* byte-order, bitmap-unit */
	LSBFirst, 8, 1		/* bitmap-bit-order, bitmap-pad, depth */
};

typedef struct {
	int         pixelmode;
	int         xs, ys;
	int         xb, yb;
	int         generation;
	int         nrows, ncols, border;
	int         width, height;
	int         k, r;
	long        code;
	unsigned char nextstate[MAXSUM_SIZE];
	int         colors[STATES - 1];

	unsigned char *newcells;
	unsigned char *oldcells;
	unsigned char *buffer;
} life1dstruct;

static life1dstruct *life1ds = NULL;
static int  icon_width, icon_height;

#ifdef TOTALISTIC
static int  totalistic_rules[][3] =
{

  /* Well behaved rules */
  /* Scientific American (Dewdney) */
	{2, 2, 20},
	{2, 2, 52},		/* A bit too prolific but I like it anyway */
	{2, 3, 88},		/* James K. Park's 1D Gun (1111111111011) */
	{2, 4, 368},

  /* Nature */
	{3, 1, 792},

  /* BYTE (Translated to Wolfram's notation) */
	{4, 1, 39744},
	{4, 1, 81036},
	{4, 1, 126092},
	{4, 1, 147824},
	{4, 1, 156272},
	{4, 1, 189468},
	{4, 1, 176412},
	{4, 1, 214840},
	{4, 1, 245028},
	{4, 1, 267320},
	{4, 1, 257808},
	{4, 1, 258596},
	{4, 1, 260224},
	{4, 1, 267408},
	{4, 1, 290960},
	{4, 1, 330056},
	{4, 1, 330436},
	{4, 1, 400192},
	{4, 1, 433296},
	{4, 1, 434492},
	{4, 1, 447368},
	{4, 1, 453768},
	{4, 1, 454416},
	{4, 1, 485488},
	{4, 1, 505904},
	{4, 1, 618960},
	{4, 1, 642948},
	{4, 1, 680528},
	{4, 1, 708484},
	{4, 1, 741004},
	{4, 1, 749708},
	{4, 1, 756420},
	{4, 1, 761356},
	{4, 1, 769088},
	{4, 1, 778568},
	{4, 1, 779792},
	{4, 1, 797456},
	{4, 1, 803728},
	{4, 1, 844092},
	{4, 1, 874524},
	{4, 1, 881440},
	{4, 1, 921476},
	{4, 1, 936832},
	{4, 1, 937792},
	{4, 1, 1004600},

  /* Nature */
	{5, 1, 580020},
	{5, 1, 5694390},
	{5, 1, 59123000},

#if 0				/* OK but annoying */

  /* BYTE */
	{4, 1, 10552},
	{4, 1, 14708},
	{4, 1, 25284},
	{4, 1, 42848},
	{4, 1, 44328},
	{4, 1, 51788},
	{4, 1, 107364},
	{4, 1, 111448},
	{4, 1, 155848},
	{4, 1, 173024},
	{4, 1, 224148},
	{4, 1, 238372},
	{4, 1, 241656},
	{4, 1, 243764},
	{4, 1, 255856},
	{4, 1, 259222},
	{4, 1, 310148},
	{4, 1, 324148},
	{4, 1, 346696},
	{4, 1, 364424},
	{4, 1, 403652},
	{4, 1, 436072},
	{4, 1, 456708},
	{4, 1, 461912},
	{4, 1, 534812},
	{4, 1, 546700},
	{4, 1, 552708},
	{4, 1, 569092},
	{4, 1, 616736},
	{4, 1, 658564},
	{4, 1, 717956},
	{4, 1, 748432},
	{4, 1, 800964},
	{4, 1, 800972},
	{4, 1, 801144},
	{4, 1, 821116},
	{4, 1, 840172},
	{4, 1, 858312},
	{4, 1, 865394},
	{4, 1, 914952},
	{4, 1, 919244},
	{4, 1, 984296},
	{4, 1, 997964},
	{4, 1, 1018488},
	{4, 1, 1018808},
	{4, 1, 1023864},
	{4, 1, 1024472},
	{4, 1, 1033776},
	{4, 1, 1033784},
	{4, 1, 1034552},

  /* Nature */
	{5, 1, 583330},
	{5, 1, 672900},
#endif

#if 0				/* rejects */
  /* Nature */
	{2, 1, 4},
	{2, 3, 18},
	{2, 3, 22},
	{2, 3, 90},
	{2, 3, 94},
	{2, 3, 126},
	{2, 3, 128},

  /* Scientific American (Dewdney) */
	{2, 3, 54},
	{3, 2, 66},		/* RIPPLE, Dewdney's personal 1d rule */
	{3, 1, 257},

  /* BYTE */
	{4, 1, 16},
	{4, 1, 56},
	{4, 1, 4408},
	{4, 1, 101988},
	{4, 1, 113688},
	{4, 1, 227892},
	{4, 1, 254636},
	{4, 1, 258598},
	{4, 1, 294146},
	{4, 1, 377576},
	{4, 1, 472095},
	{4, 1, 538992},
	{4, 1, 615028},
	{4, 1, 901544},
	{4, 1, 911876},

  /* Nature */
	{5, 1, 10175},
	{5, 1, 566780},
	{5, 1, 570090},
#endif
};

#define NRULES (sizeof totalistic_rules / sizeof totalistic_rules[0])

#else /* !TOTALISTIC */
#if 0
unsigned char lcau21_rules[][8] =
{
	"00010010",		/* 18 Hollow enlarging triangles */
	"00110110",		/* 54 Hollow triangles */
	"01101110",		/* 110 Hollow right triangles */
	"01111100",		/* 124 Hollow left triangles */
	"10010011",		/* 147 Solid triangles */
	"10001001",		/* 137 Solid right triangles */
	"11000001",		/* 193 Solid left triangles */
};

#define N2RULES (sizeof lcau21_rules / sizeof lcau21_rules[0])
#endif

unsigned char lcau31_rules[][27] =
{
	"000222111000111222222111000",	/* equal thirds */
	"002220210110110211110002200",	/* threads on triangles */
	"001121102222110110111002100",	/* interfaces of 2 vel */
	"020201010201010102010102020",	/* class iv, sparse */
	"020201011201011112011112120",	/* blue bground class iv */
	"021211110211110101110101020",	/* diagonal black gaps */
	"022221211221211012222111120",	/* macrocell w/ 012 membrane */
	"100002021002021210021210100",	/* totalistic rule 792 (iv) */
	"100002200112201200020121200",	/* binary counter */
	"101021220111100222112102020",	/* two patterns compete #1 */
	"110001000112221222112221000",	/* reversible rule */
	"111012101002110122121102120",	/* small blue triangles */
	"111021210012110202120212122",	/* 2 glider on 1 background */
	"111111101010202020202010101",	/* motes and triangles */
	"111220012222012120001221020",	/* black triangles */
	"112110201012001210101200010",	/* dendrites */
	"112122000000122112112122000",	/* reversible rule's reverse */
	"200211020111110002001101210",	/* crocodile skin */
	"202000200112001200120211100",	/* two slow gliders collide */
	"202211000201021001100200200",	/* Red Queen's binary counter */
	"210101012101012120012120200",	/* blue on red background */
	"212021022100200221201101020",	/* two patterns compete #2 */
	"220222010220211111000211010",	/* Fisch's cyclic eater */
	"222022001222211202022120012",	/* macrocell w/ 0122 membrane */
};

#define N3RULES (sizeof lcau31_rules / sizeof lcau31_rules[0])

unsigned char lcau41_rules[][64] =
{
	"0000000313131323232312121213232323231313131101010202020202010101",	/* skewed triangle */
	"0000020202000000020232220203300000000101020030000100322121003020",	/* slow glider - copies bar */
	"0000213323132331123303003213323113233123002033211332313233120001",	/* slightly chaotic symmetri */
	"0000332030033323010020122303001002120210000000100110020220000010",	/* shuttle squeeze */
	"0100030000030323200323120202001003000203000220001310220100200010",	/* coo gldrs */
	"0121200113213320110223311213201013131010111011101120012132103210",	/* mixture of types */
	"0212301103023320313223121332310023203323333231301020023120303020",	/* slo gl w/ many f gl */
	"0320032100113332112321213211003321233210121032320000321000010200",	/* crosshatching */
	"0323323323313310323323313310310223313310310210213310310210210210",	/* Perry's 245028 */
	"0323330023210000121122232322122113310131032101002110122321102120",	/*  */
	"0332200131100112230221012111210202022013210332120210222311212100",	/* cycles on dgl bgrnd */
	"0332200131100112230221012111210202022013210332120210222311212300",	/* cycles on dgl bgrnd */
	"1230320301231030321132211021112010202330101010112312220310311000",	/* very complex glider */
	"1230320301321030321132211021112010212121232212112312220310311000",	/* nice cross hatching */
	"2031122112031101123123233000321210301112010303203232213000311000",	/* gliders among stills */
	"2202003300010200010011010011020003033000032302002203110033010200",	/* bin ctr */
	"2313032202112320111221023212120203132000233322021310311101010030",	/* diagonal growth on mesh */
	"2332102112210200233210120311023110322213112123233132331213211212",	/* gliderettes & latice */
	"3000213323132331123302003213323113233123001033211332313233120000",	/* symmetric rule */
	"3003003203213211003203213211211303213211211311323211211311321320",	/* Perry's blue background */
	"3010213223113300321221011010123112301033010221203333211323110100",	/* v. bars w/entanglement */
	"3111213323132331123313113213323113233123220133211332313233121110",	/* not purely symmetric */
	"3112001202313030102330122003321023102122030102001210223122203020",	/* slow & fast gliders */
	"3130123232333201012202313210121032302200211020313130002001103210",	/* crocodile skin */
	"3230120202031130033311232111223012311113322032103210123012303210",	/* y/o on b/g */
	"3330333312110010333032222222001033303222121111110000322212110010",	/* Fisch's rule */
	"3332032303133100221122122213220022000212021022100200221203103020",	/* slow glider */
	"3333101022220101323201012323101033331010222201013232010123231010",	/* reverse of rvble #22 */
	"3333111122000022333311112200002233111133002222003311113300222200",	/* reversible Rule 22 */
};

#define N4RULES (sizeof lcau41_rules / sizeof lcau41_rules[0])

#ifdef N2RULES
#define NRULES (N2RULES + N3RULES + N4RULES)
#else
#define NRULES (N3RULES + N4RULES)
#endif
#endif /* !TOTALISTIC */

static void
drawcell(ModeInfo * mi, int col, int row, unsigned int state)
{
	Display    *display = MI_DISPLAY(mi);
	Window      window = MI_WINDOW(mi);
	GC          gc = MI_GC(mi);
	life1dstruct *lp = &life1ds[MI_SCREEN(mi)];

	if (!state) {
		XSetForeground(display, gc, MI_WIN_BLACK_PIXEL(mi));
		XFillRectangle(display, window, gc,
		lp->xb + lp->xs * col, lp->yb + lp->ys * row, lp->xs, lp->ys);
		return;
	}
	if (MI_NPIXELS(mi) > 2)
		XSetForeground(display, gc, MI_PIXEL(mi, lp->colors[state - 1]));
	else
		XSetForeground(display, gc, MI_WIN_WHITE_PIXEL(mi));
	if (lp->pixelmode)
		XFillRectangle(display, window, gc,
		lp->xb + lp->xs * col, lp->yb + lp->ys * row, lp->xs, lp->ys);
	else
		XPutImage(display, window, gc, &logo,
			  0, 0, lp->xb + lp->xs * col, lp->yb + lp->ys * row,
			  icon_width, icon_height);
}

static void
RandomSoup(life1dstruct * lp, int n, int v)
{
	int         col;

	v /= 2;
	if (v < 1)
		v = 1;
	for (col = lp->ncols / 2 - v; col < lp->ncols / 2 + v; ++col)
		if (NRAND(100) < n && col >= 0 && col < lp->ncols)
			lp->newcells[col + lp->border] = (unsigned char) NRAND(lp->k - 1) + 1;
}

static long
power(int x, int n)
{				/* raise x to the nth power n >= 0 */
	int         i;
	long        p = 1;

	for (i = 1; i <= n; ++i)
		p = p * x;
	return p;
}

static void
GetRule(life1dstruct * lp, int i)
{
	long        maxsum_size, j;

#ifdef TOTALISTIC
	long        code, pow;

	lp->k = totalistic_rules[i][0];
	lp->r = totalistic_rules[i][1];
	maxsum_size = (lp->k - 1) * (lp->r * 2 + 1) + 1;
	code = lp->code = totalistic_rules[i][2];

	pow = power(lp->k, maxsum_size - 1);	/* Should be less than max long */
	for (j = 0; j < maxsum_size; j++) {
		lp->nextstate[maxsum_size - 1 - j] = (unsigned char) (code / pow);
		code -= (long) lp->nextstate[maxsum_size - 1 - j] * pow;
		pow /= (long) lp->k;
	}
#else
	lp->r = 1;
#ifdef N2RULES
	if (i < N2RULES) {
		lp->k = 2;
		maxsum_size = power(lp->k, 2 * lp->r + 1);
		for (j = 0; j < maxsum_size; j++)
			lp->nextstate[maxsum_size - 1 - j] = lcau21_rules[i][j] - '0';
	} else if (i < N2RULES + N3RULES) {
#else
	if (i < N3RULES) {
#endif
		lp->k = 3;
		maxsum_size = power(lp->k, 2 * lp->r + 1);
#ifdef N2RULES
		i -= N2RULES;
#endif
		for (j = 0; j < maxsum_size; j++)
			lp->nextstate[maxsum_size - 1 - j] = lcau31_rules[i][j] - '0';
	} else {
		lp->k = 4;
		maxsum_size = power(lp->k, 2 * lp->r + 1);
#ifdef N2RULES
		i -= (N2RULES + N3RULES);
#else
		i -= N3RULES;
#endif
		for (j = 0; j < maxsum_size; j++)
			lp->nextstate[maxsum_size - 1 - j] = lcau41_rules[i][j] - '0';
	}
#endif
}

/* The following sometimes does not detect when there are multiple periodic
   life forms side by side. */
static int
compare(ModeInfo * mi)
{
	life1dstruct *lp = &life1ds[MI_SCREEN(mi)];
	int         row, col, tryagain = False;
	unsigned char *initl, *cmpr;

	initl = lp->buffer + (lp->nrows - 1) * lp->ncols;
	for (row = lp->nrows - 2; row >= 0; row--) {
		cmpr = lp->buffer + row * lp->ncols;
		for (col = 0; col < lp->ncols; col++) {
			tryagain = False;
			if (MI_NPIXELS(mi) > 2) {
				if (*(initl + col) != *cmpr) {
					tryagain = True;
					break;
				}
			} else {
				if ((*(initl + col) == 0) != (*cmpr == 0)) {
					tryagain = True;
					break;
				}
			}
			cmpr++;
		}
		if (!tryagain)
			return True;
	}
	return False;
}

void
init_life1d(ModeInfo * mi)
{
	life1dstruct *lp;
	int         i;

	if (life1ds == NULL) {
		if ((life1ds = (life1dstruct *) calloc(MI_NUM_SCREENS(mi),
					     sizeof (life1dstruct))) == NULL)
			return;
	}
	lp = &life1ds[MI_SCREEN(mi)];

	lp->generation = 0;

  if (!logo.data) {
 	  icon_width = CELL_WIDTH;
	  icon_height = CELL_HEIGHT;
	  logo.data = (char *) CELL_BITS;
	  logo.width = icon_width;
	  logo.height = icon_height;
	  logo.bytes_per_line = (logo.width + 7) / 8;
  }
	if (lp->newcells != NULL)
		(void) free((void *) lp->newcells);
	if (lp->oldcells != NULL)
		(void) free((void *) lp->oldcells);
	if (lp->buffer != NULL)
		(void) free((void *) lp->buffer);
	lp->width = MI_WIN_WIDTH(mi);
	lp->height = MI_WIN_HEIGHT(mi);
	if (lp->width < 2)
		lp->width = 2;
	if (lp->height < 2)
		lp->height = 2;
	lp->pixelmode = (lp->width + lp->height < 8 * (icon_width + icon_height));
	if (lp->pixelmode) {
		lp->ncols = min(lp->width / 2, gridsize);
		lp->nrows = min(lp->height / 2, gridsize);
	} else {
		lp->ncols = min(lp->width / icon_width, gridsize);
		lp->nrows = min(lp->height / icon_height, gridsize);
	}
	lp->border = (lp->nrows / 2 + 1) * MI_CYCLES(mi);
	lp->newcells = (unsigned char *) calloc(lp->ncols + 2 * lp->border,
						sizeof (unsigned char));
	lp->oldcells = (unsigned char *) calloc(lp->ncols + 2 * (RADIUS * lp->border),
						sizeof (unsigned char));

	lp->buffer = (unsigned char *) calloc(lp->ncols * lp->nrows,
					      sizeof (unsigned char));
	lp->xs = lp->width / lp->ncols;
	lp->ys = lp->height / lp->nrows;
	lp->xb = (lp->width - lp->xs * lp->ncols) / 2;
	lp->yb = (lp->height - lp->ys * lp->nrows) / 2;

	GetRule(lp, (int) NRAND(NRULES));
	if (MI_WIN_IS_VERBOSE(mi)) {
		(void) printf("colors %d, radius %d, code %ld, rule ",
			      lp->k, lp->r, lp->code);
		for (i = (lp->k - 1) * (lp->r * 2 + 1); i >= 0; i--)
			(void) printf("%d", (int) lp->nextstate[i]);
		(void) printf("\n");
	}
	if (MI_NPIXELS(mi) > 2)
		for (i = 0; i < lp->k - 1; i++)
			lp->colors[i] = (NRAND(MI_NPIXELS(mi)) + i * MI_NPIXELS(mi)) /
				(lp->k - 1);
	RandomSoup(lp, 40, 25);
	(void) memcpy((char *) (lp->oldcells + RADIUS + lp->border),
		      (char *) (lp->newcells + lp->border), lp->ncols);
	XClearWindow(MI_DISPLAY(mi), MI_WINDOW(mi));
}

void
draw_life1d(ModeInfo * mi)
{
	life1dstruct *lp = &life1ds[MI_SCREEN(mi)];
	int         row, col;

	if (lp->generation > MI_CYCLES(mi) || (lp->generation && compare(mi)))
		init_life1d(mi);

	for (col = 0; col < lp->ncols; col++)
		if (lp->buffer[col] != lp->newcells[col + lp->border])
			drawcell(mi, col, 0, lp->newcells[col + lp->border]);
	(void) memcpy((char *) lp->buffer, (char *) (lp->newcells + lp->border),
		      lp->ncols);

	for (row = 1; row < lp->nrows; row++) {
		for (col = 0; col < lp->ncols + 2 * lp->border; col++) {
#ifdef TOTALISTIC
			int         sum = 0, m;

			for (m = col - lp->r; m <= col + lp->r; m++)
				sum += lp->oldcells[m + RADIUS];
#else
			int         sum = 0, pow = 1, m;

			for (m = col + lp->r; m >= col - lp->r; m--) {
				sum += lp->oldcells[m + RADIUS] * pow;
				pow *= lp->k;
			}
#endif
			lp->newcells[col] = lp->nextstate[sum];
		}
		(void) memcpy((char *) (lp->oldcells + RADIUS),
			  (char *) lp->newcells, lp->ncols + 2 * lp->border);

		for (col = 0; col < lp->ncols; col++)
			if (lp->buffer[col + row * lp->ncols] != lp->newcells[col + lp->border])
				drawcell(mi, col, row, lp->newcells[col + lp->border]);
		(void) memcpy((char *) (lp->buffer + row * lp->ncols),
			    (char *) (lp->newcells + lp->border), lp->ncols);
	}
	lp->generation++;
}

void
release_life1d(ModeInfo * mi)
{
	if (life1ds != NULL) {
		int         screen;

		for (screen = 0; screen < MI_NUM_SCREENS(mi); screen++) {
			life1dstruct *lp = &life1ds[screen];

			if (lp->newcells != NULL)
				(void) free((void *) lp->newcells);
			if (lp->oldcells != NULL)
				(void) free((void *) lp->oldcells);
			if (lp->buffer != NULL)
				(void) free((void *) lp->buffer);
		}
		(void) free((void *) life1ds);
		life1ds = NULL;
	}
}

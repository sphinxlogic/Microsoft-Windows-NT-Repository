static char *RcsID = "$Id: getres.c,v 1.2 1993/02/13 14:34:24 rfs Exp $";
/*
 * $Log: getres.c,v $
 * Revision 1.2  1993/02/13  14:34:24  rfs
 * *** empty log message ***
 *
 * Revision 1.1  1993/02/13  02:06:39  rfs
 * Initial revision
 *
 * Revision 1.1  1993/02/13  02:06:39  rfs
 * Initial revision
 *
*/

/*
 * Get resources that don't apply to widgets, per se.
 *
 * cursorFg: foreground color for "cursor"
 * cursorBg: background color for "cursor"
 *
 * Note: "cursor" is the data byte clicked on with the mouse
*/

#include <xod.h>

/* #defined here, since no one else need to know about them */
#define XtNcursorForeground "cursorFg"
#define XtCCursorForeground "CursorFg"
#define XtNcursorBackground "cursorBg"
#define XtCCursorBackground "CursorBg"

/* the 7 colors allowed by the CTW widget */
static char *CTWcolors[] = {
	"black",
	"red",
	"green",
	"yellow",
	"blue",
	"magenta",
	"cyan",
	"white"
};

static char *intoRev = NULL;
static char *selCurFg = NULL;
static char *selCurBg = NULL;

/* returns ESC seequence to set desired reverse video on data */
char *
RevVideo(void) {
	return intoRev;
}

typedef struct {
	String cursorFg;
	String cursorBg;
} InstanceRec;

static XtResource resources[] = {
	{ 
		XtNcursorForeground, XtCCursorForeground,
		XtRString, sizeof(char *),
		XtOffsetOf(InstanceRec, cursorFg),
		XtRString, "black"
	},
	{ 
		XtNcursorBackground, XtCCursorBackground,
		XtRString, sizeof(char *),
		XtOffsetOf(InstanceRec, cursorBg),
		XtRString, "yellow"
	}
};

static int
XlateColor(char *color) {
	int  i;

	for (i=0; i < (sizeof(CTWcolors)/sizeof(char *)); i++) {
		if (!strcmp(CTWcolors[i], color))
			return i;
	}
	return -1;
}

void
FigureRevVidSeq(void) {
	char buffer[MAXLEN];
	char *cfg, *cbg;
	int fg, bg;

	fg = XlateColor(selCurFg);
	bg = XlateColor(selCurBg);
	if (fg < 0)
		fprintf(stderr, "Foreground color %s is illegal\n", selCurFg);
	if (bg < 0)
		fprintf(stderr, "Background color %s is illegal\n", selCurBg);
	if (fg < 0 || bg < 0)
		exit(1);
	switch (fg) {
		case 0: cfg = FGBLACK; break;
		case 1: cfg = FGRED; break;
		case 2: cfg = FGGREEN; break;
		case 3: cfg = FGYELLOW; break;
		case 4: cfg = FGBLUE; break;
		case 5: cfg = FGMAGENTA; break;
		case 6: cfg = FGCYAN; break;
		case 7: cfg = FGWHITE; break;
	}
	switch (bg) {
		case 0: cbg = BGBLACK; break;
		case 1: cbg = BGRED; break;
		case 2: cbg = BGGREEN; break;
		case 3: cbg = BGYELLOW; break;
		case 4: cbg = BGBLUE; break;
		case 5: cbg = BGMAGENTA; break;
		case 6: cbg = BGCYAN; break;
		case 7: cbg = BGWHITE; break;
	}
	sprintf(buffer, "%s%s", cfg, cbg);
	intoRev = (char *)malloc(strlen(buffer)+1);
	strcpy(intoRev, buffer);
}

void
XodGetResources(Widget top) {
	InstanceRec ir;
	XtGetApplicationResources(top, &ir, resources, XtNumber(resources),
		NULL, 0);
	selCurFg = ir.cursorFg;
	selCurBg = ir.cursorBg;
}

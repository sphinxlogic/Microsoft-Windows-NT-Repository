/*****************************************************************************
 *
 * FACILITY:
 *   XPool	DECwindows Pool game
 *	
 * ABSTRACT:
 *   This module contains the routines which interact with the XToolkit.
 *
 * AUTHOR:
 *   Doug Stefanelli
 *
 * CREATION DATE: 1-October-1989
 *
 * Edit History 
 *
 *  DS	1-Oct-1989	Original version
 *  DS	4-Dec-1989	Modified to run from XtMainLoop()
 *  DS	13-Mar-1990	Support 2 displays.  Add Ultrix support.
 *
 */

/*
 * Routines included in this module
 */
void init_pool_graphics();	/* initial graphics window for a display */
void draw_table();		/* draw pool table on all displays */
static void draw_tbl();		/* draw pool table on a specific display */
static void draw_tick_mark();	/* draw table tick marks on a specific display */
void draw_ball();		/* draw a pool ball on all display */
static void draw_b();		/* draw a pool ball on a specific display */
void erase_ball();		/* erase a pool ball on all displays */
void number_ball();		/* number a pool ball on all displays */
static void number_b();		/* number a pool ball on a specific display */
void display_intro();		/* display introductory text */
void save_text();		/* save message text for next exposure event */
void draw_centered_text();	/* draw one line message on current display */
void draw_centered_text2();	/* draw two line message on current display */
void draw_remote_text();	/* draw one line message on remote display */
void draw_remote_text2();	/* draw two line message on remote display */
static void draw_rtext();	/* draw a right justified message on a specific display */
static void draw_ctext();	/* draw one line message on a specific display */
static void draw_ctext2();	/* draw two line message on a specific display */
void clear_message_area();	/* clear message area on current display */
void clear_remote_area();	/* clear message area on remote display */
static void clear_marea();	/* clear message area on a specific display */
static void draw_ballrack();	/* draw an empty ballrack on a specific display */
void add_to_ballrack();		/* add a sunken ball to the ballrack */
void redraw_ballrack();		/* redraw ballrack w/ sunken balls on all displays */
static void redraw_brack();	/* redraw ballrack w/ sunken balls on a specific displays */
void display_score();		/* display score on all displays */
static void display_scr();	/* display score on a specific display */
void draw_arrow();		/* draw confirmation arrow */
void erase_arrow();		/* erase confirmation arrow */
void redraw_display();		/* redraw display on a specific display (exposures) */
void get_window();		/* report window position on display */
int is_monochrome();		/* returns true if screen is monochrome */

/*
 * Include files
 */
#include <stdio.h>
#include "xpool.h"
#ifdef VMS
#include <decw$include:intrinsic.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#define FontName "-ADOBE-NEW CENTURY SCHOOLBOOK-MEDIUM-R-NORMAL--*-240-*-*-P-*"
#define SmallFontName "-ADOBE-NEW CENTURY SCHOOLBOOK-MEDIUM-R-NORMAL--*-120-*-*-P-*"
#define MediumFontName "-ADOBE-NEW CENTURY SCHOOLBOOK-MEDIUM-R-NORMAL--*-180-*-*-P-*"
#define WindowName "Pool"
#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))
#define displayY (20+bottom_edge/2)

/*
 * Local variables
 */
static Display *display[MAX_SCREENS];
static Window window[MAX_SCREENS];
static GC gc[MAX_SCREENS];
static Screen *screen[MAX_SCREENS];
static XFontStruct *font[MAX_SCREENS];
static XFontStruct *smallfont[MAX_SCREENS];
static XFontStruct *mediumfont[MAX_SCREENS];
static Cursor cursor[MAX_SCREENS];
static char save_string1[MAX_SCREENS][80];
static char save_string2[MAX_SCREENS][80];
static int num_strings[MAX_SCREENS];

static struct {
    XColor *background;
    XColor *foreground;
    XColor *felt;
    XColor *table;
    XColor *ball[max_balls];
} colors[MAX_SCREENS];

#define ball_size (diameter-1)
static int cx[] = {3,6,8,9,10,11,11,12,12,12,13,13,13,13,13,13,13,12,12,12,11,11,10,9,8,6,3};
static int cy[] = {-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13};
/*
static int cx[] = {3,6,8,9,9,10,10,10,11,11,11,11,11,11,11,10,10,10,9,9,8,6,3};
static int cy[] = {-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11};
*/
static XPoint ball_point[2*ball_size];

static unsigned char arrow_drawn = FALSE;

/*
 * External variables
 */
extern int num_balls;
extern float px[], py[];
extern float vx[], vy[];
extern int my_score, his_score;
extern int game;
extern int leftx, rightx;
extern int wins, losses;
extern int system;
extern int num_screens;
extern unsigned char my_turn;
extern unsigned char net_game;
extern unsigned char lock_game;
extern unsigned char im_remote;
extern unsigned char im_high;
extern unsigned char selection_made;
extern unsigned char on_table[max_balls];
extern unsigned char action_started;
extern unsigned char balls_numbered;
extern unsigned char intro_displayed;
extern char *player_name[2];

/*
 * External routines
 */
extern open_window();

void init_pool_graphics(s)
{
    int i;
    int colormap_size;
    Colormap colormap;
    Pixmap cursor_source;
    Pixmap cursor_mask;
    static unsigned short source_data[] = {0,0,0x3e0,0x7f0,0xff8,0x1e3c,0x1c1c,
			0x1c1c,0x1c1c,0x1e3c,0xff8,0x7f0,0x3e0,0,0,0};
    static unsigned short mask_data[] = {0,0,0x3e0,0x7f0,0xff8,0x1ffc,0x1ffc,
			0x1ffc,0x1ffc,0x1ffc,0xff8,0x7f0,0x3e0,0,0,0};

/************************ Alternate larger cursor ******************************
    static unsigned short source_data[] = {0x07c0,0x1ff0,0x3ff8,0x7ffc,0x7ffc,
			0xfc7e,0xf83e,0xf83e,0xf83e,0xfc7e,0x7ffc,0x7ffc,
			0x3ff8,0x1ff0,0x07c0,0};
    static unsigned short mask_data[] = {0x07c0,0x1ff0,0x3ff8,0x7ffc,0x7ffc,
			0xfffe,0xfffe,0xfffe,0xfffe,0xfffe,0x7ffc,0x7ffc,
			0x3ff8,0x1ff0,0x07c0,0};
*******************************************************************************/

    XGCValues xgcv;

    open_window(s, windowW, windowH, &display[s], &window[s]);

    screen[s] = XDefaultScreenOfDisplay(display[s]);

    /* Create colors */

    colormap = XDefaultColormapOfScreen(screen[s]);
    colormap_size = XCellsOfScreen(screen[s]);

    get_default_colors(s, is_monochrome(screen[s]), &colors[s].background,
			&colors[s].foreground, &colors[s].felt,
			&colors[s].table, colors[s].ball);

    XAllocColor(display[s], colormap, colors[s].background);
    XAllocColor(display[s], colormap, colors[s].foreground);
    XAllocColor(display[s], colormap, colors[s].felt);
    XAllocColor(display[s], colormap, colors[s].table);
    for (i=0; i<max_balls; i++)
	XAllocColor(display[s], colormap, colors[s].ball[i]);

    /* Create graphics context. */

    xgcv.foreground = colors[s].foreground->pixel;
    xgcv.background = colors[s].background->pixel;
    xgcv.fill_style = FillSolid;

    gc[s] = XCreateGC(display[s], window[s],
	GCForeground | GCBackground | GCFillStyle, &xgcv);

    font[s] = XLoadQueryFont(display[s], FontName);
    XSetFont(display[s], gc[s], font[s]->fid);
    smallfont[s] = XLoadQueryFont(display[s], SmallFontName);
    mediumfont[s] = XLoadQueryFont(display[s], MediumFontName);

    cursor_source = XCreatePixmapFromBitmapData(display[s], window[s], source_data,
			16, 16, 1, 0, 1);
    cursor_mask = XCreatePixmapFromBitmapData(display[s], window[s], mask_data,
			16, 16, 1, 0, 1);
    cursor[s] = XCreatePixmapCursor(display[s], cursor_source, cursor_mask,
		colors[s].foreground, colors[s].background, 8, 8);
    XDefineCursor(display[s], window[s], cursor[s]);

    ball_point[1].x = 2*cx[0];
    ball_point[1].y = 0;
    for (i=1; i<ball_size; i++) {
	ball_point[2*i].x = -cx[i] - cx[i-1];
	ball_point[2*i].y = 1;
	ball_point[2*i+1].x = 2*cx[i];
	ball_point[2*i+1].y = 0;
    }

    num_strings[s] = 0;
}

/* Draw table routine */

void draw_table()
{
    int s;

    for (s=0; s<num_screens; s++) {
	draw_tbl(s);
	num_strings[s] = 0;
    }
}

static void draw_tbl(s)
{
    static XRectangle rectangles[] = {
	left_edge-border, bottom_edge-border, right_edge-left_edge+border, border,
	left_edge-border, bottom_edge, border, top_edge-bottom_edge+border,
	right_edge, bottom_edge-border, border, top_edge-bottom_edge+border,
	left_edge, top_edge, right_edge-left_edge+border, border
    };
    static XArc arcs[] = {
	left_edge-pocket_size/2+3, bottom_edge-pocket_size/2+3, pocket_size, pocket_size, 0, 64*360,
	left_edge-pocket_size/2+3, top_edge-pocket_size/2-3, pocket_size, pocket_size, 0, 64*360,
	(right_edge+left_edge)/2-pocket_size/2, bottom_edge-pocket_size/2, pocket_size, pocket_size, 0, 64*360,
	(right_edge+left_edge)/2-pocket_size/2, top_edge-pocket_size/2, pocket_size, pocket_size, 0, 64*360,
	right_edge-pocket_size/2-3, bottom_edge-pocket_size/2+3, pocket_size, pocket_size, 0, 64*360,
	right_edge-pocket_size/2-3, top_edge-pocket_size/2-3, pocket_size, pocket_size, 0, 64*360
    };
    int i;

    /* Draw the table */

    XSetForeground(display[s], gc[s], colors[s].background->pixel);
    XFillRectangle(display[s], window[s], gc[s], 0, 0, windowW, windowH);

    XSetForeground(display[s], gc[s], colors[s].table->pixel);
    XFillRectangles(display[s], window[s], gc[s], rectangles, 4);

    XSetForeground(display[s], gc[s], colors[s].felt->pixel);
    XFillArcs(display[s], window[s], gc[s], arcs, 6);
    XFillRectangle(display[s], window[s], gc[s], left_edge, bottom_edge,
	right_edge-left_edge+1, top_edge-bottom_edge+1);

    draw_tick_mark(s, left_edge+break_offset, top_edge+border/2);
    draw_tick_mark(s, left_edge+break_offset, bottom_edge-border/2);
    draw_tick_mark(s, right_edge-break_offset, top_edge+border/2);
    draw_tick_mark(s, right_edge-break_offset, bottom_edge-border/2);
    draw_tick_mark(s, left_edge-border/2, (top_edge+bottom_edge)/2);
    draw_tick_mark(s, right_edge+border/2, (top_edge+bottom_edge)/2);

    for (i=0; i<num_balls; i++)
	if (on_table[i]) {
	    draw_b(s, i);
	    if (i>0 && balls_numbered) number_b(s, i);
	}

    draw_ballrack(s);
}

static void draw_tick_mark(s, x, y)
{
    static int tcx[] = {2,3,4,4,4,4,4,3,2};
    static int tcy[] = {-4,-3,-2,-1,0,1,2,3,4};
    int i;

    for (i=0; i<sizeof(tcx)/4; i++)
	XDrawLine(display[s], window[s], gc[s], x-tcx[i], y+tcy[i], x+tcx[i], y+tcy[i]); 
}

void draw_ball(index)
{
    int s;

    for (s=0; s<num_screens; s++)
	draw_b(s, index);
}

static void draw_b(s, index)
{
    ball_point[0].x = (int)px[index] - cx[0];
    ball_point[0].y = (int)py[index] + cy[0];

    XSetForeground(display[s], gc[s], colors[s].ball[index]->pixel);
    XDrawLines(display[s], window[s], gc[s], ball_point, 2*ball_size, CoordModePrevious);
}

void erase_ball(fx,fy)
float fx, fy;
{
    int s;

    ball_point[0].x = (int)fx - cx[0];
    ball_point[0].y = (int)fy + cy[0];
    for (s=0; s<num_screens; s++) {
	XSetForeground(display[s], gc[s], colors[s].felt->pixel);
	XDrawLines(display[s], window[s], gc[s], ball_point, 2*ball_size, CoordModePrevious);
    }
}

void number_ball(index)
{
    int s;

    for (s=0; s<num_screens; s++)
	number_b(s, index);
}

static void number_b(s, index)
{
    int x = (int)px[index];
    int y = (int)py[index];
    char number_string[3];
    int stringlen;
    int direction;
    int ascent, descent;
    XCharStruct overall;
    static int scx[] = {2,4,5,6,6,7,7,7,7,7,6,6,5,4,2};
    static int scy[] = {-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
/*
    static int scx[] = {3,4,5,6,6,6,6,6,6,6,5,4,3};
    static int scy[] = {-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6};
*/
    int i;

    sprintf(number_string, "%d", index);
    stringlen = strlen(number_string);

    XSetForeground(display[s], gc[s], colors[s].foreground->pixel);
    for (i=0; i<sizeof(scx)/4; i++)
	XDrawLine(display[s], window[s], gc[s], x-scx[i], y+scy[i],
		x+scx[i], y+scy[i]); 

    XSetForeground(display[s], gc[s], colors[s].background->pixel);
    XSetFont(display[s], gc[s], smallfont[s]->fid);
    XTextExtents(smallfont[s], number_string, stringlen, &direction, &ascent,
		&descent, &overall);
    XDrawString(display[s], window[s], gc[s],
		x - (overall.width-1)/2,
		y + (overall.ascent+overall.descent+1)/2,
		number_string, stringlen);
}

void display_intro()
{
    int i;
    int next_pos;
    int stringlen;
    int direction;
    int ascent, descent;
    XCharStruct overall;
    static char *string[] = {
	"Welcome to XPool",
	"An Interactive DECwindows",
	"Pool Game"
    };

    XSetForeground(display[LOCAL], gc[LOCAL], colors[LOCAL].foreground->pixel);
    XSetFont(display[LOCAL], gc[LOCAL], font[LOCAL]->fid);
    next_pos = bottom_edge + 150;
    for (i=0; i<sizeof(string)/4; i++) {
	stringlen = strlen(string[i]);
	XTextExtents(font[LOCAL], string[i], stringlen, &direction, &ascent,
		&descent, &overall);
	XDrawString(display[LOCAL], window[LOCAL], gc[LOCAL],
		(windowW - overall.width)/2,
		next_pos,
		string[i], stringlen);
	next_pos += (overall.ascent + overall.descent)*3;
    }
    draw_centered_text2("USE MENU BAR TO SET OPTIONS",
			"AND TO START THE GAME");
}

void save_text(s, string)
char *string;
{
    strcpy(save_string1[s], string);
    num_strings[s] = 1;
}

void draw_centered_text(string)
char *string;
{
    draw_ctext(system, string);
}

void draw_centered_text2(string1, string2)
char *string1, *string2;
{
    draw_ctext2(system, string1, string2);
}

void draw_remote_text(string)
char *string;
{
    draw_ctext(system == REMOTE ? LOCAL : REMOTE, string);
}

void draw_remote_text2(string1, string2)
char *string1, *string2;
{
    draw_ctext2(system == REMOTE ? LOCAL : REMOTE, string1, string2);
}

static void draw_ctext(s, string)
char *string;
{
    int stringlen;
    int direction;
    int ascent, descent;
    XCharStruct overall;

    clear_marea(s);

    strcpy(save_string1[s], string);
    num_strings[s] = 1;

    XSetForeground(display[s], gc[s], colors[s].foreground->pixel);
    XSetFont(display[s], gc[s], font[s]->fid);
    stringlen = strlen(string);
    XTextExtents(font[s], string, stringlen, &direction, &ascent,
		&descent, &overall);
    XDrawString(display[s], window[s], gc[s],
		(windowW - overall.width)/2,
		(windowH + top_edge + overall.ascent + overall.descent)/2,
		string, stringlen);
}

static void draw_rtext(s, y, string)
char *string;
{
    int direction;
    int ascent, descent;
    XCharStruct overall;

    XTextExtents(font[s], string, strlen(string), &direction, &ascent,
		&descent, &overall);
    XDrawString(display[s], window[s], gc[s], windowW-15-overall.width,
		y, string, strlen(string));
}

static void draw_ctext2(s, string1, string2)
char *string1;
char *string2;
{
    int stringlen;
    int direction;
    int ascent, descent;
    XCharStruct overall;

    clear_marea(s);

    strcpy(save_string1[s], string1);
    strcpy(save_string2[s], string2);
    num_strings[s] = 2;

    XSetForeground(display[s], gc[s], colors[s].foreground->pixel);
    XSetFont(display[s], gc[s], font[s]->fid);

    stringlen = strlen(string1);
    XTextExtents(font[s], string1, stringlen, &direction, &ascent,
		&descent, &overall);
    XDrawString(display[s], window[s], gc[s],
		(windowW - overall.width)/2,
		(windowH + top_edge)/2 - (overall.ascent + overall.descent)/2,
		string1, stringlen);

    stringlen = strlen(string2);
    XTextExtents(font[s], string2, stringlen, &direction, &ascent,
		&descent, &overall);
    XDrawString(display[s], window[s], gc[s],
		(windowW - overall.width)/2,
		(windowH + top_edge)/2 + (overall.ascent + overall.descent)*3/2,
		string2, stringlen);
}

void clear_message_area()
{
    clear_marea(system);
}

void clear_remote_area()
{
    clear_marea(system == REMOTE ? LOCAL : REMOTE);
}

static void clear_marea(s)
{
    XSetForeground(display[s], gc[s], colors[s].background->pixel);
    XFillRectangle(display[s], window[s], gc[s], 0, top_edge+border,
		windowW, windowH-top_edge-border-diameter-1);
    num_strings[s] = 0;
}

static void draw_ballrack(s)
{
    XSetForeground(display[s], gc[s], colors[s].felt->pixel);
    XFillRectangle(display[s], window[s], gc[s], left_edge-diameter, windowH-diameter-1,
		right_edge-left_edge+2*diameter, diameter+1);
}

void add_to_ballrack(index)
{
    int ballx;

    /* Redraw ball in lower ball rack */

    if (index < 8) {
	ballx = leftx;
	leftx += diameter;
    } else if (index == 8)
	ballx = (left_edge+right_edge)/2;
    else {
	ballx = rightx;
	rightx -= diameter;
    }
    px[index] = (float)ballx;
    py[index] = (float)(windowH - radius - 1);
    draw_numbered_ball(index);
}

void redraw_ballrack()
{
    int s;

    for (s=0; s<num_screens; s++)
	redraw_brack(s);
}

static void redraw_brack(s)
{
    unsigned char good_ball[max_balls];
    unsigned char found_one;
    float limit;
    int index;
    int i;
    int ballx;

    leftx = left_edge;
    rightx = right_edge;

    draw_ballrack(s);		/* draw empty ballrack */

    for (i=0; i<num_balls; i++)
	good_ball[i] = !on_table[i];

    found_one = 1;
    while(found_one) {
	limit = (left_edge+right_edge)/2;
	found_one = 0;

	/* Find leftmost ball left of center */

	for (i=0; i < num_balls && i < 8; i++)
	    if (good_ball[i] && px[i] < limit) {
		index = i;
		limit = px[i];
		found_one = 1;
	    }
	if (found_one) {
	    ballx = leftx;
	    leftx += diameter;
	} else if (num_balls >= 9) {		/* find the 8-ball */
	    if (good_ball[8]) {
		found_one = 1;
		index = 8;
		ballx = windowW/2;
	    }
	}

	/* find rightmost ball, left of center */

	if (!found_one) {
	    for (i=9; i<num_balls; i++)
		if (good_ball[i] && px[i] > limit) {
		    index = i;
		    limit = px[i];
		    found_one = 1;
		}
	    if (found_one) {
		ballx = rightx;
		rightx -= diameter;
	    }
	}
	if (found_one) {
	    good_ball[index] = 0;
	    px[index] = (float)ballx;
	    py[index] = (float)(windowH - radius - 1);
	    draw_b(s, index);		/* Redraw ball in lower ball rack */
	    if (index != 0) number_b(s, index);
	}
    }
}

void display_score()
{
    int s;

    for (s=0; s<num_screens; s++)
	display_scr(s);
}

static void display_scr(s)
{
    char string[30];
    unsigned char no_swap;
    int idx;

    XSetForeground(display[s], gc[s], colors[s].background->pixel);
    XFillRectangle(display[s], window[s], gc[s], 0, 0, windowW, bottom_edge-border);
    XSetFont(display[s], gc[s], font[s]->fid);

    XSetForeground(display[s], gc[s], colors[s].foreground->pixel);

    strcpy(string, "GAME: ");
    switch (game) {
	case STRAIGHT_POOL:
	    strcat(string, "STRAIGHT POOL");
	    break;
	case EIGHT_BALL:
	    strcat(string, "EIGHT BALL");
	    break;
	case NINE_BALL:
	    strcat(string, "NINE BALL");
	    break;
	case ROTATION:
	    strcat(string, "ROTATION");
	    break;
    }
    XDrawString(display[s], window[s], gc[s], 15, displayY-40, string, strlen(string));

    string[0] = '\0';
    if (!net_game && !lock_game)
	idx = (my_turn ? 0 : 1);
    else {
	if (s == LOCAL && my_turn || s == REMOTE && !my_turn)
	    strcat(string, "(*)");
	if (im_remote || s == REMOTE)
	    idx = 1;
	else
	    idx = 0;
    }
    strcat(string, player_name[idx]);
    draw_rtext(s, displayY-40, string);

    no_swap = (my_turn || net_game || lock_game) && s == 0;

    sprintf(string, "WINS: %d", (no_swap ? wins : losses));
    XDrawString(display[s], window[s], gc[s], 15, displayY, string, strlen(string));
    sprintf(string, "LOSSES: %d", (no_swap ? losses : wins));
    XDrawString(display[s], window[s], gc[s], 190, displayY, string, strlen(string));

    if (game == STRAIGHT_POOL || game == ROTATION) {
	if (no_swap)
	    sprintf(string, "MINE: %d -- YOURS: %d", my_score, his_score);
	else
	    sprintf(string, "MINE: %d -- YOURS: %d", his_score, my_score);
	draw_rtext(s, displayY, string);
    } else if (game == EIGHT_BALL) {
	if (selection_made)
	    if (no_swap ^ !im_high)
		draw_rtext(s, displayY, "HIGH NUMBERS");
	    else
		draw_rtext(s, displayY, "LOW NUMBERS");
    }
}

#define arrow_scale 5
#define arrowhead_ratio 8

void draw_arrow()
{
    XPoint arrow[5];
    int x = (int)px[0];
    int y = (int)py[0];
    int dx = (int)(arrow_scale*vx[0]);
    int dy = (int)(arrow_scale*vy[0]);

    arrow_drawn = TRUE;
    arrow[0].x = x;
    arrow[0].y = y;
    arrow[1].x = dx;
    arrow[1].y = dy;
    arrow[2].x = (dy - dx)/arrowhead_ratio;
    arrow[2].y = (-dx - dy)/arrowhead_ratio;
    arrow[3].x = -arrow[2].x;
    arrow[3].y = -arrow[2].y;
    arrow[4].x = (-dy - dx)/arrowhead_ratio;
    arrow[4].y = (dx - dy)/arrowhead_ratio;
    XSetForeground(display[system], gc[system], colors[system].foreground->pixel);
    XDrawLines(display[system], window[system], gc[system], arrow, 5, CoordModePrevious);
}

void erase_arrow()
{
    XExposeEvent event;
    int minx, miny;
    int maxx, maxy;
    int x, y;
    int dx, dy;

    if (!arrow_drawn) return;
    arrow_drawn = FALSE;

    x = (int)px[0];
    y = (int)py[0];
    dx = (int)(arrow_scale*vx[0]);
    dy = (int)(arrow_scale*vy[0]);
    minx = min(x, min(x+dx, min(x+dx+(dy-dx)/arrowhead_ratio, x+dx-(dy+dx)/arrowhead_ratio)));
    maxx = max(x, max(x+dx, max(x+dx+(dy-dx)/arrowhead_ratio, x+dx-(dy+dx)/arrowhead_ratio)));
    miny = min(y, min(y+dy, min(y+dy-(dx+dy)/arrowhead_ratio, y+dy+(dx-dy)/arrowhead_ratio)));
    maxy = max(y, max(y+dy, max(y+dy-(dx+dy)/arrowhead_ratio, y+dy+(dx-dy)/arrowhead_ratio)));
    event.x = minx;
    event.y = miny;
    event.width = maxx - minx + 1;
    event.height = maxy - miny + 1;
    redraw_display(system, &event);
}

void redraw_display(s, event)
XExposeEvent *event;
{
    XRectangle rectangle;

    rectangle.x = event->x;
    rectangle.y = event->y;
    rectangle.width = event->width;
    rectangle.height = event->height;
    XSetClipRectangles(display[s], gc[s], 0, 0, &rectangle, 1, YXBanded);

    draw_tbl(s);
    if (action_started) {
	redraw_brack(s);
	display_scr(s);
    }
    if (arrow_drawn)
	draw_arrow();
    if (intro_displayed)
	display_intro();
    else if (num_strings[s] == 1)
	draw_ctext(s, save_string1[s]);
    else if (num_strings[s] == 2)
	draw_ctext2(s, save_string1[s], save_string2[s]);

    XSetClipMask(display[s], gc[s], None);
}

void get_window(s, x, y)
int *x, *y;
{
    Window child;

    XTranslateCoordinates(display[s], window[s], XDefaultRootWindow(display[s]),
	0, 0, x, y, &child);
}

int is_monochrome(screen)
Screen *screen;
{
    return((XDefaultVisualOfScreen(screen))->class == StaticGray ||
	   (XDefaultVisualOfScreen(screen))->class == GrayScale);
}

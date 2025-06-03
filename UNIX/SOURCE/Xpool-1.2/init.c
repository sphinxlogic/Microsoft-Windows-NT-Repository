


/*
 * I. ARIT 1992 Hidirbeyli,AYDIN,TR.  09400 
 *              Golden,    CO,   USA. 80401 
 *
 *
 * Copyright (C) 1992 Ismail ARIT 
 *
 * This file  is distributed in the hope that it will be useful, but without any
 * warranty.  No author or distributor accepts responsibility to anyone for
 * the consequences of using it or for whether it serves any particular
 * purpose or works at all. 
 *
 *
 * Everyone is granted permission to copy, modify and redistribute this file
 * under the following conditions: 
 *
 *
 * Permission is granted to anyone to make or distribute copies of the source
 * code, either as received or modified, in any medium, provided that all
 * copyright notices, permission and nonwarranty notices are preserved, and
 * that the distributor grants the recipient permission for further
 * redistribution as permitted by this document. 
 *
 * No part of this program can be used in any commercial product. 
 */





#include <X11/X.h>
#include    <stdio.h>
#include    <string.h>
#include    <X11/Xlib.h>
#include    <X11/Xutil.h>
#include    <X11/cursorfont.h>
#include    "definition.h"
#include    "ball.bmp"
#include    "ball1.bmp"
#include    "ball2.bmp"
#include    "ball3.bmp"
#include    "ball4.bmp"
#include    "ball5.bmp"
#include    "ball6.bmp"
#include    "ball7.bmp"
#include    "ball8.bmp"
#include    "ball9.bmp"
#include    "ball10.bmp"
#include    "ball11.bmp"
#include    "ball12.bmp"
#include    "ball13.bmp"
#include    "ball14.bmp"
#include    "ball15.bmp"
#include    "ball16.bmp"
#include    "tile.bmp"


/* some global variables */
Display        *disp;
int             screen;
Colormap        Cmap;
Window          main_window, Pane;
GC              gc;
GC              XorGc;

XWindowAttributes xwa;
Pixmap          BallBitmap[16];
Pixmap          TileBitmap;



Window          exitbutton, newgame, Yourscoreboard, Myscoreboard;
Window          WhoseTurnWindow;
Window          MyPlus, YourPlus, MyMinus, YourMinus;
Window          TraJectory;


char           *BallBitmapFileName = "ball.bmp";
int             ballWidth, ballHeight, ballHotX, ballHotY;

unsigned int    linewidth = 8;
int             linestyle = LineSolid;
int             capstyle = CapRound;
int             joinstyle = JoinRound;

extern void
setcolor(int color);

int             ascent, descent;
unsigned long   fg, bg, bd;
int             depth;

XWMHints        xwmh =
{
 (InputHint | StateHint),	/* flags */
 False,				/* input */
 NormalState,			/* initial_state */
 0,				/* icon pixmap */
 0,				/* icon window */
 0, 0,				/* icon location */
 0,				/* icon mask */
 0,
};



/* every parameters should go in to the this string here */

Cursor          buttonCursor;

XFontStruct    *fontstruct;	/* Fonts */



char           *progname = "pool table";

void
shadow(button, x, y, xx, yy)
	Window          button;
	int             x, y, xx, yy;
{
	XSetLineAttributes(disp, gc, 1, linestyle, capstyle, joinstyle);

	XSetForeground(disp, gc, get_color("White"));
	XDrawLine(disp, button, gc, x, y, xx, y);
/*	XDrawLine(disp, button, gc, x + 1, y + 1, xx - 1, y + 1); */
	XDrawLine(disp, button, gc, x, y, x, yy);
/*	XDrawLine(disp, button, gc, x + 1, y + 1, x + 1, yy - 1); */
	XSetForeground(disp, gc, get_color("Black"));
	XDrawLine(disp, button, gc, xx, yy, xx, y);
/*	XDrawLine(disp, button, gc, xx - 1, yy, xx - 1, y + 1); */
	XDrawLine(disp, button, gc, xx, yy, x, yy);
/*	XDrawLine(disp, button, gc, xx - 1, yy - 1, x + 1, yy - 1); */
	/* go back to thick style */
	XSetLineAttributes(disp, gc, linewidth, linestyle, capstyle, joinstyle);

	XSync(disp, 0);
}


void
reverse_shadow(button, x, y, xx, yy)
        Window          button;
        int             x, y, xx, yy;
{
        XSetForeground(disp, gc, get_color("Black"));
        XDrawLine(disp, button, gc, x, y, xx, y);
        XDrawLine(disp, button, gc, x + 1, y + 1, xx - 1, y + 1);
        XDrawLine(disp, button, gc, x, y, x, yy);
        XDrawLine(disp, button, gc, x + 1, y + 1, x + 1, yy - 1);
        XSetForeground(disp, gc, get_color("White"));
        XDrawLine(disp, button, gc, xx, yy, xx, y);
        XDrawLine(disp, button, gc, xx - 1, yy, xx - 1, y + 1);
        XDrawLine(disp, button, gc, xx, yy, x, yy);
        XDrawLine(disp, button, gc, xx - 1, yy - 1, x + 1, yy - 1);
        XSync(disp, 0);
}


void
PutIn(win, string, gc, fg, bg)
        Window          win;
        char           *string;
        GC              gc;
        int             fg, bg;
{

        int             x, y, w, h, len;
        XWindowAttributes attrib;
        XGetWindowAttributes(disp, win, &attrib);
        w = attrib.width;
        h = attrib.height;

        XSetWindowBackground(disp, win, bg);
        XSetForeground(disp, gc, fg);
        XSetBackground(disp, gc, bg);

        XClearWindow(disp, win);
        x = (w - XTextWidth(fontstruct, string, (len = strlen(string)))) / 2;

        y = 1 + (h + fontstruct->max_bounds.ascent - fontstruct->max_bounds.descent) / 2;

        XDrawImageString(disp, win, gc, x, y, string, len);
        shadow(win, 0, 0, w - 1, h - 1);
        XSync(disp, 0);

}







void
show_button_pressed(the_win, gc, the_string)
        Window          the_win;
        char           *the_string;
        GC              gc;
{
        char            str[20];
        XWindowAttributes attrib;



        XGetWindowAttributes(disp, the_win, &attrib);
        reverse_shadow(the_win, 0, 0, attrib.width, attrib.height);
        sprintf(str, "%s\0", the_string);
       PutIn(the_win, str, gc, get_color(MENU_FG), get_color(MENU_BG));
       
/* we gotta put delay hereeee..*/ 

        PutIn(the_win, str, gc, get_color(MENU_FG), get_color(BUTTON_COLOR));
}





Window
create_button(parent, x, y, w, h, fg, bg)
	Window          parent;
	int             x, y, w, h, fg, bg;
{
	Window          button;
	button = XCreateSimpleWindow(disp, parent, x, y, w, h, 1, fg, bg);
	XDefineCursor(disp, button, buttonCursor);
	XSelectInput(disp, button, ExposureMask | ButtonPressMask);
	XMapWindow(disp, button);
	XSync(disp, 0);
	shadow(button, 0, 0, w - 1, h - 1);
	return (button);

}




initgraph(argc, argv)
	int             argc;
	char          **argv;
{
	int             button_wide;
	int             button_height;
	int             slide_wide, slide_height;
	unsigned long   bw;	/* Border width */
	XGCValues       gcv;	/* Struct for creating GC */
	XSizeHints      xsh;	/* Size hints for window manager */
	int             direction;
	XCharStruct    *xchar;
	int             tableBorder;
        char           *displayName  ;  

if(argc>2){
  if(!strcmp(argv[1],"-d"))
    displayName = argv[2];
  else
    {printf("usage: %s -d displayName \n",argv[0]);exit(1);}
}else displayName = (char *)0;
printf("%s \n",displayName);

	if ((disp = XOpenDisplay(displayName)) == NULL) {
		fprintf(stderr, "%s: can't open %s\n", progname, XDisplayName(displayName));
		exit(1);
	}
	Cmap = DefaultColormap(disp, DefaultScreen(disp));


	if ((fontstruct = XLoadQueryFont(disp, FONT)) == NULL) {
		fprintf(stderr, "display %s doesn't know font %s\n",
			DisplayString(disp), FONT);
		exit(1);
	}
	bd = WhitePixel(disp, DefaultScreen(disp));
	/*
	 * bg = BlackPixel(disp, DefaultScreen(disp)); 
	 */

	bg = get_color(MAIN_BG);
	fg = BlackPixel(disp, DefaultScreen(disp));

	/*
	 * Set the border width of the window,  and the gap between the text
	 * and the edge of the window. 
	 */
	bw = BORDER;
	xsh.flags = (PPosition | PSize);
	xsh.height = 500 + 20;
	xsh.width = 875 + 20;	/* these are window initial sizes */
	xsh.x = 0;
	xsh.y = 0;

	/*
	 * Create the Window with the information in the XSizeHints, the
	 * border width,  and the border & background pixels. 
	 */
	main_window = XCreateSimpleWindow(disp, DefaultRootWindow(disp),
					xsh.x, xsh.y, xsh.width, xsh.height,
					  bw, bd, bg);


	/* None is the icon_bitmap */

	XSetStandardProperties(disp, main_window, progname, progname, None,
			       argv, argc, &xsh);
	XSetWMHints(disp, main_window, &xwmh);

	xwa.colormap = DefaultColormap(disp, DefaultScreen(disp));
	xwa.bit_gravity = CenterGravity;

	XChangeWindowAttributes(disp, main_window, (CWColormap | CWBitGravity), &xwa);

	gcv.font = fontstruct->fid;
	gcv.foreground = fg;
	gcv.background = bg;

	gc = XCreateGC(disp, main_window, (GCFont | GCForeground | GCBackground), &gcv);
	if (!gc)
		printf("problem with the gc..\n");

	gcv.function = GXxor;
	XorGc = XCreateGC(disp, main_window, (GCFunction | GCFont | GCForeground
					      | GCBackground), &gcv);
	if (!XorGc)
		printf("problem with the XorGc ..\n");

	XSetLineAttributes(disp, gc, linewidth, linestyle, capstyle, joinstyle);
	XSetBackground(disp, XorGc, get_color(PANE_BG));

	XSelectInput(disp, main_window, ExposureMask | KeyPressMask | ButtonReleaseMask | ButtonPressMask | StructureNotifyMask);

	XTextExtents(fontstruct, "W", 1, &direction, &ascent, &descent, &xchar);

	button_wide = 100;
	button_height = 17;
	slide_wide = 15;
	slide_height = 150;
	fg = get_color(MENU_FG);
	bg = get_color(MENU_BG);

	buttonCursor = XCreateFontCursor(disp, BUTTONCURSOR);

	fg = get_color(PANE_FG);
	bg = get_color(PANE_BG);
	tableBorder = 3;
	Pane = XCreateSimpleWindow(disp, main_window,
		(button_wide + 20), 30, (xsh.width - button_wide - 30 - 20),
	       (xsh.height - 40 - 20), tableBorder, get_color("Black"), bg);
	XSelectInput(disp, Pane, ButtonPressMask | ButtonReleaseMask | ButtonMotionMask
		     | ExposureMask | PointerMotionHintMask);
	XMapWindow(disp, Pane);

	XMapWindow(disp, main_window);
	XSync(disp, 0);

	/* exitbutton,newgame,Yourscoreboard,Myscoreboard */

	newgame = create_button(main_window, 10, 70, button_wide, button_height, fg, bg);
	TraJectory =  create_button(main_window, 10, 70 + (button_height + 10) * 1, button_wide, button_height, fg, bg);

	Yourscoreboard = create_button(main_window, 10, 370 + (button_height + 10) * 2, button_wide / 2 - 20, button_height * 2, fg, bg);
	YourPlus = create_button(main_window, 10 + button_wide / 2 - 20, 370 + (button_height + 10) * 2, 20, button_height, fg, bg);
	YourMinus = create_button(main_window, 10 + button_wide / 2 - 20, 370 + (button_height + 10) * 2 + button_height, 20, button_height, fg, bg);


	Myscoreboard = create_button(main_window, 10 + button_wide / 2 + 2, 370 + (button_height + 10) * 2, button_wide / 2 - 20, button_height * 2, fg, bg);
	MyPlus = create_button(main_window, 10 + button_wide - 20, 370 + (button_height + 10) * 2, 20, button_height, fg, bg);
	MyMinus = create_button(main_window, 10 + button_wide - 20, 370 + (button_height + 10) * 2 + button_height, 20, button_height, fg, bg);




	WhoseTurnWindow = create_button(main_window, 10, 480, button_wide - 20, button_height * 2, fg, bg);
	exitbutton  = create_button(main_window, 10, 70 + (button_height + 10) * 2, button_wide , button_height, fg, bg);





	/*
	 * one last thing before we go,    ballWidth,ballHeight,ballHotX,
	 * ballHotY 
	 */

	depth = XAllPlanes();

	BallBitmap[0] = XCreateBitmapFromData(disp, Pane, ball_bits, ball_width, ball_height);
	if (!BallBitmap[0]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};

	/* because we defined ball 1 as ball numbered 8 in the program */
	/* v  */
	/* v  */
	/* V  */
	BallBitmap[1] = XCreateBitmapFromData(disp, Pane, ball8_bits, ball_width, ball_height);
	if (!BallBitmap[1]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[2] = XCreateBitmapFromData(disp, Pane, ball1_bits, ball_width, ball_height);
	if (!BallBitmap[2]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};

	BallBitmap[3] = XCreateBitmapFromData(disp, Pane, ball2_bits, ball_width, ball_height);
	if (!BallBitmap[3]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[4] = XCreateBitmapFromData(disp, Pane, ball3_bits, ball_width, ball_height);
	if (!BallBitmap[4]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[5] = XCreateBitmapFromData(disp, Pane, ball4_bits, ball_width, ball_height);
	if (!BallBitmap[5]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[6] = XCreateBitmapFromData(disp, Pane, ball5_bits, ball_width, ball_height);
	if (!BallBitmap[6]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[7] = XCreateBitmapFromData(disp, Pane, ball6_bits, ball_width, ball_height);
	if (!BallBitmap[7]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[8] = XCreateBitmapFromData(disp, Pane, ball7_bits, ball_width, ball_height);
	if (!BallBitmap[8]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[9] = XCreateBitmapFromData(disp, Pane, ball9_bits, ball_width, ball_height);
	if (!BallBitmap[9]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[10] = XCreateBitmapFromData(disp, Pane, ball10_bits, ball_width, ball_height);
	if (!BallBitmap[10]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[11] = XCreateBitmapFromData(disp, Pane, ball11_bits, ball_width, ball_height);
	if (!BallBitmap[11]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[12] = XCreateBitmapFromData(disp, Pane, ball12_bits, ball_width, ball_height);
	if (!BallBitmap[12]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[13] = XCreateBitmapFromData(disp, Pane, ball13_bits, ball_width, ball_height);
	if (!BallBitmap[13]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[14] = XCreateBitmapFromData(disp, Pane, ball14_bits, ball_width, ball_height);
	if (!BallBitmap[14]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};
	BallBitmap[15] = XCreateBitmapFromData(disp, Pane, ball15_bits, ball_width, ball_height);
	if (!BallBitmap[15]) {
		printf("can't read bitmap ..\n");
		exit(0);
	};



	TileBitmap = XCreateBitmapFromData(disp, main_window, tile_bits, tile_width, tile_height);
	if (!TileBitmap) {
		printf("can't read bitmap ..\n");
		exit(0);
	};



}

void
LayTheFloor(void)
{
	int             i, j;
	setbkcolor(get_color(MAIN_BG));
	for (i = 0; i < 900; i += tile_width)
		for (j = 0; j < 600; j += tile_height) {
			XCopyPlane(disp, TileBitmap, main_window, gc, 0, 0, tile_width, tile_height, i, j, 0x01);
		}
	XSync(disp, 0);
	setbkcolor(get_color(PANE_BG));
};



void
showBall(int x, int y, int number)
{
	XCopyPlane(disp, BallBitmap[number], Pane, gc, 0, 0, ball_width, ball_height, x, y, 0x01);
	XSync(disp, 0);
}

void
showBallOut(int x, int y, int color, int number)
{
	setcolor(color);
	setbkcolor(get_color("black"));
	XCopyPlane(disp, BallBitmap[number], main_window, gc, 0, 0, ball_width, ball_height, x, y, 0x01);
	XSync(disp, 0);
	setbkcolor(get_color(PANE_BG));
}


void
line(int x, int y, int xx, int yy)
{
	XDrawLine(disp, Pane, gc, x, y, xx, yy);
	XSync(disp, 0);
};

void
circle(int x, int y, int rad)
{
	XDrawArc(disp, Pane, gc, x - rad, y - rad, rad * 2, rad * 2, 0, 360 * 64);
	XSync(disp, 0);
};


void
circleOut(int x, int y, int rad, int color)
{
	setcolor(color);
	setbkcolor(get_color(MAIN_BG));
	XDrawArc(disp, main_window, gc, x - rad, y - rad, rad * 2, rad * 2, 0, 360 * 64);
	XSync(disp, 0);
	setbkcolor(get_color(PANE_BG));
};




void
Fillcircle(int x, int y, int rad)
{
	/* this is for drawing pockets ,,     table 0,0  at 120,30 */
	setcolor(get_color("black"));
	XFillArc(disp, main_window, gc, x - rad + 120, y - rad + 30, rad * 2, rad * 2, 0, 360 * 64);
};



void
rectangle(int x, int y, int xx, int yy)
{
	XDrawRectangle(disp, Pane, gc, x, y, xx, yy);
	XSync(disp, 0);
};

void
Fillrectangle(int x, int y, int xx, int yy)
{
	setcolor(get_color(BORDER_COLOR));
	XFillRectangle(disp, main_window, gc, x + 120, y + 30, xx, yy);
	XSync(disp, 0);
};





void
SyncDisplay(void)
{
	XSync(disp, 0);
};

void
ClearTable(void)
{
	XClearWindow(disp, Pane);
};

void
RubberLineOnPane(int type, int x, int y, int xx, int yy, int color)
{
	if (type == TRAJECTORY)
		XSetLineAttributes(disp, gc, 1, linestyle, capstyle, joinstyle);
	XSetFunction(disp, gc, GXxor);
	setcolor(color);
	XDrawLine(disp, Pane, gc, x, y, xx, yy);
	XSync(disp, 0);
	XSetFunction(disp, gc, GXcopy);
	XSetLineAttributes(disp, gc, linewidth, linestyle, capstyle, joinstyle);
};


/*
 * graphics.c
 * @(#)graphics.c	1.32 (UCLA) 10/16/92
 *
 * xbomb is Copyright (C) 1992 by Matthew Merzbacher, Los Angeles, CA.
 * All rights reserved.  Permission is granted to freely distribute
 * this as long as this copyright message is retained intact.
 * Permission to distribute this as part of a commerical product
 * requires explicit permission of the author.
 *
 */

#include "icon.bit"

#include "xbomb.h"
#include <sys/param.h>

	
int borderWidth = -1;
char *geom = "";
char *bitdir = NULL;

	
Display     *dpy;
int         screen;
GC          gc;
	
Window      rootWindow, puzzleWindow;
long        FgPixel, BgPixel, BdPixel;
	
Pixmap pixmaps[20];
	


Pixmap
piecePixmap(x,y) 
	int x, y;
{ 
	if (map[x][y].status == KEMPTY) {
		if (!ted)
			return(pixmaps[map[x][y].neighbors]);
		else
			return(pixmaps[map[x][y].neighbors - map[x][y].neighbor_kbombs]);
	}
	if (map[x][y].status == KBOMB)
		return(pixmaps[BOMB]);
	if (map[x][y].status == EBOMB)
		return(pixmaps[EBOMB]);
	if (map[x][y].flag)
		return(pixmaps[FLAG]);
	if (map[x][y].status == BOMB)
		return(pixmaps[EMPTY]);
	if (map[x][y].status == EMPTY)
		return(pixmaps[EMPTY]);
	
	fprintf(stderr,"Unknown status: (%d, %d) %d\n", x, y, map[x][y].status);
}

void
putpix(c, r)
	int c, r;
{
	XCopyPlane(dpy, piecePixmap(c,r), puzzleWindow, gc,
		   0, 0, WIDTH, HEIGHT,
		   c*WIDTH, (r+1)*HEIGHT, 1);
}


int
xfcb(file, w, h, ret)
	char *file;
	int *w, *h;
	Pixmap *ret;
{
	int hx, hy;
	int retcode;
	char s[80];

	retcode = XReadBitmapFile(dpy, rootWindow, file, w, h, ret, &hx, &hy);

	if ((retcode == BitmapOpenFailed) || (retcode == BitmapSuccess)) {
		return(retcode);
	}

	switch(retcode) {
	case BitmapFileInvalid:
		sprintf(s,"Invalid bitmap file format: %s", file);
		bail(s);
		break;
	case BitmapNoMemory:
		sprintf(s,"Insufficient Memory opening file: %s", file);
		bail(s);
		break;
	default:
		bail("A very bad thing has happened while reading a bitmap");
	}
}


static struct bit_to_load {
	char *name;
	int place;
} btl[] = {
	"bomb.bit", BOMB,
	"empty.bit", EMPTY,
	"ebomb.bit", EBOMB,
	"flag.bit", FLAG,
	"blank.bit", BLANK,
	"think.bit", THINK,
	"known0.bit", 0,
	"known1.bit", 1,
	"known2.bit", 2,
	"known3.bit", 3,
	"known4.bit", 4,
	"known5.bit", 5,
	"known6.bit", 6,
	"known7.bit", 7,
	"known8.bit", 8,
	"known9.bit", 9,
	NULL, 19
	};


make_bitmaps()
{
	int dummyw, dummyh;
	int i, gotit;
	char filename[MAXPATHLEN];

	static char *defbitdir = "standard";

	for (i = 0; btl[i].name != NULL; i++) {
		gotit = 0;
		if (bitdir) {
			sprintf(filename, "%s/%s/%s", 
				_PATH_BITMAPS, bitdir, btl[i].name);
			if (xfcb(filename, &dummyw, &dummyh, 
			     &(pixmaps[btl[i].place])) == BitmapSuccess)
				gotit = 1;
		}
		if (!gotit) {
			sprintf(filename, "%s/%s/%s", 
				_PATH_BITMAPS, defbitdir, btl[i].name);
			if (xfcb(filename, &dummyw, &dummyh, 
				 &(pixmaps[btl[i].place])) == BitmapOpenFailed)
			{
				sprintf(filename, "Unable to open bitmap: %s", 
					btl[i].name);
				bail(filename);
			}
		}
	}
							

/***
	xfcb("bitmaps/bomb.bit", &dummyw, &dummyh, &(pixmaps[BOMB]));
	xfcb("bitmaps/empty.bit", &dummyw, &dummyh, &(pixmaps[EMPTY]));
	xfcb("bitmaps/ebomb.bit", &dummyw, &dummyh, &(pixmaps[EBOMB]));
	xfcb("bitmaps/flag.bit", &dummyw, &dummyh, &(pixmaps[FLAG]));
	xfcb("bitmaps/blank.bit", &dummyw, &dummyh, &(pixmaps[BLANK]));
	xfcb("bitmaps/think.bit", &dummyw, &dummyh, &(pixmaps[THINK]));
	xfcb("bitmaps/known0.bit", &dummyw, &dummyh, &(pixmaps[0]));
	xfcb("bitmaps/known1.bit", &dummyw, &dummyh, &(pixmaps[1]));
	xfcb("bitmaps/known2.bit", &dummyw, &dummyh, &(pixmaps[2]));
	xfcb("bitmaps/known3.bit", &dummyw, &dummyh, &(pixmaps[3]));
	xfcb("bitmaps/known4.bit", &dummyw, &dummyh, &(pixmaps[4]));
	xfcb("bitmaps/known5.bit", &dummyw, &dummyh, &(pixmaps[5]));
	xfcb("bitmaps/known6.bit", &dummyw, &dummyh, &(pixmaps[6]));
	xfcb("bitmaps/known7.bit", &dummyw, &dummyh, &(pixmaps[7]));
	xfcb("bitmaps/known8.bit", &dummyw, &dummyh, &(pixmaps[8]));
	xfcb("bitmaps/known9.bit", &dummyw, &dummyh, &(pixmaps[9]));
***/
}

void
cleanup_pixmaps()
{
	if (pixmaps[EMPTY] != NULL) XFreePixmap(dpy, pixmaps[EMPTY]);
	if (pixmaps[BOMB] != NULL) XFreePixmap(dpy, pixmaps[BOMB]);
	if (pixmaps[EBOMB] != NULL) XFreePixmap(dpy, pixmaps[EBOMB]);
	if (pixmaps[FLAG] != NULL) XFreePixmap(dpy, pixmaps[FLAG]);
	if (pixmaps[BLANK] != NULL) XFreePixmap(dpy, pixmaps[BLANK]);
	if (pixmaps[THINK] != NULL) XFreePixmap(dpy, pixmaps[THINK]);
	if (pixmaps[0] != NULL) XFreePixmap(dpy, pixmaps[0]);
	if (pixmaps[1] != NULL) XFreePixmap(dpy, pixmaps[1]);
	if (pixmaps[2] != NULL) XFreePixmap(dpy, pixmaps[2]);
	if (pixmaps[3] != NULL) XFreePixmap(dpy, pixmaps[3]);
	if (pixmaps[4] != NULL) XFreePixmap(dpy, pixmaps[4]);
	if (pixmaps[5] != NULL) XFreePixmap(dpy, pixmaps[5]);
	if (pixmaps[6] != NULL) XFreePixmap(dpy, pixmaps[6]);
	if (pixmaps[7] != NULL) XFreePixmap(dpy, pixmaps[7]);
	if (pixmaps[8] != NULL) XFreePixmap(dpy, pixmaps[8]);
	if (pixmaps[9] != NULL) XFreePixmap(dpy, pixmaps[9]);
}


requestColor(name)
	char *name;
{
	XColor truecolor, availcolor;
	
	if (XAllocNamedColor(dpy,
			     DefaultColormap(dpy, DefaultScreen(dpy)),
			     name, &availcolor, &truecolor) == 0) {
		fprintf(stderr, "Color '%s' unknown\n", name);
		exit(1);
	}
#if 0
	if (truecolor.red != availcolor.red ||
	    truecolor.green != availcolor.green ||
	    truecolor.blue != availcolor.blue) {
		fprintf(stderr, "Warning: %s color may be wrong\n", name);
	}
#endif
	
	return availcolor.pixel;
}



void 
xmess(mess) 
	char *mess;
{
	XDrawString(dpy, puzzleWindow, gc, 10, HEIGHT*(height + 2) + 8, 
		mess, strlen(mess));
}


int
eventloop()
{
	XEvent event;
	XExposeEvent *ee = (XExposeEvent *)&event;
	XKeyPressedEvent *ke = (XKeyPressedEvent *)&event;
	XButtonPressedEvent *be = (XButtonPressedEvent *)&event;
	char keybuff[10];
	int nchar;
	char *mess1 = "You won!";
	char *mess2 = "(press anything to exit - 'r' to get your seed - 'n' to play again)";
	char *mess2a = "(press anything to play again - 'r' to get your seed - 'q' to quit)";
	char *mess3 = "Kabooom!";
	char *autosolstr = "!@#$%^&*()";
	char *solstr = "1234567890";
	char schar;
	int i, r, c;
	int row, column, nrow, ncolumn;

	while(1) {
		if (dead || ((numempty == 1) && (numflags == 0))) {
			if (!dead) xmess(mess1);
			else {
				reveal();
				xmess(mess3);
			}
			if (playagain)
				XDrawString(dpy, puzzleWindow, gc, 10, 
					    HEIGHT*(height + 2) + 25, mess2a, 
					    strlen(mess2a));
			else
				XDrawString(dpy, puzzleWindow, gc, 10, 
					    HEIGHT*(height + 2) + 25, mess2, 
					    strlen(mess2));
			while (1) {
				XNextEvent(dpy, &event);
				switch(event.type) {
				case KeyPress:
					nchar = XLookupString(ke, keybuff, 1, NULL, NULL);
					if (nchar > 0) {
						if (keybuff[0] == 'r')
							printf("Using seed: %i\n",seed);
						if ((playagain && keybuff[0] != 'q') || (keybuff[0] == 'n'))
							return(1);
					}
					finish();
					break;

				case ButtonPress:
					if (playagain)
						return(1);
					finish();
					break;

				case Expose:
					if (! ee->count)
						repaint();
					break;

				default:
					break;
				}
			}
		}

		XNextEvent(dpy, &event);
		switch(event.type) {
		case KeyPress:
			nchar = XLookupString(ke, keybuff, 1, NULL, NULL);
			i = 0;
			
			if (nchar > 0) {
				switch (keybuff[0]) {
				case '\021':
					finish();

				case 'g':
				case 'G':
					place_random(keybuff[0]=='g' ? EMPTY : BOMB);
					break;

				case 'S':
				case 's':
					scan_em();
					break;

				case 'p':
					place_em();
					print_score();
					break;
				
				case 'P':
				case 'B':
				case 'b':
					while (place_em() || scan_em())
						print_score();
					print_score();
					break;

				case 'h':
				case 'H':
				case '?':
					help_me();
					break;
					
				case '0':
					keybuff[0] += 10;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					for (i = 0; solstr[i] != keybuff[0]; i++);
					invoke_solvers(i,
						       XTOCOLUMN(be->x),
						       YTOROW(be->y));
					print_score();
					break;
				
				case '!':
				case '@':
				case '#':
				case '$':
				case '%':
				case '^':
				case '&':
				case '*':
				case '(':
				case ')':
					for (i = 0; autosolstr[i] != keybuff[0]; i++);

					if (solvers[i]) {
						while (invoke_solvers(i, -1, -1) ||
						       place_em() || 
						       scan_em())
							print_score();
						}
					break;
				}
			}
			break;
			
		case ButtonPress:
			button = event.xbutton.button;
			shifted = (event.xbutton.state & ShiftMask) ? 1 : 0;
			column = XTOCOLUMN(be->x);
			row = YTOROW(be->y);
			break;
			
		case ButtonRelease:
			ncolumn = XTOCOLUMN(be->x);
			nrow = YTOROW(be->y);
			if ((button == event.xbutton.button)
			    && (ncolumn == column) && (nrow == row)
			    && (column > 0) && (column <= width)
			    && (row > 0) && (row <= height))
				do_move(column, row);
			break;
			
		case Expose:
			if (! ee->count)
				repaint();
			break;
			
		default:
			break;
		}
	}
}

reveal()
{
	int c, r;
	
	for (c = 1; c <= width; c++)
		for (r = 1; r <= height; r++) {
			if (map[c][r].status == BOMB) map[c][r].status = EBOMB;
		}
	
	repaint();
	
}

help_me() {
	int sm;
	char *mess1 = "Keyboard: s = scan; p = place; b = both (s & p); ^Q = quit";
	char *mess2 = "Mouse: Left = empty; Right|Center|<Shift>-Any = flag";

	sm = strlen(mess1);
	XDrawString(dpy, puzzleWindow, gc, WIDTH, 
		    (height+3)*HEIGHT-25, mess1, sm);

	sm = strlen(mess2);
	XDrawString(dpy, puzzleWindow, gc, WIDTH,
		    (height+3)*HEIGHT-5, mess2, sm);

}

repaint()
{
	int c, r;
	char *mess1 = "!sbmoB";
	char *mess2 = "? for help";
	int sm;

	XClearWindow(dpy, puzzleWindow);
	
	for (c = 1; c <= width; c++)
		for (r = 1; r <= height; r++) {
			putpix(c, r);
		}
	
	print_score();

	sm = strlen(mess1);
	XDrawString(dpy, puzzleWindow, gc, WIDTH*((width+1)/2) - sm, 
		    HEIGHT+12, mess1, sm);

	sm = strlen(mess2);
	XDrawString(dpy, puzzleWindow, gc, WIDTH*((width+1)/2) - sm, 
		    HEIGHT+30, mess2, sm);
	
}


do_move(c, r)
	int c, r;
{
	int move;
	
	move = FLAG;
	
	if ((button == Button1) & !shifted) move = CHECK;
	
	switch (move) {
		
	case CHECK:
		move_somewhere(c, r);
		break;
		
	case FLAG:
		place_flag(c, r);
		break;
	}
	
	putpix(c, r);
	
	print_score();
	
}




graphics_main(argc, argv)
	int argc;
	char **argv;
{
	int i, temp;
	Visual visual;
	XGCValues xgcv;
	XSetWindowAttributes xswa;
	XSizeHints sizehints;
	
	int total_width, total_height;
	int maxwidth, maxheight;

	/*************************************/
	/** configure the window size hints **/
	/*************************************/
	
	maxwidth = DisplayWidth(dpy, screen)/WIDTH - 2;
	maxheight = DisplayHeight(dpy, screen)/HEIGHT - 4;
	
	if (maxwidth > MAXWIDTH) maxwidth = MAXWIDTH;
	if (maxheight > MAXHEIGHT) maxheight = MAXHEIGHT;
	
	if (width < 2)
		width = DEFWIDTH;
	
	if (width > maxwidth)
		width = maxwidth;
	
	if (height < 2)
		height = DEFHEIGHT;
	
	if (height > maxheight)
		height = maxheight;
	
	if (numbombs < 1)
		numbombs = width*height/4;
	
	if (numbombs > (width*height)/2)
		numbombs = (width*height)/2;
	
	
	total_width = (WIDTH * (width+2));
	total_height = (HEIGHT * (height+3));
	
	sizehints.flags = PMinSize | PMaxSize | PPosition | PSize;
	sizehints.min_width = total_width;
	sizehints.min_height = total_height;
	sizehints.max_width = total_width;
	sizehints.max_height = total_height;
	sizehints.width = total_width;
	sizehints.height = total_height;
	sizehints.x = 100;
	sizehints.y = 300;
	
	if (geom && strlen(geom)) {
		int mmwidth, mmheight, flags;
		
		flags = XParseGeometry(geom, &sizehints.x, &sizehints.y,
				       &mmwidth, &mmheight);
		if (flags & (XValue | YValue))
			sizehints.flags |= USPosition;
	}
	
	/*******************************************************************/
	/** create the puzzle main window and set its standard properties **/
	/*******************************************************************/
	
	xswa.event_mask = ExposureMask | ButtonPressMask |
		ButtonReleaseMask | KeyPressMask;
	xswa.background_pixel = BgPixel;
	xswa.border_pixel = BdPixel;
	xswa.cursor = XCreateFontCursor(dpy, XC_hand1);
	
	visual.visualid = CopyFromParent;
	
	rootWindow = RootWindow(dpy, screen);
	
	puzzleWindow = XCreateWindow(dpy, rootWindow,
				     sizehints.x, sizehints.y,
				     sizehints.width, sizehints.height,
				     borderWidth,
				     DefaultDepth(dpy,screen),
				     InputOutput,
				     &visual,
				     CWCursor | CWEventMask | CWBackPixel | CWBorderPixel,
				     &xswa);
	
	/*******************************************************************/
	/** define its name and icon-bitmap                               **/
	/*******************************************************************/
	
	XSetStandardProperties(dpy, puzzleWindow, "!sbmob","!sbmoB",
			       XCreateBitmapFromData(dpy, rootWindow,
						     icon_bits,
						     icon_width, icon_height),
			       argv, argc, &sizehints);
	
	XMapWindow(dpy, puzzleWindow);
	
	xgcv.foreground = FgPixel;
	xgcv.background = BgPixel;
	xgcv.line_width = 0;
	xgcv.font = XLoadFont(dpy, "rk16");
	gc = XCreateGC(dpy, puzzleWindow,
		       GCForeground|GCBackground|GCLineWidth|GCFont,
		       &xgcv);
	
	make_bitmaps();
}

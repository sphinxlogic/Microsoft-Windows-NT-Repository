/*
** Astrolog (Version 2.25) File: graphics.c
** (Initially programmed 10/23-29/1991)
**
** IMPORTANT: the graphics database and chart display routines used in
** this program are Copyright (C) 1991-1992 by Walter D. Pullen. Permission
** is granted to freely use and distribute these routines provided one
** doesn't sell, restrict, or profit from them in any way. Modification
** is allowed provided these notices remain with any altered or edited
** versions of the program.
*/

#include "astrolog.h"

#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
  bit m[BITMAPX][BITMAPY];
} bitmapstruct;
typedef bitmapstruct *bitmap;

#define P(B, X, Y) ((B)->m[X][Y])
#define NEW(B) B = (bitmapstruct *) malloc(sizeof(bitmapstruct))
#define DISPOSE(B) free(B)


/*
*******************************************************************************
** Variables
*******************************************************************************
*/

char *maincolorname[]
  = {"black", "white", "gold", "gray35", "tan", "pink", "blue"};
char *rainbowcolorname[]
  = {"white", "orangered", "orange", "yellow", "green",
       "cyan", "deepskyblue", "violet"};
int elemcolorindex[]   = {-1,  4, -4, -6};
int aspectcolorindex[] = { 1, -3,  6, -1, -4, -5,  4,  4, -2, -2,
			  -7, -7, -7,  2,  5,  5,  2,  2,  5};

color maincolor[6+1], rainbowcolor[7+1],
  elemcolor[4], aspectcolor[ASPECTS+1], objectcolor[TOTAL+1], hilite, gray;

Colormap cmap;
Display *disp;
Font font;
GC gc, pmgc;
KeySym key;
Pixmap pixmap, icon;
Window window, root;
XEvent event;
XSizeHints hint;
XWMHints *wmhint;
int screen, depth;
char xkey[10];
color fg, bg, on = ON, off = OFF;

bitmap bm;
char modex = 'c', outputfile[STRING] = "tty", string[STRING];
int xmono = FALSE, xcolor = TRUE, xreverse = FALSE,
  xbitmap = FALSE, xroot = FALSE, xtext = TRUE, bonus = FALSE, label = TRUE,
  turtlex = 0, turtley = 0, scale = 200, chartx = DEFAULTX, charty = DEFAULTY,
  degree = 0, xnow = FALSE;
double Mon1, Day1, Yea1, Tim1, Zon1, Lon1, Lat1, tilt = 0.0;

#define STORE1  Mon1 = Mon; Day1 = Day; Yea1 = Yea; Tim1 = Tim; Zon1 = Zon; \
  Lon1 = Lon; Lat1 = Lat
#define RECALL1 Mon = Mon1; Day = Day1; Yea = Yea1; Tim = Tim1; Zon = Zon1; \
  Lon = Lon1; Lat = Lat1


/*
*******************************************************************************
** Astrolog icon
*******************************************************************************
*/

#define icon_width 63
#define icon_height 32
static char icon_bits[] = {
 0x00,0x00,0x00,0xa8,0x0a,0x00,0x00,0x00,0x00,0x00,0x40,0x55,0x55,0x01,0x00,
 0x00,0x00,0x00,0xa8,0xaa,0xaa,0x0a,0x00,0x00,0x00,0x00,0x54,0xf5,0x57,0x15,
 0x00,0x00,0x00,0x80,0xaa,0xaa,0xaa,0xaa,0x00,0x00,0x00,0x40,0xd5,0xff,0xff,
 0x55,0x01,0x00,0x00,0xa0,0xaa,0xaa,0xaa,0xaa,0x02,0x00,0x00,0x50,0xfd,0xff,
 0xff,0x5f,0x05,0x00,0x00,0xa8,0xaa,0x2a,0xaa,0xaa,0x0a,0x00,0x00,0xd4,0xff,
 0xaf,0xfa,0xff,0x15,0x00,0x00,0xaa,0x2a,0x00,0x00,0xaa,0x2a,0x00,0x00,0xf5,
 0xbf,0xaa,0xaa,0xfe,0x57,0x00,0x80,0xaa,0x02,0x00,0x00,0xa0,0xaa,0x00,0x40,
 0xfd,0xab,0xfa,0xaf,0xea,0x5f,0x01,0xa0,0xaa,0x80,0xff,0xff,0x80,0xaa,0x02,
 0x50,0xff,0xea,0xff,0xff,0xab,0x7f,0x05,0xa0,0x2a,0xf0,0xff,0xff,0x07,0xaa,
 0x02,0xd0,0xbf,0xfa,0x0f,0xf8,0xaf,0x7e,0x05,0xa8,0x0a,0xfc,0x01,0xc0,0x1f,
 0xa8,0x0a,0xd4,0xaf,0x7e,0x00,0x00,0xbf,0xfa,0x15,0xa8,0x0a,0x3f,0x00,0x00,
 0x7e,0xa8,0x0a,0xf4,0xaf,0x1f,0xe0,0x03,0xfc,0xfa,0x15,0xaa,0x82,0x0f,0xdc,
 0x1d,0xf8,0xa0,0x2a,0xf4,0xab,0x07,0x23,0x62,0xf0,0xea,0x17,0xaa,0xc2,0x87,
 0x91,0xc4,0xf0,0xa1,0x2a,0xf4,0xeb,0xc3,0xd0,0x85,0xe1,0xeb,0x17,0xaa,0xe0,
 0x83,0x91,0xc4,0xe0,0x83,0x2a,0xf5,0xeb,0x03,0x23,0x62,0xe0,0xeb,0x57,0xaa,
 0xe0,0x01,0xdc,0x1d,0xc0,0x83,0x2a,0xf5,0xeb,0x01,0xe0,0x03,0xc0,0xeb,0x57,
 0xaa,0xe0,0x01,0x00,0x00,0xc0,0x83,0x2a,0xfd,0xeb,0x01,0x00,0x00,0xc0,0xeb,
 0x5f};


/*
*******************************************************************************
** Standard procedures
*******************************************************************************
*/

sort(n1, n2)
int *n1, *n2;
{
  int temp;
  if (*n1 > *n2) {
    temp = *n1;
    *n1  = *n2;
    *n2  = temp;
  }
}

int intinstring(str)
char **str;
{
  int num = 0, i;
  for (i = 0; ; i++) {
    if (**str < '0' || **str > '9')
      return num > 0 ? num : (i < 1 ? 1 : 0);
    num = num*10+(**str)-'0';
    (*str)++;
  }
}

writefile()
{
  FILE *data;
  char formatw = BITMAPMODE, tty;
  tty = (outputfile[0] == 't' && outputfile[1] == 't' &&
	 outputfile[2] == 'y' && outputfile[3] == 0);
  while (TRUE) {
    if (tty) {
      printf("Enter name of file to write X bitmap to - ");
      scanf("%s", outputfile);
    }
    data = fopen(outputfile, "w");
    if (data != NULL)
      break;
    else {
      printf(" Bad output file.\n");
      tty = 1;
    }
  }
  writebitmap(data, outputfile, formatw);
  fclose(data);
}


/*
*******************************************************************************
** Graphic procedures
*******************************************************************************
*/

legalize(x, y)
int *x, *y;
{
  if (*x < 0)
    *x = 0;
  else if (*x >= chartx)
    *x = chartx-1;
  if (*y < 0)
    *y = 0;
  else if (*y >= charty)
    *y = charty-1;
}

void drawpoint(x, y, o)
int x, y;
bit o;
{
  legalize(&x, &y);
  if (xbitmap)
    P(bm, x, y) = o;
  else
    XDrawPoint(disp, pixmap, gc, x, y);
}

void block(x1, y1, x2, y2, o)
int x1, y1, x2, y2;
bit o;
{
  int x, y;
  legalize(&x1, &y1); legalize(&x2, &y2);
  sort(&y1, &y2);
  for (y = y1; y <= y2; y++)
    drawline(x1, y, x2, y, o, 0);
}

void rect(x1, y1, x2, y2, xsiz, ysiz, o)
int x1, y1, x2, y2, xsiz, ysiz;
bit o;
{
  block(x1, y1, x2, y1 + ysiz - 1, o);
  block(x1, y1 + ysiz, x1 + xsiz - 1, y2 - ysiz, o);
  block(x2 - xsiz + 1, y1 + ysiz, x2, y2 - ysiz, o);
  block(x1, y2 - ysiz + 1, x2, y2, o);
}

void drawline(x1, y1, x2, y2, o, skip)
int x1, y1, x2, y2, skip;
bit o;
{
  int x = x1, y = y1, xadd, yadd, xinc, yinc, xabs, yabs, i, j = 0, temp;
  if (!xbitmap) {
    Xcolor(o);
    if (skip <= 0) {
      XDrawLine(disp, pixmap, gc, x1, y1, x2, y2);
      return;
    }
  }
  if (skip < 0)
    skip = 0;
  if (x2 - x1 >= 0)
    xadd = 1;
  else
    xadd = 3;
  if (y2 - y1 >= 0)
    yadd = 2;
  else
    yadd = 4;
  xabs = abs(x2 - x1);
  yabs = abs(y2 - y1);
  if (xabs < yabs) {
    temp = xadd;
    xadd = yadd;
    yadd = temp;
    temp = xabs;
    xabs = yabs;
    yabs = temp;
  }
  xinc = xabs;
  yinc = xabs >> 1;
  if (!(xabs & 1) && xadd > 2)
    yinc--;
  for (i = 0; i <= xinc; i++) {
    if (j < 1)
      drawpoint(x, y, o);
    j = j < skip ? j+1 : 0;
    switch (xadd) {
    case 1: x++;
      break;
    case 2: y++;
      break;
    case 3: x--;
      break;
    case 4: y--;
    }
    yinc += yabs;
    if (yinc - xabs >= 0) {
      yinc -= xabs;
      switch (yadd) {
      case 1: x++;
	break;
      case 2: y++;
	break;
      case 3: x--;
	break;
      case 4: y--;
      }
    }
  }
}

clipnegative(x1, y1, x2, y2)
int *x1, *y1, *x2, *y2;
{
  *x1 -= *y1*(*x2-*x1)/(*y2-*y1);
  *y1 = 0;
}

clipgreater(x1, y1, x2, y2, s)
int *x1, *y1, *x2, *y2, s;
{
  *x1 += (s-*y1)*(*x2-*x1)/(*y2-*y1);
  *y1 = s;
}

void drawclip(x1, y1, x2, y2, o, skip)
int x1, y1, x2, y2, skip;
bit o;
{
  if (x1 < 0)
    clipnegative(&y1, &x1, &y2, &x2);
  if (x2 < 0)
    clipnegative(&y2, &x2, &y1, &x1);
  if (y1 < 0)
    clipnegative(&x1, &y1, &x2, &y2);
  if (y2 < 0)
    clipnegative(&x2, &y2, &x1, &y1);
  if (x1 > chartx)
    clipgreater(&y1, &x1, &y2, &x2, chartx);
  if (x2 > chartx)
    clipgreater(&y2, &x2, &y1, &x1, chartx);
  if (y1 > charty)
    clipgreater(&x1, &y1, &x2, &y2, charty);
  if (y2 > charty)
    clipgreater(&x2, &y2, &x1, &y1, charty);
  drawline(x1, y1, x2, y2, o, skip);
}

void turtle(lin, x0, y0, o)
char *lin;
int x0, y0;
bit o;
{
  int i, j, x, y, deltax, deltay, blank = 0, noupdate = 0;
  char cmd;
  turtlex = x0; turtley = y0;
  if (!xbitmap)
    Xcolor(o);
  while (TRUE) {
    cmd = CAP(*lin);
    lin++;
    switch (cmd) {
    case 0:
      return;
    case 'B':
      blank = 1; break;
    case 'N':
      noupdate = 1; break;
    case 'U': case 'D': case 'L': case 'R':
    case 'E': case 'F': case 'G': case 'H':
      switch (cmd) {
      case 'U': deltax =  0; deltay = -1; break;
      case 'D': deltax =  0; deltay =  1; break;
      case 'L': deltax = -1; deltay =  0; break;
      case 'R': deltax =  1; deltay =  0; break;
      case 'E': deltax =  1; deltay = -1; break;
      case 'F': deltax =  1; deltay =  1; break;
      case 'G': deltax = -1; deltay =  1; break;
      case 'H': deltax = -1; deltay = -1; break;
      default: ;
      }
      x = turtlex;
      y = turtley;
      j = intinstring(&lin)*scale/100;
      if (blank) {
	turtlex += deltax*j;
	turtley += deltay*j;
      } else {
	drawpoint(turtlex, turtley, o);
	for (i = 0; i < j; i++) {
	  turtlex += deltax;
	  turtley += deltay;
	  drawpoint(turtlex, turtley, o);
	}
      }
      if (noupdate) {
	turtlex = x;
	turtley = y;
      }
      break;
    default:
      fprintf(stderr, "Bad turtle subcommand: %c\n", cmd);
    }
    if (cmd != 'B' && cmd != 'N')
      blank = noupdate = 0;
  }
}

#define INTTOHEX(I) (char) ((I) < 10 ? '0' + (I) : 'a' + (I) - 10)

writebitmap(data, name, mode)
FILE *data;
char *name, mode;
{
  char line[STRING];
  int x, y, i, value, temp = 0;

  fprintf(data, "#define %s_width %d\n" , name, chartx);
  fprintf(data, "#define %s_height %d\n", name, charty);
  fprintf(data, "static %s %s_bits[] = {",
	  mode != 'S' ? "char" : "short", name);
  for (y = 0; y < charty; y++) {
    x = 0;
    do {
      if (y + x > 0)
        fprintf(data, ",");
      if (temp == 0)
      	fprintf(data, "\n%s",
		mode == 'B' ? "  " : (mode == 'C' ? " " : ""));
      value = 0;
      for (i = (mode != 'S' ? 7 : 15); i >= 0; i--)
        value = (value << 1) + (!P(bm, x + i, y) && x + i < chartx);
      if (mode == 'B')
	putc(' ', data);
      fprintf(data, "0x");
      if (mode == 'S')
        fprintf(data, "%c%c",
		INTTOHEX(value >> 12), INTTOHEX((value >> 8) & 15));
      fprintf(data, "%c%c",
	      INTTOHEX((value >> 4) & 15), INTTOHEX(value & 15));
      temp++;
      if ((mode == 'B' && temp >= 12) ||
	  (mode == 'C' && temp >= 15) ||
	  (mode == 'S' && temp >= 11))
      	temp = 0;
      x += (mode != 'S' ? 8 : 16);
    } while (x < chartx);
  }
  fprintf(data, "};\n");
}


/*
*******************************************************************************
** X routines
*******************************************************************************
*/

color Xmakecolor(name)
char *name;
{
  XColor col;
  XParseColor(disp, cmap, name, &col);
  XAllocColor(disp, cmap, &col);
  return col.pixel;
}

Xcolorinit()
{
  int i, j;
  for (i = 1; i <= 7; i++)
    maincolor[i-1] = rainbowcolor[i] = !xreverse;
  maincolor[0] = rainbowcolor[0] = xreverse;
  if (!xbitmap) {
    cmap = XDefaultColormap(disp, screen);
    for (i = 0; i <= 6; i++)
      maincolor[i] = Xmakecolor(maincolorname[xcolor ?
	(i > 1 ? i : (xreverse ? 1-i : i)) : (xreverse ? i < 1 : i > 0)]);
    for (i = 1; i <= 7; i++)
      rainbowcolor[i] = xcolor ?
	Xmakecolor(rainbowcolorname[i]) : maincolor[1];
  }
  off = maincolor[0]; on = maincolor[1];
  hilite = maincolor[2]; gray = maincolor[3];
  for (i = 0; i < 4; i++)
    elemcolor[i] = (j = elemcolorindex[i]) < 0 ?
      rainbowcolor[-j] : maincolor[j];
  for (i = 0; i <= ASPECTS; i++)
    aspectcolor[i] = (j = aspectcolorindex[i]) < 0 ?
      rainbowcolor[-j] : maincolor[j];
  objectcolor[0] = elemcolor[1];
  for (i = 1; i <= 10; i++)
    objectcolor[i] = elemcolor[(ruler1[i]-1)%4];
  for (i = 11; i <= 15; i++)
    objectcolor[i] = hilite;
  for (i = 16; i <= 20; i++)
    objectcolor[i] = rainbowcolor[7];
  objectcolor[18] = elemcolor[1]; objectcolor[19] = elemcolor[0];
  objectcolor[21] = elemcolor[2]; objectcolor[22] = elemcolor[3];
  objectcolor[23] = elemcolor[1]; objectcolor[24] = elemcolor[2];
  for (i = objects+5; i <= objects+12; i++)
    objectcolor[i] = maincolor[5];
  for (i = BASE+1; i <= total; i++) {
    j = i-BASE;
    objectcolor[i] = (starbright[j] < 0.0 ? rainbowcolor[3] :
		      (starbright[j] < 1.0 ? hilite :
		       (starbright[j] < 2.0 && starbright[j] != 1.70 ?
			rainbowcolor[2] : rainbowcolor[1])));
  }
  if (!xbitmap) {
    XSetBackground(disp, gc, off);
    XSetForeground(disp, pmgc, off);
    Xcolor(on);
  }
}

Xbegin()
{
  disp = XOpenDisplay(0);
  if (!disp) {
    printf("Can't open display.\n");
    exit(1);
  }
  screen = DefaultScreen(disp);
  bg = BlackPixel(disp, screen);
  fg = WhitePixel(disp, screen);
  hint.x = hint.width = chartx; hint.y = hint.height = charty;
  hint.min_width = BITMAPX1; hint.min_height = BITMAPY1;
  hint.max_width = BITMAPX;  hint.max_height = BITMAPY;
  hint.flags = PPosition | PSize | PMaxSize | PMinSize;
  /*wmhint = XGetWMHints(disp, window);
  wmhint->input = True;
  XSetWMHints(disp, window, wmhint);*/
  depth = DefaultDepth(disp, screen);
  if (depth < 5) {
    xmono = TRUE;
    xcolor = FALSE;
  }
  root = RootWindow(disp, screen);
  if (xroot)
    window = root;
  else
    window = XCreateSimpleWindow(disp, DefaultRootWindow(disp),
      hint.x, hint.y, hint.width, hint.height, 5, fg, bg);
  pixmap = XCreatePixmap(disp, window, chartx, charty, depth);
  icon = XCreateBitmapFromData(disp, DefaultRootWindow(disp),
    icon_bits, icon_width, icon_height);
  if (!xroot)
    XSetStandardProperties(disp, window, "Astrolog", "Astrolog",
      icon, xkey, 0, &hint);
  gc = XCreateGC(disp, window, 0, 0);
  XSetGraphicsExposures(disp, gc, 0);
  font = XLoadFont(disp, FONT);
  XSetFont(disp, gc, font);
  pmgc = XCreateGC(disp, window, 0, 0);
  Xcolorinit();
  if (!xroot)
    XSelectInput(disp, window, KeyPressMask | StructureNotifyMask |
      ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask);
  XMapRaised(disp, window);
  XSync(disp, 0);
  XFillRectangle(disp, pixmap, pmgc, 0, 0, chartx, charty);
}

addtime(mod, vec)
int mod, vec;
{
  float toadd, h, m, j;
  toadd = vec;
  h = floor(F);
  m = (F-floor(F))*100.0;
  if (mod == 1)
    m += 1.0/60.0*(double)toadd;
  else if (mod == 2)
    m += 1.0*(double)toadd;
  if (m < 0.0 || m >= 60.0 || mod == 3) {
    if (m >= 60.0) {
      m -= 60.0; toadd = sgn(toadd);
    } else if (m < 0.0) {
      m += 60.0; toadd = sgn(toadd);
    }
    h += 1.0*(double)toadd;
  }
  if (h >= 24.0 || h < 0.0 || mod == 4) {
    if (h >= 24.0) {
      h -= 24.0; toadd = sgn(toadd);
    } else if (h < 0.0) {
      h += 24.0; toadd = sgn(toadd);
    }
    D += 1.0*(double)toadd;
  }
  if (D > (j = (double) dayinmonth((int) M, (int) Y)) ||
      D < 1.0 || mod == 5) {
    if (D > j) {
      D -= j; toadd = sgn(toadd);
    } else if (D < 1.0) {
      D += j; toadd = sgn(toadd);
    }
    M += 1.0*(double)toadd;
  }
  if (M > 12.0 || M < 1.0 || mod == 6) {
    if (M > 12.0) {
      M -= 12.0; toadd = sgn(toadd);
    } else if (M < 1.0) {
      M += 12.0; toadd = sgn(toadd);
    }
    Y += 1.0*(double)toadd;
  }
  if (mod == 7)
    Y += 10.0*(double)toadd;
  else if (mod == 8)
    Y += 100.0*(double)toadd;
  else if (mod == 9)
    Y += 1000.0*(double)toadd;
  F = h+m/100.0;
}

displaykeys()
{
  printf("\nAstrolog window keypress options (version %s):\n", VERSION);
  printf(" Press 'H' to display this list of key options.\n");
  printf(" Press 'p' to toggle pause status on or off.\n");
  printf(" Press 'x' to toggle fg/bg colors in window.\n");
  printf(" Press 'm' to toggle color/monochrome display in window.\n");
  printf(" Press 'T' to toggle header info on current chart in window.\n");
  printf(" Press 'i' to toggle status of the minor chart modification.\n");
  printf(" Press 'l' to toggle labelling of object points in chart.\n");
  printf(" Press 'v' to display current chart in window on text screen.\n");
  printf(" Press 'R' to toggle restriction status of minor objects.\n");
  printf(" Press 'C' to toggle restriction status of minor house cusps.\n");
  printf(" Press 'u' to toggle restriction status of uranian planets.\n");
  printf(" Press 'U' to toggle restriction status of fixed stars.\n");
  printf(" Press 's', 'h', 'f', 'F' to toggle status of siderial zodiac,\n");
  printf("        heliocentric charts, domal charts, and decan charts.\n");
  printf(" Press 'O' and 'o' to recall/store a previous chart from memory.\n");
  printf(" Press 'B' to dump current window contents to root background.\n");
  printf(" Press 'Q' to resize wheel chart display to a square.\n");
  printf(" Press '<' and '>' to decrease/increase the scale size of the\n");
  printf("       glyphs and the size of world map.\n");
  printf(" Press '[' and ']' to decrease/increase tilt in globe display.\n");
  printf(" Press 'N' to toggle animation status on or off. Charts will\n");
  printf("       be updated to current status and globe will rotate.\n");
  printf(" Press '!'-'(' to begin updating current chart by adding times.\n");
  printf("       !: seconds, @: minutes, #: hours, $: days, %: months,\n");
  printf("       ^: years, &: years*10, *: years*100, (: years*1000.\n");
  printf(" Press 'r' to reverse direction of time-lapse or animation.\n");
  printf(" Press '1'-'9' to set rate of animation to 'n' degrees, etc.\n");
  printf(" Press 'V','L','A','Z','S','W','G','P' to switch to normal (-v),\n");
  printf("       astrograph (-L), grid (-g), local (-Z), space (-S),\n");
  printf("       world (-XW), globe (-XG), and polar (-XP) modes.\n");
  printf(" Press 'q' to terminate the window and program.\n\n");
  printf(" Left   mouse button: Draw lines on chart in window.\n");
  printf(" Middle mouse button: Print coordinates of pointer on world map.\n");
  printf(" Right  mouse button: Terminate the window and program.\n");
}

Xspin()
{
  int xbreak = FALSE, xpause = FALSE, xupdate = FALSE, xevent = TRUE,
    xnew = FALSE, buttonx = 0, buttony = 0, dir = 1, length, i, j, k;
  double lon, lat;
  while (!xbreak) {
    if (chartx < BITMAPX1) {
      chartx = BITMAPX1;
      xupdate = TRUE;
    } else if (chartx > BITMAPX) {
      chartx = BITMAPX;
      xupdate = TRUE;
    }
    if (charty < BITMAPY1) {
      charty = BITMAPY1;
      xupdate = TRUE;
    } else if (charty > BITMAPY) {
      charty = BITMAPY;
      xupdate = TRUE;
    }
    if (modex == 'l' || modex == 'w') {
      length = 360*scale/100+2;
      if (chartx != length) {
	chartx = length;
	xupdate = TRUE;
      }
      length = (90*2+1)*scale/100+2;
      if (charty != length) {
	charty = length;
	xupdate = TRUE;
      }
    } else if (modex == 'a') {
      if (chartx != (length = (objects+(relation==1))*CELLSIZE*scale/100+1)) {
	chartx = length;
	xupdate = TRUE;
      } if (charty != length) {
	charty = length;
	xupdate = TRUE;
      }
    }
    if (xupdate) {
      xupdate = FALSE;
      XResizeWindow(disp, window, chartx, charty);
      XFreePixmap(disp, pixmap);
      pixmap = XCreatePixmap(disp, window, chartx, charty, depth);
      xevent = TRUE;
    }
    if (xnew && (!xnow || xpause)) {
      xnew = FALSE;
      M = Mon; D = Day; Y = Yea; F = Tim; X = Zon; L5 = Lon; LA = Lat;
      castchart(TRUE);
      xevent = TRUE;
    }
    if (xnow && !xpause && modex != 'w')
      xevent = TRUE;
    if (xevent) {
      xevent = FALSE;
      if (xnow)
	if (modex == 'g' || modex == 'p') {
	  degree += dir;
	  if (degree >= 360)
	    degree -= 360;
	  else if (degree < 0)
	    degree += 360;
	} else if (modex != 'w') {
	  if (xnow == 10)
#ifdef TIME
	    inputdata("now")
#endif
	      ;
	  else {
	    M = Mon; D = Day; Y = Yea; F = Tim; X = Zon; L5 = Lon; LA = Lat;
	    addtime(xnow, dir);
	  }
	  Mon = M; Day = D; Yea = Y; Tim = F; Zon = X; Lon = L5; Lat = LA;
	  castchart(TRUE);
	  if (relation) {
	    for (i = 1; i <= total; i++)
	      planet2[i] = planet[i];
	    for (i = 1; i <= SIGNS; i++)
	      house2[i] = house[i];
	  }
	}
      Xcolor(on);
      XFillRectangle(disp, pixmap, pmgc, 0, 0, chartx, charty);
      xchart();
      XSync(disp, 0);
      XCopyArea(disp, pixmap, window, gc, 0, 0, chartx, charty, 0, 0);
    }
    if (XEventsQueued(disp, QueuedAfterFlush)) { /*QueuedAfterReading)) {*/
      XNextEvent(disp, &event);
      if (event.type == Expose && event.xexpose.count == 0) {
	XSync(disp, 0);
	XCopyArea(disp, pixmap, window, gc, 0, 0, chartx, charty, 0, 0);
      }
      switch (event.type) {
      case ConfigureNotify:
	chartx = event.xconfigure.width;
	charty = event.xconfigure.height;
	XFreePixmap(disp, pixmap);
	pixmap = XCreatePixmap(disp, window, chartx, charty, depth);
	xevent = TRUE;
	break;
      case MappingNotify:
	XRefreshKeyboardMapping(&event);
	break;
      case ButtonPress:
	buttonx = event.xbutton.x; buttony = event.xbutton.y;
	if (event.xbutton.button == Button1) {
	  Xcolor(hilite);
	  drawpoint(buttonx, buttony, hilite);
	  XSync(disp, 0);
	  XCopyArea(disp, pixmap, window, gc, 0, 0, chartx, charty, 0, 0);
	} else if (event.xbutton.button == Button2 &&
		   (modex == 'l' || modex == 'w')) {
	  lon = 180.0-(double)(event.xbutton.x-1)/(double)(chartx-2)*DEGREES;
	  lat =  90.0-(double)(event.xbutton.y-1)/(double)(charty-2)*181.0;
	  printf("Mouse is at %s.\n", stringlocation(lon, lat, 60.0));
	} else if (event.xbutton.button == Button3)
	  xbreak = TRUE;
	break;
      case MotionNotify:
	Xcolor(hilite);
	drawline(buttonx, buttony, event.xbutton.x, event.xbutton.y,
		 hilite, 0);
	XSync(disp, 0);
	XCopyArea(disp, pixmap, window, gc, 0, 0, chartx, charty, 0, 0);
	buttonx = event.xbutton.x; buttony = event.xbutton.y;
      case KeyPress:
	length = XLookupString(&event, xkey, 10, &key, 0);
	if (length == 1) {
	  i = xkey[0];
	  switch (i) {
	  case 'p':
	    xpause = 1-xpause;
	    break;
	  case 'r':
	    dir = -dir;
	    break;
	  case 'x':
	    xreverse = 1-xreverse;
	    Xcolorinit();
	    xevent = TRUE;
	    break;
	  case 'm':
	    if (!xmono) {
	      xcolor = 1-xcolor;
	      Xcolorinit();
	      xevent = TRUE;
	    }
	    break;
	  case 'B':
	    XSetWindowBackgroundPixmap(disp, root, pixmap);
	    XClearWindow(disp, root);
	    break;
	  case 'T':
	    xtext = 1-xtext;
	    xevent = TRUE;
	    break;
	  case 'i':
	    bonus = 1-bonus;
	    xevent = TRUE;
	    break;
	  case '<':
	    if (scale > 100) {
	      scale -= 100;
	      xupdate = TRUE;
	    }
	    break;
	  case '>':
	    if (scale < 300) {
	      scale += 100;
	      xupdate = TRUE;
	    }
	    break;
	  case '[':
	    if (modex == 'g') {
	      tilt = tilt > -90.0 ? tilt-11.25 : -90.0;
	      xevent = TRUE;
	    }
	    break;
	  case ']':
	    if (modex == 'g') {
	      tilt = tilt < 90.0 ? tilt+11.25 : 90.0;
	      xevent = TRUE;
	    }
	    break;
	  case 'Q':
	    if (chartx > charty)
	      chartx = charty;
	    else
	      charty = chartx;
	    xupdate = TRUE;
	    break;
	  case 'R':
	    for (i = 11; i <= 15; i++)
	      ignore[i] = 1-ignore[i];
	    ignore[17] = 1-ignore[17]; ignore[20] = 1-ignore[20];
	    xevent = TRUE;
	    break;
	  case 'C':
	    cusp = 1-cusp;
	    for (i = objects+1; i <= objects+4; i++)
	      ignore[i] = 1-ignore[i];
	    xnew = TRUE;
	    break;
	  case 'u':
	    uranian = 1-uranian;
	    for (i = objects+5; i <= BASE; i++)
	      ignore[i] = 1-ignore[i];
	    xnew = TRUE;
	    break;
	  case 'U':
	    universe = universe ? 0 : 1;
	    for (i = BASE+1; i <= total; i++)
	      ignore[i] = 1-ignore[i];
	    xnew = TRUE;
	    break;
	  case 's':
	    siderial = 1-siderial;
	    xnew = TRUE;
	    break;
	  case 'h':
	    centerplanet = centerplanet ? 0 : 1;
	    xnew = TRUE;
	    break;
	  case 'f':
	    flip = 1-flip;
	    xnew = TRUE;
	    break;
	  case 'F':
	    decan = 1-decan;
	    xnew = TRUE;
	    break;
	  case 'o':
	    STORE1;
	    break;
	  case 'O':
	    RECALL1;
	    xnew = TRUE;
	    break;
	  case 'l':
	    label = 1-label;
	    xevent = TRUE;
	    break;
	  case 'N':
	    xnow = xnow ? 0 : 10;
	    break;
	  case '!': xnow = 1; break;
	  case '@': xnow = 2; break;
	  case '#': xnow = 3; break;
	  case '$': xnow = 4; break;
	  case'\%': xnow = 5; break;
	  case '^': xnow = 6; break;
	  case '&': xnow = 7; break;
	  case '*': xnow = 8; break;
	  case '(': xnow = 9; break;
	  case 'V': modex = 'c'; xevent = TRUE; break;
	  case 'L': modex = 'l'; xevent = TRUE; break;
	  case 'A': modex = 'a'; xevent = TRUE; break;
	  case 'Z': modex = 'z'; xevent = TRUE; break;
	  case 'S': modex = 's'; xevent = TRUE; break;
	  case 'W': modex = 'w'; xevent = TRUE; break;
	  case 'P': modex = 'p'; xevent = TRUE; break;
	  case 'G': modex = 'g'; xevent = TRUE; break;
	  case 'v':
	    chartlocation();
	    break;
	  case 'H': case '?':
	    displaykeys();
	    break;
	  case 'q':
	    xbreak = TRUE;
	    break;
	  default:
	    if (i > '0' && i <= '9')
	      dir = (dir > 0 ? 1 : -1)*(i-'0');
	  }
	}
      default: ;
      }
    }
  }
}

Xend()
{
  XFreeGC(disp, gc);
  XFreeGC(disp, pmgc);
  XFreePixmap(disp, pixmap);
  XDestroyWindow(disp, window);
  XCloseDisplay(disp);
}


/*
*******************************************************************************
** Main processing
*******************************************************************************
*/

void xdisplayswitches()
{
  printf("\nSwitches to access X window options:\n");
  printf(" -X: Create a graphics chart instead of displaying it as text.\n");
  printf(" -Xb: Create X bitmap instead of putting graphics in window.\n");
  printf(" -XB: Display X chart on root instead of in a separate window.\n");
  printf(" -Xm: For color monitors, display charts in black and white.\n");
  printf(" -Xr: Create window or bitmap in reversed colors (B on W).\n");
  printf(" -Xw <hor> [<ver>]: Change the size of the chart window.\n");
  printf(" -Xs <percentage>: Change the size of characters by n%%.\n");
  printf(" -Xi: Display chart in slightly modified form.\n");
  printf(" -XT: Inhibit display of chart info at bottom of window.\n");
  printf(" -XW: Simply create the bitmap of the world.\n");
  printf(" -XP: Create the bitmap of the world, but as a polar projection.\n");
  printf(" -XG [<degrees>]: Display the bitmap of the world as a globe.\n");
  printf(" -Xn: Start up chart or globe display in animation mode.\n");
  printf(" -Xo <file>: Write output bitmap to specified file.\n");
  printf("Also, press 'H' within X window for list of key press options.\n");
}

int xprocess(argc, argv, pos)
int argc, pos;
char **argv;
{
  int i = 0, j;
  switch (argv[0][pos]) {
  case 0:
    break;
  case 'W':
    modex = 'w';
    autom = TRUE;
    break;
  case 'P':
    modex = 'p';
    autom = TRUE;
    break;
  case 'G':
    if (argc > 1 && ((degree = atoi(argv[1])) || argv[1][0] == '0')) {
      i++;
      if (degree < 0 || degree > 359) {
	fprintf(stderr, "Value %d passed to switch -XG out of range.\n",
		degree);
	exit(1);
      }
    } else
      degree = 0;
    modex = 'g';
    autom = TRUE;
    break;
  case 'b':
    xbitmap = TRUE;
    xcolor = FALSE;
    break;
  case 'B':
    xroot = TRUE;
    break;
  case 'm':
    xcolor = FALSE;
    break;
  case 'r':
    xreverse = TRUE;
    break;
  case 'w':
    if (argc <= 1)
      toofew("Xw");
    chartx = atoi(argv[1]);
    if (argc > 2 && (charty = atoi(argv[2]))) {
      argc--; argv++;
      i++;
    } else
      charty = chartx;
    if (chartx < BITMAPY1 || chartx > BITMAPX ||
	charty < BITMAPY1 || charty > BITMAPY) {
      fprintf(stderr, "Values %d,%d passed to switch -Xw out of range.\n",
	      chartx, charty);
      exit(1);
    }
    i++;
    break;
  case 's':
    if (argc <= 1)
      toofew("Xs");
    scale = atoi(argv[1]);
    if (scale < 100 || scale > 300 || scale%100 != 0) {
      fprintf(stderr, "Value %d passed to switch -Xs out of range.\n", scale);
      exit(1);
    }
    i++;
    break;
  case 'i':
    bonus = TRUE;
    break;
  case 'T':
    xtext = FALSE;
    break;
  case 'n':
    if (argc > 1 && (xnow = atoi(argv[1])))
      i++;
    else
      xnow = 10;
    if (xnow < 1 || xnow > 10) {
      fprintf(stderr, "Value %d passed to switch -Xn out of range.\n", xnow);
      exit(1);
    }
    break;
  case 'o':
    if (argc <= 1)
      toofew("Xo");
    xbitmap = TRUE;
    xcolor = FALSE;
    for (j = 0; (outputfile[j] = argv[1][j]) != 0; j++)
      ;
    i++;
    break;
  default:
    fprintf(stderr, "Unknown switch -X%c\n", argv[0][2]);
    exit(1);
  }
  return i;
}

void xaction()
{
  if (modex == 'c' && (todisplay & 64) > 0)
    modex = 'l';
  else if (modex == 'c' && (todisplay & 4) > 0) {
    modex = 'a';
    chartx = charty = (objects+(relation==1))*CELLSIZE*scale/100+1;
  } else if (modex == 'c' && (todisplay & 8) > 0) {
    modex = 'z';
    chartx = (360+12)*scale/100;
    charty = (90*2+12)*scale/100;
  } else if (modex == 'c' && (todisplay & 16) > 0)
    modex = 's';
  if (modex == 'l' || modex == 'w') {
    chartx = 360*scale/100+2;
    charty = (90*2+1)*scale/100+2;
  }
  if (xbitmap) {
    NEW(bm);
    Xcolorinit();
  } else
    Xbegin();
  if (xroot || xbitmap)
    xchart();
  if (xbitmap)
    writefile();
  else {
    if (xroot) {
      XSetWindowBackgroundPixmap(disp, root, pixmap);
      XClearWindow(disp, root);
    } else {
      STORE1;
      Xspin();
    }
    Xend();
  }
}
#endif

/**/

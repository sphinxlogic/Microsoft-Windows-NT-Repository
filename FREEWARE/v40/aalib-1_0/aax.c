#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "config.h"
#ifdef X11_DRIVER
#include <X11/Xlib.h>
#include "aalib.h"
struct aa_driver X11_d;
#define dp __X_display
#define wi __X_window
#define pi __X_pixmap
#define attr __X_attr
#define screen __X_screen
#define dr (pixmapmode?pi:wi)
#define fontwidth __X_fontwidth
#define fontheigth __X_fontheight
Display *dp = NULL;
Window wi;
Pixmap pi;
XSetWindowAttributes attr;
int fontheight;
int fontwidth = 8;
static GC normalGC, dimGC, boldGC, currGC, specialGC;
static int cvisible = 1;
static GC blackGC;
static Colormap cmap;
static int screen;
static long bold, normal, dim, black, special;
static int width = 80;
static int height = 32;
static XFontStruct *font_s;
static int Xpos = 0;
static int pixmapmode;
static int Ypos = 0;
static int mmwidth, mmheight;
static unsigned char *previoust = NULL, *previousa = NULL;
static int font;
static int pixelwidth, pixelheight;
#define C2 0x68
#define C1 0xB2

static int X_init(struct aa_hardware_params *p, void *none)
{
    static int registered = 0;
    static aa_font aafont;
    static XColor c;
    c.red = C1 * 256;
    c.green = C1 * 256;
    c.blue = C1 * 256;
    if ((dp = XOpenDisplay(NULL)) == NULL)
	return 0;
    screen = DefaultScreen(dp);
    font = XLoadFont(dp, "8x13bold");
    /*font = XLoadFont(dp, "-adobe-courier-medium-r-*-*-13-*-*-*-*-*-*-*"); */
    /*font = XLoadFont(dp, "vga"); */
    if (!font) {
	XCloseDisplay(dp);
	return 0;
    }
    font_s = XQueryFont(dp, font);
    if (!font_s) {
	XCloseDisplay(dp);
	return 0;
    }
    fontheight = font_s->max_bounds.ascent + font_s->max_bounds.descent;
    cmap = DefaultColormap(dp, screen);
    black = attr.border_pixel = attr.background_pixel = BlackPixel(dp, screen);
    bold = WhitePixel(dp, screen);
    normal = XAllocColor(dp, cmap, &c);
    /*c.flags=DoRed | DoGreen | DoBlue; */
    c.red = C2 * 256;
    c.green = C2 * 256;
    c.blue = C2 * 256;
    if (!normal) {
	normal = bold;
	X11_d.params.supported &= ~AA_BOLD_MASK;
	X11_d.params.supported &= ~AA_DIM_MASK;
    }
    normal = c.pixel;
    dim = XAllocColor(dp, cmap, &c);
    if (!dim) {
	dim = bold;
	X11_d.params.supported &= ~AA_DIM_MASK;
    }
    dim = c.pixel;
    c.red = 0;
    c.green = 0;
    c.blue = 65535UL;
    special = XAllocColor(dp, cmap, &c);
    if (!special) {
	special = black;
    }
    special = c.pixel;
    attr.event_mask = ExposureMask;
    attr.override_redirect = False;
    if (p->width)
	width = p->width;
    else {
	char c[256];
	width = X11_d.params.recwidth;
	if (p->recwidth)
	    width = p->recwidth;
	printf("Width?[%i]", width);
	gets(c);
	sscanf(c, "%i", &width);
    }
    if (p->height)
	height = p->height;
    else {
	char c[256];
	height = X11_d.params.recheight;
	if (p->recheight)
	    height = p->recheight;
	printf("Height?[%i]", height);
	gets(c);
	sscanf(c, "%i", &height);
    }
    if (p->maxwidth && width > p->maxwidth)
	width = p->maxwidth;
    if (p->minwidth && width < p->minwidth)
	width = p->minwidth;
    if (p->maxheight && height > p->maxheight)
	height = p->maxheight;
    if (p->minheight && height < p->minheight)
	height = p->minheight;
    wi = XCreateWindow(dp, RootWindow(dp, screen), 0, 0, width * fontwidth, height * fontheight, 0, DefaultDepth(dp, screen), InputOutput, DefaultVisual(dp, screen), CWBackPixel | CWBorderPixel | CWEventMask, &attr);
    if (!registered) {
	pi = XCreatePixmap(dp, wi, 8, fontheight * 256, 1);
	if (pi) {
	    int i;
	    unsigned char c;
	    unsigned char *data;
	    XImage *image;
	    registered = 1;
	    specialGC = XCreateGC(dp, pi, 0L, NULL);
	    XSetForeground(dp, specialGC, 0);
	    XSetBackground(dp, specialGC, 0);
	    XFillRectangle(dp, pi, specialGC, 0, 0, 8, 256 * fontheight);
	    XSetForeground(dp, specialGC, 1);
	    XSetFont(dp, specialGC, font);
	    for (i = 0; i < 256; i++) {
		c = i;
		XDrawString(dp, pi, specialGC, 0, (i + 1) * fontheight - font_s->descent, &c, 1);
	    }
	    image = XGetImage(dp, pi, 0, 0, 8, 256 * fontheight, 1, XYPixmap);
	    if (image != NULL) {
		data = malloc(256 * fontheight);
		for (i = 0; i < 256; i++) {
		    int y;
		    for (y = 0; y < fontheight; y++) {
			int o;
			o = ((XGetPixel(image, 0, i * fontheight + y) != 0) << 7) +
			    ((XGetPixel(image, 1, i * fontheight + y) != 0) << 6) +
			    ((XGetPixel(image, 2, i * fontheight + y) != 0) << 5) +
			    ((XGetPixel(image, 3, i * fontheight + y) != 0) << 4) +
			    ((XGetPixel(image, 4, i * fontheight + y) != 0) << 3) +
			    ((XGetPixel(image, 5, i * fontheight + y) != 0) << 2) +
			    ((XGetPixel(image, 6, i * fontheight + y) != 0) << 1) +
			    ((XGetPixel(image, 7, i * fontheight + y) != 0) << 0);
			data[i * fontheight + y] = o;
		    }
		}
		aafont.name = "Font used by X server";
		aafont.shortname = "current";
		aafont.height = fontheight;
		aafont.data = data;
		aa_registerfont(&aafont);
		X11_d.params.font = &aafont;
	    }
	}
    }
    XStoreName(dp, wi, "aa for X");
    XMapWindow(dp, wi);
    specialGC = XCreateGC(dp, wi, 0L, NULL);
    XSetForeground(dp, specialGC, special);
    XSetFont(dp, specialGC, font);
    normalGC = XCreateGC(dp, wi, 0L, NULL);
    XSetForeground(dp, normalGC, normal);
    XSetFont(dp, normalGC, font);
    boldGC = XCreateGC(dp, wi, 0L, NULL);
    XSetForeground(dp, boldGC, bold);
    XSetFont(dp, boldGC, font);
    dimGC = XCreateGC(dp, wi, 0L, NULL);
    XSetForeground(dp, dimGC, dim);
    XSetFont(dp, dimGC, font);
    blackGC = XCreateGC(dp, wi, 0L, NULL);
    XSetForeground(dp, blackGC, black);
    currGC = normalGC;
    mmwidth = DisplayWidthMM(dp, screen);
    mmheight = DisplayHeightMM(dp, screen);
    X11_d.params.mmwidth = mmwidth * width * fontwidth / DisplayWidth(dp, screen);
    X11_d.params.mmheight = mmheight * height * fontheight / DisplayHeight(dp, screen);
    pixelwidth = fontwidth * width;
    pixelheight = fontheight * height;
    XSync(dp, 0);
    aa_recommendlowkbd("X11");
    return 1;
}
int __X_getsize(void)
{
    int px, py;
    int tmp;
    Window wtmp;
    XSync(dp, 0);
    XGetGeometry(dp, wi, &wtmp, &tmp, &tmp, &px, &py, (unsigned int *) &tmp, (unsigned int *) &tmp);
    tmp = 0;
    if (px != pixelwidth || py != pixelheight)
	tmp = 1;
    pixelwidth = px;
    pixelheight = py;
    if (tmp) {
	if (previoust != NULL)
	    free(previoust), free(previousa), previoust = NULL, previousa = NULL;
	if (pixmapmode)
	    XFreePixmap(dp, pi), pixmapmode = 0;
    }
    if (!pixmapmode && !getenv("AABlink"))
	pi = XCreatePixmap(dp, wi, pixelwidth, pixelheight, DefaultDepth(dp, screen));
    if (!pi) {
	pixmapmode = 0;
	XSetWindowBackgroundPixmap(dp, wi, black);
    } else {
	pixmapmode = 1;
	XSetWindowBackgroundPixmap(dp, wi, pi);
	XFillRectangle(dp, pi, blackGC, 0, 0, pixelwidth, pixelheight);
    }
    X11_d.params.mmwidth = mmwidth * width * fontwidth / DisplayWidth(dp, screen);
    X11_d.params.mmheight = mmheight * height * fontheight / DisplayHeight(dp, screen);
    return (tmp);
}
static void X_uninit(aa_context * c)
{
    if (previoust != NULL)
	free(previoust), free(previousa);
    if (pixmapmode)
	XFreePixmap(dp, pi);
    XCloseDisplay(dp);
}
static void X_getsize(aa_context * c, int *width1, int *height1)
{
    __X_getsize();
    *width1 = width = pixelwidth / fontwidth;
    *height1 = height = pixelheight / fontheight;
}
static void X_setattr(int attr)
{
    switch (attr) {
    case 0:
    case 4:
	currGC = normalGC;
	break;
    case 1:
	currGC = dimGC;
	break;
    case 2:
	currGC = boldGC;
	break;
    case 3:
	currGC = blackGC;
	break;
    }
#if 0
    switch (attr) {
    case AA_NORMAL:
	currGC = normalGC;
	break;
    case AA_DIM:
	currGC = dimGC;
	break;
    case AA_BOLD:
	currGC = boldGC;
	break;
    case AA_BOLDFONT:
	currGC = normalGC;
	break;
    case AA_REVERSE:
	currGC = blackGC;
	break;
    case AA_SPECIAL:
	currGC = normalGC;
	break;
    }
#endif
}
/*quite complex but fast drawing routing for X */
#define NATT 5
#define texty(l,a,x) _texty[((l)*NATT+(a))*width+(x)]
#define rectangles(a,x) _rectangles[(a)*height*width+(x)]
static XTextItem *_texty;
static int (*nitem)[NATT];
static int (*startitem)[NATT];
static XRectangle *_rectangles;
static int nrectangles[3];
static int drawed;
static void alloctables()
{
    _texty = malloc(sizeof(XTextItem) * width * NATT * height);
    nitem = calloc(sizeof(*nitem) * height,1);
    startitem = calloc(sizeof(*startitem) * height,1);
    _rectangles = malloc(sizeof(*_rectangles) * width * height * NATT);
}
static void freetables()
{
    free(_texty);
    free(nitem);
    free(startitem);
    free(_rectangles);
}

static void MyDrawString(int attr, int x, int y, unsigned char *c, int i)
{
    XTextItem *it;
    XRectangle *rect;
    int n, a;
    switch (attr) {
    case AA_NORMAL:
    case AA_DIM:
    case AA_BOLD:
    case AA_BOLDFONT:
    default:
	n = 0;
	break;
    case AA_REVERSE:
	n = 1;
	break;
    case AA_SPECIAL:
	n = 2;
	break;
    }
    switch (attr) {
    default:
    case AA_SPECIAL:
    case AA_NORMAL:
	a = 0;
	break;
    case AA_DIM:
	a = 1;
	break;
    case AA_BOLD:
	a = 2;
	break;
    case AA_REVERSE:
	a = 3;
	break;
    case AA_BOLDFONT:
	a = 4;
	break;
    }
    it = &texty(y, a, nitem[y][a]);
    it->delta = x * fontwidth - startitem[y][a];
    if (!it->delta && x) {
	it--;
	it->nchars += i;
    } else {
	nitem[y][a]++;
	it->chars = c;
	it->nchars = i;
	it->font = font;
	drawed = 1;
    }
    startitem[y][a] = (x + i) * fontwidth;
    rect = &rectangles(n, nrectangles[n]);
    rect->x = x * fontwidth;
    rect->y = (y) * fontheight + 1;
    rect->width = i * fontwidth;
    if (nrectangles[n] && (rect - 1)->y == rect->y &&
	(rect - 1)->x + (rect - 1)->width == rect->x)
	nrectangles[n]--, (--rect)->width += i * fontwidth;
    rect->height = fontheight;
    nrectangles[n]++;
}
static int Black[] =
{0, 0, 0, 0, 1, 1};


static void X_flush(aa_context * c)
{
    int x, y, attr;
    int xs = 0, ys = 0;
    int l, same;
    int s = 0;
    int pos;
    attr = AA_NORMAL;
    alloctables();
    drawed = 0;
    nrectangles[0] = 0;
    nrectangles[1] = 0;
    nrectangles[2] = 0;
    if (previoust == NULL) {
	previoust = malloc(width * height);
	previousa = calloc(width * height, 1);
	memset(previoust, ' ', width * height);
    }
    for (y = 0; y < aa_scrheight(c); y++) {
	s = l = 0;
	xs = 0;
	ys = y;
	for (x = 0; x < aa_scrwidth(c); x++) {
	    pos = x + y * aa_scrwidth(c);
	    if (s > 5 || (c->attrbuffer[pos] != attr && (c->textbuffer[pos] != ' ' || Black[c->attrbuffer[pos]] || Black[attr]))) {
		if (l - s)
		    MyDrawString(attr, xs, ys,
			&c->textbuffer[xs + ys * aa_scrwidth(c)], l - s);
		attr = c->attrbuffer[pos];
		s = l = 0;
		xs = x;
		ys = y;
	    }
	    if ((previoust[pos] == c->textbuffer[pos] && previousa[pos] == c->attrbuffer[pos]) || (!Black[attr] && previoust[pos] == ' ' && c->textbuffer[pos] == ' ' && !Black[previousa[pos]]))
		same = 1;
	    else
		same = 0;
	    if (xs == x && same)
		xs++;
	    else {
		if (same)
		    s++;
		else
		    s = 0;
		l++;
	    }
	}
	if (l - s)
	    MyDrawString(attr, xs, ys,
			 &c->textbuffer[xs + ys * aa_scrwidth(c)], l - s);
    }
    if (drawed) {
	memcpy(previousa, c->attrbuffer, width * height);
	memcpy(previoust, c->textbuffer, width * height);
	/*printf("%i\n",nrectangles); */
	if (nrectangles[0])
	    XFillRectangles(dp, dr, blackGC, &rectangles(0, 0), nrectangles[0]);
	if (nrectangles[1])
	    XFillRectangles(dp, dr, normalGC, &rectangles(1, 0), nrectangles[1]);
	if (nrectangles[2])
	    XFillRectangles(dp, dr, specialGC, &rectangles(2, 0), nrectangles[2]);
	if (cvisible)
	    XDrawLine(dp, dr, normalGC, Xpos * fontwidth, (Ypos + 1) * fontheight - 1, (Xpos + 1) * fontwidth - 1, (Ypos + 1) * fontheight - 1);

	for (y = 0; y < height; y++) {
	    for (x = 0; x < NATT; x++) {
		if (nitem[y][x]) {
		    X_setattr(x);
		    XDrawText(dp, dr, currGC, 0, (y + 1) * fontheight - font_s->descent, &texty(y, x, 0), nitem[y][x]);
		    if (x == 4)
			XDrawText(dp, dr, currGC, 1, (y + 1) * fontheight - font_s->descent, &texty(y, x, 0), nitem[y][x]);
		}
	    }
	}
	if (pixmapmode) {
	    XClearWindow(dp, wi);
	}
	/*if(!pixmapmode) */
	XSync(dp, 0);
    }
    freetables();
}
static void X_gotoxy(aa_context * c, int x, int y)
{
    if (Xpos != x || Ypos != y) {
	if (previoust != NULL)
	    previoust[Ypos * width + Xpos] = 255;
	Xpos = x;
	Ypos = y;
	X_flush(c);
    }
}
static void X_cursor(aa_context * c, int mode)
{
    cvisible = mode;
}

struct aa_driver X11_d =
{
    "X11", "X11 driver 1.0",
    {&fontX13B, AA_DIM_MASK | AA_REVERSE_MASK | AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK | AA_EXTENDED,
     0, 0,
     0, 0,
     80, 32,
     0, 0},
    X_init,
    X_uninit,
    X_getsize,
    NULL,
    NULL,
    NULL,
    X_gotoxy,
    X_flush,
    X_cursor
};
#endif

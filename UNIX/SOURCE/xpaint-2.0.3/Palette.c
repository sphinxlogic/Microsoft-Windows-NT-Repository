/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, 1993, David Koblas (koblas@netcom.com)            | */
/* |                                                                   | */
/* | Permission to use, copy, modify, and to distribute this software  | */
/* | and its documentation for any purpose is hereby granted without   | */
/* | fee, provided that the above copyright notice appear in all       | */
/* | copies and that both that copyright notice and this permission    | */
/* | notice appear in supporting documentation.  There is no           | */
/* | representations about the suitability of this software for        | */
/* | any purpose.  this software is provided "as is" without express   | */
/* | or implied warranty.                                              | */
/* |                                                                   | */
/* +-------------------------------------------------------------------+ */

#include <X11/Intrinsic.h>
#include <stdio.h>
#include "Palette.h"
#include "Hash.h"

#define NO_LIST

typedef struct list_s {
	Display		*dpy;
	Colormap	cmap;
	Palette		*map;
	struct list_s	*next;
} PaletteList;

typedef struct Col_s {
	XColor		color;
	Boolean		used;
#ifndef NO_LIST
	struct Col_s	*next, *prev;
#endif
} Col;

#define HASH_SIZE 128
#define HASH(c)   	  ((int)((c).red + (c).green + (c).blue) % HASH_SIZE)
#define HASH_PIXEL(c)     (c % HASH_SIZE)

static PaletteList	*cmapList = NULL;

static int      readCMP(Col *cca, Col *ccb)
{
	XColor	*ca = &cca->color, *cb = &ccb->color;

	if (ca->red != cb->red)
		return ca->red < cb->red ? -1 : 1;
	if (ca->green != cb->green)
		return ca->green < cb->green ? -1 : 1;
	if (ca->blue != cb->blue)
		return ca->blue < cb->blue ? -1 : 1;
        return 0;
}

static int      cmpPixel(Col *cca, Col *ccb)
{
	if (cca->color.pixel == ccb->color.pixel)
		return 0;
	return (cca->color.pixel < ccb->color.pixel) ? -1 : 1;
}

static void	freeFunc(void *junk)
{
	/*
	**  Nop free func
	*/
}

static void	entryUnlink(Palette *map, Col *node)
{
#ifndef NO_LIST
	if (node->prev == NULL)
		map->list = node->next;
	else
		node->prev->next = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	node->next = NULL;
	node->prev = NULL;
#endif
	node->used = True;
	map->nfree--;
}


#define STEP	256

Palette *PaletteCreate(Widget w)
{
	int		i;
	Display		*dpy = XtDisplay(w);
	Screen		*screen = XtScreen(w);
	Colormap	rcmap = DefaultColormapOfScreen(screen);
	Palette		*map = XtNew(Palette);
	Col		*ctable, *prev = NULL;
	Visual		*visual = DefaultVisualOfScreen(screen);
	PaletteList	*new;
#if 0
	{
		int		depth = DefaultDepthOfScreen(screen);
		int		class = StaticColor;
		XVisualInfo	vinfo;

		if (XMatchVisualInfo(dpy, DefaultScreen(dpy), depth, class, &vinfo) != 0)
			visual = vinfo.visual;
	}
#endif

	map->htable  = NULL;
	map->ltable  = NULL;
	map->display = dpy;
	map->htable  = HashCreate(readCMP,  freeFunc, HASH_SIZE);
	map->ltable  = HashCreate(cmpPixel, freeFunc, HASH_SIZE);
	map->list    = NULL;
	map->last    = NULL;
	map->isMapped= visual->class != TrueColor;
	map->isGrey  = (visual->class == StaticGray || visual->class == GrayScale);
	map->ncolors = visual->map_entries;
	map->nfree   = 0;
	map->ctable  = NULL;

	switch (visual->class) {
	case StaticGray:
	case StaticColor:
	case TrueColor:
		map->readonly = True;
		map->cmap     = DefaultColormapOfScreen(screen);
		goto addlist;
	default:
		map->readonly = False;
		map->cmap = XCreateColormap(dpy, RootWindowOfScreen(screen), 
						visual, AllocAll);
		break;
	}

	ctable = (Col *)XtCalloc(sizeof(Col), visual->map_entries);
	map->ctable = ctable;
	for (i = 0; i < visual->map_entries; i += STEP) {
		XColor	xcol[STEP];
		int	cnt = visual->map_entries - i;
		int	j;

		if (cnt > STEP)
			cnt = STEP;

		for (j = 0; j < cnt; j++) {
			Col	*c = &ctable[i + j];

			c->color.pixel = i + j;
			c->color.flags = DoRed|DoGreen|DoBlue;
			xcol[j].pixel = i + j;
			xcol[j].flags = DoRed|DoGreen|DoBlue;

			c->used   = False;

#ifndef NO_LIST
			c->prev   = prev;
			if (prev == NULL)
				map->list = c;
			else
				prev->next = c;
			c->next   = NULL;
			prev      = c;
#endif
		}

		XQueryColors(dpy, rcmap, xcol, cnt);
		XStoreColors(dpy, map->cmap, xcol, cnt);

		for (j = 0; j < cnt; j++) {
			Col		*c = &ctable[i + j];

			c->color.red   = xcol[j].red & 0xff00;
			c->color.green = xcol[j].green & 0xff00;
			c->color.blue  = xcol[j].blue & 0xff00;
			HashAdd(map->htable, HASH(c->color), c);
			HashAdd(map->ltable, HASH_PIXEL(c->color.pixel), c);
		}
	}

	map->nfree = visual->map_entries;

	for (i = 0; i < visual->map_entries; i++) {
		if (ctable[i].color.pixel == BlackPixelOfScreen(screen))
			entryUnlink(map, &ctable[i]);
		if (ctable[i].color.pixel == WhitePixelOfScreen(screen))
			entryUnlink(map, &ctable[i]);
	}

addlist:
	new = (PaletteList*)XtMalloc(sizeof(PaletteList));
	new->dpy     = XtDisplay(w);
	new->cmap    = map->cmap;
	new->next    = cmapList;
	new->map     = map;
	cmapList     = new;

	return map;
}

static void addColor(Palette *map, XColor *color)
{
	Col	*node, *n;
	int	i;

	if (map->readonly) {
		/*
		**  The temporary color is needed since alloc
		**  will change the pixel values.
		*/
		XColor	newc;
		newc = *color;
		node = XtNew(Col);

		newc.flags = DoRed|DoGreen|DoBlue;
		XAllocColor(map->display, map->cmap, &newc);
		node->color.pixel = color->pixel = newc.pixel;
	} else {
		Col		*cptr = (Col*)map->ctable;
		int		d, curDif = -1;
		int		rd, gd, bd;

		/*
		**  Find the closest match in the existing color map.
		*/
		node = cptr;

		for (i = 0; i < map->ncolors; i++, cptr++) {
			if (map->nfree != 0 && cptr->used)
				continue;

			rd = (cptr->color.red >> 8)   - (color->red >> 8);
			gd = (cptr->color.green >> 8) - (color->green >> 8);
			bd = (cptr->color.blue >> 8)  - (color->blue >> 8);
			d  = rd * rd + gd * gd + bd * bd;

			if (d < curDif || curDif == -1) {
				node   = cptr;
				curDif = d;
			}
		}

		/*
		**  All free colors used up.
		*/
		if (map->nfree == 0) {
			color->pixel = node->color.pixel;
			return;
		}

		/*
		**  Get next free color.
		*/
		entryUnlink(map, node);

		n = HashFind(map->ltable, HASH_PIXEL(node->color.pixel), node);
		HashRemove(map->ltable, HASH_PIXEL(color->pixel), n);
		HashRemove(map->htable, HASH(*color), node);

		color->pixel = node->color.pixel;
	}

	node->used        = True;
	node->color.red   = color->red   & 0xff00;
	node->color.green = color->green & 0xff00;
	node->color.blue  = color->blue  & 0xff00;
	node->color.flags = DoRed|DoGreen|DoBlue;
	if (!map->readonly)
		XStoreColor(map->display, map->cmap, &node->color);
	HashAdd(map->htable, HASH(*color), node);
	HashAdd(map->ltable, HASH_PIXEL(color->pixel), node);
}

int PaletteAllocN(Palette *map, XColor *color, int ncolor, Pixel *list)
{
	char	*flg = XtCalloc(sizeof(char), ncolor);
	Col	c, *node;
	int	i;

	for (i = 0; i < ncolor; i++) {
		Col	c;

		c.color.red   = color[i].red   & 0xff00;
		c.color.green = color[i].green & 0xff00;
		c.color.blue  = color[i].blue  & 0xff00;

		if ((node = HashFind(map->htable, HASH(c.color), &c)) != NULL) {
			flg[i] = True;
			/*
			**  Match found, if the entry hasn't been "alloced"
			**   yet, mark it so, and remove it from the "free" list.
			*/
			if (!node->used) 
				entryUnlink(map, node);
			list[i] = node->color.pixel;
		} else {
			flg[i] = False;
		}
	}

	for (i = 0; i < ncolor; i++) {
		if (flg[i])
			continue;

		c.color.red   = color[i].red   & 0xff00;
		c.color.green = color[i].green & 0xff00;
		c.color.blue  = color[i].blue  & 0xff00;

		if ((node = HashFind(map->htable, HASH(c.color), &c)) == NULL) {
			addColor(map, &color[i]);
			list[i] = color[i].pixel;
		} else {
			/*
			**  It must have been allocated in the previous
			**   pass, or by the above.
			*/
			list[i] = node->color.pixel;
		}
	}

	XtFree((XtPointer)flg);

	return 0;
}

Pixel PaletteAlloc(Palette *map, XColor *color)
{
	Col	c, *node;

	if (map->last != NULL) {
		XColor	*lc = (XColor*)map->last;

		if (lc->red == color->red && 
		    lc->green == color->green &&
		    lc->blue == color->blue) 
			return color->pixel = lc->pixel;
	}

	c.color.red   = color->red   & 0xff00;
	c.color.green = color->green & 0xff00;
	c.color.blue  = color->blue  & 0xff00;

	if ((node = HashFind(map->htable, HASH(c.color), &c)) == NULL) {
		addColor(map, color);
	} else {
		/*
		**  Match found, if the entry hasn't been "alloced"
		**   yet, mark it so, and remove it from the "free" list.
		*/
		if (!node->used)
			entryUnlink(map, node);
		color->pixel = node->color.pixel;
	}

	map->last = (void*)&node->color;
	return color->pixel;
}

XColor *PaletteLookup(Palette *map, Pixel pix)
{
	Col	col;
	Col	*c;

	col.color.pixel = pix;

	if ((c = (Col*)HashFind(map->ltable, HASH_PIXEL(pix), &col)) != NULL) {
		return &c->color;
	}

printf("Shouldn't happen\n");
	return NULL;
}

Palette *PaletteFind(Widget w, Colormap cmap)
{
	PaletteList	*cur = cmapList;
	Display		*dpy = XtDisplay(w);
	
	for (cur = cmapList; cur != NULL; cur = cur->next)
		if (cur->cmap == cmap && cur->dpy == dpy)
			return cur->map;

	return NULL;
}

Pixmap	PixmapXForm(Widget w, Colormap inMap, Pixmap inPix, Colormap outMap)
{
	GC		gc = XtGetGC(w, 0, 0);
	Display		*dpy = XtDisplay(w);
	Pixmap		outPix;
	Window		root;
	int		x, y;
	unsigned int	width, height, bw, depth;
	XImage		*inImg, *outImg;
	Pixel		p;
	Palette		*inPal  = PaletteFind(w, inMap);
	Palette		*outPal = PaletteFind(w, outMap);
	XColor		*c;

	XGetGeometry(dpy, inPix, &root, &x, &y, &width, &height, &bw, &depth);

	outPix = XCreatePixmap(dpy, XtWindow(w), width, height, depth);

	inImg = XGetImage(dpy, inPix, 0, 0, width, height, ~0, ZPixmap);
	outImg = XGetImage(dpy, inPix, 0, 0, width, height, ~0, ZPixmap);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			XColor	xc;

			p = XGetPixel(inImg, x, y);
			c = PaletteLookup(inPal, p);
			xc = *c;
			XPutPixel(outImg, x, y, PaletteAlloc(outPal, &xc));
		}
	}

	XPutImage(dpy, outPix, gc, outImg, 0, 0, 0, 0, width, height);

	XDestroyImage(inImg);
	XDestroyImage(outImg);

	return outPix;
}

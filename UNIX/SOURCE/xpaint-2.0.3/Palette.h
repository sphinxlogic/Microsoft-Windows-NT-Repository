/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, David Koblas.                                     | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

typedef struct {
	int		isGrey;
	int		isMapped;
	int		ncolors;
	/*
	**
	*/
	int		readonly;
	Display		*display;
	Colormap	cmap;
	void		*htable, *ltable;
	void		*ctable;
	/*
	**  Free color entries
	*/
	int		nfree;
	void		*list;
	/*
	**  Cache the last request value here
	*/
	void		*last;
} Palette;

Palette *PaletteCreate(Widget);
Pixel    PaletteAlloc(Palette *, XColor *);
int    PaletteAllocN(Palette *, XColor *, int, Pixel *);
Palette *PaletteFind(Widget, Colormap);
XColor *PaletteLookup(Palette *, Pixel);
Pixmap  PixmapXForm(Widget, Colormap, Pixmap, Colormap);

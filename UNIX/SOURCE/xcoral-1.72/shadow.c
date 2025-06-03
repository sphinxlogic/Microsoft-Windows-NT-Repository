/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#include <X11/Xlib.h>
#include <stdio.h>

static char gray_bits[] = {0x01, 0x02 };

#define gray_width 		2
#define gray_height 		2
#define UP			0	   
	   
static unsigned long fg_pixel, bg_pixel;	   
static GC	relief_gc;
Pixmap		pixmap;

/*
**	Function name : CreateRGC
**
**	Description : Creation du contexte graphique pour le relief.
**	Input : Le display.
**	Ouput :
*/
void CreateRGC ( display )
	Display *display;
{
	GC 		gc;
	XGCValues	gcv;
	unsigned long 	mask;
	XColor		tcolor, color;
	int 		screen;
	
	screen = DefaultScreen ( display );

	mask = GCBackground | GCForeground | GCFunction;
	gcv.function = GXcopy;

	if ( DefaultDepth ( display, screen ) == 1 ) {
		/*
		 * Monochrome
		 */
		pixmap = XCreatePixmapFromBitmapData ( display, 
		 		DefaultRootWindow (display),
				gray_bits, gray_width, gray_height,
				BlackPixel ( display, screen ),
				WhitePixel ( display, screen ), 1 );

		fg_pixel = WhitePixel ( display, screen );
		bg_pixel = BlackPixel ( display, screen );
	
		gcv.background = bg_pixel;
		gcv.foreground = fg_pixel;

		gcv.tile = pixmap;
		gcv.fill_style = FillTiled;
		mask |= GCFillStyle | GCTile;		
	}
	else {
		/*
		 * color
		 */
	
		XAllocNamedColor ( display, DefaultColormap ( display, screen ),
			"white", &color, &tcolor );
		
		gcv.foreground = color.pixel;
		fg_pixel = color.pixel;
		
		XAllocNamedColor ( display, DefaultColormap ( display, screen ),
			"dim gray", &color, &tcolor );
		
		gcv.background = color.pixel;
		bg_pixel = color.pixel;

		gcv.fill_style = FillSolid;
		mask |= GCFillStyle;
	}
	gc = XCreateGC ( display, DefaultRootWindow (display), mask, &gcv );
	XSetLineAttributes ( display, gc, 2, LineSolid, CapButt, JoinMiter );
	relief_gc = gc;
}



/*
**	Function name : Display3D
**
**	Description : Affiche le top et le bottom shadow.
**	Input : Le display, la fenetre.
**	Ouput :
*/
void Display3D ( display, window, top, bottom, size, direct )
	Display		*display;
	Window		window;
	unsigned long   top, bottom;
	register        int size, direct;
{
	XWindowAttributes att;
	XGCValues	gcv;
	XRectangle 	rec [10];
	XPoint		points [10];
	unsigned long 	mask;

#ifdef DEBUG
	fprintf ( stderr, "top = %d bot =%d\n", top, bottom );
#endif

	if ( size == 0 ) return;

	XGetWindowAttributes ( display, window, &att );

	if ( size > att.width / 2 ) return;
	
	if ( direct == UP ) {
	        /* Les rectangles en haut et a gauche */
		rec [0].x = 0;
		rec [0].y = 0;
		rec [0].width = att.width;
		rec [0].height = size ;
		rec [1].x = 0;
		rec [1].y = size;
		rec [1].width = size ;
		rec [1].height = att.height - size;
	}
	else {
		rec [0].x = 0;
		rec [0].y = att.height - size;
		rec [0].width = att.width;
		rec [0].height = size;
		rec [1].x = att.width - size;
		rec [1].y = 0;
		rec [1].width = size;
		rec [1].height = att.height - size;
	}
	if ( DefaultDepth ( display, DefaultScreen ( display ) ) == 1 ) {
	       gcv.fill_style = FillTiled;
	       gcv.tile = pixmap;
	       gcv.foreground = fg_pixel; gcv.background = bg_pixel;
	       mask = GCFillStyle | GCTile | GCBackground | GCForeground;
	}
	else {
	       gcv.foreground = top; gcv.background = bottom;
	       gcv.fill_style = FillSolid;
	       mask = GCForeground | GCBackground | GCFillStyle;
	}
	XChangeGC ( display, relief_gc, mask, &gcv );

	XFillRectangles ( display, window, relief_gc, rec, 2 );
	
	if ( direct == UP ) { 
	        /* le polygone en bas et a droite */
		points [0].x = att.width; points [0].y = 0;
		points [1].x = att.width - size; points [1].y = size;
		points [2].x = att.width - size; points [2].y = att.height - size;
		points [3].x = size; points [3].y = att.height - size;
		points [4].x = 0; points [4].y = att.height;
		points [5].x = att.width; points [5].y = att.height;
	}
	else {
	        /* le polygone en haut et a gauche */
		points [0].x = att.width; points [0].y = 0;
		points [1].x = att.width - size; points [1].y = size;
		points [2].x = size; points [2].y = size;
		points [3].x = size; points [3].y = att.height - size;
		points [4].x = 0; points [4].y = att.height;
		points [5].x = 0; points [5].y = 0;
	}

	/*
	 * On inverse le foreground et le background.
	 */
	if ( DefaultDepth ( display, DefaultScreen ( display ) ) == 1 ) {
	       gcv.fill_style = FillSolid;
	       gcv.foreground = bg_pixel; gcv.background = fg_pixel;
	       mask = GCFillStyle | GCBackground | GCForeground;
	}
	else {
	       gcv.foreground = bottom; gcv.background = top;
	       gcv.fill_style = FillSolid;
	       mask = GCForeground | GCBackground | GCFillStyle;
	}
	XChangeGC ( display, relief_gc, mask, &gcv );

	XFillPolygon ( display,
		window, relief_gc, points, 6, Nonconvex, CoordModeOrigin );
}


/*
**	Function name : GetShadow
**
**	Description : Calcul des couleurs pour le top et le bottom shadow
**		en fonction d'un couleur donnee.
**	Input : Le display, la structure contenant la valeur en pixel
**		de la couleur dont on veut calculer ts et bs.
**		les valeurs de retour pour ts et bs.
**	Ouput : 
*/
void GetShadow ( display, xcolor, ts, bs )
	Display *display;
	register XColor *xcolor;
	unsigned long *ts, *bs;	/* RETURN */
{
	register int depth;
	Colormap cmap;
	XColor  top_sh, bot_sh;
	register int brightness;
	float factor;
	extern void exit ();

	depth = DisplayPlanes ( display, DefaultScreen ( display ));
	cmap = DefaultColormap ( display, DefaultScreen ( display ));

   	/* 
   	 * Monochrome
   	 */
	if ( depth == 1 ) {  /* Black and White */
		*ts = fg_pixel;
		*bs = bg_pixel;
		return;
	}

	/*
	 * Couleur.
	 * Calcul de l'eclat : L'intensite intervient pour 1/4 et
	 * la luminosite pour 3/4
	 */
	brightness = (0.25 * ((int) ((xcolor -> red + xcolor -> green + xcolor -> blue) / 3)) ) +
		(0.75 * ((0.3 * xcolor -> red) + (0.59 * xcolor -> green) +(0.11 * xcolor -> blue )));

	/*
	 * Suivant la valeur de l'eclat par rapport a 2 seuils, on calcul
	 * le top et bottom shadow correspondant a la couleur de base.
	 */
#define MAX	65535
#define CLAIR	(0.75 * MAX)
#define SOMBRE	(0.15 * MAX)
	
	if ( brightness < SOMBRE ) {
		bot_sh.red = xcolor -> red + ((MAX - xcolor -> red) * 0.2 );
		bot_sh.green= xcolor -> green + ((MAX - xcolor -> green) * 0.2 );
		bot_sh.blue = xcolor -> blue + ((MAX - xcolor -> blue) * 0.2 );
		top_sh.red = xcolor -> red + ((MAX - xcolor -> red) * 0.5);
		top_sh.green= xcolor -> green + ((MAX - xcolor -> green) * 0.5);
		top_sh.blue = xcolor -> blue + ((MAX - xcolor -> blue) * 0.5);
	}
	else if ( brightness > CLAIR ) {
		bot_sh.red = xcolor -> red * 0.2;
		bot_sh.green = xcolor -> green * 0.2;
		bot_sh.blue = xcolor -> blue *  0.2;
		top_sh.red = xcolor -> red * 0.95;
		top_sh.green = xcolor -> green * 0.95;
		top_sh.blue = xcolor -> blue * 0.95;
	}
	else {
		factor =  ((0.60 - (brightness * ( 0.25 ) / MAX))); 

		bot_sh.red = xcolor -> red - (xcolor -> red * factor);
		bot_sh.green = xcolor -> green - (xcolor -> green * factor);
		bot_sh.blue = xcolor -> blue -(xcolor -> blue * factor);

		factor =  ((0.40 + (brightness * ( 0.30 ) / MAX))); 

		top_sh.red = xcolor -> red + ((MAX - xcolor -> red) * factor );
		top_sh.green= xcolor -> green + ((MAX - xcolor -> green) * factor);
		top_sh.blue = xcolor -> blue + ((MAX - xcolor -> blue) * factor );
	}

	top_sh.pixel = 0;
   	bot_sh.pixel = 0;

	if ( XAllocColor ( display, cmap, &top_sh ) == 0 ) {
		(void) fprintf ( stderr, "XAllocColor error\n" );
      		(void) exit (1);
	}
	*ts = top_sh.pixel;

	if ( XAllocColor ( display, cmap, &bot_sh ) == 0 ) {
		(void) fprintf ( stderr, "XAllocColor error\n" );
      		(void) exit (1);      
	}
	*bs = bot_sh.pixel;
}

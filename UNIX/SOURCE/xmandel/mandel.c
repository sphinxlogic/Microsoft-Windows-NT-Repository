/* ta=4 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include "mandel.h"

struct imageparm manparm = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, NULL, NULL, "\0", "\0", "\0", "\0" };

extern Display *display;
extern Screen *screen;
extern struct imageparm *activeimage;

static int	scanline = -1;

static struct zoomd *zoomtmp;
/*
char *malloc(), *calloc();
*/
extern int depth;				/*	depth of bit plane				*/
extern int	dots_per_byte;
int bias = 16;					/* bias into color table */

static XImage *ximage = NULL;
static char *image_data = NULL;

#ifdef __STDC__
static void mandel();
#else
static void mandel();
#endif


/************************************************************************
*	do_mandel()			button: mandel, 	compute mandelbort set		*
************************************************************************/

void start_mandel()
{
	if (manparm.window == 0)
		return;
	if (manparm.zoom) 
	{	/* reset to beginning by popping zps off stack if not already there */
		while (zoomtmp = manparm.zoom->zp) 
		{	free(manparm.zoom);
			manparm.zoom = zoomtmp;
		}
	} 
	else 
	{	manparm.zoom = (struct zoomd *) malloc(sizeof (struct zoomd));
		manparm.zoom->zp = NULL;	/* NULL means last in stack - don't pop */
	}
	manparm.zoom->lx = START_LX; manparm.zoom->ux = START_UX;
	manparm.zoom->ly = START_LY; manparm.zoom->uy = START_UY;

	mandel ();
}

/************************************************************************
*	redo_mandel()		button: recalc		recalc current mandelbrot	*
************************************************************************/

void redo_mandel ()
{	if (manparm.window == 0)
		return;
	mandel ();
}


/************************************************************************
*	mandel()			do the mandelbrot calculations and send to video*
************************************************************************/

static void mandel ()
{
	double wx, wy, oldwx;
	double x, y;
	double incrx, incry;
	int ix, iy;
	int		shift;
	XEvent event;
	XWindowAttributes draw_wattr;
	GC draw_gc;
	Visual *draw_v;
	long	save_event_mask;

	if (manparm.window == 0)
		return;
	manparm.lx = manparm.zoom->lx;
	manparm.ly = manparm.zoom->ly;
	manparm.ux = manparm.zoom->ux;
	manparm.uy = manparm.zoom->uy;
	update_coordinates (&manparm, 1);

	XGetWindowAttributes (display, manparm.window, &draw_wattr);
	draw_gc = screen->default_gc;
	draw_v = screen->root_visual;
	save_event_mask = draw_wattr.your_event_mask;
	if (manparm.pixwidth != draw_wattr.width  ||  manparm.pixheight != draw_wattr.height)
	{
		/*	screen dimension changed.  rebuild image line information */

		manparm.pixheight = draw_wattr.height;
		manparm.pixwidth  = draw_wattr.width;
		manparm.bytewidth = (manparm.pixwidth + dots_per_byte - 1) / dots_per_byte;
		scanline = -1;

		if (ximage)
		 	XDestroyImage (ximage);		/* also deallocates image_data automagically */
		if (manparm.pix)
			free (manparm.pix);

		image_data = malloc (manparm.bytewidth + 1);	/* Really! bug in ISC X11?? */
		manparm.pix = (unsigned char *) malloc (manparm.bytewidth * manparm.pixheight);
		if (image_data == NULL  ||  manparm.pix == NULL)
		{	printf ("malloc failed\n"); 
			return; 
		}
		ximage = XCreateImage (display, draw_v, depth, ZPixmap, 0, image_data, manparm.pixwidth, 1, 8, 0);
		ximage->byte_order = MSBFirst;
		ximage->bitmap_bit_order = MSBFirst;
	}

	/*	incremental x/y values for pixel coordinates */

	incrx = (manparm.zoom->ux - manparm.zoom->lx) / manparm.pixwidth;
	incry = (manparm.zoom->uy - manparm.zoom->ly) / manparm.pixheight;
	y = manparm.zoom->uy;

	XClearWindow (display, manparm.window);
	XSelectInput (display, manparm.window, ButtonPressMask | ButtonReleaseMask | ExposureMask);

	for (iy = 0; iy < draw_wattr.height; iy++, y-= incry) 
	{	int i;
		if (depth < 8)
			for (i = 0; i < manparm.bytewidth; i++) 
				*(image_data + i) = 0;
		x = manparm.zoom->lx;

		/*	compute colour values for each pixel in a single scan line */
		
		for (ix = 0; ix < manparm.pixwidth; ix++, x+= incrx) 
		{
			/*	compute set value for current point */

			wx = x; wy = y;
			for (i = 0; i < ITERATION_DEPTH; i++) 
			{	oldwx = wx;
				wx = wx * wx - wy * wy + x;
				wy = 2 * oldwx * wy + y;
				if (wx * wx + wy * wy > 4) 
					goto useit;
			}
			if (depth == 8)
			{	if (INFINITY_COLOR >= 0)
					i = INFINITY_COLOR - COLOR_BIAS;
			}
			else
			{	if (INFINITY_COLOR >= 0)
					i = INFINITY_COLOR;
			}

useit:		/*	assign colour to pixel	*/

			switch (depth)
			{
			case 8:
				image_data[ix] = i + COLOR_BIAS;
				break;

			case 4:
				if (ix % 2)
					image_data[ix >> 1] |= (i % 16);
				else
					image_data[ix >> 1] |= ((i % 16) << 4);
				break;

			case 2:
				switch (ix % 4)
				{
				case 0:		shift = 6;	break;
				case 1:		shift = 4;	break;
				case 2:		shift = 2;	break;
				case 3:		shift = 0;	break;
				}
				image_data[ix >> 2] |= (i % 4) << shift;
				break;

			case 1:
				if (i % 2)
					image_data[ix >> 3] |= 1 << (7 - ix%8);
			}
		}

		/*	save scan line for exposure events or gif output */
		
		memcpy (manparm.pix + iy * manparm.bytewidth, image_data, manparm.bytewidth);
		scanline = iy;

		/*	send image line to screen */

		XPutImage (display, manparm.window, draw_gc, ximage, 0,0, 0, iy, manparm.pixwidth, 1);

		/*	check for exposure or buttons after a few lines have been drawn.
			This keeps server queries do a reasonable level while still
			allowing exposures without too much delay.
		*/
		if (iy % EXPOSURE_TEST == 0)
		{	/*	XSync to ensure exposure or button is honoured "fast" */

			XSync (display, False);

			/*	first cleanup any pending exposures */

			while (XCheckTypedEvent (display, Expose, &event) == True)
				XtDispatchEvent (&event);

			/*	then check for abort if a button is pressed */

			while (XCheckWindowEvent (display, manparm.window, ButtonPressMask | ButtonReleaseMask, &event) == True)
			{	if (event.type == ButtonRelease)
					goto escape;
			}
		}
	}
escape:
	XSelectInput (display, manparm.window, save_event_mask);
}

/************************************************************************
*	expose_mandel()			handle mandel exposure event				*
************************************************************************/

void expose_mandel (w, unused, event)
Widget w;
caddr_t unused;
XExposeEvent *event;
{
	if (manparm.window == 0)
		return;
	if (event  &&  manparm.pix  &&  scanline >= 0)
		redraw_mandel (event);
}

/************************************************************************
*	redraw_mandel()			reshow mandelbrot from backup image.		*
************************************************************************/

void redraw_mandel (event)
XExposeEvent *event;
{
	int  iy;
	XWindowAttributes draw_wattr;
	GC draw_gc;
	Visual *draw_v;
	int	maxy;

	if (manparm.window == 0)
		return;
	XGetWindowAttributes (display, manparm.window, &draw_wattr);
	draw_gc = screen->default_gc;
	draw_v = screen->root_visual;
	if (event == NULL)
	{
		/*	refresh entire window */

		maxy = MIN (scanline, manparm.pixheight-1);
		for (iy = 0; iy <= maxy; iy++) 
		{	memcpy (image_data, manparm.pix + iy * manparm.bytewidth, manparm.bytewidth);
			XPutImage (display, manparm.window, draw_gc, ximage, 0,0, 0, iy, manparm.pixwidth, 1);
		}
	}
	else if (event->x < manparm.pixwidth)
	{
		/*	refresh only exposed area */

		int	first_byte, last_byte, nbytes, first_dot, last_dot, ndots;

		if (event->x < manparm.pixwidth)
		{	first_byte = event->x / dots_per_byte;
			first_dot  = first_byte * dots_per_byte;
			last_dot   = MIN (manparm.pixwidth - 1, event->x + event->width - 1);
			last_byte  = (last_dot + dots_per_byte - 1) / dots_per_byte;
			nbytes     = last_byte - first_byte + 1;
			ndots      = last_dot - first_dot + 1;
			maxy       = MIN (event->y + event->height, manparm.pixheight);
			for (iy = event->y; iy < maxy  &&  iy <= scanline; iy++) 
			{	memcpy (image_data, manparm.pix + iy * manparm.bytewidth + first_byte, nbytes);
				XPutImage (display, manparm.window, draw_gc, ximage, 0,0, event->x, iy, ndots, 1);
			}				
		}
	}

	XFlush(display);
}

/************************************************************************
*	do_zoom ()		button: zomm		zoom into mandelbrot			*
*					The rubber band preserves preserves the current		*
*					window aspect ration.								*
************************************************************************/

void do_zoom (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XWindowAttributes draw_wattr;
	GC draw_gc;
	Window Root;
	int S;
	int	hh, ww, delta;
	double scaley, xa, ya, xb, yb;
	long	save_event_mask;
	int		have_press;

	Window	root, child;
	int		pos_x, pos_y;
	unsigned int keys_buttons;
	int		root_x, root_y;

	if (activeimage->window == 0)
		return;
	if (activeimage->zoom == NULL) return;

	draw_gc = screen->default_gc;
	S = DefaultScreen (display);
	Root = RootWindow (display, S);

	XSetForeground (display, draw_gc, 255);
	XSetSubwindowMode (display, draw_gc, IncludeInferiors);
	XSetFunction (display, draw_gc, GXxor);

	XGetWindowAttributes (display, activeimage->window, &draw_wattr);

	save_event_mask = draw_wattr.your_event_mask;
	XSelectInput (display, activeimage->window, ButtonPressMask | ButtonReleaseMask 
			| PointerMotionHintMask | PointerMotionMask);

	have_press = 0;		/* Motif translation override may cause a 	*/
						/* button release BEFORE a button press		*/
	while (1) 
	{	static int rubberband = 1;
		XEvent report;

		XNextEvent (display, &report);
		if (report.xbutton.window != activeimage->window)
			continue;
		switch(report.type) 
		{
			int winx0,winy0,winx1,winy1,width,height;
			int x0, y0, x1, y1;
			double tw, th;

		case ButtonPress:
			if (report.xbutton.button != Button1)
				break;
			x0 = winx0 = report.xbutton.x;
			y0 = winy0 = report.xbutton.y;
			rubberband = 0;
			width = height = 0;
			have_press = 1;
			break;

		case ButtonRelease:
			if (have_press == 0)
				break;
			winx1 = report.xbutton.x;
			winy1 = report.xbutton.y;
			x0 = MIN (winx0, winx1);
			x1 = MAX (winx0, winx1);
			y0 = MIN (winy0, winy1);
			y1 = MAX (winy0, winy1);
			width  = x1 - x0; 
			height = y1 - y0;

			/*	ensure rubber band preserves the proportions of the
				current window.
			*/
			ww = width;
			hh = height;
			delta = MAX (ww, hh);
			scaley = (double) (draw_wattr.height) / (double) (draw_wattr.width);
			x1 = x0 + delta;
			y1 = y0 + delta * scaley;
			height = y1 - y0;
			width  = x1 - x0;

			XDrawRectangle (display,activeimage->window, draw_gc, x0, y0, width, height);
			rubberband = 1;
			XSetFunction (display, draw_gc, GXcopy);
			XFlush (display);
			if (delta > 5  &&  report.xbutton.button == 1)
			{	tw = activeimage->zoom->ux - activeimage->zoom->lx;
				th = activeimage->zoom->uy - activeimage->zoom->ly;
				zoomtmp = (struct zoomd *) malloc (sizeof (struct zoomd));
				zoomtmp->zp = activeimage->zoom;		/* push onto stack */
				zoomtmp->ux = activeimage->zoom->lx + ((double) x1 / (double) activeimage->pixwidth) * tw;
				zoomtmp->lx = activeimage->zoom->lx + ((double) x0 / (double) activeimage->pixwidth) * tw;
				zoomtmp->ly = activeimage->zoom->uy - ((double) y1 / (double) activeimage->pixheight) * th;
				zoomtmp->uy = activeimage->zoom->uy - ((double) y0 / (double) activeimage->pixheight) * th;
				activeimage->zoom = zoomtmp;			/* activeimage->zoom is current pointer */
				XSelectInput (display, activeimage->window, save_event_mask);
				if (activeimage == &manparm)
					mandel ();
				else
					julia (0, 0);
			}
			else
			{	/*	too small image or pressing button 2/3 quits zoom */

				activeimage->lx = activeimage->zoom->lx;
				activeimage->ly = activeimage->zoom->ly;
				activeimage->ux = activeimage->zoom->ux;
				activeimage->uy = activeimage->zoom->uy;
#ifndef MOTIF
				printf ("\n");
#else
				update_coordinates (activeimage, 0);
#endif
				XSelectInput (display, activeimage->window, save_event_mask);
			}
			return;

		case MotionNotify:
			if (!XQueryPointer (display, report.xmotion.window,
					&root, &child, &root_x, &root_y,
					&pos_x, &pos_y, &keys_buttons))
				break;
			winx1 = pos_x;
			winy1 = pos_y;
			if (rubberband == 0)
			{	XDrawRectangle (display,activeimage->window, draw_gc, x0, y0, width, height);
				x0 = MIN(winx0, winx1);
				x1 = MAX(winx0, winx1);
				y0 = MIN(winy0, winy1);
				y1 = MAX(winy0, winy1);
				width  = x1 - x0; 
				height = y1 - y0;

				/*	ensure rubber band preserves the proportions of the
					current window.
				*/
				ww = width;
				hh = height;
				delta = MAX (ww, hh);
				scaley = (double) (draw_wattr.height) / (double) (draw_wattr.width);
				x1 = x0 + delta;
				y1 = y0 + delta * scaley;
				height = y1 - y0;
				width  = x1 - x0;

				XDrawRectangle (display,activeimage->window, draw_gc, x0, y0, width, height);

				tw = activeimage->zoom->ux - activeimage->zoom->lx;
				th = activeimage->zoom->uy - activeimage->zoom->ly;
				xa = activeimage->zoom->lx + ((double) x0 / (double) activeimage->pixwidth) * tw;
				ya = activeimage->zoom->uy - ((double) y0 / (double) activeimage->pixheight) * th;
				xb = activeimage->zoom->lx + ((double) x1 / (double) activeimage->pixwidth) * tw;
				yb = activeimage->zoom->uy - ((double) y1 / (double) activeimage->pixheight) * th;

				activeimage->lx = MIN (xa, xb);
				activeimage->ly = MIN (ya, yb);
				activeimage->ux = MAX (xa, xb);
				activeimage->uy = MAX (ya, yb);
				update_coordinates (activeimage, 0);
			}
			else
			{
				tw = activeimage->zoom->ux - activeimage->zoom->lx;
				th = activeimage->zoom->uy - activeimage->zoom->ly;
				xa = activeimage->zoom->lx + ((double) winx1 / (double) activeimage->pixwidth) * tw;
				ya = activeimage->zoom->uy - ((double) winy1 / (double) activeimage->pixheight) * th;

				activeimage->pcolour = pixel_colour (activeimage, winx1, winy1);
				activeimage->px = xa;
				activeimage->py = ya;
				update_point (activeimage);
			}
			fflush (stdout);
			XFlush (display);
			break;

		default:
			break;
		}
	}
}

/************************************************************************
*	do_unzoom()			button: unzoom		recalc previous zoom level.	*
************************************************************************/

void do_unzoom (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (activeimage->window == 0)
		return;
	if (activeimage->zoom) 
	{	/* selecting mooz before mandel invalid */
		if (zoomtmp = activeimage->zoom->zp) 
		{	/* last on stack */
			free(activeimage->zoom);
			activeimage->zoom = zoomtmp;
		}
		else
			return;
		if (activeimage == &manparm)
			mandel ();
		else
			julia (0, 0);
	}
}

/************************************************************************
*	pixel_colour()		return the colour of a pixel					*
************************************************************************/

int pixel_colour (p, ix, iy)
struct imageparm *p;
int		ix, iy;
{	int		colour, shift;
	unsigned char *imageline;

	imageline = p->pix + iy * p->bytewidth;
	switch (depth)
	{
	case 8:
		colour = imageline[ix] & 0x0ff;
		break;

	case 4:
		if (ix % 2)
			shift = 0;
		else
			shift = 4;
		colour = (imageline[ix >> 1] >> shift) & 0x0f;
		break;

	case 2:
		switch (ix % 4)
		{
		case 0:		shift = 6;	break;
		case 1:		shift = 4;	break;
		case 2:		shift = 2;	break;
		case 3:		shift = 0;	break;
		}
		colour  = (imageline[ix >> 2] >> shift) & 0x03;
		break;

	case 1:
		colour = (imageline[ix >> 3]  >> (7 - ix % 8)) & 0x01;
	}
	return (colour);
}

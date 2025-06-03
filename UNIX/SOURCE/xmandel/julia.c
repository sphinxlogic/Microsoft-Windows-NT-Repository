/* ta=4 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include "mandel.h"

struct imageparm juliaparm = { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0, NULL, NULL, "\0", "\0", "\0", "\0" };
static struct zoomd *zoomtmp;
extern struct imageparm manparm;

void julia ();

int	 julia_in_progress = 0;

extern Display *display;
extern Screen *screen;
extern int depth;
extern int dots_per_byte;

static int scanline = -1;
static int bottomscan = 0;

static GC draw_gc;
static XImage *ximage = NULL;
static char *image_data = NULL;
static XWindowAttributes draw_wattr;
static double a = 1.2;
static double b = 1.2;

/************************************************************************
*	do_mandel()			button: mandel, 	compute mandelbort set		*
************************************************************************/

void start_julia (destroy_pixmap, newimage)
int		destroy_pixmap;
int		newimage;
{
	if (juliaparm.window == 0)
		return;
	if (juliaparm.zoom)
	{	while (zoomtmp = juliaparm.zoom->zp) 
		{	free(juliaparm.zoom);
			juliaparm.zoom = zoomtmp;
		}
	} 
	else 
	{	juliaparm.zoom = (struct zoomd *) malloc(sizeof (struct zoomd));
		juliaparm.zoom->zp = NULL;	/* NULL means last in stack - don't pop */
	}
	juliaparm.zoom->lx = juliaparm.lx;
	juliaparm.zoom->ux = juliaparm.ux;
	juliaparm.zoom->ly = juliaparm.ly;
	juliaparm.zoom->uy = juliaparm.uy;

	julia (destroy_pixmap, 1);
}


/************************************************************************
*	do_julia()			button:	julia		compute julia set.			*
************************************************************************/

void julia (destroy_pixmap, newimage)
int		destroy_pixmap;
int		newimage;
{
	XEvent ev;
	double wx, wy, oldwx;
	double sx, x, y;
	double incrx, incry;
	int ix, iy;
	long	save_event_mask;
	int		shift, mirror;
	Visual *draw_v;

	if (manparm.zoom == NULL  ||  juliaparm.window == 0)
		return;
	juliaparm.lx = juliaparm.zoom->lx;
	juliaparm.ly = juliaparm.zoom->ly;
	juliaparm.ux = juliaparm.zoom->ux;
	juliaparm.uy = juliaparm.zoom->uy;
	update_coordinates (&juliaparm, 1);

	scanline = -1;
	julia_in_progress = 1;

	draw_gc = screen->default_gc;
	draw_v = screen->root_visual;
	XGetWindowAttributes(display, manparm.window, &draw_wattr);
	save_event_mask = draw_wattr.your_event_mask;

	if (newimage)
	{
		/*	user must provide a point in the Mandelbrot window */

		a = b = 0;
		XSelectInput (display, manparm.window, ButtonPressMask | ButtonReleaseMask
					| PointerMotionHintMask | PointerMotionMask | ExposureMask);
		while (1) 
		{	XEvent report;

			XNextEvent (display, &report);
			if (report.type == ButtonRelease  &&  report.xmotion.window == manparm.window)
			{	a = ( (double) report.xbutton.x / (double) draw_wattr.width);
				b = ( (double) report.xbutton.y / (double) draw_wattr.height);
				a = (a * (manparm.zoom->ux - manparm.zoom->lx)) + manparm.zoom->lx;
				b = manparm.zoom->uy - (b * (manparm.zoom->uy - manparm.zoom->ly));
				juliaparm.px = a;
				juliaparm.py = b;
				update_point (&juliaparm);
				break;
			}
			else if (report.type == MotionNotify)
			{	Window	root,  child;
				int		pos_x, pos_y;
				unsigned int keys_buttons;
				int		root_x, root_y;
				if (XQueryPointer (display, report.xmotion.window,
						&root, &child, &root_x, &root_y,
						&pos_x, &pos_y, &keys_buttons)  &&  report.xmotion.window  == manparm.window)
				{
					a = ( (double) pos_x / (double) draw_wattr.width);
					b = ( (double) pos_y / (double) draw_wattr.height);
					a = (a * (manparm.zoom->ux - manparm.zoom->lx)) + manparm.zoom->lx;
					b = manparm.zoom->uy - (b * (manparm.zoom->uy - manparm.zoom->ly));
					juliaparm.px = a;
					juliaparm.py = b;
					juliaparm.pcolour = pixel_colour (&manparm, pos_x, pos_y);
					update_point (&juliaparm);
				}
			}
			else
				XtDispatchEvent (&report);
			continue;
		}
	}
	XSelectInput (display, manparm.window, save_event_mask);

	XGetWindowAttributes (display, juliaparm.window, &draw_wattr);
	if (destroy_pixmap)
	{	if (ximage)
			XDestroyImage(ximage); 			/* also deallocated image_data */
		if (juliaparm.pix)
			free (juliaparm.pix);
		image_data 		= NULL;  
		juliaparm.pix 	= NULL; 
		ximage 			= NULL;
		juliaparm.pixwidth = juliaparm.pixheight = -9999;

	}

	XClearWindow (display, juliaparm.window);
	XSelectInput(display, juliaparm.window, ExposureMask | ButtonPressMask | ButtonReleaseMask);
	if (juliaparm.pixwidth != draw_wattr.width  ||  juliaparm.pixheight != draw_wattr.height)
	{
		/*	if window changes size, rebuild the image information */
	
		juliaparm.pixwidth = draw_wattr.width;
		juliaparm.bytewidth = (juliaparm.pixwidth + dots_per_byte - 1) / dots_per_byte;

		if (ximage)
			XDestroyImage (ximage); 
		if (juliaparm.pix)
			free (juliaparm.pix);

		image_data = malloc (juliaparm.bytewidth + 1);
		juliaparm.pix = (unsigned char *) malloc (draw_wattr.height * juliaparm.bytewidth);
		if (image_data == NULL  ||  juliaparm.pix == NULL) 
		{	printf ("malloc failed\n"); 
			return; 
		}
		ximage = XCreateImage (display, draw_v, depth, ZPixmap, 0, image_data, juliaparm.pixwidth, 1, 8, juliaparm.bytewidth);
		ximage->byte_order = MSBFirst;
		ximage->bitmap_bit_order = MSBFirst;
	}
	juliaparm.pixheight = draw_wattr.height;


	/*	compute julia set for given coordinates */

	sx = x = juliaparm.lx;
	y = juliaparm.uy;
	incrx = (juliaparm.ux - juliaparm.lx)  / draw_wattr.width;
	incry = (juliaparm.ly - juliaparm.uy)  / draw_wattr.height;
	scanline = -1;
	bottomscan = draw_wattr.height;
	for (iy = 0; iy < draw_wattr.height;  iy++, y+=incry) 
	{	int i;

		x = sx;
		if (depth < 8) 
		{	for (i = 0 ; i < juliaparm.bytewidth; i++) 
				*(image_data + i) = 0;
		}

		/*	build all points for one scan line of output */

		for (ix = 0; ix < draw_wattr.width; ix++, x+=incrx) 
		{
			/*	compute the colour of a single pixel */

			wx = x; wy = y;
			for (i = 0; i < ITERATION_DEPTH; i++) 
			{	oldwx = wx;
				wx = wx * wx - wy * wy + a;
				wy = 2 * oldwx * wy + b;
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

useit:		/*	assign colour to point based on screen depth */

			switch (depth)
			{
			case 8:
				*(image_data + ix) = i + COLOR_BIAS;
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

		/*	save a copy of the scan line for exposure events or gif output */

		memcpy (juliaparm.pix + iy * juliaparm.bytewidth, image_data, juliaparm.bytewidth);
		scanline = iy;
		/*	send the image line to the screen */

		XPutImage (display, juliaparm.window, draw_gc, ximage, 0,0, 0, iy, juliaparm.pixwidth, 1);

		/*	handle exposure or button pressed events */

		if (iy % EXPOSURE_TEST == 0)
		{	XSync (display, False);
			while (XCheckTypedEvent (display, Expose, &ev) == True)
				XtDispatchEvent (&ev);
			while (XCheckWindowEvent (display, juliaparm.window, ButtonPressMask | ButtonReleaseMask, &ev) == True)
			{	if (ev.type == ButtonRelease)
					goto escape;
			}
		}
	}
escape:
	XSync (display, False);
	julia_in_progress = 0;
}


/************************************************************************
*	expose_julia()		handle exposure event on julia window.			*
************************************************************************/

void expose_julia (w, unused, event)
Widget w;
caddr_t unused;
XExposeEvent *event;
{
	if (juliaparm.window == 0)
		return;
	if (event  &&  juliaparm.pix  && scanline >= 0)
		redraw_julia (event);
}

/************************************************************************
*	redraw_julia()		send julia from image bit vectors				*
************************************************************************/

void redraw_julia (event)
XExposeEvent *event;
{
	int  iy;
	XWindowAttributes draw_wattr;
	int		jump;
	int	maxy;

	if (juliaparm.window == 0)
		return;
	if (juliaparm.pix == NULL  ||  scanline < 0)
		return;

	XGetWindowAttributes (display, juliaparm.window, &draw_wattr);
	jump = 1;
	if (event == NULL)
	{
		/*	refresh entire image */

		maxy = MIN (draw_wattr.height, juliaparm.pixheight);
		for (iy = 0; iy <= scanline; iy++) 
		{	memcpy (image_data, juliaparm.pix + iy * juliaparm.bytewidth, juliaparm.bytewidth);
			XPutImage (display, juliaparm.window, draw_gc, ximage, 0,0, 0, iy, juliaparm.pixwidth, 1);
		}
	}
	else
	{	/*	refresh only exposed area */

		int	first_byte, last_byte, nbytes, first_dot, last_dot, ndots;

		if (event->x < juliaparm.pixwidth)
		{	first_byte = event->x / dots_per_byte;
			first_dot  = first_byte * dots_per_byte;
			last_dot   = MIN (juliaparm.pixwidth - 1, event->x + event->width - 1);
			last_byte  = (last_dot + dots_per_byte - 1) / dots_per_byte;
			nbytes     = last_byte - first_byte + 1;
			ndots      = last_dot - first_dot + 1;
			maxy       = MIN (event->y + event->height, juliaparm.pixheight);
			for (iy = event->y; iy <= scanline; iy++) 
			{	memcpy (image_data, juliaparm.pix + iy * juliaparm.bytewidth + first_byte , nbytes);
				XPutImage (display, juliaparm.window, draw_gc, ximage, 0,0, event->x, iy, ndots, 1);
			}
		}
	}
	XFlush(display);
}


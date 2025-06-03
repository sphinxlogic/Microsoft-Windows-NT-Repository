/* ta=4 */
/*  xmandel.h  */

/* ---- some changes you may wish to make ------- */

/*  memcpy(dest,src,nbytes) is used in various routines.  If memcpy()
    is not available, use the #define for bcopy().  If  bcopy() is not
    available then enable "NEEDMEMCPY" to use a very inefficient
    memory copy.
*/

#ifdef HAVEBCOPY
#define memcpy(a,b,c)	bcopy(b,a,c)	/* bsd type */
#endif

/* #define NEEDMEMCPY				/* for systems with neither */


/* set maximum depth of iteration for mandelbrot and julia sets. */

#define ITERATION_DEPTH		256

/* Check for exposure events after this many scan lines have been drawn
   by the Mandelbrot or Julia generator.  If you have a very large
   ITERATION_DEPTH (e.g. > 400) an EXPOSURE_TEST of 1 or 2 is reasonable  */

#define EXPOSURE_TEST		4

/* If a 256 colour display is used, Set a "color bias" to offset into 
   the colour table for colour value 0.  Iterations arc coloured
   as "iteration_count + COLOR_BIAS".
*/

#define COLOR_BIAS		0

/* Set a color value for INFINITY.  A value >= 0 assigns the specified
   colour number.  A value < 0 uses the "default" colour.
*/

#define INFINITY_COLOR		-1

/*  Initial starting coordinates for mandelbrot set */

#define START_LX -2.25
#define START_UX  1.0
#define START_LY -1.5
#define START_UY  1.5


/* ----- end configuration ------- */

#define MIN(a,b)((a) < (b) ? (a) : (b))
#define MAX(a,b)((a) < (b) ? (b) : (a))
#define ABS(a) ((a) < 0 ? (-a) : (a))

/*	for those places where I forgot to make the change... */

#define malloc(x)	XtMalloc(x)
#define calloc(x,y)	XtCalloc(x,y)
#define free(x)		XtFree(x)

typedef unsigned char	byte;

struct zoomd 
{	struct zoomd *zp;		/*	previous zoom pointer		*/
	double lx, ux;			/*	bounds for zoom				*/
	double ly, uy;
};

#define SAVESIZE 25

struct imageparm
{	Window	window;			/* image in this window			*/
	double	lx, ly, ux, uy;	/* image corner coordinates.	*/
	double	px, py;			/* point location (for julia)	*/
	int		rotation;		/* current colour rotation		*/
	int		totrot;			/*	total rotation count		*/
	int		pcolour;		/*	pixel colour				*/
	long	pixheight;		/* image height in pixels		*/
	long	pixwidth;		/* impage width in pixels		*/
	long	bytewidth;		/* width of image line in bytes	*/
	unsigned char *pix;		/* copy of generated image 		*/
	struct	zoomd *zoom;	/* zoom stack pointer			*/
	char 	sux[SAVESIZE];	/* text ux, uy etc. to detect	*/
	char 	suy[SAVESIZE];	/* if user changed values.		*/
	char	slx[SAVESIZE];
	char	sly[SAVESIZE];
} ;

struct enter_buttons
{	char	*button;		/*	label text					*/
	int		bx;				/*	lable x						*/
	int		by;				/*	label y						*/
	char	*field;			/*	data field name				*/
	int		fsize;			/*	data field size chars		*/
} ;


struct button_list
{	char	*ident;				/*	widget name				*/
	void	(*bcall)();			/*	callback function		*/
	int		x;					/*	x column location		*/
	int		y;					/*	y row location			*/
} ;


#ifdef __STDC__
/* main.c */
int main(int argc, char **argv);
void update_coordinates(struct imageparm *image, int lf);
void update_point(struct imageparm *image);
/* mandel.c */
void start_mandel(void);
void redo_mandel(void);
void expose_mandel(Widget w, caddr_t unused, XExposeEvent *event);
void redraw_mandel(XExposeEvent *event);
void do_zoom(Widget w, caddr_t client_data, caddr_t call_data);
void do_unzoom(Widget w, caddr_t client_data, caddr_t call_data);
int pixel_colour(struct imageparm *p, int ix, int iy);
/* julia.c */
void start_julia(int destroy_pixmap, int newimage);
void julia(int destroy_pixmap, int newimage);
void expose_julia(Widget w, caddr_t unused, XExposeEvent *event);
void redraw_julia(XExposeEvent *event);
/* writegif.c */
void ws1te_gif_image(char *fname, struct imageparm *imageinfo);
#else
/* main.c */
int main();
void update_coordinates();
void update_point();
/* mandel.c */
void start_mandel();
void redo_mandel();
void expose_mandel();
void redraw_mandel();
void do_zoom();
void do_unzoom();
int pixel_colour();
/* julia.c */
void start_julia();
void julia();
void expose_julia();
void redraw_julia();
/* writegif.c */
void write_gif_image();
#endif

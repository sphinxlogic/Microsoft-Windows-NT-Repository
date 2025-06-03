/* Copyright (C) 1992 Nathan Sidwell */
/*{{{  about me*/
/* a simple program to compress the monochrome, mask & color sprites
 * together into one set of planes. This is done by color separation
 * overlay. Not particularly well written or documented, but then
 * its not supposed to be a general tool.
 * If anybody wants to make it more general, they are welcome
 */
/*}}}*/
/*#define SYNCHRONIZE*/
/*{{{  ANSI or K&R?*/
#if __STDC__
#include <stdarg.h>
#define PROTOARG(list) list
#define VARARG ...
#define FUNCARG(list, defs) (defs)
#define FUNCVARARG(list, defs) (defs, ...)
#define ARGSEP ,
#define FUNCARGVOID ()
#define VARARGSET(args, last) va_start(args, last)
#define VOIDFUNC void
#define VOIDRETURN
#else /* !__STDC__ */
#include <varargs.h>
#define PROTOARG(list) ()
#define const
#define volatile
#define VARARG va_alist
#define FUNCARG(list, defs) list defs;
#define FUNCVARARG(list, defs) list defs; va_dcl
#define ARGSEP  ;
#define FUNCARGVOID ()
#define VARARGSET(args, last) va_start(args)
#define VOIDFUNC int
#define VOIDRETURN 0
#endif /* __STDC__ */
/*}}}*/
/*{{{  includes*/
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
/*}}}*/
#define MAX_PLANES 5
#define MAX_COLORS (1 << MAX_PLANES)
/*{{{  typedef struct Coord*/
typedef struct Coord
/* general coordinate store */
{
  int       x;
  int       y;
} COORD;
/*}}}*/
/*{{{  typedef struct Size*/
typedef struct Size
/* general size store */
{
  unsigned  x;
  unsigned  y;
} SIZE;
/*}}}*/
/*{{{  typedef struct Source*/
typedef struct Source
{
  Pixmap    pixmap;
  COORD     hot;
  SIZE      size;
  unsigned  negate;
  unsigned  edge;
} SOURCE;
/*}}}*/
static Display *display;
static Pixmap source;
static Pixmap current;
static Pixmap scratch;
static SIZE size;
static Pixmap color_bitmap;
static int color_planes = 0;
/*{{{  contexts*/
static GC gc_xor;
static GC gc_or;
static GC gc_and;
static GC gc_copy;
static GC gc_clear;
/*}}}*/
static unsigned long mono_noswap;
static unsigned long mono_swap;
static Pixmap pixmaps[MAX_COLORS];
static char *noswap_colors[MAX_COLORS];
static char *swap_colors[MAX_COLORS];
static int colors;
static char *filename;
static int error = 0;
/*{{{  int error_handler(display, event)*/
static int error_handler
FUNCARG((dpy, event),
	Display *dpy
ARGSEP  XErrorEvent *event
)
{
  fprintf(stderr, "X error %d, request %d:%d\n",
      event->error_code, event->request_code, event->minor_code);
  while(QLength(display))
    {
      XEvent  buffered;
      
      XNextEvent(dpy, &buffered);
      fprintf(stderr, "0x%x type %d\n", buffered.xany.window, buffered.type);
    }
  fflush(stderr);
  abort();
  return 0;
}
/*}}}*/
/*{{{  unsigned isblank(Pixmap pixmap)*/
static unsigned isblank
FUNCARG((pixmap),
	Pixmap pixmap
)
{
  unsigned  ix;
  XImage *image;
  
  if(pixmap != scratch)
    XCopyArea(display, pixmap, scratch, gc_copy,
	0, 0, size.x, size.y, 0, 0);
  for(ix = 1; ix < size.y; ix <<= 1)
    /*EMPTY*/;
  for(ix >>= 1; ix; ix >>= 1)
    XCopyArea(display, scratch, scratch, gc_or,
	0, (int)ix, size.x, ix, 0, 0);
  for(ix = 1; ix < size.x; ix <<= 1)
    /*EMPTY*/;
  for(ix >>= 1; ix; ix >>= 1)
    XCopyArea(display, scratch, scratch, gc_or,
	(int)ix, 0, ix, 1, 0, 0);
  image = XGetImage(display, scratch, 0, 0, 1, 1, 1, XYPixmap);
  ix = XGetPixel(image, 0, 0);
  XDestroyImage(image);
  return !ix;
}
/*}}}*/
/*{{{  void make_color_pixmaps(char **name, Pixmap *pixmaps, char const *title)*/
static VOIDFUNC make_color_pixmaps
FUNCARG((name, pixmaps, title),
	char      **name
ARGSEP  Pixmap    *pixmaps
ARGSEP  char const *title
)
{
  unsigned ix;
  
  for(ix = 0; name[ix]; ix++)
    if(ix >= (1 << color_planes))
      XFillRectangle(display, pixmaps[ix], gc_clear, 0, 0, size.x, size.y);
    else
      {
	unsigned bit;
	
	XCopyArea(display, source, pixmaps[ix], gc_copy,
	    0, 0, size.x, size.y, 0, 0);
	for(bit = 0; bit != color_planes; bit++)
	  if((1 << bit) & ix)
	    {
	      XCopyArea(display, color_bitmap, pixmaps[ix], gc_and,
		0, (int)(size.y * bit), size.x, size.y, 0, 0);
	    }
	  else
	    {
	      XCopyArea(display, color_bitmap, current, gc_copy,
		0, (int)(size.y * bit), size.x, size.y, 0, 0);
	      XFillRectangle(display, current, gc_xor, 0, 0, size.x, size.y);
	      XCopyArea(display, current, pixmaps[ix], gc_and,
		  0, 0, size.x, size.y, 0, 0);
	    }
      }
  XCopyArea(display, source, current, gc_copy, 0, 0, size.x, size.y, 0, 0);
  for(ix = 0; name[ix]; ix++)
    XCopyArea(display, pixmaps[ix], current, gc_xor,
	0, 0, size.x, size.y, 0, 0);
  if(!isblank(current))
    {
      unsigned  x, y;
      XImage *image;
      
      error = 1;
      fprintf(stderr, "%s: Error on %s at", filename, title);
      image = XGetImage(display, current, 0, 0, size.x, size.y, 1, XYPixmap);
      for(y = 0; y != size.y; y++)
	for(x = 0; x != size.x; x++)
	  if(XGetPixel(image, (int)x, (int)y))
	    fprintf(stderr, " (%d, %d)", x, y);
      fprintf(stderr, "\n");
      XDestroyImage(image);
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void add_edge(unsigned ix, Pixmap *pixmaps)*/
static VOIDFUNC add_edge
FUNCARG((ix, pixmaps),
	unsigned ix
ARGSEP  Pixmap *pixmaps
)
{
  XCopyArea(display, source, current, gc_copy, 0, 0, size.x, size.y, 0, 0);
  XCopyArea(display, source, current, gc_and, 0, 0, size.x, size.y,  0,  1);
  XCopyArea(display, source, current, gc_and, 0, 0, size.x, size.y,  0, -1);
  XCopyArea(display, source, current, gc_and, 0, 0, size.x, size.y,  1,  0);
  XCopyArea(display, source, current, gc_and, 0, 0, size.x, size.y, -1,  0);
  XDrawRectangle(display, current, gc_clear, 0, 0, size.x - 1, size.y - 1);
  XCopyArea(display, source, current, gc_xor, 0, 0, size.x, size.y, 0, 0);
  XCopyArea(display, current, pixmaps[ix], gc_copy, 0, 0, size.x, size.y, 0, 0);
  XFillRectangle(display, current, gc_xor, 0, 0, size.x, size.y);
  while(ix--)
    XCopyArea(display, current, pixmaps[ix], gc_and, 0, 0, size.x, size.y, 0, 0);
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void compress_colors(char **name, Pixmap *pixmaps, char const *title)*/
static VOIDFUNC compress_colors
FUNCARG((name, pixmaps, title),
	char **name
ARGSEP  Pixmap *pixmaps
ARGSEP  char const *title
)
{
  unsigned  ix, j;
  
  for(ix = 0; name[ix + 1]; ix++)
    {
      for(j = ix + 1; name[j]; j++)
	if(!strcmp(name[ix], name[j]))
	  {
	  
	    XCopyArea(display, pixmaps[j], pixmaps[ix], gc_or, 0, 0, size.x, size.y, 0, 0);
	    XFillRectangle(display, pixmaps[j], gc_clear, 0, 0, size.x, size.y);
	    name[j] = (char *)"-";
	  }
    }
  for(ix = 0; name[ix]; ix++)
    {
      if(isblank(pixmaps[ix]))
	{
	  Pixmap old;
	  
	  if(strcmp(name[ix], "-"))
	    fprintf(stderr, "Warning: %s color '%s' has no pixels\n",
		title, name[ix]);
	  old = pixmaps[ix];
	  for(j = ix; name[j + 1]; j++)
	    {
	      name[j] = name[j+1];
	      pixmaps[j] = pixmaps[j+1];
	    }
	  name[j] = NULL;
	  pixmaps[j] = old;
	  ix--;
	}
      else if(!strcmp(name[ix], "-"))
      {
	unsigned  x, y;
	XImage *image;
	
	error = 1;
	fprintf(stderr, "%s: Void color on %s at", filename, title);
	image = XGetImage(display, pixmaps[ix], 0, 0, size.x, size.y, 1, XYPixmap);
	for(y = 0; y != size.y; y++)
	  for(x = 0; x != size.x; x++)
	    if(XGetPixel(image, (int)x, (int)y))
	      fprintf(stderr, " (%d, %d)", x, y);
	fprintf(stderr, "\n");
	XDestroyImage(image);
      }
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  void merge_mono()*/
static VOIDFUNC merge_mono FUNCARGVOID
{
  unsigned ix;

  for(ix = 1; ix != colors; ix++)
    {
      if(isblank(source))
	break;
      XCopyArea(display, source, current, gc_copy,
	  0, 0, size.x, size.y, 0, 0);
      XCopyArea(display, pixmaps[ix], current, gc_and,
	  0, 0, size.x, size.y, 0, 0);
      if(!isblank(current))
	{
	  mono_swap |= 1 << ix;
	  XCopyArea(display, current, scratch, gc_copy,
	      0, 0, size.x, size.y, 0, 0);
	  XCopyArea(display, pixmaps[ix], scratch, gc_xor,
	      0, 0, size.x, size.y, 0, 0);
	  if(!isblank(scratch))
	    {
	      XCopyArea(display, pixmaps[ix], pixmaps[colors], gc_copy,
		  0, 0, size.x, size.y, 0, 0);
	      XCopyArea(display, current, pixmaps[colors], gc_xor,
		  0, 0, size.x, size.y, 0, 0);
	      noswap_colors[colors] = noswap_colors[ix];
	      swap_colors[colors] = swap_colors[ix];
	      mono_noswap = mono_noswap |
		  (((mono_noswap >> ix) & 1) << colors);
	      XCopyArea(display, current, pixmaps[ix], gc_copy,
		  0, 0, size.x, size.y, 0, 0);
	      colors++;
	    }
	  XCopyArea(display, pixmaps[ix], source, gc_xor,
	      0, 0, size.x, size.y, 0, 0);
	}
    }
  return VOIDRETURN;
}
/*}}}*/
/*{{{  int main(int argc, char **argv)*/
int main
FUNCARG((argc, argv),
	int argc
ARGSEP  char **argv
)
{
  Pixmap mono_bitmap;
  Pixmap mono_source;
  int  planes;
  char *edge = NULL;
  char *noswap[MAX_COLORS + 1];
  char *swap[MAX_COLORS + 1];
  Pixmap noswap_pixmaps[MAX_COLORS];
  Pixmap swap_pixmaps[MAX_COLORS];
  COORD hot;
  Window  root;
  int status;
  unsigned ix;
  Pixmap output_bitmap;
  unsigned noswap_edge = 0;
  unsigned noswap_invert = 0;
  unsigned swap_invert = 0;
  unsigned swap_copy = 0;
  unsigned no_mono = 0;
  unsigned mono_planes;
  
  /*{{{  slurp up the arguments*/
  {
    filename = argv[1];
    if(argc > 2)
      {
	argv += 2;
	argc -= 2;
	for(; argc; argc--, argv++)
	  {
	    if(argv[0][0] == '+')
	      {
		if(argv[0][2])
		  error = 1;
		else if(argv[0][1] == 's')
		  swap_copy = 1;
		else if(argv[0][1] == 'n')
		  noswap_edge = 1;
		else
		  error = 1;
	      }
	    else if(argv[0][0] == '-')
	      {
		if(!argv[0][1])
		  break;
		else if(argv[0][2])
		  error = 1;
		else if(argv[0][1] == 's')
		  swap_invert = 1;
		else if(argv[0][1] == 'n')
		  noswap_invert = 1;
		else if(argv[0][1] == '0')
		  no_mono = 1;
		else if(isdigit(argv[0][1]))
		  {
		    no_mono = 1;
		    color_planes = argv[0][1] - '0';
		  }
		else
		  error = 1;
	      }
	    else
	      break;
	    if(error)
	      {
		fprintf(stderr, "'%s' not a flag\n", argv[0]);
		argc = 0;
		break;
	      }
	  }
      }
    for(ix = 0; argc && strcmp(*argv, "+"); argc--, argv++)
      noswap[ix++] = *argv;
    if(argc)
      argv++, argc--;
    while(ix != MAX_COLORS + 1)
      noswap[ix++] = NULL;
    for(ix = 0; argc && strcmp(*argv, "+"); argc--, argv++)
      swap[ix++] = *argv;
    if(argc)
      argv++, argc--;
    while(ix != MAX_COLORS + 1)
      swap[ix++] = NULL;
    if(argc)
      {
	edge = *argv;
	argv++, argc--;
      }
    if(argc || !filename || (no_mono &&
	(noswap_edge || noswap_invert || swap_invert || swap_copy)))
      {
	fprintf(stderr,
	    "Usage: scram <name> [-<n> | [+n] [+s] [-n] [-s]] <noswap_list> + <swap_list> + [<edge>]\n");
	return 1;
      }
  }
  /*}}}*/
  if(no_mono)
    mono_planes = 1;
  else
    mono_planes = noswap_edge | noswap_invert | swap_copy | swap_invert ?
	2 : 3;
  display = XOpenDisplay(NULL);
  if(!display)
    {
      fprintf(stderr, "Cannot open display\n");
      return 1;
    }
  XSetErrorHandler(error_handler);
#ifdef SYNCHRONIZE
  XSynchronize(display, True);
#endif
  root = DefaultRootWindow(display);
  /*{{{  read files*/
  {
    char *name;
    unsigned width, height;
    int x, y;
    
    name = malloc(strlen(filename) + 7);
    if(!name)
      {
	fprintf(stderr, "Cannot malloc.\n");
	exit(1);
      }
    strcpy(name, filename);
    strcat(name, ".mono");
    status = XReadBitmapFile(display, root, name,
	&size.x, &size.y, &mono_source, &hot.x, &hot.y);
    if(status || size.y % mono_planes)
      {
	fprintf(stderr, "Cannot read monochrome '%s'\n", name);
	return 1;
      }
    size.y /= mono_planes;
    strcpy(name, filename);
    strcat(name, ".color");
    status = XReadBitmapFile(display, root, name,
	&width, &height, &color_bitmap, &x, &y);
    if(status)
      {
	fprintf(stderr, "Cannot read colour '%s'\n", name);
	return 1;
      }
    if(color_planes)
      size.y = height / color_planes;
    else
      color_planes = height / size.y;
    if(height % size.y || width != size.x)
      {
	fprintf(stderr, "Bitmaps are incompatible sizes\n");
	return 1;
      }
    free(name);
  }
  /*}}}*/
  /*{{{  alloc some pixmaps*/
  {
    source = XCreatePixmap(display, root, size.x, size.y, 1);
    current = XCreatePixmap(display, root, size.x, size.y, 1);
    scratch = XCreatePixmap(display, root, size.x, size.y, 1);
    mono_bitmap = XCreatePixmap(display, root, size.x, size.y * 2, 1);
    if(!source || !current || !scratch || !mono_bitmap)
      {
	fprintf(stderr, "Cannot make pixmaps.\n");
	return 1;
      }
  }
  /*}}}*/
  /*{{{  create graphics contexts*/
  {
    XGCValues gcv;
	
    gcv.plane_mask = 1;
    gcv.line_width = 1;
    gcv.line_style = LineSolid;
    gcv.join_style = JoinMiter;
    gcv.fill_style = FillSolid;
    gcv.cap_style = CapNotLast;
    gcv.background = 0;
    gcv.function = GXxor;
    gcv.foreground = 1;
    gc_xor = XCreateGC(display, source,
	    GCForeground | GCBackground | GCFunction |
	    GCPlaneMask | GCCapStyle |
	    GCLineStyle | GCLineWidth | GCJoinStyle | GCFillStyle, &gcv);
    gcv.function = GXor;
    gcv.foreground = 1;
    gc_or = XCreateGC(display, source,
	    GCForeground | GCBackground | GCFunction |
	    GCPlaneMask | GCCapStyle |
	    GCLineStyle | GCLineWidth | GCJoinStyle | GCFillStyle, &gcv);
    gcv.function = GXand;
    gcv.foreground = 1;
    gc_and = XCreateGC(display, source,
	    GCForeground | GCBackground | GCFunction |
	    GCPlaneMask | GCCapStyle |
	    GCLineStyle | GCLineWidth | GCJoinStyle | GCFillStyle, &gcv);
    gcv.function = GXcopy;
    gcv.foreground = 1;
    gc_copy = XCreateGC(display, source,
	    GCForeground | GCBackground | GCFunction |
	    GCPlaneMask | GCCapStyle |
	    GCLineStyle | GCLineWidth | GCJoinStyle | GCFillStyle, &gcv);
    gcv.function = GXcopy;
    gcv.foreground = 0;
    gc_clear = XCreateGC(display, source,
	    GCForeground | GCBackground | GCFunction |
	    GCPlaneMask | GCCapStyle |
	    GCLineStyle | GCLineWidth | GCJoinStyle | GCFillStyle, &gcv);
    if(!gc_xor || !gc_or || !gc_and || !gc_copy || !gc_clear)
      {
	fprintf(stderr, "Cannot make contexts");
	return 1;
      }
  }
  /*}}}*/
  /*{{{  alloc a pile of pixmaps*/
  {
    for(ix = 0; ix < MAX_COLORS; ix++)
      {
	pixmaps[ix] = XCreatePixmap(display, root, size.x, size.y, 1);
	if(!pixmaps[ix])
	  {
	    fprintf(stderr, "Cannot make pixmap %d\n", ix);
	    return 1;
	  }
	XFillRectangle(display, pixmaps[ix], gc_clear, 0, 0, size.x, size.y);
      }
    for(ix = 0; ix < MAX_COLORS; ix++)
      {
	noswap_pixmaps[ix] = XCreatePixmap(display, root, size.x, size.y, 1);
	if(!noswap_pixmaps[ix])
	  {
	    fprintf(stderr, "Cannot make noswap pixmap %d\n", ix);
	    return 1;
	  }
	XFillRectangle(display, noswap_pixmaps[ix], gc_clear, 0, 0, size.x, size.y);
      }
    for(ix = 0; ix < MAX_COLORS; ix++)
      {
	swap_pixmaps[ix] = XCreatePixmap(display, root, size.x, size.y, 1);
	if(!swap_pixmaps[ix])
	  {
	    fprintf(stderr, "Cannot make swap pixmap %d\n", ix);
	    return 1;
	  }
	XFillRectangle(display, swap_pixmaps[ix], gc_clear, 0, 0, size.x, size.y);
      }
  }
  /*}}}*/
  colors = !no_mono;
  if(!no_mono)
    {
      /*{{{  fiddle mono noswap*/
      {
	XCopyArea(display, mono_source, source, gc_copy,
	    0, (int)size.y, size.x, size.y, 0, 0);
	if(noswap_invert)
	  XFillRectangle(display, source, gc_xor, 0, 0, size.x, size.y);
	if(noswap_edge)
	  {
	    XCopyArea(display, mono_source, current, gc_copy, 0, 0,
		size.x, size.y, 0, 0);
	    XCopyArea(display, mono_source, current, gc_and, 0, 0,
		size.x, size.y, 0, 1);
	    XCopyArea(display, mono_source, current, gc_and, 0, 0,
		size.x, size.y, 0, -1);
	    XCopyArea(display, mono_source, current, gc_and, 0, 0,
		size.x, size.y, 1, 0);
	    XCopyArea(display, mono_source, current, gc_and, 0, 0,
		size.x, size.y, -1, 0);
	    XDrawRectangle(display, current, gc_clear,
		0, 0, size.x - 1, size.y - 1);
	    XFillRectangle(display, current, gc_xor, 0, 0, size.x, size.y);
	    XCopyArea(display, current, source, gc_or,
		0, 0, size.x, size.y, 0, 0);
	  }
	XCopyArea(display, mono_source, source, gc_and, 0, 0, size.x, size.y, 0, 0);
	XCopyArea(display, source, mono_bitmap, gc_copy, 0, 0, size.x, size.y, 0, 0);
      }
      /*}}}*/
      /*{{{  fiddle mono swap*/
      {
	XCopyArea(display, mono_source, source, gc_copy,
	    0, (int)(size.y * (mono_planes - 1)), size.x, size.y, 0, 0);
	if(swap_invert)
	  XFillRectangle(display, source, gc_xor, 0, 0, size.x, size.y);
	XCopyArea(display, mono_source, source, gc_and, 0, 0, size.x, size.y, 0, 0);
	XCopyArea(display, source, mono_bitmap, gc_copy, 0, 0, size.x, size.y, 0, (int)size.y);
      }
      /*}}}*/
    }
  XCopyArea(display, mono_source, source, gc_copy, 0, 0, size.x, size.y, 0, 0);
  make_color_pixmaps(noswap, noswap_pixmaps, "Noswap");
  if(edge)
    {
      for(ix = 0; noswap[ix]; ix++)
	/*EMPTY*/;
      add_edge(ix, noswap_pixmaps);
      noswap[ix] = edge;
    }
  XCopyArea(display, mono_source, source, gc_copy, 0, 0, size.x, size.y, 0, 0);
  make_color_pixmaps(swap, swap_pixmaps, "Swap");
  compress_colors(noswap, noswap_pixmaps, "Noswap");
  compress_colors(swap, swap_pixmaps, "Swap");
  /*{{{  merge the noswap colors*/
  {
    for(ix = 0; noswap[ix]; ix++)
      {
	XCopyArea(display, noswap_pixmaps[ix], pixmaps[ix + colors], gc_copy,
	  0, 0, size.x, size.y, 0, 0);
	noswap_colors[ix + colors] = noswap[ix];
      }
    colors = ix + colors;
  }
  /*}}}*/
  /*{{{  merge the swap colors*/
  for(ix = 0; swap[ix]; ix++)
    {
      unsigned j;
      
      for(j = 0; j != colors; j++)
	{
	  if(isblank(swap_pixmaps[ix]))
	    break;
	  XCopyArea(display, swap_pixmaps[ix], current, gc_copy,
	      0, 0, size.x, size.y, 0, 0);
	  XCopyArea(display, pixmaps[j], current, gc_and,
	      0, 0, size.x, size.y, 0, 0);
	  if(!isblank(current))
	    {
	      swap_colors[j] = swap[ix];
	      XCopyArea(display, current, scratch, gc_copy,
		  0, 0, size.x, size.y, 0, 0);
	      XCopyArea(display, pixmaps[j], scratch, gc_xor,
		  0, 0, size.x, size.y, 0, 0);
	      if(!isblank(scratch))
		{
		  XCopyArea(display, pixmaps[j], pixmaps[colors], gc_copy,
		      0, 0, size.x, size.y, 0, 0);
		  XCopyArea(display, current, pixmaps[colors], gc_xor,
		      0, 0, size.x, size.y, 0, 0);
		  noswap_colors[colors] = noswap_colors[j];
		  XCopyArea(display, current, pixmaps[j], gc_copy,
		      0, 0, size.x, size.y, 0, 0);
		  colors++;
		}
	      XCopyArea(display, pixmaps[j], swap_pixmaps[ix], gc_xor,
		  0, 0, size.x, size.y, 0, 0);
	    }
	}
    }
  /*}}}*/
  if(!no_mono)
    {
      /*{{{  merge the noswap mono*/
      {
	XCopyArea(display, mono_bitmap, source, gc_copy,
	    0, 0, size.x, size.y, 0, 0);
	merge_mono();
	mono_noswap = mono_swap;
	mono_swap = 0;
      }
      /*}}}*/
      /*{{{  merge the swap mono*/
      {
	XCopyArea(display, mono_bitmap, source, gc_copy,
	    0, (int)size.y, size.x, size.y, 0, 0);
	merge_mono();
      }
      /*}}}*/
      /*{{{  add mono color masks*/
      {
	char st[20];
	
	sprintf(st, "0x%lx", mono_noswap);
	noswap_colors[0] = strcpy(malloc(strlen(st) + 1), st);
	sprintf(st, "0x%lx", mono_swap);
	swap_colors[0] = strcpy(malloc(strlen(st) + 1), st);
      }
      /*}}}*/
    }
  /*{{{  create the output bitmap*/
  {
    unsigned mask;
    
    for(mask = 1, planes = 0; mask < colors; mask <<= 1)
      planes++;
    fprintf(stderr, "Creating %s.%s with %d colors on %d planes\n", filename,
	no_mono ? "packed" : "h", colors, planes);
    output_bitmap = XCreatePixmap(display, root, size.x, size.y * planes, 1);
    if(!output_bitmap)
      {
	fprintf(stderr, "Cannot make output pixmap\n");
	return 1;
      }
    XFillRectangle(display, output_bitmap, gc_clear, 0, 0, size.x, size.y * planes);
    for(ix = 0; ix != colors; ix++)
      {
	int bit;
	
	for(bit = 0; bit != planes; bit++)
	  if(ix & (1 << bit))
	    XCopyArea(display, pixmaps[ix], output_bitmap, gc_or,
		0, 0, size.x, size.y, 0, (int)(bit * size.y));
      }
  }
  /*}}}*/
  /*{{{  write the file*/
  {
    char *name;
    FILE  *stream;
    char *ptr;
    XImage *image;
    unsigned  x, y;
    unsigned  count;
    unsigned  val;
    unsigned  ix;
    
    name = malloc(strlen(filename) + 8);
    if(!name)
      {
	fprintf(stderr, "Cannot malloc\n");
	return 1;
      }
    strcpy(name, filename);
    strcat(name, no_mono ? ".packed" : ".h");
    image = XGetImage(display, output_bitmap, 0, 0, size.x, size.y * planes,
	1, XYPixmap);
    if(!image)
      {
	fprintf(stderr, "Cannot get image\n");
	return 1;
      }
    ptr = strrchr(filename, '/');
    if(!ptr)
      ptr = filename;
    else
      ptr++;
    stream = fopen(name, "w");
    if(!stream)
      {
	fprintf(stderr, "Cannot write file\n");
	return 1;
      }
    fprintf(stream, "#define %s_width %d\n", ptr, (int)size.x);
    fprintf(stream, "#define %s_height %d\n", ptr, (int)(size.y * planes));
    if(hot.x >= 0)
      {
	fprintf(stream, "#define %s_x_hot %d\n", ptr, hot.x);
	fprintf(stream, "#define %s_y_hot %d\n", ptr, hot.y);
      }
    if(!no_mono)
      fprintf(stream, "#define %s_depth %d\n", ptr, (int)planes);
    fprintf(stream, "static unsigned char %s_bits[] =\n{\n ", ptr);
    count = 10;
    for(y = 0; y < size.y * planes; y++)
      {
	ix = val = 0;
	for(x = 0; x < size.x; x++)
	  {
	    val = val | (XGetPixel(image, (int)x, (int)y) << ix++);
	    if(ix == 8)
	      {
		if(!count--)
		  {
		    count = 9;
		    fputs("\n ", stream);
		  }
		fprintf(stream, " 0x%02x,", (int)val);
		val = ix = 0;
	      }
	  }
	if(ix)
	  {
	    if(!count--)
	      {
		count = 9;
		fputs("\n ", stream);
	      }
	    fprintf(stream, " 0x%02x,", (int)val);
	  }
      }
    fprintf(stream, "\n};\n");
    if(no_mono)
      {
	fputs("  ", stdout);
	for(ix = 0; ix != colors; ix++)
	  fprintf(stdout, "%s ", noswap_colors[ix]);
	fputs("+ \\\n", stdout);
	fputs("  ", stdout);
	for(ix = 0; ix != colors; ix++)
	  fprintf(stdout, "%s ", swap_colors[ix]);
	fputs("+\n", stdout);
      }
    else
      {
	fprintf(stream, "static unsigned long %s_noswap[] =\n{\n", ptr);
	for(ix = 0; ix != colors; ix++)
	  fprintf(stream, "  %s,\n", noswap_colors[ix]);
	while(ix++ < 1 << planes)
	  fprintf(stream, "  ~(unsigned long)0,\n");
	fprintf(stream, "};\n");
	fprintf(stream, "static unsigned long %s_swap[] =\n{\n", ptr);
	for(ix = 0; ix != colors; ix++)
	  fprintf(stream, "  %s,\n", swap_colors[ix]);
	while(ix++ < 1 << planes)
	  fprintf(stream, "  ~(unsigned long)0,\n");
	fprintf(stream, "};\n");
      }
    if(ferror(stream))
      {
	fprintf(stderr, "Error writing file");
	error = 1;
      }
    fclose(stream);
    XDestroyImage(image);
    if(error)
      unlink(name);
    free(name);
  }
  /*}}}*/
  XCloseDisplay(display);
  return error;
}
/*}}}*/

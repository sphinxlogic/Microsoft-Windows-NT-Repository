/*
 * X11 version of SUN spinning globe demo 
 * 
 * From ELKTRA::DW_EXAMPLES #249.33 modified by Mike Augeri 18-OCT-1990
 *
 * Added different size builds and fixed for OpenVMS AXP - Ed Smith 11/3/92
 */

#include <stdio.h>
#include <file.h>
#include <decw$include:Xlib.h>
#include <decw$include:Xatom.h>
#include <decw$include:Xutil.h>

#ifdef VMS
#define index strchr 
#endif /* VMS */

#ifdef SMALL
#define FRAMEWIDTH 64
#define FRAMEHEIGHT 64
#define FRAMESIZE 63
#else
#ifdef MEDIUM
#define FRAMEWIDTH 128
#define FRAMEHEIGHT 128
#define FRAMESIZE 127
#else
#define FRAMEWIDTH 256
#define FRAMEHEIGHT 256
#define FRAMESIZE 255
#endif
#endif

#define NFRAMES 30
#define PICTDEFAULT "-100-50"
#define NCMAP 64
#define MAXPLANES 6
#define DEFAULT_DELAY_FACTOR 200000 /* no units, just a delay in main loop */

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

Display *globe_disp;
Screen *globe_screen;
Window wind;

char *malloc();
char *rindex();
float fmsecs;

main(argc, argv)
int argc;
char **argv;
{
  char *geometry = NULL;
  int x = 100, y = 100, k;
  int delay_factor = DEFAULT_DELAY_FACTOR;
  char data_file[128] = "globe_bitmap";
  char program_name[128];
  char *left;

  char *display = "decw$display:";
  char *window_title = "XGlobe";

  int nplanes;	/* number of planes to allocate */
  int ncolors;
  int nbytes;
  int index_pixel;		/* color cell from allocation */
  int planes[MAXPLANES];		/* plane mask */
  int msb;

  unsigned char weight[FRAMEWIDTH*FRAMEHEIGHT];

  int fd, open(), close(), offset;


  XSetWindowAttributes xswa;

  XImage *imgptr;
  XImage *weimgptr;
  char globe_data[NFRAMES*FRAMEWIDTH*FRAMEHEIGHT/8];

  XGCValues pixgcv;
  GC pixgc;
  XGCValues globegcv;
  GC globegc;
  XGCValues wgcv;
  GC wgc;

  Pixmap gpix;
  Pixmap wepix;

  int update=1000;
  XEvent event, peekevent;
  XExposeEvent *expw  = (XExposeEvent *)&event;
  register int i, j;

#ifndef VMS
    left = rindex(argv[0],'/');
#else VMS
    left = rindex(argv[0],']');
#endif VMS
    if (left) {
        left++;
        strcpy(program_name,left);
    } else {
        strcpy(program_name,argv[0]);
    }
    for (i = 1; i < argc; i++) {
    if (argv[i][0] == '=') {
      geometry = argv[i];
      continue;
    }
    if (strcmp(argv[i], "-x") == 0) {    /* x pos */
	  if (++i >= argc) usage(argv[0]);
	  x = atoi(argv[i]);
	}
    if (strcmp(argv[i], "-y") == 0) {    /* y pos*/
	  if (++i >= argc) usage(argv[0]);
	  y = atoi(argv[i]);
	}
    if (strcmp(argv[i], "-d") == 0) {    /* delay factor */
	  if (++i >= argc) usage(argv[0]);
	  delay_factor = atoi(argv[i]);
	}
    if (strcmp(argv[i], "-t") == 0){
          if (++i >= argc) usage(argv[0]);
          window_title = argv[i];
        }
    if (argv[i][0] == '-') usage(argv[0]);
  }

  fd = open (data_file,O_RDONLY);
  if (fd == -1) {
    fprintf(stderr,"%s:  Cannot open file with logical name %s -- exiting.\n",
	program_name, data_file);
    exit(1);
  }
  offset = 0;
  while ((nbytes = read(fd,&globe_data[offset],
            NFRAMES*FRAMEWIDTH*FRAMEHEIGHT/8-offset)) > 0)
  {
    offset = offset + nbytes;
  };
  if (offset != NFRAMES*FRAMEWIDTH*FRAMEHEIGHT/8)
  {  
    fprintf(stderr, "Cannot read globe data\n");
    exit(1);
  };
  if (!(globe_disp = XOpenDisplay(display))) {
    perror(argv[0]);
    exit(1);
  }
  globe_screen = DefaultScreenOfDisplay(globe_disp);
  
  XSynchronize(globe_disp,1);
  XFlush(globe_disp,globe_screen);

  xswa.event_mask = ButtonPressMask|VisibilityChangeMask;
  xswa.background_pixel = BlackPixelOfScreen(globe_screen);
  wind = XCreateWindow(globe_disp,RootWindowOfScreen(globe_screen),
				x,y,FRAMEWIDTH,FRAMEHEIGHT,0,
				DefaultDepthOfScreen(globe_screen),
				InputOutput,
				DefaultVisualOfScreen(globe_screen),
				(CWEventMask|CWBackPixel),&xswa);
  if (!wind) { 
    fprintf(stderr, "XCreateWindow failed\n");
    exit(1);
  }
  XStoreName(globe_disp,wind,window_title);
  XSelectInput(globe_disp,wind, ButtonPressMask | VisibilityChangeMask);
  
  XMapWindow(globe_disp,wind);
  
  initcmap(&ncolors,planes,&nplanes,&index_pixel);
  makeweights(&weight,FRAMEHEIGHT,FRAMEWIDTH,ncolors,planes,index_pixel);

  msb = 0;  /* determine most significant plane */
  for (i = 0; i < nplanes; i++)
    if (planes[i] > msb) msb = planes[i];

  globegcv.foreground = msb;
  globegcv.background = 0;
  globegcv.plane_mask = msb;
  globegc = XCreateGC(globe_disp,wind,(GCForeground|GCBackground|GCPlaneMask),
	&globegcv);

  gpix = XCreatePixmap(globe_disp,RootWindowOfScreen(globe_screen),
	    FRAMEWIDTH,FRAMEHEIGHT,1);

  imgptr = XCreateImage(globe_disp,DefaultVisualOfScreen(globe_screen),
	1,XYPixmap,0,&globe_data,FRAMEWIDTH,FRAMEHEIGHT*NFRAMES,0,FRAMEWIDTH/8);

  pixgcv.plane_mask = 0xff;
  pixgc = XCreateGC(globe_disp,gpix,(GCPlaneMask),&pixgcv);

  wgcv.plane_mask = 0xff;
  wgc = XCreateGC(globe_disp,wind,(GCPlaneMask),&wgcv);

  wepix = XCreatePixmap(globe_disp,wind, FRAMEWIDTH,FRAMEHEIGHT,
		DefaultDepthOfScreen(globe_screen));

  weimgptr = XCreateImage(globe_disp,DefaultVisualOfScreen(globe_screen),
		DefaultDepthOfScreen(globe_screen),ZPixmap,0,&weight,
		    FRAMEWIDTH,FRAMEHEIGHT,0,FRAMEWIDTH);

  i = 0;
  while(1) {
    if(XPending(globe_disp) != 0) {
      XNextEvent(globe_disp,&event); 
      switch((int)event.type) {
	
      case ButtonPress:
	if(QLength(globe_disp) > 0) {
	  XPeekEvent(globe_disp,&peekevent);
	  if(peekevent.type == VisibilityNotify) break;
	}
      case VisibilityNotify:

	XPutImage(globe_disp,wepix,wgc,weimgptr,0,0,0,0,FRAMEWIDTH,FRAMEHEIGHT);
	XCopyArea(globe_disp,wepix,wind,wgc,0,0,FRAMEWIDTH,FRAMEHEIGHT,0,0);
	break;
      }
    }

    XPutImage(globe_disp,gpix,pixgc,imgptr,0,(FRAMESIZE*i)+i,0,0,FRAMEWIDTH,FRAMEHEIGHT);
    XCopyPlane(globe_disp,gpix,wind,globegc,0,0,FRAMEWIDTH,FRAMEHEIGHT,0,0,0x01);
    XFlush(globe_disp) ;
    if(++i >= NFRAMES) i = 0;
    k = delay_factor;
    fmsecs = (float) k/1000.0;     /* Convert from msecs to seconds */
    lib$wait(&fmsecs);
    while (k > 0) k--;

  }
}

initcmap(ncolors,planes,nplanes,index_pixel)
     int *ncolors, *planes, *nplanes, *index_pixel;
{
  int i;
  XColor cmap[1<<MAXPLANES];
  Colormap colmap;

  *nplanes = MAXPLANES;
  
  *ncolors = 1 << MAXPLANES;

  colmap = DefaultColormapOfScreen(globe_screen);
  
  while (XAllocColorCells(globe_disp, colmap,
			1,
			planes,
			*nplanes,
			index_pixel,
			1) == 0)
  {
    (*nplanes)--;
    *ncolors = (*ncolors)/2;
  };

  if (*ncolors < 2)
  {
    fprintf(stderr,"Insufficient color planes resource!\n");
    exit(1);
  };

  cmap[0].pixel = index_to_pixel(0,planes,*index_pixel);
  cmap[0].red=cmap[0].green=cmap[0].blue=0;
  cmap[0].flags = (DoRed|DoGreen|DoBlue);
  for (i = 1; i < (*ncolors); i++) {
    cmap[i].pixel = index_to_pixel(i,planes,*index_pixel);
    if(i<((*ncolors)/2)) {
      cmap[i].red = 0;
      cmap[i].green = i * (1<<15)/(*ncolors) + (1<<14);
      cmap[i].blue = i * (1<<16)/(*ncolors) + (1<<15);
      cmap[i].flags = (DoRed|DoGreen|DoBlue);
    } else {
      cmap[i].red = i * (1<<13)/(*ncolors) + (1<<13);
      cmap[i].green = i * (1<<15)/(*ncolors) + (1<<15)-1;
      cmap[i].blue = 0;
      cmap[i].flags = (DoRed|DoGreen|DoBlue);
    }
  }
  if(!(XStoreColors(globe_disp,colmap,&cmap,(*ncolors))))
		printf("Cannot Store Colormap \n");

  if(!(XSetWindowColormap(globe_disp,wind,colmap)) )
	printf("UNABLE TO SET WINDOW ColorMap\n");

  if(!(XInstallColormap(globe_disp,colmap)))
	printf("UNABLE TO INSTALL COLORMAP");
}

makeweights(weight, height, width, ncolors, planes, index_pixel)
     unsigned char *weight;
     int height, width, ncolors, *planes, index_pixel;
{
  int i,j,radius,r;
  
  r = FRAMEHEIGHT / 2 ;
  for(i=0;i<FRAMEHEIGHT;i++) {
    for(j=0;j<FRAMEWIDTH;j++) {
      radius = (((i-r)*(i-r)+(j-r)*(j-r))*(ncolors/2))/(r*r);
      radius = (radius*radius)/(ncolors/2);
      *weight++ = index_to_pixel((ncolors/2)-MIN((ncolors/2)-1,radius)-1,
		planes,index_pixel); 
    }
  }
}

usage (program)
    char *program;
{
    fprintf(stderr,"\n");
    fprintf(stderr,"Usage: %s [ options ] \n",program);
    fprintf(stderr,"\n");
    fprintf(stderr,"     where options are one or more of:\n");
    fprintf(stderr,"     [-x #] [-y #] [-d delay] [-t WindowTitle]\n");
    fprintf(stderr,"\n");
    exit(1);
}

static int index_to_pixel(index,planes,offset_pixel)
int index, *planes, offset_pixel;
{
int i;
int pixel = 0;

    for (i = 0; i < MAXPLANES; i++)
    {
	if (index & 1<<i)
	    pixel = pixel | planes[i];
    }
    return(pixel|offset_pixel);
}



/* @(#)rindex.c 4.1 (Berkeley) 12/21/80 */
/*
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
*/

/* #define NULL 0 */

char *
rindex(sp, c)
register char *sp, c;
{
        register char *r;

        r = NULL;
        do {
                if (*sp == c)
                        r = sp;
        } while (*sp++);
        return(r);
}



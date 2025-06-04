/******************************************************************************
 * Description
 *	Display a color wheel with various options
 *
 * Arguments:
 *
 *  See help below
 *****************************************************************************/


    /*Author:  Burns Fisher, VMS Development
    
    As someone else said in playing around with this file:  This programs
    has undergone about a million iterations, and is not as clear and
    pelucid as one might like.  However...*/


#include <decw$include/Xlib.h>
#include <decw$include/Xatom.h>
#include <decw$include/Xutil.h>
#include <stdio.h>
#include <math>

#ifdef VMS
#define index strchr
#endif

extern GC XCreateGC();
extern long time();
extern long rand();
extern long sys$hiber(),sys$wake();
void hls_to_rgb();
void cmap$GetAndDispatchEvent();

static Display *dpy;
static int chroma = 1;
static Window win;
static GC gc;
static XColor *colors;
static Pixmap backingStore;
static int flush=0,synchronize=0,cmap=0,allall=0,doprimary=7,installCmap=0;
static int setcmap=0,readQueueAction;
static	XExposeEvent *xexev;
static	XConfigureEvent *xConfigEvent;
static	XColormapEvent *xCmapEvent;
static	XRectangle rect;
static	int backW,backH,offsetX,offsetY;
static	int rotate = 1;
static	int nVis;
static	int icoX, icoY;
static	int icoDeltaX, icoDeltaY;
static	int icoW, icoH;
static	int state=0;
static  int verbose=0;
static	int pixel,plane,mapstart,ihop=1 ;
static	XEvent xev;
static	XGCValues xgcv;
#define max_error_size 60
/*
    Handler to report errors.
 */

int error_handler(disp, error_event)

  Display     *disp;
  XErrorEvent *error_event;

  {
    char error_text_buf[max_error_size];

    XGetErrorText(disp, error_event->error_code, error_text_buf, max_error_size);
    if (verbose) printf("\n You received an error packet of '%s'.", error_text_buf);
    return(0);
  }

main(argc, argv)
int argc;
char **argv;
	{
	char *display = NULL;
	char *geom = NULL;
	int useRoot = 0;
        int updateColor;
	int i;
	int winX, winY, winW, winH;
	XSetWindowAttributes xswa;
	XVisualInfo *pVisualInfo,vInfoDummy;
	Colormap map;
	XPoint *pgon;
	int xcent,ycent,ipsize,colorCount=250;
	int jj;

        XSetErrorHandler(error_handler);

	/* Process arguments: */

	ipsize = colorCount;
	while (*++argv)
		{
		if (**argv == '=')                       /* obsolete */
			geom = *argv;
                else if (!strncmp (*argv, "-g", 2))
                        geom = *++argv;
		else if (index(*argv, ':'))
			display = *argv;
		else if (!strcmp(*argv, "-sync"))
			{
			synchronize=1;
			flush=0;
			}

/* Doesn't seem to work, just removed for now...
*
*		else if (!strcmp(*argv, "-r"))
*			useRoot = 1;
*/
		else if (!strcmp(*argv, "-v"))
                        verbose = 1;
                else if (!strcmp(*argv, "-hop"))
			{
			++argv;
			ihop = atoi(*argv);
			}
		else if (!strcmp(*argv, "-primary"))
			{
			++argv;
			doprimary = atoi(*argv);
			}
		else if (!strcmp(*argv, "-psize"))
			{
			++argv;
			ipsize = atoi(*argv);
			}
		else if (!strcmp(*argv, "-colors"))
			{
			++argv;
			colorCount = atoi(*argv);
			}
		else if (!strcmp(*argv, "-flush"))
			{
			flush=1;
			synchronize=0;
			}
		else if (!strcmp(*argv, "-nochroma"))
			{
			chroma=0;
			}
		else if (!strcmp(*argv, "-cmap"))
			{
			cmap=1;
			}
		else if (!strcmp(*argv, "-install"))
			{
			cmap=1;
			installCmap=1;
			}
		else if (!strcmp(*argv, "-setcmap"))
			{
			cmap=1;
			setcmap=1;
			}
		else if (!strcmp(*argv, "-allocall"))
			{
			cmap=1;
			allall=1;
			}
                else usage(); 
		}



	if (!(dpy= XOpenDisplay(display)))
	        {
		perror("Cannot open display\n");
		exit(-1);
	        }


	/* Set up the colormap */

        vInfoDummy.visualid = (DefaultVisual(dpy,0))->visualid;
	pVisualInfo = XGetVisualInfo(dpy,VisualIDMask,&vInfoDummy,&nVis);
	pixel = WhitePixel(dpy,DefaultScreen(dpy));
	updateColor = 0;

	

	if (pVisualInfo->class != PseudoColor &&
	    pVisualInfo->class != DirectColor &&
	    pVisualInfo->class != GrayScale)
	    {

		fprintf(stderr,"Unable to use this visual class");
		exit(0);
	    }
	if (colorCount > pVisualInfo->colormap_size)
	    colorCount = pVisualInfo->colormap_size-2;
	if (cmap)
	    map = XCreateColormap(dpy,RootWindow(dpy,0),DefaultVisual(dpy,0),
		allall?AllocAll:AllocNone);
	else map = XDefaultColormap(dpy,DefaultScreen(dpy));

	if (verbose) printf("Colormap is %x\n",map);
	/* Set up window parameters, create and map window if necessary: */

	backW = backH = 800;

	/* Set up a graphics context: */

	if (useRoot)
		{
		win = RootWindow(dpy, 0);
		winX = 0;
		winY = 0;
		winW = DisplayWidth(dpy, 0);
		winH = DisplayHeight(dpy, 0);
		}
	else
		{
		winW = 600;
		winH = 600;
		winX = (DisplayWidth(dpy, 0) - winW) >> 1;
		winY = (DisplayHeight(dpy, 0) - winH) >> 1;
		XSynchronize(dpy,1);
		if (geom) 
			XParseGeometry(geom, &winX, &winY, &winW, &winH);

		backingStore = XCreatePixmap( dpy ,RootWindow(dpy,0), backW,
					       backH, DefaultDepth(dpy,0));


		xswa.event_mask = ExposureMask | StructureNotifyMask |
		    ColormapChangeMask;
		xswa.background_pixel = BlackPixel(dpy, 0);
		xswa.colormap = map;
		win = XCreateWindow(dpy, RootWindow(dpy, 0), winX, winY, winW, winH, 0, 
		    DefaultDepth(dpy, 0), InputOutput, DefaultVisual(dpy, 0),
		    CWEventMask | CWBackPixel | (setcmap ? 0 : CWColormap),
		    &xswa);
		XChangeProperty(dpy, win, XA_WM_NAME, XA_STRING, 8, 
				PropModeReplace, "ColorWheel Demo", 15);
		}
	gc = XCreateGC(dpy, win, 0, NULL);

	/* Set up aliases for the different kinds of events */

	xexev=(XExposeEvent *) &xev;
	xConfigEvent = (XConfigureEvent *) &xev;
	xCmapEvent = (XColormapEvent *) &xev;

	/* Set up the colormap */

	colors = malloc(sizeof(XColor)*colorCount);
	LoadColormap(colors,map,&colorCount);
	XSynchronize(dpy,synchronize);

	/* Clear the backing store */

	/* Note that even though this may be displayed on a non-default
	   colormap, we will still fill the pixmap with BlackPixel, since
	   we are trying to avoid allocating a pixel value that matches
	   blackpixel.  This is slightly expoiting extra knowledge about
	   the way the DECwindows server works, but everything will still
	   work on another server; it just won't be quite as pretty.  To
	   keep everything strictly on the up and up, we should allcoate
	   explicitly a pixel value for the background. */

	XSetForeground(dpy,gc,BlackPixel(dpy,0));
	XFillRectangle(dpy,backingStore,gc,0,0,backW,backH);

	/* Now set up wheel...actually write the pgons into backing store*/

	pgon = malloc(sizeof(XPoint)*3*colorCount+1);
	xcent=backW/2;
	ycent=backH/2;
	pgon[0].x = backW;
	pgon[0].y = backH/2;
	for (i=0;i<colorCount*3;i+=3)
	    {
	    pgon[i+1].x = xcent;
	    pgon[i+1].y = ycent;
	    }
	for (pixel=0,i=0;  pixel<colorCount; i+=3, pixel++)
	    {
		double x,y,xcent_f,ycent_f;
		xcent_f = (double)xcent;
		ycent_f = (double)ycent;
		x=cos( (((double)pixel+1.)/(double)colorCount)*2.*3.14159);
		y=sin( (((double)pixel+1.)/(double)colorCount)*2.*3.14159);
		pgon[i+2].x = (int)(x*xcent_f)+xcent;
		pgon[i+2].y = (int)(y*ycent_f)+ycent;
		pgon[i+3].x = pgon[i+2].x;
		pgon[i+3].y = pgon[i+2].y;
	    }
	
	fillPolys(pgon,colorCount);
	offsetX = (backW - winW)/2;
	offsetY = (backH - winH)/2;

    /* Set up for events, do the maps, etc to generate them, and
       flush to be sure we get them before we hibernate. */

#ifdef VMS
	XSelectAsyncInput(dpy,win,xswa.event_mask,
					&cmap$GetAndDispatchEvent,0);
#endif
	if (!useRoot) XMapWindow(dpy, win);
	if (setcmap) XSetWindowColormap(dpy,win,map);
/*	XCopyArea(dpy,backingStore,win,gc,
	    offsetX,
	    offsetY,
	    winW,
	    winH,
	    0,
	    0);
*/
	XFlush(dpy);
/******************************************/
/* Start rotating (This is the main loop) */
/******************************************/

	for (mapstart=0;;mapstart = (mapstart+ihop) % colorCount)
	    {
		CopyColors(mapstart,colorCount,colors);
		for (i=0;  i<colorCount && rotate ;  i+=ipsize)
		    {
		    jj=ipsize;
		    if (i+ipsize > colorCount) jj = colorCount-i;
		    XStoreColors(dpy,map,colors+i,jj);
		    if (flush)XFlush(dpy);
		    }

		/*If rotate is false, drop through all these tests and hang
		  at XNextEvent (for the synchronous implementation) or by
		  hibernating (for VMS) until some notification comes through*/

		/*The ifdef is because on VMS we are going to use the ASYNC
		  notification mechanism.  The async notification should also
		  work on ULTRIX, but since I am using the wake/hiber mechanism 
		  to synchronize, this particular code is VMS-only.  It should
		  fallback to conventional serial behavior on Ultrix et al)
		  Note that for VMS we don't check here for anything to do
		  because if the rotate flag is set, all incoming events are
		  dealt with in the AST routine*/

#ifdef VMS
		if(!rotate)
		/*You might think there is a race here if the AST fires between
		  the test and the hiber.  However, if this happens, the wake
		  will have been called in the ast routine and the hiber will
		  complete immediately */
		    sys$hiber();
#else
		if(XEventsQueued(dpy,QueuedAfterReading) || !rotate)
		    {
		    cmap$GetAndDispatchEvent(1);
		    }
#endif
		
	    }
	}



CopyColors(start,count,colors)
int start,count;
XColor *colors;
	{
	    unsigned int i,temp;
	    double h,r,g,b;
	    temp = colors[0].pixel;
	    for (i=0; i<count-1 ;  i++)
		{
		colors[i].pixel = colors[i+1].pixel;
		}
		colors[count-1].pixel = temp;
	}
LoadColormap(colors,map,count)
int *count;
XColor *colors;
Colormap map;
{

    unsigned int i,j,fullcount;
    double h,r,g,b;
    int pixels[256],dummy;

    if (allall)
	for (i=0;i<*count;i++) colors[i].pixel=pixels[i] = i;
    else
	{
	while (!XAllocColorCells(dpy,map,0,&dummy,0,pixels,*count) && *count >= 1)
	    {
	    (*count)--;
	    }
	if (*count < 1)
	    {
	    fprintf(stderr,"Could not allocate enough colors");
	    exit(0);
	    }

	/* This loop find out if we are using WhitePixel and BlackPixel */

	fullcount = *count;
	for (j=0,i=0; i < fullcount; i++)
	    {
	    if (pixels[i]==BlackPixel(dpy,0) || pixels[i]==WhitePixel(dpy,0))
		{
		if (XAllocColorCells(dpy,map,0,&dummy,0,&pixels[i],1))
		    i--; /*Got a replacement value; try it again*/
		else
		    (*count)--; /*No replacement; forget this one*/
		}
	    else
		{
		colors[j++].pixel = pixels[i];
		}
	    }

	if (*count < 1)
	    {
	    fprintf(stderr,"Could not allocate enough colors");
	    exit(0);
	    }
	}
    if (verbose) printf("Using %d colors\n",*count);
    /*This loop fills in the colors*/

    for (i=0; i<*count ; i++)
	{
	colors[i].flags=7;
	colors[i].red = 0;
	colors[i].green = 0;
	colors[i].blue = 0;
	}

    XStoreColors(dpy,map,colors,*count);
    for (i=0; i<*count ; i++)
	{
	colors[i].flags=doprimary;
	colors[i].red = ((i*256)/(*count)) * 256;
	colors[i].green = ((i*256)/(*count))* 256;
	colors[i].blue = ((i*256)/(*count))*256;
	}

    XStoreColors(dpy,map,colors,*count);
    if (installCmap) XInstallColormap(dpy,map);

    if (chroma)
	{
        for (i=0; i<*count ; i++)
	    {
	    h = (double)i * 360./((double)*count+1.);
	    hls_to_rgb(&h,&.5,&.5,&r,&g,&b);
	    colors[i].red = r * 65535.0;
	    colors[i].green = g * 65535.0;
	    colors[i].blue = b * 65535.0;
	    }
	}
    XFlush(dpy);
}
fillPolys(pgon,colorCount)
    XPoint  *pgon;
    int	    colorCount;
{
    int pixel;
    int i,j;
    for (j=i=0;j<colorCount;i+=3,j++)
	{
	XSetForeground(dpy,gc,colors[j].pixel);
	XFillPolygon(dpy,backingStore,gc,&pgon[i],3,Convex,CoordModeOrigin);
	}
    return;
}

/*HLS algorithm from Foley and VanDam.  C implementation thanks to
  some anonymous source at AutoTrol */

void hls_to_rgb (h,l,s, r,g,b)
    double *h,*l,*s,*r,*g,*b;
{
    double m1,m2;
    double value();

    m2 = (*l < 0.5) ? (*l)*(1+*s) : *l + *s- (*l)*(*s) ;
    m1 = 2*(*l) - m2;
     if ( *s == 0 )
      { (*r)=(*g)=(*b)=(*l); }      /*Gray shade*/
    else
      { *r=value(m1,m2,(double)(*h+120.));
	*g=value(m1,m2,(double)(*h+000.));
	*b=value(m1,m2,(double)(*h-120.));
      }
    return;
}

double value (n1,n2,hue)
double n1,n2,hue;
{
      double val;

      if (hue>360.)  hue -= 360.;
      if (hue<0.  )  hue += 360.;

      if (hue<60)
	val = n1+(n2-n1)*hue/60.;
      else if (hue<180.)
	val = n2;
       else if (hue<240.)
	val = n1+(n2-n1)*(240.-hue)/60.;
      else
	val = n1;
      return (val);
}
    

void cmap$GetAndDispatchEvent(arg)
int arg;
/*
  Arg = True means  "It is ok to block on NextEvent"
  Arg = False means "Make sure you don't block on a NextEvent"
*/
{

    /*Drain the queue dry once we get here...this fixes problems of having
      some events in the queue which we don't get notified about; also if
      we get notified but we have already read it, that is ok too*/

    while (arg || XEventsQueued(dpy,QueuedAfterReading))
	{
	XNextEvent(dpy,&xev);
	if (xev.type == ColormapNotify && xexev->window == win)
	    {
	    if (xCmapEvent->new)
		if (verbose) printf("New colormap %x set in window\n",
		xCmapEvent->colormap);
                

	    if (xCmapEvent->state == ColormapUninstalled)
		{
		if (verbose) printf("Colormap %x uninstalled\n",
		    xCmapEvent->colormap);
		rotate = 0;
		}
	    else
		{
		if (verbose) printf("Colormap %x installed\n",
		    xCmapEvent->colormap);
		rotate = 1;
#ifdef VMS
		sys$wake(0,0);
#endif
		}
	    }			    
	if (xev.type == ConfigureNotify && xexev->window == win)
	    {
	    offsetX = (backW - xConfigEvent->width)/2;
	    offsetY = (backH - xConfigEvent->height)/2;
	    }
	if (xev.type == Expose && xexev->window == win)
	    {
	    XCopyArea(dpy,backingStore,win,gc,
		offsetX+(xexev->x),
		offsetY+(xexev->y),
		xexev->width,
		xexev->height,
		xexev->x,
		xexev->y);
	    /* Flush the expose if this is the last rectangle */
	    if (!xexev->count) XFlush(dpy);
	    }
	}
}

usage()
{
        fprintf(stderr, " \n");
        fprintf(stderr, "Usage: ColorWheel [options] server::0\n");
        fprintf(stderr, " \n");
        fprintf(stderr, "-g WxH+X+Y     :Window size and location\n");
        fprintf(stderr, "-nochroma      :Do this with grey shades\n");
        fprintf(stderr, "-cmap          :Use an alternate colormap (else default)\n");
        fprintf(stderr, "-colors n      :How many colors in the wheel\n");
        fprintf(stderr, "-flush         :Flush buffer after color update\n");
        fprintf(stderr, "-hop n         :How much to rotate wheel each step\n");
        fprintf(stderr, "-install       :Install a new colormap\n");
        fprintf(stderr, "-psize n       :How many colors per StoreColor request\n");
        fprintf(stderr, "-primary n     :The doColor mask for storecolors\n");
/*
*       fprintf(stderr, "-r             :Use root instead of new window\n");
*/
        fprintf(stderr, "-setcmap       :Set cmap in window after is is mapped\n");
        fprintf(stderr, "-sync          :Wait for server after each op\n");
        fprintf(stderr, "-allocall      :Allocall on new colormap\n");
        fprintf(stderr, "-v             :Verbose info on colormaps\n");
        fprintf(stderr, " \n");
        exit(1);
}

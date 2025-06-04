/* $Header$ */

/* 
 * xstdcmap.c - create and remove XStandardColormaps in default colormap
 * 
 * Author:	Christopher A. Kent
 * 		Western Software Laboratory
 * 		Digital Equipment Corporation
 * Date:	Fri Feb 16 1990
 * Copyright (c) 1990 Digital Equipment Corporation
 */

/*
 * $Log$
 */

static char rcs_ident[] = "$Header$";

#include <stdio.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

/*
 * This atom is not already defined in Xlib
 */

static Atom	XA_GRAY_DEFAULT_MAP;

/*  program arguments */

char	*display = NULL;
int	nReds, nGreens, nBlues, nGrays;
Bool	removeIt;
Bool	doGamma;
Bool	grayOnly;
int	zero = 0 ;

/* bloody forwards */

Bool	allocContiguousCells();
Status	getStandardColormap();

main(argc, argv)
char	**argv;
{
    Display	*dpy, *OpenDisplay();
    Window	root;
    int		nColorCells;
    Colormap	cmap;
    unsigned long	*pixels;
    XStandardColormap	colorStd, grayStd;
    Visual	vis;
    Bool	status;
    char	c[10];
    GC		gc;
    XID		clientID;

    procargs(argc, argv);

    if (!(dpy = XOpenDisplay(display))) {
	fprintf(stderr, "%s: can't open display %s\n", argv[0], display);
	exit(-1);
    }

    root = RootWindow(dpy, 0);

    /*
     * If the properties already exist, we want to overwrite them, so remove
     * them. If all we're doing is removal, we can exit after this.
     */

    XA_GRAY_DEFAULT_MAP = XInternAtom(dpy, "DEFAULT_GRAY", False);

    if (getStandardColormap(dpy, root, &colorStd, XA_RGB_DEFAULT_MAP)) {
	if(colorStd.killid == 1)	/* in another colormap, just free */
	    XFreeColormap(dpy, colorStd.colormap);
	else if (colorStd.killid != 0)	/* in this colormap */
	    XKillClient(dpy, colorStd.killid);
					/* else can't kill it */
	XDeleteProperty(dpy, root, XA_RGB_DEFAULT_MAP);
    }

    if (getStandardColormap(dpy, root, &grayStd, XA_GRAY_DEFAULT_MAP)) {
	if(grayStd.killid == 1)
	    XFreeColormap(dpy, grayStd.colormap);
	else if ((grayStd.killid != 0) && (grayStd.killid != colorStd.killid))
	    XKillClient(dpy, grayStd.killid);

	XDeleteProperty(dpy, root, XA_GRAY_DEFAULT_MAP);
    }

    if (removeIt) {			/* we're done now */
	XFlush(dpy);
	exit(0);
    }

    cmap = DefaultColormap(dpy, 0);
    gc = XCreateGC(dpy, root, 0, NULL);
    clientID = XGContextFromGC(gc);
    
    if (!grayOnly)
       {
        nColorCells = nReds * nGreens * nBlues;
	pixels = (unsigned long *)malloc(nColorCells * sizeof(unsigned long));
	if (!allocContiguousCells(dpy, cmap, pixels, nColorCells)) {
	    fprintf(stderr, "%s: no room for colors\n", argv[0]);
	    exit(-1);
	    }
	}
    else
	{
	nReds = 1;
	nGreens = 1;
	nBlues = 1;
	pixels = &zero;
	}
    colorStd.red_max = nReds - 1;
    colorStd.red_mult = grayOnly ? 0 : 1;
    colorStd.green_max = nGreens - 1;
    colorStd.green_mult = nReds;
    colorStd.blue_max = nBlues - 1;
    colorStd.blue_mult = nReds * nGreens;

    colorStd.base_pixel = pixels[0];
    colorStd.colormap = cmap;
    colorStd.killid = clientID;
    colorStd.visualid = XVisualIDFromVisual(DefaultVisual(dpy, 0));

    if (!grayOnly) fillInMapRamp(dpy, pixels[0], colorStd);
    XSetRGBColormaps(dpy, root, &colorStd, 1, XA_RGB_DEFAULT_MAP);

    grayStd.red_max = nGrays - 1;
    grayStd.green_max = 0;
    grayStd.green_mult = 0;
    grayStd.blue_max = 0;
    grayStd.blue_mult = 0;

    grayStd.colormap = cmap;
    grayStd.killid = clientID;
    grayStd.visualid = XVisualIDFromVisual(DefaultVisual(dpy, 0));

    if ((nReds == nGrays) && (nReds == nGreens) && (nReds == nBlues)) {  
	/* use diagonal */
	grayStd.base_pixel = colorStd.base_pixel;
	grayStd.red_mult = (nGrays * nGrays) + nGrays + 1;
    } else {
        free(pixels);
        pixels = (unsigned long *)malloc(nGrays * sizeof(unsigned long));
        if (!allocContiguousCells(dpy, cmap, pixels, nGrays)) {
	    fprintf(stderr, "%s: no room for grays\n", argv[0]);
	    exit(-1);
        }
        grayStd.base_pixel = pixels[0];
        grayStd.red_mult = 1;
        fillInGrayRamp(dpy, pixels[0], grayStd);
    }

    XSetRGBColormaps(dpy, root, &grayStd, 1, XA_GRAY_DEFAULT_MAP);

    XFlush(dpy);
    
    XSetCloseDownMode(dpy, RetainPermanent);

    XCloseDisplay(dpy);
    exit(0);
}

/*
 * A slight hack on XGetStandardColormap
 */

static Status
getStandardColormap(dpy, w, cmap, property)
Display	*dpy;
Window	w;
XStandardColormap   *cmap;
Atom	property;
{
    Status  stat;
    XStandardColormap	*stdcmaps;
    int	    nstdcmaps;

    stat = XGetRGBColormaps(dpy, w, &stdcmaps, &nstdcmaps, property);
    if (stat) {
	XStandardColormap   *use;

	if (nstdcmaps > 1) {
	    VisualID	vid;
	    Screen	*sp = XScreenOfDisplay(dpy, 0);
	    int		i;

	    if (!sp) {
		if (stdcmaps)
		    XFree((char *) stdcmaps);
		return False;
	    }
	    vid = sp->root_visual->visualid;

	    for (i = 0; i < nstdcmaps; i++)
		if (stdcmaps[i].visualid == vid)
		    break;

	    if (i == nstdcmaps) {   /* not found */
		XFree((char *) stdcmaps);
		return False;
	    }
	    use = &stdcmaps[i];
	} else {
	    use = stdcmaps;
	}
	*cmap = *use;
    }
    return stat;
}

static Bool
allocContiguousCells(dpy, cmap, pixels, npixels)
Display	    *dpy;
Colormap    cmap;
unsigned long	*pixels;		/* filled in by routine */
int npixels;
{
    unsigned long   *waste = (unsigned long *)NULL;
    int		    nwaste = 0;
    Bool	    contig = False;
    int		    status, i;
    unsigned long   masks = NULL;

    while (!contig) {
        status = XAllocColorCells(dpy, cmap, False, &masks, 0, pixels,
		npixels);
        if (!status)
	    break;   /* can't get enough contiguous cells */
        for (i=0; i < (npixels-1); i++) {
            if (pixels[i] + 1 != pixels[i+1]) {
		/* isn't contiguous, keep trying */
                XFreeColors(dpy, cmap, &pixels[i+1], npixels - (i+1), 0);
		if (!waste) 
		    waste = (unsigned long *)malloc((i + 1) *
				sizeof(unsigned long));
		else
		    waste = (unsigned long *)realloc(waste, (nwaste +
				(i + 1)) * sizeof(unsigned long));
		bcopy(pixels, waste+nwaste, (i+1) * sizeof(unsigned long));
		nwaste += (i+1);
		break;
            }
        }
        if (i == (npixels-1))
	    contig = True;
    }
    /* clean up and return 'contig' */
    if (nwaste) {
        XFreeColors(dpy, cmap, waste, nwaste, 0);
        free(waste);
    }
    return (contig);
}

/*
 *	Fills in a gamma corrected ramp into cmap based on max values
 */

#define ROUND(x)  ((unsigned short)((x) + 0.5))
#define COLORFIX(x) (((x) > 1.0) ? maxColor : ROUND((x) * maxColor))

static
fillInMapRamp(dpy, base_pixel, stdmap)
Display	*dpy;
int	base_pixel;
XStandardColormap	stdmap;
{
    Colormap	cmap = stdmap.colormap;
    int		redmax = stdmap.red_max, 
	        greenmax = stdmap.green_max,
		bluemax = stdmap.blue_max;
    double	red, green, blue;
    XColor	color[1024];
    double      gamma;
    double      igamma;
    double      inc_red, inc_blue, inc_green;
    double      pow();
    int         maxColor = (1 << 16) - 1;
    double      r, g, b;
    int         i;

    gamma = doGamma ? 1.5 : 1.0;
    igamma = 1.0/gamma;
    inc_red = 1.0/redmax;
    inc_green = 1.0/greenmax;
    inc_blue = 1.0/bluemax;
    i = 0;
    for (b=0.0; b <= 1.0001; b += inc_blue) {
	blue = COLORFIX(pow(b, igamma));
	for (g=0.0; g <= 1.0001; g+=inc_green) {
	    green = COLORFIX(pow(g, igamma));
	    for (r=0.0; r <= 1.0001; r+=inc_red) {
		color[i].red = COLORFIX(pow(r, igamma));
		color[i].green = green;
		color[i].blue = blue;
		color[i].pixel = base_pixel + i;
		color[i++].flags = DoRed | DoGreen | DoBlue;
	    }
	}
    }
    XStoreColors(dpy, cmap, color, i);
}

/*
 *	Fills in a gamma corrected gray scale in the passed colormap
 */

static 
fillInGrayRamp(dpy, base_pixel, stdmap)
Display	*dpy;
int	base_pixel;
XStandardColormap	stdmap;
{
    int		max = stdmap.red_max;
    Colormap	cmap = stdmap.colormap;
    XColor	color[1024];
    double      gamma;
    double      igamma;
    double      inc_red;
    double      pow();
    int         maxColor = (1 << 16) - 1;
    double	r;
    int		i;

    gamma = doGamma ? 1.5 : 1.0;
    igamma = 1.0/gamma;
    inc_red = 1.0/max;
    for (r=0.0, i= 0; r <= 1.0001; r += inc_red, i++) {
	color[i].red =
	color[i].green = 
	color[i].blue = COLORFIX(pow(r, igamma));
	color[i].pixel = base_pixel + i;
	color[i].flags = DoRed | DoGreen | DoBlue;
    }
    XStoreColors(dpy, cmap, color, max + 1);
}

procargs(argc, argv)
int argc;
char *argv[];
{
    int arg, skip, unswitched, more;
    char *swptr;

    display = NULL;
    doGamma = True;
    removeIt = False;
    grayOnly = False;
    nReds = -1;
    nGreens = -1;
    nBlues = -1;
    nGrays = -1;

    unswitched = 0;
    skip = 1;
    for ( arg=1 ; arg<argc ; arg += skip ) {
	skip = 1;
	if ( argv[arg][0] == '-' ) {
	    more = 1;
	    swptr = &argv[arg][1];
	    while ( more && *swptr!='\0' ) {
		switch(*swptr++) {
		case 'd':
		    if (arg+skip >= argc)
			usagexit(argv[0]);
		    display = argv[arg+skip];
		    skip++;
		    break;
		case 'g':
		    doGamma = False;
		    break;
		case 'r':
		    removeIt = True;
		    break;
		case 'm':
		    grayOnly = True;
		    if (unswitched > 1) usagexit(argv[0]);
		    nGrays = nReds;
		    break;
		default:
		    usagexit(argv[0]);
		}
	    }
	} else { /* there's no dash in front */
	    switch ( unswitched++ ) {
	    case 0:
		if (grayOnly) nGrays = atoi(argv[arg]);
		else nReds = atoi(argv[arg]);
		break;
	    case 1:
		if (grayOnly) usagexit(argv[0]);
		nGreens = atoi(argv[arg]);
		break;
	    case 2:
		if (grayOnly) usagexit(argv[0]);
		nBlues = atoi(argv[arg]);
		break;
	    case 3:
		if (grayOnly) usagexit(argv[0]);
		nGrays = atoi(argv[arg]);
		break;
	    default:
		if (!removeIt)
		    usagexit(argv[0]);
	    }
	}
    }
    if (removeIt)
	return;
    if (grayOnly)
	if (nGrays < 2) usagexit(argv[0]);
	else return;
    if ((nReds < 2) || (nGreens < 2) || (nBlues < 2) || (nGrays < 2))
	usagexit(argv[0]);
    if (nReds < 1)
	nReds = 1;
    if (nGreens < 1)
	nGreens = 1;
    if (nBlues < 1)
	nBlues = 1;
    if (nGrays < 2)
	nGrays = 2;
}

usagexit(pgm)
char *pgm;
{
	fprintf(stderr,"usage: %s [-d display] [-r] [-g]",pgm);
	fprintf(stderr, " nReds nGreens nBlues nGrays\n");
	fprintf(stderr, "\t(-r just removes the properties)\n");
	fprintf(stderr, "\t(-g turns off gamma correction)\n");
	fprintf(stderr, "\t(-m does monochrome.  Only specify nGrays)\n");
	exit(1);
}

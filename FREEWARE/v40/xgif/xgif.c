/*
 * xgif.c - displays GIF pictures on an X11 display
 *
 *  Author:    John Bradley, University of Pennsylvania
 *                (bradley@cis.upenn.edu)
 *
 *  1/24/89	Ben Thomas
 *		Quickly hack in support for both 24 and 8 plane systems.
 *  9/19/91    Jeff McLeman
 *              Hack for greyscale
 *  11/04/91    Allow a -gs parameter to display greyscale on color servers
 *  03/31/92    Port to alpha
 */

#define MAIN
#include "xgif.h"

extern Bool Verbose;

/*******************************************/
main(argc, argv)
    int   argc;
    char *argv[];
/*******************************************/
{
    int        i;
    int greyscale;
    int        pause_window = False;
    char      *display, *geom, *fname;
    XEvent     event;

    XVisualInfo vinfo, *vinfos;
    int win_depth;

    cmd = argv[0];
    display = geom = fname = NULL;
    expImage = NULL;

    expand = 1;  strip = 0;  nostrip = 0; greyscale=0;

    /*********************Options*********************/

    for (i = 1; i < argc; i++) {
        char *strind;

        if (!strncmp(argv[i],"-g",2)) {		/* geometry */
            i++;
            geom = argv[i];
            continue;
            }

        if (argv[i][0] == '=') {		/* old-style geometry */
            geom = argv[i];
            continue;
            }

        if (!strncmp(argv[i],"-p",2)) {		/* pause window */
            pause_window = True;
            continue;
            }

        if (!strncmp(argv[i],"-v",2)) {		/* verbose */
            i++;
            Verbose = 1;
            continue;
            }

        if (!strncmp(argv[i],"-d",2)) {		/* display */
            i++;
            display = argv[i];
            continue;
            }

	if (display) {
            strind = strchr(argv[i], ':');	/* old-style display */

            if(strind != NULL) {
                display = argv[i];
                continue;
                }
          }

        if (!strcmp(argv[i],"-e")) {		/* expand */
            i++;
            expand=atoi(argv[i]);
            continue;
            }

        if (!strcmp(argv[i],"-s")) {		/* strip */
            i++;
            strip=atoi(argv[i]);
            continue;
            }

        if (!strcmp(argv[i],"-ns")) {		/* nostrip */
            nostrip++;
            continue;
            }

        if (!strcmp(argv[i],"-gs")) {		/* greyscale */
            greyscale++;
            continue;
            }

        if (argv[i][0] != '-') {		/* the file name */
            fname = argv[i];
            continue;
            }

        Syntax(cmd);
    }

    /* If the pause_window was requested, use maximum expansion */

    if (pause_window == True) expand = MAXEXPAND;

    if (fname==NULL) fname="-";
    if (expand<1 || expand>MAXEXPAND) Syntax(cmd);
    if (strip<0 || strip>7) Syntax(cmd);

    /*****************************************************/

    /* Open up the display. */

    if ( (theDisp=XOpenDisplay(display)) == NULL) {
        fprintf(stderr, "%s: Can't open display\n",argv[0]);
        exit(1);
        }

    theScreen = DefaultScreen(theDisp);
    theCmap   = DefaultColormap(theDisp, theScreen);
    rootW     = RootWindow(theDisp,theScreen);
    theGC     = DefaultGC(theDisp,theScreen);
    fcol      = WhitePixel(theDisp,theScreen);
    bcol      = BlackPixel(theDisp,theScreen);

/* Get matching 8 plane pseudocolor visual */

    vinfo.class  = PseudoColor;
    vinfo.depth  = 8;
    vinfo.screen = theScreen;

    vinfos = (XVisualInfo *)
	     XGetVisualInfo(theDisp,
			    /* XXX ignoring rgb mask differences */
			    VisualScreenMask,	/* |VisualDepthMask */
			    &vinfo,
			    &i);
    if (i > 0) {
       if (greyscale) {

	        theVisual = GrayScale;
   		win_depth = vinfos[0].depth;
               }
         else
                {
	        theVisual = vinfos[0].visual;
   		win_depth = vinfos[0].depth;
		}
    } 
else {
	fprintf(stderr, "XGIF: could not find matching visual.\n");
	fprintf(stderr, "depth: %d, class: %d\n",
		vinfo.depth, vinfo.class);
	FatalError("exiting.\n");
    }

    if (theVisual != DefaultVisual(theDisp,theScreen)) {
	theCmap = XCreateColormap(theDisp,rootW,theVisual,AllocNone);
        XInstallColormap(theDisp,theCmap);
    }

    dispcells = DisplayCells(theDisp, theScreen);
    if (dispcells<=2) 
        FatalError("This program requires a color display, pref. 8 bits.");


    /****************** Open/Read the File  *****************/
    LoadGIF(fname);
    iWIDE = theImage->width;  iHIGH = theImage->height;

    eWIDE = iWIDE * expand;  eHIGH = iHIGH * expand;
    if (eWIDE > DisplayWidth(theDisp,theScreen)) 
        eWIDE = DisplayWidth(theDisp,theScreen);
    if (eHIGH > DisplayHeight(theDisp,theScreen)) 
        eHIGH = DisplayHeight(theDisp,theScreen);

    /**************** Create/Open X Resources ***************/
    if ((mfinfo = XLoadQueryFont(theDisp,"variable"))==NULL)
       FatalError("couldn't open 'variable' font\n");
    mfont=mfinfo->fid;
    XSetFont(theDisp,theGC,mfont);
    XSetForeground(theDisp,theGC,fcol);
    XSetBackground(theDisp,theGC,bcol);

    CreateMainWindow(cmd,geom,argc,argv,pause_window,win_depth);
    Resize(eWIDE,eHIGH);
    XSelectInput(theDisp, mainW, ExposureMask | KeyPressMask 
                               | StructureNotifyMask);
    XMapWindow(theDisp,mainW);

    /* If this is a pause window, then mimic an expose event.  Otherwise
       the user would have to wait before pausing the window */

    if (pause_window == True)
         XPutImage(theDisp,mainW,theGC,expImage,0,0,0,0,eWIDE,eHIGH);

    /**************** Main loop *****************/
    while (1) {
        XNextEvent(theDisp, &event);
        HandleEvent(&event);
        }
}



/****************/
HandleEvent(event)
    XEvent *event;
/****************/
{
    switch (event->type) {
        case Expose: {
            XExposeEvent *exp_event = (XExposeEvent *) event;

            if (exp_event->window==mainW) 
                DrawWindow(exp_event->x,exp_event->y,
                           exp_event->width, exp_event->height);
            }
            break;

        case KeyPress: {
            XKeyEvent *key_event = (XKeyEvent *) event;
            char buf[128];
            KeySym ks;
            XComposeStatus status;

            XLookupString(key_event,buf,128,&ks,&status);
            if (buf[0]=='q' || buf[0]=='Q') Quit();
            }
            break;

        case ConfigureNotify: {
            XConfigureEvent *conf_event = (XConfigureEvent *) event;

            if (conf_event->window == mainW && 
                 (conf_event->width != eWIDE || conf_event->height != eHIGH))
                Resize(conf_event->width, conf_event->height);
            }
            break;


        case CirculateNotify:
        case MapNotify:
        case DestroyNotify:
        case GravityNotify:
        case ReparentNotify:
        case UnmapNotify:       break;

        default:		/* ignore unexpected events */
	  break;
        }  /* end of switch */
}


/***********************************/
Syntax()
{
    printf("\nUsage: %s\n",cmd);
    printf("\n       filename\n");
    printf("\n       [[-geometry] geom]    Window Geometry");
    printf("\n       [[-display] display]  Display name if other than default");
    printf("\n       [-e 1..%d]            Expansion factor",MAXEXPAND);
    printf("\n       [-s 0-7] [-ns]        Stripping factor, or nostrip");
    printf("\n       [-p]                  Pause window - maximum expansion\n");
    printf("\n       [-v]                  Verbose\n");
    exit(1);
}


/***********************************/
FatalError (identifier)
       char *identifier;
{
    fprintf(stderr, "%s:  %s\n",cmd, identifier);
    exit(-1);
}


/***********************************/
Quit()
{
    exit(0);
}


/***********************************/
DrawWindow(x,y,w,h)
{
    XPutImage(theDisp,mainW,theGC,expImage,x,y,x,y,w,h);
}


/***********************************/
Resize(w,h)
int w,h;
{
    int  ix,iy,ex,ey;
    byte *ximag,*ilptr,*ipptr,*elptr,*epptr;
    static char *rstr = "Resizing Image.  Please wait...";

    /* warning:  this code'll only run machines where int=32-bits */

    if (w==iWIDE && h==iHIGH) {		/* very special case */
        if (expImage != theImage) {
            if (expImage) XDestroyImage(expImage);
            expImage = theImage;
            eWIDE = iWIDE;  eHIGH = iHIGH;
            }
        }

    else {				/* have to do some work */
        /* if it's a big image, this'll take a while.  mention it */
        if (w*h>(500*500)) {
            XDrawImageString(theDisp,mainW,theGC,CENTERX(mfinfo,w/2,rstr),
                  CENTERY(mfinfo,h/2),rstr, strlen(rstr));
            XFlush(theDisp);
            }

	/* first, kill the old expImage, if one exists */
	if (expImage && expImage != theImage) {
            free(expImage->data);  expImage->data = NULL;
            XDestroyImage(expImage);
            }

        /* create expImage of the appropriate size */
        
        eWIDE = w;  eHIGH = h;
        ximag = (byte *) malloc(w*h);
        expImage = XCreateImage(theDisp,theVisual,8,ZPixmap,0,ximag,
                        eWIDE,eHIGH,8,eWIDE);

        if (!ximag || !expImage) {
            fprintf(stderr,"ERROR: unable to create a %dx%d image\n",w,h);
            exit(0);
            }

        elptr = epptr = (byte *) expImage->data;

        for (ey=0;  ey<eHIGH;  ey++, elptr+=eWIDE) {
            iy = (iHIGH * ey) / eHIGH;
            epptr = elptr;
            ilptr = (byte *) theImage->data + (iy * iWIDE);
            for (ex=0;  ex<eWIDE;  ex++,epptr++) {
                ix = (iWIDE * ex) / eWIDE;
                ipptr = ilptr + ix;
                *epptr = *ipptr;
                }
            }
        }
}

/***********************************/

/* Find the session manager's pause window */
                
Window SmPauseWindow(dpy, root)
    Display *dpy;
    Window root;
{
    Window r2;
    Window parent;
    Window *child;
    Window *child1;
    Window *child2;
    unsigned int nchildren, nc1;
    XWindowAttributes rootatt, childatt;
    int x, y, w, h, bw, d;
    
    if (!XGetWindowAttributes(dpy, root, &rootatt))
	return 0;

    if (XQueryTree(dpy, root, &r2, &parent, &child, &nchildren))
        {
	int i;
	
	for (i = 0; i < nchildren; i++)
	    {
	    if (!XGetWindowAttributes(dpy, child[i], &childatt))
		return 0;

	    if (childatt.width == rootatt.width - 6 &&
	        childatt.height == rootatt.height - 6 &&
	        childatt.x == 0 &&
	        childatt.y == 0)
		{
		if (!XQueryTree(dpy, child[i], &r2, &parent, &child1, &nc1))
		    return 0;

		if (nc1 == 1)
		    {
		    if (!XGetWindowAttributes(dpy, child1[0], &childatt))
			return 0;

		    if (!XQueryTree(dpy, child1[0], &r2, &parent, &child2,
			    &nc1))
			return 0;

		    if (childatt.width == rootatt.width - 6 &&
			childatt.height == rootatt.height - 6 &&
			childatt.x == 0 &&
			childatt.y == 0 &&
			nc1 == 2)
			return child1[0];
		    }
		}
	    }
        }

    /* If we get here, we can't find the proper window */
    
    return 0;
}

/***********************************/
CreateMainWindow(name,geom,argc,argv,pause_window,win_depth)
    char *name,*geom,**argv;
    int   argc,pause_window;
    int	  win_depth;

{
    XSetWindowAttributes xswa;
    unsigned int xswamask;
    XSizeHints hints;
    int i,x,y,w,h;

    x=y=w=h=1;
    i=XParseGeometry(geom,&x,&y,&w,&h);
    if (i&WidthValue)  eWIDE = w;
    if (i&HeightValue) eHIGH = h;

    if (i&XValue || i&YValue) hints.flags = USPosition;  
                         else hints.flags = PPosition;

    hints.flags |= USSize;

    if (i&XValue && i&XNegative) 
        x = XDisplayWidth(theDisp,theScreen)-eWIDE-abs(x);
    if (i&YValue && i&YNegative) 
        y = XDisplayHeight(theDisp,theScreen)-eHIGH-abs(y);

    hints.x=x;             hints.y=y;
    hints.width  = eWIDE;  hints.height = eHIGH;
    hints.max_width = DisplayWidth(theDisp,theScreen);
    hints.max_height = DisplayHeight(theDisp,theScreen);
    hints.flags |= PMaxSize;

    xswa.background_pixel = bcol;
    xswa.border_pixel     = fcol;
    xswa.colormap	  = theCmap;
    xswamask = CWBackPixel | CWBorderPixel | CWColormap;

    /* If the pause window was requested, find it.  Otherwise make our own */

    if (pause_window == True)
         {
         mainW = SmPauseWindow(theDisp, rootW);
         while (!mainW)
         	mainW = SmPauseWindow(theDisp, rootW);
         }
    else mainW = XCreateWindow(theDisp, rootW, x, y, eWIDE, eHIGH, 0, win_depth,
                          InputOutput, theVisual, xswamask, &xswa);

    theGC	= XCreateGC(theDisp,mainW,0,0);

    XSetStandardProperties(theDisp,mainW,"xgif","xgif",None,
                            argv,argc,&hints);

    if (!mainW) FatalError("Can't open main window");

}


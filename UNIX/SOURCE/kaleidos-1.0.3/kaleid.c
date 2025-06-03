/* Horrible kludge by Greg Lehey, LEMIS, 7 May 1993:
 * the standard USL header file sys/signal.h only defines sigaction if
 * __STDC__ is set to 0. In gcc, it is set to 1, so... */
#ifdef __GNUC__
#undef __STDC__
#define __STDC__ 0
#endif

#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include "kalicon.h"
#include <signal.h>

/*

  KALEIDOSCOPE (X11 Version)

  By Nathan Meyers, nathanm@hp-pcd.hp.com.

  Based on a kaleidoscope algorithm from a PC-based program by:
    Judson D. McClendon (Compuserve: [74415,1003])

  
  Modified by Laurent JULLIARD, laurentj@hpgnse2.grenoble.hp.com
    for LINUX 0.96 and X11 v1.0 shared library
    [compile with -DLINUX]
*/

char *progname;

char *strchr();
char *calloc();

/* Approximation of CGA/EGA/VGA colors -- (out of standard order) */
unsigned long colors[][3] = { { 0x0000, 0x0000, 0x0000 },    /* black */
			      { 0xffff, 0xffff, 0xffff },    /* bright white */
			      { 0xaaaa, 0x0000, 0x0000 },    /* red */
			      { 0x0000, 0xaaaa, 0x0000 },    /* green */
			      { 0x0000, 0x0000, 0xaaaa },    /* blue */
			      { 0x0000, 0xaaaa, 0xaaaa },    /* cyan */
			      { 0xaaaa, 0x0000, 0xaaaa },    /* magenta */
			      { 0xffff, 0xffff, 0x5555 },    /* yellow */
			      { 0xaaaa, 0x5555, 0x0000 },    /* brown */
			      { 0xaaaa, 0xaaaa, 0xaaaa },    /* white  */
			      { 0x5555, 0x5555, 0x5555 },    /* dark grey */
			      { 0x5555, 0x5555, 0xffff },    /* light blue */
			      { 0x5555, 0xffff, 0x5555 },    /* light green */
			      { 0x5555, 0xffff, 0xffff },    /* light cyan */
			      { 0xffff, 0x5555, 0x5555 },    /* light red */
			      { 0xffff, 0x5555, 0xffff } };  /* light magenta */

#define NCOLORS 16

static char *what = "@(#)kaleid.c $Revision: 1.98 $";

Display *display;
int screen;
Window *window;
int nwindows=1, nvisible=0;
unsigned long palette[NCOLORS];
GC *window_gcs;
int nclips = 0;
XRectangle *cliplist;
struct fclip {
  double x, y, width, height;
} *clips;

int *visible;
int *CX, *CY, *M;
int *OX, *OY;
int *X1, *Y1, *X2, *Y2, *XV1, *YV1, *XV2, *YV2;
int *XA, *YA, *XB, *YB, *XC, *YC, *XD, *YD;
int qix = 0;
int D[] = { 2, 4, 4, 4, 4, 2, -2, -4, -4, -4, -4, -2, 2, 4, 4, 4, 4, 2 };
struct timeval *schedule;
long *intervals;
int *refreshcount, *itercount;
unsigned short (*randomseed)[3], (*randombase)[3];
int numcolors=NCOLORS;
int dynamic_colors=0;
long refresh=0L;
long rndm();
int rootx=1, rooty=1;
int ww, wh;
int doroot = 0;
int killed = 0;
char *ucolors = NULL;

void sighandler()
{
  killed=1;
  return;
}

void refreshrootquit()
{
  XClearWindow(display,RootWindow(display,screen));
  XSync(display,0);
  exit(0);
}

main(argc,argv)
int argc;
char *argv[];
{
  unsigned long bgcolor, bdcolor;
  int bwidth = 2;
  XSizeHints size_hints;
  XIconSize icon_size_hints, *icon_size_list;
  int numsizes, currsize;
  XWMHints wm_hints;
  char *displayname = NULL, *background = NULL, *border = NULL;
  char *geomstring = NULL;
  XColor screen_in_out, visual_def_return, exact_def_return;
  XEvent event;
  int iconic=0, bstore=0;
  long delayvalue = -1L, icondelay=100L;
  char *clipcoords = NULL;
  int monochrome=0;
  int o_argc = argc;
  char **o_argv = argv;
  int i,j;
  long time();
  char *basename;
  long atol();

  progname = argv[0];

  if ((basename=strrchr(progname,'/'))!=NULL) basename++;
  else basename=progname;

  while (--argc>0) {
    char *option = (*++argv);
    if (!strcmp(option,"-delay")) {
      if (--argc==0) usage();
      delayvalue = atol(*++argv);
      if (delayvalue<0L) delayvalue = 0L;
    }
    else if (!strcmp(option,"-qix")) {
      qix=1;
    }
    else if (!strcmp(option,"-mono")) {
      monochrome=1;
    }
    else if (!strcmp(option,"-randomcolor")) {
      dynamic_colors = 1;
    }
    else if (!strcmp(option,"-display")) {
      if (--argc==0) usage();
      displayname = (*++argv);
    }
    else if (strchr(option,':')) {
      displayname = option;
    }
    else if (!strcmp(option,"-bg")) {
      if (--argc==0) usage();
      background = (*++argv);
    }
    else if (!strcmp(option,"-bd")) {
      if (--argc==0) usage();
      border = (*++argv);
    }
    else if (!strcmp(option,"-bstore")) {
      bstore = 1;
    }
    else if (!strcmp(option,"-bw")) {
      if (--argc==0) usage();
      bwidth = atoi(*++argv);
      if (bwidth<0) bwidth = 0;
    }
    else if (!strcmp(option,"-colors")) {
      if (--argc==0) usage();
      ucolors = (*++argv);
    }
    else if (!strcmp(option,"-clip")) {
      if (--argc==0) usage();
      clipcoords = (*++argv);
    }
    else if (!strcmp(option,"-icondelay")) {
      if (--argc==0) usage();
      icondelay = atol(*++argv);
      if (icondelay<0L) icondelay = 0L;
    }
    else if (!strcmp(option,"-iconic")) {
      iconic = 1;
    }
    else if (!strcmp(option,"-geometry")) {
      if (--argc==0) usage();
      geomstring = (*++argv);
    }
    else if (*option=='=') {
      geomstring = option;
    }
    else if (!strcmp(option,"-mult")) {
      if (--argc==0) usage();
      nwindows = atoi(*++argv);
      if (nwindows<1) nwindows = 1;
    }
    else if (!strcmp(option,"-r")) {
      doroot = 1;
    }
    else if (!strcmp(option,"-refresh")) {
      refresh = ExposureMask;
    }
    else if (!strcmp(option,"-root")) {
      doroot = 1;
    }
    else usage();
  }

if (monochrome) dynamic_colors = 0;

  if (delayvalue == -1L) delayvalue = doroot ? 100L : 10L;

  display = XOpenDisplay(displayname);
  if (display==NULL) {
    (void)fprintf(stderr,
	          (displayname==NULL) ?
	          "%s: Failed to open display.\n" :
	          "%s: Failed to open display %s.\n",
	          progname,displayname);
    exit(1);
  }
  screen = DefaultScreen(display);

  srand48(time((long *)NULL));

  if (background == NULL ||
      XAllocNamedColor(display,
		       DefaultColormap(display,screen),
		       background,
		       &visual_def_return,
		       &exact_def_return)==False)
    bgcolor = BlackPixel(display,screen);
  else bgcolor = exact_def_return.pixel;

  if (border==NULL ||
      XAllocNamedColor(display,
		       DefaultColormap(display,screen),
		       border,
		       &visual_def_return,
		       &exact_def_return)==False)
   bdcolor = WhitePixel(display,screen);
  else bdcolor = exact_def_return.pixel;

  if (clipcoords != NULL) {
    char *temp = clipcoords;
    char *calloc();
    double strtod();
    int commacount = 0;
    int len;
    while (*temp != '\0') commacount += (*(temp++) == ',') ? 1 : 0;
    if ((commacount & 0x3) != 3) usage();
    nclips = (commacount >> 2) + 1;
    clips = (struct fclip *)calloc(nclips, sizeof(struct fclip));
    cliplist = (XRectangle *)calloc(nclips, sizeof(XRectangle));
    if (clips == NULL || cliplist == NULL) {
      fprintf(stderr,"%s: calloc() failed\n",progname);
      exit(1);
    }
    for (i=0; i<nclips; i++) {
      clips[i].x = strtod(clipcoords, &temp);
      if (temp == clipcoords) usage();
      clipcoords = strchr(temp, ',') + 1;
      clips[i].y = strtod(clipcoords, &temp);
      if (temp == clipcoords) usage();
      clipcoords = strchr(temp, ',') + 1;
      clips[i].width = strtod(clipcoords, &temp);
      if (temp == clipcoords) usage();
      clipcoords = strchr(temp, ',') + 1;
      clips[i].height = strtod(clipcoords, &temp);
      if (temp == clipcoords) usage();
      clipcoords = strchr(temp, ',') + 1;
    }
  }

  if (!monochrome && DisplayCells(display,screen) > 2) {
    if (dynamic_colors) {
      for (numcolors=NCOLORS; numcolors>=2; numcolors--) {
	if (XAllocColorCells(display,DefaultColormap(display,screen),
			     0, (unsigned long *)NULL, 0,
			     palette, (unsigned int)numcolors) == True) {
	  randomize_colors();
	  break;
	}
      }
      if (numcolors < 2) fatalerror("Cannot allocate R/W color cells",NULL);
    }
    else if (ucolors != NULL) {
      int thiscolor;
      numcolors = 0;
      for (thiscolor=0; thiscolor<NCOLORS; thiscolor++) {
	char *comma=strchr(ucolors,',');

	if (comma != NULL) *comma='\0';
	if (XAllocNamedColor(display,
		             DefaultColormap(display,screen),
		             ucolors,
		             &visual_def_return,
		             &exact_def_return) == True)
          palette[numcolors++] = exact_def_return.pixel;
	if (comma == NULL) break;
	ucolors = comma + 1;
      }
      if (numcolors < 1) fatalerror("Cannot allocate colors",NULL);
    }
    else {
      int thiscolor;
      numcolors = 0;
      for (thiscolor=0; thiscolor<NCOLORS; thiscolor++) {
        screen_in_out.flags = DoRed | DoGreen | DoBlue;
        screen_in_out.red = colors[thiscolor][0];
        screen_in_out.green = colors[thiscolor][1];
        screen_in_out.blue = colors[thiscolor][2];
        if (XAllocColor(display, DefaultColormap(display,screen),
		        &screen_in_out)==True)
          palette[numcolors++] = screen_in_out.pixel;
      }
      if (numcolors < 2) fatalerror("Cannot allocate colors",NULL);
    }
  }
  else {
    numcolors=2;
    palette[0] = WhitePixel(display,screen);
    palette[1] = BlackPixel(display,screen);
  }

  size_hints.x = 0;
  size_hints.y = 0;
  size_hints.width = 300;
  size_hints.height = 300;

  size_hints.flags = PPosition | PSize;
  if (geomstring!=NULL) {
    int result;
    result = XParseGeometry(geomstring,&size_hints.x,&size_hints.y,
		            &size_hints.width,&size_hints.height);
    if (result & XNegative)
      size_hints.x += DisplayWidth(display,screen)
		    - size_hints.width
		    - bwidth*2;
    if (result & YNegative)
      size_hints.y += DisplayHeight(display,screen)
		    - size_hints.height
		    - bwidth*2;
    if (result & XValue || result & YValue) {
      size_hints.flags |= USPosition;
      size_hints.flags &= ~PPosition;
    }
    if (result & WidthValue || result & HeightValue) {
      size_hints.flags |= USSize;
      size_hints.flags &= ~PSize;
    }
    if (result & WidthValue && !(rootx = size_hints.width)) rootx = 1;
    if (result & HeightValue && !(rooty = size_hints.height)) rooty = 1;
  }

  if (doroot) {
    nwindows=rootx*rooty;
    if (!allocate_arrays(nwindows)) {
      (void)fprintf(stderr,"%s: malloc() failure\n",progname);
      exit(1);
    }
    for (i=0; i<nwindows; i++) {
      intervals[i] = delayvalue;
      window[i] = RootWindow(display,screen);
      visible[i]=1;
    }
    nvisible=nwindows;
    XSelectInput(display,window[0],refresh);
  }
  else {
    Pixmap bitmapicon;

    if (!allocate_arrays((unsigned)(2*nwindows))) {
      (void)fprintf(stderr,"%s: malloc() failure\n",progname);
      exit(1);
    }

    for (i=0; i<nwindows; i++)
      window[i] = XCreateSimpleWindow(display,RootWindow(display,screen),
				        size_hints.x,size_hints.y,
				        size_hints.width,size_hints.height,
				        bwidth,bdcolor,bgcolor);

    icon_size_hints.max_width=64;
    icon_size_hints.max_height=64;
    currsize=0;
    if (XGetIconSizes(display,RootWindow(display,screen),
		      &icon_size_list,&numsizes)) {
      for (i=1; i<numsizes; i++) {	/* Look for largest icon size */
	if (icon_size_list[i].max_width >= icon_size_list[currsize].max_width &&
	    icon_size_list[i].max_height >= icon_size_list[currsize].max_height)
	  currsize=i;
      }
      if (icon_size_list[currsize].max_width <= 0 ||
	  icon_size_list[currsize].max_height <= 0 ) {
	XFree(icon_size_list);
        icon_size_list = &icon_size_hints;
      }
    }
    else
      icon_size_list = &icon_size_hints;

    for (i=0; i<nwindows; i++) {
      window[i+nwindows] = XCreateSimpleWindow(display,
					  RootWindow(display,screen), 0,0,
				          icon_size_list[currsize].max_width,
				          icon_size_list[currsize].max_height,
				          2,BlackPixel(display,screen),
				          WhitePixel(display,screen));

      if (qix) {
        CX[i+nwindows] = icon_size_list[currsize].max_width;
        CY[i+nwindows] = icon_size_list[currsize].max_height;
      }
      else {
        CX[i+nwindows] = icon_size_list[currsize].max_width >> 1;
        CY[i+nwindows] = icon_size_list[currsize].max_height >> 1;
        M[i+nwindows] = (CX[i+nwindows]>CY[i+nwindows]) ?
			   CX[i+nwindows] :
			   CY[i+nwindows];
        M[i+nwindows] = M[i+nwindows] ? M[i+nwindows] : 1;
      }
    }

    if (bstore) {
      XSetWindowAttributes attributes;
      attributes.backing_store = WhenMapped;
      for (i=0; i<2*nwindows; i++) {
	XChangeWindowAttributes(display,window[i],CWBackingStore,&attributes);
      }
    }

    for (i=0; i<nwindows; i++)
      XSetStandardProperties(display,window[i],"Kaleidoscope",basename,
			     None,o_argv,o_argc,&size_hints);

    /* Create bitmap icon for wm's that don't support window icons */
    bitmapicon=XCreateBitmapFromData(display,RootWindow(display,screen),
				     icon_bits, icon_width, icon_height);

    /* Use window icon for window managers that support one */
    wm_hints.icon_pixmap = bitmapicon;
    wm_hints.flags = IconPixmapHint | IconWindowHint;

    if (iconic) {
      wm_hints.initial_state = IconicState;
      wm_hints.flags |= StateHint;
    }

    for (i=0; i<nwindows; i++) {
      wm_hints.icon_window = window[i+nwindows];
      XSetWMHints(display,window[i],&wm_hints);
      XSelectInput(display,window[i],
	StructureNotifyMask|VisibilityChangeMask|refresh);
      XSelectInput(display,window[i+nwindows],
	StructureNotifyMask|VisibilityChangeMask|refresh);
      XMapWindow(display,window[i]);

      intervals[i] = delayvalue;
      intervals[i+nwindows] = icondelay;

      if (qix) {
        CX[i] = size_hints.width;
        CY[i] = size_hints.height;
      }
      else {
        CX[i] = size_hints.width/2;
        CY[i] = size_hints.height/2;
        M[i] = (CX[i]>CY[i]) ? CX[i] : CY[i];
        M[i] = M[i] ? M[i] : 1;
      }
    }

  }

  for (i = 0; i<(doroot ? nwindows : nwindows << 1 ); i++)
    window_gcs[i] = XCreateGC(display,window[i],0,(XGCValues *)NULL);

  if (doroot) {
    int dontcare, winno;
    Window wdontcare;
#if defined (LINUX) || defined (SYSV)
    struct sigaction vec;
#else
    struct sigvec vec;
#endif

    XGetGeometry(display,RootWindow(display,screen),&wdontcare,&dontcare,
                 &dontcare, &ww, &wh, &dontcare, &dontcare);
    for (winno=0; winno<nwindows; winno++) {
      i = winno % rootx;
      j = winno / rootx;
      CX[winno] = ww/rootx;
      CY[winno] = wh/rooty;
      OX[winno] = i * CX[winno];
      OY[winno] = j * CY[winno];
      if (!qix) {
        CX[winno] >>= 1;
        CY[winno] >>= 1;
        M[winno] = (CX[winno]>CY[winno]) ? CX[winno] : CY[winno];
        M[winno] = M[winno] ? M[winno] : 1;
      }
      if (nclips) {
	int k;
	double frootx = (double)rootx, frooty = (double)rooty;
	double fww = (double)ww, fwh = (double)wh;
	for (k=0; k<nclips; k++) {
	  cliplist[k].x = (int)(fww * clips[k].x / frootx + .5);
	  cliplist[k].y = (int)(fwh * clips[k].y / frooty + .5);
	  cliplist[k].width = (int)(fww * clips[k].width / frootx + .5);
	  cliplist[k].height = (int)(fwh * clips[k].height / frooty + .5);
	}
        XSetClipRectangles(display, window_gcs[winno], OX[winno], OY[winno],
			   cliplist, nclips, Unsorted);
      }
    }
    for (i=0; i<nwindows; i++) {
      refreshcount[i]=0;
      itercount[i]=0;
      randombase[i][0]=randomseed[i][0];
      randombase[i][1]=randomseed[i][1];
      randombase[i][2]=randomseed[i][2];
      randomize_state_vars(i);
    }
#if defined (LINUX) || defined (__STDC__)
    vec.sa_handler = sighandler;
    vec.sa_flags = 0;
    (void)sigaction(SIGINT, &vec, (struct sigaction *)NULL);
    (void)sigaction(SIGQUIT, &vec, (struct sigaction *)NULL);
    (void)sigaction(SIGTERM, &vec, (struct sigaction *)NULL);            
#else
    vec.sv_handler = sighandler;
    vec.sv_mask = 0x0;
    vec.sv_flags = 0;
    (void)sigvec(SIGINT, &vec, (struct sigvec *)NULL);
    (void)sigvec(SIGQUIT, &vec, (struct sigvec *)NULL);
    (void)sigvec(SIGTERM, &vec, (struct sigvec *)NULL);
#endif

  }
  else {
    for (i=0; i<nclips; i++) {
      cliplist[i].x = (int)((double)size_hints.width * clips[i].x + .5);
      cliplist[i].y = (int)((double)size_hints.height * clips[i].y + .5);
      cliplist[i].width = (int)((double)size_hints.width * clips[i].width + .5);
      cliplist[i].height = (int)((double)size_hints.height * clips[i].height +
				 .5);
    }
    if (nclips) for (i=0; i<nwindows; i++)
      XSetClipRectangles(display, window_gcs[i], 0, 0,
			 cliplist, nclips, Unsorted);
    for (i=0; i<nclips; i++) {
      cliplist[i].x = (int)((double)icon_size_list[currsize].max_width *
			    clips[i].x + .5);
      cliplist[i].y = (int)((double)icon_size_list[currsize].max_height *
			    clips[i].y + .5);
      cliplist[i].width = (int)((double)icon_size_list[currsize].max_width *
			        clips[i].width + .5);
      cliplist[i].height = (int)((double)icon_size_list[currsize].max_height *
				 clips[i].height + .5);
    }
    if (nclips) for (i=0; i<nwindows; i++)
      XSetClipRectangles(display, window_gcs[i+nwindows], 0, 0,
			 cliplist, nclips, Unsorted);
    nwindows <<= 1;
    if (icon_size_list != &icon_size_hints) XFree(icon_size_list);
  }


  for (;;) {
    int winno;
    for (winno=0; winno<nwindows; winno++) if (visible[winno]) break;
    while (nvisible) {
      if (qix) {
        XDrawLine(display,window[winno], window_gcs[winno],
                  X1[winno]+OX[winno], Y1[winno]+OY[winno],
		  X2[winno]+OX[winno], Y2[winno]+OY[winno]);
        if (!rndm(25L,winno)) {
	  XGCValues values;
	  values.foreground = palette[rndm((long)numcolors,winno)];
	  XChangeGC(display, window_gcs[winno], GCForeground, &values);
	}
	if (X1[winno] < 3) XV1[winno] = rndm(6L,winno);
	if (Y1[winno] > CY[winno]-4) XV1[winno] = rndm(6L,winno)+3;
	if (X1[winno] > CX[winno]-4) XV1[winno] = rndm(6L,winno)+6;
	if (Y1[winno] < 3) XV1[winno] = rndm(6L,winno)+9;
	if (X2[winno] < 3) XV2[winno] = rndm(6L,winno);
	if (Y2[winno] > CY[winno]-4) XV2[winno] = rndm(6L,winno)+3;
	if (X2[winno] > CX[winno]-4) XV2[winno] = rndm(6L,winno)+6;
	if (Y2[winno] < 3) XV2[winno] = rndm(6L,winno)+9;
	X1[winno] += D[XV1[winno]];
	Y1[winno] += D[XV1[winno]+3];
	X2[winno] += D[XV2[winno]];
	Y2[winno] += D[XV2[winno]+3];
      }
      else {
	XSegment segs[8];
        if (!rndm(50L,winno)) {
          X1[winno] = rndm((long)M[winno],winno) + 1;
          X2[winno] = rndm((long)M[winno],winno) + 1;
          Y1[winno] = rndm((long)X1[winno],winno);
          Y2[winno] = rndm((long)X2[winno],winno);
        }
        if (!rndm(10L,winno)) {
	  XGCValues values;
          XV1[winno] = rndm(7L,winno)-3;
          XV2[winno] = rndm(7L,winno)-3;
          YV1[winno] = rndm(7L,winno)-3;
          YV2[winno] = rndm(7L,winno)-3;
	  values.foreground = palette[rndm((long)numcolors,winno)];
	  XChangeGC(display, window_gcs[winno], GCForeground, &values);
        }
        if (CX[winno]<CY[winno]) {
          XA[winno] = (long)X1[winno]*(long)CX[winno]/(long)CY[winno];
          YA[winno] = (long)Y1[winno]*(long)CX[winno]/(long)CY[winno];
          XB[winno] = X1[winno];
          YB[winno] = Y1[winno];
          XC[winno] = (long)X2[winno]*(long)CX[winno]/(long)CY[winno];
          YC[winno] = (long)Y2[winno]*(long)CX[winno]/(long)CY[winno];
          XD[winno] = X2[winno];
          YD[winno] = Y2[winno];
        }
        else {
          XA[winno] = X1[winno];
          YA[winno] = Y1[winno];
          XB[winno] = (long)X1[winno]*(long)CY[winno]/(long)CX[winno];
          YB[winno] = (long)Y1[winno]*(long)CY[winno]/(long)CX[winno];
          XC[winno] = X2[winno];
          YC[winno] = Y2[winno];
          XD[winno] = (long)X2[winno]*(long)CY[winno]/(long)CX[winno];
          YD[winno] = (long)Y2[winno]*(long)CY[winno]/(long)CX[winno];
        }
        segs[0].x1 = CX[winno]+XA[winno]+OX[winno];
        segs[0].y1 = CY[winno]-YB[winno]+OY[winno];
        segs[0].x2 = CX[winno]+XC[winno]+OX[winno];
        segs[0].y2 = CY[winno]-YD[winno]+OY[winno];
        segs[1].x1 = CX[winno]-YA[winno]+OX[winno];
        segs[1].y1 = CY[winno]+XB[winno]+OY[winno];
        segs[1].x2 = CX[winno]-YC[winno]+OX[winno];
        segs[1].y2 = CY[winno]+XD[winno]+OY[winno];
        segs[2].x1 = CX[winno]-XA[winno]+OX[winno];
        segs[2].y1 = CY[winno]-YB[winno]+OY[winno];
        segs[2].x2 = CX[winno]-XC[winno]+OX[winno];
        segs[2].y2 = CY[winno]-YD[winno]+OY[winno];
        segs[3].x1 = CX[winno]-YA[winno]+OX[winno];
        segs[3].y1 = CY[winno]-XB[winno]+OY[winno];
        segs[3].x2 = CX[winno]-YC[winno]+OX[winno];
        segs[3].y2 = CY[winno]-XD[winno]+OY[winno];
        segs[4].x1 = CX[winno]-XA[winno]+OX[winno];
        segs[4].y1 = CY[winno]+YB[winno]+OY[winno];
        segs[4].x2 = CX[winno]-XC[winno]+OX[winno];
        segs[4].y2 = CY[winno]+YD[winno]+OY[winno];
        segs[5].x1 = CX[winno]+YA[winno]+OX[winno];
        segs[5].y1 = CY[winno]-XB[winno]+OY[winno];
        segs[5].x2 = CX[winno]+YC[winno]+OX[winno];
        segs[5].y2 = CY[winno]-XD[winno]+OY[winno];
        segs[6].x1 = CX[winno]+XA[winno]+OX[winno];
        segs[6].y1 = CY[winno]+YB[winno]+OY[winno];
        segs[6].x2 = CX[winno]+XC[winno]+OX[winno];
        segs[6].y2 = CY[winno]+YD[winno]+OY[winno];
        segs[7].x1 = CX[winno]+YA[winno]+OX[winno];
        segs[7].y1 = CY[winno]+XB[winno]+OY[winno];
        segs[7].x2 = CX[winno]+YC[winno]+OX[winno];
        segs[7].y2 = CY[winno]+XD[winno]+OY[winno];
        XDrawSegments(display,window[winno],window_gcs[winno],
		      segs, 8);
        X1[winno]= (X1[winno] + XV1[winno]) % M[winno];
        Y1[winno]= (Y1[winno] + YV1[winno]) % M[winno];
        X2[winno]= (X2[winno] + XV2[winno]) % M[winno];
        Y2[winno]= (Y2[winno] + YV2[winno]) % M[winno];
      }
      if (!refreshcount[winno] && intervals[winno])
        XFlush(display);
      itercount[winno]++;
      if (!rndm(500L,winno)) {
	if (doroot) {
	  int x,y,w,h;
	  i = winno % rootx;
	  j = winno / rootx;
	  w = ww / rootx;
	  h = wh / rooty;
	  x = i * w;
	  y = j * h;
	  if (i == rootx-1) w=0;
	  if (j == rooty-1) h=0;
	  XClearArea(display,window[winno],x,y,w,h,False);
	}
	else XClearWindow(display,window[winno]);
  	refreshcount[winno]=0;
  	itercount[winno]=0;
  	randombase[winno][0]=randomseed[winno][0];
  	randombase[winno][1]=randomseed[winno][1];
  	randombase[winno][2]=randomseed[winno][2];
	randomize_state_vars(winno);
      }
      winno=scheduler();
      if (!refreshcount[winno] && dynamic_colors &&
          !rndm((long)(800/numcolors),-1)) randomize_color();
      if (XCheckMaskEvent(display,~0L,&event)==True) handle_event(&event);
    }
    XNextEvent(display,&event);
    handle_event(&event);
  }
}

randomize_color()
{
  XColor color;
  color.pixel = palette[rndm((long)numcolors,-1)];
  color.red = rndm(65535L,-1);
  color.green = rndm(65535L,-1);
  color.blue = rndm(65535L,-1);
  color.flags = DoRed|DoGreen|DoBlue;
  XStoreColor(display, DefaultColormap(display,screen), &color);
}

randomize_colors()
{
  int i;
  XColor color[NCOLORS];
  for (i=0; i<numcolors; i++) {
    color[i].pixel = palette[i];
    color[i].red = rndm(65535L,-1);
    color[i].green = rndm(65535L,-1);
    color[i].blue = rndm(65535L,-1);
    color[i].flags = DoRed|DoGreen|DoBlue;
  }
  XStoreColors(display, DefaultColormap(display,screen), color, numcolors);
}

handle_event(event)
XEvent *event;
{
  int i;
  if (event->type==ConfigureNotify) {
    for (i=0; i<nwindows; i++) {
      if (event->xconfigure.window==window[i]) {
	int dorandom=0;
	if (qix) {
          if (CX[i] != event->xconfigure.width ||
              CY[i] != event->xconfigure.height) {
	    XClearWindow(display,event->xconfigure.window);
  	    refreshcount[i]=0;
  	    itercount[i]=0;
  	    randombase[i][0]=randomseed[i][0];
  	    randombase[i][1]=randomseed[i][1];
  	    randombase[i][2]=randomseed[i][2];
	    dorandom=1;
	  }
          CX[i] = event->xconfigure.width;
          CY[i] = event->xconfigure.height;
	}
	else {
	  int dorandom=0;
          if (CX[i] != event->xconfigure.width/2 ||
              CY[i] != event->xconfigure.height/2) {
	    XClearWindow(display,event->xconfigure.window);
  	    refreshcount[i]=0;
  	    itercount[i]=0;
  	    randombase[i][0]=randomseed[i][0];
  	    randombase[i][1]=randomseed[i][1];
  	    randombase[i][2]=randomseed[i][2];
	    dorandom=1;
	  }
          CX[i] = event->xconfigure.width/2;
          CY[i] = event->xconfigure.height/2;
          M[i] = (CX[i]>CY[i]) ? CX[i] : CY[i];
          M[i] = M[i] ? M[i] : 1;
	}
	if (dorandom) randomize_state_vars(i);
	if (nclips) {
	  int j;
          for (j=0; j<nclips; j++) {
            cliplist[j].x = (int)((double)event->xconfigure.width *
				  clips[j].x + .5);
            cliplist[j].y = (int)((double)event->xconfigure.height *
				  clips[j].y + .5);
            cliplist[j].width = (int)((double)event->xconfigure.width *
				      clips[j].width + .5);
            cliplist[j].height = (int)((double)event->xconfigure.height *
				       clips[j].height + .5);
          }
          XSetClipRectangles(display, window_gcs[i], 0, 0,
			     cliplist, nclips, Unsorted);
	}
        break;
      }
    }
  }
  else if (event->type==MapNotify) {
    for (i=0; i<nwindows; i++) {
      if (event->xmap.window==window[i]) {
        refreshcount[i]=0;
        itercount[i]=0;
        randombase[i][0]=randomseed[i][0];
        randombase[i][1]=randomseed[i][1];
        randombase[i][2]=randomseed[i][2];
	randomize_state_vars(i);
        break;
      }
    }
  }
  else if (event->type==Expose) {
    for (i=0; i<nwindows; i++) {
      if (doroot) {
	int x,y,w,h,ix,iy;
	ix = i % rootx;
	iy = i / rootx;
	w = ww / rootx;
	h = wh / rooty;
	x = ix * w;
	y = iy * h;
	if (ix != rootx-1 && event->xexpose.x >= x+w ||
	    event->xexpose.x + event->xexpose.width < x ||
	    iy != rooty-1 && event->xexpose.y >= y+h ||
	    event->xexpose.y + event->xexpose.height < y)
	      continue;
      }
      if (event->xexpose.window==window[i]) {
	refreshcount[i] += itercount[i];
	itercount[i] = 0;
	randomseed[i][0] = randombase[i][0];
	randomseed[i][1] = randombase[i][1];
	randomseed[i][2] = randombase[i][2];
	randomize_state_vars(i);
        if (!doroot) break;
      }
    }
  }
  else if (event->type==VisibilityNotify) {
    for (i=0; i<nwindows; i++) {
      if (event->xvisibility.window==window[i]) {
	if (visible[i] &&
	    event->xvisibility.state == VisibilityFullyObscured) {
          visible[i]=0;
          nvisible--;
	}
	else if (!visible[i] &&
		 event->xvisibility.state != VisibilityFullyObscured) {
          visible[i]=1;
          nvisible++;
	}
        break;
      }
    }
  }
}

randomize_state_vars(i)
int i;
{
  XGCValues values;
  if (qix) {
    X1[i] = (CX[i]>10) ? rndm((long)(CX[i]-10),i)+5 : 5;
    X2[i] = (CX[i]>10) ? rndm((long)(CX[i]-10),i)+5 : 5;
    Y1[i] = (CY[i]>10) ? rndm((long)(CY[i]-10),i)+5 : 5;
    Y2[i] = (CY[i]>10) ? rndm((long)(CY[i]-10),i)+5 : 5;
    XV1[i] = rndm(12L,i)+1;
    XV2[i] = rndm(12L,i)+1;
    values.foreground = palette[rndm((long)numcolors,i)];
    XChangeGC(display, window_gcs[i], GCForeground, &values);
  }
  else {
    X1[i] = rndm((long)M[i],i) + 1;
    X2[i] = rndm((long)M[i],i) + 1;
    Y1[i] = rndm((long)X1[i],i);
    Y2[i] = rndm((long)X2[i],i);
    XV1[i] = rndm(7L,i)-3;
    XV2[i] = rndm(7L,i)-3;
    YV1[i] = rndm(7L,i)-3;
    YV2[i] = rndm(7L,i)-3;
    values.foreground = palette[rndm((long)numcolors,i)];
    XChangeGC(display, window_gcs[i], GCForeground, &values);
  }
}

usage()
{
  int i,j,spaces,xloc;
  static char *options[] = {
    "[-bd <border>]",
    "[-bg <background>]",
    "[-bstore]",
    "[-bw <borderwidth>]",
    "[-clip x,y,w,h[,x,y,w,h[,x,y,w,h]]]",
    "[-colors <color1>[,<color2>[,...]]]",
    "[-delay <msec>]",
    "[-display <displayname>]",
    "[-geometry <geometry>]",
    "[-icondelay <msec>]",
    "[-iconic]",
    "[-mono]",
    "[-mult <number>]",
    "[-qix]",
    "[-r]",
    "[-randomcolor]",
    "[-refresh]"
  };
  (void)fprintf(stderr,"Usage: %s", progname);
  spaces=strlen(progname)+7;
  xloc=spaces;

  for (j=0; j<(sizeof(options)/sizeof(char *)); j++) {
    if (xloc+strlen(options[j]) > 78) {
      putc('\n',stderr);
      for (i = 0; i<spaces; i++) (void)putc(' ',stderr);
      xloc=spaces;
    }
    xloc += strlen(options[j])+1;
    fprintf(stderr," %s",options[j]);
  }
  putc('\n',stderr);

  exit(1);
}

fatalerror(s1,s2)
char *s1,*s2;
{
  (void)fprintf(stderr,"%s: ",progname);
  (void)fprintf(stderr,s1,s2);
  (void)putc('\n',stderr);
  exit(1);
}

long rndm(maxval,element)
long maxval;
int element;
{
  long lrand48();
  int sr1=15, sr2=16;
  long mv=maxval;
  while (mv > 0x8000L) {
    sr1++;
    sr2--;
    mv >>= 1;
  }
  if (element == -1) return ((lrand48() >> sr1) * maxval) >> sr2;
  return ((nrand48(randomseed[element]) >> sr1) * maxval) >> sr2;
}

allocate_arrays(nwin)
unsigned nwin;
{
  int i;
  if ((window=(Window *)calloc(nwin,sizeof(Window))) == NULL) return 0;
  if ((window_gcs=(GC *)calloc(nwin,sizeof(GC))) == NULL) return 0;
  if ((CX=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((CY=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((OX=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((OY=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((visible=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((X1=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((Y1=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((X2=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((Y2=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((XV1=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((XV2=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((schedule=(struct timeval *)calloc(nwin,sizeof(struct timeval))) == NULL)
    return 0;
  if ((intervals=(long *)calloc(nwin,sizeof(long))) == NULL) return 0;
  if ((randomseed=(unsigned short **)calloc(nwin,3*sizeof(short))) == NULL)
    return 0;
  if ((randombase=(unsigned short **)calloc(nwin,3*sizeof(short))) == NULL)
    return 0;
  for (i=0; i<nwin; i++) {
    randombase[i][0] = randomseed[i][0] = (unsigned short)rndm(0xffffL,-1);
    randombase[i][1] = randomseed[i][1] = (unsigned short)rndm(0xffffL,-1);
    randombase[i][2] = randomseed[i][2] = (unsigned short)rndm(0xffffL,-1);
  }
  if ((refreshcount=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((itercount=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if (qix) return 1;
  if ((M=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((YV1=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((YV2=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((XA=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((YA=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((XB=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((YB=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((XC=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((YC=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((XD=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  if ((YD=(int *)calloc(nwin,sizeof(int))) == NULL) return 0;
  return 1;
}

int scheduler()
{
  struct timeval currtime, *nextalarm, *alarmindex;
  struct timezone tzp;
  long interval;
  int i;

  /* Has root-mode operation gotten a die signal? */
  if (killed) refreshrootquit();

  /* Look for any windows performing refresh... immediate return if found */
  for (i=0; i<nwindows; i++) if (refreshcount[i] && visible[i]) {
    refreshcount[i]--;
    return i;
  }

  /* Get current time */
  (void)gettimeofday(&currtime, &tzp);

  /* Find earliest alarm due */
  alarmindex = nextalarm = schedule;
  for (i=1; i<nwindows; i++) {
    if (visible[++alarmindex - schedule] &&
	( alarmindex->tv_sec < nextalarm->tv_sec ||
          alarmindex->tv_sec == nextalarm->tv_sec &&
          alarmindex->tv_usec < nextalarm->tv_usec ))
      nextalarm = alarmindex;
  }

  /* If the next alarm is not past due, sleep until it comes due */
  if (currtime.tv_sec < nextalarm->tv_sec ||
      currtime.tv_sec == nextalarm->tv_sec &&
      currtime.tv_usec < nextalarm->tv_usec) {
    struct timeval timeout;
    int fd=ConnectionNumber(display), readfds;

    timeout.tv_sec = nextalarm->tv_sec - currtime.tv_sec;
    timeout.tv_usec = nextalarm->tv_usec - currtime.tv_usec;
    if (timeout.tv_usec < 0) {
      timeout.tv_sec -= 1L;
      timeout.tv_usec += 1000000L;
    }

    readfds = 1<<fd;
    if (killed) refreshrootquit();
    (void)select(fd+1, &readfds, NULL, NULL, &timeout);

    /* Recompute current time */
    (void)gettimeofday(&currtime, &tzp);

  }

  /* Set next alarm to current time + interval.  If more than one window
     is visible, introduce a random variance so update is staggered.  */
  interval = intervals[nextalarm-schedule];
  if (nvisible > 1) interval += rndm(interval/5L,-1) - interval/10L;
  nextalarm->tv_sec = currtime.tv_sec + interval/1000;
  nextalarm->tv_usec = currtime.tv_usec + (interval%1000)*1000;
  if (nextalarm->tv_usec >= 1000000) {
    nextalarm->tv_sec += 1;
    nextalarm->tv_usec -= 1000000;
  }

  /* Return index of next alarm */
  return nextalarm-schedule;
}

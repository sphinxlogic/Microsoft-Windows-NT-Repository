/*
	Author:		Paul Tallett
*/
#include <stdio.h>
#include <math.h>

#ifdef VMS
#include <decw$include/DECwDwtApplProg.h>
#else
#include <X11/DECwDwtApplProg.h>
#endif

Widget	toplevel, scratch_widget;
Arg arglist[2];
int narg;
int need_gc=0;
Display *dpy;
Window win;
GC gc, white_gc;
XGCValues xgcv;
int width, height, win_depth, cmap_size;
Visual *visual = NULL;

static DRMHierarchy	s_DRMHierarchy;		/* DRM database hierarch id */
static char		*vec[]={"colormap.uid"};
						/* DRM database file list   */
static DRMCode		class ;

static void exit_cb();
static void colour_cb();
static void scratch_cb();
static void free_cb();
static void zap_cb();

static DRMRegisterArg	regvec[] = {
	{"scratch_cb",(caddr_t)scratch_cb},
	{"free_cb",(caddr_t)free_cb},
	{"zap_cb",(caddr_t)zap_cb},
	{"colour_cb",(caddr_t)colour_cb},
	{"exit_cb",(caddr_t)exit_cb}
	};

/*
 *  Main program
 */
int main(argc, argv)
    unsigned int argc;
    char **argv;
{
    Widget main_widget;

    DwtInitializeDRM ();

    toplevel = XtInitialize(
	"Colormap",			/* application name */
	"Colormap",            		/* application class */
	NULL, 0,                        /* options */
	&argc, argv);                   /* command line parameters */

    if (DwtOpenHierarchy (1,			    /* number of files	    */
			vec, 			    /* files     	    */
			NULL,			    /* os_ext_list (null)   */
			&s_DRMHierarchy)	    /* ptr to returned id   */
			!= DRMSuccess) {
	printf ("can't open hierarchy");
     }

    DwtRegisterDRMNames (regvec, XtNumber(regvec)) ;
    
    narg = 0;
    XtSetArg (arglist[narg], DwtNtitle, "ColorMap"); narg++;
    XtSetValues (toplevel, arglist, narg);

    if (DwtFetchWidget (s_DRMHierarchy,
			"main",
			toplevel,
			&main_widget,
			&class)
			!= DRMSuccess)
			    printf("can't fetch interface");

    XtManageChild(main_widget);
    
    XtRealizeWidget(toplevel);

    XtMainLoop();

    /* UNREACHABLE */
    return (0);
}

static void exit_cb(Widget widget, char *tag, DwtAnyCallbackStruct *cbdata )
{
        exit(0);
}

static void scratch_cb(Widget widget, char *tag, DwtAnyCallbackStruct *cbdata )
{
	scratch_widget=widget;
}

do_gc()
{
XWindowAttributes wa;
XVisualInfo vinfo, *vinfos;
int vinfo_mask, j;

	dpy=XtDisplay(scratch_widget);
	if((win=XtWindow(scratch_widget))==NULL)
	    {printf("colormap -- couldn't get window ID");
	    return;}

	width = 300;
	height = 300;
	xgcv.foreground=BlackPixel(dpy,0);
        xgcv.background=BlackPixel(dpy,0);
        gc = XCreateGC(dpy,win,GCForeground|GCBackground, &xgcv);
	xgcv.foreground=WhitePixel(dpy,0);
        white_gc = XCreateGC(dpy,win,GCForeground|GCBackground, &xgcv);
	need_gc = -1;

	vinfo.class = PseudoColor;
	vinfo_mask = VisualClassMask;
	vinfos = (XVisualInfo *) XGetVisualInfo(dpy, vinfo_mask, &vinfo, &j);
	win_depth = 0;
	cmap_size = 256;
	if (j > 0) {
	    visual = vinfos[0].visual;
	    win_depth = vinfos[0].depth;
	    cmap_size = vinfos[0].colormap_size;
	}
}

static void colour_cb(Widget widget, char *tag, DwtAnyCallbackStruct *cbdata )
{
Window wi;
int x,y,w,h,bord,d,i,j,k,l,m;
XExposeEvent *eve = cbdata->event;

	if(need_gc==0)do_gc();
	if(eve->count!=0) return;
	XFillRectangle(dpy,win,white_gc,0,0,width,height);
	k = cmap_size;
	l= sqrt(k);
	m=k/l;
	printf("%d colours, %dx%d\n",k,l,m);
	for(i=0;i<l;i++)
	    for(j=0;j<m;j++){
		XSetForeground(dpy,gc,i+j*l);
		XFillRectangle(dpy,win,gc,i*16,j*16,16,16);
                XDrawRectangle(dpy,win,white_gc,i*16,j*16,16,16);
	    }
}
static void free_cb(Widget widget, char *tag, DwtAnyCallbackStruct *cbdata)
{
    XColor colors[256],red[256],new[256];
    long pixels[256];
    int i, ncols;
    Colormap colormap, colormap2;
    Window root = RootWindow(dpy, DefaultScreen(dpy));
    static int rgb = 0;

    colormap  = DefaultColormap(dpy, DefaultScreen(dpy));

    for(i=0; i<256; i++) colors[i].pixel = i;

    XQueryColors(dpy, colormap, colors, cmap_size);

    for(ncols=cmap_size;ncols>0;ncols--)
	if(XAllocColorCells(dpy, colormap, False, 0, 0, &pixels, ncols)!=0)
			break;

    printf("%d colors free\n", ncols);

    for(i=0;i<ncols;i++) {
	red[i].red = (rgb==0) ? 65534 : 0;
	red[i].green = (rgb==1) ? 65534 : 0;
	red[i].blue = (rgb==2) ? 65534 : 0;
	red[i].pixel = pixels[i];
	red[i].flags = DoRed | DoGreen | DoBlue;
	new[i]=colors[pixels[i]];	
	new[i].flags = DoRed | DoGreen | DoBlue;
	colors[i].flags = DoRed | DoGreen | DoBlue;
    }

    if(++rgb>2) rgb=0;
    if(ncols>0) {
	for(i=0;i<3;i++) {
	    XStoreColors(dpy, colormap, red, ncols);
	    XSync(dpy, False);
	    sleep(1);
	    XStoreColors(dpy, colormap, new, ncols);
	    XSync(dpy, False);
	    sleep(1);
	}
    }
    XFreeColors(dpy, colormap, pixels, ncols, 0, 0);

}
static void zap_cb(Widget widget, char *tag, DwtAnyCallbackStruct *cbdata)
{
    XColor colors[256],red[256],new[256];
    long pixels[256];
    int i, j, ncols;
    Colormap colormap, colormap2;
    Window root = RootWindow(dpy, DefaultScreen(dpy));
    static int rgb = 0;

    colormap  = DefaultColormap(dpy, DefaultScreen(dpy));

    for(ncols=cmap_size;ncols>0;ncols--)
	if(XAllocColorCells(dpy, colormap, False, 0, 0, &pixels, ncols)!=0)
			break;

    printf("%d colors free\n", ncols);

    for(i=0;i<ncols;i++) {

        j = 65534/ncols * i;
	if(rgb<3) j= 65534-j;

	red[i].red = (rgb&3)==0 ? j : 0;
	red[i].green = (rgb&3)==1 ? j : 0;
	red[i].blue = (rgb&3)==2 ? j : 0;
	red[i].pixel = pixels[i];
	red[i].flags = DoRed | DoGreen | DoBlue;
    }

    if(++rgb>5) rgb=0;
    XStoreColors(dpy, colormap, red, ncols);
    XFreeColors(dpy, colormap, pixels, ncols, 0, 0);

}
    


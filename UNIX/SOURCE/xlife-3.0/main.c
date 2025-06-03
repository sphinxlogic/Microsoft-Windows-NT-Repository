/*
 * XLife Copyright 1989 Jon Bennett jb7m+@andrew.cmu.edu, jcrb@cs.cmu.edu
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * CMU SUCKS
 */


/* Coordinate display window added by Paul Callahan (callahan@cs.jhu.edu) */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <stdio.h>

#include "defs.h"
#include "icon.h"
#include "cellbox.h"
#include "cursor_data.h"
#include "cursor_mask.h"
#include "patchlevel.h"

extern cellbox *tentative;
extern LoadReq *loadscript; 

main(argc, argv)
int argc;
char **argv;
{
    int i,tmp;
    Pixmap icon,cursor_data,cursor_mask;
    XSizeHints hints;
    XWMHints wm_hints;
    XClassHint class_hints;
    XSetWindowAttributes winat;
    XColor white,black,dummy;
    XComposeStatus stat;
    char *geomstring = NULL;
    char *initpat = NULL;
    int bwidth = BORDERWIDTH;
    struct timeval inputdelay, timeouttemp;
    LoadReq *loadqueue = NULL;
    XTextProperty windowName, iconName;
    char * window_name = "Xlife: the game of bit manipulation, version 3.0";
    char * icon_name = "Xlife";
    
    if (!(disp = XOpenDisplay(getenv("DISPLAY"))))
	fatal("Can't open Display\n");

    for (i = 1; i < argc; i++)
    {
	if (strcmp(argv[i], "-geometry") == 0)
	    geomstring = argv[++i];
	else if (*argv[i] == '=')
	    geomstring = argv[i];
	else if (strcmp(argv[i], "-bw") == 0)
	    bwidth = atoi(argv[++i]);
	else if (*argv[i] != '-')
	{
	    initpat = argv[i];
	    break;
	}
    }

    screen = DefaultScreen(disp);
    rootw = RootWindow(disp, screen);
    fcolor = WhitePixel(disp, screen);
    bcolor = BlackPixel(disp, screen);

    hints.x = 0;
    hints.y = 0;    
    width = DisplayWidth(disp,screen);
    height = DisplayHeight(disp,screen);

    hints.width = width - bwidth*2;
    hints.height = height - bwidth*2;
    
    hints.flags = PPosition | PSize;

    if (geomstring != NULL)
    {
	int result;

	result = XParseGeometry(geomstring,&hints.x,&hints.y,
				&hints.width,&hints.height);
	if (result & XNegative)
	    hints.x += DisplayWidth(disp,screen) - hints.width - bwidth * 2;
	if (result & YNegative)
	    hints.y += DisplayHeight(disp,screen) - hints.height - bwidth*2;
	if (result & XValue || result & YValue)
	{
	    hints.flags |= USPosition;
	    hints.flags &= ~PPosition;
	}
	if (result & WidthValue || result & HeightValue)
	{
	    hints.flags |= USSize;
	    hints.flags &= ~PSize;
	}
    }
    
    mainw = XCreateSimpleWindow(disp, rootw,
		0, 0, hints.width, hints.height, bwidth, fcolor, bcolor);

    if(!mainw)
	fatal("Can't open main window");

    icon = XCreateBitmapFromData(disp, mainw, icon_bits, icon_width, icon_height);

    if ( XStringListToTextProperty ( &window_name, 1, &windowName ) == 0 ) {
        (void) fprintf ( stderr, "%s: structure allocation for windowName failed.\n", argv[0] );
        exit (-1);
    }

    if ( XStringListToTextProperty ( &icon_name, 1, &iconName ) == 0 ) {
        (void) fprintf ( stderr, "%s: structure allocation for iconName failed.\n", argv[0] );
        exit (-1);
    }

    wm_hints.initial_state = NormalState;
    wm_hints.input = True;
    wm_hints.icon_pixmap = icon;
    wm_hints.flags = IconPixmapHint | StateHint | InputHint;

    class_hints.res_name =  argv[0];
    class_hints.res_class =  "Basicwin";

    XSetWMProperties(disp, mainw, &windowName, &iconName, argv, argc, &hints, &wm_hints, &class_hints );

    xgcv.background = bcolor;
    xgcv.foreground = bcolor;
    blackgc = XCreateGC(disp, mainw, GCForeground | GCBackground, &xgcv);
    xgcv.foreground = fcolor;
    ntextgc = XCreateGC(disp, mainw, GCForeground | GCBackground, &xgcv);
    btextgc = XCreateGC(disp, mainw, GCForeground | GCBackground, &xgcv);
    whitegc = XCreateGC(disp, mainw, GCForeground | GCBackground, &xgcv);

    /* create XOR GC for pivot display */
    tmp=xgcv.function;
    xgcv.function = GXinvert;
    xorgc = XCreateGC(disp, mainw, GCForeground |
                                   GCBackground | GCFunction, &xgcv);
    xgcv.function=tmp;
    
    if(!((nfont = XLoadQueryFont(disp, NORMALFONT)) && (bfont = XLoadQueryFont(disp, BOLDFONT))))
	fatal("Can't load font\n");
    XSetFont(disp, ntextgc, nfont->fid);
    XSetFont(disp, btextgc, bfont->fid);
    xgcv.function = GXcopy;
    xgcv.plane_mask = 1;

    XAllocNamedColor(disp, DefaultColormap(disp, screen), "white", &white, &dummy);
    XAllocNamedColor(disp, DefaultColormap(disp, screen), "black", &black, &dummy);
    cursor_data = XCreateBitmapFromData(disp, mainw, cursor_data_bits, cursor_data_width, cursor_data_height);
    cursor_mask = XCreateBitmapFromData(disp, mainw, cursor_mask_bits, cursor_mask_width, cursor_mask_height);
    cursor = XCreatePixmapCursor(disp, cursor_data, cursor_mask, &white, &black, cursor_data_x_hot, cursor_data_y_hot);
    XDefineCursor(disp, mainw, cursor);
    
    inputgc = XCreateGC(disp, mainw, GCFunction | GCPlaneMask, &xgcv);

    width = hints.width;
    height = hints.height;

    lifew = XCreateSimpleWindow(disp, mainw,
		0, 0,width-BORDERWIDTH*2, (height - INPUTH - BORDERWIDTH*3), 
                BORDERWIDTH,
		fcolor, bcolor);
    coordw = XCreateSimpleWindow(disp, mainw,
		width-COORDW-BORDERWIDTH*2, (height-INPUTH-BORDERWIDTH*2),
                COORDW, INPUTH, BORDERWIDTH,
		fcolor, bcolor);
    inputw = XCreateSimpleWindow(disp, mainw,
		0, (height - INPUTH - BORDERWIDTH*2), 
                width-dispcoord*(COORDW+BORDERWIDTH)-BORDERWIDTH*2, 
                INPUTH, BORDERWIDTH,
		fcolor, bcolor);

    winat.win_gravity = SouthGravity;
    XChangeWindowAttributes(disp,inputw,CWWinGravity,&winat);

    XSelectInput(disp, mainw, ExposureMask | StructureNotifyMask);
    XSelectInput(disp, inputw, KeyPressMask | ExposureMask);
    XSelectInput(disp, lifew, KeyPressMask | ButtonPressMask | Button1MotionMask | PointerMotionMask | Button3MotionMask | ExposureMask);
    XSelectInput(disp, coordw, ExposureMask);

    initcells();
    numcomments=0;
    settimeout(0);
    xpos = ypos = 
       xorigin = yorigin = 
       lastx = lasty = 
       STARTPOS;
    dispcoord=1;
    dispcells=0;
    scale = 1;
    born = 8;
    live = 12;
    maxdead = 10;
    generations = 0;
    numboxes = 0;
    numcells = 0;
    inputlength = width/FONTWIDTH;
    state = STOP;
    fname[0]=NULL;
    strcpy(loadirbuf,DIR);

    /* the one actual comment :-). only accept one pattern since it is highly
     *  unlikely that overlaying n patterns is what you want to do
     */
    gentab();
    /* make tentative list empty before any loading is done */
    tentative = NULL; 

    /* initialize load script to empty (circular list) */
    loadscript = (LoadReq *)malloc(sizeof(LoadReq)); 
    loadscript->next=loadscript;

    if (initpat != NULL) {
        loadx = xpos; 
        loady = ypos;
        txx = 1; 
        txy = 0; 
        tyx = 0; 
        tyy = 1; 
        add_loadreq(&loadqueue,0,initpat,0,
                    STARTPOS,STARTPOS,1,0,0,1);
        do_loadreq(loadqueue);
    }

    XMapWindow(disp, inputw);
    XMapWindow(disp, lifew);
    XMapWindow(disp, mainw);
    XMapWindow(disp, coordw);

    XFlush(disp);
    
    for(;;) {
	while(XCheckMaskEvent(disp, KeyPressMask | ButtonPressMask | Button1MotionMask | PointerMotionMask | Button3MotionMask | ExposureMask | StructureNotifyMask,&event)){
	    switch(event.type) {
	      case KeyPress:
		XLookupString(&event.xkey, keybuf, 16, &ks, &stat);
		if (!DoKeySymIn(ks))
		    DoKeyIn(keybuf);
		break;
	      case MotionNotify:
                /* don't allow drawing until load is confirmed */
		if (!tentative) Motion();
		break;
	      case ButtonPress:
		Button();
		break;
	      case ConfigureNotify:
		DoResize();
		break;
	      case Expose:
		DoExpose(ClassifyWin(event.xexpose.window));
	      default:
		break;
	    }

        }

        if (dispcoord) {
            unsigned long x,y;
            char pstring[50];

            x=XPOS(event.xmotion.x,xpos)-xorigin; 
            y=YPOS(event.xmotion.y,ypos)-yorigin; 

            if (lastx!=x || lasty!=y) {
               lastx=x;
               lasty=y; 
               sprintf(pstring,"(%d,%d)",x,y);
               XClearWindow(disp,coordw);
               XDrawString(disp,coordw,ntextgc,CWCOORDS(0,0),
                           pstring,strlen(pstring));
            }
        }

	if ((state == RUN) | (state == HIDE)) {
	    generate();
	    redisplay();
	} else {
	    inputdelay.tv_sec = 0;
	    inputdelay.tv_usec = 100000;
	    (void) select(32,0,0,0,&inputdelay);
	}

	if (state== RUN){
	    timeouttemp.tv_sec = timeout.tv_sec;
	    timeouttemp.tv_usec = timeout.tv_usec;
	    (void) select(32,0,0,0,&timeouttemp);
	}
    }
}



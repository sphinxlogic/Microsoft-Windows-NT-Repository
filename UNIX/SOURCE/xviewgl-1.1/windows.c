#include "patchlevel.h"
/* Written by Brad Daniels, February 1991

   This code is provided as is, with no warrantees, express or implied.
   I believe this code to be free of encumbrance, and offer it to the
   public domain.  I ask, however, that this paragraph and my name be
   retained in any modified versions of the file you may make, and
   that you notify me of any improvements you make to the code.

   Brad Daniels (daniels@bigred.enet.dec.com)
*/
#include <stdio.h>
#ifdef STDLIB
#include <stdlib.h>
#endif
#include "my_types.h"
#include "video.h"
#include "windows.h"
#include "pcdata.h"
#include "registers.h"
#include "timer.h"
#include "utils.h"

WINDOW MainWindow;
char *WindowName="GRASP Run Window";
char *IconName="GRASP";
CMap current_cmap;
unsigned long int current_drawcolor;
unsigned long int current_drawcolor2;
unsigned long int pixarr[MAXCOLOR];
MyFont fonts[NUMFONTBUFS];
MyFont *curfont=fonts;
BOOLEAN fixaspect=FALSE;
int xaspect=1, yaspect=1;
BOOLEAN color_friendly=FALSE;
BOOLEAN install_cmap=FALSE;
BOOLEAN edge_mode=FALSE;
BOOLEAN quiet_mode=TRUE;
BOOLEAN transparent_mode=FALSE;
BOOLEAN is_transparent[MAXCOLOR];
BOOLEAN monochrome=FALSE;
struct CMap_struct CMaps[MAXPICREG+1];
static unsigned int clipx1,clipy1,clipx2,clipy2;
#ifndef CLIP_ALL_BUT_CFADE
static unsigned int clipxoffs,clipyoffs;
#endif
static BOOLEAN clipping=FALSE;
static BOOLEAN window_active=TRUE;
static BOOLEAN do_expose=TRUE;

#define twobits_at(p,x) \
	   (((p)[(x)/4]&(3<<(6-2*((x)%4))))>>(6-2*((x)%4)))
#define bit_at(p,x) \
	   (((p)[(x)/8]&(1<<(7-((x)%8))))>>(7-((x)%8)))
#define put_bit(p,x) \
	   (p)[(x)/8] |= 1<<((x)%8)

#define dither_pix(c,cmap) (void)(((c)>=MainWindow.maxcolor)? \
				      do_dither(c,cmap):0)

static int do_dither(int c, CMap cmap)
{
unsigned long int i, mindiff=MAXPIXEL,minind=0,tmp,dtmp;
    if (c < MainWindow.maxcolor) return;
    for (i=0;i<MainWindow.maxcolor; i++) {
	tmp = cmap->clist[cmap->cindx[c]].red - cmap->clist[cmap->cindx[i]].red;
	tmp *= tmp;
	dtmp = tmp;
	tmp = cmap->clist[cmap->cindx[c]].green -
		cmap->clist[cmap->cindx[i]].green;
	tmp *= tmp;
	dtmp += tmp;
	tmp = cmap->clist[cmap->cindx[c]].blue -
		cmap->clist[cmap->cindx[i]].blue;
	tmp *= tmp;
	dtmp += tmp;
	if (dtmp < mindiff) {
	    mindiff = dtmp;
	    minind = i;
	}
    }
    pixarr[c]=minind;
    return 0;
}


#include "ibm_font.c"	/* Contains definition for IBM font */

BOOLEAN setup_for_trans_mode(struct PIC_register *r)
{
byte *data,*p;
int px,py;
    if (!r->is_trans) {
	p = data = calloc(1,((r->width+7)>>3)*r->height);
	for (py=0;py<r->height;py++) {
	    for (px=0;px<r->width;px++) {
		if (!is_transparent[XGetPixel(r->img,px,py)]) {
		    put_bit(p,px);
		}
	    }
	    p += (r->width + 7) >> 3;
	}
	r->clippix = XCreateBitmapFromData(
	    MainWindow.theDisp, MainWindow.theWindow,
	    (char *)data, r->width, r->height);
	free(data);
	r->trans_GC = XCreateGC(MainWindow.theDisp,
			  MainWindow.theWindow, 0, NULL);
	XCopyGC(MainWindow.theDisp, MainWindow.createGC,
		~0, r->trans_GC);
	XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
		GCForeground, r->trans_GC);
	XSetClipMask(MainWindow.theDisp, r->trans_GC, r->clippix);
	r->is_trans = TRUE;
    }
}

BOOLEAN tile_window(struct PIC_register *r)
{
    XSetTile(MainWindow.theDisp,MainWindow.tileGC,r->p);
    XSetTSOrigin(MainWindow.theDisp,MainWindow.tileGC,0,0);
    XFillRectangle(MainWindow.theDisp, MainWindow.thePix, MainWindow.tileGC,
		   0, 0, MainWindow.Width, MainWindow.Height);
    if (window_active)
	XFillRectangle(MainWindow.theDisp, MainWindow.theWindow,
		       MainWindow.tileGC,
		       0, 0, MainWindow.Width, MainWindow.Height);
    return TRUE;
}

BOOLEAN put_partial(struct PIC_register *r, int x, int y, BOOLEAN rel)
{
GC theGC;
int width, height;

    x *= MainWindow.pixwid;
    y *= MainWindow.pixht;
    y = r->height - y;
    height = r->height - y;
    if (height > MainWindow.Height)
	height = MainWindow.Height;
    y -= height;
    r->xoffs = x + (rel?r->xoffs:0);
    r->yoffs = y + (rel?r->yoffs:0);
    return TRUE;

}

void copy_pix(Pixmap src, Pixmap dst)
{
XRectangle rectangle;
XGCValues gcval;
GC theGC;
    gcval.function=GXcopy;
    gcval.foreground=0;
    gcval.background=0;
    theGC = XCreateGC(MainWindow.theDisp, dst,
		GCFunction|GCForeground|GCBackground,
		&gcval);
    XFillRectangle(MainWindow.theDisp, dst, theGC,
		   0,0,MainWindow.Width,MainWindow.Height);
    XSetForeground(MainWindow.theDisp, theGC, 1);
#ifdef CLIP_ALL_BUT_CFADE
    if (clipping) {
	rectangle.x = 0;
	rectangle.y = 0;
	rectangle.width = (clipx2-clipx1);
	rectangle.height = (clipy2-clipy1);
	XSetClipRectangles(MainWindow.theDisp, theGC, clipx1, clipy1,
			   &rectangle, 1, YXSorted);
    }
#endif
    XCopyArea(MainWindow.theDisp, src, dst, theGC,
	      0,0,MainWindow.Width,MainWindow.Height,0,0);
    XFreeGC (MainWindow.theDisp, theGC);
}

BOOLEAN put_reg(struct PIC_register *r, int x, int y, int effect,
		 int speed, BOOLEAN transient, BOOLEAN clip,
		 BOOLEAN to_window, char *err_msg)
{
static struct PIC_register blankreg;
static Pixmap hbars,vbars,grid,workpix,workpix2;
static BOOLEAN bars_initialized = FALSE;
static int barwid, barht;
#ifndef FAST_SPARKLE
static Pixmap *sparkle_frames;
static BOOLEAN sparkle_frames_initialized = FALSE;
static int sparklewid, sparkleht;
byte *sparkle_data, *sparkle_index;
#else
int *sparkle_seq;
#endif
static int frame_list[MIN_EFFECT_FRAMES];
XPoint tr1[3],tr2[3],par[4];
byte *data,*p;
int ox,oy,width,height,xnframes,ynframes,nframes,tmp,i,j,k,l,m,xstep,ystep,del,
    nextb,rowlen,iwid,iht,ix,minframes;
GC theGC,tmpGC;
XGCValues gcval;
    if (bars_initialized && ((barwid < MainWindow.Width) ||
			     (barht < MainWindow.Height))) {
	XFreePixmap(MainWindow.theDisp,hbars);
	XFreePixmap(MainWindow.theDisp,vbars);
	XFreePixmap(MainWindow.theDisp,grid);
	XFreePixmap(MainWindow.theDisp,workpix);
	XFreePixmap(MainWindow.theDisp,workpix2);
	bars_initialized = FALSE;
    }
    if (!bars_initialized) {
        barwid = MainWindow.Width;
	barht = MainWindow.Height;
	hbars = XCreatePixmap(MainWindow.theDisp,MainWindow.theWindow,
		    MainWindow.Width, MainWindow.Height, 1);
	vbars = XCreatePixmap(MainWindow.theDisp,MainWindow.theWindow,
		    MainWindow.Width, MainWindow.Height, 1);
	grid = XCreatePixmap(MainWindow.theDisp,MainWindow.theWindow,
		    MainWindow.Width, MainWindow.Height, 1);
	workpix = XCreatePixmap(MainWindow.theDisp,MainWindow.theWindow,
		    MainWindow.Width, MainWindow.Height, 1);
	workpix2 = XCreatePixmap(MainWindow.theDisp,MainWindow.theWindow,
		    MainWindow.Width, MainWindow.Height, 1);
	gcval.function=GXcopy;
	gcval.foreground=0;
	gcval.background=0;
	gcval.graphics_exposures = False;
	theGC = XCreateGC(MainWindow.theDisp, hbars,
		    GCFunction|GCForeground|GCBackground|GCGraphicsExposures,
		    &gcval);
	XFillRectangle(MainWindow.theDisp, hbars, theGC,
		       0,0,MainWindow.Width,MainWindow.Height);
	XFillRectangle(MainWindow.theDisp, vbars, theGC,
		       0,0,MainWindow.Width,MainWindow.Height);
	XSetForeground(MainWindow.theDisp, theGC, 1);
	for (i=0;i<MainWindow.Height;i+=2) {
	    XFillRectangle(MainWindow.theDisp, hbars, theGC,
			   0,i*MainWindow.pixht,
			   MainWindow.Width,MainWindow.pixht);
	}
	for (i=0;i<MainWindow.Width;i+=2) {
	    XFillRectangle(MainWindow.theDisp, vbars, theGC,
			   i*MainWindow.pixwid,0,
			   MainWindow.pixwid,MainWindow.Height);
	}
	XCopyArea(MainWindow.theDisp, hbars, grid, theGC,
		  0,0,MainWindow.Width,MainWindow.Height,0,0);
	XSetFunction(MainWindow.theDisp, theGC, GXand);
	XCopyArea(MainWindow.theDisp, vbars, grid, theGC,
		  0,0,MainWindow.Width,MainWindow.Height,0,0);
	XFreeGC(MainWindow.theDisp, theGC);
        bars_initialized = TRUE;
    }
    if (r==NULL) {
	r = &blankreg;
	if (!r->haspixmap) {
	    r->p=XCreatePixmap(MainWindow.theDisp,MainWindow.theWindow,
		    MainWindow.Width, MainWindow.Height,
		    MainWindow.theDepth);
	    r->width = MainWindow.Width;
	    r->height = MainWindow.Height;
	    r->haspixmap = TRUE;
	}
	r->hascmap = TRUE;
	r->cmap = current_cmap;
	r->maxpval = current_drawcolor;
	XFillRectangle(MainWindow.theDisp, r->p, MainWindow.drawGC,
		       0,0,r->width,r->height);
	theGC = MainWindow.edgeGC;
	ox=clipx1;
	oy=clipy1;
	width = clipx2-clipx1;
	height = clipy2-clipy1;
    } else {
#ifdef CLIP_ALL_BUT_CFADE
	ox = r->xoffs;
	oy = r->yoffs;
	width = r->width - ox;
	height = r->height - oy;
	if (width > MainWindow.Width)
	    width = MainWindow.Width;
	if (height > MainWindow.Height)
	    height = MainWindow.Height;
#else
	if (clip) {
	    x += clipxoffs;
	    y += clipyoffs;
	    ox = r->xoffs+clipx1;
	    oy = r->yoffs+clipy1;
	    width = r->width - ox;
	    height = r->height - oy;
	    if (width > (clipx2-clipx1))
		width = (clipx2-clipx1);
	    if (height > (clipy2-clipy1))
		height = (clipy2-clipy1);
	} else {
	    ox = r->xoffs;
	    oy = r->yoffs;
	    width = r->width - ox;
	    height = r->height - oy;
	    if (width > MainWindow.Width)
		width = MainWindow.Width;
	    if (height > MainWindow.Height)
		height = MainWindow.Height;
	}
#endif
#ifdef NDEBUG
fprintf(stderr,"putting (%d,%d)-(%dx%d) (image is (%d,%d)-(%dx%d))\n%s clipping - clip=(%d,%d)-(%d,%d)\n",
	ox,oy,width,height,r->xoffs,r->yoffs,width,height,(clip)?"":"not",clipx1,clipy1,clipx2,clipy2);
#endif
	if ((width==0) || (height == 0))
	    return TRUE;
	if (transparent_mode || r->force_trans) {
	    if (!r->is_trans) {
		setup_for_trans_mode(r);
	    }
	    theGC = r->trans_GC;
#ifdef CLIP_ALL_BUT_CFADE
	    if (clipping) {
		copy_pix(r->clippix,workpix);
		XSetClipMask(MainWindow.theDisp, r->trans_GC, workpix);
	    }
#endif
	    /* If clipping, this may produce the wrong result... */
	    XSetClipOrigin(MainWindow.theDisp, theGC,
			   screen_x(x)-ox, screen_y(y,height)-oy);
	} else {
	    if (r->is_trans) {
		r->is_trans = FALSE;
		XFreeGC(MainWindow.theDisp, r->trans_GC);
	    }
	    theGC = MainWindow.edgeGC;
	}
    }
    /* Ignore expose events during fades. */
    do_expose = FALSE;
    if (!transient)
	XCopyArea(MainWindow.theDisp,r->p,
		  to_window?MainWindow.winPix:MainWindow.thePix,
		  theGC,ox,oy,width,height,
		  screen_x(x),screen_y(y,height));
    if (!to_window && !window_active) {
	/* Handle Expose events now. */
	do_expose = TRUE;
	evt_handler(0);
	return TRUE;
    }
    if (r->hascmap && (r->cmap != current_cmap)) {
	current_cmap = r->cmap;
	activate_current_cmap(r->maxpval);
    } else {
	update_current_cmap(r->maxpval);
    }
    minframes = MIN_EFFECT_FRAMES*((1.0*width)/(1.0*MainWindow.Width));
    if (minframes > MIN_EFFECT_FRAMES*((1.0*height)/(1.0*MainWindow.Height)))
	minframes = MIN_EFFECT_FRAMES*((1.0*height)/(1.0*MainWindow.Height));
    switch (effect) {
	case EFFECT_HWIPE_L_R:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    xstep = width/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/xstep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow,
			      theGC,
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i*xstep),oy,xstep,height,
			  screen_x(x)+(i*xstep),screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+i*xstep,oy,width-i*xstep,height,
			  screen_x(x)+i*xstep,screen_y(y,height));
	    }
	    break;
	case EFFECT_HWIPE_R_L:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    xstep = width/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/xstep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow,
			      theGC,
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width-((i+1)*xstep),oy,xstep,height,
			  screen_x(x)+width-((i+1)*xstep),screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width-i*xstep,height,
			  screen_x(x),screen_y(y,height));
	    }
	    break;
	case EFFECT_HWIPE_E_C:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1)/2;
	    xstep = width/2/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/2/xstep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow,
			      theGC,
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow,
			      theGC,
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i*xstep),oy,xstep,height,
			  screen_x(x)+(i*xstep),screen_y(y,height));
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width-((i+1)*xstep),oy,xstep,height,
			  screen_x(x)+width-((i+1)*xstep),screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (2*i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+i*xstep,oy,width-2*i*xstep,height,
			  screen_x(x)+i*xstep,screen_y(y,height));
	    }
	    break;
	case EFFECT_HWIPE_C_E:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1)/2;
	    xstep = width/2/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/2/xstep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow,
			      theGC,
			      screen_x(x)+width/2-((i+1)*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+width/2-((i+1)*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow,
			      theGC,
			      screen_x(x)+width/2+(i*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+width/2+(i*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2-((i+1)*xstep),oy,xstep,height,
			  screen_x(x)+width/2-((i+1)*xstep),screen_y(y,height));
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2+(i*xstep),oy,xstep,height,
			  screen_x(x)+width/2+(i*xstep),screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (2*i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width/2-i*xstep,height,
			  screen_x(x),screen_y(y,height));
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2+i*xstep,oy,width/2-i*xstep,height,
			  screen_x(x)+width/2+i*xstep,screen_y(y,height));
	    }
	    break;
	case EFFECT_HFILTER1:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1)/2;
	    xstep = width/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/xstep;
	    del = speed/nframes;
	    theGC = XCreateGC(MainWindow.theDisp,
			      MainWindow.theWindow, 0, NULL);
	    XCopyGC(MainWindow.theDisp, MainWindow.copyGC, ~0, theGC);
	    XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
		    GCForeground, theGC);
#ifdef CLIP_ALL_BUT_CFADE
	    if (clipping) {
		copy_pix(hbars,workpix);
		XSetClipMask(MainWindow.theDisp, theGC, workpix);
	    } else {
		XSetClipMask(MainWindow.theDisp, theGC, hbars);
	    }
#else
	    XSetClipMask(MainWindow.theDisp, theGC, hbars);
#endif
	    for (i=0;i<nframes;i++) {
		XSetClipOrigin(MainWindow.theDisp, theGC,0,0);
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    XSetClipOrigin(MainWindow.theDisp,theGC,0,MainWindow.pixht);
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    XSetClipOrigin(MainWindow.theDisp,theGC,0,0);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i*xstep),oy,xstep,height,
			  screen_x(x)+(i*xstep),
			  screen_y(y,height));
		XSetClipOrigin(MainWindow.theDisp, theGC,0,MainWindow.pixht);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width-((i+1)*xstep),oy,xstep,height,
			  screen_x(x)+width-((i+1)*xstep),
			  screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*xstep < width) {
		XSetClipOrigin(MainWindow.theDisp, theGC,0,0);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+i*xstep,oy,width-i*xstep,height,
			  screen_x(x)+i*xstep,
			  screen_y(y,height));
		XSetClipOrigin(MainWindow.theDisp, theGC,0,MainWindow.pixht);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width-i*xstep,height,
			  screen_x(x),screen_y(y,height));
	    }
	    XFreeGC(MainWindow.theDisp, theGC);
	    break;
	case EFFECT_HFILTER2:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1)/2;
	    xstep = width/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/xstep;
	    del = speed/nframes;
	    theGC = XCreateGC(MainWindow.theDisp,
			      MainWindow.theWindow, 0, NULL);
	    XCopyGC(MainWindow.theDisp, MainWindow.copyGC, ~0, theGC);
	    XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
		    GCForeground, theGC);
#ifdef CLIP_ALL_BUT_CFADE
	    if (clipping) {
		copy_pix(hbars,workpix);
		XSetClipMask(MainWindow.theDisp, theGC, workpix);
	    } else {
		XSetClipMask(MainWindow.theDisp, theGC, hbars);
	    }
#else
	    XSetClipMask(MainWindow.theDisp, theGC, hbars);
#endif
	    XSetClipOrigin(MainWindow.theDisp, theGC,0,0);
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i*xstep),oy,xstep,height,
			  screen_x(x)+(i*xstep),
			  screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+i*xstep,oy,width-i*xstep,height,
			  screen_x(x)+i*xstep,
			  screen_y(y,height));
		sit_for(del);
	    }
	    XSetClipOrigin(MainWindow.theDisp, theGC,0,MainWindow.pixht);
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width-((i+1)*xstep),oy,xstep,height,
			  screen_x(x)+width-((i+1)*xstep),
			  screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width-i*xstep,height,
			  screen_x(x),screen_y(y,height));
	    }
	    XFreeGC(MainWindow.theDisp, theGC);
	    break;
	case EFFECT_HHWIPE_1:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/2/(mindel?mindel:1);
	    xstep = width/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/xstep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height)+height/2,
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i*xstep),oy+height/2,xstep,height/2,
			  screen_x(x)+(i*xstep),screen_y(y,height)+height/2);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+i*xstep,oy+height/2,width-i*xstep,height/2,
			  screen_x(x)+i*xstep,screen_y(y,height)+height/2);
	    }
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+width-((i+1)*xstep)+xstep/2,
			      screen_y(y,height) + height/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width-((i+1)*xstep),oy,xstep,height/2,
			  screen_x(x)+width-((i+1)*xstep),screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width-i*xstep,height/2,
			  screen_x(x),screen_y(y,height));
	    }
	    break;
	case EFFECT_HHWIPE_2:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    xstep = width/2/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/2/xstep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+(i*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x)+width/2+(i*xstep)+xstep/2,
			      screen_y(y,height),
			      screen_x(x)+width/2+(i*xstep)+xstep/2,
			      screen_y(y,height) + height);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i*xstep),oy,xstep,height,
			  screen_x(x)+(i*xstep),screen_y(y,height));
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2+(i*xstep),oy,xstep,height,
			  screen_x(x)+width/2+(i*xstep),screen_y(y,height));
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (2*i*xstep < width) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+i*xstep,oy,width-2*i*xstep,height,
			  screen_x(x)+i*xstep,screen_y(y,height));
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2+i*xstep,oy,width-2*i*xstep,height,
			  screen_x(x)+width/2+i*xstep,screen_y(y,height));
	    }
	    break;
	case EFFECT_VWIPE_T_B:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    ystep = height/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/ystep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+(i*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+(i*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+(i*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width,height-i*ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
	    }
	    break;
#ifdef VMS	/* Work around VMS X server bug */
	case EFFECT_VFINGER_WIPE:
#endif
	case EFFECT_VWIPE_B_T:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    ystep = height/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/ystep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height-((i+1)*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+height-(i+1)*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width,height-i*ystep,
			  screen_x(x),screen_y(y,height));
	    }
	    break;
	case EFFECT_VWIPE_C_E:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    ystep = height/2/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/2/ystep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+height/2-((i+1)*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height/2-((i+1)*ystep)+ystep/2);
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+height/2+(i*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height/2+(i*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height/2-((i+1)*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+height/2-(i+1)*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height/2+(i*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+height/2+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (2*i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width,height-2*i*ystep,
			  screen_x(x),screen_y(y,height));
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height/2+i*ystep,width,height-2*i*ystep,
			  screen_x(x),screen_y(y,height)+height/2+i*ystep);
	    }
	    break;
	case EFFECT_VWIPE_E_C:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    ystep = height/2/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/2/ystep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2);
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+(i*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+(i*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height-((i+1)*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+height-(i+1)*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+(i*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (2*i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width,height-2*i*ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
	    }
	    break;
#ifdef VMS	/* Work around VMS X server bug */
	case EFFECT_VFINGER_FILTER:
#endif
	case EFFECT_VFILTER_B_T:
	    ystep = MainWindow.pixht;
	    nframes = height/2/ystep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height-(2*(i+1)*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+height-2*(i+1)*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+(2*i+1)*ystep,width,ystep,
			  screen_x(x),screen_y(y,height)+(2*i+1)*ystep);
		sit_for(del);
	    }
	    if (2*i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+2*i*ystep,width,height-2*i*ystep,
			  screen_x(x),screen_y(y,height)+2*i*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width,height-2*i*ystep,
			  screen_x(x),screen_y(y,height));
	    }
	    break;
	case EFFECT_VHWIPE_B_T:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    ystep = height/2/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/2/ystep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+height/2+(i*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height/2+(i*ystep)+ystep/2);
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+(i*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+(i*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height/2+i*ystep,width,ystep,
			  screen_x(x),screen_y(y,height)+height/2+i*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+(i*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (2*i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height/2+i*ystep,width,height-2*i*ystep,
			  screen_x(x),screen_y(y,height)+height/2+i*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width,height-2*i*ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
	    }
	    break;
	case EFFECT_VHWIPE_L_R:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/2/(mindel?mindel:1);
	    ystep = height/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/ystep;
	    del = speed/2/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+(i*ystep)+ystep/2,
			      screen_x(x) + width/2,
			      screen_y(y,height)+(i*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width/2,ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width/2,height-i*ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
	    }
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x) + width/2,
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2,oy+height-((i+1)*ystep),width/2,ystep,
			  screen_x(x)+width/2,screen_y(y,height)+height-(i+1)*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2,oy,width/2,height-i*ystep,
			  screen_x(x)+width/2,screen_y(y,height));
	    }
	    break;
	case EFFECT_VQWIPE:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/4/(mindel?mindel:1);
	    ystep = height/2/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/2/ystep;
	    del = speed/4/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+(i*ystep)+ystep/2,
			      screen_x(x) + width/2,
			      screen_y(y,height)+(i*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width/2,ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (2*i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width/2,height-2*i*ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		sit_for(del);
	    }
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x) + width/2,
			      screen_y(y,height)+height/2+(i*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height/2+(i*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2,oy+height/2+i*ystep,width/2,ystep,
			  screen_x(x)+width/2,screen_y(y,height)+height/2+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if ((i*ystep+height/2) < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2,oy+height/2+i*ystep,
			  width/2,height-2*i*ystep,
			  screen_x(x)+width/2,
			  screen_y(y,height)+height/2+i*ystep);
		sit_for(del);
	    }
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2,
			      screen_x(x) + width/2,
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height-(i+1)*ystep,width/2,ystep,
			  screen_x(x),screen_y(y,height)+height-(i+1)*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*ystep < height/2) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height/2,width/2,height-2*i*ystep,
			  screen_x(x),screen_y(y,height)+height/2);
		sit_for(del);
	    }
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x) + width/2,
			      screen_y(y,height)+height/2-((i+1)*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height/2-((i+1)*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2,oy+height/2-(i+1)*ystep,
			  width/2,ystep,
			  screen_x(x)+width/2,screen_y(y,height)+height/2-(i+1)*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if ((i*ystep+height/2) < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2,oy,width/2,height-2*i*ystep,
			  screen_x(x)+width/2,screen_y(y,height));
	    }
	    break;
#ifndef VMS	/* This code crashes VMS X server */
	case EFFECT_VFINGER_WIPE:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/2/(mindel?mindel:1);
	    ystep = height/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/ystep;
	    del = speed/2/nframes;
	    theGC = XCreateGC(MainWindow.theDisp,
			      MainWindow.theWindow, 0, NULL);
	    XCopyGC(MainWindow.theDisp, MainWindow.copyGC, ~0, theGC);
	    XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
		    GCForeground, theGC);
#ifdef CLIP_ALL_BUT_CFADE
	    if (clipping) {
		copy_pix(vbars,workpix);
		XSetClipMask(MainWindow.theDisp, theGC, workpix);
	    } else {
		XSetClipMask(MainWindow.theDisp, theGC, vbars);
	    }
#else
	    XSetClipMask(MainWindow.theDisp, theGC, vbars);
#endif
	    XSetClipOrigin(MainWindow.theDisp, theGC,0,0);
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+height-((i+1)*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height-((i+1)*ystep),width,ystep,
			  screen_x(x),
			  screen_y(y,height)+height-(i+1)*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width,height-i*ystep,
			  screen_x(x),screen_y(y,height));
		sit_for(del);
	    }
	    XSetClipOrigin(MainWindow.theDisp, theGC,MainWindow.pixwid,0);
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      screen_x(x),
			      screen_y(y,height)+(i*ystep)+ystep/2,
			      screen_x(x) + width,
			      screen_y(y,height)+(i*ystep)+ystep/2);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+(i*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  MainWindow.copyGC,ox,oy+i*ystep,width,height-i*ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
	    }
	    XFreeGC(MainWindow.theDisp, theGC);
	    break;
	case EFFECT_VFINGER_FILTER:
	    if (speed<(2*minframes*(mindel?mindel:1)))
		speed = 2*minframes*(mindel?mindel:1);
	    nframes = speed/4/(mindel?mindel:1);
	    ystep = height/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
	    }
	    nframes = height/ystep;
	    del = speed/4/nframes;
	    theGC = XCreateGC(MainWindow.theDisp,
			      MainWindow.theWindow, 0, NULL);
	    XCopyGC(MainWindow.theDisp, MainWindow.copyGC, ~0, theGC);
	    XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
		    GCForeground, theGC);
#ifdef CLIP_ALL_BUT_CFADE
	    if (clipping) {
		copy_pix(grid,workpix);
		XSetClipMask(MainWindow.theDisp, theGC, workpix);
	    } else {
		XSetClipMask(MainWindow.theDisp, theGC, grid);
	    }
#else
	    XSetClipMask(MainWindow.theDisp, theGC, grid);
#endif
	    XSetClipOrigin(MainWindow.theDisp, theGC,0,0);
	    for (i=0;i<nframes;i++) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height-((i+1)*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+height-(i+1)*ystep);
		sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width,height-i*ystep,
			  screen_x(x),screen_y(y,height));
		sit_for(del);
	    }
	    XSetClipOrigin(MainWindow.theDisp, theGC,MainWindow.pixwid,0);
	    for (i=0;i<nframes;i++) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+(i*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width,height-i*ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		sit_for(del);
	    }
	    XSetClipOrigin(MainWindow.theDisp, theGC,0,MainWindow.pixht);
	    for (i=0;i<nframes;i++) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height-((i+1)*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+height-(i+1)*ystep);
		sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy,width,height-i*ystep,
			  screen_x(x),screen_y(y,height));
		sit_for(del);
	    }
	    XSetClipOrigin(MainWindow.theDisp, theGC,MainWindow.pixwid,MainWindow.pixht);
	    for (i=0;i<nframes;i++) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+(i*ystep),width,ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
		sit_for(del);
	    }
	    if (i*ystep < height) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+i*ystep,width,height-i*ystep,
			  screen_x(x),screen_y(y,height)+i*ystep);
	    }
	    XFreeGC(MainWindow.theDisp, theGC);
	    break;
#endif /* !VMS */
	case EFFECT_SLITHER:
	    ystep = MainWindow.pixht;
	    nframes = height/ystep;
	    del = speed/nframes;
	    for (j=0;j<nframes;j++) {
		for (i=0;i<(nframes-j);i++) {
		    XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			      theGC,ox,oy+height-(j+1)*ystep,width,ystep,
			      screen_x(x),screen_y(y,height)+i*ystep);
		}
		sit_for(del);
	    }
	    break;
	case EFFECT_SPARKLE:
#ifdef FAST_SPARKLE
	    /* This algorithm was inspired by Patrick Naughton's xgrasp */
	    if (speed<(4*minframes*(mindel?mindel:1)))
		speed =  4*minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    iwid  = width;
	    iht = height;
	    for (i=2;nframes < (iwid*iht);i++) {
		iwid  = (width+i-1)/i;
		iht = (height+i-1)/i;
	    }
	    nframes = iwid*iht;
	    xstep = (width+iwid-1)/iwid;
	    ystep = (height+iht-1)/iht;
	    del = speed/nframes;
	    sparkle_seq = malloc(nframes*sizeof(int));
	    /* Pick a random sequence in which to display the boxes by starting
	       with a linear sequence, then swapping random pairs. */
	    for (i=0;i<nframes;i++)
		sparkle_seq[i] = i;
	    for (i=0;i<nframes;i++) {
		j = rand()%nframes;
		do {
		    k = rand()%nframes;
		} while (j==k);
		l = sparkle_seq[j];
		sparkle_seq[j] = sparkle_seq[k];
		sparkle_seq[k] = l;
	    }
	    for (i=0;i<nframes;i++) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,theGC,
			  ox+(sparkle_seq[i]%iwid)*xstep,
			  oy+(sparkle_seq[i]/iwid)*ystep,
			  xstep,ystep,
			  screen_x(x)+(sparkle_seq[i]%iwid)*xstep,
			  screen_y(y,height)+(sparkle_seq[i]/iwid)*ystep);
		sit_for(del);
	    }
	    free(sparkle_seq);
	    break;
#else /* FAST_SPARKLE */
	    del = speed/MIN_EFFECT_FRAMES;
	    if (sparkle_frames_initialized &&
		((sparklewid < MainWindow.Width) ||
		 (sparkleht < MainWindow.Height))) {
		for (i=0;i<MIN_EFFECT_FRAMES;i++)
		    XFreePixmap(MainWindow.theDisp,sparkle_frames[i]);
		free(sparkle_frames);
		sparkle_frames_initialized = FALSE;
	    }
	    if (!sparkle_frames_initialized) {
		sparklewid = MainWindow.Width;
		sparkleht = MainWindow.Height;
		sparkle_frames = calloc(MIN_EFFECT_FRAMES,sizeof(Pixmap));
		rowlen = (MainWindow.Width+7)>>3;
		sparkle_data = calloc(MainWindow.Height,rowlen);
		iwid  = (sparklewid+SPARKLE_WIDTH-1)/SPARKLE_WIDTH;
		iht = (sparkleht+SPARKLE_HEIGHT-1)/SPARKLE_HEIGHT;
		sparkle_index = calloc(iwid,iht);
		/* Set 1 point within each SPARKLE_WIDTH*SPARKLE_HEIGHT area */
		for (i=0; i < iht; i++) {
		    for (j=0; j < iwid; j++) {
			sparkle_index[i*iwid+j] = rand() % MIN_EFFECT_FRAMES;
		    }
		}
		/* Since MIN_EFFECT_FRAMES === SPARKLE_WIDTH*SPARKLE_HEIGHT,
		   we can guarantee a complete fill of the screen by rotating
		   each bit through all locations in the square. */
		for (i=0; i< MIN_EFFECT_FRAMES; i++) {
		    for (j=0; j < iht; j++) {
			for (k=0; k < iwid; k++) {
			    ix = j*iwid+k;
			    l = sparkle_index[ix]/SPARKLE_WIDTH +
				j*SPARKLE_HEIGHT;;
			    m = sparkle_index[ix]%SPARKLE_WIDTH +
				k*SPARKLE_WIDTH;
			    if ((l > MainWindow.Height) ||
				(m > MainWindow.Width)) {
				sparkle_index[ix] = 0;
				l = j*SPARKLE_HEIGHT;;
				m = k*SPARKLE_WIDTH;
			    }
			    sparkle_data[l*rowlen+(m>>3)] &= (~(1<<(m&7)));
			    sparkle_index[ix]++;
			    sparkle_index[ix] %= MIN_EFFECT_FRAMES;
			    l = sparkle_index[ix]/SPARKLE_WIDTH +
				j*SPARKLE_HEIGHT;;
			    m = sparkle_index[ix]%SPARKLE_WIDTH +
				k*SPARKLE_WIDTH;
			    if ((l > MainWindow.Height) ||
				(m > MainWindow.Width)) {
				sparkle_index[ix] = 0;
				l = j*SPARKLE_HEIGHT;;
				m = k*SPARKLE_WIDTH;
			    }
			    sparkle_data[l*rowlen+(m>>3)] |= (1<<(m&7));
			}
		    }
		    sparkle_frames[i]= XCreateBitmapFromData(
			MainWindow.theDisp, MainWindow.theWindow,
			(char *)sparkle_data,MainWindow.Width,
			MainWindow.Height);
		}
		free(sparkle_data);
		free(sparkle_index);
		for (i=0;i<MIN_EFFECT_FRAMES;i++)
		    frame_list[i] = -1;
		/* Make the effect truly random by picking a random sequence
		   in which to display the frames. */
		for (i=0;i<MIN_EFFECT_FRAMES;i++) {
		    j = rand()%MIN_EFFECT_FRAMES;
		    while (frame_list[j] != -1)
			j = (j+1)%MIN_EFFECT_FRAMES;
		    frame_list[j] = i;
		}
		sparkle_frames_initialized = TRUE;
	    }
	    theGC = XCreateGC(MainWindow.theDisp,
			      MainWindow.theWindow, 0, NULL);
	    XCopyGC(MainWindow.theDisp, MainWindow.copyGC, ~0, theGC);
	    XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
		    GCForeground, theGC);
	    XSetClipOrigin(MainWindow.theDisp, theGC,0,0);
	    for (i=0;i<MIN_EFFECT_FRAMES;i++) {
		if (transparent_mode || r->force_trans) {
		    gcval.function=GXcopy;
		    gcval.background=0;
		    gcval.foreground=1;
		    tmpGC = XCreateGC(MainWindow.theDisp, workpix2,
				      GCFunction|GCForeground|GCBackground,
				      &gcval);
		    XSetClipOrigin(MainWindow.theDisp, tmpGC,
				   screen_x(x),screen_y(y,height));
		    XSetClipMask(MainWindow.theDisp, tmpGC, r->clippix);
		    XCopyArea(MainWindow.theDisp,sparkle_frames[frame_list[i]],
			      workpix2,tmpGC,
			      0,0,MainWindow.Width,MainWindow.Height,0,0);
		    XFreeGC(MainWindow.theDisp, tmpGC);
		    XSetClipMask(MainWindow.theDisp, theGC, workpix2);
#ifdef CLIP_ALL_BUT_CFADE
		} else if (clipping) {
		    copy_pix(sparkle_frames[frame_list[i]],workpix);
		    XSetClipMask(MainWindow.theDisp, theGC, workpix);
#endif
		} else {
		    XSetClipMask(MainWindow.theDisp, theGC, sparkle_frames[frame_list[i]]);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			   theGC,ox,oy,width,height,
			   screen_x(x),screen_y(y,height));
		sit_for(del);
	    }
	    XFreeGC(MainWindow.theDisp, theGC);
	    break;
#endif /* !FAST_SPARKLE */
	case EFFECT_DIAGONAL:
	    if (speed<(2*minframes*(mindel?mindel:1)))
		speed =  2*minframes*(mindel?mindel:1);
	    nframes = speed/2/(mindel?mindel:1);
	    del = speed/2/nframes;
	    XSetTile(MainWindow.theDisp,MainWindow.tileGC,r->p);
	    XSetTSOrigin(MainWindow.theDisp,MainWindow.tileGC,
			 screen_x(x)-ox,screen_y(y,height)-oy);
	    xstep = width/nframes;
	    ystep = height/nframes;
	    if ((width/xstep) > (height/ystep))
		nframes = height/ystep;
	    else
		nframes = width/xstep;
	    par[0].x = screen_x(x);
	    par[0].y = screen_y(y,height) + height;
	    par[1].x = par[0].x;
	    par[2].y = screen_y(y,height)+height;
	    par[3].y = screen_y(y,height)+height;
	    par[3].x = screen_x(x);
	    for (i=0;i<nframes;i++) {
		par[0].y -= ystep;
		par[1].y = par[0].y + ystep;
		par[2].x = par[3].x;
		par[3].x = par[2].x + xstep;
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      par[0].x, par[0].y + ystep/2,
			      par[2].x + xstep/2, par[3].y);
		    sit_for(del);
		}
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,par,4,Convex,CoordModeOrigin);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if ((i*xstep < width) || (i*ystep < height)) {
		par[1].y = par[0].y;
		par[0].y = screen_y(y,height);
		par[2].x = par[3].x;
		par[3].x = screen_x(x) + width;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,par,4,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    par[0].y = screen_y(y,height);
	    par[1].x = screen_x(x);
	    par[1].y = screen_y(y,height);
	    par[2].x = screen_x(x)+width;
	    par[2].y = screen_y(y,height) + height;
	    par[3].x = par[2].x;
	    for (i=0;i<nframes;i++) {
		par[0].x = par[1].x;
		par[1].x = par[0].x + xstep;
		par[2].y -= ystep;
		par[3].y = par[2].y + ystep;
		if (edge_mode) {
		    XDrawLine(MainWindow.theDisp, MainWindow.theWindow, theGC,
			      par[0].x + xstep/2, par[1].y,
			      par[2].x, par[2].y + ystep/2);
		    sit_for(del);
		}
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,par,4,Convex,CoordModeOrigin);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if ((i*xstep < width) || (i*ystep < height)) {
		par[0].x = par[1].x;
		par[1].x = screen_x(x) + width;
		par[3].y = par[2].y;
		par[2].y = screen_y(y,height);
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,par,4,Convex,CoordModeOrigin);
	    }
	    break;
	case EFFECT_APERTURE_E_C:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1)/2;
	    xstep = width/2/nframes;
	    ystep = height/2/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
		nframes = width/2/xstep;
		ystep = height/2/nframes;
	    }
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
		nframes = height/2/ystep;
		xstep = width/2/nframes;
		if (xstep < MainWindow.pixwid)
		    xstep = MainWindow.pixwid;
	    }
	    if ((width/2/xstep) > (height/2/ystep))
		nframes = height/2/ystep;
	    else
		nframes = width/2/xstep;
	    del = speed/nframes;
	    for (i=0;i<nframes;i++) {
		if (edge_mode) {
		    XDrawRectangle(MainWindow.theDisp,MainWindow.theWindow,
			theGC,
			screen_x(x)+(i*xstep)+xstep/2,
			screen_y(y,height)+i*ystep+ystep/2,
			width - (2*i+1)*xstep,
			height - (2*i+1)*ystep);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i*xstep),oy+i*ystep,xstep,height-2*i*ystep,
			  screen_x(x)+(i*xstep),screen_y(y,height)+i*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width-((i+1)*xstep),oy+i*ystep,
			  xstep,height-2*i*ystep,
			  screen_x(x)+width-((i+1)*xstep),
			  screen_y(y,height)+i*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i+1)*xstep,oy+height-((i+1)*ystep),
			  width-2*(i+1)*xstep,ystep,
			  screen_x(x)+(i+1)*xstep,
			  screen_y(y,height)+height-(i+1)*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i+1)*xstep,oy+(i*ystep),
			  width-2*(i+1)*xstep,ystep,
			  screen_x(x)+(i+1)*xstep,
			  screen_y(y,height)+i*ystep);
		if (!edge_mode)
		    sit_for(del);
	    }
	    if ((2*i*xstep < width) || (2*i*ystep < height)) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+(i*xstep),oy+(i*ystep),
			  width-(2*i-1)*xstep, height-(2*i-1)*ystep,
			  screen_x(x)+i*xstep,screen_y(y,height)+i*ystep);
	    }
	    break;
	case EFFECT_APERTURE_C_E:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1)/2;
	    xstep = width/2/nframes;
	    ystep = height/2/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
		nframes = width/2/xstep;
		ystep = height/2/nframes;
	    }
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixht;
		nframes = height/2/ystep;
		xstep = width/2/nframes;
		if (xstep < MainWindow.pixwid)
		    xstep = MainWindow.pixwid;
	    }
	    xnframes = width/2/xstep;
	    ynframes = height/2/ystep;
	    del = speed/nframes;
	    for (i=j=0;(i<xnframes) || (j<ynframes);) {
		if (edge_mode) {
		    XDrawRectangle(MainWindow.theDisp,MainWindow.theWindow,
			theGC,
			screen_x(x)+width/2-(i*xstep)-xstep/2,
			screen_y(y,height)+height/2-j*ystep-ystep/2,
			(2*i+1)*xstep, (2*j+1)*ystep);
		    sit_for(del);
		}
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,theGC,
			  ox+width/2+(i*xstep),oy+height/2-(j+1)*ystep,
			  xstep,2*(i+1)*ystep,
			  screen_x(x)+width/2+(i*xstep),
			  screen_y(y,height)+height/2-(j+1)*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,theGC,
			  ox+width/2-((i+1)*xstep),oy+height/2-(j+1)*ystep,
			  xstep,2*(j+1)*ystep,
			  screen_x(x)+width/2-((i+1)*xstep),
			  screen_y(y,height)+height/2-(j+1)*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2-i*xstep,oy+height/2-(j+1)*ystep,
			  2*i*xstep,ystep,
			  screen_x(x)+width/2-i*xstep,
			  screen_y(y,height)+height/2-(j+1)*ystep);
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2-i*xstep,oy+height/2+(j*ystep),
			  2*i*xstep,ystep,
			  screen_x(x)+width/2-i*xstep,
			  screen_y(y,height)+height/2+j*ystep);
		if (!edge_mode)
		    sit_for(del);
		if (i < xnframes)
		    i++;
		if (j < ynframes)
		    j++;
	    }
	    if ((2*i*xstep < width) || (2*j*ystep < height)) {
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox+width/2+(i*xstep),oy,
			  width-2*(i*xstep),height,
			  screen_x(x)+width/2+(i*xstep),
			  screen_y(y,height));
		if (i*xstep < width/2)
		    XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			      theGC,ox,oy,width-2*i*xstep,height,
			      screen_x(x),screen_y(y,height));
		if (j*ystep < height/2)
		    XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			      theGC,ox,oy,width,height-2*j*ystep,
			      screen_x(x),screen_y(y,height));
		XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
			  theGC,ox,oy+height/2+(j*ystep),
			  width,height-2*j*ystep,
			  screen_x(x),screen_y(y,height)+height/2+j*ystep);
	    }
	    break;
	case EFFECT_CLOCKWISE:
	    if (speed<(8*minframes*(mindel?mindel:1)))
		speed =  8*minframes*(mindel?mindel:1);
	    nframes = speed/8/(mindel?mindel:1);
	    XSetTile(MainWindow.theDisp,MainWindow.tileGC,r->p);
	    XSetTSOrigin(MainWindow.theDisp,MainWindow.tileGC,
			 screen_x(x)-ox,screen_y(y,height)-oy);
	    xstep = width/2/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/2/xstep;
	    del = speed/8/nframes;
	    tr1[0].x=screen_x(x) + width/2;
	    tr1[0].y=screen_y(y,height)+height/2;
	    tr1[1].y=screen_y(y,height);
	    tr1[2].x = screen_x(x) + width/2;
	    tr1[2].y=screen_y(y,height);
	    for (i=0;i<nframes;i++) {
		tr1[1].x = tr1[2].x;
		tr1[2].x = tr1[1].x + xstep;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    if (2*i*xstep < width) {
		tr1[1].x = tr1[2].x;
		tr1[2].x = screen_x(x) + width;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    ystep = height/2/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixwid;
	    }
	    nframes = height/2/ystep;
	    del = speed/8/nframes;
	    tr1[1].x=screen_x(x)+width;
	    tr1[2].x=screen_x(x)+width;
	    tr1[2].y = screen_y(y,height);
	    for (i=0;i<2*nframes;i++) {
		tr1[1].y = tr1[2].y;
		tr1[2].y = tr1[1].y + ystep;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    if (i*ystep < height) {
		tr1[1].y = tr1[2].y;
		tr1[2].y = screen_y(y,height) + height;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    xstep = width/2/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/2/xstep;
	    del = speed/8/nframes;
	    tr1[1].x = screen_x(x) + width;
	    tr1[1].y=screen_y(y,height)+height;
	    tr1[2].y=tr1[1].y;
	    for (i=0;i<2*nframes;i++) {
		tr1[1].x -= xstep;
		tr1[2].x = tr1[1].x + xstep;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    if (i*xstep < width) {
		tr1[2].x = tr1[1].x;
		tr1[1].x = screen_x(x);
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    ystep = height/2/nframes;
	    if (ystep < MainWindow.pixht) {
		ystep = MainWindow.pixwid;
	    }
	    nframes = height/2/ystep;
	    del = speed/8/nframes;
	    tr1[1].x=screen_x(x);
	    tr1[2].x=tr1[1].x;
	    tr1[1].y = screen_y(y,height) + height;
	    for (i=0;i<2*nframes;i++) {
		tr1[1].y -= ystep;
		tr1[2].y = tr1[1].y + ystep;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    if (i*ystep < height) {
		tr1[2].y = tr1[1].y;
		tr1[1].y = screen_y(y,height);
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    xstep = width/2/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/2/xstep;
	    del = speed/8/nframes;
	    tr1[1].y=screen_y(y,height);
	    tr1[2].x = screen_x(x);
	    tr1[2].y=tr1[1].y;
	    for (i=0;i<nframes;i++) {
		tr1[1].x = tr1[2].x;
		tr1[2].x = tr1[1].x + xstep;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    if (2*i*xstep < width) {
		tr1[1].x = tr1[2].x;
		tr1[2].x = screen_x(x) + width/2;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
	    }
	    break;
	case EFFECT_DOUBLE_SLANT:
	    if (speed<(minframes*(mindel?mindel:1)))
		speed =  minframes*(mindel?mindel:1);
	    nframes = speed/(mindel?mindel:1);
	    xstep = width/nframes;
	    if (xstep < MainWindow.pixwid) {
		xstep = MainWindow.pixwid;
	    }
	    nframes = width/xstep;
	    del = speed/nframes;
	    /* Triangle 1 handles upper half */
	    tr1[0].x=screen_x(x);
	    tr1[0].y=screen_y(y,height)+height;
	    tr1[1].y=screen_y(y,height);
	    tr1[2].x=screen_x(x) + width;
	    tr1[2].y=screen_y(y,height);
	    /* Triangle 2 handles lower half */
	    tr2[0].x=screen_x(x)+width;
	    tr2[0].y=screen_y(y,height);
	    tr2[1].y=screen_y(y,height)+height;
	    tr2[2].x=screen_x(x);
	    tr2[2].y=screen_y(y,height)+height;
	    XSetTile(MainWindow.theDisp,MainWindow.tileGC,r->p);
	    XSetTSOrigin(MainWindow.theDisp,MainWindow.tileGC,
			 screen_x(x)-ox,screen_y(y,height)-oy);
	    for (i=0;i<nframes;i++) {
		tr1[1].x = tr1[2].x;
		tr1[2].x = tr1[1].x - xstep;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		tr2[1].x = tr2[2].x;
		tr2[2].x = tr2[1].x + xstep;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr2,3,Convex,CoordModeOrigin);
		sit_for(del);
	    }
	    if (i*xstep < width) {
		tr1[1].x = tr1[2].x;
		tr1[2].x = screen_x(x);
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr1,3,Convex,CoordModeOrigin);
		tr2[1].x = tr2[2].x;
		tr2[2].x = screen_x(x) + width;
		XFillPolygon(MainWindow.theDisp,MainWindow.theWindow,
			     MainWindow.tileGC,tr2,3,Convex,CoordModeOrigin);
	    }
	    break;
	case EFFECT_SNAP: /* Synonym for EFFECT_NONE */
	    /* Fall through */
	default:
	    XCopyArea(MainWindow.theDisp,r->p,MainWindow.theWindow,
		       theGC,ox,oy,width,height,
		       screen_x(x),screen_y(y,height));
	    if (speed>0)
		sit_for(speed);
    }
    /* Handle Expose events now. */
    do_expose = TRUE;
    evt_handler(0);
    return TRUE;
}

BOOLEAN pan_preg(struct PIC_register *r, int x1, int y1, int x2, int y2,
		 char *err_msg)
{
    strcpy(err_msg,"Panning not yet implemented\n");
    return FALSE;
}

int evt_handler(int dummy)
{
static BOOLEAN expose_pending;
XEvent event;
char buf[24];
    flush_windows();
    if (expose_pending ||
	XCheckWindowEvent(MainWindow.theDisp, MainWindow.theWindow,
			  ExposureMask, &event)) {
	while (XCheckWindowEvent(MainWindow.theDisp, MainWindow.theWindow,
			         ExposureMask, &event));
	if (do_expose) {
	    redraw_screen();
	    expose_pending = FALSE;
	} else {
	    expose_pending = TRUE;
	}
    }
    while (XCheckWindowEvent(MainWindow.theDisp, MainWindow.theWindow,
			     KeyPressMask, &event)) {
	if (XLookupString(&event.xkey, buf, 24, NULL, NULL) == 0) {
	    continue;
	}
	if (buf[0] == '\033') {
	    cleanup();
	    exit(0);
	}
	if (buf[0] == ' ') {
	    paused = TRUE;
	    continue;
	}
	paused = FALSE;
	timer_intr(0);
    }
}

int get_keystroke(int timeout)
{
XEvent event;
char buf[24];
#ifdef HAS_ASYNC_INPUT
    XSelectAsyncInput(MainWindow.theDisp, MainWindow.theWindow,
		      ExposureMask|KeyPressMask, NULL, 0);
#endif
    timed_out=FALSE;
    if (timeout != -1)
	set_timer(timeout/100,(timeout%100)*10000);
    flush_windows();
    while (!timed_out) {
        if (XCheckWindowEvent(MainWindow.theDisp, MainWindow.theWindow,
			      KeyPressMask, &event)) {
	    if (XLookupString(&event.xkey, buf, 24, NULL, NULL) == 0) {
		continue;
	    }
	    if (buf[0] == '\033') {
		cleanup();
		exit(0);
	    }
	    if (buf[0] == ' ') {
		paused = TRUE;
		continue;
	    }
	    paused = FALSE;
	    if (buf[0] != '\033') {
#ifdef HAS_ASYNC_INPUT
		XSelectAsyncInput(MainWindow.theDisp, MainWindow.theWindow,
				  ExposureMask|KeyPressMask, evt_handler, 0);
#endif
		return buf[0];
	    }
	} else if (XCheckWindowEvent(MainWindow.theDisp, MainWindow.theWindow,
				     ExposureMask, &event)) {
	    while (XCheckWindowEvent(MainWindow.theDisp, MainWindow.theWindow,
				     ExposureMask, &event));
	    redraw_screen();
	}
    }
#ifdef HAS_ASYNC_INPUT
    XSelectAsyncInput(MainWindow.theDisp, MainWindow.theWindow,
		      ExposureMask|KeyPressMask, evt_handler, 0);
#endif
    return -1;
}

/* init_window opens the display, determines it relevant attributes,
   and creates the main window.  In the future, it may parse the command
   line for X-related options (a la XrmParseCommand), but for now, its
   arguments are not used. */
BOOLEAN init_window(int *argc, char **argv)
{
int i,nmatched,isave;
XVisualInfo vmatch;
XVisualInfo *vlist;

    if ((MainWindow.theDisp=XOpenDisplay(NULL)) == NULL) {
	perror("Error opening display");
	return FALSE;
    }
    MainWindow.theScreen = DefaultScreen(MainWindow.theDisp);
    MainWindow.theVisual =
	DefaultVisual(MainWindow.theDisp,MainWindow.theScreen);
    MainWindow.RootW	 =
	RootWindow(MainWindow.theDisp,MainWindow.theScreen);
    MainWindow.theWindow = MainWindow.RootW;

    vmatch.screen = MainWindow.theScreen;
    vmatch.depth = 8;
    vmatch.class = PseudoColor;

    vlist = XGetVisualInfo(MainWindow.theDisp,
	VisualClassMask | VisualScreenMask | VisualDepthMask,
	&vmatch,&nmatched);
    
    if (nmatched <= 0) {
	vmatch.depth = 4;
	vlist = XGetVisualInfo(MainWindow.theDisp,
	    VisualClassMask | VisualScreenMask | VisualDepthMask,
	    &vmatch,&nmatched);
	if (nmatched <= 0)
	    monochrome = TRUE;
    }

    if (monochrome) {
	MainWindow.theDepth = 2;
    } else {
	MainWindow.theDepth = vmatch.depth;
	for (i=0;i<(nmatched-1);i++)
	    if (vlist[i].visual == MainWindow.theVisual) {
		if (vlist[i].colormap_size == 256)
		    break;
		isave = i;
		for (i=0;i<(nmatched-1);i++)
		    if (vlist[i].colormap_size == 256)
			break;
		if (vlist[i].colormap_size != 256)
		    i = isave;
		break;
	    }
	MainWindow.theVisual = vlist[i].visual;
	MainWindow.maxcells = vlist[i].colormap_size;
	if (MainWindow.theVisual !=
	    DefaultVisual(MainWindow.theDisp,MainWindow.theScreen)) {
	    MainWindow.theCmap =
		XCreateColormap(MainWindow.theDisp, MainWindow.theWindow,
				MainWindow.theVisual, AllocNone);
	} else {
	    MainWindow.theCmap =
		DefaultColormap(MainWindow.theDisp,MainWindow.theScreen);
	}
	XFree((MyXPointer)vlist);
    }

    init_EGA_colors();
    for (i=0;i<MainWindow.maxcells;i++)
	pixarr[i] = (color_friendly? (MainWindow.maxcells-1-(i)) : (i));

    return TRUE;
}

void synchronize_display()
{
    XSynchronize(MainWindow.theDisp,True);
}

BOOLEAN start_window(struct screen_info *vid, byte mode, char *err_msg)
{
XEvent event;
XGCValues gcval;
XColor colcopy[MAXCOLOR];
XSetWindowAttributes winatts;
unsigned long pixels[MAXCOLOR];
int i,c;
BOOLEAN try_smaller;

    if (MainWindow.current_mode) {
	if (MainWindow.current_mode == mode) {
	    clear_window();
	    if (vid->init != NULL)
		vid->init();	/* Call mode initialization routine */
	    return TRUE;
	}
	for (i=0;i<NPAGES;i++)
	    XFreePixmap(MainWindow.theDisp, MainWindow.page[i]);
	if (!monochrome) {
	    XFreeColors(MainWindow.theDisp, MainWindow.theCmap,
			pixarr, MainWindow.maxcolor, 0);
	}
	XFreeGC(MainWindow.theDisp, MainWindow.overlayGC);
        XFreeGC(MainWindow.theDisp, MainWindow.clearbitsGC);
        XFreeGC(MainWindow.theDisp, MainWindow.copyGC);
        XFreeGC(MainWindow.theDisp, MainWindow.createGC);
        XFreeGC(MainWindow.theDisp, MainWindow.drawGC);
        XFreeGC(MainWindow.theDisp, MainWindow.clearGC);
	XDestroyWindow(MainWindow.theDisp, MainWindow.theWindow);
    }
    if (vid->maxcolor > (1<<MainWindow.theDepth)) {
	fprintf(stderr,"Have %d colors, need %d colors!\n",
		MainWindow.maxcells,vid->maxcolor);
	sprintf(err_msg,"Video type '%c' not supported on this monitor",mode);
	return FALSE;
    }
    MainWindow.maxcolor = (MainWindow.maxcells < vid->maxcolor)?
			      MainWindow.maxcells :
			      vid->maxcolor;
    MainWindow.cscalemax = vid->cmax;
    MainWindow.xcmult = vid->xcmult;
    MainWindow.ycmult = vid->ycmult;
    if (!fixaspect) {
	MainWindow.pixwid = xaspect;
	MainWindow.pixht = yaspect;
    } else {
	MainWindow.pixwid = vid->xmult;
	MainWindow.pixht = vid->ymult;
    }
    MainWindow.xoffs = MainWindow.yoffs = 0;
    MainWindow.Width = vid->width*MainWindow.pixwid;
    MainWindow.Height = vid->height*MainWindow.pixht;
    MainWindow.xloc =
	    (DisplayWidth(MainWindow.theDisp, MainWindow.theScreen) -
	     MainWindow.Width)/2;
    MainWindow.yloc =
	    (DisplayHeight(MainWindow.theDisp, MainWindow.theScreen) -
	     MainWindow.Height)/2;

    /* Create the window */
    winatts.background_pixel = winatts.border_pixel = 0;
    winatts.colormap = MainWindow.theCmap;
    winatts.backing_store = NotUseful;
    MainWindow.theWindow = XCreateWindow(
	MainWindow.theDisp, MainWindow.RootW,
	MainWindow.xloc, MainWindow.yloc,
	MainWindow.Width, MainWindow.Height,
	0, MainWindow.theDepth,
	InputOutput, MainWindow.theVisual,
	CWBackPixel | CWBorderPixel | CWBackingStore |
	(monochrome?0:CWColormap), 
	&winatts);

    for (i=0;i<NPAGES;i++) {
	MainWindow.page[i] = XCreatePixmap(
	    MainWindow.theDisp, MainWindow.theWindow,
	    MainWindow.Width, MainWindow.Height, MainWindow.theDepth);
    }
    MainWindow.thePix = MainWindow.winPix = MainWindow.page[0];
    XStoreName(MainWindow.theDisp, MainWindow.theWindow, WindowName);
    XSetIconName(MainWindow.theDisp, MainWindow.theWindow, IconName);
    if (monochrome) {
	color_friendly=FALSE;
	MainWindow.maxcolor = 2;
    } else {
	if (XAllocColorCells(MainWindow.theDisp, MainWindow.theCmap, FALSE,
		 NULL,0,MainWindow.pixvals,
		 MainWindow.maxcolor) == 0) {
	    if (MainWindow.theCmap ==
		DefaultColormap(MainWindow.theDisp,MainWindow.theScreen)) {
		MainWindow.theCmap =
		    XCreateColormap(MainWindow.theDisp, MainWindow.theWindow,
				    MainWindow.theVisual, AllocNone);
		try_smaller = FALSE;
	    } else {
		try_smaller = TRUE;
	    }
	    if (try_smaller ||
		XAllocColorCells(MainWindow.theDisp, MainWindow.theCmap, FALSE,
		     NULL,0,MainWindow.pixvals,
		     MainWindow.maxcolor) == 0) {
		/* Some platforms reserve cells 254 and 255...  maxcells should
		   handle this case, but I like to be on the safe side... */
		MainWindow.maxcolor -= 2;
		if (XAllocColorCells(MainWindow.theDisp, MainWindow.theCmap,
				     FALSE, NULL, 0, MainWindow.pixvals,
				     MainWindow.maxcolor) == 0) {
		    sprintf(err_msg,
			    "Video mode %c not supported on this system",mode);
		    return FALSE;
		}
	    }
	    for (i=0;i<MainWindow.maxcolor;i++)
		colcopy[i].pixel = MainWindow.pixvals[i];
	    XQueryColors(MainWindow.theDisp,
			 DefaultColormap(MainWindow.theDisp,
					 MainWindow.theScreen), colcopy,
					 MainWindow.maxcolor);
	    XStoreColors(MainWindow.theDisp,MainWindow.theCmap,colcopy,
			 MainWindow.maxcolor);
	    XSetWindowColormap(MainWindow.theDisp,MainWindow.theWindow,
		MainWindow.theCmap);
	} else {
	/* If we got the color from the default map, there's no need to try to
	   play nice with other colormaps. */
	    color_friendly=FALSE;
	}
    }
    for (i=0;i<MainWindow.maxcolor;i++)
	pixarr[i] = MainWindow.pixvals[(color_friendly?
					   (MainWindow.maxcolor-1-(i)) : (i))];
    MainWindow.fcol=index_to_pixel(1);
    MainWindow.bcol=index_to_pixel(0);
    gcval.line_width=MainWindow.pixwid;
    gcval.graphics_exposures = False;
    gcval.function=GXandInverted;
    gcval.background=0;
    gcval.foreground=0xffffffff;
    MainWindow.clearbitsGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow,
	GCForeground|GCBackground|GCFunction|GCGraphicsExposures, &gcval);
    gcval.function=GXcopy;
    MainWindow.createGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow,
	GCForeground|GCBackground|GCFunction|GCGraphicsExposures, &gcval);
    MainWindow.copyGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow,
	GCForeground|GCBackground|GCFunction|GCGraphicsExposures, &gcval);
    MainWindow.tileGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow,
	GCForeground|GCBackground|GCFunction|GCGraphicsExposures, &gcval);
    XSetFillStyle(MainWindow.theDisp,MainWindow.tileGC,FillTiled);
    MainWindow.edgeGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow,
	GCForeground|GCBackground|GCFunction|GCGraphicsExposures, &gcval);
    MainWindow.drawGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow,
	GCForeground|GCBackground|GCFunction|GCGraphicsExposures, &gcval);
    MainWindow.overlayGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow,
	GCForeground|GCBackground|GCFunction|GCGraphicsExposures, &gcval);
    gcval.foreground=index_to_pixel(0);
    MainWindow.clearGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow,
	GCForeground|GCBackground|GCFunction|GCGraphicsExposures, &gcval);
    for (i=0;i<NPAGES;i++) {
	XFillRectangle(MainWindow.theDisp,MainWindow.page[i],MainWindow.clearGC,
		       0,0,MainWindow.Width,MainWindow.Height);
    }
    for (i=0; i<MAXPICREG; i++) {
	for (c=0; c<MAXCOLOR; c++) {
	    CMaps[i].cindx[c] = -1;
	}
	CMaps[i].ncolors = 0;
	preg[i].cmap = &CMaps[i];
    }
    current_cmap = CMaps;
    if (vid->init != NULL)
	vid->init();	/* Call mode initialization routine */
    XSelectInput(MainWindow.theDisp, MainWindow.theWindow,
		 ExposureMask|KeyPressMask);
#ifdef HAS_ASYNC_INPUT
    XSelectAsyncInput(MainWindow.theDisp, MainWindow.theWindow,
		      ExposureMask|KeyPressMask, evt_handler, 0);
#endif
    if (!MainWindow.current_mode) {
	curfont = &fonts[0];
	if (!set_font_default(curfont,err_msg))
	    return FALSE;;
    }
    MainWindow.current_mode = mode;
    clear_window();
    set_current_cmap_entry(0,0,0,0);
    set_current_cmap_entry(1,CGA_color[7].red,CGA_color[7].green,CGA_color[7].blue);
    XMapWindow(MainWindow.theDisp, MainWindow.theWindow);
    if (install_cmap && !monochrome)
	XInstallColormap(MainWindow.theDisp, MainWindow.theCmap);
    return set_clipping_region(-1,0,MainWindow.Width/MainWindow.pixwid,
			       MainWindow.Height/MainWindow.pixht, err_msg);
    return FALSE;
}


void flush_windows()
{
    XSync(MainWindow.theDisp,False);
}

void cleanup()
{
    if (install_cmap && !monochrome)
	XUninstallColormap(MainWindow.theDisp, MainWindow.theCmap);
    XCloseDisplay(MainWindow.theDisp);
}

void beep()
{
    if (!quiet_mode)
	XBell(MainWindow.theDisp,0);
}

void drawbox(int x1, int y1, int x2, int y2, int width)
{
XGCValues gcval;
int ht;
    x1 += width/2;
    y1 += width/2;
    x2 -= width/2;
    y2 -= width/2;
    ht = (y2-y1)*MainWindow.pixht;
    gcval.line_width = width*MainWindow.pixwid;
    XChangeGC(MainWindow.theDisp, MainWindow.drawGC, GCLineWidth, &gcval);
    XDrawRectangle(MainWindow.theDisp, MainWindow.thePix, MainWindow.drawGC,
		   screen_x(x1), screen_y(y1,ht),
		   (x2-x1)*MainWindow.pixwid, ht);
    if (window_active) {
	XDrawRectangle(MainWindow.theDisp, MainWindow.theWindow,
		       MainWindow.drawGC,
		       screen_x(x1), screen_y(y1,ht),
		       (x2-x1)*MainWindow.pixwid, ht);
    }
    gcval.line_width = MainWindow.pixwid;
    XChangeGC(MainWindow.theDisp, MainWindow.drawGC, GCLineWidth, &gcval);
}

void drawellipse(int cx, int cy, int rx, int ry)
{

    XDrawArc(MainWindow.theDisp, MainWindow.thePix, MainWindow.drawGC,
	     screen_x(cx),screen_y(cy,0),
	     screen_x(rx),screen_y(ry,0),0,360*64);
    if (window_active) {
	XDrawArc(MainWindow.theDisp, MainWindow.theWindow, MainWindow.drawGC,
		 screen_x(cx),screen_y(cy,0),
		 screen_x(rx),screen_y(ry,0),0,360*64);
    }

}

void drawpoint(int x, int y)
{
    XDrawPoint(MainWindow.theDisp, MainWindow.thePix, MainWindow.drawGC,
	       screen_x(x), screen_y(y,0));
    if (window_active) {
	XDrawPoint(MainWindow.theDisp, MainWindow.theWindow, MainWindow.drawGC,
		   screen_x(x), screen_y(y,0));
    }
}

void drawline(int x1, int y1, int x2, int y2)
{
    XDrawLine(MainWindow.theDisp, MainWindow.thePix, MainWindow.drawGC,
	      screen_x(x1), screen_y(y1,0),
	      screen_x(x2), screen_y(y2,0));
    if (window_active) {
	XDrawLine(MainWindow.theDisp, MainWindow.theWindow, MainWindow.drawGC,
		  screen_x(x1), screen_y(y1,0),
		  screen_x(x2), screen_y(y2,0));
    }
}

BOOLEAN setup_creg_for_display(struct PIC_register *r)
{
    /* Do nothing */
    return TRUE;
}

void clear_creg(struct PIC_register *r)
{
    if (r->hascmap)
	r->cmap->last_upd=0;
    r->xoffs = r->yoffs = 0;
    if (r->haspixmap) {
	XFreePixmap(MainWindow.theDisp, r->p);
	if (r->img != NULL)
	    XDestroyImage(r->img);
	r->img = NULL;
	r->haspixmap = FALSE;
	if (r->is_trans) {
	    XFreePixmap(MainWindow.theDisp, r->clippix);
	    XFreeGC(MainWindow.theDisp, r->trans_GC);
	}
	r->is_trans = FALSE;
	r->force_trans = FALSE;
    }
}

void clear_preg(struct PIC_register *r)
{
    clear_creg(r);
}

void redraw_screen()
{
    XCopyArea(MainWindow.theDisp,MainWindow.winPix,MainWindow.theWindow,
	      MainWindow.createGC,0,0,MainWindow.Width,MainWindow.Height,0,0);
    flush_windows();
}

void clear_window()
{
char dummy[132];
    set_clipping_region(-1,0,MainWindow.Width/MainWindow.pixwid,
			MainWindow.Height/MainWindow.pixht, dummy);
    clipping=FALSE;
    XFillRectangle(MainWindow.theDisp,MainWindow.thePix,MainWindow.clearGC,
		   0,0,MainWindow.Width,MainWindow.Height);
    if (window_active)
	XFillRectangle(MainWindow.theDisp,MainWindow.theWindow,
		       MainWindow.clearGC,
		       0,0,MainWindow.Width,MainWindow.Height);
}

void set_edge_color(int color)
{
    dither_pix(color,current_cmap);
    XSetForeground(MainWindow.theDisp, MainWindow.edgeGC,
		   index_to_pixel(color));
}

void set_current_drawcolor(int color)
{
    dither_pix(color,current_cmap);
    update_current_cmap(color);
    current_drawcolor=color;
    XSetForeground(MainWindow.theDisp, MainWindow.drawGC,index_to_pixel(color));
}

void set_current_drawcolor2(int color)
{
    dither_pix(color,current_cmap);
    update_current_cmap(color);
    current_drawcolor2=color;
    XSetBackground(MainWindow.theDisp, MainWindow.drawGC,index_to_pixel(color));
}

void update_current_cmap(int maxv)
{
int i,m=0;
    if (monochrome) return;
    for (i=0;i<=maxv;i++) {
	if (current_cmap->cindx[i]>m) {
	    m=current_cmap->cindx[i];
	}
    }
    m-=current_cmap->last_upd-1;
    if (m>0) {
	XStoreColors(MainWindow.theDisp, MainWindow.theCmap,
	       current_cmap->clist+current_cmap->last_upd,
	       (m<(current_cmap->ncolors-current_cmap->last_upd))?
		    m:(current_cmap->ncolors-current_cmap->last_upd));
	current_cmap->last_upd += m;
    }
}

void copy_cmap(CMap dst, CMap src)
{
int i;
    if (monochrome) return;
    for (i=0;i<=src->ncolors;i++) {
	dst->cindx[i] = src->cindx[i];
	memcpy(&dst->clist[src->cindx[i]],&src->clist[src->cindx[i]],sizeof(XColor));
    }
    dst->last_upd = 0;
    dst->ncolors = src->ncolors;
}

void activate_current_cmap(int maxv)
{
int i,m=0;
    if (monochrome) return;
    for (i=0;i<=maxv;i++) {
	if (current_cmap->cindx[i]>m) {
	    m=current_cmap->cindx[i];
	}
    }
    m++;
    if (m<current_drawcolor)
	m = current_drawcolor;
    current_cmap->last_upd = m;
    XStoreColors(MainWindow.theDisp, MainWindow.theCmap,
	   current_cmap->clist, (m<current_cmap->ncolors)?
				    m:current_cmap->ncolors);
}

void set_current_cmap_entry(int cnum, unsigned int red, unsigned int green,
		       unsigned int blue)
{
int i;

    if (monochrome) return;
    if (cnum < MainWindow.maxcolor) {
	if ((i=current_cmap->cindx[cnum]) == -1) {
	    i = current_cmap->ncolors++;
	    current_cmap->clist[i].flags = DoRed|DoGreen|DoBlue;
	    current_cmap->cindx[cnum] = i;
	}
	current_cmap->clist[i].pixel = index_to_pixel(cnum);
	current_cmap->clist[i].red = red;
	current_cmap->clist[i].green = green;
	current_cmap->clist[i].blue = blue;
	XStoreColor(MainWindow.theDisp, MainWindow.theCmap,
		    &current_cmap->clist[i]);
    } else {
	dither_pix(cnum,current_cmap);
    }

}

CMap convert_clist_to_cmap(byte *clist, int reg, unsigned cscalemax)
{
CMap map = &CMaps[reg];
int i;
    map->ncolors=1<<preg[reg].depth;
    for (i=0; i < map->ncolors; i++) {
	map->cindx[i]=i;
	map->clist[i].flags = DoRed|DoGreen|DoBlue;
	map->clist[i].pixel = index_to_pixel(i);
	map->clist[i].red = clist[i*3]*(MAXPIXEL/cscalemax);
	map->clist[i].green = clist[i*3+1]*(MAXPIXEL/cscalemax);
	map->clist[i].blue = clist[i*3+2]*(MAXPIXEL/cscalemax);
    }
    return map;
}

static byte reorder_bits[] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 
    0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0, 0x08, 0x88, 0x48, 0xc8, 
    0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 
    0x78, 0xf8, 0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4, 0x0c, 0x8c, 
    0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 
    0x3c, 0xbc, 0x7c, 0xfc, 0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 
    0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2, 
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 
    0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa, 0x06, 0x86, 0x46, 0xc6, 
    0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 
    0x76, 0xf6, 0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe, 0x01, 0x81, 
    0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 
    0x31, 0xb1, 0x71, 0xf1, 0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 
    0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9, 
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 
    0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5, 0x0d, 0x8d, 0x4d, 0xcd, 
    0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 
    0x7d, 0xfd, 0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3, 0x0b, 0x8b, 
    0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 
    0x3b, 0xbb, 0x7b, 0xfb, 0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 
    0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7, 
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 
    0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

BOOLEAN set_font_default(MyFont *font, char *err_msg)
{
    font->charcnt = deffont.cnt;
    font->width = deffont.wid;
    font->height = deffont.ht;
    font->firstch = deffont.fc;
    return create_font(font,deffont.data,err_msg);
}

BOOLEAN create_font(MyFont *font, byte *p, char *err_msg)
{
unsigned int linesize=curfont->width*MainWindow.pixwid;
int fontht=curfont->height*curfont->charcnt;
byte *data = calloc(((linesize+7)>>3)*fontht*MainWindow.pixht,1);
byte *q;
unsigned pix;
int x,y,px,py,dx,i,j;
BOOLEAN got_bit;

    if (font->active) {
	dispose_font(font);
    }
    font->img = XCreateImage(MainWindow.theDisp, MainWindow.theVisual,
			       1, XYBitmap, 0, (char *)data, linesize,
			       MainWindow.pixht*fontht, 8, 0);
    /* The following is adapted from xgrasp.c */
    q=data;
    font->cwidths = malloc(sizeof(*font->cwidths)*font->charcnt);
    for (i=0;i<font->charcnt;i++) {
	font->cwidths[i].firstx=font->width;
	font->cwidths[i].lastx=0;
    }
    for(y=0;y<fontht;y++) {
	i = y/font->height;
	for(py=0;py<MainWindow.pixht;py++) {
	    for (got_bit=FALSE,x=0;x<font->width;x++) {
		pix=bit_at(p,x);
		if (pix) {
		    dx=(x*MainWindow.pixwid);
		    for (px=0;px<MainWindow.pixwid;px++) {
			put_bit(q,dx+px);
		    }
		    if (got_bit && (x>font->cwidths[i].lastx)) {
			font->cwidths[i].lastx=x;
		    } else if (x<font->cwidths[i].firstx) {
			font->cwidths[i].firstx=x;
		    }
		    got_bit=TRUE;
		}
	    }
	    q += font->img->bytes_per_line;
	}
	p += (font->width + 7) >> 3;
    }
    for (i=0;i<font->charcnt;i++) {
	font->cwidths[i].lastx *= MainWindow.pixwid;
	font->cwidths[i].lastx += MainWindow.pixwid-1;
	font->cwidths[i].firstx *= MainWindow.pixwid;
	font->cwidths[i].width=font->cwidths[i].lastx -
				 font->cwidths[i].firstx + 1;
    }
    if ((' ' > font->firstch) && ((' '-font->firstch) < font->charcnt)) {
	font->cwidths[' '-font->firstch].firstx=0;
	font->cwidths[' '-font->firstch].lastx=font->width/2;
	font->cwidths[' '-font->firstch].width=font->width/2;
    }
    font->active = TRUE;
    font->cgap = font->def_cgap = 2*MainWindow.pixwid;
    font->sgap = font->def_sgap = font->width/2+font->cgap;
    font->width *= MainWindow.pixwid;
    font->height *= MainWindow.pixht;

    return TRUE;
}

void dispose_font(MyFont *font)
{
    if (font->active) {
	XDestroyImage(font->img);
	free(font->cwidths);
	font->active = FALSE;
    }
}

/* x and y are in screen coordinates. */
void put_char(char c, int x, int y)
{
Pixmap tmppix;
int chrsize;
int x2,y2;
BOOLEAN shadow;
GC theGC;

    if (!curfont->active)
	set_font_default(curfont,NULL);	/* Just core dump if it fails. */
    chrsize=curfont->img->bytes_per_line*curfont->height;
    c -= curfont->firstch;
    tmppix = XCreateBitmapFromData(
		MainWindow.theDisp, MainWindow.theWindow,
		curfont->img->data + (c * chrsize),
		curfont->width, curfont->height);
    theGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow, 0, NULL);
    XCopyGC(MainWindow.theDisp, MainWindow.drawGC, ~0, theGC);
    XSetClipMask(MainWindow.theDisp, theGC, tmppix);
    if (curfont->style) {
	switch (curfont->style) {
	    case FSTYLE_BOLD_UP:
		x2 = x;
		y2 = y - MainWindow.pixht;
		shadow = FALSE;
		break;
	    case FSTYLE_BOLD_RIGHT:
		x2 = x + MainWindow.pixwid;
		y2 = y;
		shadow = FALSE;
		break;
	    case FSTYLE_SHADOW_UP_RIGHT:
		x2 = x + MainWindow.pixwid;
		y2 = y - MainWindow.pixht;
		shadow = TRUE;
		break;
	    case FSTYLE_SHADOW_UP_LEFT:
		x2 = x - MainWindow.pixwid;
		y2 = y - MainWindow.pixht;
		shadow = TRUE;
		break;
	    case FSTYLE_SHADOW_2UP_2RIGHT:
		x2 = x + 2*MainWindow.pixwid;
		y2 = y - 2*MainWindow.pixht;
		shadow = TRUE;
		break;
	    case FSTYLE_SHADOW_2UP_2LEFT:
		x2 = x - 2*MainWindow.pixwid;
		y2 = y - 2*MainWindow.pixht;
		shadow = TRUE;
		break;
	    default:
		x2 = x;
		y2 = y;
		break;
	}
	if ((x2!=x) || (y2!=y)) {
	    if (shadow)
		XSetForeground(MainWindow.theDisp, theGC,
			       index_to_pixel(current_drawcolor2));
	    XSetClipOrigin(MainWindow.theDisp,theGC,
			   x2-curfont->cwidths[c].firstx,y2);
	    XCopyPlane(MainWindow.theDisp, tmppix, MainWindow.thePix,
		       theGC, curfont->cwidths[c].firstx, 0,
		       curfont->cwidths[c].width, curfont->height,
		       x2, y2, 1);
	    if (window_active) {
		XCopyPlane(MainWindow.theDisp, tmppix, MainWindow.theWindow,
			   theGC, curfont->cwidths[c].firstx, 0,
			   curfont->cwidths[c].width,
			   curfont->height, x2,y2, 1);
	    }
	    if (shadow)
		XSetForeground(MainWindow.theDisp, theGC,
			       index_to_pixel(current_drawcolor));
	}
    }
    XSetClipOrigin(MainWindow.theDisp,theGC,
		   x-curfont->cwidths[c].firstx,y);
    XCopyPlane(MainWindow.theDisp, tmppix, MainWindow.thePix,
	       theGC, curfont->cwidths[c].firstx, 0,
	       curfont->cwidths[c].width, curfont->height,
	       x, y, 1);
    if (window_active) {
	XCopyPlane(MainWindow.theDisp, tmppix, MainWindow.theWindow,
		   theGC, curfont->cwidths[c].firstx, 0,
		   curfont->cwidths[c].width, curfont->height,
		   x,y, 1);
    }
    XFreeGC(MainWindow.theDisp, theGC);
    XFreePixmap(MainWindow.theDisp, tmppix);
}

BOOLEAN convert_img_for_screen(struct PIC_register *r, byte *dp, int trancolor, char *err_msg)
{
int linesize = r->width*MainWindow.pixwid;
byte *data = calloc(linesize*MainWindow.pixht*r->height,1),*q,*p=dp;
byte *clipdata = (trancolor == -1)?NULL:calloc(((r->width+7)>>3)*r->height,1);
int x,y,px,py,i;
unsigned long int pix;
XImage *img;
Pixmap tmppix;

    if (r->maxpval >= MainWindow.maxcolor) {
	/* Note that Clip registers do not get "dithered".  This may sometimes
	   (rarely) result in bad colors in Clip registers */
	for (i=MainWindow.maxcolor; i <= r->maxpval; i++) {
	    if (r->hascmap)
		dither_pix(i,r->cmap);
	}
    }
    /* For now, handle only depth 1 (CGA 2 color) and 8 (VGA).   after,
       we will need to handle depth 2 (CGA 4 color) and 4 (EGA 16/64).
       Although this could be done more generally using XGetPixel and
       XPutPixel, performance considerations make that impractical. */
    switch (r->depth) {

	case 1:
	    img = XCreateImage(MainWindow.theDisp, MainWindow.theVisual,
			    1, XYBitmap, 0, (char *)data, linesize,
			    MainWindow.pixht*r->height, 8, 0);
	    /* The following is adapted from xgrasp.c */
	    q=data+img->height*img->bytes_per_line ;
	    if (MainWindow.pixwid == 1) {

		for (y = 0; y < r->height; y++)
		{
		    for (py = 0; py < MainWindow.pixht; py++)
		    {
			q -= img->bytes_per_line;
			for (x=0;x<((r->width + 7) >> 3); x++)
			    q[x] = reorder_bits[p[x]];
		    }
		    p += (r->width + 7) >> 3;
		}

	    } else {

		for(y=0;y<r->height;y++) {
		    for(py=0;py<MainWindow.pixht;py++) {
			q -= img->bytes_per_line;
			for (x=0;x<r->width;x++) {
			    pix=bit_at(p,x);
			    if (pix) {
				for (px=0;px<MainWindow.pixwid;px++) {
				    put_bit(q,(x*MainWindow.pixwid)+px);
				}
			    }
			}
		    }
		    p += (r->width + 7) >> 3;
		}
	    }
	    if (trancolor != -1) {
		p = dp;
		q = clipdata+r->height*((r->width + 7) >> 3);
		for(y=0;y<r->height;y++) {
		    for(py=0;py<MainWindow.pixht;py++) {
			q -= (r->width + 7) >> 3;
			for (x=0;x<r->width;x++) {
			    pix=bit_at(p,x);
			    if ((pix != trancolor) &&
				(!transparent_mode || !is_transparent[pix])) {
				for (px=0;px<MainWindow.pixwid;px++) {
				    put_bit(q,(x*MainWindow.pixwid)+px);
				}
			    }
			}
		    }
		    p += (r->width + 7) >> 3;
		}
	    }

	    r->width *= MainWindow.pixwid;
	    r->height *= MainWindow.pixht;
	    if (r->haspixmap) {
		clear_creg(r);
	    }

	    r->p = XCreatePixmapFromBitmapData(
			MainWindow.theDisp, MainWindow.theWindow,
			(char *)data, r->width, r->height,
			index_to_pixel(1), index_to_pixel(0),
			MainWindow.theDepth);
	    r->force_trans = (trancolor != -1);
	    if (r->force_trans) {
		r->clippix = XCreateBitmapFromData(
			    MainWindow.theDisp, MainWindow.theWindow,
			    (char *)clipdata, r->width, r->height);
		r->trans_GC = XCreateGC(MainWindow.theDisp,
				  MainWindow.theWindow, 0, NULL);
		XCopyGC(MainWindow.theDisp, MainWindow.createGC,
			~0, r->trans_GC);
		XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
			GCForeground, r->trans_GC);
		XSetClipMask(MainWindow.theDisp, r->trans_GC, r->clippix);
		r->is_trans = TRUE;
	    }
	    r->haspixmap=TRUE;
	    break;

	case 2:
	    img = XCreateImage(MainWindow.theDisp, MainWindow.theVisual,
			    MainWindow.theDepth, ZPixmap, 0, (char *)data,
			    linesize, MainWindow.pixht*r->height, 8, linesize);
	    q=data+img->height*linesize;
	    for(y=0;y<r->height;y++) {
		for(py=0;py<MainWindow.pixht;py++) {
		    q -= linesize;
		    for (x=0;x<r->width;x++) {
			pix=index_to_pixel(twobits_at(p,x));
			for (px=0;px<MainWindow.pixwid;px++) {
			    if (MainWindow.theDepth == 8)
				q[x*MainWindow.pixwid+px]=pix;
			    else if (MainWindow.theDepth=4)
				q[(x*MainWindow.pixwid+px)/2]|=pix<<(((x*MainWindow.pixwid+px)%2)?4:0);
			    else {
				sprintf(err_msg,"Bad screen depth for image");
				return FALSE;
			    }
			}
		    }
		}
		p += (r->width+3)>>2;
	    }
	    if (trancolor != -1) {
		p = dp;
		q = clipdata+r->height*((r->width + 7) >> 3);
		for(y=0;y<r->height;y++) {
		    for(py=0;py<MainWindow.pixht;py++) {
			q -= (r->width + 7) >> 3;
			for (x=0;x<r->width;x++) {
			    pix=index_to_pixel(twobits_at(p,x));
			    if ((pix != trancolor) &&
				(!transparent_mode || !is_transparent[pix])) {
				for (px=0;px<MainWindow.pixwid;px++) {
				    put_bit(q,(x*MainWindow.pixwid)+px);
				}
			    }
			}
		    }
		    p += (r->width+3)>>2;
		}
	    }
	     
	    r->width *= MainWindow.pixwid;
	    r->height *= MainWindow.pixht;
	    if ((r->haspixmap) && (trancolor == -1)) {
		clear_creg(r);
	    }

	    r->p = XCreatePixmap(MainWindow.theDisp, MainWindow.theWindow,
				 r->width, r->height, MainWindow.theDepth);
	    XPutImage(MainWindow.theDisp, r->p, MainWindow.createGC, img,
		      0, 0, 0, 0, r->width, r->height);
	    r->haspixmap=TRUE;
	    r->force_trans = (trancolor != -1);
	    if (r->force_trans) {
		r->clippix = XCreateBitmapFromData(
			    MainWindow.theDisp, MainWindow.theWindow,
			    (char *)clipdata, r->width, r->height);
		r->trans_GC = XCreateGC(MainWindow.theDisp,
				  MainWindow.theWindow, 0, NULL);
		XCopyGC(MainWindow.theDisp, MainWindow.createGC,
			~0, r->trans_GC);
		XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
			GCForeground, r->trans_GC);
		XSetClipMask(MainWindow.theDisp, r->trans_GC, r->clippix);
		r->is_trans = TRUE;
	    }
	    break;

	case 4:
	    img = XCreateImage(MainWindow.theDisp, MainWindow.theVisual,
			    MainWindow.theDepth, ZPixmap, 0, (char *)data,
			    linesize, MainWindow.pixht*r->height, 8, linesize);
	    q=data+img->height*linesize;
	    for(y=0;y<r->height;y++) {
		for(py=0;py<MainWindow.pixht;py++) {
		    q -= linesize;
		    for (x=0;x<r->width;x++) {
			pix=index_to_pixel((p[x/2]&(0xf<<((1^x&1)*4)))>>((1^x&1)*4));
			for (px=0;px<MainWindow.pixwid;px++) {
			    if (MainWindow.theDepth == 8)
				q[x*MainWindow.pixwid+px]=pix;
			    else if (MainWindow.theDepth=4)
				q[(x*MainWindow.pixwid+px)/2]|=pix<<(((x*MainWindow.pixwid+px)%2)?4:0);
			    else {
				sprintf(err_msg,"Bad screen depth for image");
				return FALSE;
			    }
			}
		    }
		}
		p += (r->width+1)>>1;
	    }
	    if (trancolor != -1) {
		p = dp;
		q = clipdata+r->height*((r->width + 7) >> 3);
		for(y=0;y<r->height;y++) {
		    for(py=0;py<MainWindow.pixht;py++) {
			q -= (r->width + 7) >> 3;
			for (x=0;x<r->width;x++) {
			    pix=index_to_pixel((p[x/2]&(0xf<<((1^x&1)*4)))>>((1^x&1)*4));
			    if ((pix != trancolor) &&
				(!transparent_mode || !is_transparent[pix])) {
				for (px=0;px<MainWindow.pixwid;px++) {
				    put_bit(q,(x*MainWindow.pixwid)+px);
				}
			    }
			}
		    }
		    p += (r->width+1)>>1;
		}
	    }
	     
	    r->width *= MainWindow.pixwid;
	    r->height *= MainWindow.pixht;
	    if ((r->haspixmap) && (trancolor == -1)) {
		clear_creg(r);
	    }

	    r->p = XCreatePixmap(MainWindow.theDisp, MainWindow.theWindow,
				 r->width, r->height, MainWindow.theDepth);
	    XPutImage(MainWindow.theDisp, r->p, MainWindow.createGC, img,
		      0, 0, 0, 0, r->width, r->height);
	    r->haspixmap=TRUE;
	    r->force_trans = (trancolor != -1);
	    if (r->force_trans) {
		r->clippix = XCreateBitmapFromData(
			    MainWindow.theDisp, MainWindow.theWindow,
			    (char *)clipdata, r->width, r->height);
		r->trans_GC = XCreateGC(MainWindow.theDisp,
				  MainWindow.theWindow, 0, NULL);
		XCopyGC(MainWindow.theDisp, MainWindow.createGC,
			~0, r->trans_GC);
		XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
			GCForeground, r->trans_GC);
		XSetClipMask(MainWindow.theDisp, r->trans_GC, r->clippix);
		r->is_trans = TRUE;
	    }
	    break;
	case 8:
	    img = XCreateImage(MainWindow.theDisp, MainWindow.theVisual,
			    MainWindow.theDepth, ZPixmap, 0, (char *)data,
			    linesize, MainWindow.pixht*r->height, 8, linesize);
	    q=data+img->height*linesize;
	    if ((MainWindow.pixwid == 1) && !color_friendly) {
		for(y=0;y<r->height;y++) {
		    for(py=0;py<MainWindow.pixht;py++) {
			q -= linesize;
			memcpy(q, p, r->width);
		    }
		    p += r->width;
		}
	    } else { 
		for(y=0;y<r->height;y++) {
		    for(py=0;py<MainWindow.pixht;py++) {
			q -= linesize;
			for (x=0;x<r->width;x++) {
			    pix=index_to_pixel(p[x]);
			    for (px=0;px<MainWindow.pixwid;px++) {
				q[x*MainWindow.pixwid+px]=pix;
			    }
			}
		    }
		    p += r->width;
		}
	    }
	    if (trancolor != -1) {
		p = dp;
		q = clipdata+r->height*((r->width + 7) >> 3);
		for(y=0;y<r->height;y++) {
		    for(py=0;py<MainWindow.pixht;py++) {
			q -= (r->width + 7) >> 3;
			for (x=0;x<r->width;x++) {
			    pix=index_to_pixel(p[x]);
			    if ((pix != trancolor) &&
				(!transparent_mode || !is_transparent[pix])) {
				for (px=0;px<MainWindow.pixwid;px++) {
				    put_bit(q,(x*MainWindow.pixwid)+px);
				}
			    }
			}
		    }
		    p += r->width;
		}
	    }
	     
	    r->width *= MainWindow.pixwid;
	    r->height *= MainWindow.pixht;
	    if ((r->haspixmap) && (trancolor == -1)) {
		clear_creg(r);
	    }

	    r->p = XCreatePixmap(MainWindow.theDisp, MainWindow.theWindow,
				 r->width, r->height, MainWindow.theDepth);
	    XPutImage(MainWindow.theDisp, r->p, MainWindow.createGC, img,
		      0, 0, 0, 0, r->width, r->height);
	    r->haspixmap=TRUE;
	    r->force_trans = (trancolor != -1);
	    if (r->force_trans) {
		r->clippix = XCreateBitmapFromData(
			    MainWindow.theDisp, MainWindow.theWindow,
			    (char *)clipdata, r->width, r->height);
		r->trans_GC = XCreateGC(MainWindow.theDisp,
				  MainWindow.theWindow, 0, NULL);
		XCopyGC(MainWindow.theDisp, MainWindow.createGC,
			~0, r->trans_GC);
		XCopyGC(MainWindow.theDisp, MainWindow.edgeGC,
			GCForeground, r->trans_GC);
		XSetClipMask(MainWindow.theDisp, r->trans_GC, r->clippix);
		r->is_trans = TRUE;
	    }
	    break;
	default:
	    sprintf(err_msg,"Unrecognized image depth (%d)",r->depth);
	    return FALSE;
    }
    r->img = img;
    if (r->hascmap)
	r->cmap->last_upd=0;
    return TRUE;
}

BOOLEAN create_preg(struct PIC_register *r, int width, int height, char *err_msg)
{
    if (r->haspixmap) {
	clear_creg(r);
    }
    r->width = width*MainWindow.pixwid;
    r->height = height*MainWindow.pixht;
    r->hascmap = TRUE;
    r->depth = MainWindow.theDepth;
    copy_cmap(r->cmap,current_cmap);
    r->p = XCreatePixmap(MainWindow.theDisp, MainWindow.theWindow,
			 r->width, r->height, MainWindow.theDepth);
    r->haspixmap = TRUE;
    return TRUE;
}

BOOLEAN convert_gif_for_screen(struct PIC_register *r, byte *p, char *err_msg)
{
int linesize = r->width*MainWindow.pixwid;
byte *data = calloc(linesize*MainWindow.pixht*r->height,1),*q;
int x,y,px,py,dx=0,dy=0,i;
unsigned long int pix;
XImage *img;
Pixmap bitpix;

    if (r->maxpval >= MainWindow.maxcolor) {
	/* Note that Clip registers do not get "dithered".  This may sometimes
	   (rarely) result in bad colors in Clip registers */
	for (i=MainWindow.maxcolor; i <= r->maxpval; i++) {
	    if (r->hascmap)
		dither_pix(i,r->cmap);
	}
    }
    r->depth=8;
    img = XCreateImage(MainWindow.theDisp, MainWindow.theVisual,
		    MainWindow.theDepth, ZPixmap, 0, (char *)data,
		    linesize, MainWindow.pixht*r->height, 8, linesize);
    if (MainWindow.theDepth != 8) {
	for(y=0;y<r->height;y++) {
	    for(py=0;py<MainWindow.pixht;py++) {
		for (x=0;x<r->width;x++) {
		    pix=index_to_pixel(p[x]);
		    for (px=0;px<MainWindow.pixwid;px++) {
			XPutPixel(img,dx,dy,pix);
			dx++;
		    }
		}
		dy++;
	    }
	    p += r->width;
	}
    } else {
	q=data;
	for(y=0;y<r->height;y++) {
	    for(py=0;py<MainWindow.pixht;py++) {
		for (x=0;x<r->width;x++) {
		    pix=index_to_pixel(p[x]);
		    for (px=0;px<MainWindow.pixwid;px++) {
			*q = pix;
			q++;
		    }
		}
	    }
	    p += r->width;
	}
    }
	 
    r->width *= MainWindow.pixwid;
    r->height *= MainWindow.pixht;
    if (r->haspixmap) {
	clear_creg(r);
    }
    r->p = XCreatePixmap(MainWindow.theDisp, MainWindow.theWindow,
			 r->width, r->height, MainWindow.theDepth);
    r->haspixmap=TRUE;

    XPutImage(MainWindow.theDisp, r->p, MainWindow.createGC, img,
	      0, 0, 0, 0, r->width, r->height);
    r->img = img;
    if (r->hascmap)
	r->cmap->last_upd=0;
    return TRUE;
}

BOOLEAN setup_preg_for_display(struct PIC_register *r)
{
    /* Do Nothing */
    /* Later, this may need to do transparent mode processing. */
    return TRUE;
}

void restore_bg(int x1, int y1, struct PIC_register *was,
	        int x2, int y2, struct PIC_register *is)
{
int minx, miny, width, height;
Pixmap restore_pix, tmppix;
GC theGC;

    x1 = screen_x(x1);
    y1 = screen_y(y1,was->height);
    if (is == NULL) {
	if (transparent_mode || was->force_trans) {
	    XSetClipMask(MainWindow.theDisp, was->trans_GC, was->clippix);
	    XSetClipOrigin(MainWindow.theDisp,was->trans_GC,x1,y1);
	    theGC = was->trans_GC;
	} else {
	    theGC = MainWindow.copyGC;
	}
	XCopyArea(MainWindow.theDisp, MainWindow.winPix, MainWindow.theWindow,
		  theGC, x1, y1, was->width, was->height,x1,y1);
	return;
    }
    x2 = screen_x(x2);
    y2 = screen_y(y2,was->height);
    if (x1 > x2)
	minx=x2;
    else
	minx=x1;
    if (y1 > y2)
	miny=y2;
    else
	miny=y1;
    if ((x1+was->width) > (x2+is->width))
	width = x1 + was->width - minx;
    else
	width = x2 + is->width - minx;
    if ((y1+was->height) > (y2+is->height))
	height = y1 + was->height - miny;
    else
	height = y2 + is->height - miny;
    restore_pix = XCreatePixmap(MainWindow.theDisp,MainWindow.theWindow,
				width,height,1);
    theGC = XCreateGC(MainWindow.theDisp, restore_pix, 0, NULL);
    XSetForeground(MainWindow.theDisp, theGC, 0);
    XFillRectangle(MainWindow.theDisp, restore_pix, theGC,0,0,width,height);
    if (transparent_mode || was->force_trans) {
	XSetForeground(MainWindow.theDisp, theGC, 1);
	XCopyArea(MainWindow.theDisp, was->clippix, restore_pix, theGC,
		  0,0,was->width,was->height,x1-minx,y1-miny);
	XSetClipMask(MainWindow.theDisp, theGC, was->clippix);
	XSetClipOrigin(MainWindow.theDisp,theGC,x1-minx,y1-miny);
	XSetFunction(MainWindow.theDisp,theGC,GXxor);
	setup_for_trans_mode(is);
	XCopyArea(MainWindow.theDisp, is->clippix, restore_pix, theGC,
		  0,0,is->width,is->height,x2-minx,y2-miny);
	XFreeGC(MainWindow.theDisp, theGC);
	theGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow, 0, NULL);
	XCopyGC(MainWindow.theDisp, MainWindow.copyGC,
		~0, theGC);
	XSetClipMask(MainWindow.theDisp, theGC, restore_pix);
	XSetClipOrigin(MainWindow.theDisp,theGC,minx,miny);
	XCopyArea(MainWindow.theDisp, MainWindow.winPix, MainWindow.theWindow,
		  theGC,minx,miny,width,height,minx,miny);
    } else {
	tmppix = XCreatePixmap(MainWindow.theDisp,MainWindow.theWindow,
			       width,height,1);
	XFillRectangle(MainWindow.theDisp, restore_pix, theGC,0,0,width,height);
	XSetForeground(MainWindow.theDisp, theGC, 1);
	XFillRectangle(MainWindow.theDisp, tmppix, theGC,
		       x1-minx,y1-miny,was->width,was->height);
	XFillRectangle(MainWindow.theDisp, restore_pix, theGC,
		       x1-minx,y1-miny,was->width,was->height);
	XSetFunction(MainWindow.theDisp, theGC, GXxor);
	XSetClipMask(MainWindow.theDisp, theGC, tmppix);
	XSetClipOrigin(MainWindow.theDisp, theGC,0,0);
	XFillRectangle(MainWindow.theDisp, restore_pix, theGC,
		       x2-minx,y2-miny,is->width,is->height);
	XFreeGC(MainWindow.theDisp, theGC);
	theGC = XCreateGC(MainWindow.theDisp, MainWindow.theWindow, 0, NULL);
	XCopyGC(MainWindow.theDisp, MainWindow.copyGC,
		~0, theGC);
	XSetClipMask(MainWindow.theDisp, theGC, restore_pix);
	XSetClipOrigin(MainWindow.theDisp,theGC,minx,miny);
	XCopyArea(MainWindow.theDisp, MainWindow.winPix, MainWindow.theWindow,
		  theGC,minx,miny,width,height,minx,miny);
	XFreePixmap(MainWindow.theDisp,tmppix);
    }
    XFreeGC(MainWindow.theDisp,theGC);
    XFreePixmap(MainWindow.theDisp,restore_pix);
}

BOOLEAN set_clipping_region(int x1, int y1, int x2, int y2, char *err_msg)
{
XRectangle rectangle;
#ifdef CLIP_ALL_BUT_CFADE
    if (!clipping && (x1 == -1)) {
	return TRUE;
    }
#endif
    clipping=((x1 != -1) && !((x1 == 0) && (y1 == 0) &&
			      (x2 == (MainWindow.Width/MainWindow.pixwid/
				      MainWindow.xcmult)) &&
			      (y2 == (MainWindow.Height/MainWindow.pixht/
				      MainWindow.ycmult))));
    if (x1 == -1) {
#ifdef CLIP_ALL_BUT_CFADE
	x1 = 0;
	y1 = 0;
	x2 = MainWindow.Width/MainWindow.pixwid/MainWindow.xcmult;
	y2 = MainWindow.Height/MainWindow.pixht/MainWindow.ycmult;
#else
	clipx1 = clipxoffs = 0;
	clipy1 = clipxoffs = 0;
	clipx2 = MainWindow.Width;
	clipy2 = MainWindow.Height;
	return TRUE;
    } else {
	clipxoffs = x1;
	clipyoffs = y1;
#endif
    }
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = (x2-x1)*MainWindow.pixwid;
    rectangle.height = (y2-y1)*MainWindow.pixht;
    clipx1 = screen_x(x1);
    clipy1 = screen_y(y1,rectangle.height);
    clipx2 = clipx1 + rectangle.width;
    clipy2 = clipy1 + rectangle.height;
#ifdef CLIP_ALL_BUT_CFADE
    if (edge_mode) {
	XDrawRectangle(MainWindow.theDisp, MainWindow.thePix,
		       MainWindow.edgeGC, rectangle.x-1, rectangle.y-1,
		       rectangle.width+2, rectangle.height+2);
	if (window_active)
	    XDrawRectangle(MainWindow.theDisp, MainWindow.theWindow,
			   MainWindow.edgeGC, rectangle.x-1, rectangle.y-1,
			   rectangle.width+2, rectangle.height+2);
    }
    XSetClipRectangles(MainWindow.theDisp, MainWindow.drawGC,
	    screen_x(x1), screen_y(y1,rectangle.height), &rectangle,
	    1,YXSorted);
    XSetClipRectangles(MainWindow.theDisp, MainWindow.tileGC,
	    screen_x(x1), screen_y(y1,rectangle.height), &rectangle,
	    1,YXSorted);
    XSetClipRectangles(MainWindow.theDisp, MainWindow.edgeGC,
	    screen_x(x1), screen_y(y1,rectangle.height), &rectangle,
	    1,YXSorted);
    XSetClipRectangles(MainWindow.theDisp, MainWindow.copyGC,
	    screen_x(x1), screen_y(y1,rectangle.height), &rectangle,
	    1,YXSorted);
    XSetClipRectangles(MainWindow.theDisp, MainWindow.clearGC,
	    screen_x(x1), screen_y(y1,rectangle.height), &rectangle,
	    1,YXSorted);
    XSetClipRectangles(MainWindow.theDisp, MainWindow.clearbitsGC,
	    screen_x(x1), screen_y(y1,rectangle.height), &rectangle,
	    1,YXSorted);
#endif
    return TRUE;
}

BOOLEAN cycle_colors(int start, int end, int times, int delay)
{
int i,j,n,step=1;
unsigned long r, g, b;
    if (monochrome) return FALSE;
    if (start>end) step = -1;
    for(n=0;n<times;n++) {
	r = current_cmap->clist[current_cmap->cindx[start]].red;
	g = current_cmap->clist[current_cmap->cindx[start]].green;
	b = current_cmap->clist[current_cmap->cindx[start]].blue;
	for (i=start; (step==1)?(i<end):(i>end); i+= step) {
	    j = i+step;
	    if ((step==1)?(j<end):(j>end)) {
		current_cmap->clist[current_cmap->cindx[i]].red = 
		    current_cmap->clist[current_cmap->cindx[j]].red;
		current_cmap->clist[current_cmap->cindx[i]].green =
		    current_cmap->clist[current_cmap->cindx[j]].green;
		current_cmap->clist[current_cmap->cindx[i]].blue =
		    current_cmap->clist[current_cmap->cindx[j]].blue;
	    } else {
		current_cmap->clist[current_cmap->cindx[i]].red = r;
		current_cmap->clist[current_cmap->cindx[i]].green = g;
		current_cmap->clist[current_cmap->cindx[i]].blue = b;
	    }
	}
	XStoreColors(MainWindow.theDisp, MainWindow.theCmap,
	    &current_cmap->clist[current_cmap->cindx[(step==1)?start:end]],
	    (end-start)*step);
	sit_for(delay);
    }
    return TRUE;
}

BOOLEAN spread_colors(CMap c1, CMap c2, int rstep, int gstep, int bstep)
{
XColor clist[256];
int i;
BOOLEAN changed;
    if (c1 != current_cmap) {
	current_cmap = c1;
	activate_current_cmap(MainWindow.maxcolor-1);
    }
    for (i=0;i<c1->last_upd;i++) {
	clist[i].red = c1->clist[c1->cindx[i]].red;
	clist[i].green = c1->clist[c1->cindx[i]].green;
	clist[i].blue = c1->clist[c1->cindx[i]].blue;
	clist[i].flags = DoRed|DoGreen|DoBlue;
	clist[i].pixel = c1->clist[c1->cindx[i]].pixel;
    }
    if (rstep < 0)
	rstep *= -1;
    rstep = (rstep*MAXPIXEL)/MainWindow.maxcolor;
    if (gstep < 0)
	gstep *= -1;
    gstep = (gstep*MAXPIXEL)/MainWindow.maxcolor;
    if (bstep < 0)
	bstep *= -1;
    bstep = (bstep*MAXPIXEL)/MainWindow.maxcolor;
    do {
	changed = FALSE;
	for (i=0;i<c1->last_upd;i++) {
	    if (clist[i].red != c2->clist[c2->cindx[i]].red) {
		if (clist[i].red > c2->clist[c2->cindx[i]].red) {
		    clist[i].red -= rstep;
		    if (clist[i].red < c2->clist[c2->cindx[i]].red)
			clist[i].red = c2->clist[c2->cindx[i]].red;
		} else {
		    clist[i].red += rstep;
		    if (clist[i].red > c2->clist[c2->cindx[i]].red)
			clist[i].red = c2->clist[c2->cindx[i]].red;
		}
		if (clist[i].red > (MAXPIXEL-rstep))
		    clist[i].red = c2->clist[c2->cindx[i]].red;
		changed = TRUE;
	    }
	    if (clist[i].green != c2->clist[c2->cindx[i]].green) {
		if (clist[i].green > c2->clist[c2->cindx[i]].green) {
		    clist[i].green -= gstep;
		    if (clist[i].green < c2->clist[c2->cindx[i]].green)
			clist[i].green = c2->clist[c2->cindx[i]].green;
		} else {
		    clist[i].green += gstep;
		    if (clist[i].green > c2->clist[c2->cindx[i]].green)
			clist[i].green = c2->clist[c2->cindx[i]].green;
		}
		if (clist[i].green > (MAXPIXEL-gstep))
		    clist[i].green = c2->clist[c2->cindx[i]].green;
		changed = TRUE;
	    }
	    if (clist[i].blue != c2->clist[c2->cindx[i]].blue) {
		if (clist[i].blue > c2->clist[c2->cindx[i]].blue) {
		    clist[i].blue -= bstep;
		    if (clist[i].blue < c2->clist[c2->cindx[i]].blue)
			clist[i].blue = c2->clist[c2->cindx[i]].blue;
		} else {
		    clist[i].blue += bstep;
		    if (clist[i].blue > c2->clist[c2->cindx[i]].blue)
			clist[i].blue = c2->clist[c2->cindx[i]].blue;
		}
		if (clist[i].blue > (MAXPIXEL-bstep))
		    clist[i].blue = c2->clist[c2->cindx[i]].blue;
		changed = TRUE;
	    }
	}
	XStoreColors(MainWindow.theDisp, MainWindow.theCmap,
		     clist, c1->last_upd);
	sit_for(1);
    } while (changed);
    current_cmap = c2;
    c2->last_upd = c1->last_upd;
    return TRUE;
}

BOOLEAN set_disp_page(int visible, int drawing, char *err_msg)
{
static int drawing_window;
static int visible_window;
    if (visible < 0) {
	visible = drawing_window;
	drawing = visible_window;
    }
    if (visible >= NPAGES) {
	sprintf(err_msg,"Page number %d out of range",visible);
	return FALSE;
    }
    if ((drawing < 0) || (drawing >= NPAGES)) {
	sprintf(err_msg,"Page number %d out of range",drawing);
	return FALSE;
    }
    MainWindow.winPix = MainWindow.page[visible];
    MainWindow.thePix = MainWindow.page[drawing];
    window_active = (visible == drawing);
    drawing_window = drawing;
    visible_window = visible;
    redraw_screen();
    return TRUE;
}

BOOLEAN set_drawing_reg(int reg, char *err_msg)
{
struct PIC_register *r = &preg[reg];
    if (reg == 0) {
	MainWindow.thePix = MainWindow.winPix;
	window_active = TRUE;
	return TRUE;
    }
    if (!r->haspixmap) {
	sprintf(err_msg,"Can't draw to PIC register until it's initialized");
	return FALSE;
    }
    MainWindow.thePix = r->p;
    window_active = FALSE;
    return TRUE;
}

BOOLEAN get_reg(struct PIC_register *r, int x1, int y1,
			int x2, int y2, BOOLEAN get_cmap, char *err_msg)
{
    if (r->haspixmap)
	clear_creg(r);
    if (x1 == -1) {
	x1 = y1 = 0;
	r->width = MainWindow.Width;
	r->height = MainWindow.Height;
    } else {
	r->width = (x2-x1)*MainWindow.pixwid;
	r->height = (y2-y1)*MainWindow.pixht;
	x1 = screen_x(x1);
	y1 = screen_y(y1,r->height);
    }
    r->depth = MainWindow.theDepth;
    r->hascmap = get_cmap;
    if (get_cmap)
	copy_cmap(r->cmap,current_cmap);
    r->p = XCreatePixmap(MainWindow.theDisp, MainWindow.theWindow,
			 r->width, r->height, MainWindow.theDepth);
    r->haspixmap = TRUE;
    XCopyArea(MainWindow.theDisp,MainWindow.thePix,r->p,MainWindow.createGC,
	      x1,y1,r->width,r->height,0,0);
    return TRUE;
}

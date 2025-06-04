/* public include file for X postscript interface, ps.c.
 * requires stdio.h, Xlib.h and P_.h.
 */

extern void XPSAsk P_((int wantprint, char *title, void (go)()));
extern void XPSRegisterFont P_((Font fid, char *fname));
extern void XPSXBegin P_((Window win, int Xx0, int Xy0, int Xw, int Xh, int Px0,
    int Py0, int Pw, int fontsz));
extern void XPSXEnd P_((void));
extern void XPSDirect P_((char *s));
extern void XPSClose P_((void));

extern void XPSDrawArc P_((Display *dsp, Drawable win, GC gc, int x, int y,
    unsigned w, unsigned  h, int a1, int a2));
extern void XPSDrawArcs P_((Display *dsp, Drawable win, GC gc, XArc xa[], int
    nxa));
extern void XPSDrawLine P_((Display *dsp, Drawable win, GC gc, int x1, int x2,
    int y1, int y2));
extern void XPSDrawLines P_((Display *dsp, Drawable win, GC gc, XPoint xp[],
    int nxp, int mode));
extern void XPSDrawPoint P_((Display *dsp, Drawable win, GC gc, int x, int y));
extern void XPSDrawPoints P_((Display *dsp, Drawable win, GC gc, XPoint xp[],
    int nxp, int mode));
extern void XPSDrawRectangle P_((Display *dsp, Drawable win, GC gc,
    int x, int y, unsigned w, unsigned h));
extern void XPSDrawSegments P_((Display *dsp, Drawable win, GC gc,
    XSegment xs[], int nxs));
extern void XPSDrawString P_((Display *dsp, Drawable win, GC gc, int x, int y,
    char *s, int l));
extern void XPSFillArc P_((Display *dsp, Drawable win, GC gc, int x, int y,
    unsigned w, unsigned h, int a1, int a2));
extern void XPSFillArcs P_((Display *dsp, Drawable win, GC gc, XArc xa[], int
    nxa));
extern void XPSFillPolygon P_((Display *dsp, Drawable win, GC gc, XPoint xp[],
    int nxp, int shape, int mode));
extern void XPSFillRectangle P_((Display *dsp, Drawable win, GC gc, int x,
    int y, unsigned w, unsigned h));

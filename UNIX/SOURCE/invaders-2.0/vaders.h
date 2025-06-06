/* 
Copyright notice:

This is mine.  I'm only letting you use it.  Period.  Feel free to rip off
any of the code you see fit, but have the courtesy to give me credit.
Otherwise great hairy beasties will rip your eyes out and eat your flesh
when you least expect it.

Jonny Goldman <jonathan@think.com>

Wed May  8 1991
*/

/* vaders.h - definitions of vaders data structures. */

#ifndef _vaders_h
#define _vaders_h

#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>

/* #include <X11/Xutil.h> */
#include <X11/cursorfont.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Command.h>

#ifndef XtRFloat
#define XtRFloat "Float"
#endif

#define rint(x) ((int) (x))
#define MIN(x, y)	((x) < (y) ? (x) : (y))
#define MAX(x, y)	((x) > (y) ? (x) : (y))

#define VWIDTH 		240
#define IWIDTH		 75
#define VHEIGHT 	190

#ifndef M_PI
#define	M_PI	3.14159265358979323846
#define	M_PI_2	1.57079632679489661923
#define	M_PI_4	0.78539816339744830962
#endif

#ifdef MAIN
#define ext
#else
#define ext extern
#endif

/*
 * Definitions to make us act as a widget.
 */

/* New fields for the Vaders widget class record */
typedef struct {
     int mumble;   /* No new procedures */
} VadersClassPart;

/* Full class record declaration */
typedef struct _VadersClassRec {
    CoreClassPart       core_class;
    VadersClassPart    vaders_class;
} VadersClassRec;

extern VadersClassRec vadersClassRec;
extern WidgetClass vadersWidgetClass;

/* New fields for the Vaders widget record */
typedef struct _VadersPart {
    int dummy;
} VadersPart;



/* Full instance record declaration */

typedef struct _VadersRec {
    CorePart core;
    VadersPart vaders;
} VadersRec, *VadersWidget;




/*
 * Actual vaders definitions.
 */

ext Widget pausebutton, infobutton;
ext int scale;

ext Boolean debug;
ext int level;

ext Display *dpy;
ext Window gamewindow, labelwindow;
ext VadersWidget gamewidget, labelwidget;
ext int gamewidth, gameheight;
ext Widget toplevel;
ext int score;
ext int basesleft;

/* Base info: */

ext int basewait;		/* Number of milleseconds to wait between */
				/* moving base. */

ext XtIntervalId basetimerid;

ext Pixel basepixel;
ext Pixel buildingpixel;
ext Boolean basedestroyed;	/* TRUE if the base is non-existant */

/* Vader info: */

ext XtIntervalId vadertimerid;
ext Pixel vader1pixel;
ext Pixel vader2pixel;
ext Pixel vader3pixel;
ext int vaderwait;		/* Number of milleseconds to wait between */
				/* moving vaders. */

/* Spacer info */

ext Pixel spacerpixel;
ext XtIntervalId spacertimerid;
ext int spacerwait;		/* Number of milliseconds to wait between
				/* moving spacers. /*
ext int spacerappear;		/* same, but for the interval between appearances */

ext Boolean spacer_shown;    	/* Currnet_Spacer is something */
ext int spacer_counter;		/* number of cycles to create a spacer */

/* Shot info. */

ext XtIntervalId shottimerid;
ext XtIntervalId vshottimerid;
ext Pixel shotpixel;
ext Pixel vshotpixel;
ext int shotwait;
ext int vshotwait;
ext int maxshots;		/* How many shots are allowed to exist at */
				/* once. */
ext int maxvshots;		/* How many shots are allowed to exist at */
				/* once. */
ext int numshots;		/* how many shots (from the base) there are right now. */
ext int numvshots;		/* how many shots (from vaders) there are right now. */

/* Score info */

ext Pixel scorepixel;


ext int bases, nextbonus, hiscore, lastscore;


ext GC
  foregc, backgc, basegc, buildinggc, vadergc[3],
  shotgc, vshotgc, spacergc, scoregc;

ext XImage *me_image;

/* From widget.c */

extern Boolean TakeFocus();
extern void Quit();
extern void Pause();
extern int paused;
extern void SuspendTimers();
extern void EnableTimers();
extern void ShowInfo();

/* From base.c */

extern void MoveBase();
extern void MoveLeft();
extern void MoveRight();
extern void Stop();
extern void Fire();
extern Boolean ShotHitsBase();
extern Boolean ShotHitsBuilding();
extern void InitBase();
extern void PaintBase();
extern void DrawBuildings();

/* From vaders.c */

extern Boolean ShotHitsVader();
extern void MoveVaders();
extern void InitVaders();
extern void AddVShot();
extern void MoveVShots();
extern void CreateVaders();
extern void PaintAllVaders();

/* from spacer.c */

extern Boolean ShotHitsSpacer();
extern void MoveSpacer();
extern void MakeSpacer();
extern void InitSpacers();
extern void PaintSpacer();

/* From shot.c */

extern void AddLine();
extern void AddShot();
extern void AddVshot();
extern void MoveShots();
extern void MoveVshots();
extern void PaintAllShots();
ext Pixel defaultfore, defaultback;

ext char *vaderfont;

#endif _vaders_h

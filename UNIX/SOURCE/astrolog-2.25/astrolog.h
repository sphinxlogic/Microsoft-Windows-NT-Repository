/*
** Astrolog (Version 2.25) File: astrolog.h
** By Walter D. Pullen (cruiser1@milton.u.washington.edu)
** Programmed 8/28,30, 9/10,13,16,20,23, 10/3,6,7, 11/7,10,21/1991.
** Last revision made 6/2/1992.
** Eighth version posted to newsgroup alt.astrology.
**
** IMPORTANT: The planetary calculation routines used in this program
** have been Copyrighted and the core of this program is basically a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for our own purposes but not to sell them or profit
** from them in any way.
**
** IN ADDITION: the graphics database and chart display routines used in
** this program are Copyright (C) 1991-1992 by Walter D. Pullen. Permission
** is granted to freely use and distribute these routines provided one
** doesn't sell, restrict, or profit from them in any way. Modification
** is allowed provided these notices remain with any altered or edited
** versions of the program.
*/

/*
** To compile: For most systems, especially Unix based ones, the only changes
** that should need to be made to the code are to edit or (un)comment the eight
** #define's below to equal the particulars of your own locale and system:
*/

#define DEFAULT_LONG 122.20  /* Change these values to the longitude W and */
#define DEFAULT_LAT   47.36  /* latitude N of your current location.       */
#define DEFAULT_ZONE   8.00  /* Change to default time zone of current     */
                             /* location in hours before (west of) GMT.    */

#define DEFAULT_DIR "/home/ecp/ast/etc/"
                     /* Change this string to directory path program       */
                     /* should look in for chart files if not in current.  */

#define SWITCHES /* Comment out this #define if your system can not handle */
                 /* parameters on the command line (like Mac's, etc.)      */
#define TIME     /* Comment out this #define if your compiler can't take   */
                 /* the calls to 'time' or 'localtime' as in time.h        */
#define X11      /* Comment out this #define if you don't have X windows.  */
#define UNIX     /* Comment out this #define if you have a VMS system or   */
                 /* a system in which atof() isn't defined in stdio.h      */

/*
** Although not necessary, one may like to change some of the values below:
** These constants affect some of the default parameters, e.g. fonts, etc.
*/

#define DEFAULT_INFOFILE "astrolog.dat" /* Name of file to look in for     */
                                        /* default parameters (which may   */
                                        /* override the values here.)      */
#define ASTEROIDS            /* Comment out this #define if you don't want */
                             /* Chiron, the asteroids, the part of         */
                             /* fortune, and vertex to appear by default.  */
#define DEFAULT_SYSTEM 0     /* Normally, Placidus houses are used (unless */
                             /* user specifies otherwise). If you want a   */
                             /* different default system, change this numb */
                             /* to value from 0..9 (values same as in -c). */

#define WHEELCOLS 15    /* Affects the width of each house in wheel display */
#define WHEELROWS  4    /* Max no. of objects that can be in a wheel house. */
#define MAXINDAY 100    /* Max number of aspects or transits displayable.   */
#define MAXCROSS 500    /* Max number of latitude crossings displayable.    */
#define DIVISIONS 24    /* Greater numbers == more accuracy but slower      */
                        /* calculation, of exact aspect and transit times.  */

#ifdef X11
#define BITMAPMODE 'C'  /* For X windows, this char affects how bitmaps are */
                        /* written. 'B' is written like with the 'bitmap'   */
                        /* program, 'C' is compacted somewhat (files have   */
                        /* less spaces), and 'S' is compacted even more.    */
#define FONT "6x10"     /* Font to use when writing in X window.            */
#define FONT_X   6      /* Font_x and Font_y should be set to the pixel     */
#define FONT_Y  10      /* width and height of the (fixed) font chosen.     */
#define BITMAPX 1152    /* Maximum window size allowed */
#define BITMAPY 1152
#define BITMAPX1 180    /* Minimum window size allowed */
#define BITMAPY1 180
#define DEFAULTX 600    /* Default window size         */
#define DEFAULTY 600
#define CELLSIZE  14    /* Size for each cell in the aspect grid      */
#define DEFORB   7.0    /* Min distance glyphs can be from each other */
#endif


/*
** One shouldn't need to change anything below this line to compile.
*/

#include <stdio.h>
#ifndef UNIX
#include <stdlib.h>
#endif
#include <math.h>
#ifdef TIME
#include <time.h>
#endif
#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif


/*
*******************************************************************************
** Standard constants
*******************************************************************************
*/

#define VERSION "2.25"
#define DATE "June 1992"
#define FALSE 0
#define TRUE 1
#define OFF 0
#define ON 1
#define STRING 80
#define PI 3.14159265358979323846
#define DEGREES 360.0
#define DEGTORAD (180.0/PI)
#define SIGNS    12
#define PLANETS  14
#define THINGS   16
#define OBJECTS  20
#define URANIANS 8
#define STARS    46
#define BASE     (OBJECTS+4+URANIANS)
#define TOTAL    (BASE+STARS)
#define ASPECTS  18
#define SYSTEMS  10


/*
*******************************************************************************
** Macro functions
*******************************************************************************
*/

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define CAP(B) ((B) >= 'a' && (B) <= 'z' ? (B) - 'a' + 'A' : (B))
#define DTOR(A) ((A)/DEGTORAD)
#define RTOD(A) ((A)*DEGTORAD)
#define ASIN(A) atan((A)/sqrt(1.0-(A)*(A)))
#define ACOS(A) atan(sqrt(1.0-(A)*(A))/(A))
#define SIND(A) sin((A)/DEGTORAD)
#define COSD(A) cos((A)/DEGTORAD)

#define SIGNAM(A) signname[A][0], signname[A][1], signname[A][2]
#define OBJNAM(A) objectname[A][0], objectname[A][1], objectname[A][2]
#define int long
#define ecltoequ(Z, L) coorxform(Z, L, DTOR(23.44578889))
#define equtoecl(Z, L) coorxform(Z, L, DTOR(-23.44578889))
#define equtolocal(Z, L, T) coorxform(Z, L, T)

#ifdef X11
#define color unsigned long
#define bit   color
#define Xcolor(A) XSetForeground(disp, gc, A);
#endif


/*
*******************************************************************************
** External declarations
*******************************************************************************
*/

/* From data.c */

extern FILE *data;
extern char *filename, *filename2;

extern int total, objects, aspects, stars, universe,
  siderial, right, centerplanet, todisplay, housesystem, cusp, uranian,
  multiplyfactor, onasc, flip, decan, geodetic, graphstep,
  progress, autom, relation, operation;
extern double progday, defzone, deflong, deflat, modulus;
extern double Mon, Day, Yea, Tim, Zon, Lon, Lat,
  M, D, Y, F, X, L5, LA,
  SD, Mon2, Yea2, Delta, T, JD, Jdp,
  MC, Asc, Vtx;

extern double planet[], planetalt[], house[], ret[],
  spacex[], spacey[], spacez[],
  planet1[], planet2[], planetalt1[], planetalt2[],
  house1[], house2[], ret1[], ret2[];
extern int inhouse[], ignore[], starname[], errorcount[];

extern char *signname[], signabbrev[][3], objectname[][12],
  uranianname[][10], *systemname[],
  *aspectname[], aspectabbrev[][4], *aspectglyph[], 
  *element[], *mode[], post[][3], *monthname[], *starnames[];
extern double aspectangle[], aspectorb[], starbright[];
extern int ruler1[], ruler2[], exalt[],
  grid[TOTAL+1][TOTAL+1], gridname[TOTAL+1][TOTAL+1],
  inhouse1[TOTAL+1], inhouse2[TOTAL+1];
extern char *signdraw[], *objectdraw[], *housedraw[], *aspectdraw[];

extern void swapint(), swapdoub(), toofew(),
  printminute(), printaltitude(), inputdata();
extern int stringlen(), mod12(), dayinmonth(),
  readworlddata();
extern double dabs(), sgn(), dectodeg(), mod(), mindistance(), midpoint(),
  orb(), readplanetdata(), readstardata();
extern char dignify(), *stringlocation();

/* From formulas.c */

extern double mdytojulian(), processinput(), castchart();
extern void juliantomdy(), coorxform(), houseplace(), caststar();

/* From charts.c */

extern double objectinf[], houseinf[], aspectinf[];

extern void chartlocation(), chartgrid(), displaygrands(), chartwheel(),
  charthorizon(), chartspace(), chartinfluence(), chartastrograph();

/* From options.c */

extern void creategrid(), castrelation(), printtransit(), printephemeris(),
  printchart();

#ifdef X11
/* From graphics.c */

extern void xchart();

/* From graphics.c */

extern color maincolor[], rainbowcolor[],
  elemcolor[], aspectcolor[], objectcolor[], hilite, gray, on, off;
extern Display *disp;
extern Pixmap pixmap;
extern GC gc;
extern Window window;
extern char modex;
extern int xcolor, xreverse, xbitmap, xtext, bonus, label,
  turtlex, turtley, scale, chartx, charty, degree, xnow;
extern double tilt;
extern void drawpoint(), block(), rect(), drawline(), drawclip(), turtle();

extern void xdisplayswitches(), xaction();
extern int xprocess();
#endif

/**/

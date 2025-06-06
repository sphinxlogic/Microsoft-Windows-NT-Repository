/*

ęCopyright 1984, 1988, 1990, 1993-1995, 2000 by Rick Jansen, all rights reserved.

	X S N O W 
				*
	*   *
		*       *
				*
	 *

 In 1984 the first Macintosh program I wrote was a computer christmas
 card, which showed a picture of a snowman and falling snow. Later
 a Father Xmas in his sleigh was added. I converted this program to
 an undying desk accessory in 1988 with several additions.
 But, little boys grow up, and when they are forced to a workstation
 they want their thingies there too. So here is Xsnow. For *you*.

 Happy winter and a merry X-mas!

 Rick Jansen
 (rja@euronet.nl)
 http://www.euronet.nl/~rja/
 Xsnow WWW page:  http://www.euronet.nl/~rja/Xsnow/ 


 Latest news: (December 16th 2000)
 ------------
 After 5 years here's a new Xsnow! The program has been around bit,
 and is now even making it snow on many windows machines, and we're
 not talking X or Macintosh here (sorry!). Anyway, it's high time for
 an improved version for X. Xsnow 1.41 works with modern X servers
 like XFree86, where older versions made snow build up somewhere in
 the sky at times, defying gravity.
	
 Xsnow 1.41 now features the (big) Santa that is long featured in Snow 
 for Macintosh and in Snow for Windows, many thanks to Thomas Linder!
 (Thomas.Linder@gmx.net).
	 
 It is now possible to slow Santa down. See the option -santaupdatefactor   

 Availability
 ------------
 Xsnow is available directly from me via my WWW page:

	 http://www.euronet.nl/~rja/

 Click on the appropriate item and the tar file will be downloaded
 upon your request.

 Xsnow is also archived at ftp.x.org in the contrib/games directory.
 Ftp to ftp.x.org and enter 'anonymous' as the login name, your
 e-mail address as the password. Then 'cd contrib/games' and a 
 'get xsnow.README' for further information.

 Copyright
 ---------
 Xsnow is available freely and you may give it to other people as is, 
 but I retain all rights. Therefore it does not classify as 'Public 
 Domain' software. It *is* Alwd to package Xsnow for Unix/Linux
 distributions, CD-Roms etc, and to make the necessary changes to
 makefiles etc. to facilitate this. 

 Credits
 -------
 Xsnow borrows code for drawing on the desktop (the 'root' window as
 they insist on calling it in X) from the infamous xroach program, 
 which is copyright 1991 by J.T. Anderson (jta@locus.com).
 Xroach is available in the 'contrib' directory of 'ftp.x.org' via 
 anonymous ftp.

 For compatibility with virtual window managers Xsnow uses vroot.h
 vroot.h is copyright 1991 by Andreas Stolcke, copyright 1990 by 
 Solbourne Computer Inc. (stolcke@ICSI.Berkeley.EDU)
 The complete distribution of vroot is available in the 'contrib'
 directory of 'ftp.x.org' via anonymous ftp as 'vroot.shar.Z' 

 The code for wind was done by Eiichi TAZOE (tazoe@yamato.ibm.co.jp,
 tazoe@vnet.ibm.com)
		
 Installation
 ------------
	To build:
			xmkmf
			make depend
			make

	To run:
			./xsnow
		or:
			./xsnow&  (in the background)

	To install (be root):
			make install
			make install.man
	or:
			cc -o xsnow snow.c -lX11 [-lsocketorwhatever] [-lm] [-l...] -lXpm
 
	SUN Solaris users without a C-compiler, but with gcc installed,
	try the following two commands. The first compiles Xsnow, the second
	links Xsnow. 

		 gcc -c -O2 -I/usr/openwin/include -DSVR4 -DSYSV xsnow.c
		 gcc -o xsnow xsnow.o -O2 -L/usr/openwin/lib -lXext -lX11 -lXpm -lm -lsocket -lnsl

	Note: Xsnow-1.41 needs the Xpm library. It is available from:
	ftp://ftp.x.org/contrib/libraries/
	The current Xpm version is xpm3.4k  

 Silicon Graphics
 ----------------
 Silicon Graphics and Irix 5.x users may not see any snow or
 Santa at all, as long the desktop icons are visible. To
 circumvent this problem issue this command:

		/usr/lib/desktop/telldesktop quit

 The icons will disappear and xsnow will work perfectly. To
 restart the desktop just start /usr/lib/desktop/startdesktop
 or select Desktop->Home Directory from the toolchest.

 It's even possible to have both - desktop icons and xsnow
 (and even multiple desks). You need to modify the window
 manager's resource file 4DWm, the file ~/.desktop-
 `hostname`/4DWm. Example:

 *Global.backgroundDescription:   -execute /etc/killall -TERM
 xsnow ; /usr/local/bin/xsnow
 *Desk 1.backgroundDescription:   -execute /etc/killall -TERM
 xsnow ; /usr/local/bin/xsnow
 *Desk 2.backgroundDescription:   -execute /etc/killall -TERM
 xsnow ; /usr/local/bin/xsnow

 Restart the he toolchest and Xsnow should appear. What this
 does is stop the currently running Xsnow and start a new one
 when you switch to another desktop.

 HP
 --
 HP also uses a Workspace Manager which may interfere with
 Xsnow.  If Xsnow does not appear:  In the "Style Manager",
 choose "Backdrop" and select "NoBackdrop".  You should now
 be able to run Xsnow.


 Modification history
 --------------------
 1984/1985 Macintosh animated Christmas card
 1988/1990 Macintosh undying desk accessory, and MultiFinder version
 03DEC93 version 0 for X-windows
 04DEC93 background setting
 06DEC93 delay in main loop using select
 10DEC93 keep snow on top of windows and at bottom of display
 11DEC93 snow kept sticking to the right of display (1.11)
 11DEC93 updated calcwindowtops
 12DEC93 call calcwindowtops after last exposure event only
 13DEC93 vroot added for virtual window managers
 14DEC93 separate options for snowdepth on windows and on screen
 15DEC93 patch for fvwm from Mike Hollick (hollick@graphics.cis.upenn.edu)
 16DEC93 Really building up snow at bottom of screen
 16DEC93 A choice of Santa's. Thanks to Holger Veit (Holger.Veit@gmd.de)
				 for Santa no. 2. Santa 1 was derived from 2 by scaling down.
 20DEC93 Aaaargh! gnu C doesn't like the initialization of the new
				 Santa pixmaps. Extra braces do the trick, which is so often the case
 20DEC93 Deleted PaintSnowAtBottom. It's more of a surprise this way.
 21DEC93 In case of negative x coordinates adapt width (CalcWindowTops)
 22DEC93 Rudolf's red nose. Silly. Grmbll.
 22DEC93 Bug with -nokeepsnowonwindows removed
 10JAN94 Improved handling of erasing snow at screen bottom in case of 
				 window in or under the snow.
 18FEB94 X-speed really 0 inside snow layers - prevents snowflakes from 
				 erasing already fallen snow a bit better
 30OCT94 More efficiency due to GC's in server and not drawing where
				 the windows are.
 30OCT94 1.30 Eiichi TAZOE's code for wind included in general distribution.
 11NOV94 Bug in XRectInRegion call resolved (santa not appearing gradually
				 from behind windows)
 15NOV94 Unsigned longs changed to longs, as strtoul is troublesome on many
				 systems (snowDelay)
 08DEC94 Snow is really building up on windows now
 10DEC94 1.36 Handle Popups (no snow on them)
 16DEC94 1.37 -offset and (-nokeepsnowonwindows and -nokeepsnowonscreen back)
 20DEC94 1.38 Let Santa be a real snowplough now
 21DEC94 1.39 VMS fix for uSsleep, thanks to Ulrich Kabatek 
							<Kaba@isr.uni-stuttgart.d400.de>
 19NOV95 1.40 Memory leak fixed in CalcWindowTops, thanks to Satoshi Asami
							<asami@cs.berkeley.edu> 
 19NOV95 1.40 Greatly increased performance when using solid backgrounds
							by using XFillRectangle instead of XClearArea. Thanks to
							Benedikt Kessler <bjk@benedikt.munich.sgi.com>
 20NOV95 1.40 Bug fixed with erasing snow at bot of screen
 1996    New macintosh version, in color http://www.euronet.nl/~rja/MacSnow/
 1998    Snow for Windows http://www.euronet.nl/~rja/WinSnow/
 18DEC2000	1.41 Fix for X-servers that do not send expose events after a
						while, due to backing store.
*/

#define debug 0


/*
	And in a vocoded voice it sounds:
		Xsnow zwei-tausend
		Xsnow two-thousand 
		Xsnow deux mille
		Xsnow dos mil
	etc.
*/
#define VERSION "Xsnow-1.41, December 16th 2000 by Rick Jansen (rja@euronet.nl)\n\
WWW: http://www.euronet.nl/~rja/Xsnow/\n"


#ifdef VMS
#include <socket.h>
# if defined(__SOCKET_TYPEDEFS) && !defined(CADDR_T)
#  define CADDR_T
# endif
#endif


#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#ifndef VMS
#include <X11/xpm.h>
#else
#include "xpm.h"
#endif

/**** V R O O T ****/
/* For vroot.h see the credits at the beginning of Xsnow */
/***#include <X11/vroot.h>   /* if vroot.h installed in /usr/include/X11 ***/
#include "vroot.h"  /* Use this if vroot.h installed in current directory */

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <limits.h>
#include <string.h>


#if defined(__STDC__) || defined(VMS)
#include <stdlib.h>
#else
long strtol();
/*double strtod();*/
char *getenv();
#endif

char Copyright[] = "\nXsnow\nCopyright 1984,1988,1990,1993-1995,2000 by Rick Jansen, all rights reserved.\n";

#include "xsnow.h"

typedef int ErrorHandler();

#if !defined(GRAB_SERVER)
#define GRAB_SERVER	0
#endif

Display *display;
int screen;
Window rootWin;
int display_width, display_height;
int center_x, center_y;
GC gc;
char *display_name = NULL;
Pixel black, white;

int done = 0;
int errorVal = 0;

int flake;
int SmoothWhirl = 1;
int NoTrees = 0;
int NoSanta = 0;
int Rudolf = 1;
int NoKeepSnow = 0;
int NoKeepSBot = 0;
int NoKeepSWin = 0;
int NoPopuphandling = 1;
int NoWind = 0;
unsigned int borderWidth = 0;
int SnowOffset = 0;
int UseFillForClear = 0;

Snow *snowflakes;
int MaxSnowFlakeHeight = 0;  /* Biggest flake */
int maxSnowflakes = INITIALSNOWFLAKES;
int curSnowflakes = 0;
long snowDelay = 50000; /* useconds */
int MaxXStep = MAXXSTEP;
int MaxYStep = MAXYSTEP;
int WhirlFactor = WHIRLFACTOR;
int WhirlAdapt;
int MaxWinSnowDepth = INITWINSNOWDEPTH;
int MaxScrSnowDepth = INITSCRSNOWDEPTH;


/* The default Santa 0,1,2 */
int SantaSize = 2;
int SantaSpeed = -1;  /* Not initialized yet */
#define	MAXSANTA_SPEED 30

int SantaX;
int SantaY;
int SantaXStep;
int SantaYStep;
int SantaVisible;
int CurrentSanta;
Pixmap SantaPixmap[PIXINANIMATION];
Pixmap SantaMaskPixmap[PIXINANIMATION];
int SantaWidth;
int SantaHeight;   


/* For building up snow at bottom of screen and windows */
Region WindowsR = NULL;
Region PWindowsR = NULL;
Region WindowsDifferenceR = NULL;
Region CtchDifferenceR = NULL;   
Region RecalcSnowCtchR = NULL;

Region SnowCtchR = NULL;
Region SnowAlwdR = NULL;
Region SubR = NULL;
int StillDoDiffToDoFlag = 0;

/* Wind stuff */
int diff=0;
int wind = 0;
int direction=0;
int WindTimer=30;
int current_snow_height;
int geta=0;
XRectangle AddRect;

unsigned int RunCounter = 0;
int SantaUpdateFactor = 3;

/* Forward decls */
void Usage();
void SigHandler();
void SigHupHandler();
void InitSnowflake();
void UpdateSnowflake();
void DrawSnowflake();
void EraseSnowflake();
void PaintSnowAtBottom();
void DrawTannenbaum();
void InitSantaPixymaps();
void ResetSanta();
void DrawSanta();
void EraseSanta();
void UpdateSanta();
void uSsleep();
int MakeWR(int);
void MkASR();
void ReclcCSR();
int DoDffWLT();     
void HandleEvA(int X,int Y,int W, int H);
Pixel AllocNamedColor();
void sig_alarm();



/* Colo(u)rs */
char *snowColor = "snow";
char *slcColor = "black";
char *blackColor = "black";
char *redColor = "red";
char *whiteColor = "white";
char *bgColor = "none";
char *trColor = "chartreuse";
/* The author thoroughly recommends a cup of tea */
/* with a dash of green Chartreuse. Jum!         */
char *greenColor = "chartreuse"; 
Pixel redPix;
Pixel whitePix;
Pixel greenPix;
Pixel blackPix;
Pixel snowcPix;
Pixel bgPix;
Pixel trPix;
Pixel slcPix;

/* GC's */
GC SnowGC[SNOWFLAKEMAXTYPE+1];
GC ClearSnowGC;
GC SleighGCs[3];
GC SantaGC,RudolfGC,FurGC;
GC TreesGC;

int rc;


/* ------------------------------------------------------------------ */ 
int main(ac, av)
int ac;
char *av[];
{
		XGCValues xgcv;
		int ax;
		char *arg;
		SnowMap *rp;
		TannenbaumMap *tp;
		XEvent ev;
		int Exposed;
		int ConfigureNotified;
		int i; 
		int x,y;

		Window root; /* used in getgeometry call */
		int winX, winY;
		unsigned int winHeight, winWidth;
		unsigned int depth;
		
		/*
			 Process command line options.
		*/
		for (ax=1; ax<ac; ax++) {
			arg = av[ax];
			if (strcmp(arg, "-display") == 0) {
				display_name = av[++ax];
			}
			else if (strcmp(arg, "-sc") == 0) {
				snowColor = av[++ax];
			}
			else if (strcmp(arg, "-bg") == 0) {
				bgColor = av[++ax];
 
				/* If solid colour used as bg we can use XFillArea for clear */
				/* This results in a big performance improvement */
				/*** Uncomment this if sure all Xservers are faster with
					 XFillRectangle than with XClearArea ***/
				/** December 2000: well, this is NOT true for all X-servers!
						It does work this way on SGI, but on SUN Solaris 2.5.1 
						UseFillForClear costs 20% *more* resources! **/
				/*** UseFillForClear = 1; ***/
			}
			else if (strcmp(arg, "-solidbg") == 0) {
				/*	
						If solid colour used as bg we can use XFillArea for clear
						This results in a big performance improvement, but not on all
						X-servers! (Yes on SGI, NO on SUN (Solaris 2.5.1) (Dec 2000)
				*/
				UseFillForClear = 1;
			}
			else if (strcmp(arg, "-tc") == 0) {
				trColor = av[++ax];
			}
			else if (strcmp(arg, "-slc") == 0) {
				/* Obsolete option, Santa is a colour pic now */
				/*slcColor = av[++ax];*/
				fprintf(stderr,"** The option -slc (sleigh color) is obsolete\n");
			}
			else if (strcmp(arg, "-delay") == 0) {
				snowDelay = strtol(av[++ax], (char **)NULL,0);
				snowDelay = snowDelay * 1000;  /* ms to us */
			}
			else if (strcmp(arg, "-santaupdatefactor") == 0) {
				SantaUpdateFactor = strtol(av[++ax], (char **)NULL,0);
			}
			else if (strcmp(arg, "-snowflakes") == 0) {
				maxSnowflakes = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-unsmooth") == 0) {
				SmoothWhirl = 0;
			}
			else if (strcmp(arg, "-whirl") == 0) {
				WhirlFactor = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-nowind") == 0) {
				NoWind = 1;
			}
			else if (strcmp(arg, "-windtimer") == 0) {
				WindTimer = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-xspeed") == 0) {
				MaxXStep = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-yspeed") == 0) {
				MaxYStep = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-wsnowdepth") == 0) {
				MaxWinSnowDepth = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-ssnowdepth") == 0) {
				MaxScrSnowDepth = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-offset") == 0) {
				SnowOffset = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-notrees") == 0) {
				NoTrees = 1;     
			}
			else if (strcmp(arg, "-nosanta") == 0) {
				NoSanta = 1;     
			}
			else if (strcmp(arg, "-santa") == 0) {
				SantaSize = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-norudolf") == 0) {
				Rudolf = 0;
			}
			else if (strcmp(arg, "-santaspeed") == 0) {
				SantaSpeed = strtol(av[++ax], (char **)NULL, 0);
			}
			else if (strcmp(arg, "-nokeepsnow") == 0) {
				NoKeepSnow = 1;
				NoKeepSWin = 1;
				NoKeepSBot = 1;
			}
			else if (strcmp(arg, "-nokeepsnowonwindows") == 0) {
				NoKeepSWin = 1;
			}
			else if (strcmp(arg, "-nokeepsnowonscreen") == 0) {
				NoKeepSBot = 1;
			}

			/* And now for some confusion */
			else if (strcmp(arg, "-popup") == 0) {
				NoPopuphandling = 1;
			}
			else if (strcmp(arg, "-nopopup") == 0) {
				NoPopuphandling = 0;
			}
			else if (strcmp(arg, "-nonopopup") == 0) {
				NoPopuphandling = 1;
			}
			else if (strcmp(arg, "-nononopopup") == 0) {
				NoPopuphandling = 0;
			}

			else if (strcmp(arg, "-version") == 0) {
				printf("\nThis is %s\n\n", VERSION);
				exit(0);
			}
			else {
				Usage();
			}
		}

		printf("%s\n",VERSION);

		/* Some people... */
		if (maxSnowflakes > MAXSNOWFLAKES) {
			fprintf(stderr,"** Maximum number of snowflakes is %d\n", MAXSNOWFLAKES);
			exit(1);
		}
		if ((SantaSize < 0) || (SantaSize > MAXSANTA)) {
			fprintf(stderr,"** Maximum Santa is %d\n",MAXSANTA);
			exit(1);
		}
		if (SantaUpdateFactor == 0) {
			fprintf(stderr,"** It is naughty (sic!) to specify zero for -santaupdatefactor\n");
			exit(1);
		}

		/* Eskimo warning */
		if (strstr(snowColor,"yellow") != NULL)
			printf("\nWarning: don't eat yellow snow!\n\n");


		/* Santa speed is different for different size Santa */
		if (SantaSpeed < 0) SantaSpeed = Speed[SantaSize];

		/* Seed random */
#ifdef VMS
		srand((int)time((unsigned long *)NULL));
#else
		srand((int)time((long *)NULL));
#endif


		/*
			 Ctch some signals so we can erase any visible snowflakes.
		*/
		signal(SIGKILL, SigHandler);
		signal(SIGINT, SigHandler);
		signal(SIGTERM, SigHandler);
#if debug
		signal(SIGHUP, SigHupHandler);
#else
		signal(SIGHUP, SigHandler);
#endif

		/* Wind stuff */
		signal(SIGALRM, sig_alarm);
		alarm(WindTimer);


		/* Open X */
		display = XOpenDisplay(display_name);
		if (display == NULL) {
			if (display_name == NULL) display_name = getenv("DISPLAY");
			(void) fprintf(stderr, "%s: cannot connect to X server %s\n", av[0],
				display_name ? display_name : "(default)");
				exit(1);
		}

		screen = DefaultScreen(display);
		rootWin = RootWindow(display, screen);
		black = BlackPixel(display, screen);
		white = WhitePixel(display, screen);

		display_width = DisplayWidth(display, screen);
		display_height = DisplayHeight(display, screen);
		center_x = display_width / 2;
		center_y = display_height / 2;

		/* No snow at all yet */
		current_snow_height = display_height;

		/* Do not let all of the display snow under */
		if (MaxScrSnowDepth> (display_height-SNOWFREE)) {
			printf("** Maximum snow depth set to %d\n", display_height-SNOWFREE);
			MaxScrSnowDepth = display_height-SNOWFREE;
		}
		

		/*
				P I X M A P S 
		*/

		/* Create the snowflake pixmaps */
		for (flake=0; flake<=SNOWFLAKEMAXTYPE; flake++) {
			rp = &snowPix[flake];
			rp->pixmap = XCreateBitmapFromData(display, rootWin,
						rp->snowBits, rp->width, rp->height);
			/* Whats the biggest flake? (used for erasing kept snow later) */
			if (rp->height > MaxSnowFlakeHeight) MaxSnowFlakeHeight = rp->height;
		}

		/* Allocate structures containing the coordinates and things */
		snowflakes = (Snow *)malloc(sizeof(Snow) * MAXSNOWFLAKES);

		/* Create the tree pixmap */
		tp = &tannenbaumPix[0];
		tp->pixmap = XCreateBitmapFromData(display, rootWin,
						tp->tannenbaumBits, tp->width, tp->height);


		/* Create the Santa pixmaps, dependent on the Santa and Rudolf wanted */
		InitSantaPixymaps();

		/* Allocate colors just once */
		redPix =   AllocNamedColor(redColor, black);
		whitePix = AllocNamedColor(whiteColor, white);
		greenPix = AllocNamedColor(greenColor, black);
		blackPix = AllocNamedColor(blackColor, black);
		snowcPix = AllocNamedColor(snowColor, white);   
		trPix = AllocNamedColor(trColor, black);
		/*slcPix = AllocNamedColor(slcColor, black);*/


		gc = XCreateGC(display, rootWin, 0, NULL);
		XGetGCValues(display, gc, 0, &xgcv);
		XSetForeground(display, gc, blackPix);
		XSetFillStyle(display, gc, FillStippled);

		/* Set the background color, if specified */
		if(strcmp(bgColor,"none") != 0) {
			bgPix = AllocNamedColor(bgColor, white);

			XSetWindowBackground(display, rootWin, bgPix);
			XClearWindow(display, rootWin);
			XFlush(display);

			/* If -solidbg not specified notify that this may improve things! */
			if (!UseFillForClear) {
				printf("\nNote: when using backgrounds of one color also specifying\n");
				printf("      -solidbg MAY greatly improve performance!\n");
			}
			else {
				ClearSnowGC = XCreateGC(display, rootWin, 0L, &xgcv);
				XCopyGC(display,gc,0,ClearSnowGC);
				XSetForeground(display,ClearSnowGC, bgPix);
			}
		}
		else if (UseFillForClear) {
			printf("\n");
			printf(
			 "Sorry, -solidbg can only be specified with a background color (-bg)");
			printf("\n");
			exit(1);
		}
		
		/* Fill in GCs for snowflakes */
		for (flake=0; flake<=SNOWFLAKEMAXTYPE; flake++) {
			SnowGC[flake] = XCreateGC(display, rootWin, 0L, &xgcv);
			XCopyGC(display,gc,0,SnowGC[flake]);
			XSetStipple(display, SnowGC[flake], snowPix[flake].pixmap);
			XSetForeground(display,SnowGC[flake],snowcPix);
			XSetFillStyle(display, SnowGC[flake], FillStippled);
		}
		
		/* GC for trees */
		TreesGC = XCreateGC(display, rootWin, 0L, &xgcv);
		XCopyGC(display,gc,0,TreesGC);
		XSetStipple(display, TreesGC, tannenbaumPix[0].pixmap);
		XSetForeground(display,TreesGC,trPix);
		XSetFillStyle(display, TreesGC, FillStippled);

		/*
		 *
		 * Here snowing starts
		 *
		 */

		WindowsR = XCreateRegion();
		PWindowsR = XCreateRegion();
		SnowCtchR = XCreateRegion();
		SnowAlwdR = XCreateRegion();
		WindowsDifferenceR = XCreateRegion();
		CtchDifferenceR = XCreateRegion();                                                                                 
		RecalcSnowCtchR = XCreateRegion();

		/* The initial Ctch region: a little bit below the bottom of the screen */
		AddRect.x = 0;
		AddRect.y = display_height;
		AddRect.width = display_width - 1;
		AddRect.height = MaxYStep+MaxSnowFlakeHeight;
		if (!NoKeepSBot)
			XUnionRectWithRegion(&AddRect, SnowCtchR, SnowCtchR);

		/* Initialize all snowflakes */
		for (i=0; i<maxSnowflakes; i++) InitSnowflake(i);

    rc = MakeWR(1);
		XUnionRegion(WindowsR,PWindowsR, PWindowsR);                                                                 
		ReclcCSR();
		MkASR();

		ResetSanta();   

		/* Notify if part of the root window changed */
		XSelectInput(display, rootWin, ExposureMask | SubstructureNotifyMask);


		/*
		 *  M A I N   L O O P
		 */
		while (!done) {

			/* X event ? */
			Exposed = 0;
			ConfigureNotified = 0;
			while (XPending(display) || StillDoDiffToDoFlag) {

				XNextEvent(display, &ev);

				/* If we still had a DoDffWLT to do, do it now. */
				if (StillDoDiffToDoFlag) {
					StillDoDiffToDoFlag = !DoDffWLT();
					/*continue;*/
				}


				/* No need to do all this computing if we're not keeping the snow */
				if (!NoKeepSnow) {

					switch (ev.type) {

							case Expose:    
								
								  Exposed = 1;
									if (!DoDffWLT()) {
										StillDoDiffToDoFlag = 1;
										continue; 
									}
									/*HandleEvA(ev.xexpose.x,ev.xexpose.y,ev.xexpose.height,ev.xexpose.width);*/

								  break;


							case ConfigureNotify:
									if (!DoDffWLT()) {
										StillDoDiffToDoFlag = 1;
										continue; 
									}
									/*HandleEvA(ev.xconfigure.x,ev.xconfigure.y,ev.xconfigure.height,ev.xconfigure.width);*/
								  break;
							case ConfigureRequest:
								  break;

							case CreateNotify:
									DoDffWLT(); 
									if (!DoDffWLT()) {
										StillDoDiffToDoFlag = 1;
										continue; 
									}
								  break;
							case DestroyNotify:

									if (!DoDffWLT()) {
										StillDoDiffToDoFlag = 1;
										continue; 
									}
								  break;

							case MapNotify:
									if (!DoDffWLT()) {
										StillDoDiffToDoFlag = 1;
										continue; 
									}

									XGetGeometry(	ev.xmap.display, ev.xmap.window, 
																&root, &winX, &winY, &winWidth, &winHeight, &borderWidth, &depth);
								  break;
							case UnmapNotify:
									if (!DoDffWLT()) {
										StillDoDiffToDoFlag = 1;
										continue; 
									}

									/* Cannot fetch geometry of a window that has already been destroyed...
									XGetGeometry(	ev.xunmap.display, ev.xunmap.window, 
																&root, &winX, &winY, &winWidth, &winHeight, &borderWidth, &depth);
									printf("UnmapNotify r=%d w=%d geo=(%d,%d,%d,%d) bw=%d d=%d\n\n", 
														root, ev.xunmap.window, winX,winY,winWidth,winHeight, borderWidth,depth);
									*/
								  break;

					} /* switch */

				}  /* if (!NoKeepSnow) */
			 
			}  /* while Xpending */

			/* If things have changed while we were away... */


#if debug
/***
XClearWindow(display, rootWin);
XFlush(display);
XSetForeground(display,FurGC,trPix);
XSetFillStyle(display,FurGC, FillSolid);
XSetRegion(display,FurGC,SnowCtchR);
XFillRectangle(display,rootWin,FurGC,0,0,display_width,display_height);
XFlush(display);
***/
#endif


			/* Sleep a bit */
			uSsleep(snowDelay);

			/* 
			 *  Update     
			 */

			RunCounter++;

			/* Snowflakes */
			for (i=0; i<maxSnowflakes; i++) UpdateSnowflake(i);

			/* Draw dear Andrews */
			if (!NoTrees ) {
				DrawTannenbaum(display_width-150, display_height-500);
				DrawTannenbaum(display_width-100, display_height-200);
				DrawTannenbaum(100, display_height-200);
				DrawTannenbaum(50, display_height-150);
				DrawTannenbaum(center_x, display_height-100);
				DrawTannenbaum(200,400);
			}

			/* Dear Santa */
			if (!NoSanta) {
				if ((RunCounter % SantaUpdateFactor) == 0) UpdateSanta();
			}

		}

/***
sleep(10);
XClearWindow(display, rootWin);
XFlush(display);
XSetFillStyle(display,FurGC, FillSolid);
XSetRegion(display,FurGC,SnowAlwdR);
XFillRectangle(display,rootWin,FurGC,0,0,display_width,display_height);
XFlush(display);
sleep(4);
***/
		
		XDestroyRegion(WindowsR);
		XDestroyRegion(SnowCtchR);
		XDestroyRegion(SnowAlwdR);
		XDestroyRegion(PWindowsR);
		XDestroyRegion(WindowsDifferenceR);
		XDestroyRegion(RecalcSnowCtchR);

		XClearWindow(display, rootWin);

		XCloseDisplay(display);

		exit(0);

}		/* End of the snow */


/* ------------------------------------------------------------------ */ 







/* ------------------------------------------------------------------ */ 

#define USEPRT(msg) fprintf(stderr, msg)

void
Usage()
{
		USEPRT("Usage: xsnow [options]\n\n");
		USEPRT("Options:\n");
		USEPRT("       -display     <displayname>\n");
		USEPRT("       -sc          <snowcolor>\n");
		USEPRT("       -tc          <tree color>\n");
		USEPRT("       -bg          <background color>\n");
		USEPRT("       -solidbg     (Performance improvement!)\n");
		USEPRT("       -snowflakes  <numsnowflakes>\n");
		USEPRT("       -delay       <delay in milliseconds>\n");
		USEPRT("       -unsmooth\n");
		USEPRT("       -whirl       <whirlfactor>\n");
		USEPRT("       -nowind\n");
		USEPRT("       -windtimer   <Time between windy periods in seconds>\n");
		USEPRT("       -xspeed      <max xspeed snowflakes>\n");
		USEPRT("       -yspeed      <max yspeed snowflakes>\n");
		USEPRT("       -wsnowdepth  <max snow depth on windows>\n");
		USEPRT("       -offset      <shift snow down>\n");
		USEPRT("       -ssnowdepth  <max snow depth at bottom of display>\n");
		USEPRT("       -notrees\n");
		USEPRT("       -nosanta\n");
		USEPRT("       -norudolf\n");
		USEPRT("       -santa       <santa>\n");
		USEPRT("       -santaspeed  <santa_speed>\n");
		USEPRT("       -santaupdatefactor  <factor>\n");
		USEPRT("       -nokeepsnow\n");
		USEPRT("       -nokeepsnowonwindows\n");
		USEPRT("       -nokeepsnowonscreen\n");
		USEPRT("       -nonopopup\n");
		USEPRT("       -version\n\n");
		USEPRT("Recommended: xsnow -bg SkyBlue3\n");
		
		exit(1);
}
/* ------------------------------------------------------------------ */ 

void
SigHandler()
{
	done = 1;
}


void
SigHupHandler()
{
	void (*Sig_Hup_ptr)() = SigHupHandler;

	/*XClearWindow(display, rootWin);*/
	XFlush(display);
	XSetForeground(display,FurGC,trPix);
	XSetFillStyle(display,FurGC, FillSolid);
	XSetRegion(display,FurGC,SnowCtchR);
	XFillRectangle(display,rootWin,FurGC,0,0,display_width,display_height);
	XFlush(display);

	signal(SIGHUP, Sig_Hup_ptr);
}

/* ------------------------------------------------------------------ */ 

/*
	 Generate random integer between 0 and maxVal-1.
*/
int
RandInt(maxVal)
int maxVal;
{
	return rand() % maxVal;
}



/* ------------------------------------------------------------------ */ 

/*
 * sleep for a number of micro-seconds
 */
void uSsleep(usec) 
unsigned long usec;
{
#ifdef SVR3
		poll((struct poll *)0, (size_t)0, usec/1000);   /* ms resolution */
#else
#ifdef VMS
		void lib$wait();
		float t;
		t= usec/1000000.0;
		lib$wait(&t);
#else
		struct timeval t;
		t.tv_usec = usec%(unsigned long)1000000;
		t.tv_sec = usec/(unsigned long)1000000;
		select(0, (void *)0, (void *)0, (void *)0, &t);
#endif
#endif
}


/* ------------------------------------------------------------------ */ 

/*
	 Check for topleft point of snowflake in specified rectangle.
*/
int
SnowPtInRect(snx, sny, recx, recy, width, height)
int snx;
int sny;
int recx;
int recy;
int width;
int height;
{
		if (snx < recx) return 0;
		if (snx > (recx + width)) return 0;
		if (sny < recy) return 0;
		if (sny > (recy + height)) return 0;
		
		return 1;
}



/*
	 Give birth to a snowflake.
*/
void
InitSnowflake(rx)
int rx;
{
		Snow *r;

		r = &snowflakes[rx];

	if (curSnowflakes < maxSnowflakes) {
		r->whatFlake = RandInt(SNOWFLAKEMAXTYPE+1);

		/* Wind strikes! */
		if (wind) {
			if (direction == 1) 
				/* Create snow on the left of the display */
				r->intX = RandInt(display_width/3);
			else
				/* Create snow on the right of the display */
				r->intX = display_width - RandInt(display_width/3);
			r->intY =  RandInt(display_height);
		}
		else  {
			r->intX = RandInt(display_width - snowPix[r->whatFlake].width);
			r->intY =  RandInt(display_height/10);
		}

		r->yStep = RandInt(MaxYStep+1)+1;
		r->xStep = RandInt(r->yStep/4+1);
		if (RandInt(1000) > 500) r->xStep = -r->xStep;
		r->active = 1;
		r->visible = 1;
		r->insnow = 0;
	}
}



/*
	 Move a snowflake by erasing and redraw
*/
void
UpdateSnowflake(rx)
int rx;
{
Snow *snow;

int NewX;
int NewY;
int tmp_x;
int TouchDown;
int InVisible;
int NoErase;
		
	/* move an active snowflake */

	snow = &snowflakes[rx];
	NoErase = 0;  /* Always erase the flakes unless special case */


	/* Activate a new snowflake */
	if (!snow->active) {
		InitSnowflake(rx);
		/* Make sure newly created flakes aren't erased before actually drawn */
		/* Especially with wind, where flakes are created all over the screen */
		snow->insnow = 1; 
	}

	/* New x,y */
	if (wind) {
		tmp_x = abs(snow->xStep);
		if( wind == 1 ){
				tmp_x = tmp_x + (RandInt(WhirlFactor+1) - WhirlFactor/2);
		}else{         
				tmp_x = tmp_x + (RandInt(20));
		}
		snow->xStep = tmp_x * direction;

		if (snow->xStep > 50) snow->xStep = 50;
		if (snow->xStep < -50) snow->xStep = -50;

		if( wind == 1 ){
				NewY = NewY+3;
		}else{
				NewY = NewY+10;
		}
	}
	NewX = snow->intX + snow->xStep;
	NewY = snow->intY + snow->yStep;


	/* If flake disappears offscreen (below bottom) it becomes inactive */
	/* In case of wind also deactivate flakes blown offscreen sideways, */
	/* so they'll reappear immediately. Make sure snow at bottom isn't  */
	/* disrupted.                          */

	/* Snowflake at the end of it's life? */
	snow->active = (NewY < display_height);
	/* If it is windy we need the flakes that blow offscreen! */
	if (wind) snow->active = 
			(snow->active && (NewX > 0) && (NewX < display_width));
		

	/* If flake covered by windows don't bother drawing it */
	InVisible = XRectInRegion(WindowsR,
								            NewX,NewY,
								            snowPix[snow->whatFlake].width,
								            snowPix[snow->whatFlake].height);
	snow->visible = (InVisible != RectangleIn);


	/*
	 *  Snow touches snow
	 */
	if (snow->visible) {

		/* Do not use XRectInRegion here, as that makes snow add to the bottom */
		/* of layers on windows too */
		/* Use middle bottom of flake */
		TouchDown = XPointInRegion(SnowCtchR,
								               NewX+snowPix[snow->whatFlake].width/2,
								               NewY+snowPix[snow->whatFlake].height);

		/* If the flake already is completely inside the region */
		if (TouchDown && snow->visible && !NoKeepSnow) {
			/* Only add snow that's visible, else snow will grow */
			/* on the *back* of windows */

			NoErase = 1;

			/* Add a bit to the snow bottom region to make snow build up */
			/* Snow isn't built up more than about a certain amount: inside */
			/* SnowAlwdR */

			/* If flake already inside snow layer don't add it again */
			TouchDown = XRectInRegion(SnowCtchR,
								                NewX,NewY,
								                snowPix[snow->whatFlake].width,
								                snowPix[snow->whatFlake].height);
			if (TouchDown == RectanglePart) {
				AddRect.x = NewX;
				AddRect.y = NewY + snowPix[snow->whatFlake].height - 2;
				AddRect.height = 2;
				AddRect.width = snowPix[snow->whatFlake].width;               
				if (XRectInRegion(SnowAlwdR,
								          AddRect.x, AddRect.y,
								          AddRect.width, AddRect.height) == RectangleIn) {
					XUnionRectWithRegion(&AddRect, SnowCtchR,SnowCtchR);
				}
			}
		}


		/* Adjust horizontal speed to mimic whirling */
		if (SmoothWhirl) {
			WhirlAdapt = RandInt(WhirlFactor); 
			if (RandInt(1000) > 500) WhirlAdapt = -WhirlAdapt;

			snow->xStep = snow->xStep + WhirlAdapt;
		}
		else {
			/* Jerkier movement */
			snow->xStep = snow->xStep + RandInt(WhirlFactor);
			if (RandInt(1000) > 500) snow->xStep = -snow->xStep;
		}

	}  /* if snow->visible */



	if (!wind) {
		if (snow->xStep > MaxXStep) snow->xStep = MaxXStep;
		if (snow->xStep < -MaxXStep) snow->xStep = -MaxXStep;
	}

	/* Limit X speed inside snowlayers. 's only natural... */
	if (NoErase && !wind) snow->xStep = snow->xStep/2;
	/* xsnow 1.29 method: */
	/***if (NoErase) {
		snow->xStep = 0;
		NewX = snow->intX;
	}***/


	/* Erase, unless something special has happened */
	if (!snow->insnow) EraseSnowflake(rx);

	/* New coords, unless it's windy */
	snow->intX = NewX;
	snow->intY = NewY;

	/* If snowflake inside a snow layer now don't erase it next time */
	snow->insnow = NoErase;

	/* Draw if still active  and visible*/
	if (snow->active && snow->visible) DrawSnowflake(rx);
 
}


		
/*
	 Draw a snowflake.
*/
void
DrawSnowflake(rx)
int rx;
{
		Snow *snow;

		snow = &snowflakes[rx];

		XSetTSOrigin(display, SnowGC[snow->whatFlake], snow->intX, snow->intY);
		XFillRectangle(display, rootWin, SnowGC[snow->whatFlake],
				 snow->intX, snow->intY,
				 snowPix[snow->whatFlake].width, snowPix[snow->whatFlake].height);
}


/*
	 Erase a snowflake.
*/
void
EraseSnowflake(rx)
int rx;
{
		Snow *snow;

		snow = &snowflakes[rx];
	 
		if (UseFillForClear) {
			XFillRectangle(display, rootWin, ClearSnowGC,
								 snow->intX, snow->intY,
								 snowPix[snow->whatFlake].width, 
								 snowPix[snow->whatFlake].height);
			}
		else {
			XClearArea(display, rootWin, 
								 snow->intX, snow->intY,
								 snowPix[snow->whatFlake].width, 
								 snowPix[snow->whatFlake].height,
								 False);
		}
}


/*
	Paint thick snow at bottom of screen for impaintient people
*/
void
PaintSnowAtBottom(depth)
int depth;
{
int x,y;
Snow *snow;

	for (y=0; y < depth; y++) {
		for (x=0; x<maxSnowflakes; x++) {
			InitSnowflake(x);
			snow = &snowflakes[x];
			snow->intY = display_height - y;
			DrawSnowflake(x);
		}
	}

}


/* ------------------------------------------------------------------ */ 

/*
	 Draw a tree
*/
void
DrawTannenbaum(x,y)
int x,y;
{
		XSetTSOrigin(display, TreesGC, x,y);
		XFillRectangle(display, rootWin, TreesGC,
				 x,y,
				 tannenbaumPix[0].width, tannenbaumPix[0].height);
}




/* ------------------------------------------------------------------ */ 



/*
	Initialize the pixmaps for Santa, depending on SantaSize and Rudolf
*/
void InitSantaPixymaps()
{
	int rc;
	char t[255];

	switch (SantaSize) {
		case 0:
			if (!Rudolf) {
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSanta1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSanta2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSanta3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSanta4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*RegularSanta1,"%d %d", &SantaWidth,&SantaHeight);
			}
			else {
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSantaRudolf1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSantaRudolf2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSantaRudolf3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, RegularSantaRudolf4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*RegularSantaRudolf1,"%d %d", &SantaWidth,&SantaHeight);
			}
		break;

		case 1:
			if (!Rudolf) {
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSanta1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSanta2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSanta3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSanta4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*MediumSanta1,"%d %d", &SantaWidth,&SantaHeight);
			}
			else {
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSantaRudolf1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSantaRudolf2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSantaRudolf3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, MediumSantaRudolf4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*MediumSantaRudolf1,"%d %d", &SantaWidth,&SantaHeight);
			}
		break;

		case 2:
			if (!Rudolf) {
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSanta1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSanta2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSanta3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSanta4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*BigSanta1,"%d %d", &SantaWidth,&SantaHeight);
			}
			else {
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSantaRudolf1, &SantaPixmap[0], &SantaMaskPixmap[0], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSantaRudolf2, &SantaPixmap[1], &SantaMaskPixmap[1], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSantaRudolf3, &SantaPixmap[2], &SantaMaskPixmap[2], NULL);
				rc = XpmCreatePixmapFromData(	display, rootWin, BigSantaRudolf4, &SantaPixmap[3], &SantaMaskPixmap[3], NULL);
				sscanf(*BigSantaRudolf1,"%d %d", &SantaWidth,&SantaHeight);
			}
		break;

		default:
			fprintf(stderr,"** Maximum Santa is %d\n",MAXSANTA);
			exit(1);
		break;

	}	/* switch */


}		/* InitSantaPixymaps */




/*
	 Give birth to a Santa. (What a conception)
*/
void
ResetSanta()      
{
	SantaX = -SantaWidth;
	SantaY = RandInt(display_height / 3)+40;
	SantaXStep = SantaSpeed;
	SantaYStep = 1;
	CurrentSanta = 0;
	SantaVisible = 1;
}


/*
	Update Santa (How can you update the oldest icon in the world? Oh well. )
*/
void
UpdateSanta()
{
int Visible;
int tmp_x;
static int SantaXWindXcel = 0;

	if (SantaVisible) EraseSanta();

	if (wind) {
		/* Acceleration of Santa */
		if (wind == 2) {
			if (direction > 0) SantaXWindXcel += 2;
			SantaXWindXcel++;
			if (SantaXWindXcel > 2*MAXSANTA_SPEED) SantaXWindXcel = 0;
			SantaX = SantaX + SantaXStep + direction*SantaXWindXcel; 
		}

		if( wind == 1 ) { 
			SantaXWindXcel -= 3;
			if (SantaXWindXcel < 0) SantaXWindXcel = 0;
			SantaX = SantaX + SantaXStep + direction*SantaXWindXcel/4;  
		}
	}
	else {
		SantaX = SantaX + SantaXStep;
	}
 
	if (RandInt(10) > 3) SantaY = SantaY + SantaYStep; 
	if (SantaY < 0) SantaY = 0;
	if (RandInt(100) > 80) SantaYStep = -SantaYStep;

	if (SantaX >= display_width) ResetSanta(); 

	Visible = XRectInRegion(WindowsR,
								          SantaX, SantaY,
								          SantaWidth,SantaHeight);
	SantaVisible = (Visible != RectangleIn);
	SantaVisible = (SantaVisible || (SantaX < 0));

	Visible = XRectInRegion(SnowCtchR,
								          SantaX, SantaY,
								          SantaWidth,SantaHeight);

	if (Visible != RectangleOut) {
		SubR = XCreateRegion();

		AddRect.x = SantaX;
		AddRect.y = SantaY;
		AddRect.width = SantaWidth;
		AddRect.height = SantaHeight;
		XUnionRectWithRegion(&AddRect, SubR,SubR);
		XSubtractRegion(SubR,WindowsR, SubR);
 
		XSubtractRegion(SnowCtchR,SubR, SnowCtchR);

		XDestroyRegion(SubR);
	}

	CurrentSanta++;
	if (CurrentSanta >= PIXINANIMATION) CurrentSanta = 0;

	if (SantaVisible) DrawSanta();
}




/*
	Draw Santa
*/
void
DrawSanta() {
	
	int rc;


	rc = XSetClipMask(	display,
											gc,
											SantaMaskPixmap[CurrentSanta]);

	rc = XSetClipOrigin(display,
											gc,
											SantaX,SantaY);

	rc = XCopyArea(			display,
											SantaPixmap[CurrentSanta],
											rootWin,
											gc,
											0,0,SantaWidth,SantaHeight,
											SantaX,SantaY);

	/* Note: the fur in his hat is *imitation* white-seal fur, of course. */
	/* Santa is a big supporter of Greenpeace.                            */

}	/* DrawSanta */



/*
	Erase Santa
*/
void
EraseSanta()
{
	XClearArea(display, rootWin,
						 SantaX , SantaY,     
						 SantaWidth,SantaHeight,
						 False);
}


/* ------------------------------------------------------------------ */ 


#if !GRAB_SERVER

int
XsnowErrors(dpy, err)
Display *dpy;
XErrorEvent *err;
{
		errorVal = err->error_code;

		return 0;
}

#endif /* GRAB_SERVER */


/*
	 Allocate a color by name.
*/
Pixel
AllocNamedColor(colorName, dfltPix)
char *colorName;
Pixel dfltPix;
{
	Pixel pix;
	XColor scrncolor;
	XColor exactcolor;

	if (XAllocNamedColor(display, DefaultColormap(display, screen),
		colorName, &scrncolor, &exactcolor)) {
		pix = scrncolor.pixel;
	}
	else {
		pix = dfltPix;
	}

	return pix;
}



void sig_alarm()
{
		int rand=RandInt(100);
		void (*sig_alarm_ptr)() = sig_alarm;

		/* No wind at all? */
		if (NoWind) return;

		if( rand > 80 ){
				geta -= 1;
		}else if( rand > 55 ){
				geta += 1;
		}

		if( rand > 65 ){
				if( RandInt(10) > 4 ){
						direction = 1;
				}else{
						direction = -1;
				}

				wind = 2;
				signal(SIGALRM, sig_alarm_ptr);

				alarm(abs(RandInt(5))+1);
		}else{
				if( wind == 2 ){
						wind = 1;
						signal(SIGALRM, sig_alarm_ptr);
						alarm(RandInt(3)+1);
				}else{
						wind = 0;
						signal(SIGALRM, sig_alarm_ptr);
						alarm(WindTimer);
				}
		}
}




int MakeWR(int Force) {

		Window *children;
		unsigned int nChildren;
		Window dummy;
		XWindowAttributes wa;
		int wx;
		XRectangle CtchRect;
		XRectangle AllowRect;
		XRectangle WinRect;
		int winX, winY;
		int NouMoe;
		unsigned int winHeight, winWidth;
		unsigned int depth;

#if GRAB_SERVER
		XGrabServer(display);
#else
		XSetErrorHandler(XsnowErrors);
#endif

		XDestroyRegion(WindowsR);
		WindowsR = XCreateRegion();

		XQueryTree(display, rootWin, &dummy, &dummy, &children, &nChildren);

		for (wx=0; wx<nChildren; wx++) {
				if (XEventsQueued(display, QueuedAlready) && (! Force)) {
					XFree((char *)children); 
					return 0;
				}
				errorVal = 0;
				XGetWindowAttributes(display, children[wx], &wa);
				if (errorVal) {
					printf("XGetWindowAttributes err\n");
					continue;
				}

				if (!NoPopuphandling && wa.save_under) continue;

				if (wa.map_state == IsViewable) {
						XGetGeometry(display, children[wx], &dummy, &winX, &winY,
								&winWidth, &winHeight, &borderWidth, &depth);
						if (errorVal) continue;
 
						WinRect.x = winX;
						WinRect.y = winY;
						WinRect.height = winHeight + 2*borderWidth;
						WinRect.width = winWidth + 2*borderWidth;

						XUnionRectWithRegion(&WinRect,WindowsR, WindowsR);
				}
		}

		XFree((char *)children);

#if GRAB_SERVER
		XUngrabServer(display);
#else
		XSetErrorHandler((ErrorHandler *)NULL);
#endif

		return 1;
}		/* MakeWR */




void ReclcCSR() {

	XDestroyRegion(RecalcSnowCtchR);
	RecalcSnowCtchR = XCreateRegion();


	XUnionRegion(WindowsR,RecalcSnowCtchR, RecalcSnowCtchR);
	XOffsetRegion(RecalcSnowCtchR, 0,MaxYStep+MaxSnowFlakeHeight);
	XSubtractRegion(WindowsR,RecalcSnowCtchR, RecalcSnowCtchR);

	if (NoKeepSWin) {
		XDestroyRegion(SnowCtchR);
		SnowCtchR = XCreateRegion();
		return;
	}

	XUnionRegion(RecalcSnowCtchR,SnowCtchR, SnowCtchR);

	if (SnowOffset != 0) XOffsetRegion(SnowCtchR,0,SnowOffset);

}		/* ReclcCSR */




void MkASR() {

	XRectangle ARect;


	XDestroyRegion(SnowAlwdR);
	SnowAlwdR = XCreateRegion();
	
	if (!NoKeepSWin) {
		XUnionRegion(WindowsR,SnowAlwdR, SnowAlwdR);
		XOffsetRegion(SnowAlwdR, 0,-MaxWinSnowDepth);
	
		XSubtractRegion(SnowAlwdR,WindowsR, SnowAlwdR);
	}

	if (!NoKeepSWin) {
		ARect.x = 0;
		ARect.y = display_height - MaxScrSnowDepth;
		ARect.width = display_width - 1;
		ARect.height = MaxYStep+MaxSnowFlakeHeight+MaxScrSnowDepth;
		XUnionRectWithRegion(&ARect, SnowAlwdR, SnowAlwdR);
	}
	XUnionRegion(SnowCtchR,SnowAlwdR, SnowAlwdR);

}		/* MkASR */





int DoDffWLT() {

	XRectangle DRect;
	
	if (!MakeWR(0)) return 0;

	XClipBox( WindowsR, &DRect );

	if (! XEqualRegion(WindowsR,PWindowsR) ) {

		XXorRegion(WindowsR,PWindowsR, WindowsDifferenceR);
		XClipBox( WindowsDifferenceR, &DRect );
	  XClearArea(display, rootWin,
	             DRect.x,
	             DRect.y,
	             DRect.width,              
	             DRect.height,
	             False);

		HandleEvA(DRect.x,DRect.y,DRect.width,DRect.height);

		XDestroyRegion(PWindowsR);
		PWindowsR = XCreateRegion();
		XUnionRegion(WindowsR,PWindowsR, PWindowsR);
	}
	else {
		;;
	}

	ReclcCSR();
	MkASR();

	return 1;

}		/* DoDffWLT */




void HandleEvA(int X,int Y,int W, int H) {

int AddX,AddY;
int ClearX, ClearY;

/*return;		/* DEBUG */

	AddX = X - MaxSnowFlakeHeight;
	AddY = Y - MaxWinSnowDepth;
	AddRect.height = H +  MaxWinSnowDepth;
	AddRect.width = W + 2*MaxSnowFlakeHeight;
	if (AddY < 0) {
	  AddRect.height = AddRect.height + AddY;
	  AddY = 0;
	}
	if ((AddY+AddRect.height) > display_height) {
	  AddRect.height = display_height - AddY;
	}

	AddRect.x = AddX;
	AddRect.y = AddY;

	SubR= XCreateRegion();
	  XUnionRectWithRegion(&AddRect, SubR,SubR);
	
	  AddY = AddY - MaxYStep-MaxSnowFlakeHeight;
	  AddRect.height = 
	    MaxWinSnowDepth+MaxYStep*2+MaxSnowFlakeHeight*2;
	  if (AddY < 0) {
	    AddRect.height = AddRect.height + AddY;
	    AddY = 0;
	  }
	  AddRect.y = AddY;
	  if ((AddY+AddRect.height) > display_height) {
	    AddRect.height = display_height - AddY;
	  }
	  XUnionRectWithRegion(&AddRect, SubR,SubR);
	  XClearArea(display, rootWin,
	             AddX,
	             AddY,
	             AddRect.width,              
	             AddRect.height,
	             False);

	  XSubtractRegion(SnowCtchR,SubR, SnowCtchR);
	XDestroyRegion(SubR);

}	/* HandleEvA */

/*
 *	oneko  -  X11 $@G-(J
 */

#ifndef	lint
static char rcsid[] = "$Header: /home/sun/unix/kato/xsam/oneko/oneko.c,v 1.5 90/10/19 21:25:16 kato Exp $";
#endif

#include "oneko.h"

/*
 *	$@%0%m!<%P%kJQ?t(J
 */

char	*ClassName = "Oneko";		/* $@%3%^%s%IL>>N(J */
char	*ProgramName;			/* $@%3%^%s%IL>>N(J */

Display	*theDisplay;			/* $@%G%#%9%W%l%$9=B$BN(J */
int	theScreen;			/* $@%9%/%j!<%sHV9f(J */
int	theDepth;			/* $@%G%W%9(J */
Window	theRoot;			/* $@%k!<%H%&%#%s%I%&$N#I#D(J */
Window	theWindow;			/* $@G-%&%#%s%I%&$N#I#D(J */
Cursor	theCursor;			/* $@$M$:$_%+!<%=%k(J */

int	WindowWidth;			/* $@%k!<%H%&%#%s%I%&$NI}(J */
int	WindowHeight;			/* $@%k!<%H%&%#%s%I%&$N9b$5(J */

XColor	theForegroundColor;		/* $@?'(J ($@%U%)%"%0%i%&%s%I(J) */
XColor	theBackgroundColor;		/* $@?'(J ($@%P%C%/%0%i%&%s%I(J) */

/*
 *	$@$$$m$$$m$J=i4|@_Dj(J ($@%*%W%7%g%s!"%j%=!<%9$GJQ$($i$l$k$h(J)
 */

					/* Resource:	*/
char	*Foreground = NULL;		/*   foreground	*/
char	*Background = NULL;		/*   background	*/
long	IntervalTime = 0L;		/*   time	*/
double	NekoSpeed = (double)0;		/*   speed	*/
int	IdleSpace = 0;			/*   idle	*/
int	NekoMoyou = NOTDEFINED;		/*   tora	*/
int	NoShape = NOTDEFINED;		/*   noshape	*/
int	ReverseVideo = NOTDEFINED;	/*   reverse	*/

/*
 *	$@$$$m$$$m$J>uBVJQ?t(J
 */

Bool	DontMapped = True;

int	NekoTickCount;		/* $@G-F0:n%+%&%s%?(J */
int	NekoStateCount;		/* $@G-F10l>uBV%+%&%s%?(J */
int	NekoState;		/* $@G-$N>uBV(J */

int	MouseX;			/* $@%^%&%9#X:BI8(J */
int	MouseY;			/* $@%^%&%9#Y:BI8(J */

int	PrevMouseX = 0;		/* $@D>A0$N%^%&%9#X:BI8(J */
int	PrevMouseY = 0;		/* $@D>A0$N%^%&%9#Y:BI8(J */

int	NekoX;			/* $@G-#X:BI8(J */
int	NekoY;			/* $@G-#Y:BI8(J */

int	NekoMoveDx;		/* $@G-0\F05wN%#X(J */
int	NekoMoveDy;		/* $@G-0\F05wN%#Y(J */

int	NekoLastX;		/* $@G-:G=*IA2h#X:BI8(J */
int	NekoLastY;		/* $@G-:G=*IA2h#Y:BI8(J */
GC	NekoLastGC;		/* $@G-:G=*IA2h(J GC */

/*
 *	$@$=$NB>(J
 */

double	SinPiPer8Times3;	/* sin($@#3&P!?#8(J) */
double	SinPiPer8;		/* sin($@&P!?#8(J) */

Pixmap	Mati2Xbm, Jare2Xbm, Kaki1Xbm, Kaki2Xbm, Mati3Xbm, Sleep1Xbm, Sleep2Xbm;
Pixmap	Mati2Msk, Jare2Msk, Kaki1Msk, Kaki2Msk, Mati3Msk, Sleep1Msk, Sleep2Msk;

Pixmap	AwakeXbm, AwakeMsk;

Pixmap	Up1Xbm, Up2Xbm, Down1Xbm, Down2Xbm, Left1Xbm, Left2Xbm;
Pixmap	Up1Msk, Up2Msk, Down1Msk, Down2Msk, Left1Msk, Left2Msk;
Pixmap	Right1Xbm, Right2Xbm, UpLeft1Xbm, UpLeft2Xbm, UpRight1Xbm;
Pixmap	Right1Msk, Right2Msk, UpLeft1Msk, UpLeft2Msk, UpRight1Msk;
Pixmap	UpRight2Xbm, DownLeft1Xbm, DownLeft2Xbm, DownRight1Xbm, DownRight2Xbm;
Pixmap	UpRight2Msk, DownLeft1Msk, DownLeft2Msk, DownRight1Msk, DownRight2Msk;

Pixmap	UpTogi1Xbm, UpTogi2Xbm, DownTogi1Xbm, DownTogi2Xbm, LeftTogi1Xbm;
Pixmap	UpTogi1Msk, UpTogi2Msk, DownTogi1Msk, DownTogi2Msk, LeftTogi1Msk;
Pixmap	LeftTogi2Xbm, RightTogi1Xbm, RightTogi2Xbm;
Pixmap	LeftTogi2Msk, RightTogi1Msk, RightTogi2Msk;

GC	Mati2GC, Jare2GC, Kaki1GC, Kaki2GC, Mati3GC, Sleep1GC, Sleep2GC;

GC	AwakeGC;

GC	Up1GC, Up2GC, Down1GC, Down2GC, Left1GC, Left2GC, Right1GC, Right2GC;
GC	UpLeft1GC, UpLeft2GC, UpRight1GC, UpRight2GC, DownLeft1GC, DownLeft2GC;
GC	DownRight1GC, DownRight2GC;

GC	UpTogi1GC, UpTogi2GC, DownTogi1GC, DownTogi2GC, LeftTogi1GC;
GC	LeftTogi2GC, RightTogi1GC, RightTogi2GC;

typedef struct {
    GC		*GCCreatePtr;
    Pixmap	*BitmapCreatePtr;
    char	*PixelPattern[2];
    Pixmap	*BitmapMasksPtr;
    char	*MaskPattern;
} BitmapGCData;

BitmapGCData	BitmapGCDataTable[] =
{
    { &Mati2GC, &Mati2Xbm, mati2_bits, mati2_tora_bits,
      &Mati2Msk, mati2_mask_bits },
    { &Jare2GC, &Jare2Xbm, jare2_bits, jare2_tora_bits,
      &Jare2Msk, jare2_mask_bits },
    { &Kaki1GC, &Kaki1Xbm, kaki1_bits, kaki1_tora_bits,
      &Kaki1Msk, kaki1_mask_bits },
    { &Kaki2GC, &Kaki2Xbm, kaki2_bits, kaki2_tora_bits,
      &Kaki2Msk, kaki2_mask_bits },
    { &Mati3GC, &Mati3Xbm, mati3_bits, mati3_tora_bits,
      &Mati3Msk, mati3_mask_bits },
    { &Sleep1GC, &Sleep1Xbm, sleep1_bits, sleep1_tora_bits,
      &Sleep1Msk, sleep1_mask_bits },
    { &Sleep2GC, &Sleep2Xbm, sleep2_bits, sleep2_tora_bits,
      &Sleep2Msk, sleep2_mask_bits },
    { &AwakeGC, &AwakeXbm, awake_bits, awake_tora_bits,
      &AwakeMsk, awake_mask_bits },
    { &Up1GC, &Up1Xbm, up1_bits, up1_tora_bits,
      &Up1Msk, up1_mask_bits },
    { &Up2GC, &Up2Xbm, up2_bits, up2_tora_bits,
      &Up2Msk, up2_mask_bits },
    { &Down1GC, &Down1Xbm, down1_bits, down1_tora_bits,
      &Down1Msk, down1_mask_bits },
    { &Down2GC, &Down2Xbm, down2_bits, down2_tora_bits,
      &Down2Msk, down2_mask_bits },
    { &Left1GC, &Left1Xbm, left1_bits, left1_tora_bits,
      &Left1Msk, left1_mask_bits },
    { &Left2GC, &Left2Xbm, left2_bits, left2_tora_bits,
      &Left2Msk, left2_mask_bits },
    { &Right1GC, &Right1Xbm, right1_bits, right1_tora_bits,
      &Right1Msk, right1_mask_bits },
    { &Right2GC, &Right2Xbm, right2_bits, right2_tora_bits,
      &Right2Msk, right2_mask_bits },
    { &UpLeft1GC, &UpLeft1Xbm, upleft1_bits, upleft1_tora_bits,
      &UpLeft1Msk, upleft1_mask_bits },
    { &UpLeft2GC, &UpLeft2Xbm, upleft2_bits, upleft2_tora_bits,
      &UpLeft2Msk, upleft2_mask_bits },
    { &UpRight1GC, &UpRight1Xbm, upright1_bits, upright1_tora_bits,
      &UpRight1Msk, upright1_mask_bits },
    { &UpRight2GC, &UpRight2Xbm, upright2_bits, upright2_tora_bits,
      &UpRight2Msk, upright2_mask_bits },
    { &DownLeft1GC, &DownLeft1Xbm, dwleft1_bits, dwleft1_tora_bits,
      &DownLeft1Msk, dwleft1_mask_bits },
    { &DownLeft2GC, &DownLeft2Xbm, dwleft2_bits, dwleft2_tora_bits,
      &DownLeft2Msk, dwleft2_mask_bits },
    { &DownRight1GC, &DownRight1Xbm, dwright1_bits, dwright1_tora_bits,
      &DownRight1Msk, dwright1_mask_bits },
    { &DownRight2GC, &DownRight2Xbm, dwright2_bits, dwright2_tora_bits,
      &DownRight2Msk, dwright2_mask_bits },
    { &UpTogi1GC, &UpTogi1Xbm, utogi1_bits, utogi1_tora_bits,
      &UpTogi1Msk, utogi1_mask_bits },
    { &UpTogi2GC, &UpTogi2Xbm, utogi2_bits, utogi2_tora_bits,
      &UpTogi2Msk, utogi2_mask_bits },
    { &DownTogi1GC, &DownTogi1Xbm, dtogi1_bits, dtogi1_tora_bits,
      &DownTogi1Msk, dtogi1_mask_bits },
    { &DownTogi2GC, &DownTogi2Xbm, dtogi2_bits, dtogi2_tora_bits,
      &DownTogi2Msk, dtogi2_mask_bits },
    { &LeftTogi1GC, &LeftTogi1Xbm, ltogi1_bits, ltogi1_tora_bits,
      &LeftTogi1Msk, ltogi1_mask_bits },
    { &LeftTogi2GC, &LeftTogi2Xbm, ltogi2_bits, ltogi2_tora_bits,
      &LeftTogi2Msk, ltogi2_mask_bits },
    { &RightTogi1GC, &RightTogi1Xbm, rtogi1_bits, rtogi1_tora_bits,
      &RightTogi1Msk, rtogi1_mask_bits },
    { &RightTogi2GC, &RightTogi2Xbm, rtogi2_bits, rtogi2_tora_bits,
      &RightTogi2Msk, rtogi2_mask_bits },
    { NULL, NULL, NULL, NULL, NULL, NULL }
};

typedef struct {
    GC		*TickGCPtr;
    Pixmap	*TickMaskPtr;
} Animation;

Animation	AnimationPattern[][2] =
{
    { &Mati2GC, &Mati2Msk },
    { &Mati2GC, &Mati2Msk },		/* NekoState == NEKO_STOP */
    { &Jare2GC, &Jare2Msk },
    { &Mati2GC, &Mati2Msk },		/* NekoState == NEKO_JARE */
    { &Kaki1GC, &Kaki1Msk },
    { &Kaki2GC, &Kaki2Msk },		/* NekoState == NEKO_KAKI */
    { &Mati3GC, &Mati3Msk },
    { &Mati3GC, &Mati3Msk },		/* NekoState == NEKO_AKUBI */
    { &Sleep1GC, &Sleep1Msk },
    { &Sleep2GC, &Sleep2Msk },		/* NekoState == NEKO_SLEEP */
    { &AwakeGC, &AwakeMsk },
    { &AwakeGC, &AwakeMsk },		/* NekoState == NEKO_AWAKE */
    { &Up1GC, &Up1Msk },
    { &Up2GC, &Up2Msk },		/* NekoState == NEKO_U_MOVE */
    { &Down1GC, &Down1Msk },
    { &Down2GC, &Down2Msk },		/* NekoState == NEKO_D_MOVE */
    { &Left1GC, &Left1Msk },
    { &Left2GC, &Left2Msk },		/* NekoState == NEKO_L_MOVE */
    { &Right1GC, &Right1Msk },
    { &Right2GC, &Right2Msk },		/* NekoState == NEKO_R_MOVE */
    { &UpLeft1GC, &UpLeft1Msk },
    { &UpLeft2GC, &UpLeft2Msk },	/* NekoState == NEKO_UL_MOVE */
    { &UpRight1GC, &UpRight1Msk },
    { &UpRight2GC, &UpRight2Msk },	/* NekoState == NEKO_UR_MOVE */
    { &DownLeft1GC, &DownLeft1Msk },
    { &DownLeft2GC, &DownLeft2Msk },	/* NekoState == NEKO_DL_MOVE */
    { &DownRight1GC, &DownRight1Msk },
    { &DownRight2GC, &DownRight2Msk },	/* NekoState == NEKO_DR_MOVE */
    { &UpTogi1GC, &UpTogi1Msk },
    { &UpTogi2GC, &UpTogi2Msk },	/* NekoState == NEKO_U_TOGI */
    { &DownTogi1GC, &DownTogi1Msk },
    { &DownTogi2GC, &DownTogi2Msk },	/* NekoState == NEKO_D_TOGI */
    { &LeftTogi1GC, &LeftTogi1Msk },
    { &LeftTogi2GC, &LeftTogi2Msk },	/* NekoState == NEKO_L_TOGI */
    { &RightTogi1GC, &RightTogi1Msk },
    { &RightTogi2GC, &RightTogi2Msk },	/* NekoState == NEKO_R_TOGI */
};


/*
 *	$@%S%C%H%^%C%W%G!<%?!&(JGC $@=i4|2=(J
 */

void
InitBitmapAndGCs()
{
    BitmapGCData	*BitmapGCDataTablePtr;
    XGCValues		theGCValues;

    theGCValues.function = GXcopy;
    theGCValues.foreground = theForegroundColor.pixel;
    theGCValues.background = theBackgroundColor.pixel;
    theGCValues.fill_style = FillTiled;
    theGCValues.ts_x_origin = 0;
    theGCValues.ts_y_origin = 0;

    for (BitmapGCDataTablePtr = BitmapGCDataTable;
	 BitmapGCDataTablePtr->GCCreatePtr != NULL;
	 BitmapGCDataTablePtr++) {

	*(BitmapGCDataTablePtr->BitmapCreatePtr)
	    = XCreatePixmapFromBitmapData(theDisplay, theRoot,
		BitmapGCDataTablePtr->PixelPattern[NekoMoyou],
		BITMAP_WIDTH, BITMAP_HEIGHT,
		theForegroundColor.pixel,
		theBackgroundColor.pixel,
		DefaultDepth(theDisplay, theScreen));

	theGCValues.tile = *(BitmapGCDataTablePtr->BitmapCreatePtr);

	*(BitmapGCDataTablePtr->BitmapMasksPtr)
	    = XCreateBitmapFromData(theDisplay, theRoot,
		BitmapGCDataTablePtr->MaskPattern,
		BITMAP_WIDTH, BITMAP_HEIGHT);

	*(BitmapGCDataTablePtr->GCCreatePtr)
	    = XCreateGC(theDisplay, theWindow,
		GCFunction | GCForeground | GCBackground | GCTile |
		GCTileStipXOrigin | GCTileStipYOrigin | GCFillStyle,
		&theGCValues);
    }
}

/*
 *	$@%j%=!<%9!&%G!<%?%Y!<%9$+$iI,MW$J%j%=!<%9$r<h$j=P$9(J
 */

char	*
NekoGetDefault(resource)
char	*resource;
{
	char	*value;

	if (value = XGetDefault(theDisplay, ProgramName, resource)) {
		return value;
	}
	if (value = XGetDefault(theDisplay, ClassName, resource)) {
		return value;
	}
	return NULL;
}

/*
 *	$@%j%=!<%9!&%G!<%?%Y!<%9$+$i%*%W%7%g%s$r@_Dj(J
 */

GetResources()
{
    char	*resource;
    int		num;

    if (Foreground == NULL) {
	if ((resource = NekoGetDefault("foreground")) != NULL) {
	    Foreground = resource;
	}
    }

    if (Background == NULL) {
	if ((resource = NekoGetDefault("background")) != NULL) {
	    Background = resource;
	}
    }

    if (IntervalTime == 0) {
	if ((resource = NekoGetDefault("time")) != NULL) {
	    if (num = atoi(resource)) {
		IntervalTime = num;
	    }
	}
    }

    if (NekoSpeed == (double)0) {
	if ((resource = NekoGetDefault("speed")) != NULL) {
	    if (num = atoi(resource)) {
		NekoSpeed = (double)num;
	    }
	}
    }

    if (IdleSpace == 0) {
	if ((resource = NekoGetDefault("idle")) != NULL) {
	    if (num = atoi(resource)) {
		IdleSpace = num;
	    }
	}
    }

    if (NekoMoyou == NOTDEFINED) {
	if ((resource = NekoGetDefault("tora")) != NULL) {
	    if (IsTrue(resource)) {
		NekoMoyou = TORA_NEKO;
	    }
	}
    }

    if (NoShape == NOTDEFINED) {
	if ((resource = NekoGetDefault("noshape")) != NULL) {
	    NoShape = IsTrue(resource);
	}
    }

    if (ReverseVideo == NOTDEFINED) {
	if ((resource = NekoGetDefault("reverse")) != NULL) {
	    ReverseVideo = IsTrue(resource);
	}
    }

    if (Foreground == NULL) {
	Foreground = DEFAULT_FOREGROUND;
    }
    if (Background == NULL) {
	Background = DEFAULT_BACKGROUND;
    }
    if (IntervalTime == 0) {
	IntervalTime = DEFAULT_INTERVAL;
    }
    if (NekoSpeed == (double)0) {
	NekoSpeed = (double)DEFAULT_NEKO_SPEED;
    }
    if (IdleSpace == 0) {
	IdleSpace = DEFAULT_IDLE_SPACE;
    }
    if (NekoMoyou == NOTDEFINED) {
	NekoMoyou = NORMAL_NEKO;
    }
    if (NoShape == NOTDEFINED) {
	NoShape = False;
    }
    if (ReverseVideo == NOTDEFINED) {
	ReverseVideo = False;
    }
}

/*
 *	$@$M$:$_7?%+!<%=%k$r:n$k(J
 */

MakeMouseCursor()
{
    Pixmap			theCursorSource;
    Pixmap			theCursorMask;

    theCursorSource
	= XCreateBitmapFromData(theDisplay, theRoot,
		cursor_bits, cursor_width, cursor_height);

    theCursorMask
	= XCreateBitmapFromData(theDisplay, theRoot,
		cursor_mask_bits, cursor_mask_width, cursor_mask_height);

    theCursor = XCreatePixmapCursor(theDisplay, theCursorSource, theCursorMask,
		&theBackgroundColor, &theForegroundColor,
		cursor_x_hot, cursor_y_hot);
}

/*
 *	$@?'$r=i4|@_Dj$9$k(J
 */

SetupColors()
{
    XColor	theExactColor;
    Colormap	theColormap;

    theColormap = DefaultColormap(theDisplay, theScreen);

    if (theDepth == 1) {
	Foreground = "black";
	Background = "white";
    }

    if (ReverseVideo == True) {
	char	*tmp;

	tmp = Foreground;
	Foreground = Background;
	Background = tmp;
    }

    if (!XAllocNamedColor(theDisplay, theColormap,
		Foreground, &theForegroundColor, &theExactColor)) {
	fprintf(stderr, "%s: Can't XAllocNamedColor(\"%s\").\n",
		ProgramName, Foreground);
	exit(1);
    }

    if (!XAllocNamedColor(theDisplay, theColormap,
		Background, &theBackgroundColor, &theExactColor)) {
	fprintf(stderr, "%s: Can't XAllocNamedColor(\"%s\").\n",
		ProgramName, Background);
	exit(1);
    }
}

/*
 *	$@%9%/%j!<%s4D6-=i4|2=(J
 */

void
InitScreen(DisplayName)
    char	*DisplayName;
{
    XSetWindowAttributes	theWindowAttributes;
    unsigned long		theWindowMask;
    Window			theTempRoot;
    int				WindowPointX;
    int				WindowPointY;
    unsigned long		BorderWidth;
    int				event_base, error_base;

    if ((theDisplay = XOpenDisplay(DisplayName)) == NULL) {
	fprintf(stderr, "%s: Can't open display", ProgramName);
	if (DisplayName != NULL) {
	    fprintf(stderr, " %s.\n", DisplayName);
	} else {
	    fprintf(stderr, ".\n");
	}
	exit(1);
    }

    GetResources();

#ifdef SHAPE
    if (!NoShape && XShapeQueryExtension(theDisplay,
			&event_base, &error_base) == False) {
	fprintf(stderr, "Display not suported shape extension.\n");
	NoShape = True;
    }
#endif SHAPE

    theScreen = DefaultScreen(theDisplay);
    theDepth = DefaultDepth(theDisplay, theScreen);

    theRoot = RootWindow(theDisplay, theScreen);

    XGetGeometry(theDisplay, theRoot, &theTempRoot,
		  &WindowPointX, &WindowPointY,
		  &WindowWidth, &WindowHeight,
		  &BorderWidth, &theDepth);

    SetupColors();
    MakeMouseCursor();

    theWindowAttributes.background_pixel = theBackgroundColor.pixel;
    theWindowAttributes.cursor = theCursor;
    theWindowAttributes.save_under = True;
    theWindowAttributes.override_redirect = True;

    theWindowMask = CWCursor;
    XChangeWindowAttributes(theDisplay, theRoot, theWindowMask,
			       &theWindowAttributes);

    theWindowMask = CWBackPixel		|
		    CWCursor		|
		    CWOverrideRedirect;

    theWindow = XCreateWindow(theDisplay, theRoot, 0, 0,
#ifdef SHAPE
			(NoShape == False)? WindowWidth: BITMAP_WIDTH,
			(NoShape == False)? WindowHeight: BITMAP_HEIGHT,
#else SHAPE
			BITMAP_WIDTH, BITMAP_HEIGHT,
#endif SHAPE
			0, theDepth, InputOutput, CopyFromParent,
			theWindowMask, &theWindowAttributes);

    XStoreName(theDisplay, theWindow, ProgramName);

    InitBitmapAndGCs();

    XSelectInput(theDisplay, theWindow, EVENT_MASK);

    XFlush(theDisplay);
}


/*
 *	$@%$%s%?!<%P%k(J
 *
 *	$@!!$3$N4X?t$r8F$V$H!"$"$k0lDj$N;~4VJV$C$F$3$J$/$J$k!#G-(J
 *	$@$NF0:n%?%$%_%s%0D4@0$KMxMQ$9$k$3$H!#(J
 */

void
Interval()
{
    pause();
}


/*
 *	$@%F%#%C%/%+%&%s%H=hM}(J
 */

void
TickCount()
{
    if (++NekoTickCount >= MAX_TICK) {
	NekoTickCount = 0;
    }

    if (NekoTickCount % 2 == 0) {
	if (NekoStateCount < MAX_TICK) {
	    NekoStateCount++;
	}
    }
}


/*
 *	$@G->uBV@_Dj(J
 */

void
SetNekoState(SetValue)
    int		SetValue;
{
    NekoTickCount = 0;
    NekoStateCount = 0;

    NekoState = SetValue;
}


/*
 *	$@G-IA2h=hM}(J
 */

void
DrawNeko(x, y, DrawAnime)
    int		x;
    int		y;
    Animation	DrawAnime;
{
    register GC		DrawGC = *(DrawAnime.TickGCPtr);
    register Pixmap	DrawMask = *(DrawAnime.TickMaskPtr);

    if ((x != NekoLastX) || (y != NekoLastY)
		|| (DrawGC != NekoLastGC)) {

#ifdef SHAPE
	if (NoShape == False) {
	    XShapeCombineMask(theDisplay, theWindow, ShapeBounding,
		x, y, DrawMask, ShapeSet);

	    if (DontMapped) {
		XMapRaised(theDisplay, theWindow);
		DontMapped = 0;
	    }

	    XSetTSOrigin(theDisplay, DrawGC, x, y);

	    XFillRectangle(theDisplay, theWindow, DrawGC,
		x, y, BITMAP_WIDTH, BITMAP_HEIGHT);
	} else
#endif SHAPE
	{
	    XWindowChanges	theChanges;

	    theChanges.x = x;
	    theChanges.y = y;
	    XConfigureWindow(theDisplay, theWindow, CWX | CWY, &theChanges);

	    if (DontMapped) {
		XMapWindow(theDisplay, theWindow);
		DontMapped = 0;
	    }

	    XFillRectangle(theDisplay, theWindow, DrawGC,
		0, 0, BITMAP_WIDTH, BITMAP_HEIGHT);
	}
    }

    XFlush(theDisplay);

    NekoLastX = x;
    NekoLastY = y;

    NekoLastGC = DrawGC;
}


/*
 *	$@G-:FIA2h=hM}(J
 */

void
RedrawNeko()
{
#ifdef SHAPE
    if (NoShape == False) {
	XFillRectangle(theDisplay, theWindow, NekoLastGC,
		NekoLastX, NekoLastY,
		BITMAP_WIDTH, BITMAP_HEIGHT);
    } else
#endif SHAPE
    {
	XFillRectangle(theDisplay, theWindow, NekoLastGC,
		0, 0, BITMAP_WIDTH, BITMAP_HEIGHT);
    }

    XFlush(theDisplay);
}


/*
 *	$@G-0\F0J}K!7hDj(J
 */

void
NekoDirection()
{
    int			NewState;
    double		LargeX, LargeY;
    double		Length;
    double		SinTheta;

    if (NekoMoveDx == 0 && NekoMoveDy == 0) {
	NewState = NEKO_STOP;
    } else {
	LargeX = (double)NekoMoveDx;
	LargeY = (double)(-NekoMoveDy);
	Length = sqrt(LargeX * LargeX + LargeY * LargeY);
	SinTheta = LargeY / Length;

	if (NekoMoveDx > 0) {
	    if (SinTheta > SinPiPer8Times3) {
		NewState = NEKO_U_MOVE;
	    } else if ((SinTheta <= SinPiPer8Times3)
			&& (SinTheta > SinPiPer8)) {
		NewState = NEKO_UR_MOVE;
	    } else if ((SinTheta <= SinPiPer8)
			&& (SinTheta > -(SinPiPer8))) {
		NewState = NEKO_R_MOVE;
	    } else if ((SinTheta <= -(SinPiPer8))
			&& (SinTheta > -(SinPiPer8Times3))) {
		NewState = NEKO_DR_MOVE;
	    } else {
		NewState = NEKO_D_MOVE;
	    }
	} else {
	    if (SinTheta > SinPiPer8Times3) {
		NewState = NEKO_U_MOVE;
	    } else if ((SinTheta <= SinPiPer8Times3)
			&& (SinTheta > SinPiPer8)) {
		NewState = NEKO_UL_MOVE;
	    } else if ((SinTheta <= SinPiPer8)
			&& (SinTheta > -(SinPiPer8))) {
		NewState = NEKO_L_MOVE;
	    } else if ((SinTheta <= -(SinPiPer8))
			&& (SinTheta > -(SinPiPer8Times3))) {
		NewState = NEKO_DL_MOVE;
	    } else {
		NewState = NEKO_D_MOVE;
	    }
	}
    }

    if (NekoState != NewState) {
	SetNekoState(NewState);
    }
}


/*
 *	$@G-JI$V$D$+$jH=Dj(J
 */

Bool
IsWindowOver()
{
    Bool	ReturnValue = False;

    if (NekoY <= 0) {
	NekoY = 0;
	ReturnValue = True;
    } else if (NekoY >= WindowHeight - BITMAP_HEIGHT) {
	NekoY = WindowHeight - BITMAP_HEIGHT;
	ReturnValue = True;
    }
    if (NekoX <= 0) {
	NekoX = 0;
	ReturnValue = True;
    } else if (NekoX >= WindowWidth - BITMAP_WIDTH) {
	NekoX = WindowWidth - BITMAP_WIDTH;
	ReturnValue = True;
    }

    return(ReturnValue);
}


/*
 *	$@G-0\F0>u67H=Dj(J
 */

Bool
IsNekoDontMove()
{
    if (NekoX == NekoLastX && NekoY == NekoLastY) {
	return(True);
    } else {
	return(False);
    }
}


/*
 *	$@G-0\F03+;OH=Dj(J
 */

Bool
IsNekoMoveStart()
{
    if ((PrevMouseX >= MouseX - IdleSpace
	 && PrevMouseX <= MouseX + IdleSpace) &&
	 (PrevMouseY >= MouseY - IdleSpace 
	 && PrevMouseY <= MouseY + IdleSpace)) {
	return(False);
    } else {
	return(True);
    }
}


/*
 *	$@G-0\F0(J dx, dy $@7W;;(J
 */

void
CalcDxDy()
{
    Window		QueryRoot, QueryChild;
    int			AbsoluteX, AbsoluteY;
    int			RelativeX, RelativeY;
    unsigned int	ModKeyMask;
    double		LargeX, LargeY;
    double		DoubleLength, Length;

    XQueryPointer(theDisplay, theWindow,
		   &QueryRoot, &QueryChild,
		   &AbsoluteX, &AbsoluteY,
		   &RelativeX, &RelativeY,
		   &ModKeyMask);

    PrevMouseX = MouseX;
    PrevMouseY = MouseY;

    MouseX = AbsoluteX;
    MouseY = AbsoluteY;

    LargeX = (double)(MouseX - NekoX - BITMAP_WIDTH / 2);
    LargeY = (double)(MouseY - NekoY - BITMAP_HEIGHT);

    DoubleLength = LargeX * LargeX + LargeY * LargeY;

    if (DoubleLength != (double)0) {
	Length = sqrt(DoubleLength);
	if (Length <= NekoSpeed) {
	    NekoMoveDx = (int)LargeX;
	    NekoMoveDy = (int)LargeY;
	} else {
	    NekoMoveDx = (int)((NekoSpeed * LargeX) / Length);
	    NekoMoveDy = (int)((NekoSpeed * LargeY) / Length);
	}
    } else {
	NekoMoveDx = NekoMoveDy = 0;
    }
}


/*
 *	$@F0:n2r@OG-IA2h=hM}(J
 */

void
NekoThinkDraw()
{
    CalcDxDy();

    if (NekoState != NEKO_SLEEP) {
	DrawNeko(NekoX, NekoY,
		AnimationPattern[NekoState][NekoTickCount & 0x1]);
    } else {
	DrawNeko(NekoX, NekoY,
		AnimationPattern[NekoState][(NekoTickCount >> 2) & 0x1]);
    }

    TickCount();

    switch (NekoState) {
    case NEKO_STOP:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_STOP_TIME) {
	    break;
	}
	if (NekoMoveDx < 0 && NekoX <= 0) {
	    SetNekoState(NEKO_L_TOGI);
	} else if (NekoMoveDx > 0 && NekoX >= WindowWidth - BITMAP_WIDTH) {
	    SetNekoState(NEKO_R_TOGI);
	} else if (NekoMoveDy < 0 && NekoY <= 0) {
	    SetNekoState(NEKO_U_TOGI);
	} else if (NekoMoveDy > 0 && NekoY >= WindowHeight - BITMAP_HEIGHT) {
	    SetNekoState(NEKO_D_TOGI);
	} else {
	    SetNekoState(NEKO_JARE);
	}
	break;
    case NEKO_JARE:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_JARE_TIME) {
	    break;
	}
	SetNekoState(NEKO_KAKI);
	break;
    case NEKO_KAKI:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_KAKI_TIME) {
	    break;
	}
	SetNekoState(NEKO_AKUBI);
	break;
    case NEKO_AKUBI:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_AKUBI_TIME) {
	    break;
	}
	SetNekoState(NEKO_SLEEP);
	break;
    case NEKO_SLEEP:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	break;
    case NEKO_AWAKE:
	if (NekoStateCount < NEKO_AWAKE_TIME) {
	    break;
	}
	NekoDirection();	/* $@G-$,F0$/8~$-$r5a$a$k(J */
	break;
    case NEKO_U_MOVE:
    case NEKO_D_MOVE:
    case NEKO_L_MOVE:
    case NEKO_R_MOVE:
    case NEKO_UL_MOVE:
    case NEKO_UR_MOVE:
    case NEKO_DL_MOVE:
    case NEKO_DR_MOVE:
	NekoX += NekoMoveDx;
	NekoY += NekoMoveDy;
	NekoDirection();
	if (IsWindowOver()) {
	    if (IsNekoDontMove()) {
		SetNekoState(NEKO_STOP);
	    }
	}
	break;
    case NEKO_U_TOGI:
    case NEKO_D_TOGI:
    case NEKO_L_TOGI:
    case NEKO_R_TOGI:
	if (IsNekoMoveStart()) {
	    SetNekoState(NEKO_AWAKE);
	    break;
	}
	if (NekoStateCount < NEKO_TOGI_TIME) {
	    break;
	}
	SetNekoState(NEKO_KAKI);
	break;
    default:
	/* Internal Error */
	SetNekoState(NEKO_STOP);
	break;
    }

    Interval();
}


/*
 *	$@%-!<%$%Y%s%H=hM}(J
 */

Bool
ProcessKeyPress(theKeyEvent)
    XKeyEvent	*theKeyEvent;
{
    int			Length;
    int			theKeyBufferMaxLen = AVAIL_KEYBUF;
    char		theKeyBuffer[AVAIL_KEYBUF + 1];
    KeySym		theKeySym;
    XComposeStatus	theComposeStatus;
    Bool		ReturnState;

    ReturnState = True;

    Length = XLookupString(theKeyEvent,
			    theKeyBuffer, theKeyBufferMaxLen,
			    &theKeySym, &theComposeStatus);

    if (Length > 0) {
	switch (theKeyBuffer[0]) {
	case 'q':
	case 'Q':
	    if (theKeyEvent->state & Mod1Mask) {	/* META (Alt) $@%-!<(J */
		ReturnState = False;
	    }
	    break;
	default:
	    break;
	}
    }

    return(ReturnState);
}


/*
 *	$@%$%Y%s%H=hM}(J
 */

Bool
ProcessEvent()
{
    XEvent	theEvent;
    Bool	ContinueState = True;

    while (XCheckMaskEvent(theDisplay, EVENT_MASK, &theEvent)) {
	switch (theEvent.type) {
	case Expose:
	    if (theEvent.xexpose.count == 0) {
		RedrawNeko();
	    }
	    break;
	case KeyPress:
	    ContinueState = ProcessKeyPress(&theEvent);
	    if (!ContinueState) {
		    return(ContinueState);
	    }
	    break;
	default:
	    /* Unknown Event */
	    break;
	}
    }

    return(ContinueState);
}


/*
 *	$@G-=hM}(J
 */

void
ProcessNeko()
{
    struct itimerval	Value;

    /* $@G-$N=i4|2=(J */

    NekoX = (WindowWidth - BITMAP_WIDTH / 2) / 2;
    NekoY = (WindowHeight - BITMAP_HEIGHT / 2) / 2;

    NekoLastX = NekoX;
    NekoLastY = NekoY;

    SetNekoState(NEKO_STOP);

    /* $@%?%$%^!<@_Dj(J */

    timerclear(&Value.it_interval);
    timerclear(&Value.it_value);

    Value.it_interval.tv_usec = IntervalTime;
    Value.it_value.tv_usec = IntervalTime;

    setitimer(ITIMER_REAL, &Value, 0);

    /* $@%a%$%s=hM}(J */

    do {
	NekoThinkDraw();
    } while (ProcessEvent());
}


/*
 *	SIGALRM $@%7%0%J%k=hM}(J
 */

void
NullFunction()
{
    /* No Operation */
}

/*
 *	SIGINT $@%7%0%J%k=hM}(J
 */

void
RestoreCursor()
{
    XSetWindowAttributes	theWindowAttributes;

    theWindowAttributes.cursor = None;
    XChangeWindowAttributes(theDisplay, theRoot, CWCursor,
			&theWindowAttributes);
    XCloseDisplay(theDisplay);
    exit(0);
}


/*
 *	Usage
 */

char	*message[] = {
"",
"Options are:",
"-display <display>	: Neko appears on specified display.",
"-fg <color>		: Foreground color",
"-bg <color>		: Background color",
"-tora			: TORANEKO mode",
"-speed <dots>",
"-time <microseconds>",
"-idle <dots>",
"-rv			: Reverse video. (effects monochrome display only)",
NULL };

void
Usage()
{
    char	**mptr;

    mptr = message;
    fprintf(stderr, "Usage: %s [<options>]\n", ProgramName);
    while (*mptr) {
	printf("%s\n", *mptr);
	mptr++;
    }
}


/*
 *	$@%*%W%7%g%s$NM}2r(J
 */

Bool
GetArguments(argc, argv, theDisplayName)
    int		argc;
    char	*argv[];
    char	*theDisplayName;
{
    int		ArgCounter;

    theDisplayName[0] = '\0';

    if (strcmp(ProgramName, "tora") == 0) {
	NekoMoyou = TORA_NEKO;
    }

    for (ArgCounter = 0; ArgCounter < argc; ArgCounter++) {

	if (strncmp(argv[ArgCounter], "-h", 2) == 0) {
	    Usage();
	    exit(0);
	} else if (strcmp(argv[ArgCounter], "-display") == 0) {
	    ArgCounter++;
	    if (ArgCounter < argc) {
		strcpy(theDisplayName, argv[ArgCounter]);
	    } else {
		fprintf(stderr, "%s: -display option error.\n", ProgramName);
		exit(1);
	    }
	} else if (strcmp(argv[ArgCounter], "-speed") == 0) {
	    ArgCounter++;
	    if (ArgCounter < argc) {
		NekoSpeed = atof(argv[ArgCounter]);
	    } else {
		fprintf(stderr, "%s: -speed option error.\n", ProgramName);
		exit(1);
	    }
	} else if (strcmp(argv[ArgCounter], "-time") == 0) {
	    ArgCounter++;
	    if (ArgCounter < argc) {
		IntervalTime = atol(argv[ArgCounter]);
	    } else {
		fprintf(stderr, "%s: -time option error.\n", ProgramName);
		exit(1);
	    }
	} else if (strcmp(argv[ArgCounter], "-idle") == 0) {
	    ArgCounter++;
	    if (ArgCounter < argc) {
		IdleSpace = atol(argv[ArgCounter]);
	    } else {
		fprintf(stderr, "%s: -idle option error.\n", ProgramName);
		exit(1);
	    }
	} else if ((strcmp(argv[ArgCounter], "-fg") == 0) ||
		(strcmp(argv[ArgCounter], "-foreground") == 0)) {
	    ArgCounter++;
	    Foreground = argv[ArgCounter];
	} else if ((strcmp(argv[ArgCounter], "-bg") == 0) ||
		(strcmp(argv[ArgCounter], "-background") == 0)) {
	    ArgCounter++;
	    Background = argv[ArgCounter];
	} else if (strcmp(argv[ArgCounter], "-rv") == 0) {
	    ReverseVideo = True;
	} else if (strcmp(argv[ArgCounter], "-tora") == 0) {
	    NekoMoyou = TORA_NEKO;
	} else if (strcmp(argv[ArgCounter], "-noshape") == 0) {
	    NoShape = True;
	} else {
	    fprintf(stderr,
		     "%s: Unknown option \"%s\".\n", ProgramName,
						     argv[ArgCounter]);
	    Usage();
	    exit(1);
	}
    }

    if (strlen(theDisplayName) < 1) {
	theDisplayName = NULL;
    }
}


/*
 *	$@%a%$%s4X?t(J
 */

int
main(argc, argv)
    int		argc;
    char	*argv[];
{
    char	theDisplayName[MAXDISPLAYNAME];

    ProgramName = argv[0];

    argc--;
    argv++;

    GetArguments(argc, argv, theDisplayName);

    InitScreen(theDisplayName);

    signal(SIGALRM, NullFunction);
    signal(SIGINT, RestoreCursor);
    signal(SIGTERM, RestoreCursor);
    signal(SIGQUIT, RestoreCursor);

    SinPiPer8Times3 = sin(PI_PER8 * (double)3);
    SinPiPer8 = sin(PI_PER8);

    ProcessNeko();

    RestoreCursor();
}

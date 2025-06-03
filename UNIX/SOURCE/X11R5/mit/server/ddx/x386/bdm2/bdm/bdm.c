/*
 * BDM2: Banked dumb monochrome driver
 * Pascal Haible 8/93, haible@izfm.uni-stuttgart.de
 *
 * bdm2/bdm/bdm.c
 *
 * derived from:
 * hga2/*
 * Author:  Davor Matic, dmatic@athena.mit.edu
 * and
 * vga256/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * see bdm2/COPYRIGHT for copyright and disclaimers.
 */

/* $XFree86: mit/server/ddx/x386/bdm2/bdm/bdm.c,v 2.7 1994/02/10 21:25:11 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "mipointer.h"
#include "cursorstr.h"
#include "gcstruct.h"

#include "compiler.h"

#include "x386.h"
#include "x386Priv.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "mfb.h"
#include "bdm.h"

/* From bdmBank.h */
#ifdef __BSD__
#define BDMBASE 0xFF000000
#else
#define BDMBASE 0xF0000000
#endif

extern int bdm_mfbDoBitbltCopy();
extern int bdm_mfbDoBitbltCopyInverted();
extern int oneBankbdm_mfbDoBitbltCopy();
extern int oneBankbdm_mfbDoBitbltCopyInverted();
       int (*ourbdm_mfbDoBitbltCopy)();
       int (*ourbdm_mfbDoBitbltCopyInverted)();

extern void bdmPushRead2Banks();
extern void bdmPushRead1Bank();
extern void bdmPopRead2Banks();
extern void bdmPopRead1Bank();
       void (*bdmPushRead)() = NoopDDA;
       void (*bdmPopRead)()  = NoopDDA;

extern Bool x386Exiting, x386Resetting, x386ProbeFailed;
extern void NoopDDA();

/* ScrnInfoRec (common/x386.h) describes the SCREEN */
/* This record is device and driver independent */
/* Used by the upper levels: x386Init.c uses */
/*   ScrnInfoPtr x386Screens[] defined in x386Conf.c */
/*   Each entry there is a possibly configured device */
ScrnInfoRec bdm2InfoRec = {
  FALSE,		/* Bool configured */
  -1,			/* int tmpIndex */
  -1,			/* int scrnIndex */
  bdmProbe,		/* Bool (* Probe)() */
  bdmScreenInit,	/* Bool (* Init)() */
  bdmEnterLeaveVT,	/* void (* EnterLeaveVT)() */
  NoopDDA,		/* void (* EnterLeaveMonitor)() */
  NoopDDA,		/* void (* EnterLeaveCursor)() */
  bdmAdjustFrame,	/* void (* AdjustFrame)() */
  (Bool (*)())NoopDDA,	/* Bool (* SwitchMode)() */
  bdmPrintIdent,	/* void (* PrintIdent)() */
  1,			/* int depth */
  1,			/* int bitsPerPixel */
  StaticGray,		/* int defaultVisual */
  -1, -1,		/* int virtualX,virtualY */
  -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
  {0, },		/* OFlagSet options */
  {0, },		/* OFlagSet clockOptions */
  {0, },                /* OFlagSet xconfigFlag */
  NULL,			/* char *chipset */
  0,			/* int clocks */
  {0, },		/* int clock[MAXCLOCKS] */
  0,			/* int maxClock */
  0,			/* int videoRam */
  0,			/* int BIOSbase */
  0,			/* int MemBase, memory base address of framebuffer */
  350, 285,		/* int width, height : Screen picture size in mm */
			/* This should be given from the lowlevel driver! */
  0,			/* unsigned long speedup */
  NULL,			/* DisplayModePtr modes */
  NULL,			/* char *clockprog */
  -1,			/* int textclock */
  FALSE,		/* Bool bankedMono */
  "BDM2",		/* char *name */
  {0, },		/* RgbRec blackColour */
  {0, },		/* RgbRec whiteColour */
  bdm2ValidTokens,	/* int *validTokens */
  BDM2_PATCHLEVEL,	/* char *patchLevel */
};

pointer bdmBase = NULL;
pointer bdmVirtBase = NULL;

pointer bdmOrigVideoState = NULL;
pointer bdmNewVideoState = NULL;

extern void *bdmwriteseg; /* ?? */

/* Banking constants set from chipset values */
void *bdmReadBottom;
void *bdmReadTop;
void *bdmWriteBottom;
void *bdmWriteTop = (pointer)&bdmwriteseg; /* dummy for linking (???) */
int bdmMapBase;
int bdmMapSize;
int bdmSegmentSize;
int bdmSegmentShift;
int bdmSegmentMask;
Bool bdmUse2Banks;
int bdmScanLineWidth;
/* Used by bdmBank.h */
Bool bdmReadFlag, bdmWriteFlag;

/*
 * hooks for communicating with the VideoChip on the BDM
 */
void (* bdmEnterLeaveFunc)()   = NoopDDA;
void * (* bdmInitFunc)()       = (void *(*)())NoopDDA;
void * (* bdmSaveFunc)()       = (void *(*)())NoopDDA;
void (* bdmRestoreFunc)()      = NoopDDA;
void (* bdmAdjustFunc)()       = NoopDDA;
Bool (* bdmSaveScreenFunc)()   = (Bool (*)())NoopDDA;
void (* bdmGetModeFunc)()      = NoopDDA;
void (* bdmSetReadFunc)()      = NoopDDA;
void (* bdmSetWriteFunc)()     = NoopDDA;
void (* bdmSetReadWriteFunc)() = NoopDDA;

OFlagSet bdmOptionFlags;

/* Pointer to the Screen while VT is switched out */
static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

/* Banking functions etc. are saved and set to NoopDDA
 * while VT is switched out */
static void * (* saveInitFunc)();
static void * (* saveSaveFunc)();
static void (* saveRestoreFunc)();
static void (* saveAdjustFunc)();
static Bool (* saveSaveScreenFunc)();
static void (* saveSetReadFunc)();
static void (* saveSetWriteFunc)();
static void (* saveSetReadWriteFunc)();

extern miPointerScreenFuncRec x386PointerScreenFuncs;

extern bdmVideoChipPtr bdmDrivers[];

/*
 * bdmPrintIdent
 *     Prints out identifying strings for drivers included in the server
 */
void
bdmPrintIdent()
{
int i, j, n=0, c=0;
char* id;

ErrorF("  %s: server for banked dumb monochrome graphics adaptors",
	bdm2InfoRec.name);
ErrorF(" (Patchlevel %s):\n      ", bdm2InfoRec.patchLevel);
  
for (i=0; bdmDrivers[i]; i++)
	for (j = 0; id = (bdmDrivers[i]->ChipIdent)(j); j++, n++) {
		if (n) {
			ErrorF(",");
			c++;
			if (c + 1 + strlen(id) < 70) {
				ErrorF(" ");
				c++;
			}
			else {
				ErrorF("\n      ");
				c = 0;
			}
		}
		ErrorF("%s",id);
		c += strlen(id);
	}
ErrorF("\n");
}

/*
 * bdmProbe --
 *     probe and initialize the hardware driver
 */
Bool
bdmProbe()
{
  int            i, j;

  for (i=0; bdmDrivers[i]; i++) {
    /* Probe every driver */
    if ((bdmDrivers[i]->ChipProbe)())
      { /* The first that succeeded */
	/* succeeding probe fills in:
	 * bdm2.InfoRec.chipset,
	 * bdm2.InfoRec.videoRam,
	 * bdm2InfoRec.virtualX,
	 * bdm2InfoRec.virtualY,
	 */
        if (x386Verbose) {
		ErrorF("%s %s: chipset:  %s\n",
			OFLG_ISSET(XCONFIG_CHIPSET,&bdm2InfoRec.xconfigFlag) ?
				XCONFIG_GIVEN : XCONFIG_PROBED ,
			bdm2InfoRec.name,
			bdm2InfoRec.chipset);
		ErrorF("%s %s: videoram: %dk\n",
			OFLG_ISSET(XCONFIG_VIDEORAM,&bdm2InfoRec.xconfigFlag) ?
				XCONFIG_GIVEN : XCONFIG_PROBED ,
			bdm2InfoRec.name,
			bdm2InfoRec.videoRam);
	}
	/* this drivers's functions and consts are now bdm's functions */
	bdmEnterLeaveFunc   = bdmDrivers[i]->ChipEnterLeave;
	bdmInitFunc         = bdmDrivers[i]->ChipInit;
	bdmSaveFunc         = bdmDrivers[i]->ChipSave;
	bdmRestoreFunc      = bdmDrivers[i]->ChipRestore;
	bdmAdjustFunc       = bdmDrivers[i]->ChipAdjust;
	bdmSaveScreenFunc   = bdmDrivers[i]->ChipSaveScreen;
	bdmGetModeFunc      = bdmDrivers[i]->ChipGetMode;
	bdmSetReadFunc      = bdmDrivers[i]->ChipSetRead;
	bdmSetWriteFunc     = bdmDrivers[i]->ChipSetWrite;
	bdmSetReadWriteFunc = bdmDrivers[i]->ChipSetReadWrite;
	bdmReadBottom       = bdmDrivers[i]->ChipReadBottom;
	bdmReadTop          = bdmDrivers[i]->ChipReadTop;
	bdmWriteBottom      = bdmDrivers[i]->ChipWriteBottom;
	bdmWriteTop         = bdmDrivers[i]->ChipWriteTop;
	bdmMapBase          = bdmDrivers[i]->ChipMapBase;
	bdmMapSize          = bdmDrivers[i]->ChipMapSize;
	bdmSegmentSize      = bdmDrivers[i]->ChipSegmentSize;
	bdmSegmentShift     = bdmDrivers[i]->ChipSegmentShift;
	bdmSegmentMask      = bdmDrivers[i]->ChipSegmentMask;
	bdmUse2Banks        = bdmDrivers[i]->ChipUse2Banks;
	bdmScanLineWidth    = bdmDrivers[i]->ChipScanLineWidth;
	bdmOptionFlags      = bdmDrivers[i]->ChipOptionFlags;

	xf86VerifyOptions(&bdmOptionFlags, &bdm2InfoRec);

	/* Set up later (?) by x386InitViewport in common/x386Cursor.c */
	/* (X0,Y0) = ViewPort */
	bdm2InfoRec.frameX0 = 0;
	bdm2InfoRec.frameY0 = 0;
	bdm2InfoRec.frameX1 = 0;
	bdm2InfoRec.frameY1 = 0;

	/* Only one mode */
	bdm2InfoRec.modes = (DisplayModePtr)xalloc(sizeof(DisplayModeRec));
	/* display mode: real display size */
	bdm2InfoRec.modes->HDisplay = bdmDrivers[i]->ChipHDisplay;
	bdm2InfoRec.modes->VDisplay = bdmDrivers[i]->ChipVDisplay;
	bdm2InfoRec.modes->next = bdm2InfoRec.modes;
	bdm2InfoRec.modes->prev = bdm2InfoRec.modes;
	/* Virtual screen size bdm2InfoRec.virtualX, ...virtualY
	 * is set up by ChipProbe() */
	if (bdm2InfoRec.virtualX > bdm2InfoRec.modes->HDisplay ||
	    bdm2InfoRec.virtualY > bdm2InfoRec.modes->VDisplay)
		if (x386Verbose)
		    ErrorF("%s %s: Virtual resolution set to %dx%d\n",
			OFLG_ISSET(XCONFIG_VIRTUAL,&bdm2InfoRec.xconfigFlag) ?
			XCONFIG_GIVEN : XCONFIG_PROBED ,
			bdm2InfoRec.name,
			bdm2InfoRec.virtualX, bdm2InfoRec.virtualY);
        if (bdmUse2Banks) {
		ourbdm_mfbDoBitbltCopy = bdm_mfbDoBitbltCopy;
		ourbdm_mfbDoBitbltCopyInverted = bdm_mfbDoBitbltCopyInverted;
		bdmPushRead = bdmPushRead2Banks;
		bdmPopRead  = bdmPopRead2Banks;
		ErrorF("%s %s: Using 2 banks mode\n", XCONFIG_PROBED,
			bdm2InfoRec.name);
	} else {
		ourbdm_mfbDoBitbltCopy = oneBankbdm_mfbDoBitbltCopy;
		ourbdm_mfbDoBitbltCopyInverted = oneBankbdm_mfbDoBitbltCopyInverted;
		bdmPushRead = bdmPushRead1Bank;
		bdmPopRead  = bdmPopRead1Bank;
		ErrorF("%s %s: Using one bank mode\n", XCONFIG_PROBED,
			bdm2InfoRec.name);
	}
	/* exit after the first that succeeded */
	return TRUE;
      } /* if */
  } /* for */
  if (bdm2InfoRec.chipset)
      ErrorF("%s: '%s' is an invalid chipset\n", bdm2InfoRec.name,
	     bdm2InfoRec.chipset);
  return FALSE;
}

/*
 * bdmScreenInit --
 *      Attempt to find and initialize a BDM framebuffer
 *      Driver is already detected, driver specific values are known.
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
bdmScreenInit (index, pScreen, argc, argv)
    int            index;        /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
  int displayResolution = 92;    /* default to 92dpi */
  extern int monitorResolution;
  int mapSize;

  if (serverGeneration == 1) {
#if defined(__BSD__) && !defined(__bsdi__)
	/* Hack for mmap() problem on 386bsd */
	if (bdmMapSize < 0x18000)
		mapSize = 0x18000;
	else
#endif /* __BSD__ && !__bsdi__ */
	mapSize = bdmMapSize;
	bdmBase = xf86MapVidMem(index, VGA_REGION /* ?? */,
				(pointer)bdmMapBase, mapSize);
	bdmVirtBase = (pointer)BDMBASE;
	bdmReadBottom  = (void *)((unsigned int)bdmReadBottom
			 + (unsigned int)bdmBase);
	bdmReadTop     = (void *)((unsigned int)bdmReadTop
			 + (unsigned int)bdmBase);
	bdmWriteBottom = (void *)((unsigned int)bdmWriteBottom
			 + (unsigned int)bdmBase);
	bdmWriteTop    = (void *)((unsigned int)bdmWriteTop
			 + (unsigned int)bdmBase);

  }

/* Warning: bdmInit returns the video state (copy of it) that is to be set */
  if (!(bdmNewVideoState=(pointer)(bdmInitFunc)(bdm2InfoRec.modes)))
	FatalError("%s: hardware initialisation failed\n", bdm2InfoRec.name);
  if (!bdmOrigVideoState)
	bdmOrigVideoState=(pointer)(bdmSaveFunc)(bdmOrigVideoState);
  (bdmRestoreFunc)(bdmNewVideoState);
#ifndef DIRTY_STARTUP
  bdmSaveScreenFunc(NULL, FALSE); /* blank the screen */
#endif
  (bdmAdjustFunc)(bdm2InfoRec.frameX0, bdm2InfoRec.frameY0);
  
  /*
   * Take display resolution from the -dpi flag if specified
   */

  if (monitorResolution)
	displayResolution = monitorResolution;

  /*
   * Inititalize the dragon to mono display
   */
  if (!bdm_mfbScreenInit(pScreen,
			(pointer)bdmVirtBase,
			bdm2InfoRec.virtualX,
			bdm2InfoRec.virtualY,
			displayResolution, displayResolution,
			bdmScanLineWidth))
	return(FALSE);

  pScreen->blackPixel = 0;
  pScreen->whitePixel = 1;

  pScreen->CloseScreen = bdmCloseScreen;
  pScreen->SaveScreen = bdmSaveScreenFunc;
  
  miDCInitialize (pScreen, &x386PointerScreenFuncs);

  if (!bdm_mfbCreateDefColormap(pScreen))
	return(FALSE);

#ifndef DIRTY_STARTUP
  /* Fill the screen with black */
  if (serverGeneration == 1)
  {
    BoxRec  pixBox;
    PixmapPtr   pspix;

    pixBox.x1 = 0; pixBox.x2 = pScreen->width;
    pixBox.y1 = 0; pixBox.y2 = pScreen->height;
    pspix = (PixmapPtr)pScreen->devPrivate;
    bdm_mfbSolidBlackArea(&pspix->drawable, 1, &pixBox, GXcopy, NULL);
  }
  bdmSaveScreenFunc(NULL, TRUE); /* unblank the screen */
#endif /* ! DIRTY_STARTUP */

  savepScreen = pScreen;

  return(TRUE);
}

static void saveDummy() {}

/*
 * bdmEnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
bdmEnterLeaveVT(enter, screen_idx)
	Bool enter;
	int screen_idx;
{
  pointer p;
  BoxRec  pixBox;	/* Box: Region to copy ? */
  RegionRec pixReg;	/* */
  DDXPointRec pixPt;	/* Point: upper left corner */
  PixmapPtr   pspix;	/* Pointer to the pixmap of the saved screen */
  ScreenPtr   pScreen = savepScreen;	/* This is the 'old' Screen:
				/* real screen on leave, dummy on enter */

  /* Set up pointer to the saved pixmap (pspix) only if not resetting
						and not exiting */
  /* bdm2 should treat this differently:
   * As this screen might be a secondary screen, it is better to clear
   * it on exit.
   * This makes VT switching on a primary mono card look better (& slower)
   */
  if (!x386Resetting /* && !x386Exiting */ ) {
	pixBox.x1 = 0; pixBox.x2 = pScreen->width;
	pixBox.y1 = 0; pixBox.y2 = pScreen->height;
	pixPt.x = 0; pixPt.y = 0;
	(pScreen->RegionInit)(&pixReg, &pixBox, 1);
	pspix = (PixmapPtr)pScreen->devPrivate;
  }

  if (enter)
    {
	bdmInitFunc	= saveInitFunc;
	bdmSaveFunc	= saveSaveFunc;
	bdmRestoreFunc	= saveRestoreFunc;
	bdmAdjustFunc	= saveAdjustFunc;
	bdmSaveScreenFunc = saveSaveScreenFunc;
	bdmSetReadFunc	= saveSetReadFunc;
	bdmSetWriteFunc	= saveSetWriteFunc;
	bdmSetReadWriteFunc = saveSetReadWriteFunc;

	xf86MapDisplay(screen_idx, VGA_REGION);

	(bdmEnterLeaveFunc)(ENTER);
	bdmOrigVideoState=(pointer)(bdmSaveFunc)(bdmOrigVideoState);
	(bdmRestoreFunc)(bdmNewVideoState);

      /*
       * point pspix back to bdmVirtBase, and copy the dummy buffer to the
       * real screen.
       */
      if (!x386Resetting)
        if ((pointer)pspix->devPrivate.ptr != (pointer)bdmVirtBase && ppix)
        {
	  pspix->devPrivate.ptr = (pointer)bdmVirtBase;
	/* mfbbitblt.c:mfbDoBitblt (pSrc, pDst, alu, prgnDst, pptSrc) */
	  bdm_mfbDoBitblt(&ppix->drawable, &pspix->drawable, GXcopy,
			  &pixReg, &pixPt);
        }
      if (ppix) {
	(pScreen->DestroyPixmap)(ppix);
	ppix = NULL;
      }
    }
  else /* enter */
    { /* -> leaving */
      /* Make sure that another driver hasn't disabeled IO */    
      xf86MapDisplay(screen_idx, VGA_REGION);

      (bdmEnterLeaveFunc)(ENTER);

      /*
       * Create a dummy pixmap to write to while VT is switched out.
       * Copy the screen to that pixmap
       *
       * This pixmap has to be ScanLineWidth pixels wide !!
       */
      {
        ppix = (pScreen->CreatePixmap)(pScreen, /* pScreen->width */
		bdmScanLineWidth, pScreen->height, pScreen->rootDepth);
        if (ppix)
        {
	  if (!x386Exiting)
	      bdm_mfbDoBitblt(&pspix->drawable, &ppix->drawable, GXcopy, &pixReg,
		&pixPt, 0xFF);
	  bdm_mfbSolidBlackArea(&pspix->drawable, 1, &pixBox, GXcopy, NULL);
	  pspix->devPrivate.ptr = ppix->devPrivate.ptr;
        }
      }
      bdmNewVideoState=(pointer)(bdmSaveFunc)(bdmNewVideoState);
      /*
       * We come here in many cases, but one is special: When the server aborts
       * abnormaly. Therefore there MUST be a check whether bdmOrigVideoState
       * is valid or not.
       */
      if (bdmOrigVideoState)
	(bdmRestoreFunc)(bdmOrigVideoState);

      (bdmEnterLeaveFunc)(LEAVE);
      xf86UnMapDisplay(screen_idx, VGA_REGION);
      saveInitFunc         = bdmInitFunc;
      saveSaveFunc         = bdmSaveFunc;
      saveRestoreFunc      = bdmRestoreFunc;
      saveAdjustFunc       = bdmAdjustFunc;
      saveSaveScreenFunc   = bdmSaveScreenFunc;
      saveSetReadFunc      = bdmSetReadFunc;
      saveSetWriteFunc     = bdmSetWriteFunc;
      saveSetReadWriteFunc = bdmSetReadWriteFunc;

	bdmInitFunc		= (void *(*)()) saveDummy;
	bdmSaveFunc		= (void *(*)()) saveDummy;
	bdmRestoreFunc		= saveDummy;
	bdmAdjustFunc		= saveDummy;
	bdmSaveScreenFunc	= (Bool (*)()) saveDummy;
	bdmSetReadFunc		= saveDummy;
	bdmSetWriteFunc		= saveDummy;
	bdmSetReadWriteFunc	= saveDummy;
    }
}

/*
 * bdmCloseScreen --
 *      called to ensure video is enabled when server exits.
 */

Bool
bdmCloseScreen(screen_idx)
{
  /*
   * Hmm... The server may shut down even if it is not running on the
   * current vt. Let's catch this case here.
   */
  x386Exiting = TRUE;
  if (x386VTSema)
    bdmEnterLeaveVT(LEAVE, screen_idx);
  else if (ppix) {
    /*
     * 7-Jan-94 CEG: The server is not running on the current vt.
     * Free the screen snapshot taken when the server vt was left.
     */
    (savepScreen->DestroyPixmap)(ppix);
    ppix = NULL;
  }
  return(TRUE);
}

/*
 * bdmAdjustFrame --
 *      Set a new viewport
 */
void
bdmAdjustFrame(x, y)
int x, y;
{
/* Strange: gets called with
 * x > bdm2InfoRec.virtualX - bdm2InfoRec.modes->HDisplay
 * Shouldn't be. Can mess up the lowlevel driver (does no checking).
 * - Seems not to exceed maxshift+1 -
 */
int maxshift;
maxshift = bdm2InfoRec.virtualX - bdm2InfoRec.modes->HDisplay;
if (x <= maxshift)
	(bdmAdjustFunc)(x, y);
else {
	(bdmAdjustFunc)(maxshift, y);
#if 1
	ErrorF("%s: Warning: bdmAdjustFrame() called with x=%d\n",
	    bdm2InfoRec.name, x);
#endif
}
}

/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD,
 * AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD, OR
 * TIAGO GONS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Scott Laird (lair@kimbark.uchicago.edu)
 * and Tiago Gons (tiago@comosjn.hobby.nl)
 *
 * Header: /proj/X11/mit/server/ddx/x386/vga/RCS/vga.c,v 1.2 1991/06/27 00:02:49 root Exp
 */

/* $XFree86: mit/server/ddx/x386/accel/mach8/mach8.c,v 2.34 1994/03/03 12:43:39 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "mipointer.h"
#include "cursorstr.h"

#include "compiler.h"

#include "x386.h"
#include "x386Priv.h"
#include "x386Procs.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "mach8.h"
#include "regmach8.h"
#include "cfb.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

extern int mach8MaxClock;
extern Bool x386Exiting, x386Resetting, x386ProbeFailed, x386Verbose;
extern void NoopDDA();
extern Bool mfbRegisterCopyPlaneProc();
extern Bool miDCInitialize();
extern void SetTimeSinceLastInputEvent();

ScrnInfoRec mach8InfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    mach8Probe,      	/* Bool (* Probe)() */
    mach8Initialize,	/* Bool (* Init)() */
    mach8EnterLeaveVT,	/* void (* EnterLeaveVT)() */
    NoopDDA,		/* void (* EnterLeaveMonitor)() */
    NoopDDA,		/* void (* EnterLeaveCursor)() */
    mach8Adjust,	/* void (* AdjustFrame)() */
    mach8SwitchMode,	/* Bool (* SwitchMode)() */
    mach8PrintIdent,	/* void (* PrintIdent)() */
    8,			/* int depth */
    8,			/* int bitsPerPixel */
    PseudoColor,       	/* int defaultVisual */
    -1, -1,		/* int virtualX,virtualY */
    -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
    {0, },              /* OFlagSet options */
    {0, },              /* OFlagSet clockOptions */
    {0, },              /* OFlagSet xconfigFlag */
    NULL,	       	/* char *chipset */
    0,			/* int clocks */
    {0, },		/* int clock[MAXCLOCKS] */
    0,			/* int maxClock */
    0,			/* int videoRam */
    0,                  /* int BIOSbase, 1.3 new */
    0,			/* unsigned long MemBase, unused for this driver */
    240, 180,		/* int width, height */
    0,                  /* unsigned long  speedup */
    NULL,	       	/* DisplayModePtr modes */
    NULL,               /* char *clockprog */
    -1,                 /* int textclock, 1.3 new */
    FALSE,              /* Bool bankedMono */
    "Mach8",            /* char *name */
    {0, },		/* RgbRec blackColour */
    {0, },		/* RgbRec whiteColour */
    mach8ValidTokens,	/* int *validTokens */
    MACH8_PATCHLEVEL,	/* char *patchlevel */
};

short mach8alu[16] = {
    MIX_0,
    MIX_AND,
    MIX_SRC_AND_NOT_DST,
    MIX_SRC,
    MIX_NOT_SRC_AND_DST,
    MIX_DST,
    MIX_XOR,
    MIX_OR,
    MIX_NOR,
    MIX_XNOR,
    MIX_NOT_DST,
    MIX_SRC_OR_NOT_DST,
    MIX_NOT_SRC,
    MIX_NOT_SRC_OR_DST,
    MIX_NAND,
    MIX_1
};

extern miPointerScreenFuncRec x386PointerScreenFuncs;

void (*mach8ImageWriteFunc)();
void (*mach8ImageReadFunc)();
void (*mach8RealImageFillFunc)();
void (*mach8ImageStippleFunc)();

static Bool mach8DramUsed = FALSE;
static int AlreadyInited = FALSE;

Bool mach8clkprobedif4fix = FALSE;

static struct mach8vmodedef  mach8ScreenMode;

static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

static unsigned mach8_IOPorts[] = {
	/* 8514 Registers */
	DAC_MASK, DAC_R_INDEX, DAC_W_INDEX, DAC_DATA, DISP_STAT,
	H_TOTAL, H_DISP, H_SYNC_STRT, H_SYNC_WID,
	V_TOTAL, V_DISP, V_SYNC_STRT, V_SYNC_WID,
	DISP_CNTL, ADVFUNC_CNTL, SUBSYS_CNTL, ROM_PAGE_SEL,
	CUR_Y, CUR_X, DESTY_AXSTP, DESTX_DIASTP, 
	ERR_TERM, MAJ_AXIS_PCNT, GP_STAT, CMD, SHORT_STROKE,
	BKGD_COLOR, FRGD_COLOR, WRT_MASK, RD_MASK,
	COLOR_CMP, BKGD_MIX, FRGD_MIX, MULTIFUNC_CNTL, PIX_TRANS,
	/* Mach8 Registers */
	CLOCK_SEL, CONFIG_STATUS_1, CRT_OFFSET_HI, CRT_OFFSET_LO, CRT_PITCH,
	DP_CONFIG, EXT_FIFO_STATUS, GE_OFFSET_HI, GE_OFFSET_LO, GE_PITCH,
	LINEDRAW_INDEX, LINEDRAW_OPT, LINEDRAW, ROM_ADDR_1, SHADOW_CTL,
	SHADOW_SET, READ_SRC_X,
};
static int Num_mach8_IOPorts = (sizeof(mach8_IOPorts)/
				sizeof(mach8_IOPorts[0]));

/*
 * mach8Probe --
 *     probe and initialize the hardware driver
 */
Bool
mach8Probe()
{
    int            j, memavail, rounding;
    short          temp;
    DisplayModePtr pMode, pEnd, pmaxX = NULL, pmaxY = NULL;
    int            maxX, maxY;
    OFlagSet       validOptions;

    mach8InfoRec.maxClock = mach8MaxClock;

    xf86ClearIOPortList(mach8InfoRec.scrnIndex);
    xf86AddIOPorts(mach8InfoRec.scrnIndex, Num_mach8_IOPorts, mach8_IOPorts);

    if (mach8InfoRec.chipset) {
	if (StrCaseCmp(mach8InfoRec.chipset, "mach8")) {
	    ErrorF("Chipset specified in Xconfig is not \"mach8\" (%s)!\n",
		   mach8InfoRec.chipset);
	    return(FALSE);
	}
	xf86EnableIOPorts(mach8InfoRec.scrnIndex);
    }
    else
    {
	xf86EnableIOPorts(mach8InfoRec.scrnIndex);

	/* Reset the 8514/A, and disable all interrupts. */
	outw(SUBSYS_CNTL, GPCTRL_RESET | CHPTEST_NORMAL);
	outw(SUBSYS_CNTL, GPCTRL_ENAB | CHPTEST_NORMAL);

	/* Check to see if an 8514/A is actually installed by writing to
	 * the ERR_TERM register, and reading back.  The 0x5a5a value is
	 * entirely arbitrary.
	 */
	outw(ERR_TERM, 0x5a5a);
	ProbeWaitIdleEmpty();
	if (inw(ERR_TERM) != 0x5a5a) {
	    ErrorF("%s: No 8514/A registers detected!\n", mach8InfoRec.name);
	    xf86DisableIOPorts(mach8InfoRec.scrnIndex);
	    return(FALSE);
	}
	/* 6 mar 93 TCG : let's make certain */
	outw(ERR_TERM, 0x2525);
	ProbeWaitIdleEmpty();
	if (inw(ERR_TERM) != 0x2525) {
	    ErrorF("%s: No 8514/A registers detected!\n", mach8InfoRec.name);
	    xf86DisableIOPorts(mach8InfoRec.scrnIndex);
	    return(FALSE);
	}

	temp = inw(ROM_ADDR_1);
	outw(ROM_ADDR_1, 0x5555);
	ProbeWaitIdleEmpty();
	if (inw(ROM_ADDR_1) != 0x5555) {
	    ErrorF("%s: No ATI accelerator detected\n", mach8InfoRec.name);
	    xf86DisableIOPorts(mach8InfoRec.scrnIndex);
	    return(FALSE);
	}
	outw(ROM_ADDR_1, 0x2a2a);
	ProbeWaitIdleEmpty();
	if (inw(ROM_ADDR_1) != 0x2a2a) {
	    ErrorF("%s: No ATI accelerator detected\n", mach8InfoRec.name);
	    xf86DisableIOPorts(mach8InfoRec.scrnIndex);
	    return(FALSE);
	}
	outw(ROM_ADDR_1, temp);

	outw(DESTX_DIASTP, 0xaaaa);
	ProbeWaitIdleEmpty();
	if ((inw(READ_SRC_X) != 0xaaaa) && x386Verbose) {
	    outw(DESTX_DIASTP, 0x5555);
	    ProbeWaitIdleEmpty();
	    if (inw(READ_SRC_X) == 0x0555) {
		ErrorF("%s %s: Mach-32 detected, used as a Mach-8\n",
			XCONFIG_PROBED, mach8InfoRec.name);
	    }
	    else
		ErrorF("%s %s: Mach-8 detected\n", XCONFIG_PROBED,
			mach8InfoRec.name);
	}
    }

    OFLG_ZERO(&validOptions);
    OFLG_SET(OPTION_CSYNC, &validOptions);
    xf86VerifyOptions(&validOptions, &mach8InfoRec);

    if (!mach8InfoRec.clocks)
    {
	outw(DISP_CNTL, DISPEN_DISAB /*| INTERLACE*/ | MEMCFG_4 | ODDBNKENAB);
	/* 13-jun-93 TCG : set up dummy video mode */
	outw(SHADOW_SET, 1);
	outw(SHADOW_CTL, 0);
	outw(SHADOW_SET, 2);
	outw(SHADOW_CTL, 0);
	outw(SHADOW_SET, 0);
	outw(ADVFUNC_CNTL, DISABPASSTHRU);

	/* vt: 480 488 +31 528 hz: 640 656 +248 920 */
	outw(V_TOTAL, 0x420);
	outw(V_DISP, 0x3c0);
	outw(V_SYNC_STRT, 0x3d0);
	outw(V_SYNC_WID, 0x1f);
	outw(H_TOTAL, 0x72);
	outw(H_DISP, 0x4f);
	outw(H_SYNC_STRT, 0x51);
	outw(H_SYNC_WID, 0x1f);

	outw(DAC_MASK, 0x00);
	outw(DISP_CNTL, DISPEN_ENAB | MEMCFG_4 | ODDBNKENAB);

	/* 2-oct-93 TCG : detect clocks with dif4 fix */
	mach8clkprobedif4fix = TRUE;
	xf86GetClocks(16, mach8ClockSelect, NoopDDA, NoopDDA,
		      DISP_STAT, 2, 7, 44900, &mach8InfoRec);
	outw(CLOCK_SEL, 0); /* reset pass-through */
	mach8clkprobedif4fix = FALSE;

	outw(SHADOW_SET, 1);
	outw(SHADOW_CTL, 0x3f);
	outw(SHADOW_SET, 2);
	outw(SHADOW_CTL, 0x3f);
	outw(SHADOW_SET, 0);

	outw(DAC_MASK, 0xff);

	for (j = 0; j < 16; j++)
	    mach8InfoRec.clock[j + 16] = mach8InfoRec.clock[j] / 2;

	mach8InfoRec.clocks = 32;
    }

    temp = inw(CONFIG_STATUS_1);
    if (!mach8InfoRec.videoRam) {
        switch((temp & 0x60) >> 5)
        {
            case 0:
                mach8InfoRec.videoRam = 512;
                break;
            case 1:
                mach8InfoRec.videoRam = 1024;
                break;
            default: /* 2 & 3 reserved in mach-8, used in mach-32 ? */
                mach8InfoRec.videoRam = 1024;
                break;
        }
    }

    mach8InfoRec.chipset = "mach8";
    x386ProbeFailed = FALSE;
    mach8DramUsed = (temp & 0x10) != 0;
    if (x386Verbose)
    {
        ErrorF("%s %s: (mem: %dk %cRAM numclocks: %d)",
               OFLG_ISSET(XCONFIG_VIDEORAM,&mach8InfoRec.xconfigFlag) ?
               XCONFIG_GIVEN : XCONFIG_PROBED,
               mach8InfoRec.name,
               mach8InfoRec.videoRam,
               mach8DramUsed ? 'D' : 'V',
               mach8InfoRec.clocks);

        for (j=0; j < mach8InfoRec.clocks; j++)
        {
            if ((j % 8) == 0)
                ErrorF("\n%s %s: clocks:", 
                   OFLG_ISSET(XCONFIG_CLOCKS,&mach8InfoRec.xconfigFlag) ?
                   XCONFIG_GIVEN : XCONFIG_PROBED,
                   mach8InfoRec.name);
            ErrorF(" %6.2f", (double)mach8InfoRec.clock[j]/1000.0);
        }
        ErrorF("\n");
    }

    memavail = mach8InfoRec.videoRam*1024;
    if (mach8InfoRec.virtualX > 0 &&
        mach8InfoRec.virtualX * mach8InfoRec.virtualY > memavail)
    {
        ErrorF("%s: Too little memory for virtual resolution %d %d\n",
               mach8InfoRec.name, mach8InfoRec.virtualX,
               mach8InfoRec.virtualY);
        return(FALSE);
    }

    maxX = maxY = -1;
    pMode = pEnd = mach8InfoRec.modes;
    do {
        x386LookupMode(pMode, &mach8InfoRec);
  
        if (pMode->HDisplay * pMode->VDisplay > memavail)
        {
            ErrorF("%s: Too little memory for mode %s\n", mach8InfoRec.name,
                   pMode->name);
            return(FALSE);
        }
  
        if (pMode->HDisplay > maxX)
        {
            maxX = pMode->HDisplay;
            pmaxX = pMode;
        }
        if (pMode->VDisplay > maxY)
        {
            maxY = pMode->VDisplay;
            pmaxY = pMode;
        }
        pMode = pMode->next;
    } while (pMode != pEnd);
  
    mach8InfoRec.virtualX = max(maxX, mach8InfoRec.virtualX);
    mach8InfoRec.virtualY = max(maxY, mach8InfoRec.virtualY);
  
    rounding = 8;
  
    if (mach8InfoRec.virtualX % rounding)
    {
        mach8InfoRec.virtualX -= mach8InfoRec.virtualX % rounding;
	ErrorF("%s %s: Virtual width rounded down to a multiple of %d (%d)\n",
	       XCONFIG_PROBED, mach8InfoRec.name, rounding,
	       mach8InfoRec.virtualX);
        if (mach8InfoRec.virtualX < maxX)
        {
            ErrorF(
		"%s: Rounded down virtual width (%d) is too small for mode %s",
                   mach8InfoRec.name, mach8InfoRec.virtualX, pmaxX->name);
            return(FALSE);
        }
        ErrorF("%s: Virtual width rounded down to a multiple of %d (%d)\n",
               mach8InfoRec.name, rounding, mach8InfoRec.virtualX);
    }

    if (mach8InfoRec.virtualX > 1024)
    {
        ErrorF("%s: Virtual width must be no greater than 1024\n");
        return(FALSE);
    }
    if ( mach8InfoRec.virtualX * mach8InfoRec.virtualY > memavail)
    {
        if (mach8InfoRec.virtualX != maxX || mach8InfoRec.virtualY != maxY)
            ErrorF(
	      "%s: Too little memory to accomodate virtual size and mode %s\n",
                   mach8InfoRec.name,
                   (mach8InfoRec.virtualX == maxX) ? pmaxX->name : pmaxY->name);
        else
            ErrorF("%s: Too little memory to accomodate modes %s and %s\n",
                   mach8InfoRec.name, pmaxX->name, pmaxY->name);
        return(FALSE);
    }
    if (x386Verbose)
        ErrorF("%s %s: Virtual resolution set to %dx%d\n",
	       OFLG_ISSET(XCONFIG_VIRTUAL,&mach8InfoRec.xconfigFlag) ?
		  XCONFIG_GIVEN : XCONFIG_PROBED, mach8InfoRec.name,
               mach8InfoRec.virtualX, mach8InfoRec.virtualY);

    return(TRUE);
}

void
mach8PrintIdent()
{
    ErrorF("  %s: accelerated server for ", mach8InfoRec.name);
    ErrorF("ATI Mach-8 graphics adaptors (Patchlevel %s)\n", 
	   mach8InfoRec.patchLevel);
}

/*
 * mach8Initialize --
 *      Attempt to find and initialize a VGA framebuffer
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
mach8Initialize (scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
    int displayResolution = 75;  /* default to 75dpi */
    extern int monitorResolution;

    /*
     *  Convert from XFree mode definitions to Mach8 register values.
     */
    mach8calcvmode(&mach8ScreenMode,mach8InfoRec.modes);

    mach8Init(&mach8ScreenMode);
    mach8InitEnvironment();
    AlreadyInited = TRUE;

    mach8CacheInit();
    mach8FontCache8Init();
    if( mach8DramUsed ) {
	mach8ImageWriteFunc = mach8ImageWriteDram;
	mach8ImageReadFunc = mach8ImageReadDram;
	mach8RealImageFillFunc = mach8RealImageFillDram;
	mach8ImageStippleFunc = mach8ImageStippleDram;
    }
    else {
	mach8ImageWriteFunc = mach8ImageWrite;
	mach8ImageReadFunc = mach8ImageRead;
	mach8RealImageFillFunc = mach8RealImageFill;
	mach8ImageStippleFunc = mach8ImageStipple;
    }
    mach8ImageInit();

    /*
     * Take display resolution from the -dpi flag if specified
     */

    if (monitorResolution)
	displayResolution = monitorResolution;

    if (!mach8ScreenInit(pScreen,
			   (pointer) 0,
			   mach8InfoRec.virtualX, mach8InfoRec.virtualY,
			   displayResolution, displayResolution,
			   mach8InfoRec.virtualX))
	return(FALSE);

    pScreen->CloseScreen = mach8CloseScreen;
    pScreen->SaveScreen = mach8SaveScreen;
    pScreen->InstallColormap = mach8InstallColormap;
    pScreen->UninstallColormap = mach8UninstallColormap;
    pScreen->ListInstalledColormaps = mach8ListInstalledColormaps;
    pScreen->StoreColors = mach8StoreColors;

    mfbRegisterCopyPlaneProc (pScreen, miCopyPlane);
  
    miDCInitialize (pScreen, &x386PointerScreenFuncs);

    savepScreen = pScreen;

    return (cfbCreateDefColormap(pScreen));
}

/*
 * mach8EnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
mach8EnterLeaveVT(enter, screen_idx)
     Bool enter;
     int screen_idx;
{
  PixmapPtr   pspix;
  ScreenPtr   pScreen = savepScreen;

    if (!x386Resetting && !x386Exiting)
    {
        pspix = (PixmapPtr)pScreen->devPrivate;
    }

    if (enter) {
	xf86EnableIOPorts(mach8InfoRec.scrnIndex);
	mach8Init(&mach8ScreenMode);
	mach8InitEnvironment();
        mach8RestoreDACvalues();
        mach8CacheInit();
        mach8FontCache8Init();
	AlreadyInited = TRUE;

        /* 11-jun-93 TCG */
        if (!x386Resetting)
           if ((pointer)pspix->devPrivate.ptr != NULL && ppix)
           {
              pspix->devPrivate.ptr = NULL;
              (mach8ImageWriteFunc)(0, 0, pScreen->width, pScreen->height,
				    ppix->devPrivate.ptr,
				    PixmapBytePad(pScreen->width,
						  pScreen->rootDepth),
				    0, 0, mach8alu[GXcopy], 0xFF);

           }
        if (ppix) {
           (pScreen->DestroyPixmap)(ppix);
	   ppix = NULL;
	}
    } else {
        if (!x386Exiting)
        {
           ppix = (pScreen->CreatePixmap)(pScreen, pScreen->width,
                                          pScreen->height, pScreen->rootDepth);
           if (ppix)
           {
              (mach8ImageReadFunc)(0, 0, pScreen->width, pScreen->height,
				   ppix->devPrivate.ptr,
				   PixmapBytePad(pScreen->width,
						 pScreen->rootDepth),
				   0, 0, 0xff);
              pspix->devPrivate.ptr = ppix->devPrivate.ptr;
           }
        }
        /* 19 jun 93 TCG : relock shadow set */
        outw(SHADOW_SET, 1);
        outw(SHADOW_CTL, 0x3f);
        outw(SHADOW_SET, 2);
        outw(SHADOW_CTL, 0x3f);
        outw(SHADOW_SET, 0);

	/*
	 * We come here in many cases, but one is special: When the server
	 * aborts abnormaly. Therefore there MUST be a check whether
	 * vgaOrigVideoState is valid or not.
	 */
	/* The above refers to the video state not being set up until the
	   Initialize function (above) is called.  If it has not been called
	   then "vgaOrigVideoState" will not be initialized and thus cannot be
	   restored!  A similar thing must be done in the 8514 and Mach-8.
	   */
	if (AlreadyInited) {
	    mach8CleanUp();
	}
    }
}

/*
 * mach8CloseScreen --
 *      called to ensure video is enabled when server exits.
 */

Bool
mach8CloseScreen(screen_idx)
     int screen_idx;
{
    /*
     * Hmm... The server may shut down even if it is not running on the
     * current vt. Let's catch this case here.
     */
    x386Exiting = TRUE;
    if (x386VTSema) 
	mach8EnterLeaveVT(LEAVE, screen_idx);
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
 * mach8SaveScreen --
 *      blank the screen.
 */

Bool
mach8SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
    if (on)
	SetTimeSinceLastInputEvent();
    if (x386VTSema) {
	if (on) {
	    mach8RestoreColor0(pScreen);

	    outw(DAC_MASK, 0xff);
	} else {
	    outb(DAC_W_INDEX, 0);
	    outb(DAC_DATA, 0);
	    outb(DAC_DATA, 0);
	    outb(DAC_DATA, 0);

	    outw(DAC_MASK, 0x00);
	}
    }
    return(TRUE);
}

/*
 * mach8Adjust --
 *      Pans the screen for virtual displays.
 */     
void
mach8Adjust(x,y)
     int x,y;
{
  int offset;

  /*
   * The Mach8 has two sets of registers to define the start of video
   *  memory.  One for the CRT controller, and on for the Graphics Engine.
   *  We adjust the CRT offset to pan the display.
   */

    if( mach8InfoRec.videoRam > 512 )
	offset = (2 + x + 1024*y) >> 2;
    else
	offset = (2 + x + mach8InfoRec.virtualX*y) >> 2;
  
    outw(CRT_OFFSET_HI, (offset >> 16) & 0xF);
    outw(CRT_OFFSET_LO, offset & 0xFFFF);
}

/*
 * mach8SwitchMode --
 *     Set a new display mode
 */
Bool
mach8SwitchMode(mode)
     DisplayModePtr mode;
{
    struct mach8vmodedef newmode;

    mach8calcvmode(&newmode,mach8InfoRec.modes);
    mach8Init(&newmode);
    mach8ScreenMode = newmode;

    return(TRUE);
}

/*
 * mach8ClockSelect
 */
Bool
mach8ClockSelect(no)
     int no;
{
  switch(no)
  {
    case CLK_REG_SAVE:
      break;
    case CLK_REG_RESTORE:
      break;
    default:
      if (mach8clkprobedif4fix)
         outw(CLOCK_SEL, (no << 2) | 0xac1);
      else
         outw(CLOCK_SEL, (no << 2) | 0xa01);
  }
  return(TRUE);
}

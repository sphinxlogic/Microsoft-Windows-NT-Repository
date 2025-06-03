/*
 * BDM2: Banked dumb monochrome driver
 * Pascal Haible 10/93, haible@izfm.uni-stuttgart.de
 *
 * bdm2/driver/visa/visadriv.c
 *
 * Parts derived from:
 * hga2/*
 * Author:  Davor Matic, dmatic@athena.mit.edu
 * and
 * vga256/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * see bdm2/COPYRIGHT for copyright and disclaimers.
 */

/* Thanks to Christian (cm@htu.tuwien.ac.at) who did the first driver */

/* $XFree86: mit/server/ddx/x386/bdm2/drivers/visa/visadriv.c,v 2.0 1993/12/01 12:37:41 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"

#include "x386.h"
#include "x386Priv.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "bdm.h"
#include "visaHW.h"

typedef struct {
  unsigned char conf;	/* write only conf register at port 0x3BF */
  unsigned char mode;	/* write only mode register at port 0x3B8 */
  unsigned char tbl[16];/* graph. params */
  } visaRec, *visaPtr;

visaRec visaRegsGraf1280x1024 = {
0x03,	/* conf */
0x0A,	/* mode */
{ 0x35, 0x50, 0x2E, 0x07, 0x5B, 0x02, 0x57, 0x57,
  0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/*
 * Since the table of 6845 registers is write only, we need to keep
 * a local copy of the state here.  The initial state is assumed to
 * be 160x55 text mode.
 */
visaRec visaRegsActual = {
0x00,
0x00,
{ 0x61, 0xA0, 0x52, 0x0F, 0x19, 0x06, 0x19, 0x19,
  0x02, 0x0D, 0x0B, 0x0C, 0x00, 0x00, 0x00, 0x00 }
};

/*
 * Define the VISA I/O Ports
 */
unsigned VISA_IOPorts[] = {
VISA_INDEX, VISA_DATA, VISA_MODE, VISA_STATUS, VISA_CONFIG /* = VISA_BANK */ };
int Num_VISA_IOPorts = (sizeof(VISA_IOPorts)/sizeof(VISA_IOPorts[0]));

volatile int visa_delay_dummy=0;
#define VISA_DELAY	{ visa_delay_dummy++; }

char *	VISAIdent();
Bool	VISAProbe();
void	VISAEnterLeave();
void *	VISAInit();
void *	VISASave();
void	VISARestore();
void	VISAAdjust();
Bool	VISASaveScreen();
void	VISAGetMode();

/* Assembler functions in visabank.s
 * - to be called by assembler functions only! */
extern void VISASetRead();
extern void VISASetWrite();
extern void VISASetReadWrite();

#if 0
/* From bdm.h -- see there, this here might not be up to date */
/*
 * structure for accessing the video chip`s functions
 *
 * We are doing a total encapsulation of the driver's functions.
 * Banking (bdmSetReadWrite(p) etc.) is done in bdmBank.c
 *   using the chip's function pointed to by
 *   bmpSetReadWriteFunc(bank) etc.
 */
typedef struct {
  char * (* ChipIdent)();
  Bool (* ChipProbe)();
  void (* ChipEnterLeave)();
  void * (* ChipInit)();
  void * (* ChipSave)();
  void (* ChipRestore)();
  void (* ChipAdjust)();
  Bool (* ChipSaveScreen)();
  void (* ChipGetMode)();
  /* These are the chip's banking functions:		*/
  /* they do the real switching to the desired bank	*/
  /* they 'become' bdmSetReadFunc() etc.		*/
  void (* ChipSetRead)();
  void (* ChipSetWrite)();
  void (* ChipSetReadWrite)();
  /* Bottom and top of the banking window (rel. to ChipMapBase)	*/
  /* Note: Top = highest accessable byte + 1			*/
  void *ChipReadBottom;
  void *ChipReadTop;
  void *ChipWriteBottom;
  void *ChipWriteTop;
  /* Memory to map      */
  int ChipMapBase;
  int ChipMapSize;      /* replaces MEMTOMAP */
  int ChipSegmentSize;
  int ChipSegmentShift;
  int ChipSegmentMask;
  Bool ChipUse2Banks;
  /* Display size is given by the driver */
  int ChipHDisplay;
  int ChipVDisplay;
  /* In case Scan Line in mfb is longer than HDisplay */
  int ChipScanLineWidth;
  OFlagSet ChipOptionFlags;
} bdmVideoChipRec, *bdmVideoChipPtr;
#endif /* 0 */

bdmVideoChipRec VISA = {
  /* Functions */
  VISAIdent,
  VISAProbe,
  VISAEnterLeave,
  VISAInit,
  VISASave,
  VISARestore,
  VISAAdjust,
  VISASaveScreen,
  NoopDDA,			/* VISAGetMode */
  VISASetRead,
  VISASetWrite,
  VISASetReadWrite,
  (void *)VISA_BANK1_BOTTOM,	/* ReadBottom */
  (void *)VISA_BANK1_TOP,	/* ReadTop */
  (void *)VISA_BANK2_BOTTOM,	/* WriteBottom */
  (void *)VISA_BANK2_TOP,	/* WriteTop */
  VISA_MAP_BASE,		/* MapBase */
  VISA_MAP_SIZE,		/* MapSize */
  VISA_SEGMENT_SIZE,		/* SegmentSize */
  VISA_SEGMENT_SHIFT,		/* SegmentShift */
  VISA_SEGMENT_MASK,		/* SegmentMask */
  FALSE,			/* Use2Banks */
  VISA_HDISPLAY,		/* HDisplay */
  VISA_VDISPLAY,		/* VDisplay */
  VISA_SCAN_LINE_WIDTH,	/* ScanLineWidth */
  {0,},
};

/*
 * VISAIdent
 */

char *
VISAIdent(n)
	int n;
{
static char *chipsets[] = {"visa6845"};
if (n >= sizeof(chipsets) / sizeof(char *))
	return(NULL);
else return(chipsets[n]);
}

/*
 * VISAProbe --
 *      check whether an VISA based board is installed
 */

Bool
VISAProbe()
{
  /*
   * Set up I/O ports to be used by this card
   */
  xf86ClearIOPortList(bdm2InfoRec.scrnIndex);
  xf86AddIOPorts(bdm2InfoRec.scrnIndex, Num_VISA_IOPorts, VISA_IOPorts);

  if (bdm2InfoRec.chipset) {
	/* Chipset preset */
	if (strcmp(bdm2InfoRec.chipset, VISAIdent(0)))
		/* desired chipset != this one */
		return (FALSE);
	else {
		VISAEnterLeave(ENTER);
		/* go on with videoram etc. below */
	}
  }
  else {
	/* do the real probing */
/***** This currently detects any herc board *****/
#define DSP_VSYNC_MASK  0x80
#define DSP_ID_MASK  0x70
      unsigned char dsp, dsp_old;
      int i, cnt;

      VISAEnterLeave(ENTER);
      /*
       * Checks if there is a VISA 6845 based bard in the system.
       * The following loop tries to see if the Hercules display
       * status port register is counting vertical syncs (50Hz).
       */
      cnt = 0;
      dsp_old = inb(0x3BA) & DSP_VSYNC_MASK;
      for (i = 0; i < 0x10000; i++) {
        dsp = inb(0x3BA) & DSP_VSYNC_MASK;
        if (dsp != dsp_old) cnt++;
        dsp_old = dsp;
      }

      /* If there are active sync changes, we found a Hercules board. */
      if (cnt) {
		bdm2InfoRec.chipset = VISAIdent(0);
		ErrorF("%s %s: %s detected\n",
			XCONFIG_PROBED,
			bdm2InfoRec.name,
			bdm2InfoRec.chipset);
	} else {
	/* there is no VISA 6845 card */
		VISAEnterLeave(LEAVE);
		return(FALSE);
	}
  } /* else (bdm2InfoRec.chipset) -- bdm2InfoRec.chipset is already set */
  if (!bdm2InfoRec.videoRam) {
	/* videoram not given in Xconfig */
	bdm2InfoRec.videoRam=256;
  }
  /* We do 'virtual' handling here as it is highly chipset specific */
  /* Screen size (pixels) is fixed, virtual size can be larger up to
   * ChipMaxVirtualX and ChipMaxVirtualY */
  /* Real display size is given by VISA_HDISPLAY and VISA_VDISPLAY,
   * desired virtual size is bdm2InfoRec.virtualX and bdm2InfoRec.virtualY.
   * Think they can be -1 at this point.
   * Maximum virtual size as done by the driver is
   * VISA_MAX_VIRTUAL_X and ..._Y
   */
   if (!(bdm2InfoRec.virtualX < 0)) {
	/* virtual set in Xconfig */
	ErrorF("%s %s: %s: Virtual not allowed for this chipset\n",
		XCONFIG_PROBED, bdm2InfoRec.name, bdm2InfoRec.chipset);
   }
   /* Set virtual to real size */
   bdm2InfoRec.virtualX = VISA_HDISPLAY;
   bdm2InfoRec.virtualY = VISA_VDISPLAY;
   /* Must return real display size */
   /* hardcoded in VISA */
   return(TRUE);
}

/*
 * VISAEnterLeave --
 *      enable/disable io permissions
 */

void 
VISAEnterLeave(enter)
     Bool enter;
{
  if (enter)
	xf86EnableIOPorts(bdm2InfoRec.scrnIndex);
  else
	xf86DisableIOPorts(bdm2InfoRec.scrnIndex);
}

/*
 * VISAInit --
 *      Handle the initialization of the VISAs registers
 */

void *
VISAInit(mode)
     DisplayModePtr mode;
{
/* this is a r/w copy of the initial graph mode */
static visaPtr visaInitVideoMode = NULL;

if (!visaInitVideoMode)
	visaInitVideoMode = (visaPtr)Xalloc(sizeof(visaRec));
/* memcpy(dest,source,size) */
memcpy((void *)visaInitVideoMode, (void *)&visaRegsGraf1280x1024,
       sizeof(visaRec));
return((void *)visaInitVideoMode);
}

/*
 * VISASave --
 *      save the current video mode
 */

void *
VISASave(save)
     visaPtr save;
{
unsigned char i;

if (save==NULL)
	save=(visaPtr)Xalloc(sizeof(visaRec));
save->conf=inb(VISA_CONFIG);
save->mode=inb(VISA_MODE);
/* The table of 6845 registers is write only, we return the local copy */
for (i=0; i<16; i++)
	save->tbl[i] = visaRegsActual.tbl[i];
return((void *)save);
}

/*
 * VISARestore --
 *      restore a video mode
 */

void
VISARestore(restore)
     visaPtr restore;
{
unsigned char i;
if (restore!=NULL) /* better be shure */ {
	outb(VISA_CONFIG, (visaRegsActual.conf = restore->conf));
	outb(VISA_MODE, (visaRegsActual.mode = restore->mode));
	for (i=0; i<16; i++) {
		outb(VISA_INDEX, i);
		outb(VISA_DATA, (visaRegsActual.tbl[i] = restore->tbl[i]));
	}
}
else ErrorF("Warning: VISARestore called with arg==NULL\n");
}

/*
 * VISASaveScreen();
 *	Disable the video on the frame buffer (screensaver)
 */

Bool
VISASaveScreen(pScreen,on)
	ScreenPtr pScreen;
	Bool      on;
{
if (on == SCREEN_SAVER_FORCER)
	SetTimeSinceLastInputEvent();
if (x386VTSema) {
	if (on) { /* Grrr! SaveScreen(on=TRUE) means turn ScreenSaver off */
		outb(VISA_MODE, (visaRegsActual.mode |= 0x08)); /* unblank */
	} else {
		outb(VISA_MODE, (visaRegsActual.mode &= 0xF7)); /* blank */
	}
} /* if we are not on the active VT, don't do anything - the screen
   * will be visible as soon as we switch back anyway (?) */
return(TRUE);
}

/* VISAAdjust --
 *      adjust the current video frame to display the mousecursor
 *      (x,y) is the upper left corner to be displayed.
 *      The VISA 6845 can't pan.
 */
void
VISAAdjust(x,y)
	int x, y;
{
}

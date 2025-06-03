/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * THOMAS ROELL, KEVIN E. MARTIN, AND RICKARD E. FAITH DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Rewritten for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 *
 * Header: /proj/X11/mit/server/ddx/x386/vga/RCS/vga.c,v 1.2 1991/06/27 00:02:49 root Exp
 */

/* $XFree86: mit/server/ddx/x386/accel/mach32/mach32.c,v 2.47 1994/03/03 12:43:18 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "mipointer.h"
#include "cursorstr.h"

#include "compiler.h"

#include "x386.h"
#include "x386Priv.h"
#include "x386Procs.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "mach32.h"
#include "regmach32.h"
#include "cfb.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

extern int mach32MaxClock;
extern int mach32MaxTlc34075Clock;
extern Bool x386Verbose, x386Resetting, x386Exiting, x386ProbeFailed;
extern void NoopDDA();
extern void mach32QueryBestSize();
extern void mach32WarpCursor();
extern void mach32RepositionCursor();
extern Bool miDCInitialize();
extern void SetTimeSinceLastInputEvent();
unsigned short mach32MemorySize = 0;

ScrnInfoRec mach32InfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    mach32Probe,      	/* Bool (* Probe)() */
    mach32Initialize,	/* Bool (* Init)() */
    mach32EnterLeaveVT,/* void (* EnterLeaveVT)() */
    NoopDDA,		/* void (* EnterLeaveMonitor)() */
    NoopDDA,		/* void (* EnterLeaveCursor)() */
    mach32AdjustFrame,	/* void (* AdjustFrame)() */
    mach32SwitchMode,	/* Bool (* SwitchMode)() */
    mach32PrintIdent,	/* void (* PrintIdent)() */
    8,			/* int depth */
    8,			/* int bitsPerPixel */
    PseudoColor,       	/* int defaultVisual */
    -1, -1,		/* int virtualX,virtualY */
    -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
    {0, },	       	/* OFlagSet options */
    {0, },	       	/* OFlagSet clockOptions */
    {0, },	       	/* OFlagSet xconfigFlag */
    NULL,	       	/* char *chipset */
    0,			/* int clocks */
    {0, },		/* int clock[MAXCLOCKS] */
    0,			/* int maxClock */
    0,			/* int videoRam */
    0xC0000,            /* int BIOSbase */   
    0,			/* unsigned long MemBase */
    240, 180,		/* int width, height */
    0,                  /* unsigned long  speedup */
    NULL,	       	/* DisplayModePtr modes */
    NULL,               /* char *clockprog */
    -1,                 /* int textclock */   
    FALSE,              /* Bool bankedMono */
    "Mach32",           /* char *name */
    {0, },		/* RgbRec blackColour */
    {0, },		/* RgbRec whiteColour */
    mach32ValidTokens,	/* int *validTokens */
    MACH32_PATCHLEVEL,	/* char *patchlevel */
};

short mach32alu[16] = {
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

LUTENTRY mach32savedLUT[256];
static Bool LUTissaved = FALSE;

short mach32MaxX, mach32MaxY;
short mach32VirtX, mach32VirtY;

Bool mach32Use4MbAperture = FALSE;
Bool mach32DAC8Bit = FALSE;

Bool mach32clkprobedif4fix = FALSE;

static unsigned Mach32_IOPorts[] = {
	/* VGA Registers */
        0x3B4, 0x3B5, 0x3BA, 0x3C0, 0x3C1, 0x3C2, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 
	0x3C8, 0x3C9, 0x3CA, 0x3CB, 0x3CC, 0x3CE, 0x3CF, 0x3D4, 0x3D5, 0x3DA,
	/* ATI VGA Registers */
	ATIEXT, ATIEXT+1,
	/* 8514 Registers */
        DAC_MASK, DAC_R_INDEX, DAC_W_INDEX, DAC_DATA, DISP_STAT,
	H_TOTAL, H_DISP, H_SYNC_STRT, H_SYNC_WID,
	V_TOTAL, V_DISP, V_SYNC_STRT, V_SYNC_WID,
	DISP_CNTL, ADVFUNC_CNTL, SUBSYS_CNTL, ROM_PAGE_SEL,
	CUR_Y, CUR_X, DESTY_AXSTP, DESTX_DIASTP, 
	ERR_TERM, MAJ_AXIS_PCNT, GP_STAT, CMD, SHORT_STROKE,
	BKGD_COLOR, FRGD_COLOR, WRT_MASK, RD_MASK,
	COLOR_CMP, BKGD_MIX, FRGD_MIX, MULTIFUNC_CNTL, PIX_TRANS,
	/* Mach32 Registers */
	GE_OFFSET_LO, GE_OFFSET_HI, CRT_OFFSET_LO, CRT_OFFSET_HI,
	CLOCK_SEL, SHADOW_SET, SHADOW_CTL, CRT_PITCH, GE_PITCH,
	EXT_SCISSOR_L, EXT_SCISSOR_R, EXT_SCISSOR_T, EXT_SCISSOR_B,
	MISC_OPTIONS, CONFIG_STATUS_1, CONFIG_STATUS_2, 
	MEM_CFG, MEM_BNDRY, ROM_ADDR_1, READ_SRC_X, CHIP_ID, EXT_FIFO_STATUS,
	R_EXT_GE_CONFIG, EXT_GE_CONFIG, DP_CONFIG, DEST_X_START, DEST_X_END,
	DEST_Y_END, ALU_FG_FN, MISC_CNTL, R_MISC_CNTL, HORZ_OVERSCAN,
	VERT_OVERSCAN, EXT_GE_STATUS, LINEDRAW, LINEDRAW_OPT, LINEDRAW_INDEX,
};
static int Num_Mach32_IOPorts = (sizeof(Mach32_IOPorts)/
				 sizeof(Mach32_IOPorts[0]));

static mach32CRTCRegRec mach32CRTCRegs;
static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

/*
 * ATI Hardware Probe
 *
 */

typedef struct ATIInformationBlock {
   char board_identifier[2];
   char equipment_flags[2];
   char asic_identifier;
   int  bios_major;
   int  bios_minor;
   char bios_date[21];
   int  VGA_Wonder_Present;
   int  Graphics_Ultra_Present;
   int  ATI_Accelerator_Present;
   int  Mach8_Present;
   int  Mach32_Present;
   int  Bus_8bit_Only;
   int  Bus_Type;
   int  Mem_Size;
   int  Mem_Type;
   int  DAC_Type;
} ATIInformationBlock;

int	mach32Ramdac;
char	*mach32ramdac_names[] = {
	"ATI-68830",
	"IMS-G173/SC1148[368]",
	"ATI68875/TLC34075/Bt885",
	"Bt47[68]/INMOS17[68]",
	"AT&T20C49[01]/Bt48[12]/IMS-G174/MU9C{1880,4910}/SC1502[56]",
	"ATI-68860",
	"unknown",
	"unknown",
};


int	mach32BusType;

static ATIInformationBlock *GetATIInformationBlock()
{
#define BIOS_DATA_SIZE 0x70
   char                       signature[]    = " 761295520";
   char                       bios_data[BIOS_DATA_SIZE];
   char                       bios_signature[10];
   int                        tmp;
   static ATIInformationBlock info = { 0, };
	 
   if (xf86ReadBIOS(mach32InfoRec.BIOSbase, 0x30,
		    (unsigned char *)bios_signature, 10) != 10) {
      return NULL;
   }
   if (strncmp( signature, bios_signature, 10 ))
	 return NULL;

   if (xf86ReadBIOS(mach32InfoRec.BIOSbase, 0x00,
		    (unsigned char *)bios_data, BIOS_DATA_SIZE)
       != BIOS_DATA_SIZE) {
      return NULL;
   }

   info.board_identifier[0]    = bios_data[ 0x40 ];
   info.board_identifier[1]    = bios_data[ 0x41 ];
   info.equipment_flags[0]     = bios_data[ 0x42 ];
   info.equipment_flags[1]     = bios_data[ 0x44 ];
   info.asic_identifier        = bios_data[ 0x43 ];
   info.bios_major             = bios_data[ 0x4c ];
   info.bios_minor             = bios_data[ 0x4d ];
   strncpy( info.bios_date, bios_data + 0x50, 20 );
   
   info.VGA_Wonder_Present     = bios_data[ 0x44 ] & 0x40;
   info.Graphics_Ultra_Present = !(bios_data[ 0x44 ] & 0x40);


				/* Test for graphics acceleration */
   if (!info.Graphics_Ultra_Present)
	 return &info;

				/* Test for ATI accelerator product */
   info.ATI_Accelerator_Present = 1;

   tmp = inw( ROM_ADDR_1 );
   outw( ROM_ADDR_1, 0x5555 );
   ProbeWaitIdleEmpty();
   if (inw( ROM_ADDR_1 ) != 0x5555) {
      info.ATI_Accelerator_Present = 0;
   } else {
      outw( ROM_ADDR_1, 0x2a2a );
      ProbeWaitIdleEmpty();
      if (inw( ROM_ADDR_1 ) != 0x2a2a) {
	 info.ATI_Accelerator_Present = 0;
      }
   }
   outw( ROM_ADDR_1, tmp );

   if (!info.ATI_Accelerator_Present)
	 return &info;
   
				/* Determine chip family */
   outw( DESTX_DIASTP, 0xaaaa );
   WaitIdleEmpty();
   if (inw( READ_SRC_X ) != 0x02aa)
	 info.Mach8_Present = 1;
   else
	 info.Mach32_Present = 1;

   outw( DESTX_DIASTP, 0x5555 );
   WaitIdleEmpty();
   if (inw( READ_SRC_X ) != 0x0555) {
      if (!info.Mach8_Present) {
	 ErrorF( "WARNING: Chip should be Mach8, but isn't!\n" );
	 info.Mach8_Present = 0;
      }
   } else {
      if (!info.Mach32_Present) {
	 ErrorF( "WARNING: Chip should be Mach32, but isn't!\n" );
	 info.Mach32_Present = 0;
      }
   }

   if (info.Mach8_Present) {
      tmp = inw( CONFIG_STATUS_1 );

      if (!(tmp & 0x02))
	    info.Bus_8bit_Only = 1;
      
      switch( (tmp & MEM_INSTALLED) >> 5 ) {
      case 0:
	 info.Mem_Size = 512;
	 break;
      case 1:
	 info.Mem_Size = 1024;
	 break;
      }

      info.Mem_Type = (tmp & DRAM_ENA) >> 4;
   }

   if (info.Mach32_Present) {
      tmp = inw( CONFIG_STATUS_1 );
      info.Bus_Type = tmp & BUS_TYPE;
      info.Mem_Type = tmp & MEM_TYPE;
      info.DAC_Type = (tmp & DACTYPE) >> 9;

      tmp = inw( CONFIG_STATUS_2 );
      if ((info.Bus_Type == ISA_16_BIT) && !(tmp & ISA_16_ENA))
	    info.Bus_8bit_Only = 1;

      tmp = inw( MISC_OPTIONS );
      switch (tmp & MEM_SIZE_ALIAS) {
      case MEM_SIZE_512K:
	 info.Mem_Size = 512;
	 break;
      case MEM_SIZE_1M:
	 info.Mem_Size = 1024;
	 break;
      case MEM_SIZE_2M:
	 info.Mem_Size = 2*1024;
	 break;
      case MEM_SIZE_4M:
	 info.Mem_Size = 4*1024;
	 break;
      }
   }

   return &info;
}

/*
 * mach32Probe --
 *     Probe the hardware
 */
Bool
mach32Probe()
{
    int                   i, j;
    DisplayModePtr        pMode, pEnd;
    ATIInformationBlock   *info;
    int                   extra_ram;
    int                   extra_caches;
    Bool                  sw_cursor_supplied;
    OFlagSet              validOptions;
    int                   tx, ty;


    xf86ClearIOPortList(mach32InfoRec.scrnIndex);
    xf86AddIOPorts(mach32InfoRec.scrnIndex, Num_Mach32_IOPorts, Mach32_IOPorts);

    xf86EnableIOPorts(mach32InfoRec.scrnIndex);

    info = GetATIInformationBlock();
    if (!info || !info->Mach32_Present) {
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }

    switch(info->DAC_Type) {
    case DAC_TLC34075: 
	mach32InfoRec.maxClock = mach32MaxTlc34075Clock;
	break;
    default:
	mach32InfoRec.maxClock = mach32MaxClock;
	break;
    }

    OFLG_ZERO(&validOptions);
    OFLG_SET(OPTION_SW_CURSOR, &validOptions);
    OFLG_SET(OPTION_NOLINEAR_MODE, &validOptions);
    OFLG_SET(OPTION_CSYNC, &validOptions);
    OFLG_SET(OPTION_INTEL_GX, &validOptions);
    OFLG_SET(OPTION_DAC_8_BIT, &validOptions);
    xf86VerifyOptions(&validOptions, &mach32InfoRec);
    
    mach32InfoRec.chipset = "mach32";
    x386ProbeFailed = FALSE;

    mach32BusType = info->Bus_Type;
    if (x386Verbose)
    {
	ErrorF("%s %s: card type: ", XCONFIG_PROBED, mach32InfoRec.name);
	switch(mach32BusType)
	{
	case EISA:
	    ErrorF("EISA\n");
	    break;
	case LOCAL_386SX:
	    ErrorF("localbus (386SX)\n");
	    break;
	case LOCAL_386DX:
	    ErrorF("localbus (386DX)\n");
	    break;
	case LOCAL_486:
	    ErrorF("localbus (486)\n");
	    break;
	case PCI:
	    ErrorF("PCI\n");
	    break;
	default:
	    ErrorF("ISA\n");
	}
    }

    if (!mach32InfoRec.clocks)
    {
        outb(DISP_CNTL, DISPEN_DISAB /*| INTERLACE*/ | MEMCFG_4 | ODDBNKENAB);
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
        outb(H_TOTAL, 0x72);
        outw(H_DISP, 0x4f);
        outb(H_SYNC_STRT, 0x51);
        outb(H_SYNC_WID, 0x1f);

        outb(DAC_MASK, 0x00);
        outb(DISP_CNTL, DISPEN_ENAB | MEMCFG_4 | ODDBNKENAB);

        /* 2-oct-93 TCG : detect clocks with dif4 fix */
        mach32clkprobedif4fix = TRUE;
        xf86GetClocks(16, mach32ClockSelect, NoopDDA, NoopDDA,
                      DISP_STAT, 2, 7, 44900, &mach32InfoRec);
        outw(CLOCK_SEL, 0); /* reset pass-through */
        mach32clkprobedif4fix = FALSE;

        outw(SHADOW_SET, 1);
        outw(SHADOW_CTL, 0x3f);
        outw(SHADOW_SET, 2);
        outw(SHADOW_CTL, 0x3f);
        outw(SHADOW_SET, 0);

        outb(DAC_MASK, 0xff);

        for (j = 0; j < 16; j++)
            mach32InfoRec.clock[j + 16] = mach32InfoRec.clock[j] / 2;

        mach32InfoRec.clocks = 32;
    }


    if (x386Verbose) {
	ErrorF("%s ",OFLG_ISSET(XCONFIG_CLOCKS,&mach32InfoRec.xconfigFlag) ?
			XCONFIG_GIVEN : XCONFIG_PROBED);
	ErrorF("%s: ", mach32InfoRec.name);
	ErrorF("Number of Clocks: %d", mach32InfoRec.clocks);

	for (i = 0; i < mach32InfoRec.clocks; i++) {
	    if (i % 8 == 0) 
               ErrorF("\n%s %s: clocks:", 
                 OFLG_ISSET(XCONFIG_CLOCKS,&mach32InfoRec.xconfigFlag) ?
                 XCONFIG_GIVEN : XCONFIG_PROBED,
                 mach32InfoRec.name);
	    ErrorF(" %6.2f", mach32InfoRec.clock[i]/1000.0);
	}
	ErrorF("\n");
    }

    tx = mach32InfoRec.virtualX;
    ty = mach32InfoRec.virtualY;
    pMode = pEnd = mach32InfoRec.modes;
    do {
	x386LookupMode(pMode, &mach32InfoRec);
	mach32InfoRec.virtualX = max(mach32InfoRec.virtualX, pMode->HDisplay);
	mach32InfoRec.virtualY = max(mach32InfoRec.virtualY, pMode->VDisplay);
	pMode = pMode->next;
    } while (pMode != pEnd);

    mach32VirtX = mach32InfoRec.virtualX = (mach32InfoRec.virtualX+7) & 0xfff8;
    mach32VirtY = mach32InfoRec.virtualY;

    if ((tx != mach32InfoRec.virtualX) || (ty != mach32InfoRec.virtualY))
	OFLG_CLR(XCONFIG_VIRTUAL,&mach32InfoRec.xconfigFlag);

    /*
     * Limitation of 8514 drawing commands  (taking into account 256 lines
     * required by the cache)
     */
    if (mach32VirtY > 1280) {
	ErrorF("Virtual screen height must be no greater than 1280\n");
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }
    if (mach32VirtX > 1536) {
	ErrorF("Virtual screen width must be no greater than 1536\n");
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }

    mach32MaxX = mach32VirtX - 1;
    mach32MaxY = mach32VirtY - 1 + 256;

    if (x386Verbose) {
	ErrorF("%s %s: Virtual resolution: %dx%d\n",
	       OFLG_ISSET(XCONFIG_VIRTUAL, &mach32InfoRec.xconfigFlag) ?
		XCONFIG_GIVEN : XCONFIG_PROBED,
		mach32InfoRec.name, mach32VirtX, mach32VirtY);
    }

    if ((mach32VirtX) < 1024) {
	ErrorF("mach32 X server requires virtual screen width >= 1024\n");
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }

    if (!mach32InfoRec.videoRam) {
#ifdef NEW_MEM_DETECT
	mach32InfoRec.videoRam = mach32GetMemSize();
#else
	mach32InfoRec.videoRam = info->Mem_Size;
#endif
    }

    /* Set mach32MemorySize to required MEM_SIZE value in MISC_OPTIONS */
    if (mach32InfoRec.videoRam <= 512)
	mach32MemorySize = MEM_SIZE_512K;
    else if (mach32InfoRec.videoRam <= 1024)
	mach32MemorySize = MEM_SIZE_1M;
    else if (mach32InfoRec.videoRam <= 2048)
	mach32MemorySize = MEM_SIZE_2M;
    else
	mach32MemorySize = MEM_SIZE_4M;
	
    if (x386Verbose) {
       ErrorF("%s %s: videoram: %dk\n",
	      OFLG_ISSET(XCONFIG_VIDEORAM, &mach32InfoRec.xconfigFlag) ?
	      XCONFIG_GIVEN : XCONFIG_PROBED, mach32InfoRec.name,
	      mach32InfoRec.videoRam );
    }

    if (((mach32MaxX+1)*(mach32MaxY+1)) > (mach32InfoRec.videoRam*1024)) {
	ErrorF("Not enough memory for requested virtual resolution (%dx%d)\n",
	       mach32VirtX, mach32VirtY);
	ErrorF("In addition to normal virtual screen size, mach32 X server\n");
	ErrorF("requires a 1024x256 area of video memory for the caches.\n");
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }

			/* Fixup mach32MaxY for larger cache.
			 * This must be done here so that cache area and
			 * scissor limits are set up correctly (this setup
			 * happens before the pixmap cache is initialized).
			 */
    
    extra_ram = mach32InfoRec.videoRam * 1024 -
		(mach32MaxX + 1) * (mach32MaxY + 1 );

    sw_cursor_supplied = OFLG_ISSET(OPTION_SW_CURSOR, &mach32InfoRec.options);

    if (!sw_cursor_supplied) {
	if (extra_ram >= MACH32_CURSBYTES)
	{
	    extra_ram -= (MACH32_CURSBYTES + 1023) & ~1023;
	    mach32InfoRec.videoRam -= (MACH32_CURSBYTES + 1023) / 1024;
	}
	else
	{
	    OFLG_SET(OPTION_SW_CURSOR, &mach32InfoRec.options);
	    ErrorF("Warning: Not enough memory to use the hardware cursor.\n");
	    ErrorF("  Decreasing the virtual Y resolution by 1 will allow\n");
	    ErrorF("  you to use the hardware cursor.\n");
	}
    }
    ErrorF("%s %s: Using %s cursor\n", sw_cursor_supplied ? XCONFIG_GIVEN :
	   XCONFIG_PROBED, mach32InfoRec.name,
	   OFLG_ISSET(OPTION_SW_CURSOR, &mach32InfoRec.options) ?
	   "software" : "hardware");

    extra_caches = (extra_ram / (mach32MaxX + 1)) / 256;
    mach32MaxY += extra_caches * 256;
    if (mach32MaxY > 1535)
	  mach32MaxY = 1535;	/* Limitation of 8514 drawing commands */
	  
    mach32Use4MbAperture = !OFLG_ISSET(OPTION_NOLINEAR_MODE,
				       &mach32InfoRec.options) &&
			   (info->Bus_Type == LOCAL_386SX ||
			    info->Bus_Type == LOCAL_386DX ||
			    info->Bus_Type == LOCAL_486 ||
			    info->Bus_Type == EISA ||
                            info->Bus_Type == PCI)
			   && xf86LinearVidMem();

    mach32Ramdac = info->DAC_Type;

    if (x386Verbose) {
	if (mach32Use4MbAperture) {
	    ErrorF("%s %s: Using 4 MB aperture\n", XCONFIG_PROBED,
		   mach32InfoRec.name);
	} else {
	    ErrorF("%s %s: Using 64 kb aperture\n",
		   OFLG_ISSET(OPTION_NOLINEAR_MODE, &mach32InfoRec.options) ?
		   XCONFIG_GIVEN : XCONFIG_PROBED, mach32InfoRec.name);
	}
	ErrorF("%s %s: Ramdac is %s\n", XCONFIG_PROBED, mach32InfoRec.name,
	       mach32ramdac_names[mach32Ramdac]);
    }

    /* The Type 2 RAMDACS can support 8 bits per RGB value, not just the
     * VGA-standard 6 bits; however, according to ATI, this is not an
     * official feature of the mach32.
     */
    mach32DAC8Bit = OFLG_ISSET(OPTION_DAC_8_BIT, &mach32InfoRec.options)
		    && (info->DAC_Type == DAC_TLC34075);

    if (x386Verbose)
	ErrorF("%s %s: Using %d bits per RGB value\n",
		(info->DAC_Type == DAC_TLC34075) ? XCONFIG_GIVEN :
		XCONFIG_PROBED, mach32InfoRec.name,
		mach32DAC8Bit ?  8 : 6);

    return(TRUE);
}


/*
 * mach32PrintIdent --
 *     Print the indentification of the video card.
 */
void
mach32PrintIdent()
{
    ErrorF("  %s: accelerated server for ATI Mach32 graphics adaptors ",
	   mach32InfoRec.name);
    ErrorF("(Patchlevel %s)\n", mach32InfoRec.patchLevel);
}


/*
 * mach32Initialize --
 *      Attempt to find and initialize a VGA framebuffer
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
mach32Initialize (scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
    int displayResolution = 75;  /* default to 75dpi */
    extern int monitorResolution;

    mach32InitDisplay(scr_index);
    mach32InitAperture(scr_index);
    mach32CalcCRTCRegs(&mach32CRTCRegs, mach32InfoRec.modes);
    mach32SetCRTCRegs(&mach32CRTCRegs);
    mach32InitEnvironment();

    /* Clear the display.
     * Need to set the color, origin, and size.  Then draw.
     */
    WaitQueue(6);
    outw(FRGD_COLOR, 1);
    outw(CUR_X, 0);
    outw(CUR_Y, 0);
    outw(MAJ_AXIS_PCNT, mach32MaxX);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | mach32MaxY);
    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

    mach32CacheInit(mach32VirtX, mach32VirtY);
    mach32FontCache8Init(mach32VirtX, mach32VirtY);

    mach32ImageInit();

    WaitIdleEmpty(); /* Make sure that all commands have finished */

    /*
     * Take display resolution from the -dpi flag if specified
     */

    if (monitorResolution)
	displayResolution = monitorResolution;

    if (!mach32ScreenInit(pScreen, mach32VideoMem,
			  mach32VirtX, mach32VirtY,
			  displayResolution, displayResolution,
			  mach32VirtX))

		return(FALSE);

    savepScreen = pScreen;

    pScreen->CloseScreen = mach32CloseScreen;
    pScreen->SaveScreen = mach32SaveScreen;

    if (OFLG_ISSET(OPTION_SW_CURSOR, &mach32InfoRec.options)) {
	miDCInitialize (pScreen, &x386PointerScreenFuncs);
    } else {
        pScreen->InstallColormap = mach32InstallColormap;
        pScreen->UninstallColormap = mach32UninstallColormap;
        pScreen->ListInstalledColormaps = mach32ListInstalledColormaps;
        pScreen->StoreColors = mach32StoreColors;
        pScreen->QueryBestSize = mach32QueryBestSize;
        x386PointerScreenFuncs.WarpCursor = mach32WarpCursor;
        (void)mach32CursorInit(0, pScreen);
    }

    return (cfbCreateDefColormap(pScreen));
}

/*
 * mach32EnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
mach32EnterLeaveVT(enter, screen_idx)
     Bool enter;
     int screen_idx;
{
    PixmapPtr pspix;
    ScreenPtr pScreen = savepScreen;

    pspix = (PixmapPtr)pScreen->devPrivate;

    if (enter) {
	if (vgaBase)
	    xf86MapDisplay(screen_idx, VGA_REGION);
	if (mach32VideoMem != vgaBase)
	    xf86MapDisplay(screen_idx, LINEAR_REGION);
	if (!x386Resetting) {
	    ScrnInfoPtr pScr = X386SCRNINFO(pScreen);

	    mach32InitDisplay(screen_idx);
	    mach32SetCRTCRegs(&mach32CRTCRegs);
	    mach32InitEnvironment();

	    WaitQueue(6);
	    outw(FRGD_COLOR, 1);
	    outw(CUR_X, 0);
	    outw(CUR_Y, 0);
	    outw(MAJ_AXIS_PCNT, mach32MaxX);
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | mach32MaxY);
	    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

	    mach32CacheInit(mach32VirtX, mach32VirtY);
	    mach32FontCache8Init(mach32VirtX, mach32VirtY);
	    mach32RestoreCursor(pScreen);
	    mach32AdjustFrame(pScr->frameX0, pScr->frameY0);

	    WaitIdleEmpty(); /* Make sure that all commands have finished */

	    if (LUTissaved) {
		mach32RestoreLUT(mach32savedLUT);
		LUTissaved = FALSE;
		mach32RestoreColor0(pScreen);
	    }

	    if (pspix->devPrivate.ptr != mach32VideoMem && ppix) {
		pspix->devPrivate.ptr = mach32VideoMem;
		(mach32ImageWriteFunc)(0, 0, pScreen->width, pScreen->height,
				 ppix->devPrivate.ptr,
				 PixmapBytePad(pScreen->width,
					       pScreen->rootDepth),
				 0, 0, MIX_SRC, ~0);
	    }
	}
	if (ppix) {
	    (pScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
	}
    } else {
	if (vgaBase)
	    xf86MapDisplay(screen_idx, VGA_REGION);
	if (mach32VideoMem != vgaBase)
	    xf86MapDisplay(screen_idx, LINEAR_REGION);
	if (!x386Exiting) {
	    ppix = (pScreen->CreatePixmap)(pScreen,
					   pScreen->width, pScreen->height,
					   pScreen->rootDepth);

	    if (ppix) {
		(mach32ImageReadFunc)(0, 0, pScreen->width, pScreen->height,
				ppix->devPrivate.ptr,
				PixmapBytePad(pScreen->width,
					      pScreen->rootDepth),
				0, 0, ~0);
		pspix->devPrivate.ptr = ppix->devPrivate.ptr;
	    }
	}

	mach32CursorOff();
	mach32SaveLUT(mach32savedLUT);
	LUTissaved = TRUE;
	if (!x386Resetting) {
	    mach32CleanUp();
	}
	if (vgaBase)
	    xf86UnMapDisplay(screen_idx, VGA_REGION);
	if (mach32VideoMem != vgaBase)
	    xf86UnMapDisplay(screen_idx, LINEAR_REGION);
    }
}

/*
 * mach32CloseScreen --
 *      called to ensure video is enabled when server exits.
 */

Bool
mach32CloseScreen(screen_idx)
     int screen_idx;
{
    extern void mach32ClearSavedCursor();

    /*
     * Hmm... The server may shut down even if it is not running on the
     * current vt. Let's catch this case here.
     */
    x386Exiting = TRUE;
    if (x386VTSema)
	mach32EnterLeaveVT(LEAVE, screen_idx);
    else if (ppix) {
    /* 7-Jan-94 CEG: The server is not running on the current vt.
     * Free the screen snapshot taken when the server vt was left.
     */
	    (savepScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
    }
    mach32ClearSavedCursor(screen_idx);
    savepScreen = NULL;
    return(TRUE);
}

/*
 * mach32SaveScreen --
 *      blank the screen.
 */

Bool
mach32SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
     int	   ext_ge_config;

    if (on)
	SetTimeSinceLastInputEvent();

    if (x386VTSema) {
	/*
	 *	Make sure that register 0-3 are addressed
	 */
	ext_ge_config = inw(R_EXT_GE_CONFIG) & ~0x3000;
	outw(EXT_GE_CONFIG, ext_ge_config);

	if (on) {
	    mach32RestoreColor0(pScreen);

	    outb(DAC_MASK, 0xff);
	} else {
	    outb(DAC_W_INDEX, 0);
	    outb(DAC_DATA, 0);
	    outb(DAC_DATA, 0);
	    outb(DAC_DATA, 0);

	    outb(DAC_MASK, 0x00);
	}
    }

    return(TRUE);
}

/*
 * mach32AdjustFrame --
 *      Modify the CRT_OFFSET for panning the display.
 */
void
mach32AdjustFrame(x, y)
    int x, y;
{
    int byte_offset = (x + y*mach32VirtX) >> 2;

    mach32CursorOff();
    outw(CRT_OFFSET_LO, byte_offset & 0xffff);
    outw(CRT_OFFSET_HI, (byte_offset >> 16) & 0xf);
    mach32RepositionCursor(savepScreen);
}

/*
 * mach32SwitchMode --
 *      Reinitialize the CRTC registers for the new mode.
 */
Bool
mach32SwitchMode(mode)
    DisplayModePtr mode;
{
    mach32CalcCRTCRegs(&mach32CRTCRegs, mode);
    mach32SetCRTCRegs(&mach32CRTCRegs);

    return(TRUE);
}

/*
 * mach32ClockSelect
 */
Bool
mach32ClockSelect(no)
     int no;
{
  switch(no)
  {
    case CLK_REG_SAVE:
      break;
    case CLK_REG_RESTORE:
      break;
    default:
      if (mach32clkprobedif4fix)
         outw(CLOCK_SEL, (no << 2) | 0xac1);
      else
         outw(CLOCK_SEL, (no << 2) | 0xa01);
  }
  return(TRUE);
}

/*
 * Written by Jake Richter
 * Copyright (c) 1989, 1990 Panacea Inc., Londonderry, NH - All Rights Reserved
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * Additions by Kevin E. Martin (martin@cs.unc.edu)
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Rewritten for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 *
 */

/* $XFree86: mit/server/ddx/x386/accel/mach32/mach32init.c,v 2.19 1994/02/27 05:20:32 dawes Exp $ */

#include "X.h"
#include "input.h"

#include "xf86_OSlib.h"
#include "x386.h"
#include "mach32.h"
#include "ativga.h"
#include "regmach32.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

extern Bool x386Verbose;
extern int  mach32MaxClock;

static LUTENTRY oldlut[256];
static Bool LUTInited = FALSE;

static short old_DAC_MASK;
static short old_DAC_in_clock;
static short old_DAC_out_clock;
static short old_DAC_mux_clock;
static short old_clock_sel;

static Bool mach32Inited = FALSE;

pointer mach32VideoMem = NULL;

static char old_ATI2E, old_ATI32, old_ATI36;
static char old_GRA06, old_SEQ02, old_SEQ04;

static unsigned short old_MEM_BNDRY;
static unsigned short old_MISC_OPTIONS;
static unsigned short old_EXT_GE_CONFIG;
static unsigned short old_MISC_CNTL;
extern unsigned short mach32MemorySize;

int mach32VideoPageBoundary[MACH32_MAX_Y+1];

extern void mach32SetVGAPage();

/*	mach32CalcCRTCRegs(crtcRegs, mode)

	Initializes the Mach32 for the currently selected CRTC parameters.
*/
void mach32CalcCRTCRegs(crtcRegs, mode)
     mach32CRTCRegPtr crtcRegs;
     DisplayModePtr mode;
{
    crtcRegs->h_total = (mode->HTotal >> 3) - 1;
    crtcRegs->h_disp = (mode->HDisplay >> 3) - 1;
    crtcRegs->h_sync_strt = (mode->HSyncStart >> 3) - 1;
    crtcRegs->h_sync_wid = (mode->HSyncEnd - mode->HSyncStart) >> 3;

    if (crtcRegs->h_sync_wid > 0x1f) {
	ErrorF("%s %s: Horizontal Sync width (%d) in mode \"%s\"\n",
	       XCONFIG_PROBED, mach32InfoRec.name, crtcRegs->h_sync_wid,
	       mode->name);
	ErrorF("\tshortened to 248 pixels\n");
	crtcRegs->h_sync_wid &= 0x1f;
    }

    if (mode->Flags & V_NHSYNC) crtcRegs->h_sync_wid |= H_POLARITY_NEG;

    crtcRegs->v_total = SKIP_2(mode->VTotal - 1);
    crtcRegs->v_disp = SKIP_2(mode->VDisplay - 1);
    crtcRegs->v_sync_strt = SKIP_2(mode->VSyncStart - 1);
    crtcRegs->v_sync_wid = mode->VSyncEnd - mode->VSyncStart;

    if (crtcRegs->v_sync_wid > 0x1f) {
	ErrorF("%s %s: Vertical Sync width (%d) in mode \"%s\"\n",
	       XCONFIG_PROBED, mach32InfoRec.name, crtcRegs->v_sync_wid,
	       mode->name);
	ErrorF("\tshortened to 31 lines\n");
	crtcRegs->v_sync_wid &= 0x1f;
    }

    if (mode->Flags & V_NVSYNC) crtcRegs->v_sync_wid |= V_POLARITY_NEG;

    crtcRegs->disp_cntl = DISPEN_ENAB | MEMCFG_4;
    if (mode->Flags & V_INTERLACE) crtcRegs->disp_cntl |= INTERLACE;

    crtcRegs->clock_sel = (mode->Clock << 2) | VFIFO_DEPTH_6 | DISABPASSTHRU;
    if (mode->Flags & V_CSYNC) crtcRegs->clock_sel |= COMPOSITE_SYNC;
    if (OFLG_ISSET(OPTION_CSYNC, &mach32InfoRec.options))
	crtcRegs->clock_sel |= COMPOSITE_SYNC;
}

/*	mach32SetCRTCRegs(crtcRegs)

	Initializes the Mach32 for the currently selected CRTC parameters.
*/
void mach32SetCRTCRegs(crtcRegs)
     mach32CRTCRegPtr crtcRegs;
{

    /*
     * Reset controller while setting CRTC registers
     * This is recommended by the mach32 programmer's guide
     */
    WaitQueue(1);
    outb(DISP_CNTL, DISPEN_DISAB);
    WaitIdleEmpty();

    /* Now initialize the display controller part of the Mach32.
     * The CRTC registers are passed in from the calling routine.
     */

    /* Horizontal CRTC registers */
    outb(H_TOTAL,     crtcRegs->h_total);
    outw(H_DISP,      crtcRegs->h_disp);
    outb(H_SYNC_STRT, crtcRegs->h_sync_strt);
    outb(H_SYNC_WID,  crtcRegs->h_sync_wid);

    /* Vertical CRTC registers */
    outw(V_TOTAL,     crtcRegs->v_total);
    outw(V_DISP,      crtcRegs->v_disp);
    outw(V_SYNC_STRT, crtcRegs->v_sync_strt);
    outw(V_SYNC_WID,  crtcRegs->v_sync_wid);

    /* Set the width of the display -- useful for future extensions */
    outw(CRT_PITCH, mach32VirtX >> 3);

    outw(GE_PITCH,  mach32VirtX >> 3);

    /* Clock select register */
    outw(CLOCK_SEL, crtcRegs->clock_sel);

    /* Zero overscan register to insure proper color */
    outw(HORZ_OVERSCAN, 0);
    outw(VERT_OVERSCAN, 0);

    /* Set the DAC for the currect mode */
    mach32SetRamdac(crtcRegs->clock_sel);

    /* Display control register -- this one turns on the display */
    WaitIdleEmpty();
    outb(DISP_CNTL, crtcRegs->disp_cntl);
}

/*	mach32SaveLUT(lut)

	Saves the LUT in lut.
*/
void mach32SaveLUT(lut)
     LUTENTRY *lut;
{
    short i;

    outb(DAC_R_INDEX, 0);
    for (i = 0; i < 256; i++) {
	lut[i].r = inb(DAC_DATA);
	lut[i].g = inb(DAC_DATA);
	lut[i].b = inb(DAC_DATA);
    }
}

/*	mach32RestoreLUT(lut)

	Restores the LUT in lut.
*/
void mach32RestoreLUT(lut)
     LUTENTRY *lut;
{
    short i;

    outb(DAC_W_INDEX, 0);
    for (i = 0; i < 256; i++) {
	outb(DAC_DATA, lut[i].r);
	outb(DAC_DATA, lut[i].g);
	outb(DAC_DATA, lut[i].b);
    }
}

/*	mach32InitLUT()

	Loads the Look-Up Table with all black.
	Assumes 8-bit board is in use.
*/
void mach32InitLUT()
{
    short i;

    mach32SaveLUT(oldlut);
    LUTInited = TRUE;

    outb(DAC_W_INDEX, 0);

    /* Load the LUT entries */
    for (i = 0; i < 256; i++) {
	outb(DAC_DATA, 0);
	outb(DAC_DATA, 0);
	outb(DAC_DATA, 0);
    }
}

/*	mach32InitEnvironment()

	Initializes the Mach32's drawing environment and clears the display.
*/
void mach32InitEnvironment()
{
    /* Current mixes, src, foreground active */
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

    /* Clipping rectangle to full drawable space */
    outw(EXT_SCISSOR_T, 0);
    outw(EXT_SCISSOR_L, 0);
    outw(EXT_SCISSOR_R, mach32MaxX);
    outw(EXT_SCISSOR_B, mach32MaxY);

    /* Enable writes to all planes and reset color compare */
    outw(WRT_MASK, 0xffff);
    outw(MULTIFUNC_CNTL, PIX_CNTL | 0x0000);

#ifdef CLEAR_DISPLAY
    /* Clear the display.  Need to set the color, origin, and size.
     * Then draw.
     */
    WaitQueue(6);
    outw(FRGD_COLOR, 1);
    outw(CUR_X, 0);
    outw(CUR_Y, 0);
    outw(MAJ_AXIS_PCNT, 1023);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | 1023);
    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);
#endif

    WaitQueue(4);

    /* Reset current draw position */
    outw(CUR_X, 0);
    outw(CUR_Y, 0);

    /* Reset current colors, foreground is all on, background is 0. */
    outw(FRGD_COLOR, 0xffff);
    outw(BKGD_COLOR, 0x0000);
}

/*	mach32InitAperture(screen_idx)

	Initialize the aperture for the Mach32.
*/

void mach32InitAperture(screen_idx)
    int screen_idx;
{
    int i;

    if (!mach32VideoMem) {
	if (mach32Use4MbAperture) {
	    int apaddr;
	    int mem_cfg_shift;
	    unsigned long hw_offset = 0;

	    /* For PCI, there is no hw_offset */
	    if (mach32BusType != PCI) {
		if (OFLG_ISSET(OPTION_INTEL_GX, &mach32InfoRec.options)) {
		    hw_offset = 0x78000000;
		} else if (mach32InfoRec.MemBase != 0) {
		    hw_offset = mach32InfoRec.MemBase & 0xf8000000;
		}
	    }

	    if (mach32BusType == PCI)
		mem_cfg_shift = 4;
	    else
		mem_cfg_shift = 8;

	    apaddr = inw(MEM_CFG) >> mem_cfg_shift;

	    if (!apaddr && !hw_offset)
		apaddr = 0x7c;
	    if (x386Verbose) {
		ErrorF("%s %s: Aperture mapped to 0x%x + 0x%x\n",
		       XCONFIG_PROBED, mach32InfoRec.name, apaddr << 20,
		       hw_offset);
	    }
	    /* XXXX probably should restore MEM_CFG at exit */
	    outw(MEM_CFG, (apaddr << mem_cfg_shift) | 0x02);

	    mach32VideoMem = xf86MapVidMem(screen_idx, LINEAR_REGION, 
					   (pointer)((apaddr << 20) +
						     hw_offset),
					   4 * 1024 * 1024);

	    /* Initialize the mach32VideoPageBoundary array */
	    for (i = 0; i <= mach32MaxY; i++)
		mach32VideoPageBoundary[i] = 0;
	} else {
	    if (vgaBase) {
	        mach32VideoMem = vgaBase;
	    } else {
	        mach32VideoMem = xf86MapVidMem(screen_idx, LINEAR_REGION,
					       (pointer)0xa0000, 64 * 1024);
            }

	    /* Initialize the mach32VideoPageBoundary array */
	    for (i = 0; i <= mach32MaxY; i++) {

		mach32VideoPageBoundary[i] = 65536 -
		    ((i*mach32VirtX) - ((i*mach32VirtX)&0xffff0000));
		if (mach32VideoPageBoundary[i] >= mach32MaxX)
		    mach32VideoPageBoundary[i] = 0;
	    }
	}
    }
}

/*
 * From the _Programmer's Guide to the mach32 Registers_:
 * MISC_CNTL[8:9]   BLANK_ADJUST Type 2 DAC only:  Delays BLANK by 1 or 2 PCLK
 * MISC_CNTL[10:11] PIXEL_DELAY  Adjusts pixel data skew from PCLK (adjusts
 *                               setup and hold times to suit different DACs)
 *
 * So x should have blank adjust in bits 0:1; pixel delay in 2:3.
 */
#define	SET_BLANK_ADJ(x)	{\
	unsigned short tmp; \
	tmp = inw(R_MISC_CNTL) & 0xf0f0; \
	outw(MISC_CNTL, tmp | (((x) & 0x0f) << 8)); \
}

void mach32SetRamdac(clock)
    int clock;
{
    int	old_EXT_GE_CONFIG;

    switch(mach32Ramdac) {

    case DAC_TLC34075:
	/* get the current value */
	old_EXT_GE_CONFIG = inw(R_EXT_GE_CONFIG) & ~0x3100;
	
	if (mach32InfoRec.clock[(clock >> 2) & 0xf] > 80000) {
	    
	    /* Guarantee low pixel clock */
	    outw(CLOCK_SEL, 0x11);

	    /* Make sure that upper 2 address bit are ok */
	    outw(EXT_GE_CONFIG, old_EXT_GE_CONFIG | 0x2000);

	    /* pixel clock is SCLK/2 and VCLK/2 */
	    outb(OUTPUT_CLK_SEL, 0x09);

	    /* set MUX contol 8/16 to 16 */
	    outb(MUX_CNTL, 0x1d);

	    /* Set clock source */
	    outb(INPUT_CLK_SEL, 1);

	    /* set multiplex_pixels */
	    outw(EXT_GE_CONFIG, old_EXT_GE_CONFIG | 0x0100);

	    /*set the blank adj and pixel delay values */
	    SET_BLANK_ADJ(1);

	    /* allow all 8 bit to go though dac */
	    outb(DAC_MASK, 0xff);

	    /* restore the pixel clock */
	    outw(CLOCK_SEL, clock);
	} else {
	    /* Guarantee low pixel clock */
	    outw(CLOCK_SEL, 0x11);

	    /*set the blank adj and pixel delay values */
	    SET_BLANK_ADJ(0x0c);

	    /* Make sure that upper 2 address bit are ok */
	    outw(EXT_GE_CONFIG, old_EXT_GE_CONFIG | 0x2000);

	    /* Set clock source */
	    outb(INPUT_CLK_SEL, 0);

	    /* pixel clock is SCLK/1 and VCLK */
	    outb(OUTPUT_CLK_SEL, 0x30);

	    /* set MUX contol 8/16 to 16 */
	    outb(MUX_CNTL, 0x2d);

	    /* allow all 8 bit to go though dac none mux */
	    outw(EXT_GE_CONFIG, old_EXT_GE_CONFIG);

	    outb(DAC_MASK, 0xff);

	    /* restore the pixel clock */
	    outw(CLOCK_SEL, clock);
	}
	break;
    }
}

/*	mach32InitDisplay(screen_idx)

	Initializes the display for the Mach32.
*/
void mach32InitDisplay(screen_idx)
     int screen_idx;
{
    if (mach32Inited)
	return;

    xf86EnableIOPorts(mach32InfoRec.scrnIndex);
    mach32SaveVGAInfo(screen_idx);

    outb(ATIEXT, ATI2E); old_ATI2E = inb(ATIEXT+1);
    outb(ATIEXT, ATI32); old_ATI32 = inb(ATIEXT+1);
    outb(ATIEXT, ATI36); old_ATI36 = inb(ATIEXT+1);
    outb(VGAGRA, GRA06); old_GRA06 = inb(VGAGRA+1);
    outb(VGASEQ, SEQ02); old_SEQ02 = inb(VGASEQ+1);
    outb(VGASEQ, SEQ04); old_SEQ04 = inb(VGASEQ+1);

    /* */
    outw(VGASEQ, SEQ04 | 0x0a << 8);

    /* Enable write access to all memory maps */
    outw(VGASEQ, SEQ02 | 0x0f << 8);

    /* Set linear addressing mode */
    outw(ATIEXT, ATI36 | 0x05 << 8);

    /* Set the VGA display buffer to 0xa0000 */
    outw(VGAGRA, GRA06 | 0x05 << 8);

    mach32SetVGAPage(0);

    old_MISC_CNTL = inw(R_MISC_CNTL);
    old_EXT_GE_CONFIG = inw(R_EXT_GE_CONFIG);

    outw(EXT_GE_CONFIG, old_EXT_GE_CONFIG & ~0x3000);
    old_DAC_MASK = inb(DAC_MASK);

    old_MEM_BNDRY = inw(MEM_BNDRY);
    outw(MEM_BNDRY, 0); /* Turn off the memory boundary */
    old_MISC_OPTIONS = inw(MISC_OPTIONS);
    outw(MISC_OPTIONS, (old_MISC_OPTIONS & ~MEM_SIZE_ALIAS) | mach32MemorySize);

    old_clock_sel = inw(CLOCK_SEL);

    switch(mach32Ramdac) {

    case DAC_TLC34075:
	outw(EXT_GE_CONFIG, (old_EXT_GE_CONFIG & ~0x3000) | 0x2000);
	outw(CLOCK_SEL, 0x11);

	old_DAC_in_clock = inb(INPUT_CLK_SEL);
	old_DAC_out_clock = inb(OUTPUT_CLK_SEL);
	old_DAC_mux_clock = inb(MUX_CNTL);

	outw(EXT_GE_CONFIG, (old_EXT_GE_CONFIG & ~0x3000));
	outw(CLOCK_SEL, old_clock_sel);
	break;
    }

    /* Reset the 8514/A, and disable all interrupts. */
    outw(SUBSYS_CNTL, GPCTRL_RESET | CHPTEST_NORMAL);
    outw(SUBSYS_CNTL, GPCTRL_ENAB | CHPTEST_NORMAL);

    /* Initialize the drawing and display offsets */
    outw(GE_OFFSET_LO,  0);
    outw(GE_OFFSET_HI,  0);
    mach32AdjustFrame(mach32InfoRec.frameX0, mach32InfoRec.frameY0);

    /* Save the colormap */
    mach32InitLUT();

    /* Save the shadow set registers */
    outw(SHADOW_SET, 1);
    outw(SHADOW_CTL, 0);
    outw(SHADOW_SET, 2);
    outw(SHADOW_CTL, 0);
    outw(SHADOW_SET, 0);

    if (mach32DAC8Bit)
	outw(EXT_GE_CONFIG, old_EXT_GE_CONFIG | DAC_8_BIT_EN);

    WaitIdleEmpty(); /* Make sure that all commands have finished */

    mach32Inited = TRUE;
}

/*	mach32Cleanup()

	Resets the state of the video display for text.
*/
void mach32CleanUp()
{
    if (!mach32Inited)
	return;

    switch(mach32Ramdac) {

    case DAC_TLC34075:
	SET_BLANK_ADJ(0x0c);
	outw(CLOCK_SEL, 0x11);
	outw(EXT_GE_CONFIG, (old_EXT_GE_CONFIG & ~0x3000) | 0x2000);
	outb(INPUT_CLK_SEL, old_DAC_in_clock);
	outb(OUTPUT_CLK_SEL, old_DAC_out_clock);
	outb(MUX_CNTL, old_DAC_mux_clock);
	outw(EXT_GE_CONFIG, old_EXT_GE_CONFIG & ~0x3000);
	outb(DAC_MASK, 0xff);
	outw(CLOCK_SEL, old_clock_sel);
	break;
    }

    outw(SHADOW_SET, 1);
    outw(SHADOW_CTL, 0x3f);
    outw(SHADOW_SET, 2);
    outw(SHADOW_CTL, 0x3f);
    outw(SHADOW_SET, 0);

    if (LUTInited) {
	mach32RestoreLUT(oldlut);
	LUTInited = FALSE;
    }

    /* Reset the VGA registers */
    outw(ATIEXT, ATI2E | old_ATI2E << 8);
    outw(ATIEXT, ATI32 | old_ATI32 << 8);
    outw(ATIEXT, ATI36 | old_ATI36 << 8);
    outw(VGAGRA, GRA06 | old_GRA06 << 8);
    outw(VGASEQ, SEQ02 | old_SEQ02 << 8);
    outw(VGASEQ, SEQ04 | old_SEQ04 << 8);

    outw(EXT_GE_CONFIG, (old_EXT_GE_CONFIG & ~0x3000));
    outb(DAC_MASK, old_DAC_MASK);
    outw(MEM_BNDRY, old_MEM_BNDRY);
    outw(MISC_OPTIONS, old_MISC_OPTIONS);

    outw(MISC_CNTL, old_MISC_CNTL);

    WaitIdleEmpty(); /* Make sure that all commands have finished */

    outw(ADVFUNC_CNTL, 6);

    outw(CRT_OFFSET_LO, 0);
    outw(CRT_OFFSET_HI, 0);

    mach32CursorOff();


    mach32RestoreVGAInfo();

    xf86DisableIOPorts(mach32InfoRec.scrnIndex);

    mach32Inited = FALSE;
}

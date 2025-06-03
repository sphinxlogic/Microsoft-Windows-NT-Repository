/*
 * Copyright 1993,1994 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/SuperProbe/RamDac.c,v 2.10 1994/03/05 01:58:49 dawes Exp $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, 0x3C6, 0x3C7, 0x3C8, 0x3C9};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

#define RED	0
#define GREEN	1
#define BLUE	2

static void ReadPelReg __STDCARGS((Byte, Byte *));
static void WritePelReg __STDCARGS((Byte, Byte *));
static Byte SetComm __STDCARGS((Byte));
static Bool Width8Check __STDCARGS((void));
static Bool TestDACBit __STDCARGS((Byte, Byte, Byte));
static Bool S3_Bt485Check __STDCARGS((int *));
static Bool S3_TVP3020Check __STDCARGS((int *));
static void CheckMach32 __STDCARGS((int *));

#ifdef __STDC__
static void ReadPelReg(Byte Index, Byte *Pixel)
#else
static void ReadPelReg(Index, Pixel)
Byte Index;
Byte *Pixel;
#endif
{
	outp(0x3C7, Index);
	Pixel[RED] = inp(0x3C9);
	Pixel[GREEN] = inp(0x3C9);
	Pixel[BLUE] = inp(0x3C9);

	return;
}

#ifdef __STDC__
static void WritePelReg(Byte Index, Byte *Pixel)
#else
static void WritePelReg(Index, Pixel)
Byte Index;
Byte *Pixel;
#endif
{
	outp(0x3C8, Index);
	outp(0x3C9, Pixel[RED]);
	outp(0x3C9, Pixel[GREEN]);
	outp(0x3C9, Pixel[BLUE]);
	return;
}

#ifdef __STDC__
static Byte SetComm(Byte Comm)
#else
static Byte SetComm(Comm)
Byte Comm;
#endif
{
	(void) dactocomm();
	outp(0x3C6, Comm);
	(void) dactocomm();
	return(inp(0x3C6));
}

static Bool Width8Check()
{
	Byte old, x, v;
	Byte pix[3];

	old = inp(0x3C8);
	ReadPelReg(0xFF, pix);
	v = pix[RED];
	pix[RED]= 0xFF;
	WritePelReg(0xFF, pix);
	ReadPelReg(0xFF, pix);
	x = pix[RED];
	pix[RED] = v;
	WritePelReg(0xFF, pix);
	outp(0x3C8, old);
	return(x == 0xFF);
}

#ifdef __STDC__
static Bool TestDACBit(Byte Bit, Byte OldComm, Byte OldPel)
#else
static Bool TestDACBit(Bit, OldComm, OldPel)
Byte Bit;
Byte OldComm;
Byte OldPel;
#endif
{
	Byte tmp;

	dactopel();
	outp(0x3C6, OldPel & (Bit ^ 0xFF));
	(void) dactocomm();
	outp(0x3C6, OldComm | Bit);
	tmp = inp(0x3C6);
	outp(0x3C6, tmp & (Bit ^ 0xFF));
	return((tmp & Bit) != 0);
}

static Bool S3_Bt485Check(RamDac)
int *RamDac;
{
	Byte old1, old2, old3, old4;
	Byte lock1, lock2;
	Bool Found = FALSE;
	Bool DoChecks = FALSE;

	lock1 = rdinx(CRTC_IDX, 0x38);
	lock2 = rdinx(CRTC_IDX, 0x39);
	wrinx(CRTC_IDX, 0x38, 0x48);
	wrinx(CRTC_IDX, 0x39, 0xA5);

	old1 = inp(0x3C6);
	old2 = rdinx(CRTC_IDX, 0x55);
	outp(0x3C6, 0x0F);
	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x02);
	old3 = inp(0x3C6);
	if ((old3 & 0x80) == 0x80)
	{
		Found = TRUE;
		if ((old3 & 0xC0) == 0x80)
		{
			*RamDac = DAC_BT485;
			DoChecks = TRUE;
		}
		else if ((old3 & 0xF0) == 0xD0)
		{
			DoChecks = TRUE;
			*RamDac = DAC_ATT505;
		}
		else
		{
			*RamDac = DAC_UNKNOWN;
		}
	}
	else if ((old3 & 0xF0) == 0x40)
	{
		Found = TRUE;
		*RamDac = DAC_ATT504;
		DoChecks = TRUE;
	}
	else
	{
		/* Perhaps status reg is hidden behind CR3 */
		wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x01);
		old3 = inp(0x3C6);
		if ((old3 & 0x80) == 0x80)
		{
			/* OK.  CR3 is active... */
			wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00);
			old3 = inp(0x3C8);
			outp(0x3C8, 0x00);
			wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x02);
			old4 = inp(0x3C6);
			if ((old4 & 0x80) == 0x80)
			{
				Found = TRUE;
				if ((old4 & 0xC0) == 0x80)
				{
					*RamDac = DAC_BT485;
				}
				else if ((old4 & 0xF0) == 0xD0)
				{
					*RamDac = DAC_ATT505;
				}
				else
				{
					*RamDac = DAC_UNKNOWN;
				}
			}
			else if ((old4 & 0xF0) == 0x40)
			{
				Found = TRUE;
				*RamDac = DAC_ATT504;
			}
			if ((Found) && (*RamDac != DAC_UNKNOWN))
			{
				*RamDac |= DAC_6_8_PROGRAM;
				if (Width8Check())
				{
					*RamDac |= DAC_8BIT;
				}
			}
			wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00);
			outp(0x3C8, old3);
		}
	}
	if (DoChecks)
	{
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
	}
	wrinx(CRTC_IDX, 0x55, old2);
	outp(0x3C6, old1);

	wrinx(CRTC_IDX, 0x39, lock2);
	wrinx(CRTC_IDX, 0x38, lock1);

	return(Found);
}

static Bool S3_TVP3020Check(RamDac)
int *RamDac;
{
	Byte old1, old2, old3, old4;
	Byte lock1, lock2;
	Bool Found = FALSE;

	/*
	 * TI ViewPoint TVP3020 support - Robin Cutshaw
	 *
	 * The 3020 has 8 direct registers accessed through standard
	 * VGA registers 0x3C8, 0x3C9, 0x3C6, and 0x3C7.  Bit 0 of
	 * CR55 is used to map these four register to the low four
	 * or high four direct 3020 registers.  The high register set
	 * includes index and data registers which are used to address
	 * indirect registers 0x00-0x3F and 0xFF.  Indirect register
	 * 0x3F is the chip ID register which will always return 0x20.
	 */

	lock1 = rdinx(CRTC_IDX, 0x38);
	lock2 = rdinx(CRTC_IDX, 0x39);
	wrinx(CRTC_IDX, 0x38, 0x48);
	wrinx(CRTC_IDX, 0x39, 0xA5);

	old1 = inp(0x3C6);
	old2 = rdinx(CRTC_IDX, 0x55);

	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x01); /* high four registers */
	old3 = inp(0x3C6);             /* read current index register value */

	outp(0x3C6, 0x3F);     /* write ID register index to index register */
	old4 = inp(0x3C7);     /* read ID register from data register       */
	if (old4 == 0x20) {
		Found = TRUE;
		*RamDac = DAC_TVP3020;
		*RamDac |= DAC_6_8_PROGRAM;
		wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00); /* regular VGA */
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
	}

	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x01); /* high four registers */
	outp(0x3C6, old3);                  /* restore index register value */

	wrinx(CRTC_IDX, 0x55, old2);
	outp(0x3C6, old1);

	wrinx(CRTC_IDX, 0x39, lock2);
	wrinx(CRTC_IDX, 0x38, lock1);

	return(Found);
}

static void CheckMach32(RamDac)
int *RamDac;
{
	Word Port = CONFIG_STATUS_1;

	EnableIOPorts(1, &Port);

	switch ((inpw(CONFIG_STATUS_1) & 0x0E00) >> 9)
	{
	case 0x00:
		*RamDac = DAC_ATI68830;
		break;
	case 0x01:
		*RamDac = DAC_SIERRA15_16;
		break;
	case 0x02:
		*RamDac = DAC_ATI68875;
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
		break;
	case 0x03:
		*RamDac = DAC_STANDARD;
		break;
	case 0x04:
		*RamDac = DAC_ATIMISC24;
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
		break;
	case 0x05:
		*RamDac = DAC_ATI68860;
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
		break;
	default:
		*RamDac = DAC_UNKNOWN;
		break;
	}

	DisableIOPorts(1, &Port);
	return;
}

void Probe_RamDac(Chipset, RamDac)
int Chipset;
int *RamDac;
{
	Byte x, y, z, u, v, oldcomm, oldpel, notcomm;
	Bool dac8, dac8now;

	*RamDac = DAC_STANDARD;
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Handle special cases.
	 */
	if (Chipset == CHIP_AL2101)
	{
	    *RamDac = DAC_ALG1101;
	    if (Width8Check())
	    {
		*RamDac |= DAC_8BIT;
	    }
	    DisableIOPorts(NUMPORTS, Ports);
	    return;
	}
	else if (SVGA_VENDOR(Chipset) == V_ATI)
	{
	    if (Chipset < CHIP_ATI68800_3)
	    {
		if (ReadBIOS(0x44, &x, 1) != 1)
		{
		    fprintf(stderr, "%s: Failed to read ATI BIOS data\n",
			    MyName);
		    DisableIOPorts(NUMPORTS, Ports);
		    return;
		}
		if (x & 0x80)
		{
		    *RamDac = DAC_ATI68830;
		    if (Width8Check())
		    {
			*RamDac |= DAC_8BIT;
		    }
		    DisableIOPorts(NUMPORTS, Ports);
		    return;
		}
	    }
	    else
	    {
		CheckMach32(RamDac);
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	}
	else if ((SVGA_VENDOR(Chipset) == V_CIRRUS) &&
		 (Chipset >= CHIP_CL5420))
	{
	    if ((Chipset == CHIP_CL5420) || (Chipset >= CHIP_CL5410))
	    {
		*RamDac = DAC_CIRRUSA;
	    }
	    else
	    {
		*RamDac = DAC_CIRRUSB;
	    }
	    if (Width8Check())
	    {
		*RamDac |= DAC_8BIT;
	    }
	    DisableIOPorts(NUMPORTS, Ports);
	    return;
	}
	else if ((SVGA_VENDOR(Chipset) == V_S3) && (Chipset >= CHIP_S3_928D))
	{
	    if (S3_Bt485Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_TVP3020Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	}

	/*
	 * Save current state.
	 */
	(void) dactocomm();
	oldcomm = inp(0x3C6);
	dactopel();
	oldpel = inp(0x3C6);

	/*
	 * Do 8-bit-wide check.
	 */
	(void) dactocomm();
	outp(0x3C6, 0x00);
	dac8 = Width8Check();
	dactopel();

	/*
	 * Check whether this DAC has a HiColor-style command register.
	 */
	notcomm = ~oldcomm;
	outp(0x3C6, notcomm);
	(void) dactocomm();
	v = inp(0x3C6);
	if (v != notcomm)
	{
	    /*
	     * Check for early-generation HiColor-style DAC.
	     */
	    if ((SetComm(0xE0) & 0xE0) != 0xE0)
	    {
		dactopel();
		x = inp(0x3C6);
		do
		{
		    /*
		     * Wait for same value twice.
		     */
		    y = x;
		    x = inp(0x3C6);
		}
		while (x != y);
		z = x;
		u = dactocomm();
		if (u != 0x8E)
		{
		    /*
		     * If command register is 0x8E, we've got an SS24;
		     */
		    y = 8;
		    do
		    {
			x = inp(0x3C6);
			y--;
		    }
		    while ((x != 0x8E) && (y != 0));
		}
		else
		{
		    x = u;
		}
		if (x == 0x8E)
		{
		    *RamDac = DAC_SS24;
		}
		else
		{
		    /*
		     * Sierra SC11486
		     */
		    *RamDac = DAC_SIERRA15;
		}
		dactopel();
	    }
	    else
	    {
		/*
		 * New generation of advanced DACs
		 */
		if ((SetComm(0x60) & 0xE0) == 0x00)
		{
		    /*
		     * AT&T 20C490/20C493
		     */
		    if ((SetComm(0x02) & 0x02) != 0x00)
		    {
			*RamDac = DAC_ATT490;
			*RamDac |= DAC_6_8_PROGRAM;
		    }
		    else
		    {
			*RamDac = DAC_ATT493;
		    }
		}
		else
		{
		    x = SetComm(oldcomm);
		    if (inp(0x3C6) == notcomm)
		    {
			if (SetComm(0xFF) != 0xFF)
			{
			    *RamDac = DAC_ACUMOS;
			}
			else
			{
			    dactocomm();
			    outp(0x3C6, (oldcomm | 0x02) & 0xFE);
			    dac8now = Width8Check();
			    if (dac8now)
			    {
				if (Width8Check())
				{
					*RamDac = DAC_ATT491;
					*RamDac |= DAC_6_8_PROGRAM;
				}
				else
				{
					*RamDac = DAC_CIRRUS24;
				}
			    }
			    else
			    {
				*RamDac = DAC_ATT492;
			    }
			}
		    }
		    else
		    {
			if (trigdac() == notcomm)
			{
				*RamDac = DAC_CIRRUS24;
			}
			else
			{
			    dactopel();
			    outp(0x3C6, 0xFF);
			    switch (trigdac())
			    {
			    case 0x44:
				*RamDac = DAC_MUSIC4870;
				break;
			    case 0x82:
				*RamDac = DAC_MUSIC4910;
				break;
			    case 0x8E:
				*RamDac = DAC_SS24;
				break;
			    default:
				if (TestDACBit(0x10,oldcomm,oldpel))
				{
					*RamDac = DAC_SIERRA24;
					*RamDac |= DAC_6_8_PROGRAM;
				}
				else if (TestDACBit(0x04,oldcomm,oldpel))
				{
					*RamDac = DAC_UNKNOWN;
				}
				else
				{
					*RamDac = DAC_SIERRA15_16;
				}
				break;
			    }
			}
		    }
		}
	    }
	    dactocomm();
	    outp(0x3C6, oldcomm);
	}
	dactopel();
	outp(0x3C6, oldpel);

	/*
	 * If no special DAC found, check for the EDSUN DAC.
	 */
	if (*RamDac == DAC_STANDARD)
	{
	    /*
	     * Write "CEGEDSUN" + mode to DAC index 0xDE (222)
	     */
	    waitforretrace();
	    outp(0x3C8, 0xDE);
	    outp(0x3C9, (Byte)'C');
	    outp(0x3C9, (Byte)'E');
	    outp(0x3C9, (Byte)'G');
	    outp(0x3C8, 0xDE);
	    outp(0x3C9, (Byte)'E');
	    outp(0x3C9, (Byte)'D');
	    outp(0x3C9, (Byte)'S');
	    outp(0x3C8, 0xDE);
	    outp(0x3C9, (Byte)'U');
	    outp(0x3C9, (Byte)'N');
	    outp(0x3C9, (Byte)'\n');
	    /*
	     * Should be in CEG mode now.
	     */
	    outp(0x3C6, 0xFF);
	    x = (inp(0x3C6) >> 4) & 0x07;
	    if (x < 0x07)
	    {
		*RamDac = DAC_EDSUN;
		waitforretrace();
		outp(0x3C8, 0xDF);
		outp(0x3C9, 0x00);
	    }
	}

	/*
	 * Remember if DAC was in 8-bit mode.
	 */
	if (dac8)
	{
		*RamDac |= DAC_8BIT;
	}

	DisableIOPorts(NUMPORTS, Ports);
	return;
}

#ifdef OLD_DAC_CODE /* { */

static void CheckATT __STDCARGS((int *));

static Bool Width8Check()
{
	int i;
	Byte save[3];
	Bool result = FALSE;

	/*
	 * Figure out whether this RAMDAC has 6-bit or 8-bit wide lookup
	 * table columns.
	 */
	outp(0x3C6, 0xFF);
	outp(0x3C7, 0x00);
	for (i=0; i < 3; i++)
	{
		save[i] = inp(0x3C9);
	}
	outp(0x3C8, 0x00);
	for (i=0; i < 3; i++)
	{
		outp(0x3C9, 0xFF);
	}
	outp(0x3C7, 0x00);
	if ((inp(0x3C9) == (Byte)0xFF) && 
	    (inp(0x3C9) == (Byte)0xFF) &&
	    (inp(0x3C9) == (Byte)0xFF))
	{
		result = TRUE;
	}
	outp(0x3C8, 0x00);
	for (i=0; i < 3; i++)
	{
		outp(0x3C9, save[i]);
	}

	return(result);
}

static void CheckATT(RamDac)
int *RamDac;
{
	Byte savecomm, tmp;

	(void)dactocomm();
	savecomm = inp(0x3C6);
	(void)dactocomm();
	outp(0x3C6, 0xE0);
	(void)dactocomm();
	if ((inp(0x3C6) & 0xE0) != 0xE0)
	{
		*RamDac = DAC_ATT497;
	}
	else
	{
		(void)dactocomm();
		outp(0x3C6, 0x60);
		(void)dactocomm();
		if ((inp(0x3C6) & 0xE0) == 0x00)
		{
			(void)dactocomm();
			tmp = inp(0x3C6);
			(void)dactocomm();
			outp(0x3C6, tmp | 0x02);
			(void)dactocomm();
			if ((inp(0x3C6) & 0x02) == 0x02)
			{
				*RamDac = DAC_ATT490;
				*RamDac |= DAC_6_8_PROGRAM;
			}
			else
			{
				*RamDac = DAC_ATT493;
			}
		}
		else
		{
			(void)dactocomm();
			tmp = inp(0x3C6);
			(void)dactocomm();
			outp(0x3C6, tmp | 0x02);
			if (Width8Check())
			{
				*RamDac = DAC_ATT491;
				*RamDac |= DAC_6_8_PROGRAM;
			}
			else
			{
				*RamDac = DAC_ATT492;
			}
		}
	}
	(void)dactocomm();
	outp(0x3C6, savecomm);
}

void Probe_RamDac(Chipset, RamDac)
int Chipset;
int *RamDac;
{
	Byte x, y, z, v, oldcommreg, oldpelreg;

	*RamDac = DAC_STANDARD;
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);

	if (Chipset == CHIP_AL2101)
	{
		*RamDac = DAC_ALG1101;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
		DisableIOPorts(NUMPORTS, Ports);
		return;
	}
	else if (SVGA_VENDOR(Chipset) == V_ATI)
	{
		if (ReadBIOS(0x44, &x, 1) != 1)
		{
			fprintf(stderr, "%s: Failed to read ATI BIOS data\n",
				MyName);
			DisableIOPorts(NUMPORTS, Ports);
			return;
		}
		if (x & 0x80)
		{
			*RamDac = DAC_ATI68830;
			if (Width8Check())
			{
				*RamDac |= DAC_8BIT;
			}
			DisableIOPorts(NUMPORTS, Ports);
			return;
		}
	}
	else if ((SVGA_VENDOR(Chipset) == V_CIRRUS) &&
		 (Chipset >= CHIP_CL5420) &&
		 (Chipset != CHIP_CL_UNKNOWN))
	{
		if (Chipset == CHIP_CL5420)
		{
			*RamDac = DAC_CIRRUSA;
		}
		else
		{
			*RamDac = DAC_CIRRUSB;
		}
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
		DisableIOPorts(NUMPORTS, Ports);
		return;
	}
	else if ((SVGA_VENDOR(Chipset) == V_S3) && (Chipset >= CHIP_S3_924))
	{
		if (S3_Bt485Check(RamDac))
		{
			DisableIOPorts(NUMPORTS, Ports);
			return;
		}
		if (S3_TVP3020Check(RamDac))
		{
			DisableIOPorts(NUMPORTS, Ports);
			return;
		}
	}
	dactopel();
	x = inp(0x3C6);
	do
	{
		y = x;
		x = inp(0x3C6);
	} while (x != y);
	z = x;
	x = dactocomm();
	y = 8;
	while ((x != 0x8E) && (y > 0))
	{
		x = inp(0x3C6);
		y--;
	}
	if (x == 0x8E)
	{
		*RamDac = DAC_SS24;
		dactopel();
	}
	else
	{
		(void)dactocomm();
		oldcommreg = inp(0x3C6);
		dactopel();
		oldpelreg = inp(0x3C6);
		x = oldcommreg ^ 0xFF;
		outp(0x3C6, x);
		(void)dactocomm();
		v = inp(0x3C6);
		if (v != x)
		{
			(void)dactocomm();
			x = oldcommreg ^ 0x60;
			outp(0x3C6, x);
			(void)dactocomm();
			v = inp(0x3C6);
			*RamDac = DAC_SIERRA15;
			if ((x & 0xE0) == (v & 0xE0))
			{
				x = inp(0x3C6);
				dactopel();
				*RamDac = DAC_SIERRA15_16;
				if (x == inp(0x3C6))
				{
					(void)dactocomm();
					outp(0x3C6, 0xFF);
					(void)dactocomm();
					if (inp(0x3C6) != 0xFF)
					{
						*RamDac = DAC_ACUMOS;
					}
					/*
					 * It's an AT&T RAMDAC; figure out
					 * which one.
					 */
					CheckATT(RamDac);
				}
			}
			else
			{
				(void)dactocomm();
				x = oldcommreg ^ 0xC0;
				outp(0x3C6, x);
				(void)dactocomm();
				v = inp(0x3C6);
				if ((x & 0xC0) == (v & 0xC0))
				{
					/*
					 * It's an AT&T RAMDAC; figure out
					 * which one.
					 */
					CheckATT(RamDac);
				}
			}
		}
		(void)dactocomm();
		outp(0x3C6, oldcommreg);
		dactopel();
		outp(0x3C6, oldpelreg);
	}
	if (Width8Check())
	{
		*RamDac |= DAC_8BIT;
	}
	DisableIOPorts(NUMPORTS, Ports);
	return;
}

#endif /* OLD_DAC_CODE } */

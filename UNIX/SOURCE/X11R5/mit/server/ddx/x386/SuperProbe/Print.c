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

/* $XFree86: mit/server/ddx/x386/SuperProbe/Print.c,v 2.14 1994/02/28 14:09:47 dawes Exp $ */

#include "Probe.h"

static CONST char *SVGA_Names[NUM_VENDORS+1][CHPS_PER_VENDOR] = 
{
/* dummy */	{ "" },	
/* Ahead */	{ "Ahead (chipset unknown)",
		  "Ahead V5000 Version A", "Ahead V5000 Version B" },
/* ATI */	{ "ATI (chipset unknown)", 
		  "ATI 18800", "ATI 18800-1",
		  "ATI 28800-2", "ATI 28800-4", "ATI 28800-5", 
		  "ATI 68800-3", "ATI 68800-6", 
		  "ATI 68800-LX", "ATI 68800-AX" },
/* AL */	{ "Avance Logic 2101" },
/* CT */	{ "Chips & Tech (chipset unknown)",
		  "Chips & Tech 82c450", "Chips & Tech 82c451", 
		  "Chips & Tech 82c452", "Chips & Tech 82c453",
		  "Chips & Tech 82c455", "Chips & Tech 82c456",
		  "Chips & Tech 82c457", "Chips & Tech F65510",
		  "Chips & Tech F65520", "Chips & Tech F65530" },
/* Cirrus */	{ "Cirrus (chipset unknown)",
		  "Cirrus CL-GD 510/520", "Cirrus CL-GD 610/620",
		  "Cirrus Video7 OEM",
		  "Cirrus/AcuMos AVGA2",
		  "Cirrus CL-GD5402", "Cirrus CL-GD5402 Rev 1",
		  "Cirrus CL-GD5420", "Cirrus CL-GD5420 Rev 1",
		  "Cirrus CL-GD5422", "Cirrus CL-GD5424", 
		  "Cirrus CL-GD5426", "Cirrus CL-GD5428",
		  "Cirrus CL-GD543x",
		  "Cirrus CL-GD6205", "Cirrus CL-GD6215",
		  "Cirrus CL-GD6225", "Cirrus CL-GD6235",
		  "Cirrus CL-GD5410",
		  "Cirrus CL-GD6410", "Cirrus CL-GD6412",
		  "Cirrus CL-GD6420A", "Cirrus CL-GD6420B" 
		  "Cirrus CL-GD6440" },
/* Compaq */	{ "Compaq (chipset unknown)",
		  "Compaq Int. Vid. Graphics Sys.",
		  "Compaq Advanced VGA", 
		  "Compaq QVision/1024", "Compaq QVision/1280",
		  "Compaq Advanced VGA, Portable" },
/* Genoa */	{ "Genoa GVGA 6100",
		  "Genoa GVGA 6200/6300", "Genoa GVGA 6400/6600" },
/* HMC */	{ "HMC HM86304" },
/* MX */	{ "MX 68000", "MX 68010" },
/* NCR */	{ "NCR (chipset unknown)",
		  "NCR 77C21", "NCR 77C22", "NCR 77C22E", "NCR 77C22E+",
		  "NCR 77C32BLT" },
/* Oak */	{ "Oak (chipset unknown)",
		  "Oak OTI037C", 
		  "Oak OTI-057", "Oak OTI-067", "Oak OTI-077",
		  "Oak OTI-083", "Oak OTI-087" },
/* Primus */	{ "Primus P2000" },
/* Realtek */	{ "Realtek (chipset unknown)",
		  "Realtek RT-3103", "Realtek RT-3105", "Realtek RT-3106" },
/* S3 */	{ "S3 (chipset unknown)",
		  "S3 86C911", "S3 86C924",
		  "S3 86C801, A or B-step", 
		  "S3 86C801, C-step", "S3 86C801, D-step", 
		  "S3 86C801i",
		  "S3 86C805, A or B-step", 
		  "S3 86C805, C-step", "S3 86C805, D-step",
		  "S3 86C805i",
		  "S3 86C928, A,B,C, or D-step", "S3 86C928, E-step",
		  "S3 86C928PCI" },
/* Trident */	{ "Trident (chipset unknown)",
		  "Trident LX8200",
		  "Trident 8800BR", "Trident 8800CS",
		  "Trident 8900B", "Trident 8900C", "Trident 8900CL/8900D",
		  "Trident 9000", "Trident 9000i",
		  "Trident LCD9100B", "Trident 9200CXr", "Trident LCD9320", 
		  "Trident 9400CXi", "Trident GUI9420" },
/* Tseng */	{ "Tseng (chipset unknown(",
		  "Tseng ET3000", "Tseng ET4000", 
		  "Tseng ET4000/W32", 
		  "Tseng ET4000/W32i", "Tseng ET4000/W32p" },
/* UMC */	{ "UMC 85c408" },
/* Video7 */	{ "Video7 (chipset unknown)",
		  "Video7 FastWrite/VRAM (HT208)", "Video7 1024i (HT208A)",
		  "Video7 VRAM II (HT208B)", "Video7 VRAM II (HT208C,D)",
		  "Video7 HT216B,C", "Video7 HT216D",
		  "Video7 HT216E", "Video7 HT216E",
		  "Video7 VEGA" },
/* WD */	{ "WD/Paradise (chipset unknown)",
		  "WD/Paradise PVGA1", "WD/Paradise 90C00", 
		  "WD/Paradise 90C10", "WD/Paradise 90C11",
		  "WD/Paradise 90C20", "WD/Paradise 90C20A",
		  "WD/Paradise 90C22", "WD/Paradise 90C24", 
		  "WD/Paradise 90C26",
		  "WD/Paradise 90C30", "WD/Paradise 90C31",
		  "WD/Paradise 90C33" },
/* Weitek */	{ "Weitek (chipset unknown)",
		  "Weitek 5086", "Weitek 5186", "Weitek 5286" },
/* Yamaha */	{ "Yamaha 6388 VPDC" },
};

static CONST char *Herc_Names[] = 
{
	"",		/* indices start at 1 */
	"Standard",
	"Plus",
	"InColor",
};

struct RamDac_Name RamDac_Names[] =
{
	{ "Unknown", "Non-standard but unknown DAC" },
	{ "Generic", "Generic 8-bit pseudo-color DAC" },
	{ "ALG1101", "Avance Logc ALG1101" },
	{ "SS2410", "Diamond SS2410 15/16/24-bit DAC" },
	{ "Sierra15", "Sierra SC1148{1,6,8} 15-bit HiColor DAC" },
	{ "Sierra16", 
	  "Sierra SC1148{2,3,4} 15-bit or SC1148{5,7,9} 15/16-bit HiColor" },
	{ "Sierra24", "Sierra 1502{5,6} 15/16/24-bit DAC" },
	{ "MU9C4870", "MUSIC MU9C4870 15/16-bit HiColor DAC" },
	{ "MU9C4910", "MUSIC MU9C4910 15/16/24-bit DAC" },
	{ "ADAC1", "AcuMos ADAC1 15/16/24-bit DAC" },
	{ "68830", "ATI 68830 15/16-bit HiColor DAC" },
	{ "68860", "ATI 68860 15/15/24-bit DAC w/pixel-mux" },
	{ "68875", "ATI-68875/Bt885/TLC34075 15/16/24-bit DAC w/pixel-mux" },
	{ "ATIMisc", 
	  "ATI Misc 3rd-party {AT&T, BT, Sierra} 15/16/24-bit DAC" },
	{ "Cirrus8", "Cirrus Logic Built-in 8-bit pseudo-color DAC" },
	{ "Cirrus24B", "Cirrus Logic Built-in 15/16/24-bit DAC" },
	{ "Cirrus24", "Cirrus Logic 15/16/24-bit DAC" },
	{ "20C490", "AT&T 20C490 15/16/24-bit DAC" },
	{ "20C491", "AT&T 20C491 15/16/24-bit DAC with gamma correction" },
	{ "20C492", "AT&T 20C492 15/16/18-bit DAC with gamma correction" },
	{ "20C493", "AT&T 20C493 15/16/18-bit DAC" },
	{ "20C497", "AT&T 20C497 24-bit wide, 8-bit pseudo-color DAC" },
	{ "Bt485", "BrookTree Bt485 24-bit TrueColor DAC w/cursor,pixel-mux" },
	{ "20C504", "AT&T 20C504 24-bit TrueColor DAC w/cursor,pixel-mux" },
	{ "20C505", "AT&T 20C505 24-bit TrueColor DAC w/cursor,pixel-mux" },
	{ "TVP3020",
	  "TI ViewPoint3020 24-bit TrueColor DAC w/cursor,pixel-mux" },
	{ "EDSUN", "EDSUN CEG DAC" },
};

static CONST char *CoProc_Names[NUM_CP_TYPES][CHPS_PER_CPTYPE] = 
{
/* 8514 */	{ "8514/A (or true clone)",
		  "ATI Mach-8", "ATI Mach-32",
		  "Chips & Technologies 82C480 (8514/A clone)" },
};

void Print_SVGA_Name(Chipset)
int Chipset;
{
	int vendor = SVGA_VENDOR(Chipset);
	int chip = SVGA_CHIP(Chipset);
	printf("\tChipset: %s\n", SVGA_Names[vendor][chip]);
	if ((!chip) && (Chip_data != 0xFF))
	{
		printf("\t\tSignature data: %02x (please report)\n", 
		       Chip_data);
	}
}

void Print_Herc_Name(Chipset)
int Chipset;
{
	int chip = HERC_CHIP(Chipset);
	printf("\tChipset: %s\n", Herc_Names[chip]);
}

void Print_RamDac_Name(RamDac)
int RamDac;
{
	printf("\tRAMDAC:  %s\n", RamDac_Names[DAC_CHIP(RamDac)].Long);
	if (RamDac & DAC_8BIT)
	{
		printf("\t\t (with 8-bit wide lookup tables)\n");
	}
	else
	{
		printf("\t\t (with 6-bit wide lookup tables ");
		printf("(or in 6-bit mode))\n");
	}
	if (RamDac & DAC_6_8_PROGRAM)
	{
		printf("\t\t (programmable for 6/8-bit wide lookup tables)\n");
	}
}

void Print_CoProc_Name(CoProc)
int CoProc;
{
	int class = COPROC_CLASS(CoProc);
	int chip = COPROC_CHIP(CoProc);
	printf("\t\tChipset: %s\n", CoProc_Names[class][chip]);
}

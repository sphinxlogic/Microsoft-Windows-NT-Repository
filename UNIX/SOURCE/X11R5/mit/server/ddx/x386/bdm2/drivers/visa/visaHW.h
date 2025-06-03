/*
 * BDM2: Banked dumb monochrome driver
 * Pascal Haible 10/93, haible@izfm.uni-stuttgart.de
 *
 * bdm2/driver/visa/visaHW.c
 * Register definitions for visa6845
 *
 * see bdm2/COPYRIGHT for copyright and disclaimers.
 */

/* Thanks to Christian (cm@htu.tuwien.ac.at) who did the first port */

/* $XFree86: mit/server/ddx/x386/bdm2/drivers/visa/visaHW.h,v 2.0 1993/12/01 12:37:38 dawes Exp $ */

#define C_STYLE_HEX_CONSTANTS
#include "visaPorts.h"

/* Memory Base Address */
#define VISA_MEM_BASE            (0xB0000L)

#define VISA_BANK_SIZE		(0x10000L)	/* 64k */

#define VISA_MAP_BASE		(VISA_MEM_BASE)

#define VISA_MAP_SIZE		(VISA_BANK_SIZE)

#define VISA_MEM_BASE_BANK1	(0)
#define VISA_MEM_BASE_BANK2	(0)		/* unused */

/* rel. to VISA_MAP_BASE */
#define VISA_BANK1_BOTTOM	(VISA_MEM_BASE_BANK1)
#define VISA_BANK1_TOP		(VISA_BANK1_BOTTOM+VISA_BANK_SIZE)
/* unused */
#define VISA_BANK2_BOTTOM	(VISA_MEM_BASE_BANK2)
#define VISA_BANK2_TOP		(VISA_BANK2_BOTTOM+VISA_BANK_SIZE)

#define VISA_SEGMENT_SIZE	(VISA_BANK_SIZE)
#define VISA_SEGMENT_SHIFT	(16)		/* 64k */
#define VISA_SEGMENT_MASK	(0xFFFFL)

#define VISA_HDISPLAY		(1280)
#define VISA_VDISPLAY		(1024)

#define VISA_SCAN_LINE_WIDTH	(2048)

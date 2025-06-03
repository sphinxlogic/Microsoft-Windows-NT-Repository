/*
 * BDM2: Banked dumb monochrome driver
 * Pascal Haible 10/93, haible@izfm.uni-stuttgart.de
 *
 * bdm2/driver/sigma/sigmaPorts.h
 * I/O Port definitions for Sigma L-View and Sigma LaserView PLUS
 *
 * see bdm2/COPYRIGHT for copyright and disclaimers.
 */

/* Thanks to Christian (cm@htu.tuwien.ac.at) who did the first driver */

/* $XFree86: mit/server/ddx/x386/bdm2/drivers/visa/visaPorts.h,v 2.0 1993/12/01 12:37:39 dawes Exp $ */

#if defined(C_STYLE_HEX_CONSTANTS)
#undef CONST
#define CONST(a)	(a)
#endif

#define VISA_INDEX	CONST(0x3B4)
#define VISA_DATA	CONST(0x3B5)
#define VISA_MODE	CONST(0x3B8)
#define VISA_STATUS	CONST(0x3BA)
#define VISA_CONFIG	CONST(0x3BF)
#define VISA_BANK	CONST(0x3BF)

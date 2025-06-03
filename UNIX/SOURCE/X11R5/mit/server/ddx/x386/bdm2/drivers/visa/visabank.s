/*
 * BDM2: Banked dumb monochrome driver
 * Pascal Haible 10/93, haible@izfm.uni-stuttgart.de
 *
 * bdm2/driver/visa/visabank.s
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

/* $XFree86: mit/server/ddx/x386/bdm2/drivers/visa/visabank.s,v 2.0 1993/12/01 12:37:40 dawes Exp $ */

/*
 * These are here the very lowlevel bankswitching routines.
 * The segment to switch to is passed via %eax. Only %eax and %edx my be used
 * without saving the original contents.
 *
 * WHY ASSEMBLY LANGUAGE ???
 *
 * These routines must be callable by other assembly routines. But I don't
 * want to have the overhead of pushing and poping the normal stack-frame.
 */

/* The visa6845 allows the following braindead banking:
 *             bank banked to  bank banked to
 *  Bit 6 5 4  B0000-B7FFF     B8000-BFFFF
 *      0 0 0   0               1
 *      0 0 1   1               2
 *      0 1 0   2               3
 *      0 1 1   3               4
 *      1 0 0   4               5
 *      1 0 1   5               6
 *      1 1 0   6               7
 *      1 1 1   7               0
 *
 * We have to use it as a single 64k bank.
 * This gives bank(BankNo) as
 * out(0x3BF,(BankNo<<5)|3)
 */

#include "assyntax.h"

#include "visaPorts.h"

        FILE("visabank.s")

        AS_BEGIN

        SEG_DATA

        SEG_TEXT

        ALIGNTEXT4
        GLOBL   GLNAME(VISASetRead)
GLNAME(VISASetRead):
	/* Shift left by 5 */
	SHL_L	(CONST(5),EAX)
	/* Set bit 1 and 0 */
	OR_L	(CONST(3),EAX)
	/* Out byte */
	MOV_L	(VISA_BANK,EDX)
	OUT_B
        RET

        ALIGNTEXT4
        GLOBL   GLNAME(VISASetWrite)
        GLOBL   GLNAME(VISASetReadWrite)
GLNAME(VISASetWrite):
GLNAME(VISASetReadWrite):
	/* Shift left by 5 */
	SHL_L	(CONST(5),EAX)
	/* Set bit 1 and 0 */
	OR_L	(CONST(3),EAX)
	/* Out byte */
	MOV_L	(VISA_BANK,EDX)
	OUT_B
        RET

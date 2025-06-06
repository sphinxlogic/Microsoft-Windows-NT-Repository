/*
 * BDM2: Banked dumb monochrome driver
 * Pascal Haible 8/93, haible@izfm.uni-stuttgart.de
 *
 * bdm2/driver/hgc1280/hgc1280bank.s
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

/* $XFree86: mit/server/ddx/x386/bdm2/drivers/hgc1280/hgc1280bank.s,v 2.2 1993/12/01 12:37:19 dawes Exp $ */

/*
 * These are here the very lowlevel VGA bankswitching routines.
 * The segment to switch to is passed via %eax. Only %eax and %edx my be used
 * without saving the original contents.
 *
 * WHY ASSEMBLY LANGUAGE ???
 *
 * These routines must be callable by other assembly routines. But I don't
 * want to have the overhead of pushing and poping the normal stack-frame.
 */

#include "assyntax.h"

#include "hgc1280Port.h"

        FILE("hgc1280bank.s")

        AS_BEGIN

        SEG_DATA

        SEG_TEXT

/* Functions for the card jumpered primary */

        ALIGNTEXT4
        GLOBL   GLNAME(HGC1280pSetRead)
GLNAME(HGC1280pSetRead):
	/* set index reg */
	PUSH_L	(EAX)
	MOV_L	(HGC_PRIM_PORT_INDEX,EDX)
	MOV_B	(HGC_REG_BANK1,AL)
	OUT_B
	/* output data */
	MOV_L	(HGC_PRIM_PORT_DATA,EDX)
	POP_L	(EAX)
	OUT_B
        RET

        ALIGNTEXT4
        GLOBL   GLNAME(HGC1280pSetWrite)
        GLOBL   GLNAME(HGC1280pSetReadWrite)
GLNAME(HGC1280pSetWrite):
GLNAME(HGC1280pSetReadWrite):
	/* set index reg */
	PUSH_L	(EAX)
	MOV_L	(HGC_PRIM_PORT_INDEX,EDX)
	MOV_B	(HGC_REG_BANK2,AL)
	OUT_B
	/* output data */
	MOV_L	(HGC_PRIM_PORT_DATA,EDX)
	POP_L	(EAX)
	OUT_B
        RET

/* Functions for the card jumpered secondary */

        ALIGNTEXT4
        GLOBL   GLNAME(HGC1280sSetRead)
GLNAME(HGC1280sSetRead):
	/* set index reg */
	PUSH_L	(EAX)
	MOV_L	(HGC_SEC_PORT_INDEX,EDX)
	MOV_B	(HGC_REG_BANK1,AL)
	OUT_B
	/* output data */
	MOV_L	(HGC_SEC_PORT_DATA,EDX)
	POP_L	(EAX)
	OUT_B
        RET

        ALIGNTEXT4
        GLOBL   GLNAME(HGC1280sSetWrite)
        GLOBL   GLNAME(HGC1280sSetReadWrite)
GLNAME(HGC1280sSetWrite):
GLNAME(HGC1280sSetReadWrite):
	/* set index reg */
	PUSH_L	(EAX)
	MOV_L	(HGC_SEC_PORT_INDEX,EDX)
	MOV_B	(HGC_REG_BANK2,AL)
	OUT_B
	/* output data */
	MOV_L	(HGC_SEC_PORT_DATA,EDX)
	POP_L	(EAX)
	OUT_B
        RET

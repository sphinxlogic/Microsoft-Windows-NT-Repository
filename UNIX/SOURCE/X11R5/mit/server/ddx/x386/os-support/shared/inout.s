/* $XFree86: mit/server/ddx/x386/os-support/shared/inout.s,v 2.0 1993/10/02 07:15:27 dawes Exp $ */

#include "assyntax.h"

/*
 *	Make i80386 io primitives available at C-level.
 */

	FILE("inout.s")
	AS_BEGIN
	SEG_TEXT

/*
 *-----------------------------------------------------------------------
 * inb ---
 *	Input one byte.
 *
 * Results:
 *      Byte in al.
 *-----------------------------------------------------------------------
 */
	GLOBL	GLNAME(inb)
GLNAME(inb):
	MOV_L	(REGOFF(4,ESP),EDX)
	SUB_L	(EAX,EAX)
	IN1_B	(DX)
	RET

/*
 *-----------------------------------------------------------------------
 * outb ---
 *	Output one byte.
 *
 * Results:
 *      None.
 *-----------------------------------------------------------------------
 */
	GLOBL	GLNAME(outb)
GLNAME(outb):
	MOV_L	(REGOFF(4,sp),EDX)
	MOV_L	(REGOFF(8,sp),EAX)
	OUT1_B	(DX)
	RET
/*
 *-----------------------------------------------------------------------
 * inw ---
 *	Input one 16-bit word.
 *
 * Results:
 *      Word in ax.
 *-----------------------------------------------------------------------
 */
	GLOBL	GLNAME(inw)
GLNAME(inw):
	MOV_L	(REGOFF(4,ESP),EDX)
	IN1_W	(DX)
	RET

/*
 *-----------------------------------------------------------------------
 * outw ---
 *	Output one 16-bit word.
 *
 * Results:
 *      None.
 *-----------------------------------------------------------------------
 */
	GLOBL	GLNAME(outw)
GLNAME(outw):
	MOV_L	(REGOFF(4,ESP),EDX)
	MOV_L	(REGOFF(8,ESP),EAX)
	OUT1_W	(DX)
	RET


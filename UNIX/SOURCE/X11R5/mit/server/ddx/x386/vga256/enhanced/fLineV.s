/* $XFree86: mit/server/ddx/x386/vga256/enhanced/fLineV.s,v 2.0 1993/08/07 04:29:55 dawes Exp $ */
/* Copyright 1992 by James Tsillas, Arlignton, Massachusetts.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation.

JAMES TSILLAS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
*/

#include "assyntax.h"
#include "vgaAsm.h"

	FILE("fLineV.s")
	AS_BEGIN

#define rop		REGOFF(8,EBP)
#define andv		BL
#define xorv		BH
#define	pdst		EDI
#define	nlwidth		ESI
#define x1		REGOFF(28,EBP)
#define y1		REGOFF(32,EBP)
#define len		REGOFF(36,EBP)

#define	len0		ECX
#define tmp		EAX

#define	GXcopy		CONST(3)
#define GXxor		CONST(6)


	SEG_TEXT
GLOBL	GLNAME(fastcfbVertS)
	ALIGNTEXT4
GLNAME(fastcfbVertS):
	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EBX)
	PUSH_L	(ESI)
	PUSH_L	(EDI)
	MOV_B	(REGOFF(12,EBP),andv)
	MOV_B	(REGOFF(16,EBP),xorv)
	MOV_L	(REGOFF(20,EBP),pdst)
	MOV_L	(REGOFF(24,EBP),nlwidth)
	SHL_L	(CONST(2),nlwidth)
	MOV_L	(y1,tmp)
	IMUL_L	(nlwidth,tmp)
	ADD_L	(x1,tmp)
	ADD_L	(tmp,pdst)
	MOV_L	(pdst,REGOFF(20,EBP))
	CMP_L	(GXcopy,rop)
	JNE	(.GXxorloop)

.GXcopyloop:
	CMP_L	(VGABASE,pdst)
	JB	(.nocheckC)

	ALIGNTEXT4ifNOP
.checkC:
	PUSH_L	(REGOFF(20,EBP))
	CALL	(GLNAME(vgaSetWrite))
	MOV_L	(tmp,pdst)
	ADD_L	(CONST(4),ESP)
	MOV_L	(CONTENT(GLNAME(vgaWriteTop)),tmp)
	SUB_L	(pdst,tmp)
	XOR_L	(EDX,EDX)
	DIV_L	(nlwidth)
	OR_L	(tmp,tmp)
	JZ	(.L3)
	CMP_L	(len,tmp)
	JAE	(.nocheckC)
	MOV_L	(tmp,len0)
	SUB_L	(len0,len)
	IMUL_L	(nlwidth,tmp)
	ADD_L	(tmp,REGOFF(20,EBP))
.L1:	MOV_B	(xorv,REGIND(pdst))
	ADD_L	(nlwidth,pdst)
	LOOP	(.L1)			/* len0 is %ecx */
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)),pdst)
	JAE	(.checkC)
.L3:	MOV_B	(xorv,REGIND(pdst))
	DEC_L	(len)
	JZ	(.allfinish)
	ADD_L	(nlwidth,REGOFF(20,EBP))
	JMP	(.checkC)

	ALIGNTEXT4
.nocheckC:
	CMP_L	(CONST(0),len)
	JZ	(.allfinish)
	MOV_L	(len,len0)
.L2:	MOV_B	(xorv,REGIND(pdst))
	ADD_L	(nlwidth,pdst)
	LOOP	(.L2)			/* len0 is %ecx */
.allfinish:
	POP_L	(EDI)
	POP_L	(ESI)
	POP_L	(EBX)
	LEAVE
	RET

.GXxorloop:
	CMP_L	(GXxor,rop)
	JNE	(.GXsetloop)
	CMP_L	(VGABASE,pdst)
	JB	(.nocheckX)

	ALIGNTEXT4ifNOP
.checkX:
	PUSH_L	(REGOFF(20,EBP))
	CALL	(GLNAME(vgaSetReadWrite))
	MOV_L	(tmp,pdst)
	ADD_L	(CONST(4),ESP)
	MOV_L	(CONTENT(GLNAME(vgaWriteTop)),tmp)
	SUB_L	(pdst,tmp)
	XOR_L	(EDX,EDX)
	DIV_L	(nlwidth)
	OR_L	(tmp,tmp)
	JZ	(.L6)
	CMP_L	(len,tmp)
	JAE	(.nocheckX)
	MOV_L	(tmp,len0)
	SUB_L	(len0,len)
	IMUL_L	(nlwidth,tmp)
	ADD_L	(tmp,REGOFF(20,EBP))
.L4:	XOR_B	(xorv,REGIND(pdst))
	ADD_L	(nlwidth,pdst)
	LOOP	(.L4)			/* len0 is %ecx */
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)),pdst)
	JAE	(.checkX)
.L6:	XOR_B	(xorv,REGIND(pdst))
	DEC_L	(len)
	JZ	(.allfinish)
	ADD_L	(nlwidth,REGOFF(20,EBP))
	JMP	(.checkX)

	ALIGNTEXT4
.nocheckX:
	CMP_L	(CONST(0),len)
	JZ	(.allfinish)
	MOV_L	(len,len0)
.L5:	XOR_B	(xorv,REGIND(pdst))
	ADD_L	(nlwidth,pdst)
	LOOP	(.L5)			/* len0 is %ecx */
	JMP	(.allfinish)

.GXsetloop:
	CMP_L	(VGABASE,pdst)
	JB	(.nocheckS)

	ALIGNTEXT4ifNOP
.checkS:
	PUSH_L	(REGOFF(20,EBP))
	CALL	(GLNAME(vgaSetReadWrite))
	MOV_L	(tmp,pdst)
	ADD_L	(CONST(4),ESP)
	MOV_L	(CONTENT(GLNAME(vgaWriteTop)),tmp)
	SUB_L	(pdst,tmp)
	XOR_L	(EDX,EDX)
	DIV_L	(nlwidth)
	OR_L	(tmp,tmp)
	JZ	(.L9)
	CMP_L	(len,tmp)
	JAE	(.nocheckS)
	MOV_L	(tmp,len0)
	SUB_L	(len0,len)
	IMUL_L	(nlwidth,tmp)
	ADD_L	(tmp,REGOFF(20,EBP))
.L7:	MOV_B	(REGIND(pdst),AL)
	AND_B	(andv,AL)
	XOR_B	(xorv,AL)
	MOV_B	(AL,REGIND(pdst))
	ADD_L	(nlwidth,pdst)
	LOOP	(.L7)			/* len0 is %ecx */
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)),pdst)
	JAE	(.checkS)
.L9:	MOV_B	(REGIND(pdst),AL)
	AND_B	(andv,AL)
	XOR_B	(xorv,AL)
	MOV_B	(AL,REGIND(pdst))
	DEC_L	(len)
	JZ	(.allfinish)
	ADD_L	(nlwidth,REGOFF(20,EBP))
	JMP	(.checkS)

	ALIGNTEXT4
.nocheckS:
	CMP_L	(CONST(0),len)
	JZ	(.allfinish)
	MOV_L	(len,len0)
.L8:	MOV_B	(REGIND(pdst),AL)
	AND_B	(andv,AL)
	XOR_B	(xorv,AL)
	MOV_B	(AL,REGIND(pdst))
	ADD_L	(nlwidth,pdst)
	LOOP	(.L8)			/* len0 is %ecx */
	JMP	(.allfinish)



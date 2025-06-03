/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: getpad.h,v 1.4 1995/02/20 02:15:13 tom Exp $ */

/*
 * Title:	getpad.h
 * Author:	Thomas E. Dickey
 * Created:	07 May 1984
 * Last update:
 *		19 Feb 1995, prototypes
 *		16 Apr 1985, moved secondary-def's here
 *
 *	Define integer codes for VT52/VT100 keypad sequences
 */

#ifndef	GETPAD_H
#define	GETPAD_H

#define	pad(x)		(256+x)

#define	padUP		pad('A')
#define	padDOWN		pad('B')
#define	padRIGHT	pad('C')
#define	padLEFT		pad('D')

#define	padPF1		pad('P')
#define	padPF2		pad('Q')
#define	padPF3		pad('R')
#define	padPF4		pad('S')

#define	pad0		pad('0')
#define	pad1		pad('1')
#define	pad2		pad('2')
#define	pad3		pad('3')
#define	pad4		pad('4')
#define	pad5		pad('5')
#define	pad6		pad('6')
#define	pad7		pad('7')
#define	pad8		pad('8')
#define	pad9		pad('9')

#define	padMINUS	pad('-')
#define	padCOMMA	pad(',')
#define	padDOT		pad('.')
#define	padENTER	pad('\n')

/* Keys assigned permanently: */
#define	GOLDKEY		padPF1
#define	HELPKEY		padPF2
#define	RETRIEVE	padMINUS

extern	int	getpad (void);
extern	int	getpad_look(int minc, int maxc);
extern	int	getpad_read(void);

#endif	/* GETPAD_H */

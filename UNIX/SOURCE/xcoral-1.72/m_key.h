/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#ifndef  _AUTOMATE_H_
#define  _AUTOMATE_H_

#define MAXREPEAT	9999
#define KEY		10
#define CONTROL		11
#define CONTROL_AND_KEY	12
#define ESCAPE		0x1b
#define CONTROL_AND_X	14
#define SPECIAL		15
#define ARROW		16

#define REPEAT		17

#define	RETURN		0x0d
#define	LINEFEED	0x0a
#define	BACKSPACE	0x08
#define	DELETE		0x7f
#define	TAB		0x09

#define Ctr_sp		0x00
#define	CtrA		0x01
#define	CtrB		0x02
#define	CtrC		0x03
#define	CtrD		0x04
#define	CtrE		0x05
#define	CtrF		0x06
#define	CtrG		0x07
#define	CtrH		0x08
#define	CtrI		0x09
#define	CtrJ		0x0a
#define	CtrK		0x0b
#define	CtrL		0x0c
#define	CtrM		0x0d
#define	CtrN		0x0e
#define	CtrO		0x0f
#define	CtrP		0x10
#define	CtrQ		0x11
#define	CtrR		0x12
#define	CtrS		0x13
#define	CtrT		0x14
#define	CtrU		0x15
#define	CtrV		0x16
#define	CtrW		0x17
#define	CtrX		0x18
#define	CtrY		0x19
#define CtrZ		0x1a

typedef struct {
	int	type;
	char	ch;
} InfosKey;


typedef struct {
	int	type;
	int 	(* fnt) ();
#if ((__osf__) && (__alpha))
	long	dest_stat;
#else
	int	dest_stat;
#endif
} Trans;

typedef struct { 
  Trans trans[8];
} ST;

#endif /* _AUTOMATE_H_ */

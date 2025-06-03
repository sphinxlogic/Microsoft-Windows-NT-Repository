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

#ifndef _BUF_H_
#define _BUF_H_

#define SIZEOF_BUFFER	50000
#define PSIZE		0x2000
#define NEW		0
#define INSERT		1

typedef struct {
	char *top;	/* Debut du buffer */
	char *l_cur;	/* debut du trou (left cursor) */
	char *r_cur;	/* fin du trou (right cursor) */
	char *bottom;	/* fin du buffer */
} Buf;

/*
 *	Public
 */
extern 	Buf *GetBuffer ( /* int size */ );
extern 	void DeleteBuffer ( /* Buf *buf */ );
extern 	int LoadFileInBuffer ( /* Buf *buf, FILE *fd, int len, int flag */ );
extern	int WriteCurrentFile ( /* Buf *buf, FILE *fd */ );
extern 	int MoveHole ( /* Buf *buf, int n */ );
extern	void HoleToRight ( /* Buf *buf */ );
extern	void HoleToLeft ( /* Buf *buf */ );
extern	void InsertNchar ( /* Buf *buf, char *s, int n */ );
extern	void DeleteNchar ( /* Buf *buf, int n */ );
extern	char *GetCurrentLine ( /* Buf *buf, int len */ );
extern	char *GetForwardLine ( /* Buf *buf, int n, int len */ );
extern	char *GetBackwardLine ( /* Buf *buf, int n, int len */ );
extern	int GetNcFromLeft ( /* Buf *buf */ );
extern	int GetNcFromRight ( /* Buf *buf */ );
extern 	int MoveToLine ( /* Buf *buf, int n */ );
extern	int GetNumberOfLineInBuf ( /* Buf *buf */ );
extern	char *DeleteLines ( /* Buf *buf, int n, int *len, int *dn */ );
extern	int GetNewLine ( /* char *t, int len */ );
extern	void GetCurrentChar ( /* Buf *buf,  char *c */ );
extern	void GetPrevChar ( /* Buf *buf, char *c */ );
extern	void ClearBuffer ( /* buf */ );

#define RightBuf(buf)	(buf -> r_cur + 1)
#define TopBuf(buf)		(buf -> top)
#define LeftBuf(buf)	(buf -> l_cur - 1)
#define BottomBuf(buf)	(buf -> bottom)

#endif /* _BUF_H_ */





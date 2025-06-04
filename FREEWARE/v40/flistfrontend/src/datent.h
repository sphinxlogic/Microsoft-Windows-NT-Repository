/* $Id: datent.h,v 1.3 1998/10/19 01:00:19 tom Exp $
 *
 * VMS date+time is stored as a 64-bit integer.  Use my own mode to avoid
 * long sequence of include-files for RMS.  Cover up diffs between VAX and AXP
 * by macros that use the address of the date.
 *
 * Created:
 *	04 Jun 1995 for port to AXP.
 * Updated:
 *	18 Oct 1998 add copyBigDate macro to fix compile problem reported by
 *		    Graeme Miller for DEC C 5.3 on VAX.
 *	21 Oct 1995 only use 'unsigned' type rather than 'unsigned long'.
 */
#ifndef DATENT_H
#define DATENT_H

#ifdef __alpha
#define isOkDate(p)    (*(p) != 0)
#define isBigDate(p)   (*(p) == -1)
#define makeBigDate(p)  *(p) = -1
#define copyBigDate(d,s) *(d) = *(s)
typedef __int64 DATENT;
#else
typedef struct	my_datent {
	unsigned date64[2];
	} DATENT;
#define isOkDate(p)    ((p)->date64[1] != 0)
#define isBigDate(p)   ((p)->date64[1] == -1)
#define makeBigDate(p)  (p)->date64[1] = -1
#define copyBigDate(d,s) memcpy(d, s, sizeof(DATENT))
#endif

#endif /* DATENT_H */

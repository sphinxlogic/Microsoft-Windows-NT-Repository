/* Combined Purdue/PurduePlus patches, level 2.0, 1/17/89 */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XFree86: mit/server/ddx/mfb/mfbbres.c,v 1.4 1993/03/27 09:00:37 dawes Exp $ */
/* $XConsortium: mfbbres.c,v 1.15 89/09/14 16:26:53 rws Exp $ */
#define BANKING_MODS
#include "X.h"
#include "misc.h"
#include "mfb.h"
#include "maskbits.h"

/* Solid bresenham line */
/* NOTES
   e2 is used less often than e1, so it's not in a register
*/

mfbBresS(rop, addrl, nlwidth, signdx, signdy, axis, x1, y1, e, e1, e2, len)
int rop;		/* a reduced rasterop */
int *addrl;		/* pointer to base of bitmap */
int nlwidth;		/* width in longwords of bitmap */
int signdx, signdy;	/* signs of directions */
int axis;		/* major axis (Y_AXIS or X_AXIS) */
int x1, y1;		/* initial point */
register int e;		/* error accumulator */
register int e1;	/* bresenham increments */
int e2;
int len;		/* length of line */
{
    register int yinc;	/* increment to next scanline, in bytes */
    register unsigned char *addrb;		/* bitmask long pointer 
						 * cast to char pointer */
    register unsigned int bit;	/* current bit being set/cleared/etc.  */
    unsigned int leftbit = mask[0]; /* leftmost bit to process in new word */
    unsigned int rightbit = mask[31]; /* rightmost bit to process in new word */

    register int e3 = e2-e1;
    unsigned int	tmp;
#ifdef BANKED_MFB
    unsigned char Oflag, Uflag;
#endif

    if (!len)
	return;

    BANK_FLAG(addrl);

    /* point to longword containing first point */
    addrb = (unsigned char *)(addrl + (y1 * nlwidth) + (x1 >> 5));
    SETRW(addrb);
    yinc = signdy * nlwidth * 4;                /* 4 == sizeof(int) */
#ifdef BANKED_MFB
    Oflag = (vgaWriteFlag && (yinc > 0));
    Uflag = (vgaWriteFlag && (yinc < 0));
#endif
    
    e = e-e1;			/* to make looping easier */
    bit = mask[x1 & 31];

    if (rop == RROP_BLACK)
    {
        if (axis == X_AXIS)
        {
	    if (signdx > 0)
	    {
		tmp = *MAPR(addrb);
		for (;;)
		{ 
		    tmp &= ~bit;
		    if (!--len)
			break;
		    bit = SCRRIGHT(bit,1);
		    e += e1;
 		    if (e >= 0)
		    {
			*MAPW(addrb) = tmp;
			addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
			e += e3;
			if (!bit)
			{
			    bit = leftbit;
			    addrb += 4; CHECKRWO(addrb);
			}
			tmp = *MAPR(addrb);
		    }
		    else if (!bit)
 		    {
			*MAPW(addrb) = tmp;
			bit = leftbit;
			addrb += 4; CHECKRWO(addrb);
			tmp = *MAPR(addrb);
		    }
		}
		*MAPW(addrb) = tmp;
	    }
	    else
	    {
		tmp = *MAPR(addrb);
		for (;;)
		{ 
		    tmp &= ~bit;
		    if (!--len)
			break;
		    e += e1;
		    bit = SCRLEFT(bit,1);
		    if (e >= 0)
		    {
			*MAPW(addrb) = tmp;
			addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
			e += e3;
			if (!bit)
			{
			    bit = rightbit;
			    addrb -= 4; CHECKRWU(addrb);
			}
			tmp = *MAPR(addrb);
		    }
		    else if (!bit)
 		    {
			*MAPW(addrb) = tmp;
			bit = rightbit;
			addrb -= 4; CHECKRWU(addrb);
			tmp = *MAPR(addrb);
		    }
		}
		*MAPW(addrb) = tmp;
	    }
        } /* if X_AXIS */
        else
        {
	    if (signdx > 0)
	    {
		while(len--)
		{
		    *MAPRW(addrb) &= ~bit;
		    e += e1;
		    if (e >= 0)
		    {
			bit = SCRRIGHT(bit,1);
			if (!bit) { bit = leftbit;addrb += 4; CHECKRWO(addrb); }
			e += e3;
		    }
		    addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
		}
	    }
	    else
	    {
		while(len--)
		{
		    *MAPRW(addrb) &= ~bit;
		    e += e1;
		    if (e >= 0)
		    {
			bit = SCRLEFT(bit,1);
			if (!bit) { bit = rightbit;addrb -= 4; CHECKRWU(addrb);}
			e += e3;
		    }
		    addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
		}
	    }
        } /* else Y_AXIS */
    } 
    else if (rop == RROP_WHITE)
    {
        if (axis == X_AXIS)
        {
	    if (signdx > 0)
	    {
		tmp = *MAPR(addrb);
		for (;;)
		{
		    tmp |= bit;
		    if (!--len)
			break;
		    e += e1;
		    bit = SCRRIGHT(bit,1);
		    if (e >= 0)
		    {
			*MAPW(addrb) = tmp;
			addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
			e += e3;
			if (!bit)
			{
			    bit = leftbit;
			    addrb += 4; CHECKRWO(addrb);
			}
			tmp = *MAPR(addrb);
		    }
		    else if (!bit)
 		    {
			*MAPW(addrb) = tmp;
			bit = leftbit;
			addrb += 4; CHECKRWO(addrb);
			tmp = *MAPR(addrb);
		    }
		}
		*MAPW(addrb) = tmp;
	    }
	    else
	    {
		tmp = *MAPR(addrb);
		for (;;)
		{
		    tmp |= bit;
		    if (!--len)
			break;
		    e += e1;
		    bit = SCRLEFT(bit,1);
		    if (e >= 0)
		    {
			*MAPW(addrb) = tmp;
			addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
			e += e3;
			if (!bit)
			{
			    bit = rightbit;
			    addrb -= 4; CHECKRWU(addrb);
			}
			tmp = *MAPR(addrb);
		    }
		    else if (!bit)
		    {
			*MAPW(addrb) = tmp;
			bit = rightbit;
			addrb -= 4; CHECKRWU(addrb);
			tmp = *MAPR(addrb);
		    }
		}
		*MAPW(addrb) = tmp;
	    }
        } /* if X_AXIS */
        else
        {
	    if (signdx > 0)
	    {
		while(len--)
		{
		    *MAPRW(addrb) |= bit;
		    e += e1;
		    if (e >= 0)
		    {
			bit = SCRRIGHT(bit,1);
			if (!bit) { bit = leftbit;addrb += 4; CHECKRWO(addrb); }
			e += e3;
		    }
		    addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
		}
	    }
	    else
	    {
		while(len--)
		{
		    *MAPRW(addrb) |= bit;
		    e += e1;
		    if (e >= 0)
		    {
			bit = SCRLEFT(bit,1);
			if (!bit) { bit = rightbit;addrb -= 4; CHECKRWU(addrb);}
			e += e3;
		    }
		    addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
		}
	    }
        } /* else Y_AXIS */
    }
    else if (rop == RROP_INVERT)
    {
        if (axis == X_AXIS)
        {
	    if (signdx > 0)
	    {
		while(len--)
		{
		    *MAPRW(addrb) ^= bit;
		    e += e1;
		    if (e >= 0)
		    {
			addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
			e += e3;
		    }
		    bit = SCRRIGHT(bit,1);
		    if (!bit) { bit = leftbit;addrb += 4; CHECKRWO(addrb); }
		}
	    }
	    else
	    {
		while(len--)
		{
		    *MAPRW(addrb) ^= bit;
		    e += e1;
		    if (e >= 0)
		    {
			addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
			e += e3;
		    }
		    bit = SCRLEFT(bit,1);
		    if (!bit) { bit = rightbit;addrb -= 4; CHECKRWU(addrb); }
		}
	    }
        } /* if X_AXIS */
        else
        {
	    if (signdx > 0)
	    {
		while(len--)
		{
		    *MAPRW(addrb) ^= bit;
		    e += e1;
		    if (e >= 0)
		    {
			bit = SCRRIGHT(bit,1);
			if (!bit) { bit = leftbit;addrb += 4; CHECKRWO(addrb); }
			e += e3;
		    }
		    addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
		}
	    }
	    else
	    {
		while(len--)
		{
		    *MAPRW(addrb) ^= bit;
		    e += e1;
		    if (e >= 0)
		    {
			bit = SCRLEFT(bit,1);
			if (!bit) { bit = rightbit;addrb -= 4; CHECKRWU(addrb);}
			e += e3;
		    }
		    addrb += yinc; CHECKRWD(Oflag, Uflag, addrb);
		}
	    }
        } /* else Y_AXIS */
    }
} 

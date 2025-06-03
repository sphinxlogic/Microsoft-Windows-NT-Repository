/*
 * mfb copy area
 */

/*
Copyright 1989 by the Massachusetts Institute of Technology

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of M.I.T. not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission.  M.I.T. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

Author: Keith Packard

*/
/* $XFree86: mit/server/ddx/x386/bdm2/mfb.banked/mfbbltc.c,v 2.0 1993/08/30 15:23:01 dawes Exp $ */
/* $XConsortium: mfbblt.c,v 1.5 91/01/27 13:02:19 keith Exp $ */

#define BANKING_MODS
#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"mfb.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"maskbits.h"
#include	"fastblt.h"
#include	"mergerop.h"

#ifdef ONEBANK
int (*MROP_NAME(ourbdm_mfbDoBitblt))();

MROP_NAME(oneBankbdm_mfbDoBitblt)(pSrc, pDst, alu, prgnDst, pptSrc)
#else /* ONEBANK */
MROP_NAME(mfbDoBitblt)(pSrc, pDst, alu, prgnDst, pptSrc)
#endif /* ONEBANK */
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
{
    unsigned int *psrcBase, *pdstBase;	
				/* start of src and dst bitmaps */
    int widthSrc, widthDst;	/* add to get to same position in next line */

    BoxPtr pbox;
    int nbox;

    BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew1, pboxNew2;
				/* temporaries for shuffling rectangles */
    DDXPointPtr pptTmp, pptNew1, pptNew2;
				/* shuffling boxes entails shuffling the
				   source points too */
    int w, h;
    int xdir;			/* 1 = left right, -1 = right left/ */
    int ydir;			/* 1 = top down, -1 = bottom up */

    unsigned int *psrcLine, *pdstLine;	
				/* pointers to line with current src and dst */
    register unsigned int *psrc;/* pointer to current src longword */
    register unsigned int *pdst;/* pointer to current dst longword */

    MROP_DECLARE_REG()

				/* following used for looping through a line */
    unsigned int startmask, endmask;	/* masks for writing ends of dst */
    int nlMiddle;		/* whole longwords in dst */
    int xoffSrc, xoffDst;
    register int leftShift, rightShift;
    register unsigned int bits;
    register unsigned int bits1;
    register int nl;		/* temp copy of nlMiddle */

				/* place to store full source word */
    int nstart;			/* number of ragged bits at start of dst */
    int nend;			/* number of ragged bits at end of dst */
    int srcStartOver;		/* pulling nstart bits from src
				   overflows into the next word? */
    int careful;
    int tmpSrc;

#if defined(ONEBANK) && defined(LINE_BANK_ALIGNED)
/* Don't want to bank for each long => copy them in a buffer should
 * be faster */
#define LINEBUFFERLENGTH 256
	static unsigned int linebuffer[LINEBUFFERLENGTH];
	static unsigned int linebuffer1[LINEBUFFERLENGTH];
	/* this is enough for 2048 pixels */
#endif

    MROP_INITIALIZE(alu,0);

    if (pSrc->type == DRAWABLE_WINDOW)
    {
	psrcBase = (unsigned int *)
		(((PixmapPtr)(pSrc->pScreen->devPrivate))->devPrivate.ptr);
	widthSrc = (int)
		   ((PixmapPtr)(pSrc->pScreen->devPrivate))->devKind
		    >> 2;
    }
    else
    {
	psrcBase = (unsigned int *)(((PixmapPtr)pSrc)->devPrivate.ptr);
	widthSrc = (int)(((PixmapPtr)pSrc)->devKind) >> 2;
    }

    if (pDst->type == DRAWABLE_WINDOW)
    {
	pdstBase = (unsigned int *)
		(((PixmapPtr)(pDst->pScreen->devPrivate))->devPrivate.ptr);
	widthDst = (int)
		   ((PixmapPtr)(pDst->pScreen->devPrivate))->devKind
		    >> 2;
    }
    else
    {
	pdstBase = (unsigned int *)(((PixmapPtr)pDst)->devPrivate.ptr);
	widthDst = (int)(((PixmapPtr)pDst)->devKind) >> 2;
    }

    BANK_FLAG_BOTH(psrcBase, pdstBase)

    /* XXX we have to err on the side of safety when both are windows,
     * because we don't know if IncludeInferiors is being used.
     */
    careful = ((pSrc == pDst) ||
	       ((pSrc->type == DRAWABLE_WINDOW) &&
		(pDst->type == DRAWABLE_WINDOW)));

    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    pboxNew1 = NULL;
    pptNew1 = NULL;
    pboxNew2 = NULL;
    pptNew2 = NULL;
    if (careful && (pptSrc->y < pbox->y1))
    {
        /* walk source botttom to top */
	ydir = -1;
	widthSrc = -widthSrc;
	widthDst = -widthDst;

	if (nbox > 1)
	{
	    /* keep ordering in each band, reverse order of bands */
	    pboxNew1 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    if(!pboxNew1)
		return;
	    pptNew1 = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	    if(!pptNew1)
	    {
	        DEALLOCATE_LOCAL(pboxNew1);
	        return;
	    }
	    pboxBase = pboxNext = pbox+nbox-1;
	    while (pboxBase >= pbox)
	    {
	        while ((pboxNext >= pbox) &&
		       (pboxBase->y1 == pboxNext->y1))
		    pboxNext--;
	        pboxTmp = pboxNext+1;
	        pptTmp = pptSrc + (pboxTmp - pbox);
	        while (pboxTmp <= pboxBase)
	        {
		    *pboxNew1++ = *pboxTmp++;
		    *pptNew1++ = *pptTmp++;
	        }
	        pboxBase = pboxNext;
	    }
	    pboxNew1 -= nbox;
	    pbox = pboxNew1;
	    pptNew1 -= nbox;
	    pptSrc = pptNew1;
        }
    }
    else
    {
	/* walk source top to bottom */
	ydir = 1;
    }

    if (careful && (pptSrc->x < pbox->x1))
    {
	/* walk source right to left */
        xdir = -1;

	if (nbox > 1)
	{
	    /* reverse order of rects in each band */
	    pboxNew2 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    pptNew2 = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	    if(!pboxNew2 || !pptNew2)
	    {
		if (pptNew2) DEALLOCATE_LOCAL(pptNew2);
		if (pboxNew2) DEALLOCATE_LOCAL(pboxNew2);
		if (pboxNew1)
		{
		    DEALLOCATE_LOCAL(pptNew1);
		    DEALLOCATE_LOCAL(pboxNew1);
		}
	        return;
	    }
	    pboxBase = pboxNext = pbox;
	    while (pboxBase < pbox+nbox)
	    {
	        while ((pboxNext < pbox+nbox) &&
		       (pboxNext->y1 == pboxBase->y1))
		    pboxNext++;
	        pboxTmp = pboxNext;
	        pptTmp = pptSrc + (pboxTmp - pbox);
	        while (pboxTmp != pboxBase)
	        {
		    *pboxNew2++ = *--pboxTmp;
		    *pptNew2++ = *--pptTmp;
	        }
	        pboxBase = pboxNext;
	    }
	    pboxNew2 -= nbox;
	    pbox = pboxNew2;
	    pptNew2 -= nbox;
	    pptSrc = pptNew2;
	}
    }
    else
    {
	/* walk source left to right */
        xdir = 1;
    }

    while(nbox--)
    {
	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;

	if (ydir == -1) /* start at last scanline of rectangle */
	{
	    psrcLine = psrcBase + ((pptSrc->y+h-1) * -widthSrc);
	    pdstLine = pdstBase + ((pbox->y2-1) * -widthDst);
	}
	else /* start at first scanline */
	{
	    psrcLine = psrcBase + (pptSrc->y * widthSrc);
	    pdstLine = pdstBase + (pbox->y1 * widthDst);
	}
	if ((pbox->x1 & PIM) + w <= PPW)
	{
	    maskpartialbits (pbox->x1, w, startmask);
	    endmask = 0;
	    nlMiddle = 0;
	}
	else
	{
	    maskbits(pbox->x1, w, startmask, endmask, nlMiddle);
	}
	if (xdir == 1)
	{
	    xoffSrc = pptSrc->x & PIM;
	    xoffDst = pbox->x1 & PIM;
	    pdstLine += (pbox->x1 >> PWSH);
	    psrcLine += (pptSrc->x >> PWSH);
#ifdef DO_UNALIGNED_BITBLT
	    nl = xoffSrc - xoffDst;
	    psrcLine = (unsigned int *)
			(((unsigned char *) psrcLine) + nl);
#else
	    if (xoffSrc == xoffDst)
#endif
	    {
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
                    SETW(pdst);
                    SETR(psrc);
		    pdstLine += widthDst;
		    psrcLine += widthSrc;
		    if (startmask)
		    {
                        bits = *MAPR(psrc);
                        PUSHR();
			*MAPW(pdst) = MROP_MASK(bits, *MAPR(pdst), startmask);
			pdst++; CHECKWO(pdst);
                        POPR();
			psrc++; CHECKRO(psrc);
		    }
		    nl = nlMiddle;

#ifdef LARGE_INSTRUCTION_CACHE
#ifdef FAST_CONSTANT_OFFSET_MODE

		    psrc += nl & (UNROLL-1);
		    pdst += nl & (UNROLL-1);

#define BodyOdd(n) *MAPW(&(pdst[-n])) = MROP_SOLID (*MAPR(&(psrc[-n])), *MAPR(&(pdst[-n])));
#define BodyEven(n) *MAPW(&(pdst[-n])) = MROP_SOLID (*MAPR(&(psrc[-n])), *MAPR(&(pdst[-n])));

#define LoopReset \
pdst += UNROLL; \
psrc += UNROLL;

#else /* FAST_CONSTANT_OFFSET_MODE */

#define BodyOdd(n)  *MAPW(pdst) = MROP_SOLID (*MAPR(psrc), *MAPR(pdst)); pdst++; psrc++
#define BodyEven(n) BodyOdd(n)

#define LoopReset   ;

#endif /* FAST_CONSTANT_OFFSET_MODE */
		    PackedLoop

#undef BodyOdd
#undef BodyEven
#undef LoopReset

#else /* LARGE_INSTRUCTION_CACHE */
#ifdef NOTDEF
		    /* you'd think this would be faster --
		     * a single instruction instead of 6
		     * but measurements show it to be ~15% slower
		     */
		    while ((nl -= 6) >= 0)
		    {
			asm ("moveml %1+,#0x0c0f;moveml#0x0c0f,%0"
			     : "=m" (*(char *)MAPW(pdst))
			     : "m" (*(char *)MAPR(psrc))
			     : "d0", "d1", "d2", "d3",
			       "a2", "a3");
			pdst += 6;
		    }
		    nl += 6;
		    while (nl--) {
			*MAPW(pdst) = *MAPR(psrc); pdst++; psrc++;
		      }
#endif /* NOTDEF */
#ifdef ONEBANK
#ifdef LINE_BANK_ALIGNED
/* xoffSrc == xoffDst, ONEBANK, LINE_BANK_ALIGNED */
		    if (nl<LINEBUFFERLENGTH) {
			while (nl>0) {
				linebuffer[nl] = *MAPR(psrc);
				psrc++;
				nl--;
			};
			PUSHR();
			nl =  nlMiddle;
			while (nl>0) {
				*MAPW(pdst) = MROP_SOLID(linebuffer[nl], *MAPR(pdst));
				pdst++;
				nl--;
			};
			POPR();
		    } else /* fall through to original stuff */
#endif /* LINE_BANK_ALIGNED */
		    DuffL(nl, label1,
                            bits = *MAPR(psrc);
                            PUSHR();
			    *MAPW(pdst) = MROP_SOLID (bits, *MAPR(pdst));
			    pdst++; CHECKWO(pdst);
                            POPR();
			    psrc++; CHECKRO(psrc)
                    )
#else /* ONEBANK */
#ifdef LINE_BANK_ALIGNED
		    DuffL(nl, label1,
                            *MAPW(pdst) = MROP_SOLID(*MAPR(psrc),/**/);
			    pdst++;
			    psrc++;
                    )
#else /* LINE_BANK_ALIGNED */
		    DuffL(nl, label1,
                            *MAPW(pdst) = MROP_SOLID(*MAPR(psrc),/**/);
			    pdst++; CHECKWO(pdst);
			    psrc++; CHECKRO(psrc)
                    )
#endif /* LINE_BANK_ALIGNED */
#endif /* ONEBANK */
#endif /* LARGE_INSTRUCTION_CACHE */

		    if (endmask)
                    {
			bits = *MAPR(psrc);
			PUSHR();
			*MAPW(pdst) = MROP_MASK(bits, *MAPR(pdst), endmask);
			POPR();
		    }
		} /* while (h--) */
	    }
#ifndef DO_UNALIGNED_BITBLT
	    else /* xoffSrc == xoffDst */
	    {
		if (xoffSrc > xoffDst)
		{
		    leftShift = (xoffSrc - xoffDst) << (5 - PWSH);
		    rightShift = 32 - leftShift;
		}
		else
		{
		    rightShift = (xoffDst - xoffSrc) << (5 - PWSH);
		    leftShift = 32 - rightShift;
		}
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
                    SETW(pdst);
                    SETR(psrc);
		    pdstLine += widthDst;
		    psrcLine += widthSrc;
		    bits = 0;
		    if (xoffSrc > xoffDst) {
			bits = *MAPR(psrc); psrc++; CHECKRO(psrc);
		    }
		    if (startmask)
		    {
			bits1 = BitLeft(bits,leftShift);
			bits = *MAPR(psrc); psrc++; CHECKRO(psrc);
			bits1 |= BitRight(bits,rightShift);
			PUSHR();
			*MAPW(pdst) = MROP_MASK(bits1, *MAPR(pdst), startmask);
			pdst++; CHECKWO(pdst);
			POPR();
		    }
		    nl = nlMiddle;

#ifdef LARGE_INSTRUCTION_CACHE
		    bits1 = bits;

#ifdef FAST_CONSTANT_OFFSET_MODE

		    psrc += nl & (UNROLL-1);
		    pdst += nl & (UNROLL-1);

#define BodyOdd(n) \
bits = *MAPR(&(psrc[-n])); \
*MAPW(&(pdst[-n])) = MROP_SOLID(BitLeft(bits1, leftShift) | BitRight(bits, rightShift), *MAPR(&(pdst[-n])));

#define BodyEven(n) \
bits1 = *MAPR(&(psrc[-n])); \
*MAPW(&(pdst[-n])) = MROP_SOLID(BitLeft(bits, leftShift) | BitRight(bits1, rightShift), *MAPR(&(pdst[-n])));

#define LoopReset \
pdst += UNROLL; \
psrc += UNROLL;

#else /* !FAST_CONSTANT_OFFSET_MODE */

#define BodyOdd(n) \
bits = *MAPR(psrc); psrc++; \
*MAPW(pdst) = MROP_SOLID(BitLeft(bits1, leftShift) | BitRight(bits, rightShift), *MAPR(pdst)); \
pdst++;
		   
#define BodyEven(n) \
bits1 = *MAPR(psrc); psrc++; \
*MAPW(pdst) = MROP_SOLID(BitLeft(bits, leftShift) | BitRight(bits1, rightShift), *MAPR(pdst)); \
pdst++;

#define LoopReset   ;

#endif	/* !FAST_CONSTANT_OFFSET_MODE */

		    PackedLoop

#undef BodyOdd
#undef BodyEven
#undef LoopReset

#else /* LARGE_INSTRUCTION_CACHE */
#ifdef ONEBANK
#ifdef LINE_BANK_ALIGNED
		    DuffL (nl,label2,
			bits1 = BitLeft(bits, leftShift);
			bits = *MAPR(psrc); psrc++;
			PUSHR();
			*MAPW(pdst) = MROP_SOLID (bits1 | BitRight(bits,
                                                     rightShift), *MAPR(pdst));
			pdst++;
			POPR();
		    )
#else /* LINE_BANK_ALIGNED */
		    DuffL (nl,label2,
			bits1 = BitLeft(bits, leftShift);
			bits = *MAPR(psrc); psrc++; CHECKRO(psrc);
			PUSHR();
			*MAPW(pdst) = MROP_SOLID (bits1 | BitRight(bits,
                                                     rightShift), *MAPR(pdst));
			pdst++; CHECKWO(pdst);
			POPR();
		    )
#endif /* LINE_BANK_ALIGNED */
#else /* ONEBANK */
#ifdef LINE_BANK_ALIGNED
		    DuffL (nl,label2,
			bits1 = BitLeft(bits, leftShift);
			bits = *MAPR(psrc); psrc++;
			*MAPW(pdst) = MROP_SOLID (bits1 | BitRight(bits,
                                                     rightShift), /**/);
			pdst++;
		    )
#else /* LINE_BANK_ALIGNED */
		    DuffL (nl,label2,
			bits1 = BitLeft(bits, leftShift);
			bits = *MAPR(psrc); psrc++; CHECKRO(psrc);
			*MAPW(pdst) = MROP_SOLID (bits1 | BitRight(bits,
                                                     rightShift), /**/);
			pdst++; CHECKWO(pdst);
		    )
#endif /* LINE_BANK_ALIGNED */
#endif /* ONEBANK */
#endif /* LARGE_INSTRUCTION_CACHE */

		    if (endmask)
		    {
			bits1 = BitLeft(bits, leftShift);
			if (BitLeft(endmask, rightShift))
			{
			    bits = *MAPR(psrc);
			    bits1 |= BitRight(bits, rightShift);
			}
			PUSHR();
			*MAPW(pdst) = MROP_MASK (bits1, *MAPR(pdst), endmask);
			POPR();
		    }
		}
	    }
#endif /* DO_UNALIGNED_BITBLT */
	}
	else	/* xdir == -1 */
	{
	    xoffSrc = (pptSrc->x + w - 1) & PIM;
	    xoffDst = (pbox->x2 - 1) & PIM;
	    pdstLine += ((pbox->x2-1) >> PWSH) + 1;
	    psrcLine += ((pptSrc->x+w - 1) >> PWSH) + 1;
#ifdef DO_UNALIGNED_BITBLT
	    nl = xoffSrc - xoffDst;
	    psrcLine = (unsigned int *)
			(((unsigned char *) psrcLine) + nl);
#else
	    if (xoffSrc == xoffDst)
#endif
	    {
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
                    SETW(pdst);
                    SETR(psrc);
		    pdstLine += widthDst;
		    psrcLine += widthSrc;
		    if (endmask)
		    {
			psrc--; CHECKRU(psrc);
			bits = *MAPR(psrc);
			PUSHR();
			pdst--; CHECKRWU(pdst);
			*MAPW(pdst) = MROP_MASK (bits, *MAPR(pdst), endmask);
			POPR();
		    }
		    nl = nlMiddle;

#ifdef LARGE_INSTRUCTION_CACHE
#ifdef FAST_CONSTANT_OFFSET_MODE
		    psrc -= nl & (UNROLL - 1);
		    pdst -= nl & (UNROLL - 1);

#define BodyOdd(n) *MAPW(&(pdst[n-1])) = MROP_SOLID (*MAPR(&(psrc[n-1])), *MAPR(&(pdst[n-1])));

#define BodyEven(n) BodyOdd(n)

#define LoopReset \
pdst -= UNROLL;\
psrc -= UNROLL;

#else /* FAST_CONSTANT_OFFSET_MODE */

#define BodyOdd(n)  --pdst; --psrc; *MAPW(pdst) = MROP_SOLID(*MAPR(psrc), *MAPR(pdst));
#define BodyEven(n) BodyOdd(n)
#define LoopReset   ;

#endif /* FAST_CONSTANT_OFFSET_MODE */
		    PackedLoop

#undef BodyOdd
#undef BodyEven
#undef LoopReset

#else /* LARGE_INSTRUCTION_CACHE */
#ifdef ONEBANK
#ifdef LINE_BANK_ALIGNED
		    DuffL(nl,label3,
			  --psrc;
			  bits = *MAPR(psrc);
			  PUSHR();
			  --pdst;
                          *MAPW(pdst) = MROP_SOLID (bits, *MAPR(pdst));
			  POPR();
                    )
#else /* LINE_BANK_ALIGNED */
		    DuffL(nl,label3,
			  --psrc; CHECKRU(psrc);
			  bits = *MAPR(psrc);
			  PUSHR();
			  --pdst; CHECKRWU(pdst);
                          *MAPW(pdst) = MROP_SOLID (bits, *MAPR(pdst));
			  POPR();
                    )
#endif /* LINE_BANK_ALIGNED */
#else /* ONEBANK */
#ifdef LINE_BANK_ALIGNED
		    DuffL(nl,label3,
			  --psrc;
			  --pdst;
                          *MAPW(pdst) = MROP_SOLID (*MAPR(psrc), /**/);
                    )
#else /* LINE_BANK_ALIGNED */
		    DuffL(nl,label3,
			  --psrc; CHECKRU(psrc);
			  --pdst; CHECKWU(pdst);
                          *MAPW(pdst) = MROP_SOLID (*MAPR(psrc), /**/);
                    )
#endif /* LINE_BANK_ALIGNED */
#endif /* ONEBANK */
#endif /* LARGE_INSTRUCTION_CACHE */

		    if (startmask)
		    {
			--psrc; CHECKRU(psrc);
			bits = *MAPR(psrc);
			PUSHR();
			--pdst; CHECKRWU(pdst);
			*MAPW(pdst) = MROP_MASK(bits, *MAPR(pdst), startmask);
			POPR();
		    }
		}
	    }
#ifndef DO_UNALIGNED_BITBLT
	    else /* xoffSrc == xoffDst */
	    {
		if (xoffDst > xoffSrc)
		{
		    rightShift = (xoffDst - xoffSrc) << (5 - PWSH);
		    leftShift = 32 - rightShift;
		}
		else
		{
		    leftShift = (xoffSrc - xoffDst) << (5 - PWSH);
		    rightShift = 32 - leftShift;
		}
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
                    SETW(pdst);
                    SETR(psrc);
		    pdstLine += widthDst;
		    psrcLine += widthSrc;
		    bits = 0;
		    if (xoffDst > xoffSrc) {
			--psrc; CHECKRU(psrc); bits = *MAPR(psrc);
		    }
		    if (endmask)
		    {
			bits1 = BitRight(bits, rightShift);
			--psrc; CHECKRU(psrc); bits = *MAPR(psrc);
			bits1 |= BitLeft(bits, leftShift);
			PUSHR();
			pdst--; CHECKRWU(pdst);
			*MAPW(pdst) = MROP_MASK(bits1, *MAPR(pdst), endmask);
			POPR();
		    }
		    nl = nlMiddle;

#ifdef LARGE_INSTRUCTION_CACHE
		    bits1 = bits;
#ifdef FAST_CONSTANT_OFFSET_MODE
		    psrc -= nl & (UNROLL - 1);
		    pdst -= nl & (UNROLL - 1);

#define BodyOdd(n) \
bits = *MAPR(&(psrc[n-1])); \
*MAPW(&(pdst[n-1])) = MROP_SOLID(BitRight(bits1, rightShift) | BitLeft(bits, leftShift),*MAPR(&(pdst[n-1])));

#define BodyEven(n) \
bits1 = *MAPR(&(psrc[n-1])); \
*MAPW(&(pdst[n-1])) = MROP_SOLID(BitRight(bits, rightShift) | BitLeft(bits1, leftShift),*MAPR(&(pdst[n-1])));

#define LoopReset \
pdst -= UNROLL; \
psrc -= UNROLL;

#else /* FAST_CONSTANT_OFFSET_MODE */

#define BodyOdd(n) \
--psrc; bits = *MAPR(psrc); \
--pdst; *MAPW(pdst) = MROP_SOLID(BitRight(bits1, rightShift) | BitLeft(bits, leftShift),*MAPR(pdst));

#define BodyEven(n) \
--psrc; bits1 = *MAPR(psrc); \
--pdst; *MAPW(pdst) = MROP_SOLID(BitRight(bits, rightShift) | BitLeft(bits1, leftShift),*MAPR(pdst));

#define LoopReset   ;

#endif /* FAST_CONSTANT_OFFSET_MODE */

		    PackedLoop

#undef BodyOdd
#undef BodyEven
#undef LoopReset

#else /* LARGE_INSTRUCTION_CACHE */
#ifdef ONEBANK
#ifdef LINE_BANK_ALIGNED
		    DuffL (nl, label4,
			bits1 = BitRight(bits, rightShift);
			--psrc; bits = *MAPR(psrc);
			PUSHR();
			--pdst;
                        *MAPW(pdst) = MROP_SOLID(bits1 | BitLeft(bits,
                                                    leftShift),*MAPR(pdst));
			POPR();
		    )
#else /* LINE_BANK_ALIGNED */
		    DuffL (nl, label4,
			bits1 = BitRight(bits, rightShift);
			--psrc; CHECKRU(psrc); bits = *MAPR(psrc);
			PUSHR();
			--pdst;	CHECKRWU(pdst);
                        *MAPW(pdst) = MROP_SOLID(bits1 | BitLeft(bits,
                                                    leftShift),*MAPR(pdst));
			POPR();
		    )
#endif /* LINE_BANK_ALIGNED */
#else /* ONEBANK */
#ifdef LINE_BANK_ALIGNED
		    DuffL (nl, label4,
			bits1 = BitRight(bits, rightShift);
			--psrc; bits = *MAPR(psrc);
			--pdst;
                        *MAPW(pdst) = MROP_SOLID(bits1 | BitLeft(bits,
                                                    leftShift), /**/);
		    )
#else /* LINE_BANK_ALIGNED */
		    DuffL (nl, label4,
			bits1 = BitRight(bits, rightShift);
			--psrc; CHECKRU(psrc); bits = *MAPR(psrc);
			--pdst;	CHECKWU(pdst);
                        *MAPW(pdst) = MROP_SOLID(bits1 | BitLeft(bits,
                                                    leftShift), /**/);
		    )
#endif /* LINE_BANK_ALIGNED */
#endif /* ONEBANK */
#endif /* LARGE_INSTRUCTION_CACHE */

		    if (startmask)
		    {
			bits1 = BitRight(bits, rightShift);
			if (BitRight (startmask, leftShift))
			{
			    --psrc; CHECKRU(psrc); bits = *MAPR(psrc);
			    bits1 |= BitLeft(bits, leftShift);
			}
			PUSHR();
			--pdst; CHECKRWU(pdst);
                        *MAPW(pdst) = MROP_MASK(bits1, *MAPR(pdst), startmask);
			POPR();
		    }
		}
	    }
#endif /* !DO_UNALIGNED_BITBLT */
	}
	pbox++;
	pptSrc++;
    }
    if (pboxNew2)
    {
	DEALLOCATE_LOCAL(pptNew2);
	DEALLOCATE_LOCAL(pboxNew2);
    }
    if (pboxNew1)
    {
	DEALLOCATE_LOCAL(pptNew1);
	DEALLOCATE_LOCAL(pboxNew1);
    }
}

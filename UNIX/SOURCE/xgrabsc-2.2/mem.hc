/*========================================================================
 *
 * Name - mem.hc
 *
 * ccs version:	1.2
 *
 * ccsid:	@(#)mem.hc	1.2 - 7/6/92 10:52:47
 * from: 	ccs/s.mem.hc
 * date: 	8/12/92 14:15:38
 *
 * Description:  memcpy/set functions for xgrabsc
 *
 *
 *========================================================================
 */

#ifdef MEMCPY

/* memcpy and memset routines from C News */


/*
 * memcpy - copy bytes
 */

char *
memcpy(dst, src, size)
char * dst;
 char * src;
int size;
{
        register char *d;
        register  char *s;
        register int n;

        if (size <= 0)
                return(dst);

        s = src;
        d = dst;
        if (s <= d && s + (size-1) >= d) {
                /* Overlap, must copy right-to-left. */
                s += size-1;
                d += size-1;
                for (n = size; n > 0; n--)
                        *d-- = *s--;
        } else
                for (n = size; n > 0; n--)
                        *d++ = *s++;

        return(dst);
}

/*
 * memset - set bytes
 *
 * CHARBITS should be defined only if the compiler lacks "unsigned char".
 * It should be a mask, e.g. 0377 for an 8-bit machine.
 */

#ifndef CHARBITS
#       define  UNSCHAR(c)      ((unsigned char)(c))
#else
#       define  UNSCHAR(c)      ((c)&CHARBITS)
#endif

char *
memset(s, ucharfill, size)
 char * s;
register int ucharfill;
int size;
{
        register  char *scan;
        register int n;
        register int uc;

        scan = s;
        uc = UNSCHAR(ucharfill);
        for (n = size; n > 0; n--)
                *scan++ = uc;

        return(s);
}
#endif /* MEMCPY */





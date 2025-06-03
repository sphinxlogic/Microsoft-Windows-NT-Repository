/*
 *			l z d c m 2 . c
 *
 * Actual decompression code
 */

/*
 * Edit history:
 *
 * 12-Sep-93	dp	Assorted bug fixes and cleanups for VMS.
 *
 */

#include "lz.h"

/*
 * These global parameters are read from the compressed file.
 * The decompressor needs them.
 */
extern short	maxbits;		/* settable max # bits/code	*/
extern code_int	maxmaxcode;		/* 1 << maxbits			*/

static flag	first_clear = TRUE;

/*
 * Big data storage stuff
 */

static char_type	stack[MAXSTACK];
#define	STACK_TOP	(&stack[MAXSTACK])
static U_short		tab_prefix[1 << BITS];	/* prefix code		*/
static char_type  	tab_suffix[1 << BITS];	/* last char in string	*/
code_int		next_code;

#if DEBUG
#define	CHECK(what)						\
	if (stp <= stack) {					\
	    fprintf(stderr, "Stack overflow -- %s\n", what);	\
	    abort();						\
	}
#else
#define CHECK(what)
#endif

int
decompress(out)
STREAM		*out;
/*
 * Decompress instream (global) to out.  Returns "end" signal:
 *	-1	end of file
 *	LZ_EOR	end of record
 *	LZ_ETX	end of segment
 */
{
	register char_type	*stp;		/* Stack pointer	*/
	register code_int code, oldcode, incode;
	register int		final;		/* End of a sequence?	*/
	register char_type	*pstp;		/* Stack pointer and	*/
	register int		previous;	/* char for diff flavor	*/
	extern code_int		getcode();

	stp = STACK_TOP;
	final = oldcode = getcode();
	PUT((char) final, out);
	previous = final;
	while ((code = getcode()) >= 0) {
test:	    if (code >= LZ_CLEAR && code < firstcode) {
		if ((mode & MODE_DELTA) != 0) {
		    for (pstp = stp; pstp < STACK_TOP;) {
#if UCHAR
			previous += *pstp;
#else
			previous += (*pstp & 0xFF);
#endif
			*pstp++ = previous;
		    }
		}
		lz_putbuf(stp, STACK_TOP - stp, out);
		stp = STACK_TOP;
		switch (code) {
		case LZ_ETX:
		case LZ_EOR:
		    goto finish;

		case LZ_SOH:			/* Unexpected		*/
		case LZ_STX:			/* Unexpected		*/
		default:
		    fprintf(stderr, "\nUnexpected control 0x%X\n", code);
		    FAIL("unexpected control", EXIT_FAILURE);

		case LZ_CLEAR:
		    init_decompress();		/* Before getcode() !! */
		    if ((code = getcode()) < 0
		      || ((export & (EXPORT_UNIX | EXPORT_ENDMARKER))
				 == (EXPORT_UNIX | EXPORT_ENDMARKER)
		       && code == LZ_CLEAR))
			goto finish;
		    else {
			/*
			 * init_decompress has set next_code to firstcode,
			 * however, for magical reasons, we want to toss
			 * the next substring, so we set next_code so
			 * that the tab_... entry is effectively ignored.
			 * Note below that tab_prefix[next_code] is set
			 * to the character before the LZ_CLEAR and
			 * tab_suffix to the character after the LZ_CLEAR.
			 * But, these values have no relationship to one
			 * another, so, by decrementing next_code, they
			 * will be ignored.  (I think.)
			 */
			next_code--;
			goto test;
		    }
		}
	    }
	    incode = code;
	    /*
	     * Special case for KwKwK string.
	     */
	    if (code >= next_code) {
		CHECK("KwKwK");
		*--stp = final;
		code = oldcode;
	    }
	    /*
	     * Generate output characters in reverse order
	     */
#ifdef interdata
	    while (((unsigned long) code) >= ((unsigned long) NBR_CHAR)) {
#else
	    while (code >= NBR_CHAR) {
#endif
		CHECK("generate output");
		*--stp = tab_suffix[code];
		code = tab_prefix[code];
	    }
	    CHECK("final char");
	    *--stp = final = tab_suffix[code];
	    /*
	     * And put them out in forward order
	     */
	    if ((mode & MODE_DELTA) != 0) {
		for (pstp = stp; pstp < STACK_TOP;) {
#if UCHAR
		    previous += *pstp;
#else
		    previous += (*pstp & 0xFF);
#endif
		    *pstp++ = previous;
		}
	    }
	    lz_putbuf(stp, STACK_TOP - stp, out);
	    stp = STACK_TOP;
	    /*
	     * Generate the new entry.
	     */
	    if ((code = next_code) < maxmaxcode) {
		tab_prefix[code] = (U_short) oldcode;
		tab_suffix[code] = final;
		next_code++;
	    } 
	    /*
	     * Remember previous code.
	     */
	    oldcode = incode;
	}
finish:
	return (code);
}

init_decompress()
/*
 * Called on cold start, or on LZ_SOH, LZ_STX, LZ_CLEAR.
 */
{
	register char_type	*cp;
	register U_short	*up;
	register int		code;

	if (first_clear) {
	    for (cp = &tab_suffix[0], code = 0; cp < &tab_suffix[NBR_CHAR];)
		*cp++ = code++;
	    first_clear = FALSE;
	}
	else {
#if ((NBR_CHAR % 8) != 0)
	    << error, the following won't work >>
#endif
	    for (up = &tab_prefix[0]; up < &tab_prefix[NBR_CHAR];) {
		*up++ = 0;
		*up++ = 0;
		*up++ = 0;
		*up++ = 0;
		*up++ = 0;
		*up++ = 0;
		*up++ = 0;
		*up++ = 0;
	    }
	}
	next_code = firstcode;
}

#if DEBUG
dump_tab(dumpfd)
FILE		*dumpfd;
/*
 * dump string table
 */
{
	register char_type	*stp;	/* Stack pointer		*/
	register int		i;
	register int		ent;
	extern char		*dumpchar();

	stp = STACK_TOP;
	fprintf(dumpfd, "%d %s in string table\n",
	    next_code, (next_code == 1) ? "entry" : "entries");
	for (i = 0; i < next_code; i++) {
	    fprintf(dumpfd, "%5d: %5d/'%s' ",
		i, tab_prefix[i], dumpchar(tab_suffix[i]));
	    for (ent = i;;) {
		*--stp = tab_suffix[ent];
		if (ent < firstcode)
		    break;
		ent = tab_prefix[ent];
	    }
	    dumptext(stp, STACK_TOP - stp, dumpfd);
	    stp = STACK_TOP;
	}
}
#endif

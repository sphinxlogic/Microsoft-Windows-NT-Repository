/*
 * 			l z c m p 3 . c
 * Output a given code.
 */

/*
 * Edit history:
 *
 * 12-Sep-93	dp	Assorted bug fixes and cleanups for VMS.
 *
 */

#include "lz.h"

extern STREAM	outstream;
extern code_int	next_code;
extern code_int	maxmaxcode;		/* Actual maximum output code	*/
extern short	maxbits;
extern count_int out_count;

static char_type buf[BITS];
static int	offset;
static short	n_bits = INIT_BITS;	/* # of bits in compressed file	*/
static short	n_8bits = INIT_BITS << 3;
static code_int	maxcode = MAXCODE(INIT_BITS);

#if !vax_asm && !vms_asm
static readonly char_type lmask[9] = {
    0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00
};
static readonly char_type rmask[9] = {
    0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
};
#endif

#if DEBUG
extern int	col;			/* Printout column		*/
static int	todump;			/* Dump start of the code range	*/
#endif

outputcode(code)
code_int  code;
/*
 * Output the given code.
 * Inputs:
 * 	code:	A n_bits-bit integer.  If == -1, then EOF.  This assumes
 *		that n_bits <= (long)wordsize - 1.
 * Note: if not in "export" mode, the following values are special:
 *	LZ_CLEAR	(also in export mode if block_compress TRUE)
 *			(soft) clear out compress tables and reset the
 *			number of bits per code to the minimum.
 *	LZ_SOH, LZ_STX	(hard) clear out compress tables and reset as above.
 *	LZ_ETX, LZ_EOR	force out the current output segment, analogous
 *			to fflush.
 *			
 * Outputs:
 * 	Outputs code to the file.  If the codespace has filled
 *	(next_code >= (1 << n_bits), increase n_bits.
 *	If LZ_CLEAR, LZ_SOH, or LZ_STX is seen, reset n_bits
 *	to the initial value and call init_compress to reset
 *	the lookup and cache tables.
 *
 * Assumptions:
 *	Output chars are 8 bits long.  This is deeply hardwired
 *	into the algorithm.  It is independent, however, of the
 *	size of the input data.
 *
 * Algorithm:
 * 	Maintain a BITS character long buffer (so that 8 codes will
 *	fit in it exactly).  Use the VAX insv instruction to insert each
 *	code in turn.  When the buffer fills up empty it and start over.
 */
{
	/*
	 * On the VAX (Unix), it is important to have the register declarations
	 * in exactly the order given, or the asm will break.
	 */
	register int	r_off;			/* R11 -- offset	*/
	register int	bits;			/* R10 -- bits/code	*/
	register char_type	*bp;		/* R09 -- buffer	*/
#if !vax_asm
	register code_int	r_code;
#endif

	r_off = offset;
	bits = n_bits;
	bp = buf;
	if (code >= 0) {
	    /*
	     * Not at EOF, add the code
	     */
#if DEBUG
	    if ((show & SHOW_SERIOUS_DEBUG) != 0) {
		fprintf(stderr, "%c%5d %5d",
		    ((col += 12) >= 72) ? (col = 0, '\n') : ' ',
		    code, next_code);
		if (code >= LZ_CLEAR && code < firstcode) {
		    fprintf(stderr, " = %s", lz_names[code - LZ_CLEAR]);
		    col = 74;
		}
	    }
#endif
#if vax_asm
	    /*
	     * VAX (4.x bsd) DEPENDENT!! Implementation on other
	     * machines or operating systems may be difficult.
	     *
	     * Translation: Insert BITS bits from the argument starting at
	     * offset bits from the beginning of buf.
	     */
	    0;					/* C compiler bug ??	*/
	    asm("insv	4(ap),r11,r10,(r9)");
#else
#if vms_asm
	    lib$insv(&code, &offset, &bits, bp);
#else
	    /*
	     * WARNING: byte/bit numbering on the vax is simulated
	     * by the following code
	     */
	    bp += (r_off >> 3);			/* -> first output slot	*/
	    r_off &= 7;
	    /*
	     * Since code is always >= 8 bits, only need to mask the first
	     * hunk on the left.
	     */
	    r_code = code;
	    *bp = (*bp & rmask[r_off]) | (r_code << r_off) & lmask[r_off];
	    bp++;
	    bits -= (8 - r_off);
	    r_code >>= 8 - r_off;
	    /*
	     * Get any 8 bit parts in the middle ( <= 1 for up to 16 bits).
	     */
	    if (bits >= 8) {
		*bp++ = r_code;
		r_code >>= 8;
		bits -= 8;
	    }
	    if (bits != 0)				/* Last bits. */
		*bp = r_code;
#endif
#endif
	    offset += n_bits;
	    if (offset == n_8bits) {
		out_count += n_bits;
		lz_putbuf(buf, n_bits, &outstream);
#if DEBUG
		if (todump > 0) {
		    dumphex(buf, n_bits, stderr);
		    todump -= n_bits;
		}
#endif
		offset = 0;
	    }
	    /*
	     * If the next entry is going to be too big for the code size,
	     * then increase it, if possible.  Note:
	     *     !export			firstcode == LZ_FIRST
	     *	   export && block_compress	firstcode == LZ_CLEAR + 1
	     *	   export && !block_compress	firstcode == LZ_CLEAR
	     */
	    if (next_code > maxcode) {
		if (offset > 0) {
		    lz_putbuf(buf, n_bits, &outstream);
		    out_count += n_bits;
		    offset = 0;
#if DEBUG
		    if (todump > 0) {
			dumphex(buf, n_bits, stderr);
			todump -= n_bits;
		    }
#endif
		}
		n_bits++;			/* Need more bits	*/
		n_8bits += (1 << 3);
		if (n_bits == maxbits)
		    maxcode = maxmaxcode;
		else
		    maxcode = MAXCODE(n_bits);
#if DEBUG
		if ((show & SHOW_DEBUG) != 0) {
		    fprintf(stderr,
			"%snext_code is %d, changing to %d bits (max %d)",
			(col > 0) ? "\n" : "", next_code,
			n_bits, maxcode);
		    col = 74;
		}
#endif
	    }
	    if (code >= LZ_CLEAR && code < firstcode) {
		switch (code) {
		case LZ_SOH:
		case LZ_STX:
		case LZ_CLEAR:
		    if (offset > 0) {
			lz_putbuf(buf, n_bits, &outstream);
			out_count += n_bits;
			offset = 0;
#if DEBUG
			if (todump > 0) {
			    dumphex(buf, n_bits, stderr);
			    todump -= n_bits;
			}
#endif
		    }
		    n_bits = INIT_BITS;		/* Reset codes		*/
		    n_8bits = INIT_BITS << 3;
		    maxcode = MAXCODE(INIT_BITS);
		    init_compress(code != LZ_CLEAR);
#if DEBUG
		    if ((show & SHOW_DEBUG) != 0) {
			fprintf(stderr,
		    "\n(%s) Change to %d bits, maxcode %d, next_code = %d",
			    lz_names[code - LZ_CLEAR],
			    n_bits, maxcode, next_code);
			col = 74;
			todump = 32;
		    }
#endif
		    break;

		case LZ_EOR:
		case LZ_ETX:			/* Just written out	*/
		    break;

		default:
		    abort();			/* Can't happen		*/
		}
	    }
	}
	else {
	    /*
	     * At EOF, write the rest of the buffer.
	     */
	    if ((r_off = offset) > 0) {
		r_off += 7;
		r_off >>= 3;
		lz_putbuf(buf, r_off, &outstream);
		out_count += r_off;
#if DEBUG
		if (todump > 0) {
		    dumphex(buf, r_off, stderr);
		    todump -= r_off;
		}
#endif
	    }
	    offset = 0;
	    lz_flush(&outstream);		/* Flush output buffer	*/
#if DEBUG
	    if ((show & SHOW_SERIOUS_DEBUG) != 0
	     || todump > 0) {
		fprintf(stderr, "\n*EOF*\n");
		col = 0;
	    }
#endif
	}
}

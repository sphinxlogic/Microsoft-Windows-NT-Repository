/*
 *			l z d c m 3 . c
 *
 * Read codes from the input stream.
 */

/*
 * Edit history:
 *
 * 12-Sep-93	dp	Assorted bug fixes and cleanups for VMS.
 *
 */

#include "lz.h"


#if !vax_asm && !vms_asm
static readonly char_type rmask[9] = {
	0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF
};
#endif
#if DEBUG
extern int	col;
static int	todump;
unsigned long	code_count;
unsigned long	dump_first = -1;
unsigned long	dump_last = -1;
#endif

/*
 * getcode()
 *
 * Read one code from the standard input.  If EOF, return -1.
 * Inputs:
 * 	stdin (via GET)
 * Outputs:
 * 	code or -1 is returned.
 */

extern code_int		next_code;
extern STREAM		instream;
extern code_int		maxmaxcode;
extern short		maxbits;
static short		n_bits = INIT_BITS;
static code_int		maxcode = MAXCODE(INIT_BITS);

/*
 * buf[] contains 8-bit data read from the input stream.  getcode()
 * treats buf[] as a vector of bits, repacking it into variable-bit codes.
 */
static char_type	buf[BITS];
static int		offset = 0;	/* Offset into buf    IN BITS 	*/
static int		size = 0;	/* Actual size of buf IN BITS 	*/

code_int
getcode()
{
	/*
	 * On the VAX (4.2 bsd), it is important to have the register
	 * declarations in exactly the order given, or the asm will break.
	 */
	register code_int 	code;		/* R11 extracted code	*/
	register int		r_off;		/* R10 offset		*/
	register int		bits;		/* R09 bits/code	*/
	register char_type	*bp;		/* R08 buffer pointer	*/

	bp = buf;
	if (next_code > maxcode) {
	    n_bits++;
	    if (n_bits == maxbits)
		maxcode = maxmaxcode;
	    else {
		maxcode = MAXCODE(n_bits);
	    }
	    size = 0;
#if DEBUG
	    if ((show & SHOW_DEBUG) != 0) {
		fprintf(stderr, "\nChange to %d bits", n_bits);
		col = 74;
	    }
#endif
	}
	if (offset >= size) {
	    size = lz_getbuf(buf, n_bits, &instream);
#if DEBUG
	    if ((show & SHOW_SERIOUS_DEBUG) != 0
	     || todump > 0
	     || (code_count >= dump_first && code_count <= dump_last)) {
		fprintf(stderr, "code_count %lu\n", code_count);
		dumphex(buf, size, stderr);
		if (todump > 0)
		    todump -= size;
	    }
#endif
	    if (size <= 0)
		return (-1);			/* end of file		*/
	    offset = 0;
	    /*
	     * Round size down to integral number of codes in the buffer.
	     * (Expressed as a number of bits).
	     */
	    size = (size << 3) - (n_bits - 1);
	}
	r_off = offset;
	bits = n_bits;
#if vax_asm
	asm("extzv   r10,r9,(r8),r11");
#else
#if vms_asm
	code = lib$extzv(&offset, &n_bits, bp);
#else
	/*
	 * Get to the first byte.
	 */
	bp += (r_off >> 3);
	r_off &= 7;
	/*
	 * Get first part (low order bits)
	 */
#if UCHAR
	code = (*bp++ >> r_off);
#else
	/*
	 * Don't touch the 0xFF; it prevents sign extension.
	 */
	code = ((*bp++ >> r_off) & rmask[8 - r_off]) & 0xFF;
#endif
	bits -= (8 - r_off);
	r_off = 8 - r_off;		/* now, offset into code word	*/
	/*
	 * Get any 8 bit parts in the middle (<=1 for up to 16 bits).
	 */
	if (bits >= 8) {
#if UCHAR
	    code |= *bp++ << r_off;
#else
	    code |= (*bp++ & 0xFF) << r_off;
#endif
	    r_off += 8;
	    bits -= 8;
	}
	/* high order bits. */
#if UCHAR
	code |= (*bp & rmask[bits]) << r_off;
#else
	code |= (*bp & rmask[bits]) << r_off;
#endif
	/*
	 * End of non-vax (Unix) specific code.
	 */
#endif
#endif
	offset += n_bits;
	if (code >= LZ_CLEAR && code < firstcode) {
	    switch (code) {
	    case LZ_SOH:
	    case LZ_STX:
	    case LZ_CLEAR:
		size = 0;			/* Force read next time	*/
		n_bits = INIT_BITS;
		maxcode = MAXCODE(INIT_BITS);
#if DEBUG
		if ((show & SHOW_DEBUG) != 0) {
		    fprintf(stderr, "Read %s (%d)\n",
			lz_names[code - LZ_CLEAR], code);
		    todump = 32;
		}
#endif
		break;
	    }
	}
#if DEBUG
	if ((show & SHOW_SERIOUS_DEBUG) != 0
	 || (code_count >= dump_first && code_count <= dump_last)) {
	    fprintf(stderr, "%c%5d %5d",
		((col += 12) >= 72) ? (col = 0, '\n') : ' ',
		code, next_code);
	    if (code >= LZ_CLEAR && code < firstcode) {
		fprintf(stderr, " = %s", lz_names[code - LZ_CLEAR]);
	        col = 74;
	    }
	}
	++code_count;
#endif
	return (code);
}


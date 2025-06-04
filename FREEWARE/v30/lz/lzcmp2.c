/*
 *		l z c m p 2 . c
 *
 * Actually do compression.  Terminology (and algorithm):
 *
 * Assume the input string is "abcd", we have just processed "ab" and
 * read 'c'.  At this point, a "prefix code" will be assigned to "ab".
 * Search in the prefix:character memory (either the "fast memory" or
 * the hash-code table) for the code followed by this character.  If
 * found, assign the code found to the "prefix code" and read the
 * next character.  If not found, output the current prefix code,
 * generate a new prefix code and store "old_prefix:char" in the
 * table with "new_prefix" as its definition.
 *
 * Naming conventions:
 *   code	a variable containing a prefix code
 *   c or char	a variable containing a character
 *
 * There are three tables that are searched (dependent on compile-time
 * and execution time considerations):
 *   fast	Direct table-lookup -- requires a huge amount of physical
 *		(non-paged) memory, but is very fast.
 *   hash	Hash-coded table-lookup.
 *   cache	A "look-ahead" cache for the hash table that optimizes
 *		searching for the most frequent character.  This considerably
 *		speeds up processing for raster-images (for example) at
 *		a modest amount of memory.
 * Structures are used to hold the actual tables to simplify organization
 * of the program.
 *
 * Subroutines:
 *    compress()	performs data compression on an input datastream.
 *    init_compress()	called by the output routine to clear tables.
 */

/*
 * Edit history:
 *
 * 12-Sep-93	dp	Assorted bug fixes and cleanups for VMS, including
 *
 */

#include	"lz.h"

/*
 * General variables
 * Cleared by init_compress on a "hard initialization"
 * outputcode() in lzcmp3.c refers to next_code.
 */

long int	in_count;		/* Length of input		*/
long int	out_count;		/* Bytes written to output file	*/
static flag	first_clear = TRUE;	/* Don't zero first time	*/
code_int	next_code;		/* Next output code		*/
static count_int checkpoint = CHECK_GAP; /* When to test ratio again	*/
static long	ratio = 0;		/* Ratio for last segment	*/

/*
 * These global parameters are set by mainline code.  Unchanged here.
 */
extern code_int	maxmaxcode;		/* Actual maximum output code	*/
extern long	tot_incount;		/* Total input count		*/
extern long	tot_outcount;		/* Total output count		*/
extern code_int	hsize;			/* Actual hash table size	*/

#ifdef XENIX_16
static count_int htab0[8192];
static count_int htab1[8192];
static count_int htab2[8192];
static count_int htab3[8192];
static count_int htab4[8192];
static count_int htab5[8192];
static count_int htab6[8192];
static count_int htab7[8192];
static count_int htab8[HSIZE - 65536];
static count_int *hashtab[9] = {
    htab0, htab1, htab2, htab3, htab4, htab5, htab6, htab7, htab8
};

static U_short code0[16384];
static U_short code1[16384];
static U_short code2[16384];
static U_short code3[16384];
static U_short code4[HSIZE - 65536];
static U_short *codetab[5] = {
    code0, code1, code3, code3, code4
}

#define HASH(i)		(hashtab[((unsigned) (i)) >> 13][(i) & 0x1FFF])
#define CODE(i)		(codetab[((unsigned) (i)) >> 14][(i) & 0x3FFF])

#else
count_int	hashtab[HSIZE];
U_short		codetab[HSIZE];

#define HASH(i)		hashtab[i]
#define CODE(i)		codetab[i]
#endif

/*
 * compress a datastream
 *
 * Algorithm:  on large machines, for maxbits <= FBITS, use fast direct table
 * lookup on the prefix code / next character combination.  For smaller code
 * size, use open addressing modular division double hashing (no chaining), ala
 * Knuth vol. 3, sec. 6.4 Algorithm D, along with G. Knott's relatively-prime
 * secondary probe.  Do block compression with an adaptive reset, whereby the
 * code table is cleared when the compression ratio decreases, but after the
 * table fills.  The variable-length output codes are re-sized at this point,
 * and a special LZ_CLEAR code is generated for the decompressor.  For the
 * megamemory version, the sparse array is cleared indirectly through a
 * "shadow" output code history.  Late additions: for the hashing code,
 * construct the table according to file size for noticeable speed improvement
 * on small files.  Also detect and cache codes associated with the most
 * common character to bypass hash calculation on these codes (a characteristic
 * of highly-compressable raster images).  Please direct questions about this
 * implementation to ames!jaw.
 */

compress(in)
STREAM		*in;		/* Input stream structure		*/
/*
 * Compress driver.  Global fsize is the size of the entire datastream
 * (from LZ_STX or LZ_SOH to the terminating LZ_ETX).  You must
 * force a reinitialization -- by calling outputcode() with a new header --
 * if size is changed.  If the "newer" output format is chosen (with
 * data streams delimited by LZ_SOH/LZ_STX, init_compress will be
 * called automatically.  Otherwise, you must call init_compress(TRUE)
 * before calling compress() for the first time.
 */
{
	register long		hash_code;	/* What we look for	*/
	register code_int	i;		/* Index into vectors	*/
	register unsigned int	c;		/* Current input char	*/
	register code_int	code;		/* Substring code	*/
	register int		displacement;	/* For secondary hash	*/
	register code_int	hsize_reg;	/* Size of hash table	*/
	register int		hshift;		/* For xor hasher	*/
	register unsigned int	previous;	/* Prev. char for diffs	*/
	register unsigned int	difference;	/* Temp for difference	*/

	if ((code = GET(in)) == (unsigned) EOF)
	    return;
	previous = code;
	in_count++;
	hsize_reg = hsize;
	/*
	 * Set hash code range bound
	 */
	hshift = 0;
	for (hash_code = (long) hsize; hash_code < 65536L; hash_code <<= 1)
	    hshift++;
	hshift = 8 - hshift;
	while ((c = GET(in)) != (unsigned) EOF) {
	    in_count++;
	    if ((mode & MODE_DELTA) != 0) {
#if UCHAR
		difference = (unsigned char) (c - previous);
#else
		difference = (c - previous) & 0xFF;
#endif
		previous = c;
		c = difference;
	    }
	    hash_code = (long) (((long) c << maxbits) + code);
	    i = (c << hshift) ^ code;		/* XOR hashing		*/
	    if (HASH(i) == hash_code) {		/* Found at first slot?	*/
		code = CODE(i);
		continue;
	    }
	    else if ((long) HASH(i) < 0)	/* empty slot		*/
		goto nomatch;
	    displacement = hsize_reg - i;	/* secondary hash	*/
	    if (i == 0)
		displacement = 1;
probe:
	    if ((i -= displacement) < 0)	/* Wrap around?		*/
		i += hsize_reg;
	    if (HASH(i) == hash_code) {		/* Found in hash table?	*/
		code = CODE(i);			/* Set new prefix code	*/
		continue;			/* Read next input char	*/
	    }
	    else if ((long) HASH(i) > 0)	/* If slot is occupied	*/
		goto probe;			/* Look somewhere else	*/
nomatch:
	    /*
	     * Output the current prefix and designate a new prefix.
	     * If the input character was the "hog", save it in the
	     * look-ahead cache table.  Then, save in the hash table.
	     */
	    outputcode((code_int) code);	/* No match, put prefix	*/
#if SIGNED_COMPARE_SLOW
	    if ((unsigned) next_code < (unsigned) maxmaxcode) {
#else
	    if (next_code < maxmaxcode) {
#endif
		CODE(i) = next_code++;		/* code -> hashtable	*/
		HASH(i) = hash_code;
	    }
	    else if ((export & EXPORT_BLOCK) != 0
		  && (count_int) in_count >= checkpoint) {
		clear();
	    }
	    code = c;				/* Start new substring	*/
	}
	/*
	 * At EOF, put out the final code.
	 */
	outputcode((code_int) code);
}

clear()
/*
 * Check the compression ratio to see whether it is going up
 * or staying the same.  If it is going down, the internal
 * statistics of the file have changed, so clear out our
 * tables and start over.  Inform the decompressor of the
 * change by sending out a LZ_CLEAR code.
 */
{
	register long int	rat;

	checkpoint = in_count + CHECK_GAP;
	if ((show & (SHOW_PROGRESS | SHOW_DEBUG)) != 0) {
	    divout("at compression ratio test ",  in_count, out_count, "\n");
	}
	if (in_count > 0x007FFFFL) {		/* Shift will overflow	*/
	    rat = out_count >> 8;
	    if (rat == 0)
		rat = 0x7FFFFFFFL;
	    else {
		rat = in_count / rat;
	    }
	}
	else {
	    rat = (in_count << 8) / out_count;
	}
	if (rat > ratio)
	    ratio = rat;
	else {
	    if ((show & (SHOW_PROGRESS | SHOW_DEBUG)) != 0) {
		fprintf(stderr, "Resetting compression, in %ld, out %ld\n",
		    in_count, out_count);
		fprintf(stderr, "Old ratio: %ld.%02ld",
		    ratio / 256L, ((ratio & 255L) * 100L) / 256L);
		fprintf(stderr, ", test ratio: %ld.%02ld, gap %d\n",
		    rat / 256L, ((rat & 255L) * 100L) / 256L, CHECK_GAP);
	    }
	    outputcode((code_int) LZ_CLEAR);	/* Calls init_compress	*/
	}
}

init_compress(full_init)
flag		full_init;	/* TRUE for full initialization		*/
/*
 * Clear the tables.  Called by outputcode() on LZ_SOH, LZ_STX
 * (full_init TRUE) or on LZ_CLEAR (full_init FALSE).
 * init_compress() is not called on LZ_EOR.
 */
{
#ifdef XENIX_16
	register count_int	*hp;
	register int		n;
	register int		j;
	register code_int	k;

	k = hsize;
	for (j = 0; k > 0; k -= 8192) {
	    i = (k < 8192) ? k : 8192;
	    hp = hashtab[j++];
	    n = i >> 4;
	    switch (i & 15) {
	    case 15:	*hp++ = -1;
	    case 14:	*hp++ = -1;
	    case 13:	*hp++ = -1;
	    case 12:	*hp++ = -1;
	    case 11:	*hp++ = -1;
	    case 10:	*hp++ = -1;
	    case  9:	*hp++ = -1;
	    case  8:	*hp++ = -1;
	    case  7:	*hp++ = -1;
	    case  6:	*hp++ = -1;
	    case  5:	*hp++ = -1;
	    case  4:	*hp++ = -1;
	    case  3:	*hp++ = -1;
	    case  2:	*hp++ = -1;
	    case  1:	*hp++ = -1;
	    }
	    while (--n >= 0) {
		*hp++ = -1; *hp++ = -1; *hp++ = -1; *hp++ = -1;
		*hp++ = -1; *hp++ = -1; *hp++ = -1; *hp++ = -1;
		*hp++ = -1; *hp++ = -1; *hp++ = -1; *hp++ = -1;
		*hp++ = -1; *hp++ = -1; *hp++ = -1; *hp++ = -1;
	   }
	}
#else
	register count_int	*hp;
	register code_int	n;

	hp = &hashtab[0];
	n = hsize >> 4;			/* divide by 16			*/
	switch (hsize & 15) {
	case 15:	*hp++ = -1;
	case 14:	*hp++ = -1;
	case 13:	*hp++ = -1;
	case 12:	*hp++ = -1;
	case 11:	*hp++ = -1;
	case 10:	*hp++ = -1;
	case  9:	*hp++ = -1;
	case  8:	*hp++ = -1;
	case  7:	*hp++ = -1;
	case  6:	*hp++ = -1;
	case  5:	*hp++ = -1;
	case  4:	*hp++ = -1;
	case  3:	*hp++ = -1;
	case  2:	*hp++ = -1;
	case  1:	*hp++ = -1;
	}
	while (--n >= 0) {
	    *hp++ = -1; *hp++ = -1; *hp++ = -1; *hp++ = -1;
	    *hp++ = -1; *hp++ = -1; *hp++ = -1; *hp++ = -1;
	    *hp++ = -1; *hp++ = -1; *hp++ = -1; *hp++ = -1;
	    *hp++ = -1; *hp++ = -1; *hp++ = -1; *hp++ = -1;
	}
#endif
	if (full_init) {
	    tot_incount += in_count;
	    tot_outcount += out_count;
	    in_count = 0;
	    out_count = 0;
	    ratio = 0;
	}
	first_clear = FALSE;
	next_code = firstcode;
}

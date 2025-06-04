/* #define DEBUG	TRUE  */

/*
 * Header file for all lz compression/decompression routines.
 *
 * Machine/Operating system/compiler selection: (#ifdef'ed)
 * vax				Vax/Unix or Vax/VMS
 * __alpha			Alpha/Unix or Alpha/VMS
 * pdp11			makes a small compressor
 * M_XENIX			"large-model" Z8000
 * interdata			Signed long compare is slow
 * unix				Defined on true Unix systems
 * decus			Decus C (no signal)
 * vms				VMS (vax or alpha)
 * #define readonly		If the compiler doesn't support it correctly.
 * 
 * Compiler configuration (#if'ed):
 * #define vax_asm   TRUE/FALSE	TRUE on Vax (4bsd) if the compiler supports
 *				the asm() operator.  Check the generated code!
 * #define vms_asm   TRUE/FALSE	TRUE on VMS to use the run-time library
 *				insv and extv routines.
 * #define UCHAR     TRUE/FALSE	TRUE if compiler supports unsigned char
 * #define DEBUG     TRUE/FALSE	TRUE to compile in debug printouts
 *
 * Algorithm Tuning parameters:
 * #define USERMEM   <n>	Memory available to compress.
 *				If large enough, a faster algorithm is used.
 * #define SACREDMEM <n>	Don't use this part of USERMEM.
 * #define BITS      <n>	Maximum number of code bits.
 * #define MAXIO     <n>	Output buffer size (squeeze memory if needed)
 */

/*
 * Edits:
 *
 * 12-Sep-93 dp	 Assorted cleanups for VMS.  Main changes are to allow
 *		 for use of the DEC C compiler, to work around changes in 
 *		 RMS-related headers, and to eliminate use of the vms_v4 
 *		 conditional as distinct from just vms (that is, we no 
 *		 longer support vms v3 or older).
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <time.h>
#ifndef decus
# include <signal.h>
/*
 * Arguments to signal():
 */
extern int	abort();		/* Debugging interrupt trap	*/
extern int	interrupt();		/* Non-debugging interrupt trap	*/
extern int	address_error();	/* "Segment" violation		*/
#endif

#ifndef	TRUE
# define FALSE		0
# define TRUE		1
#endif
#ifndef	EOS
# define EOS		'\0'
#endif
#define	streq(a, b)	(strcmp((a), (b)) == 0)
#define min(a,b)	((a) > (b)) ? (b) : (a))

/*
 * Set USERMEM to the maximum amount of physical user memory available
 * in bytes.  USERMEM is used to determine the maximum BITS that can be used
 * for compression.
 *
 * SACREDMEM is the amount of physical memory saved for others; compress
 * will hog the rest.
 */

#ifndef SACREDMEM
# define SACREDMEM	0
#endif

/*
 * Set machine-specific parameters
 */

#ifdef vax
# ifdef unix
#  define vax_asm	TRUE		/* If asm() supported on vax	*/
# endif
#endif
#ifdef vms
# define vms_asm	TRUE		/* lib$ routines on vax or alpha */
#endif
#ifndef	vax_asm
# define vax_asm	FALSE
#endif
#ifndef vms_asm
# define vms_asm	FALSE
#endif

#ifdef pdp11
# define BITS	12	/* max bits/code for 16-bit machine		*/
# define USERMEM 0	/* Force no user memory				*/
# define UCHAR	FALSE	/* TRUE if compiler supports unsigned char	*/
# define MAXIO 512	/* Buffer size for PDP-11 I/O buffers		*/
#endif

/*
 * Set default values for some parameters.
 */

#ifndef DEBUG
# define DEBUG	FALSE
#endif

#ifdef interdata
# define SIGNED_COMPARE_SLOW TRUE
#endif
#ifndef SIGNED_COMPARE_SLOW
# define SIGNED_COMPARE_SLOW FALSE
#endif

#ifndef USERMEM
# define USERMEM 750000	/* default user memory				*/
#endif

#ifndef	UCHAR
# define UCHAR	TRUE	/* Compiler supports unsigned char		*/
#endif

#ifndef MAXIO
# define MAXIO	2048	/* I/O buffer size				*/
#endif

/*
 * Set derived tuning parameters.
 */

#ifndef USERMEM
# define USERMEM	 0
#endif
#if USERMEM >=			(433484 + SACREDMEM)
# define PBITS		16
#else
# if USERMEM >=			(229600 + SACREDMEM)
#  define PBITS		15
# else
#  if USERMEM >=		(127536 + SACREDMEM)
#   define PBITS	14
#   else
#    if USERMEM >=		( 73464 + SACREDMEM)
#     define PBITS	13
#    else			/* Smaller systems			*/
#     define PBITS	12
#    endif
#   endif
# endif
#endif

#ifndef BITS
# define BITS PBITS
#endif

#ifdef M_XENIX
# if BITS >= 16
#  define XENIX_16		/* Enable special vector access macros	*/
# else
#  if BITS > 13
#   undef BITS
#   define BITS 13		/* Code only handles BITS = 12, 13, 16	*/
#  endif
# endif
#endif

/*
 * HSIZE is the size of the hash lookup table.  It is set to
 * 1 << BITS + fudge factor, rounded up to a prime number.
 * If it is too big, the "clear the hash" routine will take
 * too long.  The same numbers are replicated in the getsize()
 * routine's data table.
 */

#if BITS == 16
# define HSIZE	69001		/* 95% occupancy			*/
#endif
#if BITS == 15
# define HSIZE	35023		/* 94% occupancy			*/
#endif
#if BITS == 14
# define HSIZE	18013		/* 91% occupancy			*/
#endif
#if BITS == 13
# define HSIZE	 9001		/* 91% occupancy			*/
#endif
#if BITS <= 12
# define HSIZE	 5003		/* 80% occupancy			*/
#endif

/*
 * typedef's -- somewhat machine specific.
 */

/*
 * a code_int must be able to hold 2**BITS values of type int, and also -1
 */
#if BITS > 15
typedef long int	code_int;
#else
typedef int		code_int;
#endif

/*
 * A count_int must hold ((2**BITS)-1) + (255<<BITS)) and -1.
 *
 * count_int's also hold counters.
 *
 * count_short's hold small counters (for the interdata)
 *
 * Some implementations don't support unsigned char (Decus C, for example)
 * Decus C is also brain damaged with regards to unsigned shorts.
 */
#if SIGNED_COMPARE_SLOW
typedef unsigned long int count_int;
typedef unsigned short int count_short;
#else
typedef long int	count_int;
#endif

#if UCHAR
typedef	unsigned char	char_type;
#else
typedef char		char_type;
#endif

#ifdef decus
typedef unsigned	U_short;
#define	readonly			/* Dummy out readonly modifier	*/
#else
typedef unsigned short	U_short;
#endif

#ifdef unix
#define	readonly
#endif

typedef short		flag;		/* Boolean flag or parameter	*/

/*
 * The following define the "magic cookie" header
 */
#define	HEAD1_MAGIC	0x1F
#define HEAD2_MAGIC	0x9D
#define	VMS_HEAD2_MAGIC	0x9E		/* vms-private output format	*/

/*
 * Defines for third byte of header
 */
#define BIT_MASK	0x1F		/* Gets NBITS in the code	*/
#define BLOCK_MASK	0x80		/* Gets block_compress flag	*/
#define	DIFF_MASK	0x40		/* Gets differential cmprs flag	*/
/*
 * Mask0x20 is free.  I think 0x20 should mean that there is
 * a fourth header byte (for expansion).
 */

/*
 * This is for backwards compatibilty with an old version of Unix compress.
 */
#ifdef COMPATIBLE			/* Compatible, but wrong!	*/
# define MAXCODE(n_bits)	(1 << (n_bits) - 1)
#else
# define MAXCODE(n_bits)	((1 << (n_bits)) - 1)
#endif

#define INIT_BITS 9			/* initial number of bits/code */

/*
 * One code could conceivably represent (1<<BITS) characters, but
 * to get a code of length N requires an input string of at least
 * N*(N-1)/2 characters.  With 5000 chars in the stack, an input
 * file would have to contain a 25Mb string of a single character.
 * This seems unlikely.
 */
#define MAXSTACK    8000		/* size of lzdcmp output stack	*/

#ifndef CHECK_GAP
# define CHECK_GAP 	10000		/* ratio check interval		*/
#endif

#ifndef __LINE__
# define NO__LINE__
#endif
#ifndef __FILE__
# define NO__LINE__
#endif
#if DEBUG
# define VERBOSE_DEFAULT    1
# ifndef NO__LINE__
#  define FAIL(why, status)				\
    do {						\
	fprintf(stderr, "\nfatal: %s (%s at %d)\n",	\
	    why, __FILE__, __LINE__); 			\
	longjmp(failure, status);			\
    } while (0);
# else
#  define FAIL(why, status)				\
    do {						\
	fprintf(stderr, "\nfatal: %s\n", why); 		\
	longjmp(failure, status);			\
    } while (0);
# endif
#else
# define VERBOSE_DEFAULT    0
# define FAIL(why, status)	longjmp(failure, status);
#endif

/*
 * Note -- for compatibility with Unix compress,
 * NBR_CHAR and LZ_CLEAR must equal 256.
 * Also, (1 << (MIN_BITS - 1) should equal or exceed NBR_CHR
 */
#define	NBR_CHAR      256		/* Number of input codes	*/
#define MIN_BITS	9		/* Smallest code is 9 bits	*/
#if ((1 << BITS) < NBR_CHAR) || (BITS < MIN_BITS)
    << Can't compile: not enough bits for the input character set size >>
#endif
#define	LZ_CLEAR	(NBR_CHAR)	/* Clear code			*/
#define	LZ_SOH		(LZ_CLEAR + 1)	/* Start of header block	*/
#define	LZ_STX		(LZ_SOH   + 1)	/* Start of text block		*/
#define	LZ_EOR		(LZ_STX   + 1)	/* End of text record		*/
#define	LZ_ETX		(LZ_EOR   + 1)	/* End of header/text block	*/
#define	LZ_FIRST	(LZ_ETX   + 1)	/* First user (data) code	*/

#ifdef	vms
#include		errno
#include		ssdef
#include		stsdef
#include		rms
#include		descrip
#include		stat
#ifndef	EXIT_SUCCESS
#define	EXIT_SUCCESS	(SS$_NORMAL | STS$M_INHIB_MSG)
#define	EXIT_FAILURE	(SS$_ABORT  | STS$M_INHIB_MSG)
#endif
#else
extern int		errno;
#ifdef decus
#define	errno		$$ferr
#endif
#endif

/*
 * Define exit() codes.
 */

#ifndef	EXIT_SUCCESS
#ifdef decus
#define	EXIT_SUCCESS	IO_SUCCESS
#define	EXIT_FAILURE	IO_ERROR
#else
#define	EXIT_SUCCESS	0			/* Normal exit		*/
#define	EXIT_FAILURE	1			/* Error exit		*/
#endif
#endif
#if EXIT_FAILURE == 0
  << error, EXIT_FAILURE must be non-zero for longjmp to work >>
#endif

#ifdef vms
#define	ERROR_EXIT	EXIT_FAILURE
#else
#define	ERROR_EXIT	(errno)
#endif

/*
 * Parameter values are converted to internal values (to simplify
 * processing of VMS DCL)
 */
#define EXPORT_VMS		0x0001
#define EXPORT_UNIX		0x0002
#define EXPORT_BLOCK		0x0004
#define	EXPORT_HEADER		0x0008
#define EXPORT_ENDMARKER	0x0010

#define	METHOD_LZ		0x0001

#define MODE_TEXT		0x0001
#define	MODE_BINARY		0x0002
#define	MODE_FIXED		0x0004
#define	MODE_DELTA		0x0008

#define	SHOW_STATISTICS		0x0001
#define	SHOW_PROGRESS		0x0002
#define	SHOW_FDL		0x0004
#define	SHOW_DEBUG		0x0008
#define SHOW_SERIOUS_DEBUG	0x0010
#define SHOW_IO_DEBUG		0x0020


/*
 * All I/O is done by way of "streams".  To establish a stream,
 * set the parameters appropriately and off you go.  The following
 * functions are provided:
 *	lz_fill(stream)		fills the buffer from stdin
 *	lz_flush(stream)	writes the buffer to stdout
 *	lz_eof(stream)		returns EOF (for fill from memory)
 *	lz_fail(stream)		abort (for writing to memory).
 *	lz_dummy(stream)	throw an output stream away.
 * Note: on vms, if the private (non-export) format is chosen, 
 * lz_fill and lz_flush access the files appropriately.
 * Stream elements are initialized as follows:
 *	Input:	bp = NULL;	bend = NULL;
 *	Output:	bp = bstart;	bend = bstart + bsize;
 */

typedef struct STREAM {
    char_type	*bp;		/* Next character to get/put		*/
    char_type	*bend;		/* -> end of stream buffer		*/
    char_type	*bstart;	/* Start of stream buffer		*/
    short	bsize;		/* Stream buffer size			*/
    int		(*func)();	/* Read/write a buffer function		*/
#ifdef vms
    long 	filsz;		/* Compressed input file size in bytes 	*/
    int		eofhack;	/* Flag to enable end-of-file hack      */
#endif
} STREAM;

/*
 * Note also that the compress routine uses putbuf(buf, count, outstream)
 * and the decompress routine uses getbuf(buf, count, instream) to (quickly)
 * transfer multiple bytes.
 */
#if UCHAR
#define	GET(s)		\
	(((s)->bp < (s)->bend) ? *(s)->bp++        : (*(s)->func)(s))
#else
#define	GET(s)		\
	(((s)->bp < (s)->bend) ? *(s)->bp++ & 0xFF : (*(s)->func)(s))
#endif
#define	PUT(c, s)	\
	((((s)->bp >= (s)->bend) ? (*(s)->func)(s) : 0), *(s)->bp++ = (c))

extern int lz_fill();
extern int lz_flush();
extern int lz_eof();
extern int lz_fail();
extern int lz_dummy();

#if DEBUG
extern readonly char *lz_names[];		/* "LZ_CLEAR" etc.	*/
#endif

/*
 * Options and globals.
 */
#ifdef vms
#include devdef

#define	ATT_NAME	"vms$attributes "
#define	ATT_SIZE	15			/* strlen(ATT_NAME)	*/

/*
 Some RMS structure names seem to have changed in
 the latest (V6?) C RTL.   The prototype XABs aren't
 defined either.  Here's an attempt to do the right
 thing with both old and new definitions.   /dp 13-Sep-93
*/
#ifdef __DECC
#ifndef cc$rms_xabfhc
#define XAB    xabdef
#define XABFHC xabfhcdef
#define cc$rms_xabfhc (*decc$ga_rms_xabfhc)
globalref struct XABFHC cc$rms_xabfhc;
#endif
#ifndef cc$rms_xabsum
#define XAB    xabdef
#define XABSUM xabsumdef
#define cc$rms_xabsum (*decc$ga_rms_xabsum)
globalref struct XABSUM cc$rms_xabsum;
#endif
#ifndef cc$rms_xabkey
#define XAB    xabdef
#define XABKEY xabkeydef
#define cc$rms_xabkey (*decc$ga_rms_xabkey)
globalref struct XABKEY cc$rms_xabkey;
#endif
#ifndef cc$rms_xaball
#define XAB    xabdef
#define XABALL xaballdef
#define cc$rms_xaball (*decc$ga_rms_xaball)
globalref struct XABALL cc$rms_xaball;
#endif
#endif

typedef struct {
	struct	RAB	rab;		/* Record access buffer		*/
	struct	FAB	fab;		/* File access buffer		*/
	struct	NAM	nam;		/* File name buffer		*/
	struct	XABFHC	xabfhc;		/* Attributes (file header blk)	*/
	struct	XABSUM	xabsum;		/* Attributes (isam summary)	*/
	char		starname[NAM$C_MAXRSS + 1]; /* Wild file name	*/
	char		filename[NAM$C_MAXRSS + 1]; /* Open file name	*/
} FDLSTUFF;

extern int	fdl_status;	/* Error code from fdl library		*/
#endif

extern flag	export;		/* -x, /EXPORT=(...)			*/
extern flag	method;		/* LZ, of course			*/
extern flag	show;		/* -v  /SHOW=(...) Verbose logging	*/
extern flag	mode;		/* Binary and friends			*/
extern short	maxbits;	/* -b /BITS=<n>				*/
extern readonly flag is_compress; /* TRUE if compress, FALSE if decomp.	*/
extern char	*infilename;	/* For error printouts			*/
extern char	*outfilename;	/* For more error printouts		*/
/* extern short	n_bits;		   Current # of bits in compressed file	*/
extern int	firstcode;	/* First value past signals		*/
extern jmp_buf	failure;	/* For longjmp() return			*/
extern int	fblksiz;	/* Block/record size for -f		*/

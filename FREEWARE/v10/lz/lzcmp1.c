/*
 *		lzcomp [-options] infile outfile
 */
/*
 * Edit History
 *  4-Sep-1985 MM	First (Beta test) release
 * 24-Oct-1985 MM	Save and restore xab longest record length
 *			(Note: files written with this version can't
 *			be read with previous versions.)
 * 07-Nov-1985 MM	Can now compress VMS ISAM files.  Added -s option
 *			courtesy of Arthur Olsen (elsie!ado).
 * 12-Sep-93	dp	Assorted bug fixes and cleanups for VMS, including:
 *			- Replace "#if vms_v4" by "ifdef vms" for simplicity.
 *			- Add elapsed time to stats output.
 */

#ifdef	DOCUMENTATION

title	lzcomp	File Compression
index		File compression

synopsis
	.s.nf
	lzcomp [-options] [infile [outfile]]
	.s.f
description

	lzcomp implements the Lempel-Ziv file compression algorithm.
	(Files compressed by lzcomp are uncompressed by lzdcmp.)
	It operates by finding common substrings and replaces them
	with a variable-size code.  This is deterministic, and
	can be done with a single pass over the file.  Thus,
	the decompression procedure needs no input table, but
	can track the way the table was built.

	Options may be given in either case.
	.lm +8
	.p -8
	-B	Input file is "binary", not "human readable text".
	This is necessary on Dec operating systems, such as VMS and
	RSX-11M, that treat these files differently.  (Note that binary
	support is rudamentary and probably insufficient as yet.)
	(On VMS version 4, this is ignored unless the -x option is
	specified or the input file is record-oriented.)
	.p -8
	-M bits	Write using the specified number of bits in the
	code -- necessary for big machines making files for little
	machines.  For example, if compressing a file on VMS
	which is to be read on a PDP-11, you should select -M 12.
	.p -8
	-V [n]	Verbose if specified.  If a value is specified,
	it will enable debugging code (if compiled in).  The values
	are bit-encoded as follows:
	.lm +4.s
	.i -4;#0##Don't print anything.
	.i -4;#1##Print a summary of the operation.
	.i -4;#2##Print compression progress reports.
	.i -4;#4##Print FDL information (for VMS export mode).
	.i -4;#8##Print some debug information.
	.i -4;#16#Print too much debug information.
	.i -4;#32#Dump the compressed files.
	.p -8
	-S	Perform a transformation on the input file whereby
	each byte is subtracted from its predecessor.  I.e., the
	difference between bytes is compressed, rather than their
	actual value.  This improves the performance of the algorithm
	on certain kinds of data files.  However, it will worsen
	the compression of most other kinds of data.  Note that files
	compressed with the -S option cannot be decompressed on versions
	of Unix compress (or lzdcmp) that do not support this option.
	.s
	The -S option is useful for digitized video images, where each
	pixel element is represented as a single byte.  It may also be
	useful when compressing digitized encoding of similar analog
	data, where each sample is contained in a single byte.
	.p -8
	-X [n]	"Export" -- write a file format that can be read by
	other operating systems.  Only the bytes in the file are copied;
	file attributes are not preserved.  If specified, the value
	determines the level of compatiblity.  If not specified,
	or specified with an explicit value of zero, and lzcomp is
	running on Vax/VMS version 4 under VaxC and the input file
	is a disk or magtape file (block-oriented), a VMS-private output
	format is used which is incompatible with the Unix compress
	utility, but which preserves VMS file attributes.  -X may
	take on the following values:
	.lm +4.s
	.i -4;#0##Choose VMS private format.  See restrictions below.
	.i -4;#1##Compatible with Unix compress version 3.0, except
	that an end code is written that may not be compatible with
	Unix compress. This is the default if -x is given without a value.
	This is the correct mode to use when transferring files between
	VMS and RSX-11M, RSTS/E, or RT11, or when transferring files
	to LZDCMP running on Unix.
	.i -4;#2##As above, but do not write the extra end code.
	This will cause problems when decompressing on RSX-11M and
	RT11 systems.
	.i -4;#3##As in (2) above, but suppress "block compression".
	.i -4;#4##As in (3) above, but do not output
	a compress header block.  This is for compatiblity
	with a quite early version of Unix compress (and requires
	conditional-compilation to use).
	.lm -4.s
	Note that the -B (binary) option is ignored unless
	the input file is "record-oriented", such as a terminal
	or mailbox.
	.lm -8.s
	The other two arguments are the input and output
	filenames respectively.  Redirection is supported,
	however, the output must be a disk/tape file.

	The file format is almost identical to the current
	Unix implementation of compress (V4.0).  Files written
	by Unix compress should be readable by lzdcmp.  Files
	written by lzcomp in export (-x) format will be
	readable by Unix compress (except that lzcomp outputs
	two "clear" codes to mark EOF.  A patch to Unix
	compress is available.)

VMS Command Language Interface

	In addition to the above (Unix-style) command line
	interface, lzcomp supports a VMS command line interface.
	The following options are available:
	.lm +8
	.p -8
	/BITS=<value>
	.p -8
	/EXPORT=(VMS, UNIX, BLOCK, HEADER, ENDMARKER)
	.p -8
	/MODE=(DELTA)
	.p -8
	/SHOW=(ALL, PROGRESS, STATISTICS, FDL,
		DEBUG, DEBUG_SERIOUS, DEBUG_IO)
	.lm -8

VMS Restrictions

	VMS Private mode stores the true name and attributes
	of the input file into the compressed file and lzdcmp
	restores the attributes (and filename if requested).
	The following restrictions apply -- they may be lifted
	in the future as they are primarily due to the author's
	lack of understanding of the intricacies of of VMS I/O:

	    All files must be stored on disk.
	    The lzcomp output file must be specified directly.

	Also, for all usage on VMS, the compressed file must
	be written to, and read from disk.

	The following file attributes are not preserved by
	lzcomp:

	    File ownership and protection codes.
	    Date of creation, access, and backup.
	    Access control lists.

RSX-11M restrictions

	lzcomp cannot determine the file attributes and may
	not correctly read certain specialized file formats,
	such as "print image".  If a binary file is compressed,
	note that it will be decompressed as "fixed-block,
	512 byte" records.

LZW compression algorithm

	This section is abstracted from Terry Welch's article
	referenced below.  The algorithm builds a string
	translation table that maps substrings in the input
	into fixed-length codes.  The compress algorithm may
	be described as follows:

	  1. Initialize table to contain single-character
	     strings.
	  2. Read the first character.  Set <w> (the prefix
	     string) to that character.
	  3. (step): Read next input character, K.
 	  4. If at end of file, output code(<w>); exit.
	  5. If <w>K is in the string table:
		Set <w> to <w>K; goto step 3.
	  6. Else <w>K is not in the string table.
		Output code(<w>);
		Put <w>K into the string table;
		Set <w> to K; Goto step 3.

	"At each execution of the basic step an acceptable input
	string <w> has been parsed off.  The next character K is
	read and the extended string <w>K is tested to see if it
	exists in the string table.  If it is there, then the
	extended string becomes the parsed string <w> and the
	step is repeated.  If <w>K is not in the string table,
	then it is entered, the code for the successfully
	parsed string <w> is put out as compressed data, the
	character K becomes the beginning of the next string,
	and the step is repeated."

	The decompression algorithm translates each received
	code into a prefix string and extension [suffix] character.
	The extension character is stored (in a push-down stack),
	and the prefix translated again, until the prefix is a
	single character, which completes decompression of this
	code.  The entire code is then output by popping the
	stack.

	"An update to the string table is made for each code received
	(except the first one).  When a code has been translated,
	its final character is used as the extension character,
	combined with the prior string, to add a new string to
	the string table.  This new string is assigned a unique
	code value, which is the same code that the compressor
	assigned to that string.  In this way, the decompressor
	incrementally reconstructs the same string table that
	the decompressor used.... Unfortunately ... [the algorithm]
	does not work for an abnormal case.

	The abnormal case occurs whenever an input character string
	contains the sequence K<w>K<w>K, where K<w> already
	appears in the compressor string table."

	The decompression algorithm, augmented to handle
	the abnormal case, is as follows:

	  1. Read first input code;
	     Store in CODE and OLDcode;
	     With CODE = code(K), output(K);  FINchar = K;
	  2. Read next code to CODE; INcode = CODE;
	     If at end of file, exit;
	  3. If CODE not in string table (special case) then
		Output(FINchar);
		CODE = OLDcode;
		INcode = code(OLDcode, FINchar);
	
	  4. If CODE == code(<w>K) then
		Push K onto the stack;
		CODE == code(<w>);
		Goto 4.

	  5. If CODE == code(K) then
		Output K;
		FINchar = K;

	  6. While stack not empty
		Output top of stack;
		Pop stack;

	  7. Put OLDcode,K into the string table.
	     OLDcode = INcode;
	     Goto 2.

	The algorithm as implemented here introduces two additional
	complications.

	The actual codes are transmitted using a variable-length
	encoding.  The lowest-level routines increase the number
	of bits in the code when the largest possible code is
	transmitted.

	Periodically, the algorithm checks that compression is
	still increasing.  If the ratio of input bytes to output
	bytes decreases, the entire process is reset.  This can
	happen if the characteristics of the input file change.

VMS Private File Structure

	In VMS Private mode, the compressed data file contains
	a variable-length (but compressed) file header with the
	file "attributes" needed by the operating system to
 	construct the file.  This allows the decompression
	program to recreate the file in its original format,
	which is essential if ISAM databases are compressed.

	The overall file format is as follows:
	.lm +8
	.p -8
	LZ_SOH	"start of header" signal (this value cannot appear
	in user data).

	A variable-length data record (maximum 256 bytes)
	containing the header name, followed by whitespace, followed
	by header-specific information.  In this case, the name
	record will contain the string "vms$attributes" followed
	by the number of bytes in the attribute data block.
	(I assume that the name record will consist of a facility
	name, such as "vms", followed by a dollar sign, followed
	by a facility-unique word.)
	.p -8
	LZ_EOR	Signals "end of record".

	This is followed by a VMS file attributes record (generated
	by a VMS system library	routine).
	.p -8
	LZ_EOR	Signals "end of record" (optional).
	
	This is followed by another "header record" with additional
	information.  Currently, this is only used to transmit
	the "longest record length" field from the input file.

	Additional header records may be defined in the future.
	.p -8
	LZ_ETX	Signals "end of segment".
	.p -8
	ST_STX	Signals "start of text" (i.e., start of data file).

	This is followed by the user data file.
	.p -8
	LZ_ETX	Signals "end of segment"
	.p -8
	LZ_ETX	Two in a row signals "end of file".
	.s.lm -8
	Note that this format can easily be extended to include
	trailer records (with file counts and checksums) and/or
	multiple data files in one compressed file.

	Note also that the LZ_CLEAR code may appear in headers
	or data files to cause the decompression program to
	"readapt" to the characteristics of the input data.
	LZ_STX and LZ_SOH reset the compression algorithm.
	LZ_EOR does not.

Authors

	The algorithm is from "A Technique for High Performance
	Data Compression."  Terry A. Welch. IEEE Computer Vol 17,
	No. 6 (June 1984), pp 8-19.

	This revision is by Martin Minow.

	Unix Compress authors are as follows:
	.s.nf
	Spencer W. Thomas	(decvax!harpo!utah-cs!utah-gr!thomas)
	Jim McKie		(decvax!mcvax!jim)
	Steve Davies		(decvax!vax135!petsd!peora!srd)
	Ken Turkowski		(decvax!decwrl!turtlevax!ken)
	James A. Woods		(decvax!ihnp4!ames!jaw)
	Joe Orost		(decvax!vax135!petsd!joe)
	.s.f

#endif

/*
 * Compatible with compress.c, v3.0 84/11/27
 */

/*)BUILD
		$(PROGRAM) = lzcomp
		$(INCLUDE) = lz.h
		$(CPP) = 1
		$(RMS) = 1
		$(FILES) = { lzcmp1.c lzcmp2.c lzcmp3.c lzio.c lzvio.c }
*/

#include	"lz.h"

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef vms
#include types
#include stat
#endif

/*
 * These global parameters are written to the compressed file.
 * The decompressor needs them.  The initialized values are defaults
 * and are modified by command line arguments.
 */
short		maxbits = BITS;		/* settable max # bits/code	*/
code_int maxmaxcode = 1 << BITS; 	/* Totally largest code	*/
code_int	hsize = HSIZE;		/* Actual hash table size	*/

/*
 * Flags (command line arguments) to control compression.
 */
#ifdef vms
flag		export = EXPORT_VMS	/* Assume vms "private" mode	*/
			| EXPORT_HEADER
			| EXPORT_BLOCK
			| EXPORT_ENDMARKER;
#else
flag		export = EXPORT_UNIX	/* Assume Unix compatible mode	*/
			| EXPORT_HEADER
			| EXPORT_BLOCK
			| EXPORT_ENDMARKER;
#endif
flag		method = METHOD_LZ;	/* Of course			*/
flag		mode = MODE_TEXT;
flag		show = 0;
flag		background = FALSE;	/* TRUE (Unix) if detached	*/
readonly flag	is_compress = TRUE;	/* for lzdcl and lzvio		*/
long		fsize;			/* Input file size in bytes	*/
char		*infilename = NULL;	/* For error printouts		*/
char		*outfilename = NULL;	/* For openoutput and errors	*/
int		firstcode;		/* First code after internals	*/
int		fblksiz;		/* Not used by compress		*/
count_int	tot_incount = 0;	/* Total number of input bytes	*/
count_int	tot_outcount = 0;	/* Total number of output codes	*/
extern count_int in_count;
extern count_int out_count;
static time_t   start_walltime;		/* Wall time we started 	*/
static long	start_time;		/* Time we started (in msec)	*/
extern long	cputime();		/* Returns process time in msec	*/
STREAM		instream;
STREAM		outstream;
char_type	inbuffer[MAXIO];
char_type	outbuffer[MAXIO];
jmp_buf		failure;
#ifdef vms
static STREAM	mem_stream;
FDLSTUFF	*fdl_input;
FDLSTUFF	*fdl_output;
static struct dsc$descriptor fdl_descriptor;
#endif

main(argc, argv)
int		argc;
char		*argv[];
/*
 * Compress mainline
 */
{
	int		status;

#ifndef	decus
	/*
	 * background is TRUE if running detached from the command terminal.
	 */
	background = (signal(SIGINT, SIG_IGN) == SIG_IGN) ? TRUE : FALSE;
	if (!background)
	    background = !isatty(fileno(stderr));
	if (!background) {
	    if ((show & (SHOW_DEBUG | SHOW_SERIOUS_DEBUG | SHOW_IO_DEBUG)) != 0)
		signal(SIGINT, abort);
	    else {
		signal(SIGINT, interrupt);
		signal(SIGSEGV, address_error);
	    }
	}
#endif
	if ((status = setjmp(failure)) == 0) {
	    setup(argc, argv);		/* Command line parameters	*/
#if DEBUG
	    if (show & SHOW_DEBUG)
		dumpoptions();
#endif
	    openinput();		/* Open input, set instream	*/
	    getfilesize();		/* Get input file size		*/
	    gethashsize();		/* Get actual hash table size	*/
	    initialize();		/* Set maxbits and the like	*/
	    openoutput();		/* Open output file		*/
	    if ((show & SHOW_STATISTICS) != 0) {
		start_walltime = time(NULL);
		start_time = cputime();
	    }
	    put_magic_header();
	    init_compress(TRUE);
	    compress(&instream);
#ifdef vms
	    if ((export & EXPORT_VMS) != 0) {
		outputcode((code_int) LZ_ETX);
		outputcode((code_int) LZ_ETX);
		fdl_close(fdl_input);
	    }
	    else
#endif
	    if ((export & EXPORT_ENDMARKER) != 0) {
		outputcode((code_int) LZ_CLEAR);
		outputcode((code_int) LZ_CLEAR);
	    }
	    outputcode((code_int) -1);		/* Flush output buffers	*/
#ifdef vms
	    if ((export & EXPORT_VMS) != 0)
		fdl_close(fdl_output);
	    else
#endif
	    fclose(stdout);
	    if ((show & SHOW_STATISTICS) != 0)
		report();
	    exit(EXIT_SUCCESS);
	}
	else {
	    fprintf(stderr, "Error when compressing \"%s\" to \"%s\"\n",
		(infilename  == NULL) ? "<unknown>" : infilename,
		(outfilename == NULL) ? "<unknown>" : outfilename);
	    exit(status);
	}
}

report()
/*
 * Print verbose report.
 */
{
	start_time = cputime() - start_time;
	start_walltime = time(NULL) - start_walltime;
	tot_incount += in_count;
	tot_outcount += out_count;
	fprintf(stderr, "Input file:        %ld chars, %ld blocks.\n",
	    tot_incount, (tot_incount + 511L) / 512L);
	fprintf(stderr, "Output file:       %ld chars, %ld blocks.\n",
	    tot_outcount, (tot_outcount + 511L) / 512L);
	if (tot_outcount > 0) {
	    divout("Compression ratio: ",
		(long) tot_incount, (long) tot_outcount, "");
	    divout(" (",
		((long) tot_incount - (long) tot_outcount) * 100,
		(long) tot_incount, "%).\n");
	}
	fprintf(stderr,
		"%ld.%02ld seconds (process time) for compression",
		start_time / 1000L, (start_time % 1000L) / 10L);
	if (start_time > 0) {
	    divout(";  ", (long) tot_incount * 10L,
		(start_time + 50L) / 100L,
		" input bytes/sec.");
	}
	fprintf(stderr,
		"\n%ld seconds (elapsed time) for compression",
		start_walltime);
	if (start_walltime > 0) {
	    divout(";  ", (long) tot_incount, start_walltime,
		" input bytes/sec.");
	}
	fprintf(stderr, "\n");
}

divout(leader, numer, denom, trailer)
char		*leader;
long		numer;
long		denom;
char		*trailer;
/*
 * Print numer/denom without floating point on small machines.
 */
{
	register int 	negative;

	negative = FALSE;
	if (numer < 0) {
	    numer = -numer;
	    negative = TRUE;
	}
	if (denom < 0) {
	    denom = -denom;
	    negative = !negative;
	}
	fprintf(stderr, "%s%s%ld.%02ld%s",
	    leader, (negative) ? "-" : "",
	    numer / denom, ((numer % denom) * 100L) / denom, trailer);
}

static
initialize()
/*
 * Mung some global values.
 */
{
	if (maxbits < INIT_BITS)	/* maxbits is set by the -M 	*/
	    maxbits = INIT_BITS;	/* option.  Make sure it's	*/
	if (maxbits > BITS)		/* within a reasonable range	*/
	    maxbits = BITS;
	maxmaxcode = 1 << maxbits;	/* Truly biggest code		*/
	if ((export & EXPORT_VMS) != 0)
	    firstcode = LZ_FIRST;	/* VMS private			*/
	else if ((export & EXPORT_BLOCK) != 0)
	    firstcode = LZ_CLEAR + 1;	/* Default			*/
	else
	    firstcode = 256;		/* Backwards compatible		*/
}

put_magic_header()
/*
 * Write the magic header bits.
 */
{
	if ((export & (EXPORT_VMS | EXPORT_HEADER)) == EXPORT_HEADER) {
	    PUT(HEAD1_MAGIC, &outstream);
	    PUT(HEAD2_MAGIC, &outstream);
	    PUT(maxbits
		| (((export & EXPORT_HEADER) != 0) ? BLOCK_MASK : 0)
		| (((mode   & MODE_DELTA)    != 0) ? DIFF_MASK  : 0),
		&outstream);
	}
#ifdef vms
	else if ((export & EXPORT_VMS) != 0) {
	    char text[256];
	    /*
	     * VMS private mode (with attribute block)
	     */
	    PUT(HEAD1_MAGIC, &outstream);
	    PUT(VMS_HEAD2_MAGIC, &outstream);
	    PUT(maxbits
		| BLOCK_MASK
		| (((mode & MODE_DELTA) != 0) ? DIFF_MASK  : 0),
		&outstream);
	    PUT(firstcode - 0x100, &outstream);
	    init_compress();
	    outputcode(LZ_SOH);
#if DEBUG
	    if (strlen(ATT_NAME) != ATT_SIZE) {
		fprintf("\"%s\", expected %d, got %d\n",
		    ATT_NAME, ATT_SIZE, strlen(ATT_NAME));
	    }
#endif
	    sprintf(text, "%s%d;", ATT_NAME, fdl_descriptor.dsc$w_length);
	    mem_compress(text, strlen(text));
	    outputcode(LZ_EOR);
	    mem_compress(fdl_descriptor.dsc$a_pointer,
			 fdl_descriptor.dsc$w_length);
	    fdl_free(&fdl_descriptor);
	    if (fdl_input->xabfhc.xab$w_lrl != 0) {
		outputcode(LZ_EOR);
		sprintf(text, "xabfhc$w_lrl %d", fdl_input->xabfhc.xab$w_lrl);
		if ((show & SHOW_FDL) != 0)
		    fprintf(stderr, "Additional header: %s\n", text);
		mem_compress(text, strlen(text));
	    }
	    outputcode(LZ_ETX);
	    outputcode(LZ_STX);
	}
#endif
}

#ifdef vms
mem_compress(datum, length)
char_type	*datum;
int		length;
/*
 * Compress from memory
 */
{
	mem_stream.bp = mem_stream.bstart = datum;
	mem_stream.bsize = length;
	mem_stream.bend = datum + length;
	mem_stream.func = lz_eof;
	compress(&mem_stream);
}
#endif

/*
 * This routine is used to tune the hash table size according to
 * the file size.  If the filesize is unknown, fsize should be
 * set to zero.
 */

typedef struct TUNETAB {
    long	fsize;
    code_int	hsize;
} TUNETAB;

static readonly TUNETAB tunetab[] = {
#if HSIZE > 5003
    {	1 << 12,	 5003	},
#endif
#if HSIZE > 9001
    {	1 << 13,	 9001	},
#endif
#if HSIZE > 18013
    {	1 << 14,	18013	},
#endif
#if HSIZE > 35023
    {	1 << 15,	35023	},
    {	  47000,	50021	},
#endif
    {	      0,	    0	},
};

static
gethashsize()
/*
 * Tune the hash table parameters for small files.
 * We don't have a good way to find the file size on vms V3.
 * fsize is set to zero if we can't find it.
 */
{
	register TUNETAB	*tunep;

	hsize = HSIZE;
	if (fsize > 0) {
	    for (tunep = tunetab; tunep->fsize != 0; tunep++) {
		if (fsize < tunep->fsize) {
		    hsize = tunep->hsize;
		    break;
		}
	    }
	}
}

static
getfilesize()
/*
 * Set fsize to the input filesize (in bytes) if possible.
 * Magic for all operating systems.
 */
{
#ifdef	rsx
	extern char	f_efbk;	/* F.EFBK -- highest block in file	*/
#define	fdb(p,offset)	(stdin->io_fdb[((int) &p + offset)] & 0xFF)
#define efbk(offset)	fdb(f_efbk, offset)
	extern char	f_rtyp;	/* F.RTYP -- Record type		*/
	extern char	f_ratt;	/* F.RATT -- Record attributes		*/
	/*
	 * Note: Block number is stored high-order word first.
	 */
	fsize = efbk(2)
	    + (efbk(3) << 8)
	    + (efbk(0) << 16)
	    + (efbk(1) << 24);
	fsize *= 512;
#endif
#ifdef	rt11
	fsize = stdin->io_size;		/* Set by Decus C		*/
	fsize *= 512;
#endif
#ifdef	vms
	struct stat	statbuf;

	fsize = 0;
	if ((export & EXPORT_VMS) == 0) {
	    if (fstat(fileno(stdin), &statbuf) == 0)
		fsize = (long) statbuf.st_size;
	}
	else {
	    fsize = (long) fdl_fsize(fdl_input);
	}
#endif
#ifdef	unix
	struct stat	statbuf;

	fsize = 0;
	if (fstat(fileno(stdin), &statbuf) == 0)
	    fsize = (long) statbuf.st_size;
#endif
}

static readonly char *helptext[] = {
	"The following options are valid:",
	"-B\tBinary file (important on VMS/RSX, ignored on Unix)",
	"-M val\tExplicitly set the maximum number of code bits",
	"-S\tCompress difference between bytes",
	"-V val\tPrint status information or debugging data; bits are:",
	   "  -V 0\tDon't print anything",
	   "  -V 1\tPrint a summary of the operation",
#ifdef vms
	   "  -V 2\tPrint compression progress reports",
	   "  -V 4\tPrint FDL information (for VMS export mode)",
#endif
#if DEBUG
	   "  -V 8\tPrint some debug information.",
	   "  -V 16\tPrint too much debug information",
	   "  -V 32\tDump the decompressed files",
#endif
	"-X val\tSet export (compatiblity) mode:",
#ifdef vms
	"  -X 0\tExplicitly choose VMS Private mode",
#endif
	"  -X 1\tOutput format is compatible with Unix compress V3.0,",
	      "\texcept that two endcodes are written (default if -X",
	      "\tspecified with no value)",
	"  -X 2\toutput format is compatible with Unix compress V3.0",
	"  -X 3\tCompatible with Unix compress 3.0, block compression",
	      "\tsuppressed.",
	"  -X 4\tNo header (file is readable by old compress)",
	NULL,
};

static
setup(argc, argv)
int		argc;
char		*argv[];
/*
 * Get parameters and open files.  Exit fatally on errors.
 */
{
	register char	*ap;
	register int	c;
	char		**hp;
	auto int	i;
	int		j;
	int		temp;

#ifdef	vms
	argc = getredirection(argc, argv);
	/*
	 * Prescan to see whether we must do a DCL parse.
	 */
	for (j = FALSE, i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		j = TRUE;
		break;
	    }
	}
	if (j == FALSE) {
	    if ((i = lzdcl(argc, argv)) != SS$_NORMAL)
		exit(i);
	    return;
	}
#endif
	for (i = j = 1; i < argc; i++) {
	    ap = argv[i];
	    if (*ap++ != '-' || *ap == EOS)	/* Filename?		*/
		argv[j++] = argv[i];		/* Just copy it		*/
	    else {
		while ((c = *ap++) != EOS) {
		    if (islower(c))
			c = toupper(c);
		    switch (c) {
		    case 'B':
			mode &= ~MODE_TEXT;
			mode |= MODE_BINARY;
			break;

		    case 'M':
			maxbits = getvalue(ap, &i, argv);
			if (maxbits < MIN_BITS) {
			    fprintf(stderr, "Illegal -M value\n");
			    goto usage;
			}
			break;

		    case 'S':
			mode |= MODE_DELTA;
			break;

		    case 'V':
			show = getvalue(ap, &i, argv);
			break;

		    case 'X':
			switch ((temp = getvalue(ap, &i, argv))) {
			case 0:	export = EXPORT_VMS
					| EXPORT_BLOCK
					| EXPORT_HEADER
					| EXPORT_ENDMARKER;
				break;
			case 1:	export = EXPORT_UNIX
					| EXPORT_BLOCK
					| EXPORT_HEADER
					| EXPORT_ENDMARKER;
				break;

			case 2: export = EXPORT_UNIX
					| EXPORT_BLOCK
					| EXPORT_HEADER;
				break;
			case 3:	export = EXPORT_UNIX
					| EXPORT_HEADER;
				break;
			case 4:	export = EXPORT_UNIX;
				break;
			default:
			    fprintf(stderr, "Illegal -X value: %d\n", temp);
			    goto usage;
			}
			break;

		    default:
			fprintf(stderr, "Unknown option \"%c\" in \"%s\"\n",
				c, argv[i]);
usage:			for (hp = helptext; *hp != NULL; hp++)
			    fprintf(stderr, "%s\n", *hp);
			FAIL("usage", EXIT_FAILURE);
		    }				/* Switch on options	*/
		}				/* Everything for -xxx	*/
	    }					/* If -option		*/
	}					/* For all argc's	*/
	/*  infilename = NULL; */		/* Set "stdin"  signal	*/
	/* outfilename = NULL; */		/* Set "stdout" signal	*/
	switch (j) {				/* Any file arguments?	*/
	case 3:					/* both files given	*/
	    if (!streq(argv[2], "-"))		/* But - means stdout	*/
		outfilename = argv[2];
	case 2:					/* Input file given	*/
	    if (!streq(argv[1], "-"))
		infilename = argv[1];
	    break;

	case 0:					/* None!		*/
	case 1:					/* No file arguments	*/
	    break;

	default:
	    fprintf(stderr, "Too many file arguments\n");
	    FAIL("too many files", EXIT_FAILURE);
	}
}

static int
getvalue(ap, ip, argv)
register char		*ap;
int			*ip;
char			*argv[];
/*
 * Compile a "value".  We are currently scanning *ap, part of argv[*ip].
 * The following are possible:
 *	-x123		return (123) and set *ap to EOS so the caller
 *	ap^		cycles to the next argument.
 *
 *	-x 123		*ap == EOS and argv[*ip + 1][0] is a digit.
 *			return (123) and increment *i to skip over the
 *			next argument.
 *
 *	-xy or -x y	return(1), don't touch *ap or *ip.
 *
 * Note that the default for "flag option without value" is 1.  This
 * can only cause a problem for the -M option where the value is
 * mandatory.  However, the result of 1 is illegal as it is less
 * than INIT_BITS.
 */
{
	register int	result;
	register int	i;

	i = *ip + 1;
	if (isdigit(*ap)) {
	    result = atoi(ap);
	    *ap = EOS;
	}
	else if (*ap == EOS
	      && argv[i] != NULL
	      && isdigit(argv[i][0])) {
	    result = atoi(argv[i]);
	    *ip = i;
	}
	else {
	    result = 1;
	}
	return (result);
}

openinput()
{
#ifdef decus
	if (infilename == NULL) {
	    infilename = malloc(256 + 1);
	    fgetname(stdin, infilename);
	    infilename = realloc(infilename, strlen(infilename) + 1);
	}
	else {
	    if (freopen(infilename,
			((mode & MODE_BINARY) != 0) ? "rb" : "r",
			stdin) == NULL) {
		perror(infilename);
		FAIL("can't reopen input", ERROR_EXIT);
	    }
	}

#else
#ifdef vms
	int stdinflg = FALSE, isopened = FALSE;

	if (infilename == NULL) {
	    stdinflg = TRUE;
	    infilename = malloc(256 + 1);
	    fgetname(stdin, infilename);
	    infilename = realloc(infilename, strlen(infilename) + 1);
	}

	if ((export & EXPORT_VMS) != 0) {
	    if ((fdl_input = fdl_open(infilename, &fdl_descriptor)) == NULL) {
		if (!$VMS_STATUS_SUCCESS(fdl_status))
		    FAIL("can't fdl_open", fdl_status);
		fprintf(stderr, "Cannot open \"%s\"", infilename);
		fprintf(stderr, "in vms private format; trying export format.\n");
		export &= ~EXPORT_VMS;
		export |= EXPORT_UNIX;
	    }
	    else {
	        isopened = TRUE;
	    	fclose(stdin);
	    	stdin = NULL;
	    	if ((show & SHOW_FDL) != 0) {
		    fprintf(stderr, "FDL information for \"%s\"\n", infilename);
		    fdl_dump(&fdl_descriptor, stderr);
	    	}
	    }
	}

	if (!isopened && !stdinflg) {
	    if ((stdin = freopen(infilename,
		((mode & MODE_BINARY) != 0) ? "rb" : "r", stdin)) == NULL) {
		perror(infilename);
		FAIL("can't open input (export)", ERROR_EXIT);
	    }
	}

#else
	if (infilename == NULL)
	    infilename = "<stdin>";
	else {
	    if (freopen(infilename, "r", stdin) == NULL) {
		perror(infilename);
		FAIL("can't open input", ERROR_EXIT);
	    }		    
	}
#endif
#endif

	instream.bp = instream.bend = NULL;
	instream.bstart = inbuffer;
	instream.bsize = sizeof inbuffer;
	instream.func = lz_fill;
#ifdef vms
	instream.filsz = 0;
	instream.eofhack = 0;
#endif
}

openoutput()
/*
 * Open the output file (after the input file has been opened).
 * if outfilename == NULL, it's already open on stdout.
 */
{
	int ttytest = TRUE;

#ifdef	vms
	int stdoutflg = FALSE;	

	if (outfilename == NULL) {
	    stdoutflg = TRUE;
	    outfilename = malloc(256 + 1);
	    fgetname(stdout, outfilename);
	    outfilename = realloc(outfilename, strlen(outfilename) + 1);
	}
	    
 	if ((export & EXPORT_VMS) != 0) {
	    ttytest = FALSE;	/* can't do terminal test below	*/
	    if ((fdl_output = fdl_create(NULL, outfilename, 0)) == NULL) {
		fprintf(stderr, "Can't create \"%s\" (vms private)\n", outfilename);
		FAIL("can't fdl_create", fdl_status);
	    }
	    fclose(stdout);
	    stdout = NULL;
	}
 
	else if (!stdoutflg) {
	    if (freopen(outfilename, "w", stdout) == NULL) {
	    	perror(outfilename);
	    	FAIL("can't create", ERROR_EXIT);
	    }
	}

#else
#ifdef decus
	if (outfilename == NULL) {
	    outfilename = malloc(256 + 1);
	    fgetname(stdout, outfilename);
	    outfilename = realloc(outfilename, strlen(outfilename) + 1);
	}
	else {
	    if (freopen(outfilename, "wb", stdout) == NULL) {
		perror(outfilename);
		FAIL("can't create", ERROR_EXIT);
	    }
	}

#else
	if (outfilename == NULL) 
	    outfilename = "<stdout>";
	else {
	    if (freopen(outfilename, "w", stdout) == NULL) {
		perror(outfilename);
		FAIL("can't create", ERROR_EXIT);
	    }
	}
#endif
#endif

	if (ttytest && isatty(fileno(stdout))) {
	    fprintf(stderr, "%s: is a terminal.  We object.\n",
		outfilename);
	    FAIL("can't create", EXIT_FAILURE);
	}

	outstream.bp = outstream.bstart = outbuffer;
	outstream.bend = outbuffer + sizeof outbuffer;
	outstream.bsize = sizeof outbuffer;
	outstream.func = lz_flush;
}


/*
 *			l z i o . c
 *
 * I/O buffer management.  All input/output I/O is done through these
 * routines (and the macros in lz.h).  The rules of the game are:
 *
 * input via GET() and getbuf().
 *	GET returns an 8-bit byte, or -1 on eof/error.
 *	getbuf() returns the number of things gotten, or -1 on eof/error.
 *	No return on error: longjmp's to the main-line.
 *
 * output via PUT() and lz_putbuf().
 *	No return on error: longjmp's to the main-line.
 * flush output by lz_flush() before closing files -- or you'll lose data.
 *
 * Edit History:
 *
 * 12-Sep-1993 	dp	Assorted fixes and cleanups for VMS
 *			- Add a few "return (0)" statements to keep
 *			  fussy compilers happy.
 *			- Use fwrite() on VMS when writing fixed-length
 *			  records - it's faster.
 *			- In decompress, take notice of the actual file
 *			  size; EOF may be part-way through the last record
 *			  due to an artefact of the transfer from Unix
 */

/*LINTLIBRARY*/

#include	"lz.h"
#ifdef vms
#include	<rmsdef.h>
extern FDLSTUFF *fdl_input;
extern FDLSTUFF *fdl_output;
extern int	fdl_status;
#endif

int
lz_fill(s)
register STREAM		*s;
{
	register int	i;
	extern char	*infilename;

#ifdef vms
	if ((export & EXPORT_VMS) == 0) {
	    long t;
	    i = fread((char *) s->bstart, 1, s->bsize, stdin);
	    if (ferror(stdin)) {
		perror(infilename);
		FAIL("read error (export)", ERROR_EXIT);
	    }
	    if (i > 0 && s->eofhack != 0 && (t = ftell(stdin)) > s->filsz) {
		i -= t - s->filsz;
		if (i <= 0) FAIL("whoops, screwed up at EOF", ERROR_EXIT);
	    }
	}
	else {	
	    i = fdl_read(s->bstart, s->bsize, fdl_input);
	    if (i < 0 && fdl_status != RMS$_EOF)
		fdl_message(fdl_input, "Read error (vms private)");
	}

#else
#ifdef unix
	i = read(fileno(stdin), (char *) s->bstart, s->bsize);
	if (i < 0) {
	    perror(infilename);
	    FAIL("read error", ERROR_EXIT);
	}

#else
	i = fread((char *) s->bstart, 1, s->bsize, stdin);
	if (ferror(stdin)) {
	    perror(infilename);
	    FAIL("read error", ERROR_EXIT);
	}
#endif
#endif

	if (i <= 0)
	    return (EOF);
	else {
	    s->bp = s->bstart;
	    s->bend = &s->bstart[i];
#if UCHAR
	    return (*s->bp++);
#else
	    return (*s->bp++ & 0xFF);
#endif
	}
}

lz_flush(s)
register STREAM	*s;
{
	register int	count;
	extern char	*outfilename;

	count = s->bp - s->bstart;
#if DEBUG
	if (!is_compress && (show & SHOW_IO_DEBUG) != 0) {
	    fprintf(stderr, "lz_flush %d:  ", count);
	    dumptext(s->bstart, count, stderr);
	}
#endif

#ifdef vms
	if ((export & EXPORT_VMS) == 0) {
	    if (is_compress || mode & MODE_FIXED) {
		fwrite((char *) s->bstart, count, 1, stdout);
	    }
	    else {
		register char *bp, *bend;
		for (bp = s->bstart, bend = bp + count; bp < bend; bp++)
		    putchar(*bp);
	    }
	    if (ferror(stdout)) {
		perror(outfilename);
		FAIL("write error", ERROR_EXIT);
	    }
	}
	else {
	    if (fdl_write((char *) s->bstart, count, fdl_output) == -1) {
		fdl_message(fdl_output, "Write error");
		FAIL("write (fdl) error", ERROR_EXIT);
	    }
	}

#else
#ifdef unix
	if (write(fileno(stdout), (char *) s->bstart, count) != count) {
	    perror(outfilename);
	    FAIL("write error", ERROR_EXIT);
	}

#else
	fwrite((char *) s->bstart, 1, count, stdout);
	if (ferror(stdout)) {
	    perror(outfilename);
	    FAIL("write error", ERROR_EXIT);
	}
#endif
#endif

	s->bp = s->bstart;
}

int
lz_getbuf(buffer, count, s)
char_type		*buffer;
int			count;
register STREAM		*s;
/*
 * Read a block of data -- be clever.  Return number gotten, or -1
 * on eof.
 */
{
	register char_type	*bp;		/* -> buffer		*/
	register char_type	*ip;		/* -> I/O buffer	*/
	register char_type	*ep;		/* End of segment	*/
	register int		remaining;	/* Size of segment	*/
	int			datum;

	if (count == 0)				/* Shouldn't happen	*/
	    return (0);
	bp = buffer;
	while (--count >= 0) {
	    if ((datum = GET(s)) == EOF)	/* Maybe fill LZ buff	*/
		break;
	    *bp++ = datum;
	    remaining = s->bend - (ip = s->bp);
	    if (remaining > count)
		remaining = count;
	    ep = &ip[remaining];
	    while (ip < ep)
		*bp++ = *ip++;
	    count -= remaining;
	    s->bp = ip;				/* Refresh buffer	*/
	}
	return ((bp == buffer) ? -1 : bp - buffer);
}

int
lz_putbuf(bp, count, s)
register char_type	*bp;
int			count;
register STREAM		*s;
/*
 * Write a block of data -- be clever.
 */
{
	register char_type	*op;		/* -> I/O buffer	*/
	register char_type	*ep;		/* End of segment	*/
	register int		remaining;	/* Size of segment	*/

	while (--count >= 0) {
	    PUT(*bp++, s);			/* Forces a buffer	*/
	    remaining = s->bend - (op = s->bp);
	    if (remaining > count)
		remaining = count;
	    ep = &op[remaining];
	    while (op < ep)
		*op++ = *bp++;
	    count -= remaining;
	    s->bp = op;				/* Refresh buffer	*/
	}

	return (0);
}

/*ARGUSED*/
int
lz_eof(s)
STREAM		*s;
/*
 * Dummy routine for read from memory -- returns EOF.
 */
{
#ifdef decus
	return (s, EOF);
#else
	return (EOF);
#endif
}

int
lz_fail(s)
STREAM		*s;
/*
 * Dummy routine for write to memory -- called if buffer fills.
 */
{
	fprintf(stderr, "Memory buffer [%d bytes] filled -- fatal.\n",
		s->bsize);
	FAIL("crash (lz_fail)", EXIT_FAILURE);
	return (0);
}

int
lz_dummy(s)
STREAM		*s;
/*
 * Dummy routine for write to memory -- writes to the bit-bucket.
 */
{
	s->bp = s->bstart;
	return (0);
}

#ifndef decus
/*
 * Signal error handlers.
 */
#ifdef vms
#define unlink	delete
#endif

interrupt()
{
	if (outfilename != NULL
	 && !streq(outfilename, "<stdout>"))
	    unlink(outfilename);
	exit(EXIT_FAILURE);
}

address_error()
{
	if (!is_compress)
	    fprintf(stderr, "Decompress: corrupt input file\n");
	else {
	    fprintf(stderr, "fatal address error\n");
	}
#ifdef vms
	lib$signal(SS$_ACCVIO);
#else
	interrupt();
#endif
}
#endif

/*
 * getredirection() is intended to aid in porting C programs
 * to VMS (Vax-11 C) which does not support '>' and '<'
 * I/O redirection.  With suitable modification, it may
 * useful for other portability problems as well.
 */

#ifdef	vms

int
getredirection(argc, argv)
int		argc;
char		**argv;
/*
 * Process vms redirection arg's.  Exit if any error is seen.
 * If getredirection() processes an argument, it is erased
 * from the vector.  getredirection() returns a new argc value.
 *
 * Warning: do not try to simplify the code for vms.  The code
 * presupposes that getredirection() is called before any data is
 * read from stdin or written to stdout.
 *
 * Normal usage is as follows:
 *
 *	main(argc, argv)
 *	int		argc;
 *	char		*argv[];
 *	{
 *		argc = getredirection(argc, argv);
 *	}
 */
{
	register char		*ap;	/* Argument pointer	*/
	int			i;	/* argv[] index		*/
	int			j;	/* Output index		*/
	int			file;	/* File_descriptor 	*/

	for (j = i = 1; i < argc; i++) {   /* Do all arguments	*/
	    switch (*(ap = argv[i])) {
	    case '<':			/* <file		*/
		if (freopen(++ap, "r", stdin) == NULL) {
		    perror(ap);		/* Can't find file	*/
		    exit(ERROR_EXIT);	/* Is a fatal error	*/
		}
		break;

	    case '>':			/* >file or >>file	*/
		if (*++ap == '>') {	/* >>file		*/
		    /*
		     * If the file exists, and is writable by us,
		     * call freopen to append to the file (using the
		     * file's current attributes).  Otherwise, create
		     * a new file with "vanilla" attributes as if
		     * the argument was given as ">filename".
		     * access(name, 2) is TRUE if we can write on
		     * the specified file.
		     */
		    if (access(++ap, 2) == 0) {
			if (freopen(ap, "a", stdout) != NULL)
			    break;	/* Exit case statement	*/
			perror(ap);	/* Error, can't append	*/
			exit(ERROR_EXIT); /* After access test	*/
		    }			/* If file accessable	*/
		}
		/*
		 * On vms, we want to create the file using "standard"
		 * record attributes.  create(...) creates the file
		 * using the caller's default protection mask and
		 * "variable length, implied carriage return"
		 * attributes. dup2() associates the file with stdout.
		 */
		if ((file = creat(ap, 0, "rat=cr", "rfm=var")) == -1
		 || dup2(file, fileno(stdout)) == -1) {
		    perror(ap);		/* Can't create file	*/
		    exit(ERROR_EXIT);	/* is a fatal error	*/
		}			/* If '>' creation	*/
		break;			/* Exit case test	*/

	    default:
		argv[j++] = ap;		/* Not a redirector	*/
		break;			/* Exit case test	*/
	    }
	}				/* For all arguments	*/
	argv[j] = NULL;			/* Terminate argv[]	*/
	return (j);			/* Return new argc	*/
}
#endif

#if 1 || DEBUG

int		col;

readonly char *lz_names[] = {
    "LZ_CLEAR", "LZ_SOH", "LZ_STX", "LZ_EOR", "LZ_ETX", "???"
};

dumphex(buffer, count, fd)
register char_type	*buffer;
register int		count;
FILE			*fd;
{
	if (col > 0) {
	    putc('\n', fd);
	    col = 0;
	}
	fprintf(fd, "%2d:", count);
	while (--count >= 0) {
	    fprintf(fd, " %02x", (int) (*buffer++ & 0xFF));
	}
	fprintf(fd, "\n");
}

dumptext(buffer, count, fd)
register char_type	*buffer;
int			count;
FILE			*fd;
{
	extern char	*dumpchar();

	putc('"', fd);
	while (--count >= 0)
	    fputs(dumpchar((int) *buffer++), fd);
	fputs("\"\n", fd);
}

char *
dumpchar(c)
register int	c;
/*
 * Make a character printable.  Returns a static pointer.
 */
{
	static char	dump_buffer[8];

	c &= 0xFF;
	if (isascii(c) && isprint(c)) {
	    dump_buffer[0] = c;
	    dump_buffer[1] = EOS;
	}
	else {
	    switch (c) {
	    case '\n':	return ("<LF>");
	    case '\t':	return ("<TAB>");
	    case '\b':	return ("<BS>");
	    case '\f':	return ("<FF>");
	    case '\r':	return ("<CR>");
	    }
	    sprintf(dump_buffer, "<x%02x>", c);
	}
	return (dump_buffer);
}
#endif

/*
 * Cputime returns the elapsed process time (where available) in msec.
 * Note: Unix doesn't seem to have a good way to determine ticks/sec.
 */

#ifdef	decus
#include	<timeb.h>

long
cputime()
{
	struct timeb		buf;
	static struct timeb	origin;
	long			result;
	int			msec;

	if (origin.time == 0)
	    ftime(&origin);
	ftime(&buf);
	result = (buf.time - origin.time) * 1000;
	msec = ((int) buf.millitm) - ((int) origin.millitm);
	return (result + ((long) msec));
}
#else
#ifdef vms
#include	<types.h>
struct tms {
	time_t	tms_utime;
	time_t	tms_stime;
	time_t	tms_uchild;	/* forgot the */
	time_t	tms_uchildsys;	/* real names */
};
#define HERTZ	100.0				/* 10 msec units	*/
#else
#include	<sys/types.h>
#include	<sys/times.h>
#ifndef HERTZ
#define HERTZ	60.0				/* Change for Europe	*/
#endif
#endif

long
cputime()
{
	struct tms	tms;
	double		temp;
	long		result;

	times(&tms);
	result = tms.tms_utime + tms.tms_stime;
	temp = result * 1000.0 / HERTZ;		/* Time in msec.	*/
	result = temp;
	return (result);
}
#endif


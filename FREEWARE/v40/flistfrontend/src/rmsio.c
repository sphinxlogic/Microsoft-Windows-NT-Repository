#ifndef NO_IDENT
static char *Id = "$Id: rmsio.c,v 1.11 1995/10/28 13:52:43 tom Exp $";
#endif

/*
 * Title:	rmsio.c
 * Author:	T.E.Dickey
 * Created:	11 Sep 1984
 * Last update:
 *		28 Oct 1995, set flags to allow shared-read/write.
 *		05 Jun 1995, prototypes
 *		28 Feb 1989, print status-code in 'rerror()' if room.
 *		16 Jun 1985, broke out CC2.0/CC1.5 difference as 'rabrfa_???'
 *		15 Jun 1985, typed 'calloc'.  CC2.0 declares RAB's rfa as
 *			     short[3], rather than long,short.
 *		10 Apr 1985, added 'rclear', augmented 'rerror' by piping to
 *			     'warn', and making better message for file-not-
 *			     found.
 *		09 Apr 1985, broke 'ropen' into 'ropen2' to provide defaults.
 *		18 Feb 1985, polished RFA-defs a bit, fixed error return in
 *			     'rseek'.
 *		13 Feb 1984, in 'erstat', use global error if nil-pointer
 *		12 Feb 1985, added 'erstat' entry (cf: 'ferror') to provide
 *			     some error-checking.
 *		04 Feb 1985, added block-I/O (if uppercase open-mode).  Fixed
 *			     return from 'rclose', also deal only in RFILE
 *			     (instead of assuming RAB-argument).
 *		10 Nov 1984, began coding for output-file (CR-format only)
 *		23 Oct 1984, no longer need dummy newline on 'rgetr'
 *		28 Sep 1984, latch-only status in file-close
 *		17 Sep 1984
 *
 * Function:	This module is designed as a look-alike for the Unix procedures
 *		'fopen', 'fgets', 'fclose', 'ftell' and 'fseek', but with the
 *		special application of reading VAX/VMS RMS-managed files by
 *		record.  (The existing Unix-support knows only about carriage-
 *		control files.)  By supplying a new procedure 'getr' (instead
 *		of 'gets'), we read records instead of strings.  The calling
 *		syntax is designed to permit simple replacement.
 *
 *		The Unix FILE pointer is replaced by a pointer to RMS
 *		structures.  The calling format is preserved, since we wish
 *		to be able to take the calling code to other systems.  The
 *		crucial difference between VAX/VMS and Unix is that
 *
 *			'ftell' on VMS returns the address of the last record
 *				which has been read.
 *			'ftell' on Unix returns the address of the next byte
 *				which will be read.
 *
 *		'ungetc' does not work properly in this environment.
 *
 * Entry:	ropen:	(fopen)  Open an RMS-file, returning pointer to buffers
 *		ropen2:	(?)	 Open a file, providing default-specification.
 *		rgetr:	(fgets)  Read a record from RMS-file
 *		rputr:	(fputs)	 Write a record to RMS-file
 *		rtell:	(ftell)  Returns offset into file (record-address)
 *		rseek:	(fseek)  Position to specified record-address
 *		rclose:	(fclose) Close an RMS-file, releasing I/O buffers
 *		erstat:	(?)	 Format an error message, if severe
 *		rerror:	(perror) Print message for last error
 *		rsize:	(?)	 Returns size of largest record of input.
 */

#include	<starlet.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	<rms.h>
#include	<stsdef.h>
#include	<descrip.h>

#include	<ctype.h>

#include	"bool.h"
#include	"warning.h"
#include	"rabrfa.h"
#include	"rmsio.h"
#include	"rmsinit.h"
#include	"sysutils.h"

#define	DEFAULT_RSIZE	512

#define	latch		if (!rmsio$err) rmsio$err = status
#define	save_st		rmsio$err = status;

#define	CHECK(f)	sys(f)	{save_st; goto failed;}
#define	NONZERO(f)	sys(f)	{save_st; return(0);}
#define	ZERO(f)		sys(f)	{save_st; return(status);}

#define	CHECK2(f)	sys(f)	{latch; goto failed;}

/*
 * Module-level data:
 */
static	unsigned rmsio$err = 0;
static	char	rmsio$nam[NAM$C_MAXRSS] = ""; /* Last filename used in parse */

#define	zFAB	z->fab
#define	zNAM	z->nam
#define	zRAB	z->rab

#define	BLOCKED	(blocked ? FAB$M_BRO : 0)
#define	isBLOCK	(zFAB.fab$b_fac & FAB$M_BRO)

/* <ropen>:
 * Open a file for record I/O:
 *
 * mode_:
 *	"r" - read, record
 *	"w" - write, record
 *	"R" - read, block (512)
 *	"W" - write, block
 */
RFILE	*ropen (char *name_, char *mode_)
{
	return (ropen2(name_, nullC, mode_));
}

/* <ropen>:
 * Open a file for record I/O, giving a default file specification.
 *
 *	name_	=> filename
 *	dft_	=> default specification
 *	mode_	=> Unix-style (record/block) mode (see: 'ropen')
 */
RFILE	*ropen2 (char *name_, char *dft_, char *mode_)
{
	RFILE	*z = calloc(1, sizeof(RFILE));
	unsigned status;
	int	newfile	= (_tolower(*mode_) == 'w');
	int	blocked	= (isupper(*mode_));
	int	no_old	= FALSE;
	int	recsize = DEFAULT_RSIZE;
	int	len;

	rmsinit_fab (&zFAB, &zNAM, dft_, name_);
	rmsinit_nam (&zNAM, z->rsa, z->esa);

	zFAB.fab$l_xab = (char *)&z->xabfhc;
	z->xabfhc = cc$rms_xabfhc;

	zRAB = cc$rms_rab;
	zRAB.rab$l_fab = &zFAB;

	/*
	 * Open the file.  We assume that if it is a new file, then no
	 * version number is given.  Exploit this to obtain the format
	 * of any previously-existing file of that name.
	 */
	CHECK(sys$parse(&zFAB));
	strncpy (rmsio$nam, z->esa, len = zNAM.nam$b_esl);
	rmsio$nam[len] = EOS;

	if (newfile)
	{
		zFAB.fab$b_fac = FAB$M_GET | BLOCKED;
		sys(sys$search(&zFAB))
			no_old = TRUE;
		else
		{
			sys(sys$open(&zFAB))
				no_old	= TRUE;
			else
				sys$close(&zFAB);
		}
		if (no_old)
		{
			zFAB.fab$b_rfm = FAB$C_VAR;
			zFAB.fab$b_rat |= FAB$M_CR;
		}
		zFAB.fab$b_fac = FAB$M_PUT | BLOCKED;
		zFAB.fab$b_shr |= FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_SHRUPD ;
		CHECK(sys$create(&zFAB));
	}
	else
	{
		zFAB.fab$b_fac |= FAB$M_GET | BLOCKED;
		/* 1995/10/27 - I'd noticed that 'most' was able to read files
		 * that were locked, and investigated.  The shrget/shrupd flags
		 * are necessary for the cases that I looked at, and 'most'
		 * also uses the shrput flag, which doesn't seem to hurt.
		 */
		zFAB.fab$b_shr |= FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_SHRUPD ;
		CHECK(sys$search(&zFAB));
		CHECK(sys$open(&zFAB));
	}

	/*
	 * Allocate a buffer big enough for the biggest record:
	 */
	if (!newfile)	recsize = rsize(z);
	zRAB.rab$l_ubf = calloc (1, zRAB.rab$w_usz = recsize);

	/*
	 * Connect an I/O stream to the file:
	 */
	CHECK(sys$connect(&zRAB));
	return (z);

failed:	rclose (z);
	return (0);
}

/* <rgetr>:
 * Read a record, returning the number of bytes read, and returning to the
 * caller the file-address of the record.
 */
int	rgetr (
	RFILE *	z,		/* file-descriptor pointer	*/
	char *	bfr,		/* buffer to load		*/
	int	maxbfr,		/* ...its size			*/
	unsigned *mark_)	/* file-address of buffer	*/
{
	unsigned status;
	int	len	= -1;

	zRAB.rab$b_rac = RAB$C_SEQ;
	if (isBLOCK)			/* return zero if error or EOF	*/
	{
		CHECK(sys$read(&zRAB));
	}
	else
	{
		CHECK(sys$get(&zRAB));
	}

	maxbfr--;			/* Index of last permissible	*/
	len = zRAB.rab$w_rsz;		/* limit length of returned buffer */
	if (len > maxbfr) len = maxbfr;	/* ...to caller's 'maxbfr' size	*/
	memcpy (bfr, zRAB.rab$l_rbf, len); /* ...and copy buffer	*/

failed:
	*mark_ = rtell(z);		/* update file-mark		*/
	return (len);
}

/* <rputr>:
 * Write a record, given the buffer address and number of characters to
 * write.
 */
int	rputr (
	RFILE	*z,		/* file-descriptor pointer	*/
	char	*bfr,		/* buffer to load		*/
	int	maxbfr)		/* ...its size			*/
{
	unsigned status;

	zRAB.rab$b_rac = RAB$C_SEQ;

	zRAB.rab$w_rsz = maxbfr;	/* set length of buffer		*/
	zRAB.rab$l_rbf = bfr;		/* ...and copy buffer-pointer	*/
	if (isBLOCK)			/* return nonzero if error	*/
	{
		CHECK2(sys$write(&zRAB));
	}
	else
	{
		CHECK2(sys$put(&zRAB));
	}
	status = 0;
failed:
	if (status)	rerror();
	return (status);
}

/* <rtell>:
 * Return the record-file-address translated into an offset value,
 * compatible with the 'ftell' usage.
 */
unsigned rtell (RFILE *z)
{
	return (rabrfa_get (&zRAB));
}

/* <rseek>:
 * Search for a specific record, by offset.  If this was the offset obtained
 * from 'rtell', the seek should properly lock on a record.
 *
 * Patch: The 'direction' argument is not implemented.  We assume that it is
 *	0, for absolute positioning.
 */
int	rseek (RFILE *z, int offset, int direction)
{
	unsigned status;

	zRAB.rab$b_rac = RAB$C_RFA;
	rabrfa_put (&zRAB, offset);
	ZERO(sys$find(&zRAB));	/* return zero if ok */
	return (0);
}

/* <rclose>:
 * Close the file, returning nonzero status iff an error occurs.
 */
int	rclose (RFILE *z)
{
	unsigned status;
	char	*ubf_	= zRAB.rab$l_ubf;

	CHECK2(sys$disconnect(&zRAB));
	CHECK2(sys$close(&zFAB));
	status = 0;
failed:
	cfree (z);
	if (ubf_)	cfree (ubf_);
	return (status);
}

/* <erstat>:
 * If the last record operation (put/get) failed, show its status-message.
 * If the given RFILE-pointer is null, show any error condition (so we can
 * use this on a failed-open, or in place of 'rerror').
 */
int	erstat (RFILE *z, char *msg, int msglen)
{
	unsigned status	= z ? zRAB.rab$l_sts : rmsio$err;

	if (($VMS_STATUS_SEVERITY(status) == STS$K_SEVERE)
	||  (!z && !$VMS_STATUS_SUCCESS(status)) )
	{
		sysgetmsg (status, msg, msglen);
		return (TRUE);
	}
	else
		return (FALSE);
}

/* <rerror>:
 * Display the cause of the last error condition, like 'perror'
 */
void	rerror (void)
{
#define	MAX_MSG	132
	auto    char	msg[MAX_MSG+NAM$C_MAXRSS];
	register int	len;

	sysgetmsg (rmsio$err, msg, sizeof(msg));
	if (rmsio$err == RMS$_FNF) {
		len = strlen(msg);
		sprintf (msg+len, ":: %.*s", sizeof(msg) - len - 2, rmsio$nam);
	}
	if ((len = strlen(msg)) < (sizeof(msg) - 16))
		sprintf(msg+strlen(msg), " (st=%#x)", rmsio$err);
	warn ("%.*s", MAX_MSG, msg);
	rmsio$err = 0;
}

/* <rclear>:
 * Reset the error-latch.  Note that because only one latch is saved for
 * all files together, we must be careful to test the latch when we need it,
 * and to reset it otherwise.
 */
void	rclear (void)
{
	rmsio$err = 0;
}

/* <rsize>:
 * Return the length of the longest record in (an input) file:
 */
int	rsize (RFILE *z)
{
	struct	XABFHC	*xab_;
	int	size;

	if (isBLOCK)
		return (512);

	if ((xab_ = (struct XABFHC *)(zFAB.fab$l_xab)) != 0)
	{
		while (xab_)
		{
			if (xab_->xab$b_cod == XAB$C_FHC)
			{
				if (size = xab_->xab$w_lrl)
					return (size);
			}
			xab_ = (struct XABFHC *)(xab_->xab$l_nxt);
		}
	}
	return (DEFAULT_RSIZE);
}

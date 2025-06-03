/*
 *			l z v i o . c
 *
 */

/*
 * Problems:
 *	If you open a second input file (getting rms attributes)
 *	it aborts with an internal "fatal" error (15820C LIB-F-FATERRLIB)
 */
 
/*
 * Make TESTING_FDLIO non-zero to enable test code.
 *
 * Edit History
 * 26-Dec-85	MM	Create files with "large" allocation quantity
 *			to improve processing speed.
 * 18-Apr-86	Hein	Don't set xrb_lrl if the file isn't sequential.
 *  2-Jun-87	MM	fdl$generate needs a dynamic string
 * 12-Sep-1993 	dp	Assorted fixes and cleanups for VMS, mainly:
 *			- Compilation with DEC C (with /STANDARD=VAXC, sorry)
 *			- Changes in structure names in RMS .h files
 *
 */
#ifndef	TESTING_FDLIO
#define	TESTING_FDLIO	0
#endif

/*
 * RMS/FDL record level i/o routines for VMS
 * Rather crude.
 *
 * The following are provided:
 *
 *	#include descrip
 *
 *	FDLSTUFF *
 *	fdl_open(filename, fdl_descriptor)
 *	char			*filename;
 *	struct	dsc$descriptor	*fdl_descriptor;
 *		Initializes internal buffers and opens this existing
 *		file for input.  The filename may not contain wildcards.
 *		On (successful) return, fdl_descriptor will point to
 *		an initialized fdl specification.  The description
 *		string will be in malloc'ed memory.  The caller does not
 *		initialize the fdl_descriptor.  Returns NULL on error.
 *		(Note an error will be returned if the file is not
 *		block-oriented.)
 *
 *		When you don't need the fdl_descriptor information
 *		any more, free it by calling
 *		    fdl_free(fdl_descriptor);
 *		if fdl_descriptor is NULL on entry, the file is opened
 *		normally (fdl information is not collected).
 *
 *	FDLSTUFF *
 *	fdl_create(fdl_descriptor, override_filename, xab_lrl)
 *	struct	dsc$descriptor	*fdl_descriptor;
 *	char			*override_filename;
 *	int			xab_lrl;
 *		Creates a file using the fdl specification.
 *		If override_filename is not NULL and not equal to "",
 *		it will override the filename specified in the fdl.
 *		fdl_write() is used to write data to the file.
 *		Returns NULL on error.
 *
 *		if fdl_descriptor is NULL, the file is created using
 *		the name in override_filename (which must be present).
 *		The file is created in "undefined" record format.
 *
 *		xab_lrl initializes the XAB$W_LRL field.
 *
 *	fdl_free(fdl_descriptor)
 *	struct	dsc$descriptor	*fdl_descriptor;
 *		Releases the fdl descriptor block.
 *
 *	int
 *	fdl_read(buffer, buffer_length, r)
 *	char		*buffer;
 *	int		buffer_length;
 *	FDLSTUFF	*r;
 *		Read buffer_length bytes from the file (using SYS$READ).
 *		No expansion or interpretation.  buffer_length had
 *		better be even or you're asking for trouble.  Returns
 *		the actual number of bytes read.  The file has been
 *		opened by fdl_open.
 *
 *	int
 *	fdl_write(buffer, buffer_length, r)
 *	char		*buffer;
 *	int		buffer_length;
 *	FDLSTUFF	*r;
 *		Write buffer_length bytes to the file (using SYS$WRITE).
 *		No expansion or interpretation.  buffer_length had
 *		better be even or you're asking for trouble.  Returns
 *		the actual number of bytes written.  The file was opened
 *		by fdl_create();
 *
 *	fdl_getname(r, buffer)
 *	FDLSTUFF	*r;
 *	char		*buffer;
 *		Copies the currently open file's name to the caller's
 *		data buffer buffer.
 *
 *	long
 *	fdl_fsize(r)
 *		Returns the size in bytes of the opened file.
 *
 *	fdl_dump(fdl_descriptor, fd)
 *	struct	dsc$descriptor	*fdl_descriptor;
 *	FILE			*fd;
 *		Writes the fdl info to the indicated file with
 *		line breaks in appropriate places.
 *
 *	fdl_message(r, why)
 *	FDLSTUFF	*r;
 *	char		*why;
 *		All system-level routines set a global value, fdl_status.
 *		fdl_message() prints the error message text corresponding
 *		to the current value of fdl_status.  The message printed
 *		has the format:
 *			why current_filename: error_message.
 *		If why is NULL, only the error_message is printed.
 */

#include "lz.h"
#ifdef vms
#ifndef	FDL$M_FDL_SIGNAL
#define FDL$M_FDL_SIGNAL	1	/* Signal errors if set		*/
#endif
#ifndef	FDL$M_FDL_STRING
#define FDL$M_FDL_STRING	2	/* Use string for fdl text	*/
#endif
#if TESTING_FDLIO
#define	SIGNAL_ON_ERROR	FDL$M_FDL_SIGNAL
#else
#define	SIGNAL_ON_ERROR	0
#endif
#define MSG(what, code)	\
	fprintf(stderr, "Unexpected error at %s, code %X:\n\"%s\"\n", \
	what, code, vms_etext(code));

#define NXT(xptr)  ((struct XAB *)(xptr))->xab$l_nxt

int		fdl_status;		/* Set to last rms call status	*/

static char *
vms_etext(errorcode)
int		errorcode;
{
	char		*bp;
	short		errlen;		/* Actual text length		*/
	static char	errname[257];	/* Error text stored here	*/
	static $DESCRIPTOR(err, errname); /* descriptor for error text	*/


	lib$sys_getmsg(&errorcode, &errlen, &err, &15);
	/*
	 * Trim trailing junk.
	 */
	for (bp = &errname[errlen]; --bp >= errname;) {
	    if (isgraph(*bp) && *bp != ' ')
		break;
	}
	bp[1] = EOS;
	return(errname);
}

static FDLSTUFF *
fail(r, why, name)
FDLSTUFF	*r;			/* Buffer			*/
char		*why;			/* A little commentary		*/
char		*name;			/* Argument to perror		*/
/*
 * Problem exit routine
 */
{
	if (name == NULL
	 && r != NULL)
	    name = r->fab.fab$l_fna;
	if (name == NULL)
	    name = "<unknown file name>";
	if ($VMS_STATUS_SUCCESS(fdl_status)) {
	    fprintf(stderr, "%s: restriction -- %s\n", name, why);
	    fdl_status = SS$_ABORT;	/* Force an error status	*/
	}
	else {
	    message(r, why, name);
	}
	freefdlstuff(r);
	return (NULL);
}

FDLSTUFF *
fdl_open(filename,  fdl_descriptor)
char			*filename;		/* What to open		*/
struct	dsc$descriptor	*fdl_descriptor;	/* Result descriptor	*/
/*
 * Open the file.  Returns NULL on failure, else a pointer to RMS stuff.
 * Which is equivalently a pointer to the RAB. (Note that the RAB points
 * in turn to the FAB.)
 *
 * Return the file's fdl descriptor in the user-supplied (uninitialized)
 * descriptor.
 */
{
	register FDLSTUFF	*r;
	extern FDLSTUFF		*fdl_setup();

	if ((r = fdl_setup(filename)) == NULL)
	    return (NULL);
	/*
	 * Now open the file.
	 */
	r->fab.fab$b_fac = FAB$M_GET | FAB$M_BRO;
	if ((fdl_status = sys$open(&r->fab)) != RMS$_NORMAL) {
	    return (fail(r, "opening file", NULL));
	}
	if ((r->fab.fab$l_dev & DEV$M_REC) != 0) {
	    fail(r, "can't read from record only device", NULL);
	    fdl_close(r);
	    return (NULL);
	}
	if (r->fab.fab$b_org == FAB$C_IDX) {
	    /*
	     * (Much) more hacking for indexed-sequential
	     * to get all the key and allocation blocks.
	     */
	    int			i, nall, nkey;
	    char		**chain;	/* Links xab's together	*/
	    struct myxabkey {
		struct XABKEY	xabkey;		/* The xab		*/
		char		keyname[32];	/* Optional key name	*/
	    }			*key;
	    struct XABALL	*all;

	    nall = r->xabsum.xab$b_noa;	/* Allocation control blocks	*/
	    nkey = r->xabsum.xab$b_nok;	/* Isam keys			*/
	    fdl_close(r);		/* Close it to try again	*/
	    if ((r = fdl_setup(filename)) == NULL)
		return (NULL);
	    /* chain = &r->xabsum.xab$l_nxt; */
	    chain = &NXT(&r->xabsum);	/* Start xab block list	*/
	    for (i = 0; i < nkey; i++) {
		key = malloc(sizeof (struct myxabkey));
		key->xabkey = cc$rms_xabkey;
		key->xabkey.xab$b_ref = i;	/* Set key ref number	*/
	 	key->xabkey.xab$l_knm = &key->keyname[0];
		*chain = key;
		/* chain = &key->xabkey.xab$l_nxt; */
		chain = &NXT(&key->xabkey);
	    }
	    for (i = 0; i < nall; i++) {
		all = malloc(sizeof (struct XABALL));
		*all = cc$rms_xaball;
		all->xab$b_aid = i;		/* Allo area ref number	*/
		*chain = all;
		/* chain = &all->xab$l_nxt; */
		chain = &NXT(all);
	    }
	    r->fab.fab$b_fac = FAB$M_GET | FAB$M_BRO;
	    if ((fdl_status = sys$open(&r->fab)) != RMS$_NORMAL) {
		MSG("fab status", r->fab.fab$l_sts);
		MSG("fab stv   ", r->fab.fab$l_stv);
		return (fail(r, "reopening ISAM file", NULL));
	    }
	}
	r->rab.rab$l_rop = RAB$M_BIO;		/* Block I/O only	*/
	if ((fdl_status = sys$connect(&r->rab)) != RMS$_NORMAL)
	    return (fail(r, "connecting after open", NULL));
	if (fdl_descriptor != NULL) {
	    struct FAB		*fab_add;
	    struct RAB		*rab_add;
	    static int		flags = (FDL$M_FDL_STRING | SIGNAL_ON_ERROR);
	    auto int		badblk;
	    auto int		retlen;
	    /*
	     * Now, get the file attributes
	     */
	    fdl_descriptor->dsc$w_length = 0;
	    fdl_descriptor->dsc$b_dtype = DSC$K_DTYPE_T;
	    fdl_descriptor->dsc$b_class = DSC$K_CLASS_D;
	    fdl_descriptor->dsc$a_pointer = NULL;
	    fab_add = &r->fab;
	    rab_add = &r->rab;
	    if ((fdl_status = fdl$generate(
		    &flags,
		    &fab_add,
		    &rab_add,
		    0L,			/* file_dst (unused)		*/
		    0L,			/* file_resnam (unused)		*/
		    fdl_descriptor,
		    &badblk,
		    &retlen)) != SS$_NORMAL) {
		fdl_free(fdl_descriptor);
		sys$close(&r->fab);
		return(fail(r, "getting fdl info", NULL));
	    }
	}
	return (r);
}

FDLSTUFF *
fdl_create(fdl_descriptor, override_filename, xab_lrl)
struct	dsc$descriptor	*fdl_descriptor;	/* Result descriptor	*/
char			*override_filename;	/* What to open		*/
int			xab_lrl;		/* Longest record len	*/
/*
 * Create the file, Returns NULL on failure, else a pointer to RMS stuff.
 * Which is equivalently a pointer to the RAB. (Note that the RAB points
 * in turn to the FAB.)  The file is open for writing using fdl_write.
 *
 * Uses the filename in the descriptor block, or the override filename
 * if supplied (non-NULL and not == "");
 *
 * If fdl_descriptor is NULL, the override_filename is opened normally.
 *
 * If xab_lrl is non-zero, it sets the file-header "longest record"
 * field.
 */
{
	register FDLSTUFF	*r;
	int			retlen;
	int			badblk;
	static int		flags = (FDL$M_FDL_STRING | SIGNAL_ON_ERROR);
	struct	dsc$descriptor	newname;
	struct	dsc$descriptor	*newname_ptr;
	int			fid_block[3];
	auto int		rms_status, rms_stv;
	char			created_name[NAM$C_MAXRSS + 1];
	struct	dsc$descriptor	created_name_des = {
				    NAM$C_MAXRSS,
				    DSC$K_DTYPE_T, 
				    DSC$K_CLASS_S,
				    &created_name[0]
				};
	extern FDLSTUFF		*fdl_setup();

	if (fdl_descriptor == NULL) {
	    /*
	     * Normal file open.
	     */
	    if ((r = fdl_setup(override_filename)) == NULL)
		return (NULL);
	    r->fab.fab$l_alq = 256;		/* Allocation quantity	*/
	    r->fab.fab$b_fac = FAB$M_PUT | FAB$M_BIO; /* Block I/O only	*/
	    r->fab.fab$l_fop |=
		(FAB$M_TEF | FAB$M_NAM | FAB$M_SQO | FAB$M_BIO);
	    r->fab.fab$b_org = FAB$C_SEQ;	/* Sequential only	*/
	    r->fab.fab$b_rfm = FAB$C_UDF;	/* Undefined format	*/
	    r->xabfhc.xab$w_lrl = xab_lrl;	/* Set record length	*/
	    fdl_status = sys$create(&r->fab);
	    if (!$VMS_STATUS_SUCCESS(fdl_status))
		return (fail(r, "creating file", NULL));
	    goto exit;				/* Normal exit		*/
	}
	if (override_filename == NULL || override_filename[0] == '\0')
	    newname_ptr = NULL;
	else {
	    newname_ptr = &newname;
	    newname.dsc$w_length = strlen(override_filename);
	    newname.dsc$b_dtype = DSC$K_DTYPE_T;
	    newname.dsc$b_class = DSC$K_CLASS_S;
	    newname.dsc$a_pointer = override_filename;
	}
	
	fdl_status = fdl$create(fdl_descriptor,
		newname_ptr,		/* New file name if any		*/
		0,			/* Default filename		*/
		&created_name_des,	/* Resultant filename		*/
		&fid_block[0],		/* File ID block		*/
		&flags,			/* FDL flag bits		*/
		0,			/* Statement number		*/
		&retlen,		/* Created name length		*/
		&rms_status,		/* FAB$L_STS code		*/
		&rms_stv		/* FAB$L_STV			*/
	);
	if (!$VMS_STATUS_SUCCESS(fdl_status)) {
	    MSG("FAB$L_STS", rms_status);
	    MSG("FAB$L_STV", rms_stv);
	    return(fail(NULL, "creating file", NULL));
	}
	created_name[retlen] = '\0';
	if ((r = fdl_setup(created_name)) == NULL)
	    return (NULL);
	/*
	 * Now, open the file for output.
	 */
	r->fab.fab$b_fac =
	    (xab_lrl == 0) ? FAB$M_PUT | FAB$M_BIO
			   : FAB$M_PUT | FAB$M_BRO;
	if ((fdl_status = sys$open(&r->fab)) != RMS$_NORMAL) {
	    return (fail(r, "opening created file", NULL));
	}
exit:	if ((r->fab.fab$l_dev & DEV$M_REC) != 0) {
	    fail(r, "Can't write record only device", NULL);
	    fdl_close(r);
	    return (NULL);
	}
	/*
	 * We do a bit of a hack to force "longest record length"
	 */
	if (xab_lrl != 0
	 && r->fab.fab$b_org == FAB$C_SEQ) {	/* Seq. files only	*/
	    char		*tempbuf;
	    extern char		outbuffer[MAXIO];

	    if (xab_lrl <= MAXIO)
		tempbuf = &outbuffer[0];
	    else if ((tempbuf = malloc(xab_lrl)) == NULL)
		return (fail(r, "allocating memory for longest record", NULL));
	    r->rab.rab$l_rop = 0;		/* PUT I/O 		*/
	    if ((fdl_status = sys$connect(&r->rab)) != RMS$_NORMAL)
		return (fail(r, "connect after create to set length", NULL));
	    r->rab.rab$l_rbf = tempbuf;
	    r->rab.rab$w_rsz = xab_lrl;
	    if ((fdl_status = sys$put(&r->rab)) != RMS$_NORMAL) {
		if (tempbuf != &outbuffer[0])
		    free(tempbuf);
		return (fail(r, "putting longest record", NULL));
	    }
	    if (tempbuf != &outbuffer[0])
		free(tempbuf);
	    if ((fdl_status = sys$disconnect(&r->rab)) != RMS$_NORMAL)
		return (fail(r, "disconnecting after setting length", NULL));
	}
	r->rab.rab$l_rop = RAB$M_BIO;	/* Block I/O only	*/
	if ((fdl_status = sys$connect(&r->rab)) != RMS$_NORMAL)
	    return (fail(r, "connecting after create", NULL));
	return (r);
}

static FDLSTUFF *
fdl_setup(filename)
char		*filename;
/*
 * Initializes rms blocks and parses file name.  Returns the
 * FDL data block on success, NULL on error.
 */
{
	register FDLSTUFF	*r;

	if ((r = (char *)malloc(sizeof (FDLSTUFF))) == NULL)
	    return (NULL);
	r->fab = cc$rms_fab;			/* Preset fab,		*/
	r->nam = cc$rms_nam;			/*   name block		*/
	r->rab = cc$rms_rab;			/*   and record block	*/
	r->xabfhc = cc$rms_xabfhc;		/*   file header block	*/
	r->xabsum = cc$rms_xabsum;		/*   isam summary block	*/
	r->fab.fab$l_nam = &r->nam;		/* fab -> name block	*/
	r->fab.fab$l_xab = &r->xabfhc;		/* fab -> file header	*/
	r->fab.fab$l_fna = filename;		/* Argument filename	*/
	r->fab.fab$b_fns = strlen(filename);	/* ... size		*/
	r->rab.rab$l_fab = &r->fab;		/* rab -> fab		*/
						/* Stuff the name block	*/
	r->nam.nam$l_esa = r->starname;		/* Expanded filename	*/
	r->nam.nam$b_ess = NAM$C_MAXRSS;	/* ... size		*/
	r->nam.nam$b_rss = NAM$C_MAXRSS;	/* ... max size		*/
	/* r->xabfhc.xab$l_nxt = &r->xabsum; */
	NXT(&r->xabfhc) = &r->xabsum;		/* And build xab chain	*/
	if ((fdl_status = sys$parse(&r->fab)) != RMS$_NORMAL) {
	    return (fail(r, "parsing", filename));
	}
	((char *)r->nam.nam$l_esa)[r->nam.nam$b_esl] = EOS;
	r->fab.fab$l_fna = r->nam.nam$l_esa;	/* File name		*/
	r->fab.fab$b_fns = r->nam.nam$b_esl;	/* Length		*/
	r->fab.fab$l_fop |= FAB$M_NAM;		/* Use name block	*/
	return (r);
}

fdl_free(fdl_descriptor)
struct	dsc$descriptor	*fdl_descriptor;
/*
 * Release the descriptor
 */
{
	register int status;

	if ((status = str$free1_dx(fdl_descriptor)) != SS$_NORMAL) {
	    fprintf(stderr, "LZ fatal internal error \"%s\"\n", status);
	    lib$signal(status);
	}
}

fdl_close(r)
register FDLSTUFF	*r;
{
	if ((fdl_status = sys$close(&r->fab)) != RMS$_NORMAL)
	    return(fail(r, "closing", NULL));
	freefdlstuff(r);
}

static
freefdlstuff(r)
register FDLSTUFF	*r;
/*
 * Free fdl block and all XAB blocks.
 */
{
	register struct XAB	*xthis;
	register struct XAB	*xnext;

	if (r == NULL)
	    return;
	for (xnext = &NXT(&r->xabsum); xnext != NULL;) {
	    xthis = xnext;
	    xnext = xnext->xab$l_nxt;
	    free(xthis);
	}
	free(r);
}

int
fdl_read(buffer, buffer_length, r)
char		*buffer;		/* Record			*/
int		buffer_length;		/* Record length		*/
register FDLSTUFF *r;			/* Record info.			*/
/*
 * Read the next record from the file.  Returns number of bytes read or
 * -1 on any error. fdl_status has the status.
 */
{
	r->rab.rab$l_ubf = buffer;
	r->rab.rab$w_usz = buffer_length;
	r->rab.rab$l_bkt = 0;
	if ((fdl_status = sys$read(&r->rab)) != RMS$_NORMAL) {
#if TESTING_FDLIO
	    if (fdl_status != RMS$_EOF) {
		fdl_message(r, "error return from sys$read");
		sleep(1);
	    }
#endif
	    return (-1);
	}
	return (r->rab.rab$w_rsz);
}

int
fdl_write(buffer, buffer_length, r)
char		*buffer;		/* Record			*/
int		buffer_length;		/* Record length		*/
register FDLSTUFF *r;			/* Record info.			*/
/*
 * Write the next record to the file.  Returns number of bytes written or
 * -1 on any error. fdl_status has the status.
 */
{
	r->rab.rab$l_rbf = buffer;
	r->rab.rab$w_rsz = buffer_length;
	r->rab.rab$l_bkt = 0;
	if ((fdl_status = sys$write(&r->rab)) != RMS$_NORMAL) {
#if 1 || TESTING_FDLIO
	    fdl_message(r, "error return from sys$write");
	    fprintf(stderr, "writing %d bytes\n", buffer_length);
	    sleep(1);
#endif
	    return (-1);
	}
	return (r->rab.rab$w_rsz);
}

fdl_getname(r, buffer)
FDLSTUFF	*r;			/* File pointer			*/
char		*buffer;		/* Where to put it		*/
/*
 * Return current file name
 */
{
	strcpy(buffer, r->fab.fab$l_fna);
	return (buffer);
}

long
fdl_fsize(r)
FDLSTUFF	*r;			/* File pointer			*/
/*
 * Return current file size
 */
{
	return (((long) r->xabfhc.xab$l_ebk * 512) + r->xabfhc.xab$w_ffb);
}

fdl_message(r, why)
FDLSTUFF	*r;
char		*why;
/*
 * Print error message
 */
{
	extern char	*vms_etext();

	if (why == NULL) {
	    fprintf(stderr, "\n%s\n\n", vms_etext(fdl_status));
	}
	else {
	    fprintf(stderr, "\n%s%s%s:\n  %s\n\n",
		why,
		(why[0] == EOS) ? "" : " ",
		(r == NULL) ? "" : r->fab.fab$l_fna,
		vms_etext(fdl_status));
	}
}


static
message(r, why, name)
FDLSTUFF	*r;			/* Buffer			*/
char		*why;			/* A little commentary		*/
char		*name;			/* File name			*/
/*
 * Print error message
 */
{
	fprintf(stderr, "\nRMS error when %s %s\n",
	    why, (name == NULL) ? "" : name);
	fprintf(stderr, "\"%s\"\n", vms_etext(fdl_status));
}

fdl_dump(fdl_descriptor, fd)
struct	dsc$descriptor	*fdl_descriptor;
FILE			*fd;
/*
 * Dump the descriptor to fd.
 */
{
	register char	*tp, *end;

	tp = fdl_descriptor->dsc$a_pointer;
	end = tp + fdl_descriptor->dsc$w_length;
	while (tp < end) {
	    if (*tp == '"') {
		do {
		    putc(*tp++, fd);
		} while (*tp != '"');
	    }
	    putc(*tp, fd);
	    if (*tp++ == ';')
		putc('\n', fd);
	}
}


#if	TESTING_FDLIO
/*
 * Test program for rms io
 */
#include <stdio.h>

char			line[133];
char			filename[133];
char			buffer[2048];

main(argc, argv)
int		argc;
char		*argv[];
{
	FDLSTUFF	*old;
	FDLSTUFF	*new;
	int		size, total, nrecords;
	struct	dsc$descriptor	fdl_info;	/* Result descriptor	*/

	for (;;) {
	    fprintf(stderr, "Old file name: ");
	    fflush(stdout);
	    if (gets(line) == NULL)
		break;
	    if (line[0] == EOS)
		continue;
	    if ((old = fdl_open(line, &fdl_info)) == NULL) {
		fprintf(stderr, "open failed\n");
		continue;
	    }
	    fprintf(stderr, "New file name: ");
	    if (gets(line) == NULL)
		break;
	    if ((new = fdl_create(&fdl_info, line, 0)) == NULL) {
		fprintf(stderr, "create failed\n");
		fdl_free(&fdl_info);
		continue;
	    }
	    fdl_getname(old, buffer);
	    fprintf(stderr, "Fdl for \"%s\", size %ld\n",
		buffer, fdl_fsize(old));
	    fdl_dump(&fdl_info, stderr);
	    total = nrecords = 0;
	    while ((size = fdl_read(buffer, sizeof buffer, old)) > 0) {
		fdl_write(buffer, size, new);
		nrecords++;
		total += size;
	    }
	    fdl_close(old);
	    fdl_close(new);
	    fprintf(stderr, "copied %d records, %d bytes total\n",
		nrecords, total);
	    fdl_free(&fdl_info);
	}
}

#endif
#endif


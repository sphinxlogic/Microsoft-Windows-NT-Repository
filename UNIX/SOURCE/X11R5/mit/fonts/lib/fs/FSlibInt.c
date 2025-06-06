/* $XFree86: mit/fonts/lib/fs/FSlibInt.c,v 2.0 1993/11/07 14:04:06 dawes Exp $ */
/* $XConsortium: FSlibInt.c,v 1.9 91/07/22 11:29:28 rws Exp $ */

/* @(#)FSlibInt.c	3.3	91/05/02
 * Copyright 1990 Network Computing Devices;
 * Portions Copyright 1987 by Digital Equipment Corporation and the
 * Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this protoype software
 * and its documentation to Members and Affiliates of the MIT X Consortium
 * any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Network Computing Devices, Digital or
 * MIT not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.
 *
 * NETWORK COMPUTING DEVICES, DIGITAL AND MIT DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES, DIGITAL OR MIT BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 *	FSlibInt.c - Internal support routines for the C subroutine
 *	interface library (FSlib).
 */
#include <stdio.h>
#include "FSlibint.h"

static void _EatData32();

/* check for both EAGAIN and EWOULDBLOCK, because some supposedly POSIX
 * systems are broken and return EWOULDBLOCK when they should return EAGAIN
 */
#if defined(EAGAIN) && defined(EWOULDBLOCK)
#define ETEST(err) (err == EAGAIN || err == EWOULDBLOCK)
#else
#ifdef EAGAIN
#define ETEST(err) (err == EAGAIN)
#else
#define ETEST(err) (err == EWOULDBLOCK)
#endif
#endif

#ifdef CRAY
/*
 * Cray UniCOS does not have readv and writev so we emulate
 */
#include <sys/socket.h>

int
_FSReadV(fd, iov, iovcnt)
    int         fd;
    struct iovec *iov;
    int         iovcnt;
{
    struct msghdr hdr;

    hdr.msg_iov = iov;
    hdr.msg_iovlen = iovcnt;
    hdr.msg_accrights = 0;
    hdr.msg_accrightslen = 0;
    hdr.msg_name = 0;
    hdr.msg_namelen = 0;

    return (recvmsg(fd, &hdr, 0));
}

int
_FSWriteV(fd, iov, iovcnt)
    int         fd;
    struct iovec *iov;
    int         iovcnt;
{
    struct msghdr hdr;

    hdr.msg_iov = iov;
    hdr.msg_iovlen = iovcnt;
    hdr.msg_accrights = 0;
    hdr.msg_accrightslen = 0;
    hdr.msg_name = 0;
    hdr.msg_namelen = 0;

    return (sendmsg(fd, &hdr, 0));
}

#endif				/* CRAY */

#if defined(SYSV) && defined(SYSV386) && !defined(STREAMSCONN)
/*
 * SYSV/386 does not have readv so we emulate
 */
#include <sys/uio.h>

int _FSReadV(fd, iov, iovcnt)
int fd;
struct iovec *iov;
int iovcnt;
{
    int i, len, total;
    char *base;

    errno = 0;
    for (i=0, total=0;  i<iovcnt;  i++, iov++) {
	len = iov->iov_len;
	base = iov->iov_base;
	while (len > 0) {
	    register int nbytes;
	    nbytes = read(fd, base, len);
	    if (nbytes < 0 && total == 0)  return -1;
	    if (nbytes <= 0)  return total;
	    errno = 0;
	    len   -= nbytes;
	    total += nbytes;
	    base  += nbytes;
	}
    }
    return total;
}

#ifdef SCO

int _FSWriteV(fd, iov, iovcnt)
	int				 fd;
	struct iovec	*iov;
	int				 iovcnt;
{
    int i, len, total;
    char *base;

    errno = 0;
    for (i = 0, total = 0; i < iovcnt; i++, iov++) {
		len = iov->iov_len;
		base = iov->iov_base;
		while (len > 0) {
			register int nbytes;

			nbytes = write(fd, base, len);
			if (nbytes < 0 && total == 0)
				return(-1);
			if (nbytes <= 0)
				return(total);
			errno = 0;
			len   -= nbytes;
			total += nbytes;
			base  += nbytes;
		}
    }
    return(total);
}

#endif /* SCO */

#endif /* SYSV && SYSV386 && !STREAMSCONN */

/*
 * The following routines are internal routines used by FSlib for protocol
 * packet transmission and reception.
 *
 * FSIOError(FSServer *) will be called if any sort of system call error occurs.
 * This is assumed to be a fatal condition, i.e., FSIOError should not return.
 *
 * FSError(FSServer *, FSErrorEvent *) will be called whenever an FS_Error event is
 * received.  This is not assumed to be a fatal condition, i.e., it is
 * acceptable for this procedure to return.  However, FSError should NOT
 * perform any operations (directly or indirectly) on the DISPLAY.
 *
 * Routines declared with a return type of 'Status' return 0 on failure,
 * and non 0 on success.  Routines with no declared return type don't
 * return anything.  Whenever possible routines that create objects return
 * the object they have created.
 */

_FSQEvent  *_FSqfree = NULL;	/* NULL _FSQEvent. */

static int  padlength[4] = {0, 3, 2, 1};

 /*
  * lookup table for adding padding bytes to data that is read from or written
  * to the FS socket.
  */

static fsReq _dummy_request = {
    0, 0, 0
};

/*
 * _FSFlush - Flush the FS request buffer.  If the buffer is empty, no
 * action is taken.  This routine correctly handles incremental writes.
 * This routine may have to be reworked if int < long.
 */
_FSFlush(svr)
    register FSServer *svr;
{
#ifndef AMOEBA
    register long size,
                todo;
    register int write_stat;
    register char *bufindex;

    size = todo = svr->bufptr - svr->buffer;
    bufindex = svr->bufptr = svr->buffer;
    /*
     * While write has not written the entire buffer, keep looping until the
     * entire buffer is written.  bufindex will be incremented and size
     * decremented as buffer is written out.
     */
    while (size) {
	errno = 0;
	write_stat = WriteToServer(svr->fd, bufindex, (int) todo);
	if (write_stat >= 0) {
	    size -= write_stat;
	    todo = size;
	    bufindex += write_stat;
	} else if (ETEST(errno)) {
	    _FSWaitForWritable(svr);
#ifdef SUNSYSV
	} else if (errno == 0) {
	    _FSWaitForWritable(svr);
#endif

#ifdef EMSGSIZE
	} else if (errno == EMSGSIZE) {
	    if (todo > 1)
		todo >>= 1;
	    else
		_FSWaitForWritable(svr);
#endif
	} else {
	    /* Write failed! */
	    /* errno set by write system call. */
	    (*_FSIOErrorFunction) (svr);
	}
    }
    svr->last_req = (char *) &_dummy_request;
#endif
}

int
_FSEventsQueued(svr, mode)
    register FSServer *svr;
    int         mode;
{
#if !defined(AMOEBA) && !defined(_MINIX)
    register int len;
    int         pend;
    char        buf[BUFSIZE];
    register fsReply *rep;

    if (mode == QueuedAfterFlush) {
	_FSFlush(svr);
	if (svr->qlen)
	    return (svr->qlen);
    }
    if (BytesReadable(svr->fd, (char *) &pend) < 0)
	(*_FSIOErrorFunction) (svr);
    if ((len = pend) < SIZEOF(fsReply))
	return (svr->qlen);	/* _FSFlush can enqueue events */
    else if (len > BUFSIZE)
	len = BUFSIZE;
    len /= SIZEOF(fsReply);
    pend = len * SIZEOF(fsReply);
    _FSRead(svr, buf, (long) pend);

    /* no space between comma and type or else macro will die */
    STARTITERATE(rep, fsReply, buf, (len > 0), len--) {
	if (rep->generic.type == FS_Error)
	    _FSError(svr, (fsError *) rep);
	else			/* must be an event packet */
	    _FSEnq(svr, (fsEvent *) rep);
    }
    ENDITERATE
	return (svr->qlen);
#endif
}

/* _FSReadEvents - Flush the output queue,
 * then read as many events as possible (but at least 1) and enqueue them
 */
_FSReadEvents(svr)
    register FSServer *svr;
{
#if !defined(AMOEBA) && !defined(_MINIX)
    char        buf[BUFSIZE];
    long        pend_not_register;	/* because can't "&" a register
					 * variable */
    register long pend;
    register fsEvent *ev;
    Bool        not_yet_flushed = True;

    do {
	/* find out how much data can be read */
	if (BytesReadable(svr->fd, (char *) &pend_not_register) < 0)
	    (*_FSIOErrorFunction) (svr);
	pend = pend_not_register;

	/*
	 * must read at least one fsEvent; if none is pending, then we'll just
	 * flush and block waiting for it
	 */
	if (pend < SIZEOF(fsEvent)) {
	    pend = SIZEOF(fsEvent);
	    /* don't flush until we block the first time */
	    if (not_yet_flushed) {
		int         qlen = svr->qlen;

		_FSFlush(svr);
		if (qlen != svr->qlen)
		    return;
		not_yet_flushed = False;
	    }
	}
	/* but we won't read more than the max buffer size */
	if (pend > BUFSIZE)
	    pend = BUFSIZE;

	/* round down to an integral number of XReps */
	pend = (pend / SIZEOF(fsEvent)) * SIZEOF(fsEvent);

	_FSRead(svr, buf, pend);

	/* no space between comma and type or else macro will die */
	STARTITERATE(ev, fsEvent, buf, (pend > 0),
		     pend -= SIZEOF(fsEvent)) {
	    if (ev->type == FS_Error)
		_FSError(svr, (fsError *) ev);
	    else		/* it's an event packet; enqueue it */
		_FSEnq(svr, ev);
	}
	ENDITERATE
    } while (svr->head == NULL);
#endif
}

/*
 * _FSRead - Read bytes from the socket taking into account incomplete
 * reads.  This routine may have to be reworked if int < long.
 */
_FSRead(svr, data, size)
    register FSServer *svr;
    register char *data;
    register long size;
{
#ifndef AMOEBA
    register long bytes_read;

    if (size == 0)
	return;
    errno = 0;
    while ((bytes_read = ReadFromServer(svr->fd, data, (int) size))
	    != size) {

	if (bytes_read > 0) {
	    size -= bytes_read;
	    data += bytes_read;
	}
	else if (ETEST(errno)) {
	    _FSWaitForReadable(svr);
	    errno = 0;
	}
#ifdef SUNSYSV
	else if (errno == 0) {
	    _FSWaitForReadable(svr);
	}
#endif

	else if (bytes_read == 0) {
	    /* Read failed because of end of file! */
	    errno = EPIPE;
	    (*_FSIOErrorFunction) (svr);
	} else {		/* bytes_read is less than 0; presumably -1 */
	    /* If it's a system call interrupt, it's not an error. */
	    if (errno != EINTR)
		(*_FSIOErrorFunction) (svr);
	}
    }
#endif
}

#ifdef WORD64
/*
 * XXX This is a *really* stupid way of doing this....
 */

#define PACKBUFFERSIZE 4096


/*
 * _FSRead32 - Read bytes from the socket unpacking each 32 bits
 *            into a long (64 bits on a CRAY computer).
 *
 */
static
_doFSRead32(svr, data, size, packbuffer)
    register FSServer *svr;
    register long *data;
    register long size;
    register char *packbuffer;
{
    long       *lpack,
               *lp;
    long        mask32 = 0x00000000ffffffff;
    long        maskw,
                nwords,
                i,
                bits;

    _FSReadPad(svr, packbuffer, size);

    lp = data;
    lpack = (long *) packbuffer;
    nwords = size >> 2;
    bits = 32;

    for (i = 0; i < nwords; i++) {
	maskw = mask32 << bits;
	*lp++ = (*lpack & maskw) >> bits;
	bits = bits ^ 32;
	if (bits) {
	    lpack++;
	}
    }
}

_FSRead32(svr, data, len)
    FSServer   *svr;
    long       *data;
    long        len;
{
    char        packbuffer[PACKBUFFERSIZE];
    unsigned    nwords = (PACKBUFFERSIZE >> 2);	/* bytes to CARD32 */

    for (; len > nwords; len -= nwords, data += nwords) {
	_doFSRead32(svr, data, nwords, packbuffer);
    }
    _doFSRead32(svr, data, len, packbuffer);
}



/*
 * _FSRead16 - Read bytes from the socket unpacking each 16 bits
 *            into a long (64 bits on a CRAY computer).
 *
 */
static
_doFSRead16(svr, data, size, packbuffer)
    register FSServer *svr;
    register short *data;
    register long size;
    char       *packbuffer;
{
    long       *lpack,
               *lp;
    long        mask16 = 0x000000000000ffff;
    long        maskw,
                nwords,
                i,
                bits;

    _FSRead(svr, packbuffer, size);	/* don't do a padded read... */

    lp = (long *) data;
    lpack = (long *) packbuffer;
    nwords = size >> 1;		/* number of 16 bit words to be unpacked */
    bits = 48;
    for (i = 0; i < nwords; i++) {
	maskw = mask16 << bits;
	*lp++ = (*lpack & maskw) >> bits;
	bits -= 16;
	if (bits < 0) {
	    lpack++;
	    bits = 48;
	}
    }
}

_FSRead16(svr, data, len)
    FSServer   *svr;
    short      *data;
    long        len;
{
    char        packbuffer[PACKBUFFERSIZE];
    unsigned    nwords = (PACKBUFFERSIZE >> 1);	/* bytes to CARD16 */

    for (; len > nwords; len -= nwords, data += nwords) {
	_doFSRead16(svr, data, nwords, packbuffer);
    }
    _doFSRead16(svr, data, len, packbuffer);
}

_FSRead16Pad(svr, data, size)
    FSServer   *svr;
    short      *data;
    long        size;
{
    int         slop = (size & 3);
    short       slopbuf[3];

    _FSRead16(svr, data, size);
    if (slop > 0) {
	_FSRead16(svr, slopbuf, 4 - slop);
    }
}

#endif				/* WORD64 */


/*
 * _FSReadPad - Read bytes from the socket taking into account incomplete
 * reads.  If the number of bytes is not 0 mod 32, read additional pad
 * bytes. This routine may have to be reworked if int < long.
 */
_FSReadPad(svr, data, size)
    register FSServer *svr;
    register char *data;
    register long size;
{
#if !defined(AMOEBA) && !defined(_MINIX)
    register long bytes_read;
    struct iovec iov[2];
    char        pad[3];

    if (size == 0)
	return;
    iov[0].iov_len = (int) size;
    iov[0].iov_base = data;
    /*
     * The following hack is used to provide 32 bit long-word aligned padding.
     * The [1] vector is of length 0, 1, 2, or 3, whatever is needed.
     */

    iov[1].iov_len = padlength[size & 3];
    iov[1].iov_base = pad;
    size += iov[1].iov_len;

    errno = 0;
    while ((bytes_read = ReadvFromServer(svr->fd, iov, 2)) != size) {

	if (bytes_read > 0) {
	    size -= bytes_read;
	    if ((iov[0].iov_len -= bytes_read) < 0) {
		iov[1].iov_len += iov[0].iov_len;
		iov[1].iov_base -= iov[0].iov_len;
		iov[0].iov_len = 0;
	    } else
		iov[0].iov_base += bytes_read;
	}
	else if (ETEST(errno)) {
	    _FSWaitForReadable(svr);
	    errno = 0;
	}
#ifdef SUNSYSV
	else if (errno == 0) {
	    _FSWaitForReadable(svr);
	}
#endif

	else if (bytes_read == 0) {
	    /* Read failed because of end of file! */
	    errno = EPIPE;
	    (*_FSIOErrorFunction) (svr);
	} else {		/* bytes_read is less than 0; presumably -1 */
	    /* If it's a system call interrupt, it's not an error. */
	    if (errno != EINTR)
		(*_FSIOErrorFunction) (svr);
	}
    }
#endif
}

/*
 * _FSSend - Flush the buffer and send the client data. 32 bit word aligned
 * transmission is used, if size is not 0 mod 4, extra bytes are transmitted.
 * This routine may have to be reworked if int < long;
 */
_FSSend(svr, data, size)
    register FSServer *svr;
    char       *data;
    register long size;
{
#if !defined(AMOEBA) && !defined(_MINIX)
    struct iovec iov[3];
    static char pad[3] = {0, 0, 0};

    long        skip = 0;
    long        svrbufsize = (svr->bufptr - svr->buffer);
    long        padsize = padlength[size & 3];
    long        total = svrbufsize + size + padsize;
    long        todo = total;

    /*
     * There are 3 pieces that may need to be written out:
     * 
     * o  whatever is in the display buffer o  the data passed in by the user o
     * any padding needed to 32bit align the whole mess
     * 
     * This loop looks at all 3 pieces each time through.  It uses skip to figure
     * out whether or not a given piece is needed.
     */
    while (total) {
	long        before = skip;	/* amount of whole thing written */
	long        remain = todo;	/* amount to try this time, <= total */
	int         i = 0;
	long        len;

	/*
	 * You could be very general here and have "in" and "out" iovecs and
	 * write a loop without using a macro, but what the heck.  This
	 * translates to:
	 * 
	 * how much of this piece is new? if more new then we are trying this
	 * time, clamp if nothing new then bump down amount already written,
	 * for next piece else put new stuff in iovec, will need all of next
	 * piece
	 * 
	 * Note that todo had better be at least 1 or else we'll end up writing 0
	 * iovecs.
	 */
#define InsertIOV(pointer, length) \
	    len = (length) - before; \
	    if (len > remain) \
		len = remain; \
	    if (len <= 0) { \
		before = (-len); \
	    } else { \
		iov[i].iov_len = len; \
		iov[i].iov_base = (pointer) + before; \
		i++; \
		remain -= len; \
		before = 0; \
	    }

	InsertIOV(svr->buffer, svrbufsize)
	InsertIOV(data, size)
	InsertIOV(pad, padsize)

	errno = 0;
	if ((len = WritevToServer(svr->fd, iov, i)) >= 0) {
	    skip += len;
	    total -= len;
	    todo = total;
	} else if (ETEST(errno)) {
		_FSWaitForWritable(svr);
#ifdef SUNSYSV
	} else if (errno == 0) {
	    _FSWaitForWritable(svr);
#endif

#ifdef EMSGSIZE
	} else if (errno == EMSGSIZE) {
	    if (todo > 1)
		todo >>= 1;
	    else
		_FSWaitForWritable(svr);
#endif
	} else {
	    (*_FSIOErrorFunction) (svr);
	}
    }

    svr->bufptr = svr->buffer;
    svr->last_req = (char *) &_dummy_request;
#endif
}

#ifdef undef
/*
 * _FSAllocID - normal resource ID allocation routine.  A client
 * can roll his own and instatantiate it if he wants, but must
 * follow the rules.
 */
FSID
_FSAllocID(svr)
    register FSServer *svr;
{
    return (svr->resource_base + (svr->resource_id++ << svr->resource_shift));
}

#endif

/*
 * The hard part about this is that we only get 16 bits from a reply.  Well,
 * then, we have three values that will march along, with the following
 * invariant:
 *	svr->last_request_read <= rep->sequenceNumber <= svr->request
 * The right choice for rep->sequenceNumber is the largest that
 * still meets these constraints.
 */

unsigned long
_FSSetLastRequestRead(svr, rep)
    register FSServer *svr;
    register fsGenericReply *rep;
{
    register unsigned long newseq,
                lastseq;

    newseq = (svr->last_request_read & ~((unsigned long) 0xffff)) |
	rep->sequenceNumber;
    lastseq = svr->last_request_read;
    while (newseq < lastseq) {
	newseq += 0x10000;
	if (newseq > svr->request) {
	    (void) fprintf(stderr,
	       "FSlib:  sequence lost (0x%lx > 0x%lx) in reply type 0x%x!\n",
			   newseq, svr->request,
			   (unsigned int) rep->type);
	    newseq -= 0x10000;
	    break;
	}
    }

    svr->last_request_read = newseq;
    return (newseq);
}

/*
 * _FSReply - Wait for a reply packet and copy its contents into the
 * specified rep.  Mean while we must handle error and event packets that
 * we may encounter.
 */
Status
_FSReply(svr, rep, extra, discard)
    register FSServer *svr;
    register fsReply *rep;
    int         extra;		/* number of 32-bit words expected after the
				 * reply */
    Bool        discard;	/* should I discard data followind "extra"
				 * words? */
{
#ifndef AMOEBA
    /*
     * Pull out the serial number now, so that (currently illegal) requests
     * generated by an error handler don't confuse us.
     */
    unsigned long cur_request = svr->request;
    long rem_length;

    _FSFlush(svr);
    while (1) {
	_FSRead(svr, (char *) rep, (long) SIZEOF(fsReply));
	switch ((int) rep->generic.type) {

	case FS_Reply:
	    /*
	     * Reply received.  Fast update for synchronous replies, but deal
	     * with multiple outstanding replies.
	     */
	    if (rep->generic.sequenceNumber == (cur_request & 0xffff))
		svr->last_request_read = cur_request;
	    else
		(void) _FSSetLastRequestRead(svr, &rep->generic);
	    rem_length = rep->generic.length - (SIZEOF(fsReply) >> 2);
	    if (rem_length < 0) rem_length = 0;
	    if (extra == 0) {
		if (discard && rem_length)
		    /* unexpectedly long reply! */
		    _EatData32(svr, rem_length);
		return (1);
	    }
	    if (extra == rem_length) {
		/*
		 * Read the extra data into storage immediately following the
		 * GenericReply structure.
		 */
		_FSRead(svr, (char *) NEXTPTR(rep, fsReply), ((long) extra) << 2);
		return (1);
	    }
	    if (extra < rem_length) {
		/* Actual reply is longer than "extra" */
		_FSRead(svr, (char *) NEXTPTR(rep, fsReply), ((long) extra) << 2);
		if (discard)
		    _EatData32(svr, rem_length - extra);
		return (1);
	    }
	    /*
	     * if we get here, then extra > rem_length -- meaning we
	     * read a reply that's shorter than we expected.  This is an
	     * error,  but we still need to figure out how to handle it...
	     */
	    _FSRead(svr, (char *) NEXTPTR(rep, fsReply), rem_length << 2);
	    (*_FSIOErrorFunction) (svr);
	    return (0);

	case FS_Error:
	    {
		register _FSExtension *ext;
		register Bool ret = False;
		int         ret_code;
		fsError     err;
		unsigned long serial;
		long        err_data;

		err = *(fsError *) rep;
		/* read the rest of the error */
		_FSRead(svr, (char *) &err + sizeof(fsReply),
			(long) (sizeof(fsError) - SIZEOF(fsReply)));
		serial = _FSSetLastRequestRead(svr, (fsGenericReply *) rep);
		if (serial == cur_request)
		    /* do not die on certain failures */
		    switch ((int) err.request) {
			/* suck in any extra error info */
		    case FSBadResolution:
		    case FSBadLength:
		    case FSBadIDChoice:
		    case FSBadRange:
		    case FSBadFont:
		    case FSBadFormat:
			_FSRead(svr, (char *) &err_data, 4);
			break;
		    case FSBadAccessContext:
			_FSRead(svr, (char *) &err_data, 4);
			return 0;
		    case FSBadAlloc:
			return (0);
			/*
			 * we better see if there is an extension who may want
			 * to suppress the error.
			 */
		    default:
			ext = svr->ext_procs;
			while (ext) {
			    if (ext->error != NULL)
				ret = (*ext->error)
				    (svr, &err, &ext->codes, &ret_code);
			    ext = ext->next;
			}
			if (ret)
			    return (ret_code);
			break;
		    }
		_FSError(svr, &err);
		if (serial == cur_request)
		    return (0);
	    }
	    break;
	default:
	    _FSEnq(svr, (fsEvent *) rep);
	    break;
	}
    }
#endif
}


/* Read and discard "n" 8-bit bytes of data */

void
_FSEatData(svr, n)
    FSServer   *svr;
    register unsigned long n;
{
#ifndef AMOEBA
#define SCRATCHSIZE 2048
    char        buf[SCRATCHSIZE];

    while (n > 0) {
	register long bytes_read = (n > SCRATCHSIZE) ? SCRATCHSIZE : n;

	_FSRead(svr, buf, bytes_read);
	n -= bytes_read;
    }
#undef SCRATCHSIZE
#endif
}


/* Read and discard "n" 32-bit words. */

static void
_EatData32(svr, n)
    FSServer   *svr;
    unsigned long n;
{
#ifndef AMOEBA
    _FSEatData(svr, n << 2);
#endif
}


/*
 * _FSEnq - Place event packets on the display's queue.
 * note that no squishing of move events in V11, since there
 * is pointer motion hints....
 */
_FSEnq(svr, event)
    register FSServer *svr;
    register fsEvent *event;
{
#ifndef AMOEBA
    register _FSQEvent *qelt;

/*NOSTRICT*/
    if (qelt = _FSqfree) {
	/* If _FSqfree is non-NULL do this, else malloc a new one. */
	_FSqfree = qelt->next;
    } else if ((qelt =
	     (_FSQEvent *) FSmalloc((unsigned) sizeof(_FSQEvent))) == NULL) {
	/* Malloc call failed! */
	errno = ENOMEM;
	(*_FSIOErrorFunction) (svr);
    }
    qelt->next = NULL;
    /* go call through display to find proper event reformatter */
    if ((*svr->event_vec[event->type & 0177]) (svr, &qelt->event, event)) {
	if (svr->tail)
	    svr->tail->next = qelt;
	else
	    svr->head = qelt;

	svr->tail = qelt;
	svr->qlen++;
    } else {
	/* ignored, or stashed away for many-to-one compression */
	qelt->next = _FSqfree;
	_FSqfree = qelt;
    }
#endif
}

/*
 * EventToWire in seperate file in that often not needed.
 */

/*ARGSUSED*/
Bool
_FSUnknownWireEvent(svr, re, event)
    register FSServer *svr;	/* pointer to display structure */
    register FSEvent *re;	/* pointer to where event should be
				 * reformatted */
    register fsEvent *event;	/* wire protocol event */
{

#ifdef notdef
    (void) fprintf(stderr,
	   "FSlib: unhandled wire event! event number = %d, display = %x\n.",
		   event->type, svr);
#endif

    return (False);
}

/*ARGSUSED*/
Status
_FSUnknownNativeEvent(svr, re, event)
    register FSServer *svr;	/* pointer to display structure */
    register FSEvent *re;	/* pointer to where event should be
				 * reformatted */
    register fsEvent *event;	/* wire protocol event */
{

#ifdef notdef
    (void) fprintf(stderr,
	 "FSlib: unhandled native event! event number = %d, display = %x\n.",
		   re->type, svr);
#endif

    return (0);
}

/*
 * reformat a wire event into an FSEvent structure of the right type.
 */
Bool
_FSWireToEvent(svr, re, event)
    register FSServer *svr;	/* pointer to display structure */
    register FSEvent *re;	/* pointer to where event should be
				 * reformatted */
    register fsEvent *event;	/* wire protocol event */
{

    re->type = event->type & 0x7f;
    ((FSAnyEvent *) re)->serial = _FSSetLastRequestRead(svr,
						   (fsGenericReply *) event);
    ((FSAnyEvent *) re)->send_event = ((event->type & 0x80) != 0);
    ((FSAnyEvent *) re)->server = svr;

    /*
     * Ignore the leading bit of the event type since it is set when a client
     * sends an event rather than the server.
     */

    switch (event->type & 0177) {
    default:
	return (_FSUnknownWireEvent(svr, re, event));
    }
}


static char *
_SysErrorMsg(n)
    int         n;
{
#ifndef X_NO_STRERROR
    return strerror(n);
#else
    extern char *sys_errlist[];
    extern int  sys_nerr;
    char       *s = ((n >= 0 && n < sys_nerr) ? sys_errlist[n] : "unknown error");

    return (s ? s : "no such error");
#endif
}

/*
 * _FSIOError - Default fatal system error reporting routine.  Called when
 * an X internal system error is encountered.
 */
_FSIOError(svr)
    FSServer   *svr;
{
    (void) fprintf(stderr,
		   "FSIO:  fatal IO error %d (%s) on font server \"%s\"\r\n",
		   errno, _SysErrorMsg(errno), FSServerString(svr));
    (void) fprintf(stderr,
		   "      after %lu requests (%lu known processed) with %d events remaining.\r\n",
		   NextRequest(svr) - 1, LastKnownRequestProcessed(svr),
		   QLength(svr));

    if (errno == EPIPE) {
	(void) fprintf(stderr,
	"      The connection was probably broken by a server shutdown.\r\n");
    }
    exit(1);
}

/*
 * _FSError - Default non-fatal error reporting routine.  Called when an
 * FS_Error packet is encountered in the input stream.
 */
int
_FSError(svr, rep)
    FSServer   *svr;
    fsError    *rep;
{
    FSErrorEvent event;

    /*
     * FS_Error packet encountered!  We need to unpack the error before giving
     * it to the user.
     */

    event.server = svr;
    event.type = FS_Error;
    event.serial = _FSSetLastRequestRead(svr, (fsGenericReply *) rep);
    event.error_code = rep->request;
    event.request_code = rep->major_opcode;
    event.minor_code = rep->minor_opcode;
    if (_FSErrorFunction != NULL) {
	return ((*_FSErrorFunction) (svr, &event));
    }
    exit(1);
    /* NOTREACHED */
}

int
_FSDefaultIOError(svr)
    FSServer   *svr;
{
    (void) fprintf(stderr,
		   "FSIO:  fatal IO error %d (%s) on Font server \"%s\"\r\n",
		   errno, _SysErrorMsg(errno), FSServerString(svr));
    (void) fprintf(stderr,
		   "	after %lu requests (%lu known processed) with %d events remaining.\r\n",
		   NextRequest(svr) - 1, LastKnownRequestProcessed(svr),
		   QLength(svr));
    if (errno == EPIPE) {
	(void) fprintf(stderr,
		       "	The connection was probably broken by a server shutdown.\n\r");
    }
    exit(1);
}

int
_FSPrintDefaultError(svr, event, fp)
    FSServer   *svr;
    FSErrorEvent *event;
    FILE       *fp;
{
    char        buffer[BUFSIZ];
    char        mesg[BUFSIZ];
    char        number[32];
    char       *mtype = "FSlibMessage";
    register _FSExtension *ext = (_FSExtension *) NULL;

    FSGetErrorText(svr, event->error_code, buffer, BUFSIZ);
    FSGetErrorDatabaseText(svr, mtype, "FSError", "FS Error", mesg, BUFSIZ);
    (void) fprintf(fp, "%s:  %s\n  ", mesg, buffer);
    FSGetErrorDatabaseText(svr, mtype, "MajorCode", "Request Major code %d",
			   mesg, BUFSIZ);
    (void) fprintf(fp, mesg, event->request_code);
    if (event->request_code < 128) {
	sprintf(number, "%d", event->request_code);
	FSGetErrorDatabaseText(svr, "FSRequest", number, "", buffer, BUFSIZ);
    } else {
	for (ext = svr->ext_procs;
		ext && (ext->codes.major_opcode != event->request_code);
		ext = ext->next);
	if (ext)
	    strcpy(buffer, ext->name);
	else
	    buffer[0] = '\0';
    }
    (void) fprintf(fp, " (%s)\n  ", buffer);
    FSGetErrorDatabaseText(svr, mtype, "MinorCode", "Request Minor code %d",
			   mesg, BUFSIZ);
    (void) fprintf(fp, mesg, event->minor_code);
    if (ext) {
	sprintf(mesg, "%s.%d", ext->name, event->minor_code);
	FSGetErrorDatabaseText(svr, "FSRequest", mesg, "", buffer, BUFSIZ);
	(void) fprintf(fp, " (%s)", buffer);
    }
    fputs("\n  ", fp);
    FSGetErrorDatabaseText(svr, mtype, "ResourceID", "ResourceID 0x%x",
			   mesg, BUFSIZ);
    (void) fprintf(fp, mesg, event->resourceid);
    fputs("\n  ", fp);
    FSGetErrorDatabaseText(svr, mtype, "ErrorSerial", "Error Serial #%d",
			   mesg, BUFSIZ);
    (void) fprintf(fp, mesg, event->serial);
    fputs("\n  ", fp);
    FSGetErrorDatabaseText(svr, mtype, "CurrentSerial", "Current Serial #%d",
			   mesg, BUFSIZ);
    (void) fprintf(fp, mesg, svr->request);
    fputs("\n", fp);
    return 1;
}

int
_FSDefaultError(svr, event)
    FSServer   *svr;
    FSErrorEvent *event;
{
    if (_FSPrintDefaultError(svr, event, stderr) == 0)
	return 0;
    exit(1);
    /* NOTREACHED */
}

int         (*_FSIOErrorFunction) () = _FSIOError;
int         (*_FSErrorFunction) () = _FSDefaultError;

/*
 * This routine can be used to (cheaply) get some memory within a single
 * Xlib routine for scratch space.  It is reallocated from the same place
 * each time, unless the library needs a large scratch space.
 */
char       *
_FSAllocScratch(svr, nbytes)
    register FSServer *svr;
    unsigned long nbytes;
{
    if (nbytes > svr->scratch_length) {
	if (svr->scratch_buffer != NULL)
	    FSfree(svr->scratch_buffer);
	return (svr->scratch_length = nbytes,
		svr->scratch_buffer = FSmalloc((unsigned) nbytes));
    }
    return (svr->scratch_buffer);
}

FSFree(data)
    char       *data;
{
    FSfree(data);
}

unsigned char *
FSMalloc(size)
    unsigned    size;
{
    return (unsigned char *) FSmalloc(size);
}

#ifdef DataRoutineIsProcedure
void
Data(svr, data, len)
    FSServer   *svr;
    char       *data;
    long        len;
{
    if (svr->bufptr + (len) <= svr->bufmax) {
	bcopy(data, svr->bufptr, (int) len);
	svr->bufptr += ((len) + 3) & ~3;
    } else {
	_FSSend(svr, data, len);
    }
}

#endif				/* DataRoutineIsProcedure */


#ifdef WORD64
/*
 * XXX This is a *really* stupid way of doing this.  It should just use
 * svr->bufptr directly, taking into account where in the word it is.
 */

/*
 * Data16 - Place 16 bit data in the buffer.
 *
 * "svr" is a pointer to a FSServer.
 * "data" is a pointer to the data.
 * "len" is the length in bytes of the data.
 */

static
doData16(svr, data, len, packbuffer)
    register FSServer *svr;
    short      *data;
    unsigned    len;
    char       *packbuffer;
{
    long       *lp,
               *lpack;
    long        i,
                nwords,
                bits;
    long        mask16 = 0x000000000000ffff;

    lp = (long *) data;
    lpack = (long *) packbuffer;
    *lpack = 0;

/*  nwords is the number of 16 bit values to be packed,
 *  the low order 16 bits of each word will be packed
 *  into 64 bit words
 */
    nwords = len >> 1;
    bits = 48;

    for (i = 0; i < nwords; i++) {
	*lpack ^= (*lp & mask16) << bits;
	bits -= 16;
	lp++;
	if (bits < 0) {
	    lpack++;
	    *lpack = 0;
	    bits = 48;
	}
    }
    Data(svr, packbuffer, len);
}

Data16(svr, data, len)
    FSServer   *svr;
    short      *data;
    unsigned    len;
{
    char        packbuffer[PACKBUFFERSIZE];
    unsigned    nwords = (PACKBUFFERSIZE >> 1);	/* bytes to CARD16 */

    for (; len > nwords; len -= nwords, data += nwords) {
	doData16(svr, data, nwords, packbuffer);
    }
    doData16(svr, data, len, packbuffer);
}

/*
 * Data32 - Place 32 bit data in the buffer.
 *
 * "svr" is a pointer to a FSServer.
 * "data" is a pointer to the data.
 * "len" is the length in bytes of the data.
 */

static
doData32(svr, data, len, packbuffer)
    register FSServer *svr;
    long       *data;
    unsigned    len;
    char       *packbuffer;
{
    long       *lp,
               *lpack;
    long        i,
                bits,
                nwords;
    long        mask32 = 0x00000000ffffffff;

    lpack = (long *) packbuffer;
    lp = data;

    *lpack = 0;

/*  nwords is the number of 32 bit values to be packed
 *  the low order 32 bits of each word will be packed
 *  into 64 bit words
 */
    nwords = len >> 2;
    bits = 32;

    for (i = 0; i < nwords; i++) {
	*lpack ^= (*lp & mask32) << bits;
	bits = bits ^ 32;
	lp++;
	if (bits) {
	    lpack++;
	    *lpack = 0;
	}
    }
    Data(svr, packbuffer, len);
}

Data32(svr, data, len)
    FSServer   *svr;
    short      *data;
    unsigned    len;
{
    char        packbuffer[PACKBUFFERSIZE];
    unsigned    nwords = (PACKBUFFERSIZE >> 2);	/* bytes to CARD32 */

    for (; len > nwords; len -= nwords, data += nwords) {
	doData32(svr, data, nwords, packbuffer);
    }
    doData32(svr, data, len, packbuffer);
}

#endif				/* WORD64 */



/*
 * _FSFreeQ - free the queue of events, called by XCloseServer when there are
 * no more displays left on the display list
 */

void
_FSFreeQ()
{
    register _FSQEvent *qelt = _FSqfree;

    while (qelt) {
	register _FSQEvent *qnext = qelt->next;

	FSfree(qelt);
	qelt = qnext;
    }
    _FSqfree = NULL;
    return;
}

#ifdef _POSIX_SOURCE                     /* stupid makedepend [need if] */
#define NEED_UTSNAME
#endif
#ifdef hpux
#define NEED_UTSNAME
#endif
#ifdef USG
#define NEED_UTSNAME
#endif
#ifdef SVR4
#define NEED_UTSNAME
#endif

#ifdef NEED_UTSNAME
#include <sys/utsname.h>
#endif


/*
 * _FSGetHostname - similar to gethostname but allows special processing.
 */
int
_FSGetHostname(buf, maxlen)
    char       *buf;
    int         maxlen;
{
    int         len;

#ifdef NEED_UTSNAME
    /*
     * same host name crock as in server and xinit.
     */
    struct utsname name;

    uname(&name);
    len = strlen(name.nodename);
    if (len >= maxlen)
	len = maxlen - 1;
    strncpy(buf, name.nodename, len);
    buf[len] = '\0';
#else
    buf[0] = '\0';
    (void) gethostname(buf, maxlen);
    buf[maxlen - 1] = '\0';
    len = strlen(buf);
#endif				/* NEED_UTSNAME */

    return len;
}

#ifndef _FSANYSET
/*
 * This is not always a macro.
 */
_FSANYSET(src)
    long	*src;
{
    int i;

    for (i=0; i<MSKCNT; i++)
	if (src[ i ])
	    return (1);
    return (0);
}
#endif

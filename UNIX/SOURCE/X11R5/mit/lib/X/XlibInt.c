/* $XFree86: mit/lib/X/XlibInt.c,v 2.0 1993/07/28 11:56:08 dawes Exp $ */
/*
 * $XConsortium: XlibInt.c,v 11.156.1.1 92/11/11 10:10:50 rws Exp $
 */

/* Copyright    Massachusetts Institute of Technology    1985, 1986, 1987 */

/*
Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  M.I.T. makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.
*/

/*
 *	XlibInternal.c - Internal support routines for the C subroutine
 *	interface library (Xlib) to the X Window System Protocol V11.0.
 */
#define NEED_EVENTS
#define NEED_REPLIES

#include <X11/Xlibint.h>
#include <X11/Xos.h>
#include "Xlibnet.h"
#include <stdio.h>
#if defined(AMOEBA) || defined(_MINIX)
#include <assert.h>
#endif

static void _EatData32();

/* check for both EAGAIN and EWOULDBLOCK, because some supposedly POSIX
 * systems are broken and return EWOULDBLOCK when they should return EAGAIN
 */
#if !defined(AMOEBA) && !defined(_MINIX)
#if defined(EAGAIN) && defined(EWOULDBLOCK)
#define ETEST(err) (err == EAGAIN || err == EWOULDBLOCK)
#else
#ifdef EAGAIN
#define ETEST(err) (err == EAGAIN)
#else
#define ETEST(err) (err == EWOULDBLOCK)
#endif
#endif
#else  /* AMOEBA || _MINIX */
#define ETEST(err)  0	/* EAGAIN or EWOULDBLOCK not expected */
#endif /* AMOEBA || _MINIX */

#ifdef LACHMAN
#ifdef EMSGSIZE
#undef EMSGSIZE
#endif
#define EMSGSIZE ERANGE
#endif

/*
 * The following routines are internal routines used by Xlib for protocol
 * packet transmission and reception.
 *
 * XIOError(Display *) will be called if any sort of system call error occurs.
 * This is assumed to be a fatal condition, i.e., XIOError should not return.
 *
 * XError(Display *, XErrorEvent *) will be called whenever an X_Error event is
 * received.  This is not assumed to be a fatal condition, i.e., it is
 * acceptable for this procedure to return.  However, XError should NOT
 * perform any operations (directly or indirectly) on the DISPLAY.
 *
 * Routines declared with a return type of 'Status' return 0 on failure,
 * and non 0 on success.  Routines with no declared return type don't 
 * return anything.  Whenever possible routines that create objects return
 * the object they have created.
 */

extern _XQEvent *_qfree;

static int padlength[4] = {0, 3, 2, 1};
    /* lookup table for adding padding bytes to data that is read from
    	or written to the X socket.  */

static xReq _dummy_request = {
	0, 0, 0
};
/*
 * _XFlush - Flush the X request buffer.  If the buffer is empty, no
 * action is taken.  This routine correctly handles incremental writes.
 * This routine may have to be reworked if int < long.
 */
_XFlush (dpy)
	register Display *dpy;
{
	register long size, todo;
	register int write_stat;
	register char *bufindex;

	if (dpy->flags & XlibDisplayIOError) return;

	size = todo = dpy->bufptr - dpy->buffer;
	bufindex = dpy->bufptr = dpy->buffer;
	/*
	 * While write has not written the entire buffer, keep looping
	 * until the entire buffer is written.  bufindex will be incremented
	 * and size decremented as buffer is written out.
	 */
	while (size) {
	    errno = 0;
#ifndef _MINIX
	    write_stat = WriteToServer(dpy->fd, bufindex, (int) todo);
#else
	    write_stat = MNX_WriteToServer(dpy, bufindex, (int) todo);
#endif
	    if (write_stat >= 0) {
		size -= write_stat;
		todo = size;
		bufindex += write_stat;
	    } else if (ETEST(errno)) {
		_XWaitForWritable(dpy);
#ifdef SUNSYSV
	    } else if (errno == 0) {
		_XWaitForWritable(dpy);
#endif
#ifdef EMSGSIZE
	    } else if (errno == EMSGSIZE) {
		if (todo > 1) 
		  todo >>= 1;
		else
		  _XWaitForWritable(dpy);
#endif
#ifdef CLIENTS_LOCALCONN
	    } else if (errno == EAGAIN) {
	        _XWaitForWritable(dpy);
	    } else if (errno == ERANGE) {
	        if (todo > 1) 
		  todo >>= 1;
	        else 
		  _XWaitForWritable(dpy);
#endif
	    } else if (errno != EINTR) {
		/* Write failed! */
		/* errno set by write system call. */
		_XIOError(dpy);
	    }
	}
	dpy->last_req = (char *)&_dummy_request;
}

int
_XEventsQueued (dpy, mode)
    register Display *dpy;
    int mode;
{
	register int len;
	int pend;
	char buf[BUFSIZE];
	register xReply *rep;
	
#ifdef AMOEBA
	threadswitch();
#endif
	if (mode == QueuedAfterFlush)
	{
	    _XFlush(dpy);
	    if (dpy->qlen)
		return(dpy->qlen);
	}
	if (dpy->flags & XlibDisplayIOError) return(dpy->qlen);
#ifndef _MINIX
	if (BytesReadable(dpy->fd, (char *) &pend) < 0)
#else
	if (MNX_BytesReadable(dpy, (char *) &pend) < 0)
#endif
	    _XIOError(dpy);
#ifdef XCONN_CHECK_FREQ
	/* This is a crock, required because FIONREAD or equivalent is
	 * not guaranteed to detect a broken connection.
	 */
	if (!pend && !dpy->qlen && ++dpy->conn_checker >= XCONN_CHECK_FREQ)
	{
	    unsigned long r_mask[MSKCNT];
	    static struct timeval zero_time;

	    dpy->conn_checker = 0;
	    CLEARBITS(r_mask);
	    BITSET(r_mask, dpy->fd);
	    if (pend = select(dpy->fd + 1, (fd_set *)r_mask, NULL, NULL,
			      &zero_time))
	    {
		if (pend > 0)
		{
		    if (BytesReadable(dpy->fd, (char *) &pend) < 0)
			_XIOError(dpy);
		    /* we should not get zero, if we do, force a read */
		    if (!pend)
			pend = SIZEOF(xReply);
		}
		else if (pend < 0 && errno != EINTR)
		    _XIOError(dpy);
	    }
	}
#endif /* XCONN_CHECK_FREQ */
	if (!(len = pend))
	    return(dpy->qlen);	/* _XFlush can enqueue events */
      /* Force a read if there is not enough data.  Otherwise,
       * a select() loop at a higher-level will spin undesirably,
       * and we've seen at least one OS that appears to not update
       * the result from FIONREAD once it has returned nonzero.
       */
	if (len < SIZEOF(xReply))
	    len = SIZEOF(xReply);
	else if (len > BUFSIZE)
	    len = BUFSIZE;
	len /= SIZEOF(xReply);
	pend = len * SIZEOF(xReply);
#ifdef XCONN_CHECK_FREQ
	dpy->conn_checker = 0;
#endif
	_XRead (dpy, buf, (long) pend);

	/* no space between comma and type or else macro will die */
	STARTITERATE (rep,xReply, buf, (len > 0), len--) {
	    if (rep->generic.type == X_Error)
		_XError(dpy, (xError *)rep);
	    else   /* must be an event packet */
		_XEnq(dpy, (xEvent *) rep);
	}
	ENDITERATE
	return(dpy->qlen);
}

/* _XReadEvents - Flush the output queue,
 * then read as many events as possible (but at least 1) and enqueue them
 */
_XReadEvents(dpy)
	register Display *dpy;
{
	char buf[BUFSIZE];
	long pend_not_register; /* because can't "&" a register variable */
	register long pend;
	register xEvent *ev;
	Bool not_yet_flushed = True;

	do {
	    /* find out how much data can be read */
#ifndef _MINIX
	    if (BytesReadable(dpy->fd, (char *) &pend_not_register) < 0)
#else
	    if (MNX_BytesReadable(dpy, (char *) &pend_not_register) < 0)
#endif
	    	_XIOError(dpy);
	    pend = pend_not_register;

	    /* must read at least one xEvent; if none is pending, then
	       we'll just flush and block waiting for it */
	    if (pend < SIZEOF(xEvent)) {
	    	pend = SIZEOF(xEvent);
		/* don't flush until we block the first time */
		if (not_yet_flushed) {
		    int qlen = dpy->qlen;
		    _XFlush (dpy);
		    if (qlen != dpy->qlen) return;
		    not_yet_flushed = False;
		}
	    }
		
	    /* but we won't read more than the max buffer size */
	    if (pend > BUFSIZE)
	    	pend = BUFSIZE;

	    /* round down to an integral number of XReps */
	    pend = (pend / SIZEOF(xEvent)) * SIZEOF(xEvent);

	    _XRead (dpy, buf, pend);

	    /* no space between comma and type or else macro will die */
	    STARTITERATE (ev,xEvent, buf, (pend > 0),
			  pend -= SIZEOF(xEvent)) {
		if (ev->u.u.type == X_Error)
		    _XError (dpy, (xError *) ev);
		else  /* it's an event packet; enqueue it */
		    _XEnq (dpy, ev);
	    }
	    ENDITERATE
	} while (dpy->head == NULL);
}

/* 
 * _XRead - Read bytes from the socket taking into account incomplete
 * reads.  This routine may have to be reworked if int < long.
 */
_XRead (dpy, data, size)
	register Display *dpy;
	register char *data;
	register long size;
{
	register long bytes_read;
#if defined(SVR4) && defined(SYSV386)
	int have_waited=0;
#endif

	if ((dpy->flags & XlibDisplayIOError) || size == 0) return;
	errno = 0;
#ifndef _MINIX
	while ((bytes_read = ReadFromServer(dpy->fd, data, (int)size))
#else
	while ((bytes_read = MNX_ReadFromServer(dpy, data, (int)size))
#endif
		!= size) {

	    	if (bytes_read > 0) {
		    size -= bytes_read;
		    data += bytes_read;
#if defined(SVR4) && defined(SYSV386)
		    have_waited=0;
#endif
		    }
#if defined(SVR4) && defined(SYSV386)
                /*
                 * The condition where select() says the server fd is
                 * readable, but read returns -1 means the connection
                 * has been closed.  This problem has been known to cause
                 * twm to stay running (and using CPU) after the server
                 * has finished.           DHD Dec 1991.
                 */
              	else if (ETEST(errno) && bytes_read < 0 && have_waited) {
                    errno = EPIPE;
                    _XIOError(dpy);
                }
#endif
		else if (ETEST(errno)) {
		    _XWaitForReadable(dpy);
#if defined(SVR4) && defined(SYSV386)
		    have_waited=1;
#endif
		    errno = 0;
		}
#ifdef SUNSYSV
		else if (errno == 0) {
		    _XWaitForReadable(dpy);
		}
#endif
#ifdef CLIENTS_LOCALCONN
		else if (errno == EAGAIN) {
		    _XWaitForReadable(dpy);
		    errno = 0;
		}
#endif
		else if (bytes_read == 0) {
		    /* Read failed because of end of file! */
		    errno = EPIPE;
		    _XIOError(dpy);
		    }

		else  /* bytes_read is less than 0; presumably -1 */ {
		    /* If it's a system call interrupt, it's not an error. */
		    if (errno != EINTR)
		    	_XIOError(dpy);
		    }
	    	 }
}

#ifdef WORD64

/*
 * XXX This is a *really* stupid way of doing this....
 * PACKBUFFERSIZE must be a multiple of 4.
 */

#define PACKBUFFERSIZE 4096


/*
 * _XRead32 - Read bytes from the socket unpacking each 32 bits
 *            into a long (64 bits on a CRAY computer).
 * 
 */
static _doXRead32 (dpy, data, size, packbuffer)
        register Display *dpy;
        register long *data;
        register long size;
	register char *packbuffer;
{
 long *lpack,*lp;
 long mask32 = 0x00000000ffffffff;
 long maskw, nwords, i, bits;

        _XReadPad (dpy, packbuffer, size);

        lp = data;
        lpack = (long *) packbuffer;
        nwords = size >> 2;
        bits = 32;

        for(i=0;i<nwords;i++){
            maskw = mask32 << bits;
           *lp++ = ( *lpack & maskw ) >> bits;
            bits = bits ^32;
            if(bits){
               lpack++;
            }
        }
}

_XRead32 (dpy, data, len)
    Display *dpy;
    long *data;
    long len;
{
    char packbuffer[PACKBUFFERSIZE];
    unsigned nunits = PACKBUFFERSIZE >> 2;

    for (; len > PACKBUFFERSIZE; len -= PACKBUFFERSIZE, data += nunits) {
	_doXRead32 (dpy, data, PACKBUFFERSIZE, packbuffer);
    }
    if (len) _doXRead32 (dpy, data, len, packbuffer);
}



/*
 * _XRead16 - Read bytes from the socket unpacking each 16 bits
 *            into a long (64 bits on a CRAY computer).
 *
 */
static _doXRead16 (dpy, data, size, packbuffer)
        register Display *dpy;
        register short *data;
        register long size;
	char *packbuffer;
{
	long *lpack,*lp;
	long mask16 = 0x000000000000ffff;
	long maskw, nwords, i, bits;

        _XRead(dpy,packbuffer,size);	/* don't do a padded read... */

        lp = (long *) data;
        lpack = (long *) packbuffer;
        nwords = size >> 1;  /* number of 16 bit words to be unpacked */
        bits = 48;
        for(i=0;i<nwords;i++){
            maskw = mask16 << bits;
           *lp++ = ( *lpack & maskw ) >> bits;
            bits -= 16;
            if(bits < 0){
               lpack++;
               bits = 48;
            }
        }
}

_XRead16 (dpy, data, len)
    Display *dpy;
    short *data;
    long len;
{
    char packbuffer[PACKBUFFERSIZE];
    unsigned nunits = PACKBUFFERSIZE >> 1;

    for (; len > PACKBUFFERSIZE; len -= PACKBUFFERSIZE, data += nunits) {
	_doXRead16 (dpy, data, PACKBUFFERSIZE, packbuffer);
    }
    if (len) _doXRead16 (dpy, data, len, packbuffer);
}

_XRead16Pad (dpy, data, size)
    Display *dpy;
    short *data;
    long size;
{
    int slop = (size & 3);
    short slopbuf[3];

    _XRead16 (dpy, data, size);
    if (slop > 0) {
	_XRead16 (dpy, slopbuf, 4 - slop);
    }
}
#endif /* WORD64 */


/*
 * _XReadPad - Read bytes from the socket taking into account incomplete
 * reads.  If the number of bytes is not 0 mod 32, read additional pad
 * bytes. This routine may have to be reworked if int < long.
 */
_XReadPad (dpy, data, size)
    	register Display *dpy;	
	register char *data;
	register long size;
{
    	register long bytes_read;
	struct iovec iov[2];
	char pad[3];

	if ((dpy->flags & XlibDisplayIOError) || size == 0) return;
	iov[0].iov_len = (int)size;
	iov[0].iov_base = data;
	/* 
	 * The following hack is used to provide 32 bit long-word
	 * aligned padding.  The [1] vector is of length 0, 1, 2, or 3,
	 * whatever is needed.
	 */

	iov[1].iov_len = padlength[size & 3];
	iov[1].iov_base = pad;
	size += iov[1].iov_len;
	errno = 0;
#ifndef _MINIX
	while ((bytes_read = ReadvFromServer (dpy->fd, iov, 2)) != size) {
#else
	while ((bytes_read = MNX_ReadvFromServer (dpy, iov, 2)) != size) {
#endif

	    if (bytes_read > 0) {
		size -= bytes_read;
	    	if ((iov[0].iov_len -= bytes_read) < 0) {
		    iov[1].iov_len += iov[0].iov_len;
		    iov[1].iov_base -= iov[0].iov_len;
		    iov[0].iov_len = 0;
		    }
	    	else
	    	    iov[0].iov_base += bytes_read;
	    	}
	    else if (ETEST(errno)) {
		_XWaitForReadable(dpy);
		errno = 0;
	    }
#ifdef SUNSYSV
	    else if (errno == 0) {
		_XWaitForReadable(dpy);
	    }
#endif
#ifdef CLIENTS_LOCALCONN
	    else if (errno == EAGAIN) {
		_XWaitForReadable(dpy);
		errno = 0;
	    }
#endif
	    else if (bytes_read == 0) {
		/* Read failed because of end of file! */
		errno = EPIPE;
		_XIOError(dpy);
		}
	    
	    else  /* bytes_read is less than 0; presumably -1 */ {
		/* If it's a system call interrupt, it's not an error. */
		if (errno != EINTR)
		    _XIOError(dpy);
		}
	    }

}

/*
 * _XSend - Flush the buffer and send the client data. 32 bit word aligned
 * transmission is used, if size is not 0 mod 4, extra bytes are transmitted.
 * This routine may have to be reworked if int < long;
 */
_XSend (dpy, data, size)
	register Display *dpy;
	char *data;
	register long size;
{
	struct iovec iov[3];
	static char pad[3] = {0, 0, 0};
           /* XText8 and XText16 require that the padding bytes be zero! */

	long skip = 0;
	long dpybufsize = (dpy->bufptr - dpy->buffer);
	long padsize = padlength[size & 3];
	long total = dpybufsize + size + padsize;
	long todo = total;

	if (dpy->flags & XlibDisplayIOError) return;

	/*
	 * There are 3 pieces that may need to be written out:
	 *
	 *     o  whatever is in the display buffer
	 *     o  the data passed in by the user
	 *     o  any padding needed to 32bit align the whole mess
	 *
	 * This loop looks at all 3 pieces each time through.  It uses skip
	 * to figure out whether or not a given piece is needed.
	 */
	while (total) {
	    long before = skip;		/* amount of whole thing written */
	    long remain = todo;		/* amount to try this time, <= total */
	    int i = 0;
	    long len;

	    /* You could be very general here and have "in" and "out" iovecs
	     * and write a loop without using a macro, but what the heck.  This
	     * translates to:
	     *
	     *     how much of this piece is new?
	     *     if more new then we are trying this time, clamp
	     *     if nothing new
	     *         then bump down amount already written, for next piece
	     *         else put new stuff in iovec, will need all of next piece
	     *
	     * Note that todo had better be at least 1 or else we'll end up
	     * writing 0 iovecs.
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

	    InsertIOV (dpy->buffer, dpybufsize)
	    InsertIOV (data, size)
	    InsertIOV (pad, padsize)
    
	    errno = 0;
#ifndef _MINIX
	    if ((len = WritevToServer(dpy->fd, iov, i)) >= 0) {
#else
	    if ((len = MNX_WritevToServer(dpy, iov, i)) >= 0) {
#endif
		skip += len;
		total -= len;
		todo = total;
	    } else if (ETEST(errno)) {
		_XWaitForWritable(dpy);
#ifdef SUNSYSV
	    } else if (errno == 0) {
		_XWaitForWritable(dpy);
#endif
#ifdef EMSGSIZE
	    } else if (errno == EMSGSIZE) {
		if (todo > 1) 
		  todo >>= 1;
		else 
		  _XWaitForWritable(dpy);
#endif
#ifdef CLIENTS_LOCALCONN
	    } else if (errno == EAGAIN) {
		_XWaitForWritable(dpy);
	    } else if (errno == ERANGE) {
		if (todo > 1) 
		  todo >>= 1;
		else 
		  _XWaitForWritable(dpy);
#endif
	    } else if (errno != EINTR) {
		_XIOError(dpy);
	    }
	}

	dpy->bufptr = dpy->buffer;
	dpy->last_req = (char *) & _dummy_request;
}

/*
 * _XAllocID - normal resource ID allocation routine.  A client
 * can roll his own and instatantiate it if he wants, but must
 * follow the rules.
 */
XID _XAllocID(dpy)
register Display *dpy;
{
   XID id;

   id = dpy->resource_id << dpy->resource_shift;
   if (id <= dpy->resource_mask) {
       dpy->resource_id++;
       return (dpy->resource_base + id);
   }
   if (id != 0x10000000) {
       (void) fprintf(stderr,
		      "Xlib: resource ID allocation space exhausted!\n");
       id = 0x10000000;
       dpy->resource_id = id >> dpy->resource_shift;
   }
   return id;
}

/*
 * The hard part about this is that we only get 16 bits from a reply.  Well,
 * then, we have three values that will march along, with the following
 * invariant:
 *	dpy->last_request_read <= rep->sequenceNumber <= dpy->request
 * The right choice for rep->sequenceNumber is the largest that
 * still meets these constraints.
 */

unsigned long
_XSetLastRequestRead(dpy, rep)
    register Display *dpy;
    register xGenericReply *rep;
{
    register unsigned long	newseq, lastseq;

    /*
     * KeymapNotify has no sequence number, but is always guaranteed
     * to immediately follow another event, except when generated via
     * SendEvent (hmmm).
     */
    if ((rep->type & 0x7f) == KeymapNotify)
	return(dpy->last_request_read);

    newseq = (dpy->last_request_read & ~((unsigned long)0xffff)) |
	     rep->sequenceNumber;
    lastseq = dpy->last_request_read;
    while (newseq < lastseq) {
	newseq += 0x10000;
	if (newseq > dpy->request) {
	    (void) fprintf (stderr, 
	    "Xlib:  sequence lost (0x%lx > 0x%lx) in reply type 0x%x!\n",
	                           newseq, dpy->request, 
				   (unsigned int) rep->type);
	    newseq -= 0x10000;
	    break;
	}
    }

    dpy->last_request_read = newseq;
    return(newseq);
}

/*
 * _XReply - Wait for a reply packet and copy its contents into the
 * specified rep.  Mean while we must handle error and event packets that
 * we may encounter.
 */
Status _XReply (dpy, rep, extra, discard)
    register Display *dpy;
    register xReply *rep;
    int extra;		/* number of 32-bit words expected after the reply */
    Bool discard;	/* should I discard data following "extra" words? */
{
    /* Pull out the serial number now, so that (currently illegal) requests
     * generated by an error handler don't confuse us.
     */
    unsigned long cur_request = dpy->request;

    if (dpy->flags & XlibDisplayIOError) return (0);

    _XFlush(dpy);
    while (1) {
	_XRead(dpy, (char *)rep, (long)SIZEOF(xReply));
	switch ((int)rep->generic.type) {

	    case X_Reply:
	        /* Reply received.  Fast update for synchronous replies,
		 * but deal with multiple outstanding replies.
		 */
	        if (rep->generic.sequenceNumber == (cur_request & 0xffff))
		    dpy->last_request_read = cur_request;
		else
		    (void) _XSetLastRequestRead(dpy, &rep->generic);
		if (extra == 0) {
		    if (discard && (rep->generic.length > 0))
		       /* unexpectedly long reply! */
		       _EatData32 (dpy, rep->generic.length);
		    return (1);
		    }
		if (extra == rep->generic.length) {
		    /* 
		     * Read the extra data into storage immediately following
		     * the GenericReply structure. 
		     */
		    _XRead (dpy, (char *) (NEXTPTR(rep,xReply)),
			    ((long)extra) << 2);
		    return (1);
		    }
		if (extra < rep->generic.length) {
		    /* Actual reply is longer than "extra" */
		    _XRead (dpy, (char *) (NEXTPTR(rep,xReply)),
			    ((long)extra) << 2);
		    if (discard)
		        _EatData32 (dpy, rep->generic.length - extra);
		    return (1);
		    }
		/* 
		 *if we get here, then extra > rep->generic.length--meaning we
		 * read a reply that's shorter than we expected.  This is an 
		 * error,  but we still need to figure out how to handle it...
		 */
		_XRead (dpy, (char *) (NEXTPTR(rep,xReply)),
			((long) rep->generic.length) << 2);
		_XIOError (dpy);
		return (0);

    	    case X_Error:
	    	{
	        register _XExtension *ext;
		register Bool ret = False;
		int ret_code;
		xError *err = (xError *) rep;
		unsigned long serial;

		serial = _XSetLastRequestRead(dpy, (xGenericReply *)rep);
		if (serial == cur_request)
			/* do not die on "no such font", "can't allocate",
			   "can't grab" failures */
			switch ((int)err->errorCode) {
			case BadName:
			    switch (err->majorCode) {
				case X_OpenFont:
				case X_LookupColor:
				case X_AllocNamedColor:
				    return(0);
			    }
			    break;
			case BadFont:
			    if (err->majorCode == X_QueryFont)
				return (0);
			    break;
			case BadAlloc:
			case BadAccess:
				return (0);
			}
		/* 
		 * we better see if there is an extension who may
		 * want to suppress the error.
		 */
		for (ext = dpy->ext_procs; !ret && ext; ext = ext->next) {
		    if (ext->error) 
		       ret = (*ext->error)(dpy, err, &ext->codes, &ret_code);
		}
		if (!ret) {
		    _XError(dpy, err);
		    ret_code = 0;
		}
		if (serial == cur_request)
		    return(ret_code);
		}
		break;
	    default:
		_XEnq(dpy, (xEvent *) rep);
		break;
	    }
	}
}   


/* Read and discard "n" 8-bit bytes of data */

void _XEatData (dpy, n)
    Display *dpy;
    register unsigned long n;
{
#define SCRATCHSIZE 2048
    char buf[SCRATCHSIZE];

    while (n > 0) {
	register long bytes_read = (n > SCRATCHSIZE) ? SCRATCHSIZE : n;
	_XRead (dpy, buf, bytes_read);
	n -= bytes_read;
    }
#undef SCRATCHSIZE
}


/* Read and discard "n" 32-bit words. */

static void _EatData32 (dpy, n)
    Display *dpy;
    unsigned long n;
{
    _XEatData (dpy, n << 2);
}


/*
 * _XEnq - Place event packets on the display's queue.
 * note that no squishing of move events in V11, since there
 * is pointer motion hints....
 */
_XEnq (dpy, event)
	register Display *dpy;
	register xEvent *event;
{
	register _XQEvent *qelt;

/*NOSTRICT*/
	if (qelt = _qfree) {
		/* If _qfree is non-NULL do this, else malloc a new one. */
		_qfree = qelt->next;
	}
	else if ((qelt = 
	    (_XQEvent *) Xmalloc((unsigned)sizeof(_XQEvent))) == NULL) {
		/* Malloc call failed! */
		errno = ENOMEM;
		_XIOError(dpy);
	}
	qelt->next = NULL;
	/* go call through display to find proper event reformatter */
	if ((*dpy->event_vec[event->u.u.type & 0177])(dpy, &qelt->event, event)) {
	    if (dpy->tail)	dpy->tail->next = qelt;
	    else 		dpy->head = qelt;
    
	    dpy->tail = qelt;
	    dpy->qlen++;
	} else {
	    /* ignored, or stashed away for many-to-one compression */
	    qelt->next = _qfree;
	    _qfree = qelt;
	}
}
/*
 * EventToWire in separate file in that often not needed.
 */

/*ARGSUSED*/
Bool
_XUnknownWireEvent(dpy, re, event)
register Display *dpy;	/* pointer to display structure */
register XEvent *re;	/* pointer to where event should be reformatted */
register xEvent *event;	/* wire protocol event */
{
#ifdef notdef
	(void) fprintf(stderr, 
	    "Xlib: unhandled wire event! event number = %d, display = %x\n.",
			event->u.u.type, dpy);
#endif
	return(False);
}

/*ARGSUSED*/
Status
_XUnknownNativeEvent(dpy, re, event)
register Display *dpy;	/* pointer to display structure */
register XEvent *re;	/* pointer to where event should be reformatted */
register xEvent *event;	/* wire protocol event */
{
#ifdef notdef
	(void) fprintf(stderr, 
 	   "Xlib: unhandled native event! event number = %d, display = %x\n.",
			re->type, dpy);
#endif
	return(0);
}
/*
 * reformat a wire event into an XEvent structure of the right type.
 */
Bool
_XWireToEvent(dpy, re, event)
register Display *dpy;	/* pointer to display structure */
register XEvent *re;	/* pointer to where event should be reformatted */
register xEvent *event;	/* wire protocol event */
{

	re->type = event->u.u.type & 0x7f;
	((XAnyEvent *)re)->serial = _XSetLastRequestRead(dpy,
					(xGenericReply *)event);
	((XAnyEvent *)re)->send_event = ((event->u.u.type & 0x80) != 0);
	((XAnyEvent *)re)->display = dpy;
	
	/* Ignore the leading bit of the event type since it is set when a
		client sends an event rather than the server. */

	switch (event-> u.u.type & 0177) {
	      case KeyPress:
	      case KeyRelease:
	        {
			register XKeyEvent *ev = (XKeyEvent*) re;
			ev->root 	= event->u.keyButtonPointer.root;
			ev->window 	= event->u.keyButtonPointer.event;
			ev->subwindow 	= event->u.keyButtonPointer.child;
			ev->time 	= event->u.keyButtonPointer.time;
			ev->x 		= cvtINT16toInt(event->u.keyButtonPointer.eventX);
			ev->y 		= cvtINT16toInt(event->u.keyButtonPointer.eventY);
			ev->x_root 	= cvtINT16toInt(event->u.keyButtonPointer.rootX);
			ev->y_root 	= cvtINT16toInt(event->u.keyButtonPointer.rootY);
			ev->state	= event->u.keyButtonPointer.state;
			ev->same_screen	= event->u.keyButtonPointer.sameScreen;
			ev->keycode 	= event->u.u.detail;
		}
	      	break;
	      case ButtonPress:
	      case ButtonRelease:
	        {
			register XButtonEvent *ev =  (XButtonEvent *) re;
			ev->root 	= event->u.keyButtonPointer.root;
			ev->window 	= event->u.keyButtonPointer.event;
			ev->subwindow 	= event->u.keyButtonPointer.child;
			ev->time 	= event->u.keyButtonPointer.time;
			ev->x 		= cvtINT16toInt(event->u.keyButtonPointer.eventX);
			ev->y 		= cvtINT16toInt(event->u.keyButtonPointer.eventY);
			ev->x_root 	= cvtINT16toInt(event->u.keyButtonPointer.rootX);
			ev->y_root 	= cvtINT16toInt(event->u.keyButtonPointer.rootY);
			ev->state	= event->u.keyButtonPointer.state;
			ev->same_screen	= event->u.keyButtonPointer.sameScreen;
			ev->button 	= event->u.u.detail;
		}
	        break;
	      case MotionNotify:
	        {
			register XMotionEvent *ev =   (XMotionEvent *)re;
			ev->root 	= event->u.keyButtonPointer.root;
			ev->window 	= event->u.keyButtonPointer.event;
			ev->subwindow 	= event->u.keyButtonPointer.child;
			ev->time 	= event->u.keyButtonPointer.time;
			ev->x 		= cvtINT16toInt(event->u.keyButtonPointer.eventX);
			ev->y 		= cvtINT16toInt(event->u.keyButtonPointer.eventY);
			ev->x_root 	= cvtINT16toInt(event->u.keyButtonPointer.rootX);
			ev->y_root 	= cvtINT16toInt(event->u.keyButtonPointer.rootY);
			ev->state	= event->u.keyButtonPointer.state;
			ev->same_screen	= event->u.keyButtonPointer.sameScreen;
			ev->is_hint 	= event->u.u.detail;
		}
	        break;
	      case EnterNotify:
	      case LeaveNotify:
		{
			register XCrossingEvent *ev   = (XCrossingEvent *) re;
			ev->root	= event->u.enterLeave.root;
			ev->window	= event->u.enterLeave.event;
			ev->subwindow	= event->u.enterLeave.child;
			ev->time	= event->u.enterLeave.time;
			ev->x		= cvtINT16toInt(event->u.enterLeave.eventX);
			ev->y		= cvtINT16toInt(event->u.enterLeave.eventY);
			ev->x_root	= cvtINT16toInt(event->u.enterLeave.rootX);
			ev->y_root	= cvtINT16toInt(event->u.enterLeave.rootY);
			ev->state	= event->u.enterLeave.state;
			ev->mode	= event->u.enterLeave.mode;
			ev->same_screen = (event->u.enterLeave.flags & 
				ELFlagSameScreen) && True;
			ev->focus	= (event->u.enterLeave.flags &
			  	ELFlagFocus) && True;
			ev->detail	= event->u.u.detail;
		}
		  break;
	      case FocusIn:
	      case FocusOut:
		{
			register XFocusChangeEvent *ev = (XFocusChangeEvent *) re;
			ev->window 	= event->u.focus.window;
			ev->mode	= event->u.focus.mode;
			ev->detail	= event->u.u.detail;
		}
		  break;
	      case KeymapNotify:
		{
			register XKeymapEvent *ev = (XKeymapEvent *) re;
			ev->window	= dpy->current;
			bcopy ((char *)((xKeymapEvent *) event)->map,
			       &ev->key_vector[1], 
			       sizeof (((xKeymapEvent *) event)->map));
		}
		break;
	      case Expose:
		{
			register XExposeEvent *ev = (XExposeEvent *) re;
			ev->window	= event->u.expose.window;
			ev->x		= event->u.expose.x;
			ev->y		= event->u.expose.y;
			ev->width	= event->u.expose.width;
			ev->height	= event->u.expose.height;
			ev->count	= event->u.expose.count;
		}
		break;
	      case GraphicsExpose:
		{
		    register XGraphicsExposeEvent *ev =
			(XGraphicsExposeEvent *) re;
		    ev->drawable	= event->u.graphicsExposure.drawable;
		    ev->x		= event->u.graphicsExposure.x;
		    ev->y		= event->u.graphicsExposure.y;
		    ev->width		= event->u.graphicsExposure.width;
		    ev->height		= event->u.graphicsExposure.height;
		    ev->count		= event->u.graphicsExposure.count;
		    ev->major_code	= event->u.graphicsExposure.majorEvent;
		    ev->minor_code	= event->u.graphicsExposure.minorEvent;
		}
		break;
	      case NoExpose:
		{
		    register XNoExposeEvent *ev = (XNoExposeEvent *) re;
		    ev->drawable	= event->u.noExposure.drawable;
		    ev->major_code	= event->u.noExposure.majorEvent;
		    ev->minor_code	= event->u.noExposure.minorEvent;
		}
		break;
	      case VisibilityNotify:
		{
		    register XVisibilityEvent *ev = (XVisibilityEvent *) re;
		    ev->window		= event->u.visibility.window;
		    ev->state		= event->u.visibility.state;
		}
		break;
	      case CreateNotify:
		{
		    register XCreateWindowEvent *ev =
			 (XCreateWindowEvent *) re;
		    ev->window		= event->u.createNotify.window;
		    ev->parent		= event->u.createNotify.parent;
		    ev->x		= cvtINT16toInt(event->u.createNotify.x);
		    ev->y		= cvtINT16toInt(event->u.createNotify.y);
		    ev->width		= event->u.createNotify.width;
		    ev->height		= event->u.createNotify.height;
		    ev->border_width	= event->u.createNotify.borderWidth;
		    ev->override_redirect	= event->u.createNotify.override;
		}
		break;
	      case DestroyNotify:
		{
		    register XDestroyWindowEvent *ev =
				(XDestroyWindowEvent *) re;
		    ev->window		= event->u.destroyNotify.window;
		    ev->event		= event->u.destroyNotify.event;
		}
		break;
	      case UnmapNotify:
		{
		    register XUnmapEvent *ev = (XUnmapEvent *) re;
		    ev->window		= event->u.unmapNotify.window;
		    ev->event		= event->u.unmapNotify.event;
		    ev->from_configure	= event->u.unmapNotify.fromConfigure;
		}
		break;
	      case MapNotify:
		{
		    register XMapEvent *ev = (XMapEvent *) re;
		    ev->window		= event->u.mapNotify.window;
		    ev->event		= event->u.mapNotify.event;
		    ev->override_redirect	= event->u.mapNotify.override;
		}
		break;
	      case MapRequest:
		{
		    register XMapRequestEvent *ev = (XMapRequestEvent *) re;
		    ev->window		= event->u.mapRequest.window;
		    ev->parent		= event->u.mapRequest.parent;
		}
		break;
	      case ReparentNotify:
		{
		    register XReparentEvent *ev = (XReparentEvent *) re;
		    ev->event		= event->u.reparent.event;
		    ev->window		= event->u.reparent.window;
		    ev->parent		= event->u.reparent.parent;
		    ev->x		= cvtINT16toInt(event->u.reparent.x);
		    ev->y		= cvtINT16toInt(event->u.reparent.y);
		    ev->override_redirect	= event->u.reparent.override;
		}
		break;
	      case ConfigureNotify:
		{
		    register XConfigureEvent *ev = (XConfigureEvent *) re;
		    ev->event	= event->u.configureNotify.event;
		    ev->window	= event->u.configureNotify.window;
		    ev->above	= event->u.configureNotify.aboveSibling;
		    ev->x	= cvtINT16toInt(event->u.configureNotify.x);
		    ev->y	= cvtINT16toInt(event->u.configureNotify.y);
		    ev->width	= event->u.configureNotify.width;
		    ev->height	= event->u.configureNotify.height;
		    ev->border_width  = event->u.configureNotify.borderWidth;
		    ev->override_redirect = event->u.configureNotify.override;
		}
		break;
	      case ConfigureRequest:
		{
		    register XConfigureRequestEvent *ev =
		        (XConfigureRequestEvent *) re;
		    ev->window		= event->u.configureRequest.window;
		    ev->parent		= event->u.configureRequest.parent;
		    ev->above		= event->u.configureRequest.sibling;
		    ev->x		= cvtINT16toInt(event->u.configureRequest.x);
		    ev->y		= cvtINT16toInt(event->u.configureRequest.y);
		    ev->width		= event->u.configureRequest.width;
		    ev->height		= event->u.configureRequest.height;
		    ev->border_width	= event->u.configureRequest.borderWidth;
		    ev->value_mask	= event->u.configureRequest.valueMask;
		    ev->detail  	= event->u.u.detail;
		}
		break;
	      case GravityNotify:
		{
		    register XGravityEvent *ev = (XGravityEvent *) re;
		    ev->window		= event->u.gravity.window;
		    ev->event		= event->u.gravity.event;
		    ev->x		= cvtINT16toInt(event->u.gravity.x);
		    ev->y		= cvtINT16toInt(event->u.gravity.y);
		}
		break;
	      case ResizeRequest:
		{
		    register XResizeRequestEvent *ev =
			(XResizeRequestEvent *) re;
		    ev->window		= event->u.resizeRequest.window;
		    ev->width		= event->u.resizeRequest.width;
		    ev->height		= event->u.resizeRequest.height;
		}
		break;
	      case CirculateNotify:
		{
		    register XCirculateEvent *ev = (XCirculateEvent *) re;
		    ev->window		= event->u.circulate.window;
		    ev->event		= event->u.circulate.event;
		    ev->place		= event->u.circulate.place;
		}
		break;
	      case CirculateRequest:
		{
		    register XCirculateRequestEvent *ev =
		        (XCirculateRequestEvent *) re;
		    ev->window		= event->u.circulate.window;
		    ev->parent		= event->u.circulate.event;
		    ev->place		= event->u.circulate.place;
		}
		break;
	      case PropertyNotify:
		{
		    register XPropertyEvent *ev = (XPropertyEvent *) re;
		    ev->window		= event->u.property.window;
		    ev->atom		= event->u.property.atom;
		    ev->time		= event->u.property.time;
		    ev->state		= event->u.property.state;
		}
		break;
	      case SelectionClear:
		{
		    register XSelectionClearEvent *ev =
			 (XSelectionClearEvent *) re;
		    ev->window		= event->u.selectionClear.window;
		    ev->selection	= event->u.selectionClear.atom;
		    ev->time		= event->u.selectionClear.time;
		}
		break;
	      case SelectionRequest:
		{
		    register XSelectionRequestEvent *ev =
		        (XSelectionRequestEvent *) re;
		    ev->owner		= event->u.selectionRequest.owner;
		    ev->requestor	= event->u.selectionRequest.requestor;
		    ev->selection	= event->u.selectionRequest.selection;
		    ev->target		= event->u.selectionRequest.target;
		    ev->property	= event->u.selectionRequest.property;
		    ev->time		= event->u.selectionRequest.time;
		}
		break;
	      case SelectionNotify:
		{
		    register XSelectionEvent *ev = (XSelectionEvent *) re;
		    ev->requestor	= event->u.selectionNotify.requestor;
		    ev->selection	= event->u.selectionNotify.selection;
		    ev->target		= event->u.selectionNotify.target;
		    ev->property	= event->u.selectionNotify.property;
		    ev->time		= event->u.selectionNotify.time;
		}
		break;
	      case ColormapNotify:
		{
		    register XColormapEvent *ev = (XColormapEvent *) re;
		    ev->window		= event->u.colormap.window;
		    ev->colormap	= event->u.colormap.colormap;
		    ev->new		= event->u.colormap.new;
		    ev->state		= event->u.colormap.state;
	        }
		break;
	      case ClientMessage:
		{
		   register int i;
		   register XClientMessageEvent *ev 
		   			= (XClientMessageEvent *) re;
		   ev->window		= event->u.clientMessage.window;
		   ev->format		= event->u.u.detail;
		   switch (ev->format) {
			case 8:	
			   ev->message_type = event->u.clientMessage.u.b.type;
			   for (i = 0; i < 20; i++) 	
			     ev->data.b[i] = event->u.clientMessage.u.b.bytes[i];
			   break;
			case 16:
			   ev->message_type = event->u.clientMessage.u.s.type;
			   ev->data.s[0] = cvtINT16toShort(event->u.clientMessage.u.s.shorts0);
			   ev->data.s[1] = cvtINT16toShort(event->u.clientMessage.u.s.shorts1);
			   ev->data.s[2] = cvtINT16toShort(event->u.clientMessage.u.s.shorts2);
			   ev->data.s[3] = cvtINT16toShort(event->u.clientMessage.u.s.shorts3);
			   ev->data.s[4] = cvtINT16toShort(event->u.clientMessage.u.s.shorts4);
			   ev->data.s[5] = cvtINT16toShort(event->u.clientMessage.u.s.shorts5);
			   ev->data.s[6] = cvtINT16toShort(event->u.clientMessage.u.s.shorts6);
			   ev->data.s[7] = cvtINT16toShort(event->u.clientMessage.u.s.shorts7);
			   ev->data.s[8] = cvtINT16toShort(event->u.clientMessage.u.s.shorts8);
			   ev->data.s[9] = cvtINT16toShort(event->u.clientMessage.u.s.shorts9);
			   break;
			case 32:
			   ev->message_type = event->u.clientMessage.u.l.type;
			   ev->data.l[0] = cvtINT32toLong(event->u.clientMessage.u.l.longs0);
			   ev->data.l[1] = cvtINT32toLong(event->u.clientMessage.u.l.longs1);
			   ev->data.l[2] = cvtINT32toLong(event->u.clientMessage.u.l.longs2);
			   ev->data.l[3] = cvtINT32toLong(event->u.clientMessage.u.l.longs3);
			   ev->data.l[4] = cvtINT32toLong(event->u.clientMessage.u.l.longs4);
			   break;
			default: /* XXX should never occur */
				break;
		    }
	        }
		break;
	      case MappingNotify:
		{
		   register XMappingEvent *ev = (XMappingEvent *)re;
		   ev->window		= 0;
		   ev->first_keycode 	= event->u.mappingNotify.firstKeyCode;
		   ev->request 		= event->u.mappingNotify.request;
		   ev->count 		= event->u.mappingNotify.count;
		}
		break;
	      default:
		return(_XUnknownWireEvent(dpy, re, event));
	}
	return(True);
}


#if defined(__STDC__) && !(defined(SYSV) || defined(SVR4))
#define _SysErrorMsg(x) strerror(x)
#else
#ifndef USL_SHARELIB

static char *_SysErrorMsg (n)
    int n;
{
    extern char *sys_errlist[];
    extern int sys_nerr;
    char *s = ((n >= 0 && n < sys_nerr) ? sys_errlist[n] : "unknown error");

    return (s ? s : "no such error");
}

#endif 	/* USL sharedlibs in don't define for SVR3.2 */
#endif /* __STDC__ */


/*
 * _XDefaultIOError - Default fatal system error reporting routine.  Called 
 * when an X internal system error is encountered.
 */
_XDefaultIOError (dpy)
	Display *dpy;
{
	(void) fprintf (stderr, 
	 "XIO:  fatal IO error %d (%s) on X server \"%s\"\r\n",
			errno, _SysErrorMsg (errno), DisplayString (dpy));
	(void) fprintf (stderr, 
	 "      after %lu requests (%lu known processed) with %d events remaining.\r\n",
			NextRequest(dpy) - 1, LastKnownRequestProcessed(dpy),
			QLength(dpy));

	if (errno == EPIPE) {
	    (void) fprintf (stderr,
	 "      The connection was probably broken by a server shutdown or KillClient.\r\n");
	}
	exit(1);
}


static int _XPrintDefaultError (dpy, event, fp)
    Display *dpy;
    XErrorEvent *event;
    FILE *fp;
{
    char buffer[BUFSIZ];
    char mesg[BUFSIZ];
    char number[32];
    char *mtype = "XlibMessage";
    register _XExtension *ext = (_XExtension *)NULL;
    _XExtension *bext = (_XExtension *)NULL;
    XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
    XGetErrorDatabaseText(dpy, mtype, "XError", "X Error", mesg, BUFSIZ);
    (void) fprintf(fp, "%s:  %s\n  ", mesg, buffer);
    XGetErrorDatabaseText(dpy, mtype, "MajorCode", "Request Major code %d", 
	mesg, BUFSIZ);
    (void) fprintf(fp, mesg, event->request_code);
    if (event->request_code < 128) {
	sprintf(number, "%d", event->request_code);
	XGetErrorDatabaseText(dpy, "XRequest", number, "", buffer, BUFSIZ);
    } else {
	for (ext = dpy->ext_procs;
	     ext && (ext->codes.major_opcode != event->request_code);
	     ext = ext->next)
	  ;
	if (ext)
	    strcpy(buffer, ext->name);
	else
	    buffer[0] = '\0';
    }
    (void) fprintf(fp, " (%s)\n", buffer);
    if (event->request_code >= 128) {
	XGetErrorDatabaseText(dpy, mtype, "MinorCode", "Request Minor code %d",
			      mesg, BUFSIZ);
	fputs("  ", fp);
	(void) fprintf(fp, mesg, event->minor_code);
	if (ext) {
	    sprintf(mesg, "%s.%d", ext->name, event->minor_code);
	    XGetErrorDatabaseText(dpy, "XRequest", mesg, "", buffer, BUFSIZ);
	    (void) fprintf(fp, " (%s)", buffer);
	}
	fputs("\n", fp);
    }
    if (event->error_code >= 128) {
	/* kludge, try to find the extension that caused it */
	buffer[0] = '\0';
	for (ext = dpy->ext_procs; ext; ext = ext->next) {
	    if (ext->error_string) 
		(*ext->error_string)(dpy, event->error_code, &ext->codes,
				     buffer, BUFSIZ);
	    if (buffer[0]) {
		bext = ext;
		break;
	    }
	    if (ext->codes.first_error &&
		ext->codes.first_error < event->error_code &&
		(!bext || ext->codes.first_error > bext->codes.first_error))
		bext = ext;
	}    
	if (bext)
	    sprintf(buffer, "%s.%d", bext->name,
		    event->error_code - bext->codes.first_error);
	else
	    strcpy(buffer, "Value");
	XGetErrorDatabaseText(dpy, mtype, buffer, "", mesg, BUFSIZ);
	if (mesg[0]) {
	    fputs("  ", fp);
	    (void) fprintf(fp, mesg, event->resourceid);
	    fputs("\n", fp);
	}
	/* let extensions try to print the values */
	for (ext = dpy->ext_procs; ext; ext = ext->next) {
	    if (ext->error_values)
		(*ext->error_values)(dpy, event, fp);
	}
    } else if ((event->error_code == BadWindow) ||
	       (event->error_code == BadPixmap) ||
	       (event->error_code == BadCursor) ||
	       (event->error_code == BadFont) ||
	       (event->error_code == BadDrawable) ||
	       (event->error_code == BadColor) ||
	       (event->error_code == BadGC) ||
	       (event->error_code == BadIDChoice) ||
	       (event->error_code == BadValue) ||
	       (event->error_code == BadAtom)) {
	if (event->error_code == BadValue)
	    XGetErrorDatabaseText(dpy, mtype, "Value", "Value 0x%x",
				  mesg, BUFSIZ);
	else if (event->error_code == BadAtom)
	    XGetErrorDatabaseText(dpy, mtype, "AtomID", "AtomID 0x%x",
				  mesg, BUFSIZ);
	else
	    XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID 0x%x",
				  mesg, BUFSIZ);
	fputs("  ", fp);
	(void) fprintf(fp, mesg, event->resourceid);
	fputs("\n", fp);
    }
    XGetErrorDatabaseText(dpy, mtype, "ErrorSerial", "Error Serial #%d", 
			  mesg, BUFSIZ);
    fputs("  ", fp);
    (void) fprintf(fp, mesg, event->serial);
    XGetErrorDatabaseText(dpy, mtype, "CurrentSerial", "Current Serial #%d",
			  mesg, BUFSIZ);
    fputs("\n  ", fp);
    (void) fprintf(fp, mesg, dpy->request);
    fputs("\n", fp);
    if (event->error_code == BadImplementation) return 0;
    return 1;
}

int _XDefaultError(dpy, event)
	Display *dpy;
	XErrorEvent *event;
{
    if (_XPrintDefaultError (dpy, event, stderr) == 0) return 0;
    exit(1);
    /*NOTREACHED*/
}

/*ARGSUSED*/
Bool _XDefaultWireError(display, he, we)
    Display     *display;
    XErrorEvent *he;
    xError      *we;
{
    return True;
}

/*
 * _XError - prepare to upcall user protocol error handler
 */
int _XError (dpy, rep)
    Display *dpy;
    xError *rep;
{
    /* 
     * X_Error packet encountered!  We need to unpack the error before
     * giving it to the user.
     */
    XEvent event; /* make it a large event */

    event.xerror.display = dpy;
    event.xerror.type = X_Error;
    event.xerror.serial = _XSetLastRequestRead(dpy, (xGenericReply *)rep);
    event.xerror.resourceid = rep->resourceID;
    event.xerror.error_code = rep->errorCode;
    event.xerror.request_code = rep->majorCode;
    event.xerror.minor_code = rep->minorCode;
    if (dpy->error_vec &&
	!(*dpy->error_vec[rep->errorCode])(dpy, &event.xerror, rep))
	return 0;
    if (_XErrorFunction != NULL) {
      	return ((*_XErrorFunction)(dpy, &event));	/* upcall */
    } else {
	return _XDefaultError(dpy, &event);
    }
}
    
/*
 * _XIOError - call user connection error handler and exit
 */
int _XIOError (dpy)
    Display *dpy;
{
    dpy->flags |= XlibDisplayIOError;
    if (_XIOErrorFunction != NULL)
	(*_XIOErrorFunction)(dpy);
    else
	_XDefaultIOError(dpy);
    exit (1);
}


/*
 * This routine can be used to (cheaply) get some memory within a single
 * Xlib routine for scratch space.  It is reallocated from the same place
 * each time, unless the library needs a large scratch space.
 */
char *_XAllocScratch (dpy, nbytes)
	register Display *dpy;
	unsigned long nbytes;
{
	if (nbytes > dpy->scratch_length) {
	    if (dpy->scratch_buffer) Xfree (dpy->scratch_buffer);
	    if (dpy->scratch_buffer = Xmalloc((unsigned) nbytes))
		dpy->scratch_length = nbytes;
	    else dpy->scratch_length = 0;
	}
	return (dpy->scratch_buffer);
}

/*
 * Given a visual id, find the visual structure for this id on this display.
 */
Visual *_XVIDtoVisual (dpy, id)
	Display *dpy;
	VisualID id;
{
	register int i, j, k;
	register Screen *sp;
	register Depth *dp;
	register Visual *vp;
	for (i = 0; i < dpy->nscreens; i++) {
		sp = &dpy->screens[i];
		for (j = 0; j < sp->ndepths; j++) {
			dp = &sp->depths[j];
			/* if nvisuals == 0 then visuals will be NULL */
			for (k = 0; k < dp->nvisuals; k++) {
				vp = &dp->visuals[k];
				if (vp->visualid == id) return (vp);
			}
		}
	}
	return (NULL);
}

#if NeedFunctionPrototypes
XFree (void *data)
#else
XFree (data)
	char *data;
#endif
{
	Xfree (data);
}

#ifdef _XNEEDBCOPYFUNC
void _Xbcopy(b1, b2, length)
    register char *b1, *b2;
    register length;
{
    if (b1 < b2) {
	b2 += length;
	b1 += length;
	while (length--)
	    *--b2 = *--b1;
    } else {
	while (length--)
	    *b2++ = *b1++;
    }
}
#endif

#ifdef DataRoutineIsProcedure
void Data (dpy, data, len)
	Display *dpy;
	char *data;
	long len;
{
	if (dpy->bufptr + (len) <= dpy->bufmax) {
		bcopy(data, dpy->bufptr, (int)len);
		dpy->bufptr += ((len) + 3) & ~3;
	} else {
		_XSend(dpy, data, len);
	}
}
#endif /* DataRoutineIsProcedure */


#ifdef WORD64

/*
 * XXX This is a *really* stupid way of doing this.  It should just use 
 * dpy->bufptr directly, taking into account where in the word it is.
 */

/*
 * Data16 - Place 16 bit data in the buffer.
 *
 * "dpy" is a pointer to a Display.
 * "data" is a pointer to the data.
 * "len" is the length in bytes of the data.
 */

static doData16(dpy, data, len, packbuffer)
    register Display *dpy;
    short *data;
    unsigned len;
    char *packbuffer;
{
    long *lp,*lpack;
    long i, nwords,bits;
    long mask16 = 0x000000000000ffff;

        lp = (long *)data;
        lpack = (long *)packbuffer;

/*  nwords is the number of 16 bit values to be packed,
 *  the low order 16 bits of each word will be packed
 *  into 64 bit words
 */
        nwords = len >> 1;
        bits = 48;

        for(i=0;i<nwords;i++){
	   if (bits == 48) *lpack = 0;
           *lpack ^= (*lp & mask16) << bits;
           bits -= 16 ;
           lp++;
           if(bits < 0){
               lpack++;
               bits = 48;
           }
        }
        Data(dpy, packbuffer, len);
}

Data16 (dpy, data, len)
    Display *dpy;
    short *data;
    unsigned len;
{
    char packbuffer[PACKBUFFERSIZE];
    unsigned nunits = PACKBUFFERSIZE >> 1;

    for (; len > PACKBUFFERSIZE; len -= PACKBUFFERSIZE, data += nunits) {
	doData16 (dpy, data, PACKBUFFERSIZE, packbuffer);
    }
    if (len) doData16 (dpy, data, len, packbuffer);
}

/*
 * Data32 - Place 32 bit data in the buffer.
 *
 * "dpy" is a pointer to a Display.
 * "data" is a pointer to the data.
 * "len" is the length in bytes of the data.
 */

static doData32 (dpy, data, len, packbuffer)
    register Display *dpy;
    long *data;
    unsigned len;
    char *packbuffer;
{
    long *lp,*lpack;
    long i,bits,nwords;
    long mask32 = 0x00000000ffffffff;

        lpack = (long *) packbuffer;
        lp = data;

/*  nwords is the number of 32 bit values to be packed
 *  the low order 32 bits of each word will be packed
 *  into 64 bit words
 */
        nwords = len >> 2;
        bits = 32;

        for(i=0;i<nwords;i++){
	   if (bits == 32) *lpack = 0;
           *lpack ^= (*lp & mask32) << bits;
           bits = bits ^32;
           lp++;
           if(bits)
              lpack++;
        }
        Data(dpy, packbuffer, len);
}

Data32 (dpy, data, len)
    Display *dpy;
    long *data;
    unsigned len;
{
    char packbuffer[PACKBUFFERSIZE];
    unsigned nunits = PACKBUFFERSIZE >> 2;

    for (; len > PACKBUFFERSIZE; len -= PACKBUFFERSIZE, data += nunits) {
	doData32 (dpy, data, PACKBUFFERSIZE, packbuffer);
    }
    if (len) doData32 (dpy, data, len, packbuffer);
}

#endif /* WORD64 */



/*
 * _XFreeQ - free the queue of events, called by XCloseDisplay
 */

void _XFreeQ ()
{
    register _XQEvent *qelt = _qfree;
  
    while (qelt) {
	register _XQEvent *qnxt = qelt->next;
	Xfree ((char *) qelt);
	qelt = qnxt;
    }
    _qfree = NULL;
    return;
}

/* Make sure this produces the same string as DefineLocal/DefineSelf in xdm.
 * Otherwise, Xau will not be able to find your cookies in the Xauthority file.
 *
 * Note: POSIX says that the ``nodename'' member of utsname does _not_ have
 *       to have sufficient information for interfacing to the network,
 *       and so, you may be better off using gethostname (if it exists).
 */

#if (defined(_POSIX_SOURCE) && !defined(AIXV3) && !defined(_MINIX)) || defined(hpux) || defined(USG) || defined(SYSV) || defined(SVR4)
#define NEED_UTSNAME
#include <sys/utsname.h>
#endif

/*
 * _XGetHostname - similar to gethostname but allows special processing.
 */
int _XGetHostname (buf, maxlen)
    char *buf;
    int maxlen;
{
    int len;

#ifdef NEED_UTSNAME
    struct utsname name;

    uname (&name);
    len = strlen (name.nodename);
    if (len >= maxlen) len = maxlen - 1;
    strncpy (buf, name.nodename, len);
    buf[len] = '\0';
#else
    buf[0] = '\0';
    (void) gethostname (buf, maxlen);
    buf [maxlen - 1] = '\0';
    len = strlen(buf);
#endif /* NEED_UTSNAME */
    return len;
}


/*
 * _XScreenOfWindow - get the Screen of a given window
 */

Screen *_XScreenOfWindow (dpy, w)
    Display *dpy;
    Window w;
{
    register int i;
    Window root;
    int x, y;				/* dummy variables */
    unsigned int width, height, bw, depth;  /* dummy variables */

    if (XGetGeometry (dpy, w, &root, &x, &y, &width, &height,
		      &bw, &depth) == False) {
	return None;
    }
    for (i = 0; i < ScreenCount (dpy); i++) {	/* find root from list */
	if (root == RootWindow (dpy, i)) {
	    return ScreenOfDisplay (dpy, i);
	}
    }
    return NULL;
}


#if (MSKCNT > 4)
/*
 * This is a macro if MSKCNT <= 4
 */
_XANYSET(src)
    long	*src;
{
    int i;

    for (i=0; i<MSKCNT; i++)
	if (src[ i ])
	    return (1);
    return (0);
}
#endif


#ifdef CRAY
/*
 * Cray UniCOS does not have readv and writev so we emulate
 */
#include <sys/socket.h>

int _XReadV (fd, iov, iovcnt)
int fd;
struct iovec *iov;
int iovcnt;
{
	struct msghdr hdr;

	hdr.msg_iov = iov;
	hdr.msg_iovlen = iovcnt;
	hdr.msg_accrights = 0;
	hdr.msg_accrightslen = 0;
	hdr.msg_name = 0;
	hdr.msg_namelen = 0;

	return (recvmsg (fd, &hdr, 0));
}

int _XWriteV (fd, iov, iovcnt)
int fd;
struct iovec *iov;
int iovcnt;
{
	struct msghdr hdr;

	hdr.msg_iov = iov;
	hdr.msg_iovlen = iovcnt;
	hdr.msg_accrights = 0;
	hdr.msg_accrightslen = 0;
	hdr.msg_name = 0;
	hdr.msg_namelen = 0;

	return (sendmsg (fd, &hdr, 0));
}

#endif /* CRAY */

#if defined(SYSV) && defined(SYSV386) && !defined(STREAMSCONN)
/*
 * SYSV/386 does not have readv so we emulate
 */
#include <sys/uio.h>

int _XReadV (fd, iov, iovcnt)
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
/*
 *	SCO doesn't have writev either
 */

int _XWriteV (fd, iov, iovcnt)
int fd;
struct iovec *iov;
int iovcnt;
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

#ifdef STREAMSCONN
/*
 * Copyright 1988, 1989 AT&T, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of AT&T not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  AT&T makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * AT&T DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL AT&T
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/*
 iovec.c (C source file)
	Acc: 575557389 Mon Mar 28 08:03:09 1988
	Mod: 575557397 Mon Mar 28 08:03:17 1988
	Sta: 575557397 Mon Mar 28 08:03:17 1988
	Owner: 2011
	Group: 1985
	Permissions: 664
*/
/*
	START USER STAMP AREA
*/
/*
	END USER STAMP AREA
*/


extern char _XsTypeofStream[];
extern Xstream _XsStream[];

#define MAX_WORKAREA 4096
static char workarea[MAX_WORKAREA];



int
_XReadV (fd, v, n)
    int		fd;
    struct iovec v[];
    int		n;
{
	int i, rc, len, size = 0;
	char * buf = workarea;
	char * p;

	if (n <= 0 || n > 16)
	{
		errno = EINVAL;
		return (-1);
	}
	for (i = 0; i < n; ++i)
	{
		if ((len = v[i].iov_len) < 0 || v[i].iov_base == NULL)
		{
			errno = EINVAL;
			return (-1);
		}
		size += len;
	}
	if ((size > MAX_WORKAREA) && ((buf = malloc (size)) == NULL))
	{
		errno = EINVAL;
		return (-1);
	}
	if((rc = (*_XsStream[_XsTypeOfStream[fd]].ReadFromStream)(fd, buf, size,
							     BUFFERING))> 0)
	{
		for (i = 0, p = buf; i < n; ++i)
		{
			memcpy (v[i].iov_base, p, len = v[i].iov_len);
			p += len;
		}
	}
	if (size > MAX_WORKAREA)
		free (buf);

	return (rc);
}

int
_XWriteV (fd, v, n)
    int fd;
    struct iovec v[];
    int n;
{
	int i, rc, len, size = 0;
	char * buf = workarea;
	char * p;

	if (n <= 0 || n > 16)
	{
		errno = EINVAL;
		return (-1);
	}
	for (i = 0; i < n; ++i)
	{
		if ((len = v[i].iov_len) < 0 || v[i].iov_base == NULL)
		{
			errno = EINVAL;
			return (-1);
		}
		size += len;
	}

	if ((size > MAX_WORKAREA) && ((buf = malloc (size)) == NULL))
	{
		errno = EINVAL;
		return (-1);
	}
	for (i = 0, p = buf; i < n; ++i)
	{
		memcpy (p, v[i].iov_base, len = v[i].iov_len);
		p += len;
	}
	rc = (*_XsStream[_XsTypeOfStream[fd]].WriteToStream)(fd, buf, size);

	if (size > MAX_WORKAREA)
		free (buf);

	return (rc);
}



#endif /* STREAMSCONN */

#if defined(SCO) /* && defined(TCPCONN) */
BytesReadable(fd, ptr)
     int fd, ptr;
{
  /*
   * On SCO CLIENTS_LOCALCONN uses STREAMS tty's and TCPCONN uses /dev/socksys.
   * The later is not a STREAMS driver and will not respond to I_NREAD.
   * On the other hand STREAMS will not respond to a FIONREAD. So we will
   * try both here and see if any case will work.
   */
  register int n;
  if ( (n = ioctl(fd, FIONREAD, ptr)) < 0 && errno == EINVAL)
    return ioctl(fd, I_NREAD, ptr);
  
  return n;
}
#endif /* SCO */

#ifdef AMOEBA
#include "XAmoeba.h"

/*
 * Philip's TCP/IP server silently assumes a
 * maximum buffer size of 30000 bytes.
 */
#define	TCPIP_BUFSIZE	16384

/*
 * Amoeba connection information is stored in, so called,
 * channel descriptors. Channel descriptors are identified
 * by their index in the table below.
 */
XAmChanDesc XAmChanDescriptors[OPEN_MAX];

/*
 * Cleanup connection descriptors
 */
void
XAmCleanUpChanDesc()
{
    register int i;

    for (i = 0; i < OPEN_MAX; i++) {
	if (XAmChanDescriptors[i].state != ACDS_FREE) {
	    /*
	     * The Amoeba TCP/IP server is capability oriented, i.e.
	     * it uses capabilities to identify connections. Since a
	     * capability is only destroyed when it has aged too much
	     * or is explicitly deleted, the connection it identifies
	     * will tend to exist for some while even if the client is
	     * already gone. To force connections to close this loop
	     * destroys all open TCP/IP connection. This loop us auto-
	     * matically executed when exit() is called.
	     */
	    XAmChanDescriptors[i].state = ACDS_CLOSED;
	    if (XAmChanDescriptors[i].type == ACDT_TCPIP)
		std_destroy(&XAmChanDescriptors[i].chancap);
	    if (XAmChanDescriptors[i].type == ACDT_VIRTCIRC)
		vc_close(XAmChanDescriptors[i].virtcirc, VC_BOTH);
	}
	XAmChanDescriptors[i].state = ACDS_FREE;
    }
}

/*
 * Cleanup connection descriptors on a signal
 */
void
XAmSignalCleanUpChanDesc(sig)
    int sig;
{
    XAmCleanUpChanDesc();
    _exit(sig | 0x80);
}

/*
 * Allocate a channel descriptor
 */
XAmChanDesc *
XAmAllocChanDesc()
{
    register int i;
    static int initialized = False;

    /*
     * Since the TCP/IP server is capability based its connections exists
     * even if the owner process is long gone. To overcome this nuisance,
     * a sweep is made over the connection descriptors when exit() is
     * called or when an un-catched (by application program) signal is 
     * received.
     */
    if (!initialized) {
	initialized = True;
	atexit(XAmCleanUpChanDesc);
	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
	    signal(SIGHUP, XAmSignalCleanUpChanDesc);
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
	    signal(SIGQUIT, XAmSignalCleanUpChanDesc);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
	    signal(SIGINT, XAmSignalCleanUpChanDesc);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
	    signal(SIGTERM, XAmSignalCleanUpChanDesc);
    }

    for (i = 0; i < OPEN_MAX; i++) {
	if (XAmChanDescriptors[i].state == ACDS_FREE) {
	    XAmChanDescriptors[i].state = ACDS_USED;
	    return &XAmChanDescriptors[i];
	}
    }
    return (XAmChanDesc *) NULL;
}

/*
 * Convert ``file descriptor'' to channel descriptor
 */
XAmChanDesc *
XAmFdToChanDesc(fd)
    int fd;
{
    assert(fd >= 0 && fd < OPEN_MAX);
    return &XAmChanDescriptors[fd];
}

/*
 * Convert channel descriptor to ``file descriptor''
 */
int
XAmChanDescToFd(chandesc)
    XAmChanDesc *chandesc;
{
    return chandesc - XAmChanDescriptors;
}

/*
 * Free channel descriptor
 */
void
XAmFreeChanDesc(chandesc)
    XAmChanDesc *chandesc;
{
    if (chandesc->sema) {
	Xfree(chandesc->sema);
	chandesc->sema = NULL;
    }
    chandesc->state = ACDS_FREE;
}

/*
 * Shutdown TCP/IP reader thread
 */
void
XAmReaderSignalCatcher(sig, us, extra)
    signum sig;
    thread_ustate *us;
    char *extra;
{
    register XAmChanDesc *chandesc = (XAmChanDesc *)extra;

    cb_close(chandesc->circbuf);
    chandesc->state = ACDS_CLOSED;
    thread_exit();
}

/*
 * TCP/IP reader thread
 */
void
XAmReaderThread(argptr, argsize)
    void *argptr;
    int argsize;
{
    register XAmChanDesc *chandesc;

    assert(argsize == sizeof(XAmChanDesc *));
    chandesc = *((XAmChanDesc **)argptr);
    (void)sig_catch(chandesc->signal, XAmReaderSignalCatcher, (char*)chandesc);
    while (True) {
	char buffer[CIRCBUFSIZE];
	bufsize size;

	size = tcpip_read(&chandesc->chancap, buffer, sizeof(buffer));
	if (ERR_STATUS(size)) {
	    fprintf(stderr, "Xlib: TCP/IP read failed (%s)\n",
		err_why(ERR_CONVERT(size)));
	    cb_close(chandesc->circbuf);
	    thread_exit();
	}

	if (size == 0 || cb_puts(chandesc->circbuf, buffer, size)) {
	    if (size != 0)
		fprintf(stderr, "Xlib: short write to circular buffer\n");
	    cb_close(chandesc->circbuf);
	    chandesc->state = ACDS_CLOSED;
	    thread_exit();
	}
    }
}

/*
 * Determine how many bytes are readable
 */
int
_XBytesReadable(ifd, count)
    int ifd;
    int *count;
{
    register XAmChanDesc *chandesc;

    errno = 0;
    chandesc = XAmFdToChanDesc(ifd);
    if (chandesc->state == ACDS_CLOSED) {
	errno = EBADF;
	*count = 0;
	return -1;
    }
    if (chandesc->type == ACDT_TCPIP)
	*count = cb_full(chandesc->circbuf);
    if (chandesc->type == ACDT_VIRTCIRC)
	*count = vc_avail(chandesc->virtcirc, VC_IN);
    if (*count < 0) {
	errno = chandesc->state == ACDS_CLOSED ? EINTR : EPIPE;
	*count = 0;
	return -1;
    }
    return 0;
}

/*
 * Wait until input is available or until the timer expires
 */
int
_XAmSelect(ifd, timout)
    int ifd;
    int timout;
{
    register XAmChanDesc *chandesc;
    register int n;

    errno = 0;
    chandesc = XAmFdToChanDesc(ifd);
    if (chandesc->state == ACDS_CLOSED) {
	errno = EBADF;
	return -1;
    }

    /*
     * Allocate semaphore to sleep on when no characters
     * are available. The underlying circular buffer and
     * virtual circuit packages manage this semaphore.
     */
    if (chandesc->sema == (semaphore *)NULL) {
	chandesc->sema = Xmalloc(sizeof(semaphore));
	sema_init(chandesc->sema, 0);
	if (chandesc->type == ACDT_TCPIP)
	    cb_setsema(chandesc->circbuf, chandesc->sema);
	else
	    vc_setsema(chandesc->virtcirc, chandesc->sema);
    }

    /*
     * TCP/IP connection
     */
    if (chandesc->type == ACDT_TCPIP) {
	if ((n = cb_full(chandesc->circbuf)) != 0) {
	    if (n < 0) errno = EPIPE;
	    return n; /* includes error as well */
	}
	if (sema_trydown(chandesc->sema, timout) < 0) {
	    errno = EINTR;
	    return -1;
	}
	if ((n = cb_full(chandesc->circbuf)) < 0)
	    errno = EPIPE;
	return n;
    }

    /*
     * Virtual circuit connection
     */
    if (chandesc->type == ACDT_VIRTCIRC) {
	if ((n = vc_avail(chandesc->virtcirc, VC_IN)) != 0) {
	    if (n < 0) errno = EPIPE;
	    return n; /* includes error as well */
	}
	if (sema_trydown(chandesc->sema, timout) < 0) {
	    errno = EINTR;
	    return -1;
	}
	if ((n = vc_avail(chandesc->virtcirc, VC_IN)) < 0)
	    errno = EPIPE;
	return n;
    }

    errno = EINVAL;
    return -1;
}

/*
 * Read ``count'' bytes from server
 */
int
_XReadFromServer(fdi, buf, count)
    int fdi, count;
    char *buf;
{
    register XAmChanDesc *chandesc;
    register int rv;

    errno = 0;
    chandesc = XAmFdToChanDesc(fdi);
    if (chandesc->state == ACDS_CLOSED) {
	errno = EBADF;
	return -1;
    }

    /*
     * TCP/IP connection
     */
    if (chandesc->type == ACDT_TCPIP) {
	rv = cb_gets(chandesc->circbuf, buf, count, count);
	if (rv != count) {
	    if (rv == 0) {
		fprintf(stderr, "Xlib: Cannot read circular buffer\n");
		errno = EPIPE;
		rv = -1;
	    } else
		fprintf(stderr, "Xlib: Cannot read circular buffer (%d)\n", rv);
	}
    }

    /*
     * Virtual circuit connection
     */
    if (chandesc->type == ACDT_VIRTCIRC) {
	rv = vc_readall(chandesc->virtcirc, buf, count);
	if (rv < 0) {
	    fprintf(stderr, "Xlib: Cannot read virtual circuit\n");
	    errno = EPIPE;
	    rv = -1;
	}
    }

    /*
     * The circular buffer writer will only UP the semaphore when
     * characters are available; we have to down it ourselfs.
     */
    if (chandesc->sema && rv != -1)
	sema_mdown(chandesc->sema, count);
    return rv;
}

/*
 * Write ``count'' bytes to server
 */
int
_XWriteToServer(fdi, buf, count)
    int fdi;
    char *buf;
    int count;
{
    register XAmChanDesc *chandesc;
    register int size;

    errno = 0;
    chandesc = XAmFdToChanDesc(fdi);
    if (chandesc->state == ACDS_CLOSED) {
	errno = EBADF;
	return -1;
    }

    /*
     * TCP/IP connection
     */
    if (chandesc->type == ACDT_TCPIP) {
	for (size = 0; count > 0; ) {
	    bufsize bsize;
	    int wrcnt;

	    wrcnt = count > TCPIP_BUFSIZE ? TCPIP_BUFSIZE : count;
	    bsize = tcpip_write(&chandesc->chancap, buf, wrcnt);
	    if (ERR_STATUS(bsize)) {
		fprintf(stderr, "Xlib: TCP/IP write failed (%s)\n",
		    tcpip_why(ERR_CONVERT(bsize)));
		errno = EPIPE;
		return -1;
	    }
	    if (bsize != wrcnt) {
		fprintf(stderr,
		    "Xlib: TCP/IP write failed (expected %d, wrote %d)\n",
			(int)bsize, wrcnt);
		errno = EPIPE;
		return -1;
	    }
	    buf += bsize;
	    count -= (int) bsize;
	    size += (int) bsize;
	}
    }

    /*
     * Virtual circuit connection
     */
    if (chandesc->type == ACDT_VIRTCIRC) {
	if ((size = vc_write(chandesc->virtcirc, buf, count)) < 0) {
	    fprintf(stderr, "Xlib: virtual circuit write failed\n");
	    errno = EPIPE;
	    return -1;
	}
    }
    return size;
}

/*
 * _XReadV - scatter/gather routine
 */
_XReadV(fdi, iov, n)
    int fdi, n;
    register struct iovec *iov;
{
    int count = 0, thiscount;

    while (n--) {
	if (iov->iov_len) {
	    thiscount = ReadFromServer(fdi, iov->iov_base, iov->iov_len);
	    if (thiscount < 0) return thiscount;
	    count += thiscount;
	    if (thiscount < iov->iov_len) break;
	}
	iov++;
    }
    return count;
}

/*
 * _XWriteV - scatter/gather routine
 */
int
_XWriteV(fdi, iov, n)
    int fdi, n;
    register struct iovec *iov;
{
    int count = 0, thiscount;

    while (n--) {
	if (iov->iov_len) {
	    thiscount = WriteToServer(fdi, iov->iov_base, iov->iov_len);
	    if (thiscount < 0)
		return thiscount;
	    count += thiscount;
	    if (thiscount < iov->iov_len) break;
	}
	iov++;
    }
    return count;
}
#endif /* AMOEBA */

#ifdef _MINIX
int MNX_BytesReadable (dpy, ptr)
Display *dpy;
int * ptr;
{
	unsigned rd_siz;
	int r;
	asio_fd_set_t bits;
	struct fwait fw;

	*ptr= 0;
	if (dpy->read_bufnxt > dpy->read_bufptr)
	{
		*ptr= dpy->read_bufnxt-dpy->read_bufptr;
		return 0;
	}
	if (!dpy->read_inprogress)
	{
		dpy->read_bufptr= dpy->read_bufnxt= dpy->read_buffer;
		rd_siz= dpy->read_bufmax-dpy->read_bufnxt;
		r= read(dpy->fd, dpy->read_bufnxt, rd_siz);
		if (r > 0)
		{
			dpy->read_bufnxt += r;
			*ptr= r;
			return 0;
		}
		else if (r == 0)
		{
			errno= EPIPE;
			return -1;
		}
		else if (errno != EINPROGRESS)
		{
			return r;
		}
		dpy->read_inprogress= True;
		return 0;
	}
	ASIO_FD_ZERO(&bits);
	ASIO_FD_SET(dpy->fd, ASIO_READ, &bits);
	fw.fw_flags= FWF_NONBLOCK;
	fw.fw_bits= bits.afds_bits;
	fw.fw_maxfd= ASIO_FD_SETSIZE;
	r= fwait(&fw);
	assert(r == 0 || r == -1 && errno == EAGAIN);
	if (r == -1)
	{
		return 0;
	}
	assert(fw.fw_fd == dpy->fd);
	assert(fw.fw_operation == ASIO_READ);
	dpy->read_inprogress= False;
	if (fw.fw_result == -1 || fw.fw_result == 0)
	{
		if (fw.fw_result == -1)
			errno= fw.fw_errno;
		else
			errno= EPIPE;
		return fw.fw_result;
	}
	dpy->read_bufnxt += fw.fw_result;
	*ptr= fw.fw_result;
	return 0;
}

int MNX_ReadFromServer(dpy, buf, siz)
Display *dpy;
char *buf;
unsigned siz;
{
	unsigned rd_siz;
	int r;
	struct asio_fd_set bits;
	struct fwait fw;

	assert(dpy->read_buffer <= dpy->read_bufptr &&
		dpy->read_bufptr <= dpy->read_bufnxt &&
		dpy->read_bufnxt <= dpy->read_bufmax &&
		dpy->read_buffer <= dpy->read_bufmax &&
		dpy->read_buffer != NULL);
	if (dpy->read_bufptr < dpy->read_bufnxt)
		goto gotbytes;
	if (!dpy->read_inprogress)
	{
		dpy->read_bufptr= dpy->read_bufnxt= dpy->read_buffer;
		rd_siz= dpy->read_bufmax-dpy->read_bufnxt;
		r= read(dpy->fd, dpy->read_bufnxt, rd_siz);
		if (r > 0)
		{
			dpy->read_bufnxt += r;
			goto gotbytes;
		}
		if (r == -1 && errno != EINPROGRESS || r == 0)
		{
			return r;
		}
		dpy->read_inprogress= True;
	}
	ASIO_FD_ZERO(&bits);
	ASIO_FD_SET(dpy->fd, ASIO_READ, &bits);
	fw.fw_flags= 0;
	fw.fw_bits= bits.afds_bits;
	fw.fw_maxfd= ASIO_FD_SETSIZE;
	r= fwait(&fw);
	assert(r == 0 || r == -1 && errno == EINTR);
	if (r == -1)
	{
		return -1;
	}
	assert(fw.fw_fd == dpy->fd);
	assert(fw.fw_operation == ASIO_READ);
	dpy->read_inprogress= False;
	if (fw.fw_result == -1 || fw.fw_result == 0)
	{
		if (fw.fw_result == -1)
			errno= fw.fw_errno;
		return fw.fw_result;
	}
	dpy->read_bufnxt += fw.fw_result;
gotbytes:
	if (siz > dpy->read_bufnxt-dpy->read_bufptr)
		siz= dpy->read_bufnxt-dpy->read_bufptr;
	memcpy(buf, dpy->read_bufptr, siz);
	dpy->read_bufptr += siz;
	return siz;
}

int MNX_ReadvFromServer(dpy, iov, iovcnt)
Display *dpy;
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
	    nbytes = MNX_ReadFromServer(dpy, base, len);
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

int MNX_WritevToServer(dpy, iov, iovcnt)
Display *dpy;
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
	    nbytes = MNX_WriteToServer(dpy, base, len);
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

int MNX_WriteToServer(dpy, buf, siz)
Display *dpy;
char *buf;
unsigned siz;
{
/* Writing is difficult. If we just issued a blocking write then
 * we can get deadlock if the server also decides to write.
 * The server doesn't do deadlock prevention so we have to buffer
 * incoming data.
 */
	int r, pend;
	xEvent *ev;
	asio_fd_set_t bits;
	struct fwait fw;

	/* Let's start with a write, maybe we are lucky. */
	r= write(dpy->fd, buf,siz);
	if (r >= 0 || errno != EINPROGRESS)
		return r;	/* We were lucky. */
	for(;;)
	{
		/* Try to get a read in progress. */
		if (!dpy->read_inprogress)
		{
			char readbuf[BUFSIZE];

			if (MNX_BytesReadable(dpy, &pend) < 0)
				_XIOError(dpy);
			if (dpy->read_inprogress)
				continue;
			if (pend < SIZEOF(xEvent)) pend = SIZEOF(xEvent);

			if (pend > BUFSIZE) pend = BUFSIZE;

			pend= (pend / SIZEOF(xEvent)) * SIZEOF(xEvent);

			_XRead(dpy, readbuf, pend);

			STARTITERATE (ev,xEvent, readbuf, (pend > 0),
					(pend -= SIZEOF(xEvent))) {
				if (ev->u.u.type == X_Error)
					_XError(dpy, (xError *) ev);
				else
					_XEnq( dpy, ev);
			}
			ENDITERATE
			continue;
		}
		/* Let's fwait. */
		ASIO_FD_ZERO(&bits);
		ASIO_FD_SET(dpy->fd, ASIO_READ, &bits);
		ASIO_FD_SET(dpy->fd, ASIO_WRITE, &bits);
		fw.fw_flags= 0;
		fw.fw_bits= bits.afds_bits;
		fw.fw_maxfd= ASIO_FD_SETSIZE;
		r= fwait(&fw);
		if (r == -1)
		{
			assert(errno == EINTR);
			continue;
		}
		assert(r == 0);
		assert(fw.fw_fd == dpy->fd);
		if (fw.fw_operation == ASIO_WRITE)
		{
			/* We are done. */
			r= fw.fw_result;
			if (r == -1)
				errno= fw.fw_errno;
			return r;
		}
		assert(fw.fw_operation == ASIO_READ);
		r= fw.fw_result;
		if (r == -1)
		{
			errno= fw.fw_errno;
			_XIOError(dpy);
		}
		if (r == 0)
		{
			errno= EPIPE;
			_XIOError(dpy);
		}
		dpy->read_bufptr += r;
		dpy->read_inprogress= False;
	}
}
			
int MNX_XConnectionNumber(dpy)
Display *dpy;
{
	if (!dpy->read_inprogress)
		return -1;
	return dpy->fd;
}

void XReplyReadStatus(dpy, r, err)
Display *dpy;
int r;
int err;
{
	assert(dpy->read_inprogress);
	dpy->read_inprogress= False;
	if (r > 0)
	{
		dpy->read_bufnxt += r;
		return;
	} 
	if (r == 0)
		errno= EPIPE;
	else
		errno= err;
	_XIOError(dpy);
}
#endif /* _MINIX */

/*
 * $XFree86: mit/lib/Xdmcp/Fill.c,v 2.0 1993/09/22 15:32:23 dawes Exp $
 * $XConsortium: Fill.c,v 1.4 91/07/16 20:33:50 gildea Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Keith Packard, MIT X Consortium
 */

#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xmd.h>
#include <X11/Xdmcp.h>

#if defined(TCPCONN) || defined(UNIXCONN) || defined(DNETCONN)
#define HASSOCKETS
#endif

#ifdef STREAMSCONN
#include <tiuser.h>
#else
#ifdef HASSOCKETS
#ifndef _MINIX
#include <sys/socket.h>
#else /* _MINIX */
#include <net/gen/udp.h>
#include <net/gen/udp_hdr.h>
#endif /* !_MINIX */
#endif
#endif

#ifndef _MINIX
int
XdmcpFill (fd, buffer, from, fromlen)
    int		    fd;
    XdmcpBufferPtr  buffer;
    XdmcpNetaddr    from;	/* return */
    int		    *fromlen;	/* return */
{
    BYTE    *newBuf;
#ifdef STREAMSCONN
    struct t_unitdata dataunit;
    int gotallflag, result;
#endif

    if (buffer->size < XDM_MAX_MSGLEN)
    {
	newBuf = (BYTE *) Xalloc (XDM_MAX_MSGLEN);
	if (newBuf)
	{
	    Xfree (buffer->data);
	    buffer->data = newBuf;
	    buffer->size = XDM_MAX_MSGLEN;
	}
    }
    buffer->pointer = 0;
#ifdef STREAMSCONN
    dataunit.addr.buf = from;
    dataunit.addr.maxlen = *fromlen;
    dataunit.opt.maxlen = 0;	/* don't care to know about options */
    dataunit.udata.buf = (char *)buffer->data;
    dataunit.udata.maxlen = buffer->size;
    result = t_rcvudata (fd, &dataunit, &gotallflag);
    if (result < 0) {
	return FALSE;
    }
    buffer->count = dataunit.udata.len;
    *fromlen = dataunit.addr.len;
#else
#ifdef HASSOCKETS
    buffer->count = recvfrom (fd, buffer->data, buffer->size, 0,
			      (struct sockaddr *)from, fromlen);
#else
    buffer->count = 0;
#endif /* HASSOCKETS */
#endif /* STREAMSCONN */
    if (buffer->count < 6) {
	buffer->count = 0;
	return FALSE;
    }
    return TRUE;
}
#else /* _MINIX */
int
MNX_XdmcpFill (fd, buffer, from, fromlen, data, datalen)
    int		    fd;
    XdmcpBufferPtr  buffer;
    XdmcpNetaddr    from;	/* return */
    int		    *fromlen;	/* return */
    char	    *data;
    int		    datalen;
{
    BYTE    *newBuf;
    struct sockaddr *from_addr;
    udp_io_hdr_t *udp_io_hdr;

    if (buffer->size < XDM_MAX_MSGLEN)
    {
	newBuf = (BYTE *) Xalloc (XDM_MAX_MSGLEN);
	if (newBuf)
	{
	    Xfree (buffer->data);
	    buffer->data = newBuf;
	    buffer->size = XDM_MAX_MSGLEN;
	}
    }
    buffer->pointer = 0;
    udp_io_hdr= (udp_io_hdr_t *)data;
    data += sizeof(udp_io_hdr_t) + udp_io_hdr->uih_ip_opt_len;
    datalen -= sizeof(udp_io_hdr_t) + udp_io_hdr->uih_ip_opt_len;
    buffer->count= udp_io_hdr->uih_data_len;
    if (buffer->count > datalen)
    {
    	buffer->count= 0;
    	return FALSE;
    }
    bcopy(data, (char *)buffer->data, buffer->count);
    from_addr= (struct sockaddr *)from;
    from_addr->sa_u.sa_family= AF_INET;
    from_addr->sa_u.sa_in.sin_addr= udp_io_hdr->uih_src_addr;
    from_addr->sa_u.sa_in.sin_port= udp_io_hdr->uih_src_port;
    if (buffer->count < 6) {
	buffer->count = 0;
	return FALSE;
    }
    return TRUE;
}
#endif /* !_MINIX */

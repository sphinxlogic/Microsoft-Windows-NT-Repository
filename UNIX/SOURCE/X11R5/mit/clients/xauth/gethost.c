/*
 * $XFree86: mit/clients/xauth/gethost.c,v 1.3 1993/05/05 13:42:28 dawes Exp $
 * $XConsortium: gethost.c,v 1.14 91/07/26 19:54:39 keith Exp $
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
 * Author:  Jim Fulton, MIT X Consortium
 */

/* sorry, streams support does not really work yet */
#if defined(STREAMSCONN) && defined(SVR4)
#undef STREAMSCONN
#define TCPCONN
#endif

#include "xauth.h"
#include <X11/X.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#ifndef __TYPES__
#include <sys/types.h>
#define __TYPES__
#endif
#if !defined(AMOEBA) && !defined(_MINIX)
#ifndef STREAMSCONN
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#ifdef SYSV386
#ifndef SVR4
#ifndef SCO
#include <net/errno.h>
#endif /* !SCO */
#endif /* !SVR4 */
#endif /* SYSV386 */
#endif /* !STREAMSCONN */
#else /* AMOEBA || _MINIX */
#ifdef AMOEBA
#include <server/ip/types.h>
#include <server/ip/gen/socket.h>
#include <server/ip/gen/netdb.h>
#include <server/ip/gen/in.h>
#endif /* AMOEBA */
#ifdef _MINIX
#include <net/gen/in.h>
#include <net/gen/netdb.h>
#include <net/gen/socket.h>
#endif /* _MINIX */
#endif /* AMOEBA || _MINIX */
#include <errno.h>
extern int errno;			/* for stupid errno.h files */
#ifdef DNETCONN
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#endif

Bool nameserver_timedout = False;


/*
 * get_hostname - Given an internet address, return a name (CHARON.MIT.EDU)
 * or a string representing the address (18.58.0.13) if the name cannot
 * be found.  Stolen from xhost.
 */

static jmp_buf env;
static 
#ifdef SIGNALRETURNSINT
int
#else
void
#endif
nameserver_lost(sig)
{
  nameserver_timedout = True;
  longjmp (env, -1);
  /* NOTREACHED */
#ifdef SIGNALRETURNSINT
  return -1;				/* for picky compilers */
#endif
}


char *get_hostname (auth)
    Xauth *auth;
{
#if defined(TCPCONN) || defined(AMTCPCONN)
    struct hostent *hp = NULL;
    char *inet_ntoa();
#endif
#ifdef DNETCONN
    struct nodeent *np;
    static char nodeaddr[16];
#endif /* DNETCONN */

    if (auth->address_length == 0)
	return "Illegal Address";
#if defined(TCPCONN) || defined(AMTCPCONN)
    if (auth->family == FamilyInternet) {
	/* gethostbyaddr can take a LONG time if the host does not exist.
	   Assume that if it does not respond in NAMESERVER_TIMEOUT seconds
	   that something is wrong and do not make the user wait.
	   gethostbyaddr will continue after a signal, so we have to
	   jump out of it. 
	   */
	nameserver_timedout = False;
	signal (SIGALRM, nameserver_lost);
	alarm (4);
	if (setjmp(env) == 0) {
	    hp = gethostbyaddr (auth->address, auth->address_length, AF_INET);
	}
	alarm (0);
	if (hp)
	  return (hp->h_name);
	else
#if !defined(AMOEBA) && !defined(_MINIX)
	  return (inet_ntoa(*((struct in_addr *)(auth->address))));
#else
	  return (inet_ntoa(*((ipaddr_t *)(auth->address))));
#endif
    }
#endif
#ifdef DNETCONN
    if (auth->family == FamilyDECnet) {
	if (np = getnodebyaddr(auth->address, auth->address_length,
			       AF_DECnet)) {
	    sprintf(nodeaddr, "%s:", np->n_name);
	} else {
	    sprintf(nodeaddr, "%s:", dnet_htoa(auth->address));
	}
	return(nodeaddr);
    }
#endif

    return (NULL);
}

#if defined(TCPCONN) || defined(AMTCPCONN)
/*
 * cribbed from lib/X/XConnDis.c
 */
static Bool get_inet_address (name, resultp)
    char *name;
    unsigned long *resultp;		/* return */
{
    unsigned long hostinetaddr = inet_addr (name);
    struct hostent *host_ptr;
#if defined(TCPCONN) && !defined(_MINIX)
    struct sockaddr_in inaddr;		/* dummy variable for size calcs */
#endif

    if (hostinetaddr == -1) {		/* oh, gross.... */
	if ((host_ptr = gethostbyname (name)) == NULL) {
	    /* No such host! */
	    errno = EINVAL;
	    return False;
	}
	/* Check the address type for an internet host. */
	if (host_ptr->h_addrtype != AF_INET) {
	    /* Not an Internet host! */
#ifdef EPROTOTYPE
	    errno = EPROTOTYPE;
#else
	    errno = EINVAL;
#endif
	    return False;
	}
 
#if !defined(AMOEBA) && !defined(_MINIX)
	bcopy((char *)host_ptr->h_addr, (char *)&hostinetaddr,
	      sizeof(inaddr.sin_addr));
#else
	bcopy((char *)host_ptr->h_addr, (char *)&hostinetaddr,
	      sizeof(ipaddr_t));
#endif

    }
    *resultp = hostinetaddr;
    return True;
}
#endif

#ifdef DNETCONN
static Bool get_dnet_address (name, resultp)
    char *name;
    struct dn_naddr *resultp;
{
    struct dn_naddr *dnaddrp, dnaddr;
    struct nodeent *np;

    if (dnaddrp = dnet_addr (name)) {	/* stolen from xhost */
	dnaddr = *dnaddrp;
    } else {
	if ((np = getnodebyname (name)) == NULL) return False;
	dnaddr.a_len = np->n_length;
	bcopy (np->n_addr, dnaddr.a_addr, np->n_length);
    }
    *resultp = dnaddr;
    return True;
}
#endif

char *get_address_info (family, fulldpyname, prefix, host, lenp)
    int family;
    char *fulldpyname;
    int prefix;
    char *host;
    int *lenp;
{
    char *retval = NULL;
    int len = 0;
    char *src = NULL;
#if defined(TCPCONN) || defined(AMTCPCONN)
    unsigned long hostinetaddr;
#endif
#if defined(TCPCONN) && !defined(_MINIX)
    struct sockaddr_in inaddr;		/* dummy variable for size calcs */
#endif
#ifdef DNETCONN
    struct dn_naddr dnaddr;
#endif
    char buf[255];

    /*
     * based on the family, set the pointer src to the start of the address
     * information to be copied and set len to the number of bytes.
     */
    switch (family) {
      case FamilyLocal:			/* hostname/unix:0 */
					/* handle unix:0 and :0 specially */
	if (prefix == 0 && (strncmp (fulldpyname, "unix:", 5) == 0 ||
			    fulldpyname[0] == ':')) {
	    extern char *get_local_hostname();

	    if (!get_local_hostname (buf, sizeof buf)) {
		len = 0;
	    } else {
		src = buf;
		len = strlen (buf);
	    }
	} else {
	    src = fulldpyname;
	    len = prefix;
	}
	break;
      case FamilyInternet:		/* host:0 */
#if defined(TCPCONN) || defined(AMTCPCONN)
	if (!get_inet_address (host, &hostinetaddr)) return NULL;
	src = (char *) &hostinetaddr;
	len = 4; /* sizeof inaddr.sin_addr, would fail on Cray */
	break;
#else
	return NULL;
#endif
      case FamilyDECnet:		/* host::0 */
#ifdef DNETCONN
	if (!get_dnet_address (host, &dnaddr)) return NULL;
	src = (char *) &dnaddr;
	len = (sizeof dnaddr);
	break;
#else
	/* fall through since we don't have code for it */
#endif
      default:
	src = NULL;
	len = 0;
    }

    /*
     * if source was provided, allocate space and copy it
     */
    if (len == 0 || !src) return NULL;

    retval = malloc (len);
    if (retval) {
	bcopy (src, retval, len);
	*lenp = len;
    }
    return retval;
}

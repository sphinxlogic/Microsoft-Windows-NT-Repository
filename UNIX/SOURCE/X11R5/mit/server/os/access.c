/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $XFree86: mit/server/os/access.c,v 2.6 1993/09/22 15:49:42 dawes Exp $ */
/* $XConsortium: access.c,v 1.54 92/05/19 17:23:02 keith Exp $ */

#include "Xos.h"
#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "site.h"
#include <errno.h>
#ifndef AMOEBA
#ifndef _MINIX
#ifdef ESIX
#include <lan/socket.h>
#else
#include <sys/socket.h>
#endif
#include <sys/ioctl.h>
#endif
#else /* AMOEBA */
#include <amoeba.h>
#include <cmdreg.h>
#include <stdcom.h>
#include <stderr.h>
#include <ampolicy.h>
#include <server/ip/hton.h>
#include <server/ip/types.h>
#include <server/ip/tcpip.h>
#include <server/ip/tcp_io.h>
#include <server/ip/gen/in.h>
#include <server/ip/gen/tcp.h>
#include <server/ip/gen/tcp_io.h>
#include <server/ip/gen/socket.h>
#endif /* AMOEBA */
#include <X11/Xauth.h>

#if defined(TCPCONN) || defined(ISC)
#ifndef _MINIX
#include <netinet/in.h>
#else
#include <sys/ioctl.h>
#include <net/gen/in.h>
#include <net/gen/netdb.h>
#include <net/gen/tcp.h>
#include <net/gen/tcp_io.h>
#endif /* _MINIX */
#endif /* TCPCONN || ISC */
#ifdef DNETCONN
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#endif

#if defined(SVR4) || defined(ISC) || defined(linux)
#define USE_FALLBACK_DEFINESELF
#endif

#ifdef hpux
# include <sys/utsname.h>
# ifdef HAS_IFREQ
#  include <net/if.h>
# endif
#else
#if defined(SYSV386) || defined(USE_FALLBACK_DEFINESELF)
# include <sys/utsname.h>
#endif
#if defined(SYSV) && defined(SYSV386)
# include <sys/stream.h>
# ifdef ISC
#  include <sys/stropts.h>
#  include <sys/sioctl.h>
# endif /* ISC */
#endif
#ifdef ESIX
# include <lan/if.h>
#else
#if !defined(AMOEBA) && !defined(_MINIX)
# include <net/if.h>
#endif
#endif
#endif /* hpux */

#ifdef SVR4
#include <sys/sockio.h>
#include <stropts.h>
#endif

#ifdef ESIX
#include <lan/netdb.h>
#else
#ifdef AMOEBA
#include <server/ip/gen/netdb.h>
#else
#ifndef _MINIX
#include <netdb.h>
#endif
#endif /* AMOEBA */
#endif /* ESIX */
#undef NULL
#include <stdio.h>
#include "dixstruct.h"
#include "osdep.h"

Bool defeatAccessControl = FALSE;

#define acmp(a1, a2, len) bcmp((char *)(a1), (char *)(a2), len)
#define acopy(a1, a2, len) bcopy((char *)(a1), (char *)(a2), len)
#define addrEqual(fam, address, length, host) \
			 ((fam) == (host)->family &&\
			  (length) == (host)->len &&\
			  !acmp (address, (host)->addr, length))

#ifdef hpux
#define getpeername(fd, from, fromlen)	hpux_getpeername(fd, from, fromlen)
#endif
#if defined(SYSV386) && defined(SYSV)
#define getpeername(fd, from, fromlen)	sysv386_getpeername(fd, from, fromlen)
#endif

#if defined(SVR4) || defined(ISC)
/* ifioctl() for SVR4 from Ian Donaldson <iand@labtam.labtam.oz.au> */
static int
ifioctl (fd, cmd, arg)
    int fd;
    int cmd;
    char *arg;
{
    struct strioctl ioc;
    int ret;

    bzero((char *) &ioc, sizeof(ioc));
    ioc.ic_cmd = cmd;
    ioc.ic_timout = 0;
    if (cmd == SIOCGIFCONF)
    {
	ioc.ic_len = ((struct ifconf *) arg)->ifc_len;
	ioc.ic_dp = ((struct ifconf *) arg)->ifc_buf;
#ifdef ISC
	/* SIOCGIFCONF is somewhat brain damaged on ISC. The argument
	 * buffer must contain the ifconf structure as header. Ifc_req
	 * is also not a pointer but a one element array of ifreq
	 * structures. On return this array is extended by enough
	 * ifreq fields to hold all interfaces. The return buffer length
	 * is placed in the buffer header.
	 */
        ((struct ifconf *) ioc.ic_dp)->ifc_len =
                                         ioc.ic_len - sizeof(struct ifconf);
#endif
    }
    else
    {
	ioc.ic_len = sizeof(struct ifreq);
	ioc.ic_dp = arg;
    }
    ret = ioctl(fd, I_STR, (char *) &ioc);
    if (ret >= 0 && cmd == SIOCGIFCONF)
#ifdef SVR4
	((struct ifconf *) arg)->ifc_len = ioc.ic_len;
#endif
#ifdef ISC
    {
	((struct ifconf *) arg)->ifc_len =
				 ((struct ifconf *)ioc.ic_dp)->ifc_len;
	((struct ifconf *) arg)->ifc_buf = 
			(caddr_t)((struct ifconf *)ioc.ic_dp)->ifc_req;
    }
#endif
    return(ret);
}
#else /* SVR4 || ISC */
#define ifioctl ioctl
#endif /* SVR4 || ISC */

static int ConvertAddr(), CheckAddr();
static Bool NewHost();

typedef struct _host {
	short		family;
	short		len;
	unsigned char	*addr;
	struct _host *next;
} HOST;

#define MakeHost(h,l)	(h)=(HOST *) xalloc(sizeof *(h)+(l));\
			(h)->addr=(unsigned char *) ((h) + 1);
#define FreeHost(h)	xfree(h)
static HOST *selfhosts = NULL;
static HOST *validhosts = NULL;
static int AccessEnabled = DEFAULT_ACCESS_CONTROL;
static int LocalHostEnabled = FALSE;
static int UsingXdmcp = FALSE;

/*
 * called when authorization is not enabled to add the
 * local host to the access list
 */

EnableLocalHost ()
{
    if (!UsingXdmcp)
    {
	LocalHostEnabled = TRUE;
	AddLocalHosts ();
    }
}

/*
 * called at init time when XDMCP will be used; xdmcp always
 * adds local hosts manually when needed
 */

AccessUsingXdmcp ()
{
    UsingXdmcp = TRUE;
    LocalHostEnabled = FALSE;
}

/*
 * Amoeba doesn't need DefineSelf, since all TCP/IP connections made to
 * the server always originate from remote clients.
 */
#ifndef AMOEBA
#if defined (SYSV386) || defined (linux) || (defined (hpux) && ! defined (HAS_IFREQ))
/* Define this host for access control.  Find all the hosts the OS knows about 
 * for this fd and add them to the selfhosts list.
 * hpux, and some SYSV386 do not have SIOCGIFCONF ioctl;
 */
/* SVR4, ISC, linux use this if SIOCGIFCONF fails */
#ifdef USE_FALLBACK_DEFINESELF
static
FallbackDefineSelf(fd)
#else
DefineSelf (fd)
#endif
    int fd;
{
#if !defined(TCPCONN) && !defined(UNIXCONN)
    return -1;
#else
    register int n;
    int	len;
    caddr_t	addr;
    int		family;
    register HOST	*host;

    struct utsname name;
    register struct hostent  *hp;

    union {
	struct  sockaddr   sa;
	struct  sockaddr_in  in;
    } saddr;
	
    struct	sockaddr_in	*inetaddr;

    /* Why not use gethostname()?  Well, at least on my system, I've had to
     * make an ugly kernel patch to get a name longer than 8 characters, and
     * uname() lets me access to the whole string (it smashes release, you
     * see), whereas gethostname() kindly truncates it for me.
     */
    uname(&name);
    hp = gethostbyname (name.nodename);
    if (hp != NULL)
    {
	saddr.sa.sa_family = hp->h_addrtype;
	inetaddr = (struct sockaddr_in *) (&(saddr.sa));
	acopy ( hp->h_addr, &(inetaddr->sin_addr), hp->h_length);
	len = sizeof(saddr.sa);
	family = ConvertAddr ( &(saddr.sa), &len, &addr);
	if ( family != -1 && family != FamilyLocal )
	{
	    for (host = selfhosts;
		 host && !addrEqual (family, addr, len, host);
		 host = host->next) ;
	    if (!host)
	    {
		/* add this host to the host list.	*/
		MakeHost(host,len)
		if (host)
		{
		    host->family = family;
		    host->len = len;
		    acopy ( addr, host->addr, len);
		    host->next = selfhosts;
		    selfhosts = host;
		}
	    }
#ifdef XDMCP
	    /*
	     * If this isn't an Internet Address, don't register it.
	     */
	    if (family == FamilyInternet)
	    {
	      /*
 	       * ignore 'localhost' entries as they're not useful
	       * on the other end of the wire
	       */
	      if (len != 4 ||
		  addr[0] != 127 || addr[1] != 0 ||
		  addr[2] != 0 || addr[3] != 1)
	      {
		    struct sockaddr_in broad_addr;

		    XdmcpRegisterConnection (family, (char *)addr, len);
		    broad_addr = *inetaddr; /* is this our main addr? */
		    broad_addr.sin_addr.s_addr = htonl (INADDR_BROADCAST);
		    XdmcpRegisterBroadcastAddress (&broad_addr);
	      }
	    }
#endif
	}
    }
#endif /* !TCPCONN && !UNIXCONN */
}

#endif
#if (!defined(SYSV386) && !(defined (hpux) && ! defined (HAS_IFREQ))) || defined(USE_FALLBACK_DEFINESELF)
#ifndef _MINIX

#ifdef AF_LINK
#define ifr_size(p) ((p).sa_len > sizeof(p) ? (p).sa_len : sizeof(p))
#else
#define ifr_size(p) (sizeof(p))
#endif

/* Define this host for access control.  Find all the hosts the OS knows about 
 * for this fd and add them to the selfhosts list.
 */
DefineSelf (fd)
    int fd;
{
    char		buf[2048], *cp, *cplim;
    struct ifconf	ifc;
    register int	n;
    int 		len;
    pointer 		addr;
    int 		family;
    register HOST 	*host;
    register struct ifreq *ifr;
    
#ifdef DNETCONN
    struct dn_naddr *dnaddr = getnodeadd(0);
    /*
     * AF_DECnet may not be listed in the interface list.  Instead use
     * the supported library call to find out the local address (if any).
     */
    if (dnaddr)
    {    
	addr = (pointer) dnaddr;
	len = dnaddr->a_len + sizeof(dnaddr->a_len);
	family = AF_DECnet;
	for (host = selfhosts;
	     host && !addrEqual (family, addr, len, host);
	     host = host->next)
	    ;
        if (!host)
	{
	    MakeHost(host,len)
	    if (host)
	    {
		host->family = family;
		host->len = len;
		acopy(addr, host->addr, len);
		host->next = selfhosts;
		selfhosts = host;
	    }
	}
    }
#endif
    ifc.ifc_len = sizeof (buf);
    ifc.ifc_buf = buf;
    if (ifioctl (fd, (int) SIOCGIFCONF, (pointer) &ifc) < 0)
#ifdef USE_FALLBACK_DEFINESELF
	return FallbackDefineSelf();
#else
        Error ("Getting interface configuration");
#endif
#ifndef ISC
#define IFC_IFC_REQ ifc.ifc_req
#else
#define IFC_IFC_REQ (struct ifreq *)ifc.ifc_buf
#endif
    cplim = (char *)IFC_IFC_REQ + ifc.ifc_len;
    for (cp = (char *)IFC_IFC_REQ; cp < cplim;
	 cp += sizeof (ifr->ifr_name) + ifr_size(ifr->ifr_addr))
    {
	ifr = (struct ifreq*)cp;
	len = ifr_size(ifr->ifr_addr);
#ifdef DNETCONN
	/*
	 * DECnet was handled up above.
	 */
	if (ifr->ifr_addr.sa_family == AF_DECnet)
	    continue;
#endif /* DNETCONN */
	family = ConvertAddr (&ifr->ifr_addr, &len, &addr);
        if (family == -1 || family == FamilyLocal)
	    continue;
        for (host = selfhosts;
 	     host && !addrEqual (family, addr, len, host);
	     host = host->next)
	    ;
        if (host)
	    continue;
	MakeHost(host,len)
	if (host)
	{
	    host->family = family;
	    host->len = len;
	    acopy(addr, host->addr, len);
	    host->next = selfhosts;
	    selfhosts = host;
	}
#ifdef XDMCP
	{
	    struct sockaddr broad_addr;

	    /*
	     * If this isn't an Internet Address, don't register it.
	     */
	    if (family != FamilyInternet)
		continue;

	    /*
 	     * ignore 'localhost' entries as they're not useful
	     * on the other end of the wire
	     */
	    if (len == 4 &&
		addr[0] == 127 && addr[1] == 0 &&
		addr[2] == 0 && addr[3] == 1)
		continue;

	    XdmcpRegisterConnection (family, (char *)addr, len);
	    broad_addr = ifr->ifr_addr;
	    ((struct sockaddr_in *) &broad_addr)->sin_addr.s_addr =
		htonl (INADDR_BROADCAST);
#ifdef SIOCGIFBRDADDR
	    {
	    	struct ifreq    broad_req;
    
	    	broad_req = *ifr;
		if (ifioctl (fd, SIOCGIFFLAGS, (char *) &broad_req) != -1 &&
		    (broad_req.ifr_flags & IFF_BROADCAST) &&
		    (broad_req.ifr_flags & IFF_UP)
		    )
		{
		    broad_req = *ifr;
		    if (ifioctl (fd, SIOCGIFBRDADDR, &broad_req) != -1)
			broad_addr = broad_req.ifr_addr;
		    else
			continue;
		}
		else
		    continue;
	    }
#endif
	    XdmcpRegisterBroadcastAddress ((struct sockaddr_in *) &broad_addr);
	}
#endif
    }
}
#else  /* _MINIX */
/* Define this host for access control.
 */
DefineSelf (fd)
    int fd;
{
    int 		len;
    int 		r;
    pointer 		addr;
    nwio_tcpconf_t	tcpconf;
    HOST		*host;

    /* Assume we only have tcp connections. */
    r= ioctl(fd, NWIOGTCPCONF, &tcpconf);
    if (r == -1)
    {
	Error("Unable to get local address from tcp fd");
	return;
    }
    addr= (pointer)&tcpconf.nwtc_locaddr;
    len= sizeof(tcpconf.nwtc_locaddr);
    for (host = selfhosts; host && !addrEqual (0, addr, len, host);
							    host = host->next)
    {
	/* Do nothing */ ;
    }
    if (host)
	    return;
    MakeHost(host,len)
    if (host)
    {
	host->family = 0;
	host->len = len;
	acopy(addr, host->addr, len);
	host->next = selfhosts;
	selfhosts = host;
    }
#ifdef XDMCP
    {
	ipaddr_t broad_addr;

	XdmcpRegisterConnection (FamilyInternet, (char *)addr, len);
	broad_addr= htonl(0xffffffff);
	MNX_XdmcpRegisterBroadcastAddress (broad_addr);
    }
#endif
}
#endif /* _MINIX */
#endif /* hpux && !HAS_IFREQ */
#endif /* AMOEBA */

#ifdef XDMCP
void
AugmentSelf(from, len)
    struct sockaddr *from;
    int		    len;
{
    int family;
    pointer addr;
    register HOST *host;

    family = ConvertAddr(from, &len, &addr);
    if (family == -1 || family == FamilyLocal)
	return;
    for (host = selfhosts; host; host = host->next)
    {
	if (addrEqual(family, addr, len, host))
	    return;
    }
    MakeHost(host,len)
    if (!host)
	return;
    host->family = family;
    host->len = len;
    acopy(addr, host->addr, len);
    host->next = selfhosts;
    selfhosts = host;
}
#endif

AddLocalHosts ()
{
    HOST    *self;

    for (self = selfhosts; self; self = self->next)
	(void) NewHost (self->family, self->addr, self->len);
}

/* Reset access control list to initial hosts */
ResetHosts (display)
    char *display;
{
    register HOST	*host;
    char 		hostname[120];
    char		fname[32];
    FILE		*fd;
    char		*ptr;
#ifndef AMOEBA
    union {
        struct sockaddr	sa;
#if defined(TCPCONN) && !defined(_MINIX)
        struct sockaddr_in in;
#endif /* TCPCONN && !_MINIX */
#ifdef DNETCONN
        struct sockaddr_dn dn;
#endif
    } 			saddr;
#ifdef DNETCONN
    struct nodeent 	*np;
    struct dn_naddr 	dnaddr, *dnaddrp, *dnet_addr();
#endif
#endif /* AMOEBA */
    int			family;
    int			len;
    pointer		addr;
    register struct hostent *hp;

    AccessEnabled = defeatAccessControl ? FALSE : DEFAULT_ACCESS_CONTROL;
    LocalHostEnabled = FALSE;
    while (host = validhosts)
    {
        validhosts = host->next;
        FreeHost (host);
    }
    strcpy (fname, "/etc/X");
    strcat (fname, display);
    strcat (fname, ".hosts");
    if (fd = fopen (fname, "r")) 
    {
        while (fgets (hostname, sizeof (hostname), fd))
	{
    	if (ptr = index (hostname, '\n'))
    	    *ptr = 0;
#ifdef DNETCONN
    	if ((ptr = index (hostname, ':')) && (*(ptr + 1) == ':'))
	{
    	    /* node name (DECnet names end in "::") */
    	    *ptr = 0;
	    dnaddrp = dnet_addr(hostname);
    	    if (!dnaddrp && (np = getnodebyname (hostname)))
	    {
		/* node was specified by name */
		saddr.sa.sa_family = np->n_addrtype;
		len = sizeof(saddr.sa);
		if (ConvertAddr (&saddr.sa, &len, &addr) == FamilyDECnet)
		{
		    bzero ((char *) &dnaddr, sizeof (dnaddr));
		    dnaddr.a_len = np->n_length;
		    acopy (np->n_addr, dnaddr.a_addr, np->n_length);
		    dnaddrp = &dnaddr;
		}
    	    }
	    if (dnaddrp)
		(void) NewHost((short)FamilyDECnet, (pointer)dnaddrp,
			(int)(dnaddrp->a_len + sizeof(dnaddrp->a_len)));
    	}
	else
#endif /* DNETCONN */
#ifdef SECURE_RPC
	if (index (hostname, '@'))
	{
	    SecureRPCInit ();
	    (void) NewHost (FamilyNetname, hostname, strlen (hostname));
	}
	else
#endif /* SECURE_RPC */
#if defined(TCPCONN) || defined(AMTCPCONN)
	{
    	    /* host name */
    	    if (hp = gethostbyname (hostname))
	    {
#ifndef AMOEBA
#ifndef _MINIX
    		saddr.sa.sa_family = hp->h_addrtype;
#else /* _MINIX */
    		saddr.sa.sa_u.sa_family = hp->h_addrtype;
#endif /* !_MINIX */
		len = sizeof(saddr.sa);
    		if ((family = ConvertAddr (&saddr.sa, &len, &addr)) != -1)
#else
		len = sizeof(ipaddr_t);
		if (hp->h_addrtype == AF_INET)
#endif
		{
#ifdef h_addr				/* new 4.3bsd version of gethostent */
		    char **list;

		    /* iterate over the addresses */
		    for (list = hp->h_addr_list; *list; list++)
			(void) NewHost (family, (pointer)*list, len);
#else
    		    (void) NewHost (family, (pointer)hp->h_addr, len);
#endif
		}
    	    }
    	}	
#endif /* TCPCONN || AMTCPCONN */
        }
        fclose (fd);
    }
}

static Bool
AuthorizedClient(client)
    ClientPtr client;
{
    int    		alen, family;
#ifndef AMOEBA
    struct sockaddr	from;
#else
    ipaddr_t          from;
#endif
#if defined(_MINIX) || defined(AMOEBA)
    nwio_tcpconf_t    tcpconf;
#endif
    pointer		addr;
    register HOST	*host;

    if (!client || defeatAccessControl)
	return TRUE;
#ifndef AMOEBA
#ifndef _MINIX
    alen = sizeof (from);
    if (!getpeername (((OsCommPtr)client->osPrivate)->fd, &from, &alen))
    {
#else /* _MINIX */
    if (ioctl(((OsCommPtr)client->osPrivate)->fd, NWIOGTCPCONF,
    							&tcpconf) != -1)
    {
    	from.sa_u.sa_in.sin_family= AF_INET;
    	from.sa_u.sa_in.sin_addr= tcpconf.nwtc_remaddr;
    	from.sa_u.sa_in.sin_port= tcpconf.nwtc_remport;
    	alen= sizeof(from);
#endif /* !_MINIX */
#else /* AMOEBA */
    /*
     * Amoeba clients are always authorized.
     * We might deal with access rights though (XXX)
     */
    if (((OsCommPtr)client->osPrivate)->family == FamilyAmoeba)
      return TRUE;
    if (((OsCommPtr)client->osPrivate)->family == FamilyInternet &&
      tcp_ioc_getconf(&((OsCommPtr)client->osPrivate)->conn.tcp.cap, &tcpconf) == STD_OK)
    {
      alen = sizeof (ipaddr_t);
      from = tcpconf.nwtc_remaddr;
#endif /* AMOEBA */
	family = ConvertAddr (&from, &alen, &addr);
	if (family == -1)
	    return FALSE;
	if (family == FamilyLocal)
	    return TRUE;
	for (host = selfhosts; host; host = host->next)
	{
	    if (addrEqual (family, addr, alen, host))
		return TRUE;
	}
    }
    return FALSE;
}

/* Add a host to the access control list.  This is the external interface
 * called from the dispatcher */

int
AddHost (client, family, length, pAddr)
    ClientPtr		client;
    int                 family;
    unsigned            length;        /* of bytes in pAddr */
    pointer             pAddr;
{
    int			len;
    register HOST	*host;
    int                 unixFamily;

    if (!AuthorizedClient(client))
	return(BadAccess);
    switch (family) {
#ifdef SECURE_RPC
    case FamilyNetname:
	len = length;
	SecureRPCInit ();
	break;
#endif
    case FamilyInternet:
    case FamilyDECnet:
    case FamilyChaos:
	if ((len = CheckAddr (family, pAddr, length)) < 0)
	{
	    client->errorValue = length;
	    return (BadValue);
	}
	break;
    case FamilyLocal:
    default:
	client->errorValue = family;
	return (BadValue);
    }
    if (NewHost (family, pAddr, len))
	return Success;
    return BadAlloc;
}

Bool
ForEachHostInFamily (family, func, closure)
    int	    family;
    Bool    (*func)();
    pointer closure;
{
    HOST    *host;

    for (host = validhosts; host; host = host->next)
	if (family == host->family && func (host->addr, host->len, closure))
	    return TRUE;
    return FALSE;
}

/* Add a host to the access control list. This is the internal interface 
 * called when starting or resetting the server */
static Bool
NewHost (family, addr, len)
    short	family;
    pointer	addr;
    int		len;
{
    register HOST *host;

    for (host = validhosts; host; host = host->next)
    {
        if (addrEqual (family, addr, len, host))
	    return TRUE;
    }
    MakeHost(host,len)
    if (!host)
	return FALSE;
    host->family = family;
    host->len = len;
    acopy(addr, host->addr, len);
    host->next = validhosts;
    validhosts = host;
    return TRUE;
}

/* Remove a host from the access control list */

int
RemoveHost (client, family, length, pAddr)
    ClientPtr		client;
    int                 family;
    unsigned            length;        /* of bytes in pAddr */
    pointer             pAddr;
{
    int			len;
    register HOST	*host, **prev;

    if (!AuthorizedClient(client))
	return(BadAccess);
    switch (family) {
#ifdef SECURE_RPC
    case FamilyNetname:
	len = length;
	break;
#endif
    case FamilyInternet:
    case FamilyDECnet:
    case FamilyChaos:
    	if ((len = CheckAddr (family, pAddr, length)) < 0)
    	{
	    client->errorValue = length;
            return(BadValue);
    	}
	break;
    case FamilyLocal:
    default:
	client->errorValue = family;
        return(BadValue);
    }
    for (prev = &validhosts;
         (host = *prev) && (!addrEqual (family, pAddr, len, host));
         prev = &host->next)
        ;
    if (host)
    {
        *prev = host->next;
        FreeHost (host);
    }
    return (Success);
}

/* Get all hosts in the access control list */
int
GetHosts (data, pnHosts, pLen, pEnabled)
    pointer		*data;
    int			*pnHosts;
    int			*pLen;
    BOOL		*pEnabled;
{
    int			len;
    register int 	n = 0;
    register pointer	ptr;
    register HOST	*host;
    int			nHosts = 0;
    int			*lengths = (int *) NULL;
    int			*newlens;

    *pEnabled = AccessEnabled ? EnableAccess : DisableAccess;
    for (host = validhosts; host; host = host->next)
    {
	newlens = (int *) xrealloc(lengths, (nHosts + 1) * sizeof(int));
	if (!newlens)
	{
	    xfree(lengths);
	    return(BadAlloc);
	}
	lengths = newlens;
	lengths[nHosts++] = host->len;
	n += (((host->len + 3) >> 2) << 2) + sizeof(xHostEntry);
    }
    if (n)
    {
        *data = ptr = (pointer) xalloc (n);
	if (!ptr)
	{
	    xfree(lengths);
	    return(BadAlloc);
	}
	nHosts = 0;
        for (host = validhosts; host; host = host->next)
	{

	    len = lengths[nHosts++];
	    ((xHostEntry *)ptr)->family = host->family;
	    ((xHostEntry *)ptr)->length = len;
	    ptr += sizeof(xHostEntry);
	    acopy (host->addr, ptr, len);
	    ptr += ((len + 3) >> 2) << 2;
        }
    } else {
	*data = NULL;
    }
    *pnHosts = nHosts;
    *pLen = n;
    xfree(lengths);
    return(Success);
}

/* Check for valid address family and length, and return address length. */

/*ARGSUSED*/
static int
CheckAddr (family, pAddr, length)
    int			family;
    pointer		pAddr;
    unsigned		length;
{
    int	len;

    switch (family)
    {
#ifndef AMOEBA
#ifdef TCPCONN
      case FamilyInternet:
#ifndef _MINIX
	if (length == sizeof (struct in_addr))
#else
	if (length == sizeof (ipaddr_t))
#endif
	    len = length;
	else
	    len = -1;
        break;
#endif 
#else /* AMOEBA */
#ifdef AMTCPCONN
      case FamilyInternet:
      if (length == sizeof (ipaddr_t))
          len = length;
      else
          len = -1;
        break;
#endif /* AMTCPCONN */
#endif /* AMOEBA */
#ifdef DNETCONN
      case FamilyDECnet:
        {
	    struct dn_naddr *dnaddr = (struct dn_naddr *) pAddr;

	    if ((length < sizeof(dnaddr->a_len)) ||
		(length < dnaddr->a_len + sizeof(dnaddr->a_len)))
		len = -1;
	    else
		len = dnaddr->a_len + sizeof(dnaddr->a_len);
	    if (len > sizeof(struct dn_naddr))
		len = -1;
	}
        break;
#endif
      default:
        len = -1;
    }
    return (len);
}

/* Check if a host is not in the access control list. 
 * Returns 1 if host is invalid, 0 if we've found it. */

InvalidHost (saddr, len)
#ifdef AMOEBA
    register ipaddr_t         *saddr;
#else
    register struct sockaddr	*saddr;
#endif
    int				len;
{
    int 			family;
    pointer			addr;
    register HOST 		*selfhost, *host;

    if (!AccessEnabled)   /* just let them in */
        return(0);    
    family = ConvertAddr (saddr, &len, &addr);
    if (family == -1)
        return 1;
    if (family == FamilyLocal)
    {
	if (!LocalHostEnabled)
 	{
	    /*
	     * check to see if any local address is enabled.  This 
	     * implicitly enables local connections.
	     */
	    for (selfhost = selfhosts; selfhost; selfhost=selfhost->next)
 	    {
		for (host = validhosts; host; host=host->next)
		{
		    if (addrEqual (selfhost->family, selfhost->addr,
				   selfhost->len, host))
			return 0;
		}
	    }
	    return 1;
	} else
	    return 0;
    }
    for (host = validhosts; host; host = host->next)
    {
        if (addrEqual (family, addr, len, host))
    	    return (0);
    }
    return (1);
}

static int
ConvertAddr (saddr, len, addr)
#ifdef AMOEBA
    register ipaddr_t         *saddr;
#else
    register struct sockaddr	*saddr;
#endif
    int				*len;
    pointer			*addr;
{
#ifndef AMOEBA
    if (*len == 0)
        return (FamilyLocal);
#ifndef _MINIX
    switch (saddr->sa_family)
#else /* _MINIX */
    switch (saddr->sa_u.sa_family)
#endif /* !_MINIX */
    {
    case AF_UNSPEC:
#ifdef UNIXCONN
    case AF_UNIX:
#endif
        return FamilyLocal;
#ifdef TCPCONN
    case AF_INET:
#ifndef _MINIX
        *len = sizeof (struct in_addr);
        *addr = (pointer) &(((struct sockaddr_in *) saddr)->sin_addr);
#else /* _MINIX */
        *len = sizeof (ipaddr_t);
        *addr = (pointer) &(saddr->sa_u.sa_in.sin_addr);
#endif /* !_MINIX */
        return FamilyInternet;
#endif
#ifdef DNETCONN
    case AF_DECnet:
	{
	    struct sockaddr_dn *sdn = (struct sockaddr_dn *) saddr;
	    *len = sdn->sdn_nodeaddrl + sizeof(sdn->sdn_nodeaddrl);
	    *addr = (pointer) &(sdn->sdn_add);
	}
        return FamilyDECnet;
#endif
#ifdef CHAOSCONN
    case AF_CHAOS:
	{
	    not implemented
	}
	return FamilyChaos;
#endif
    default:
        return -1;
    }
#else /* AMOEBA */
    if (*len == 0) return -1;
    *len = sizeof (ipaddr_t);
    *addr = (pointer) saddr;
    return FamilyInternet;
#endif /* AMOEBA */
}

int
ChangeAccessControl(client, fEnabled)
    ClientPtr client;
    int fEnabled;
{
    if (!AuthorizedClient(client))
	return BadAccess;
    AccessEnabled = fEnabled;
    return Success;
}

/*	socket_netlib.c							*/
/*	V1.5			20-Jan-1995	IfN/Mey			*/
/*+
 * SOCKETSHR interface for NETLIB V1.
 *
 *	Copyright (c) 1994, 1995 by Eckart Meyer <meyer@ifn.ing.tu-bs.de>
 *
 * This module provides a socket interface to NETLIB. SOCKETSHR is a
 * shared image providing the socket interface.
 *
 * This is the NETLIB implementation of SOCKETSHR (there are others for
 * native UCX, for Mike O'Malley's LIBCMU and the UNIXSHR library for CMU/IP).
 *
 * Note: Most of the code here is stolen from Mike O'Malley's LIBCMU. I've
 *       made the changes to use NETLIB instead of CMU/IP. Some routines
 *	 have been changed, some are added.
 * 	 Mike's copyright is included here:
 */
/*
** 02-Dec-1998	J. Malmberg	Converted for compilation under GCC
**		    *		Added multiple casts and prototypes
**				that ANSI compilers require.
**		    *		Changed sys$getsyi() to sys$getsyiw()
**				This will prevent problems with some
**				versions of OpenVMS.
**		    *		Changed sys$qio() to not use the
**				default event flag.  See qioef note.
**
**		  **** 		BUG when UDP_POLL or TRACE is defined as true.
**				getenv() can not be used inside of a loop.
**				it does a malloc() each time it is called.
**
** 12-Mar-1999	J. Malmberg	Fix for compilation under DECC.  Some
**				constants are not defined.  DECC more picky
**				about mixing long and unsigned long than
**				GCC.
**
** 15-Mar-1999	J. Malmberg	Compilation errors and warnings when TRACE is
**				defined and DECC.
**
** 06-Nov-2000	T. Dickey	Add fallback definitions for flock stuff so
**				this will still build when the host doesn't
**				provide those definitions.
*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * LIBCMU, Copyright (C) 1993,1994 by Mike O'Malley
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*
 * Facility:	SOCKETSHR_NETLIB
 *
 * Abstract:	Socket interface routines for NETLIB.
 *
 * Module Description:
 *	This collection of routines provides the standard `C' programming
 *	interface for Matt Madison's NETLIB network transport.
 *
 * Routine provided:
 *	int socket(int domain, int type, int protocol)
 *	int bind(int s, struct sockaddr *name, int namelen)
 *	int connect(int s, struct sockaddr *name, int namelen)
 *	int listen (int s, int backlog)
 *	int accept(int s, struct sockaddr *addr, int *addrlen)
 *	int recv(int s, char *buf, int len, int flags)
 *	int recvfrom(int s, char *buf, int len, int flags,
 *			struct sockaddr *from, int *fromlen)
 *	int send(int s, char *msg, int len, int flags)
 *	int sendto(int s, char *msg, int len, int flags,
 *			struct sockaddr *to, int tolen)
 *	int shutdown(int s, int how)
 *
 *	int select(int nfds, int *readfds, int *writefds, int *execptfds,
 *			struct timeval *timeout)
 *
 *	int getsockname(int s, struct sockaddr *name, int *namelen)
 *	int getpeername(int s, struct sockaddr *name, int *namelen)
 *	int getsockopt(int s, int level, int optname,
 *			char *optval, int *optlen)
 *	int setsockopt(int s, int level, int optname,
 *			char *optval, int *optlen)
 *	int gethostname(char *name, int namelen)
 *	struct hostent *gethostbyname(char *name)
 *	struct hostent *gethostbyaddr(char *addr, int len, int type)
 *
 *	int ioctl(int s, int request, void *argp)
 *	int fcntl(int s, int request, int arg)
 *	int writev(int s, struct iovec *iov, int iovcnt)
 *
 * The following routines also present in the VAXCRTL:
 *
 *	int read(int s, char *buf, int len)
 *	int write(int s, char *msg, int len, int flags)
 *	int close(int s)
 *	FILE *netlib1_fdopen(int s)
 *
 * The following routines may be called from outside this library:
 *	short int netlib1_get_sdc(int s)
 *	int *netlib1_stdin_open(char *name)
 *	int netlib1_stdin_read(flags,buf,len,prompt,mask)
 *	int netlib1_get_errno(long status)
 *	int isNetlibSocket(int s)
 *	int netlib1_get_fd(FILE *fptr)
 *
 * The following routines should not be accessed outside this library:
 *	int netlib1_listen_accept(int s)
 *	int netlib1_queue_listen(int s)
 *	void netlib1_read_ast(int s)
 *	void netlib1_write_ast(int s)
 *	int netlib1_queue_net_read(int s)
 *	int netlib1_alloc_socket(int domain, int type, int protocol)
 * 	int netlib1_assign(int s)
#ifdef UDP_POLL
 *	int netlib1_check_udp_receive(int s, int flag)
#endif
 *
 * Acknowledgements:
 *	Guidence, concepts, examples derived from the works of:
 *		UNIXSHR		- unknown
 *		NETLIB		- Matt Madison, RPI
 *		LIBCMU		- Mike O'Malley
 *
 * SOCKETSHR Author:
 *	Eckart Meyer
 *	Institute for Telecommunication
 *	Technical University of Braunschweig
 *	Schleinitzstr. 23
 *	D-38092 Braunschweig
 *	Germany
 *	<meyer@ifn.ing.tu-bs.de>
 *
 *
 * LIBCMU Author: (LIBCMU V1.2)
 *	Mike O'Malley (mlo)				      September 1993
 *	Digital Equipment Corp.			Sandia National Laboratories
 *	Digital Consulting	   Scientific Computing Center, User Support
 *	Albuquerque, NM
 *      Mike.OMalley@aqo.mts.dec.com			  mlomall@sandia.GOV
 *
 *      With modifications by:
 *	 Chen He (che) (617)566-0001 Ext.2919
 *	 IDX Systems Corp.
 *	 he@chen.idx.com
 *
 */
#ifdef VAXC
#module SOCKETSHR_NETLIB "V1.0"
#endif

 /* For the DEC C compiler, need to force the old behavior */
/*--------------------------------------------------------*/
#ifndef _VMS_V6_SOURCE
#define _VMS_V6_SOURCE 1
#endif

/*
 * Include files
 */

/*
 * If not defined types.h will define it for us.  Don't want to handle more
 * than 32 file descriptors at this time.
 */
#define FD_SETSIZE 32

#ifdef __GNUC__
#include <fcntl.h>
		/* Something wrong with bzero in the LIBGCC I have */
#define bzero(__ptr,__cnt) memset((__ptr),0,(__cnt))
#include <strings.h>
#endif
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <netdb.h>

#include <types.h>
#include <ctype.h>
#include <time.h>

#ifdef __DECC_VER
#ifdef isdigit
#undef isdigit
#endif
#ifdef __ctype
#undef __ctype
#endif
#define __ctype (*decc$ga___ctype)
#pragma __extern_model __save
#pragma __extern_model __strict_refdef
extern const char __ctype [];
#pragma __extern_model __restore
#define _D 0x4
#define _X 0x40
#define isdigit(c)	(__ctype [(c)&0177] & _D)

#ifdef isxdigit
#undef isxdigit
#endif
#define isxdigit(c)	(__ctype [(c)&0177] & (_D|_X))

#endif

#include <file.h>
#include <ioctl.h>
#include <socket.h>

/* #include <if.h> */ /**/
#include <in.h>

#include <ssdef.h>
#include <descrip.h>
#include <lnmdef.h>
#include <msgdef.h>
#include <iodef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <syidef.h>

#ifndef __DECC_VER
#include <malloc.h>
#else
#include <stdlib.h>
#include <fcntl.h>
#endif
#include <unixio.h>

#ifndef F_GETFL
#define F_GETFL 3
#endif

#ifndef F_SETFL
#define F_SETFL 4
#endif

#ifndef F_RDLCK
#define F_RDLCK 1
#endif
#ifndef FREAD
#define FREAD F_RDLCK
#endif
#ifndef FEXLOCK
#define FEXLOCK (00400)
#endif
#ifndef F_WRLCK
#define F_WRLCK 2
#endif
#ifndef FWRITE
#define FWRITE F_WRLCK
#endif

 /* VMS System Services and Runtime library prototypes to help optimizer */
/*----------------------------------------------------------------------*/
unsigned long sys$assign
       (const struct dsc$descriptor_s * devnam,
	unsigned short * chan,
	unsigned long acmode,
	const struct dsc$descriptor_s * mbxnam,
	unsigned long flags);

unsigned long sys$bintim
       (const struct dsc$descriptor_s * timbuf,
	void * timadr);

unsigned long sys$cantim
       (unsigned long reqidt,
	unsigned long acmode);

unsigned long sys$clref(unsigned long efn);

unsigned long sys$dassgn(unsigned short chan);

unsigned long sys$getsyiw
       (unsigned long efn,
	unsigned long * csidadr,
	const struct dsc$descriptor_s * nodename,
	void * itmlst,
	void * iosb,
	void (* astadr)(),
	unsigned long astprm);

unsigned long sys$gettim(void * timadr);

unsigned long sys$qio
       (unsigned long efn,
	unsigned short chan,	/* Docs are wrong, this is right */
	unsigned long func,
	void * iosb,
	void (* astadr)(),
	...);

unsigned long sys$readef(unsigned long efn, unsigned long * state);

unsigned long sys$setef(unsigned long efn);

unsigned long sys$setimr
       (unsigned long efn,
	const void * daytim,
	void (* astadr)(),
	unsigned long reqidt,
	unsigned long flags);

unsigned long sys$waitfr(unsigned long efn);

unsigned long sys$wflor(unsigned long efn, unsigned long mask);

unsigned long lib$free_ef(const unsigned long * efn);
unsigned long lib$get_ef(unsigned long * efn);
unsigned long lib$stop(unsigned long cond, ...);

unsigned long str$copy_r
       (struct dsc$descriptor * deststr,
	const unsigned short * srclen,
	const char * srcstr);

unsigned long str$free1_dx(struct dsc$descriptor * str);
unsigned long str$get1_dx
       (const unsigned short * wrdlen,
	struct dsc$descriptor * str);

/*
 * And a NETLIB specific header
 */
#include "socket_netlib.h"

/*
 * NETLIB specific definitions
 */
#define	NET_K_TCP	1
#define	NET_K_UDP	2
#define NET_M_PUSH	1
#define	NET_M_NOTRM	2

int net_assign(void * socket);
int net_deassign(void * socket);
int net_bind(void * socket, int, unsigned short port, int, int);
int net_get_info
       (void * socket,
	unsigned long * remaddr,
	int * remport,
	unsigned long * lcladr,
	int * lclport);
int tcp_connect_addr(void * socket, int * to_addr, int to_port);
int udp_send
       (void * socket,
	unsigned long to_addr,
	int for_port,
	char * msg,
	int len);
int tcp_send
       (void * socket,
	struct dsc$descriptor * write_dsc,
	int flags,
	IO_Status_Block * iosb,
	void (* astadr)(),
	unsigned long astprm);
int net_get_hostname
       (struct dsc$descriptor * host_dsc,
	int * ret_size);
int net_get_address
       (void * ctx,
	struct dsc$descriptor * host_dsc,
	int maxaddr,
	struct in_addr * addrs,
	int * ret_size);
int net_addr_to_name
       (void * namctx,
	unsigned long laddr,
	struct dsc$descriptor * host_dsc);
int tcp_disconnect(void * ctx);
int tcp_accept
       (void * ctx,
	void * accept_ctx,
	IO_Status_Block * iosb,
	int (* astadr)(),
	unsigned long astprm);
int udp_receive
       (void * ctx,
	char * rcvbuf,
	int rcvbufsize,
	unsigned short * byte_count,
	unsigned long * addr,
	unsigned short * port,
	long * timeout,
	IO_Status_Block * iosb,
	void (* astadr)(),
	unsigned long astprm);
int tcp_receive
       (void * ctx,
	struct dsc$descriptor * read_dsc,
	IO_Status_Block * iosb,
	void (* astadr)(),
	unsigned long astprm,
	long * timeout);
/*
 * Common include
 */
#include "[-]si_socket.h"
#ifdef TRACE
  unsigned long int __ntohl( unsigned long int __n);
  char * __inet_ntoa(struct in_addr __in);
#endif

/*
 * TRACE
 */
/* #undef TRACE */ /**/
/* #define FTRACE__  stdout */ /**/
#include "[-]si_trace.h"


unsigned short int ntohs ( unsigned short int __n);
unsigned short int htons ( unsigned short int __n);


/*
 * forward declarations
 */
int netlib1_get_errno(long status);
int __trnlnm(char *table, char *name, char *mode, char *buff, int len);
int netlib1_listen_accept(int s);
int netlib1_queue_listen(int s);
void netlib1_read_ast(int s);
void netlib1_write_ast(int s);
int netlib1_queue_net_read(int s);
int netlib1_alloc_socket(int domain, int type, int protocol);
int netlib1_assign(int s);
#ifdef UDP_POLL
int netlib1_check_udp_receive(int s, int flag);
#endif
short int netlib1_get_sdc(int s);
int isNetlibSocket(int s);
int netlib1_get_fd(FILE *fptr);

FILE *netlib1_fdopen(int s);
int netlib1_close(int s);
int netlib1_getsockname(int s, struct sockaddr * name, int * namelen);
int netlib1_recvfrom
       (int s,
	char * buf,
	int len,
	int flags,
	struct sockaddr * from,
	int * fromlen);
int netlib1_sendto
       (int s,
	char * msg,
	int len,
	int flags,
	struct sockaddr * to,
	int tolen);

#define isSocket isNetlibSocket
#define get_fd netlib1_get_fd

/*
 * Global static variables
 */

/*
 * Our private file descriptor table
 */
static struct FD_ENTRY *sd[FD_SETSIZE];

/*
 * for system information call to get MAXBUF size.
 */
static int MAXBUF;
static struct ITEM_LIST syinfo[2] = {
	{ 4, SYI$_MAXBUF, &MAXBUF, 0 },
	{ 0,           0,       0, 0 }};

/*
 * File descriptor mask to keep track of i/o events.
 */
static fd_set sys_validfds;	/* is sd valid		*/
static fd_set sys_readfds;	/* is sd ready to read	*/
static fd_set sys_writefds;	/* is sd ready to write */
static fd_set sys_exceptfds;	/* does sd have an error*/
#ifdef UDP_POLL
static fd_set sys_udpfds;	/* is sd an UDP sd	*/
#endif

#define MAX_INADDR 20

static	struct hostent inet_host;
static	struct in_addr inet_list[MAX_INADDR];
static	struct in_addr *addrlist[MAX_INADDR];
static	char inet_name[128];	/* storage for official name string */
static	char *null_ptr = NULL;

/*
 * Accept event flag so we know when a request comes in
 */
/* static int accept_net_event; */

/*
 * 'NETLIB loaded' flag for the channel number hack.
 */
#define CMU 2
static int netlib_loaded = 0;

/*
 * for feof() and ferror() to work we have to set the appropriate flags
 * in the FILE structure. Note these macros depend on the implementation
 * of the structure. However, it seems the _flag item has not been
 * modified since ever in VAXC and is still used in the same way in DEC C.
 */
#define SET_FPTR(flg) if (sd[s]->fptr != NULL) (*sd[s]->fptr)->_flag |= (flg);
#define CLR_FPTR(flg) if (sd[s]->fptr != NULL) (*sd[s]->fptr)->_flag &= ~(flg);

/*
 * 03-Dec-1998	J. Malmberg
 * Because any call that uses an event flag will set and clear it,
 * Digital support has recommended that event flag 0 not be used in
 * asynchronous routines.  At the best it causes extra CPU overhead,
 * but under some rare conditions a bug could be triggered.  To
 * prevent against this, an event flag will be reserved for the qio
 */
static unsigned long qioef = 0;


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int netlib1_alloc_socket(int domain, int type, int protocol)
 *
 * Description:
 *	Internal routine. Allocates the socket.
 *
 * Returns:
 *  If successful a socket number is returned, otherwise, a -1 is returned and
 *  errno is set.
 */
int netlib1_alloc_socket(domain,type,protocol)
int domain,type,protocol;
{

int	s;

    /*
     * Verify the address family (domain)
     */
    if ( domain != AF_INET ) {
	/*
	 * we don't handle any other address formats.
	 */
	errno = EAFNOSUPPORT;
	return(-1);
    }

    /*
     * Use the dup() routine to aquire a unique discriptor.
     * Make sure it's in our range.
     */
    /* s = dup(0); */ /* Samba is now sometimes closing 0, breaks this bad! */
    s = open("NLA0:", O_RDONLY);
    if (s<0)
	return(-1);
    else if ((s < 0) || (s > FD_SETSIZE)) {
	errno = ENFILE;
	return (-1);
    }
    FD_SET(s,&sys_validfds);

    /*
     * Allocate a file descriptor data structure and initialize it.
     */
    sd[s] = (struct FD_ENTRY *) calloc( 1, sizeof(struct FD_ENTRY));
    sd[s]->domain	= domain;
    sd[s]->type		= type;

    /*
     * If not specified select a default protocol
     */
    if (protocol == IPPROTO_IP) {
	switch(type) {
	    case SOCK_STREAM :
		sd[s]->protocol	= IPPROTO_TCP;
		break;
	    case SOCK_DGRAM :
	    default:
		sd[s]->protocol	= IPPROTO_UDP;
	}
    }
    else
	switch(protocol) {
	    case IPPROTO_TCP:
	    case IPPROTO_UDP:
		sd[s]->protocol = protocol;
		break;
	    default:
		netlib1_close(s);
		errno = EPROTONOSUPPORT;
		return(-1);
	}

    sd[s]->read_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    sd[s]->read_dsc.dsc$b_class = DSC$K_CLASS_D;
    sd[s]->read_dsc.dsc$w_length = 0;
    sd[s]->read_dsc.dsc$a_pointer = (char *)0;
    sd[s]->write_dsc.dsc$b_dtype = DSC$K_DTYPE_T;
    sd[s]->write_dsc.dsc$b_class = DSC$K_CLASS_S;

    /*
     * grab the OpenVMS SYSGEN parameter MAXBUF and set the default buffer size
     * to the minimum of *_SO_RCVBUF_DEF or MAXBUF
     *
     */
    if (MAXBUF == 0) {
	vaxc$errno = sys$getsyiw( 0, 0, 0, &syinfo, 0, 0, 0);
	if (vaxc$errno != SS$_NORMAL) {
		netlib1_close(s);
		errno = EVMSERR;
		return(-1);
	}
	MAXBUF -= 128;
    }

    if (sd[s]->protocol == IPPROTO_TCP)
	sd[s]->rcvbufsize =
			MAXBUF < TCP_SO_RCVBUF_DEF ? MAXBUF : TCP_SO_RCVBUF_DEF;
    else
	sd[s]->rcvbufsize =
			MAXBUF < UDP_SO_RCVBUF_DEF ? MAXBUF : UDP_SO_RCVBUF_DEF;

    /*
     * get and clear an event flag and file descriptor.
     */
    if (sd[s]->ef == 0)
	lib$get_ef(&sd[s]->ef);
    sys$clref(sd[s]->ef);
    FD_CLR(s, &sys_readfds);

    /*
     * get file pointer for stream routines.
     */
    sd[s]->fptr = fdopen(s,"r");
    (*sd[s]->fptr)->_file = s;	/* backlink, enables standard fileno() */

    return(s);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int netlib1_assign(int s)
 *
 * Description:
 *	Internal routine. Assigns a channel to the network.
 *
 * Returns:
 *  If successful the socket number is returned, otherwise, a -1 is returned and
 *  errno is set.
 */
int netlib1_assign(s)
int s;
{
$DESCRIPTOR(dev,"NLA0:");
char name[128];
    /*
     * What comes here is really a hack: since NETLIB doesn't give us the
     * VMS channel number, only a NETLIB internal context pointer, we
     * allocate a VMS channel, store it away and release the channel.
     * We then *hope* NETLIB will grab the same channel...
     * If this is the first call to NETLIB, one channel is used to access
     * the NETLIB shared image. In this case we call net_assign/net_deassign
     * only to bring in the image. I've heard there is an undocumented
     * system service to load a sharable image, but this works too and
     * is only called once.
     *
     */
    if (!netlib_loaded) {
	netlib_loaded = 1;
	net_assign(&sd[s]->ctx);
	__trnlnm( "LNM$FILE_DEV", "NETLIB_SHR", "USER", name, 128);
	if (strstr(name, "CMU") != NULL) netlib_loaded = CMU;
	DTRACE("  --",0,"NETLIB: %d",netlib_loaded,0);
	net_deassign(&sd[s]->ctx);
    }

    sys$assign(&dev, &sd[s]->chan,0,0,0);
    sys$dassgn(sd[s]->chan);

    vaxc$errno = net_assign(&sd[s]->ctx);
    if (vaxc$errno != SS$_NORMAL) {
	netlib1_close(s);
	errno = EVMSERR;
	return(-1);
    }
    return(s);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int socket(int domain, int type, int protocol)
 *
 * Description:
 *	Creates an endpoint of a connection.
 *
 * domain   - AF_INET is the only address domain supported at this time.
 * type     - SOCK_STREAM or SOCK_DGRAM
 * protocol - IPPROTO_TCP, IPPROTO_UDP.  If IPPROTO_IP (default) is specified
 *	      protocol will be chosen that matches `type'.
 *
 * Returns:
 *  If successful a socket number is returned, otherwise, a -1 is returned and
 *  errno is set.
 */
int netlib1_socket(domain,type,protocol)
int domain,type,protocol;
{
int	s;

    /*
     * allocate a socket
     */
     s = netlib1_alloc_socket(domain,type,protocol);

    /*
     * assign a channel to the network device
     */
     s = netlib1_assign(s);
     STRACE("netlib1_socket",0,s,"returned, chan: %04X",sd[s]->chan,0);
     return(s);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int bind(int s, struct sockaddr *name, int namelen)
 *
 * Description:
 *	binds a name (address/port) to a socket.  Socket (s) must have been
 * created with call to socket.
 *
 * s       - valid socket descriptor
 * name	   - address of sockaddr structure for local host port information
 * namelen - length of the name structure in bytes
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
#ifdef __DECC_VER
    static int get_random_port();
#endif

int netlib1_bind(s,name,namelen)
int			s;	/* socket to bind to	*/
struct sockaddr		*name;	/* name stuff		*/
int			namelen;/* length of name stuff	*/
{
int	status;
/* int	protocol; */
struct	sockaddr_in *my = (struct sockaddr_in *)name;
/* struct	sockaddr_in tmp; */

    STRACE("netlib1_bind",0,s,"addr: %08X, port: %d",
	ntohl(my->sin_addr.s_addr),ntohs(my->sin_port));
    /*
     * Verify the address family (domain)
     */
    if ( name->sa_family != AF_INET ) {
	/*
	 * we don't handle any other address formats.
	 */
	errno = EAFNOSUPPORT;
	return(-1);
    }

    /*
     * check for valid socket.
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * see if it's already named
     */
    if (sd[s]->flags & SD_BIND) {
	errno = EINVAL;
	return(-1);
    }

    /*
     * copy/save sockaddr info.
     */
    sd[s]->mylen = namelen;
    memcpy(&sd[s]->my, name, namelen);
    my = (struct sockaddr_in *)&sd[s]->my;

    /*
     * If the bind is to a connectionless mode socket (IPPROTO_UDP) then
     * OPEN the channel.
     */
    if (sd[s]->protocol == IPPROTO_UDP) {
	/*
	 * Open the communication channel
	 */
	vaxc$errno = net_bind(&sd[s]->ctx, NET_K_UDP, ntohs(my->sin_port),1,1);
	if (vaxc$errno != SS$_NORMAL) {
	    errno = netlib1_get_errno(vaxc$errno);
		/* How to Abort ? */
	    DTRACE("  --",0,"ERROR status:%08X, errno: %d",vaxc$errno,errno);
	    FD_SET(s,&sys_exceptfds);
	    sys$setef(sd[s]->ef);
	    return(-1);
	}

	netlib1_getsockname(s, &sd[s]->my, &sd[s]->mylen);
	DTRACE("  --",0,"actual port: %d",ntohs(my->sin_port),0);

	FD_SET(s,&sys_writefds);
	FD_CLR(s,&sys_exceptfds);

	sd[s]->flags |= SD_CONNECTED;

	/*
	 * queue a read to the socket
	 */
	status = netlib1_queue_net_read(s);
	if (status != 0)
	    return(-1);
    }
/*  Michael Stenn's fix to the port allocation bug ..... */
    else {   /* tcp ports */
        if (my->sin_port == 0) {
            void *socket = NULL;
            unsigned long remadr = 0, lcladr = 0;
            int remport = 0, lclprt = 0;

            net_assign(&socket);
            net_bind(&socket, NET_K_TCP, 0, 0, 0);
            net_get_info (&socket, &remadr, &remport, &lcladr ,&lclprt);
            net_deassign (&socket);
            if (!lclprt) /* AFAIK only if OpenCMU */
               {
#ifndef __DECC_VER
		 static int get_random_port();
#endif
                 my->sin_port = htons (s + get_random_port (30001,65000));
               }
            else
                 my->sin_port = lclprt;
            DTRACE("  --",0,"actual port: %d",ntohs(my->sin_port),0);
        }

/*  --- End of Michael Stenn's fix ---- */

/* Old code to fake the port number where needed
    else {
	if (my->sin_port == 0) {
           unsigned int current_time[2];
           sys$gettim(current_time);

           my->sin_port = htons(current_time[0] % 32767 + 30000 + s);
        }
*/

    }

    sd[s]->flags |= SD_BIND;
    return(0);
}


/* Generate a random port number  between min_port and max_port */
static int get_random_port (int min_port, int max_port)
{
    unsigned int current_time[2];
    int port = min_port;

    if (max_port > min_port) {
        sys$gettim (current_time);
        port += current_time[0] % (max_port - min_port);
    }
    DTRACE("  --",0,"Random port: %d generated",port,0);
    return port;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int connect(int s, struct sockaddr *name, int namelen)
 *
 * Description:
 *	initiates a connection on a socket.
 *
 * s       - valid socket descriptor
 * name	   - pointer to an address structure for the remote port
 * namelen - length of the name structure in bytes
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_connect(s,name,namelen)
int s;
struct sockaddr *name;
int namelen;
{
/* int	status; */
int	to_addr, to_port, my_port;
struct	sockaddr_in *p;
    /*
     * Verify the address family (domain) we are to connect to.
     */
    if ( name->sa_family != AF_INET ) {
	/*
	 * we don't handle any other address formats.
	 */
	errno = EAFNOSUPPORT;
	return(-1);
    }

    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * see if it's already connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0) {
	errno = EISCONN;
	return(-1);
    }

    /*
     * copy/save remote sockaddr info.
     */
    sd[s]->tolen = namelen;
    memcpy(&sd[s]->to, name, namelen);

    p = (struct sockaddr_in *) &sd[s]->to;
    to_addr = p->sin_addr.s_addr;
    to_port = ntohs(p->sin_port);
    p = (struct sockaddr_in *) &sd[s]->my;
    my_port = ntohs(p->sin_port);
    STRACE("netlib1_connect",0,s,"local port: %d",my_port,0);
    STRACE("netlib1_connect",
		0,s,"-- addr: %08X, port: %d",ntohl(to_addr),to_port);
    /*
     * Open and connect to the remote system
     */
    if (sd[s]->protocol == IPPROTO_TCP) {
	vaxc$errno = net_bind(&sd[s]->ctx, NET_K_TCP, my_port,1,1);
	if (vaxc$errno == SS$_NORMAL)
	    vaxc$errno = tcp_connect_addr(&sd[s]->ctx, &to_addr, to_port);
    }
    else {
	vaxc$errno = net_bind(&sd[s]->ctx, NET_K_UDP, my_port,0,1);
    }
    if (vaxc$errno != SS$_NORMAL) {
	errno = netlib1_get_errno(vaxc$errno);
	return(-1);
    }

    /*
     *  Get the real local port number.
     */
     if (my_port == 0) {
	sd[s]->mylen = sizeof(struct sockaddr);
	netlib1_getsockname(s,&sd[s]->my,&sd[s]->mylen);
     }

    /*
     * Now queue a read on the socket.  If the connect failed then the read
     * will also fail right away.
     */
    if (vaxc$errno == SS$_NORMAL)
	if ((netlib1_queue_net_read(s)) == -1)
	    return(-1);

    /*
     * could there be a timing problem here?  Will the read AST complete (on
     * error) before the next statement?  Lets wait a sec anyway just in case.
     */
/*    sleep(1); */ /**/

    vaxc$errno = sd[s]->read_iosb.iosb_status;

    /*
     * 0 = read in progress, 1 = read complete data waiting.
     */
    if ((vaxc$errno != 0) && (vaxc$errno != 1)) {
	errno = netlib1_get_errno(sd[s]->read_iosb.iosb_status);
	FD_SET(s,&sys_exceptfds);
	sys$setef(sd[s]->ef);
	return(-1);
    }

    FD_SET(s,&sys_writefds);
    FD_CLR(s,&sys_exceptfds);

    sd[s]->flags |= SD_CONNECTED;
    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int listen (int s, int backlog)
 *
 * Description:
 *	Sets the maximum limit of outstanding connection requests for a socket
 * that is connection-oriented.
 *
 * s	   - a valid socket descriptor of type SOCK_STREAM
 * backlog - maximum number of pending connections that may be queued
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_listen(s,backlog)
int s;
int backlog;
{
struct sockaddr_in *my;
     STRACE("netlib1_listen",0,s,"backlog: %d",backlog,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    if ((sd[s]->protocol != IPPROTO_TCP) || (sd[s]->type != SOCK_STREAM)) {
	errno = EOPNOTSUPP;
	return(-1);
    }

    sd[s]->backlog = backlog;
    sd[s]->sock_opts |= SO_ACCEPTCONN;	/* socket has had listen() */

    /*
     * Allocate an event flag used to signal a blocked accept call
     */
/*    lib$get_ef(&accept_net_event); */
/*    sys$clref(accept_net_event); */
    sys$clref(sd[s]->ef);

    /*
     * Clear the listen file descriptor
     */
    FD_CLR(s,&sys_readfds);

    /*
     * Set socket to passive TCP on specified bound port.
     */
    my = (struct sockaddr_in *)&sd[s]->my;
    vaxc$errno = net_bind(&sd[s]->ctx, NET_K_TCP, ntohs(my->sin_port),1,0);
    if (vaxc$errno != SS$_NORMAL) {
	errno = netlib1_get_errno(vaxc$errno);
	return(-1);
    }
    /*
     * Queue the socket listen
     */
    if (netlib1_queue_listen(s) == 0)
	return(0);
    else
	return(-1);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int accept(int s, struct sockaddr *addr, int *addrlen)
 *
 * Description:
 *	Accepts a connection on a socket.
 *
 * s	   - valid socket that is listening
 * addr	   - sockaddr address structure filled with information from the accepted
 *           connection
 * addrlen - length of returned address structure
 *
 * Returns:
 *	Nonnegative integer that is the descriptor for the accepted socket or
 * -1 on error.  Addition error information is specified in the global
 * variable errno.
 */
int netlib1_accept( s, addr, addrlen)
int	s;
struct	sockaddr *addr;
int	*addrlen;
{
int	ns, status;
struct	backlogEntry *entry;

    STRACE("netlib1_accept",0,s,"backlogSize: %d",sd[s]->backlogSize,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * see if socket is listening
     */
    if ((sd[s]->sock_opts & SO_ACCEPTCONN) == 0) {
	errno = EBADF;
	return(-1);
    }

    /*
     * see if anything is there
     */
/* CHECK_QUEUE: */
    /*
     * check the socket for incomming connections.
     */
    if (!(FD_ISSET(s,&sys_readfds))) { /**/
/*      if (sd[s]->backlogSize == 0) { */ /**/
	/*
	 * block if necessary
	 */
	if ((sd[s]->ioctl_opts & O_NDELAY) == 0) {
/*	    sys$waitfr(accept_net_event); */
	    sys$waitfr(sd[s]->ef);
	    /*
	     * see if the socket was shutdown
	     */
	    if ((sd[s]->ioctl_opts & FREAD) != 0) {
		return(0);
	    }
	}
	else {
	    errno = EWOULDBLOCK;
	    return(-1);
	}
    }

    /*
     * Clear the event flag
     */
/*    sys$clref(accept_net_event); */ /**/
    sys$clref(sd[s]->ef);
    FD_CLR(s,&sys_readfds);

    /*
     * remove the first entry from the backlog queue
     */
    entry = sd[s]->backlogQueue;
    sd[s]->backlogQueue = entry->flink;

    /*
     * save the socket number and free the backlogEntry
     */
    ns = entry->sock;
    cfree(entry);

    /*
     * queue the first read to the accepted connection
     */
    status = netlib1_queue_net_read(ns);
    if (status != 0)
	return(-1);

    /*
     * fill in addr with the connecting entity information
     */
    if (addr != NULL)
    	memcpy (addr,&(sd[ns]->to), (*addrlen < sizeof(struct sockaddr)
				   ? *addrlen : sizeof(struct sockaddr)));

    /*
     * give the user a new socket descriptor
     */
    return(ns);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int recv(int s, char *buf, int len, int flags)
 *
 * Description:
 *	Receive bytes from a connected socket.
 *
 * s	 - a valid socket descriptor
 * buf	 - address of buffer to where input data is placed
 * len	 - max size of buf
 * flags - 0 or MSG_PEEK may be specified.
 *
 * Returns:
 *	Number of bytes read from the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int netlib1_recv(s,buf,len,flags)
int	s;
char	*buf;
int	len, flags;
{
    STRACE("netlib1_recv",0,s,"len: %d, flags: %08X",len,flags);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0)
	return(netlib1_recvfrom(s,buf,len,flags,0,0));
    else {
	errno = EBADF;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int recvfrom(int s, char *buf, int len, int flags,
 *			struct sockaddr *from, int *fromlen)
 *
 * Description:
 *	receives bytes from a socket from any source.
 *
 * s	 - a valid socket descriptor
 * buf	 - address of buffer to where input data is placed
 * len	 - max size of buf
 * flags - 0 or MSG_PEEK may be specified.
 * from	 - address of sockaddr structure address of the socket which the data
 *         is received from.  May be specified as 0; no information will be
 *         returned.
 * fromlen - length of from structure returned.
 *
 * Returns:
 *	Number of bytes read from the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int netlib1_recvfrom(s,buf,len,flags,from,fromlen)
int	s;
char	*buf;
int	len, flags;
struct	sockaddr *from;
int	*fromlen;
{
int	size, offset;
/* struct	sockaddr_in *frm; */
struct	sockaddr_in *rcv;

    STRACE("netlib1_recvfrom",0,s,"len: %d, flags: %08X",len,flags);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }
    CLR_FPTR(_IOERR | _IOEOF);

    /*
     * make sure the socket is not shutdown
     */
    if ((sd[s]->ioctl_opts & FREAD) != 0) {
	errno = EPIPE;
	goto recvfrom_ioerr;
    }

    /*
     * if the user want's nothing, return immediately
     */
    if (len == 0) return(0);

#ifdef UPD_POLL
    /*
     * NETLIB prior to V1.7:
     * Since NETLIB's udp_receive does not support AST's, we must try
     * to read here with immediate return if nothing has been received.
     */
    if (sd[s]->protocol == IPPROTO_UDP) {
	netlib1_check_udp_receive(s,0);	/* No AST's with UDP, check manually */
    }
#endif

CHECKforDATA:
    /*
     * check for a socket error (if the socket read has completed!)
     */
/*    if (FD_ISSET(s,&sys_readfds)) */ /**/
    if (sd[s]->read_iosb.iosb_status != 0) {
	if (sd[s]->read_iosb.iosb_status != SS$_NORMAL) {
	    errno = netlib1_get_errno(sd[s]->read_iosb.iosb_status);
	    STRACE("netlib1_recvfrom",0,s,"ERROR, status: %08X, errno: %d",
		sd[s]->read_iosb.iosb_status,errno);
/* sd[s]->read_iosb.iosb_status = 0; */ /**/
	    if (sd[s]->read_iosb.iosb_status == SS$_LINKDISCON) {
		SET_FPTR(_IOEOF);
		return(0);
	    }
	    if (errno == ECONNRESET) {
		SET_FPTR(_IOEOF);
		return(-1);
	    }
	    goto recvfrom_ioerr;
	}
    }

    /*
     * Check the file descriptor for data ready on the socket. Block or return
     * based on data availability.
     */
    if (!(FD_ISSET(s,&sys_readfds))) {
	if (sd[s]->ioctl_opts & O_NDELAY) {
	    errno = EWOULDBLOCK;
	    goto recvfrom_ioerr;
	}
	else {
#ifdef UDP_POLL
	    if (sd[s]->protocol == IPPROTO_UDP) {
		netlib1_check_udp_receive(s,1);	/* No AST's with UDP, wait */
	    }
#endif
	    sys$waitfr(sd[s]->ef);
	    goto CHECKforDATA;
	}
    }

    /*
     * move the from data if the user asked for it.
     */
    if (from != NULL) {
	rcv = (struct sockaddr_in *)&sd[s]->rcvfrom;
	size = (sizeof(struct sockaddr) < *fromlen)
				? sizeof(struct sockaddr) : *fromlen;
	memcpy(from, rcv, size);
	*fromlen = size;
    }

    /*
     * data is ready on the socket.  copy it to the users buffer.
     * there could be data remaining in the receive buffer from a previous
     * read or more data in the receive buffer than what would fit in the users
     * buffer.
     */
    STRACE("netlib1_recvfrom",0,s,"available: %d bytes",
					sd[s]->read_iosb.iosb_byte_count,0);
    size = len < sd[s]->read_iosb.iosb_byte_count ?
					len : sd[s]->read_iosb.iosb_byte_count;
    offset = sd[s]->rcvbufoffset;

    memcpy(buf, &sd[s]->rcvbuf[offset], size);

    /*
     * if this is just a peek then return without fixing up the byte count and
     * offset things.
     */
    if ((flags & MSG_PEEK) != 0)
	return (size);

    /*
     * now fix up the byte count in the iosb.  If there is and data left then
     * set the offset.
     */
    sd[s]->read_iosb.iosb_byte_count -= size;
    if (sd[s]->read_iosb.iosb_byte_count == 0) {
	/*
	 * The receive buffer has been drained; reset the offset and
	 * queue another read.
	 */
	sd[s]->rcvbufoffset = 0;
	netlib1_queue_net_read(s);
    }
    else
	sd[s]->rcvbufoffset += size;

    /*
     * return the number of bytes that were copied to the users buffer.
     */
    STRACE("netlib1_recvfrom",0,s,"returned %d bytes",size,0);
    return (size);

recvfrom_ioerr:
    SET_FPTR(_IOERR);
    return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int send(int s, char *msg, int len, int flags)
 *
 * Description:
 *	Send bytes through a socket to its connected peer.
 *
 * s	 - a valid socket descriptor
 * buf	 - address of buffer of data to be sent
 * len	 - size of buf
 * flags - 0 or MSG_PEEK may be specified.
 *
 * Returns:
 *	Number of bytes written to the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int netlib1_send(s,msg,len,flags)
int	s;
char	*msg;
int	len, flags;
{
    STRACE("netlib1_send",0,s,"len: %d, flags: %08X",len,flags);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0)
	return(netlib1_sendto(s,msg,len,flags,0,0));
    else {
	errno = EBADF;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int sendto(int s, char *msg, int len, int flags,
 *			struct sockaddr *to, int tolen)
 *
 * Description:
 *	Send bytes through a socket to any other socket.
 *
 * s	 - a valid socket descriptor
 * buf	 - address of buffer of data to be sent
 * len	 - size of buf
 * flags - none supported.
 * to	 - address of sockaddr structure which contains the address of the
	   socket which the data is to be written.
 * tolen - length of from structure returned.
 *
 * Returns:
 *	Number of bytes written to the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int netlib1_sendto(s,msg,len,flags,to,tolen)
int	s;
char	*msg;
int	len, flags;
struct	sockaddr *to;
int	tolen;
{
struct	sockaddr_in *too;
static unsigned long write_ef = 0;

    STRACE("netlib1_sendto",0,s,"len: %d, flags: %08X",len,flags);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }
    CLR_FPTR(_IOERR | _IOEOF);

    /*
     * make sure the socket is not shutdown
     */
    if ((sd[s]->ioctl_opts & FWRITE) != 0) {
	errno = EPIPE;
	goto sendto_ioerr;
    }

    /*
     * in a UDP environment we could get to this point without actually opening
     * the communication channel via bind.  It might be cause the user only
     * wants a send only channel.
     */
    if ((sd[s]->flags & SD_CONNECTED) == 0) {
	vaxc$errno = net_bind(&sd[s]->ctx, NET_K_UDP, 0, 0,1);
	if (vaxc$errno != SS$_NORMAL) {
	    errno = netlib1_get_errno(vaxc$errno);
	    goto sendto_ioerr;
	}
    /*
     *  Get the real local port number.
     */
        sd[s]->mylen = sizeof(struct sockaddr);
        netlib1_getsockname(s,&sd[s]->my,&sd[s]->mylen);

	sd[s]->flags |= SD_CONNECTED;

	FD_SET(s,&sys_writefds);
	FD_CLR(s,&sys_exceptfds);
    }

    /*
     * check the target domain (address family)
     */
    if (to != NULL)
	if (to->sa_family != AF_INET) {
	    errno = EAFNOSUPPORT;
	    goto sendto_ioerr;
	}

    /*
     * record who we sent it to...
     */
    if (to != NULL) {
	sd[s]->tolen = tolen;
	memcpy(&sd[s]->to, to, tolen);
    }

    too = (struct sockaddr_in *)&sd[s]->to;

/* My CMUIP doesn't like zero length writes... IfN/Mey */
    if (len == 0) return(0);

    /*
     * get an event flag specific for writing
     */
    if (write_ef == 0)
	lib$get_ef(&write_ef);

    /*
     * Clear the system write ready fds and queue the write.
     * For UDP, NETLIB does not support AST's, so we simply wait...
     */
    FD_CLR(s,&sys_writefds);
    if (sd[s]->protocol == IPPROTO_UDP) {
	vaxc$errno = udp_send(&sd[s]->ctx,
		too->sin_addr.s_addr,	/* address to send to */
		ntohs(too->sin_port),	/* foreign port */
		msg,			/* message buffer address */
		len);			/* message length */
	sd[s]->write_iosb.iosb_status = vaxc$errno;
    }
    else {
	sd[s]->write_dsc.dsc$a_pointer = msg;
	sd[s]->write_dsc.dsc$w_length = len;
	vaxc$errno = tcp_send(&sd[s]->ctx,
		&sd[s]->write_dsc,	/* message descriptor */
		NET_M_NOTRM,		/* flags: don't append CR/LF */
		&sd[s]->write_iosb,	/* I/O status block */
		netlib1_write_ast, s);
    }
    if (vaxc$errno == SS$_NORMAL)
	return (len);
    else {
	errno = netlib1_get_errno(sd[s]->write_iosb.iosb_status);
        DTRACE("  --",0,"ERROR status:%08X, errno: %d",vaxc$errno,errno);
    }
sendto_ioerr:
    SET_FPTR(_IOERR);
    return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <socket.h>
 *
 * int shutdown(int s, int how)
 *
 * Description:
 *	Shuts down all or part of a connection on a socket.
 *
 * s - a valid socket
 * how - one of:
 *	0 - no more receives
 *	1 - no more sends
 *	2 - no more receives or sends
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_shutdown(s,how)
int s, how;
{

    STRACE("netlib1_shutdown",0,s,"how: %d",how,0);
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * if not connected the no need to shutdown
     */
    if ((sd[s]->flags & SD_CONNECTED) == 0) {
	errno = ENOTCONN;
	return(-1);
    }

    switch (how) {
	case 2 :
	case 1 :
		sd[s]->ioctl_opts |= FWRITE;
		if (how != 2) break;
	case 0 :
		sd[s]->ioctl_opts |= FREAD;
		if ((sd[s]->sock_opts & SO_ACCEPTCONN) != 0)
/*		    sys$setef(accept_net_event); */ /**/
		    sys$setef(sd[s]->ef);
/*		sys$cancel(sd[s]->chan); 	can't do ?? */
		break;
	default :
	    errno = EINVAL;
	    return(-1);
    }
    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int select(int nfds, int *readfds, int *writefds, int *execptfds,
 *		struct timeval *timeout)
 *
 * Description:
 *	Allows the user to poll or check a group of sockets for I/O activity.
 * It can check what sockets are ready to be read or written, or what sockets
 * have a pending exception.
 *
 * Notes:
 *	This routine (and library) only handles 32 file descriptors max (0-31).
 * The `fd_set' type is long int.  To select on file descriptor 0 (stdin) the
 * routine netlib1_stdin_open must be used to setup the file descriptor.  See
 * routines netlib1_stdin_open and netlib1_stdin_read for further information.
 *
 * nfds		- maximum file descriptor +1 to scan
 * readfds	- address of descriptor bit mask to scan for read events
 * writefds	- address of descriptor bit mask to scan for write events
 * exceptfds	- address of descriptor bit mask to scan for exception events
 * timeout	- specifies how long to wait for a read, write, or exception
 *		  event.  If timeout argument is NULL select will block until
 *		  one of the specified descriptors is ready.
 *
 * Returns:
 *	Number of socket descriptors ready for I/O or that have exceptions,
 * 0 if the operation timed out, -1 on error.  Addition error information is
 * specified in the global variable errno.
 */
int netlib1_select(nfds,readfds,writefds,exceptfds,timeout)
int	nfds;
int	*readfds, *writefds, *exceptfds;
struct	timeval *timeout;
{
int	i;
int	maxfds, maxfds_mask, ready_fds, all_fds;
int	readyfds;

int	block, t[2];
unsigned long timer_ef;
#ifdef UDP_POLL
unsigned long	udp_tick_ef;
int	udp_tick_time[2];
#endif
char	at[20];
/* char	*p; */
struct	tm *lt;
struct dsc$descriptor_s ascii_time =
	{20, DSC$K_DTYPE_T, DSC$K_CLASS_S, at };

int	ef_mask;
#define EF_BASE 32

    DTRACE("netlib1_select",0,"nfds: %d",nfds,0);
    readyfds = 0;
    FD_ZERO(&ready_fds);
    FD_ZERO(&all_fds);
    FD_ZERO(&ef_mask);

    block = timer_ef = 0;

    maxfds = nfds < FD_SETSIZE ? nfds+1 : FD_SETSIZE;

    /*
     * Don't allow more than 32 file descriptors
     */
    if (maxfds > 32) {
	errno = EBADF;
        readyfds = -1;
	goto EXIT;
    }

    if (maxfds == 32)
	maxfds_mask = 0xffffffff;
    else {
	maxfds_mask = 0;
	for (i=0; i < maxfds; i++)
	    maxfds_mask |= (1<<i);
    }

    /*
     * Clear extranious bits and check for bad file descriptors.  Gather all
     * file descriptor bits into `all_fds' for use later.
     */
    if (exceptfds != NULL) {
	*exceptfds &= maxfds_mask;
	DTRACE("  --",0,"except fd mask: %08X",*exceptfds,0);
	if ((*exceptfds & sys_validfds.fds_bits[0]) != *exceptfds)
	{
	    /*
	     * Set the exceptfds mask to indicate which fd was bad.
	     */
	    *exceptfds ^= sys_validfds.fds_bits[0];
	    errno = EBADF;
	    return(-1);
	}
	all_fds = *exceptfds;
    }

    if (writefds != NULL) {
	*writefds &= maxfds_mask;
	DTRACE("  --",0,"write fd mask: %08X",*writefds,0);
	if ((*writefds & sys_validfds.fds_bits[0]) != *writefds) {
	    /*
	     * Set the exceptfds mask to indicate which fd was bad.
	     */
	    *writefds ^= sys_validfds.fds_bits[0];
	    errno = EBADF;
	    return(-1);
	}
	all_fds |= *writefds;
    }

    if (readfds != NULL) {
	*readfds &= maxfds_mask;
	DTRACE("  --",0,"read fd mask: %08X",*readfds,0);
	if ((*readfds & sys_validfds.fds_bits[0]) != *readfds) {
	    /*
	     * Set the exceptfds mask to indicate which fd was bad.
	     */
	    *readfds ^= sys_validfds.fds_bits[0];
	    errno = EBADF;
	    return(-1);
	}
	all_fds |= *readfds;
    }

    DTRACE("  --",0,"all_fd mask %08X",all_fds,0);

#ifdef UDP_POLL
    /*
     * NETLIB's lack of UDP receive AST's:
     * Setup polling interval.
     */
    if((sys_udpfds & all_fds) != 0) {
	    sprintf(at,"0 00:00:01.00");
	    p = getenv("SOCKETSHR_UDP_POLLING_INTERVAL");
	    if (p != NULL) strcpy(at,p);
	    ascii_time.dsc$w_length = strlen(at);
	    i = sys$bintim(&ascii_time, &udp_tick_time);
	    if (!(i&1)) lib$stop(i);
	    lib$get_ef(&udp_tick_ef);
	    FD_SET((udp_tick_ef - EF_BASE),(fd_set *)&ef_mask);
DTRACE("netlib1_select",0,"udp_tick_ef=%d, mask=%08X",udp_tick_ef-EF_BASE,ef_mask);
    }
#endif

    /*
     * if this is a timed event then setup the timer.
     */
    if (timeout == NULL)
	block++;
    else
	if (timeout->tv_sec != 0  ||  timeout->tv_usec != 0) {
	    block++;
	    /*
	     * setup a timer AST to check later
	     */
	    if (timeout->tv_sec > 86399) { /* not grater than 24 hours */
		errno = EINVAL;
		readyfds = -1;
		goto EXIT;
	    }
	    lt = localtime(&timeout->tv_sec);
	    sprintf(at,"0 %02.2d:%02.2d:%02.2d.%02.2d",
		lt->tm_hour, lt->tm_min, lt->tm_sec, (int)timeout->tv_usec);
	    ascii_time.dsc$w_length = strlen(at);
	    sys$bintim(&ascii_time, &t);
	    lib$get_ef(&timer_ef);
	    sys$setimr(timer_ef,&t,0,timer_ef,0);
	    FD_SET((timer_ef - EF_BASE), (fd_set *)&ef_mask);
DTRACE("netlib1_select",0,"timer-ef=%d, mask=%08X",timer_ef-EF_BASE,ef_mask);
	}

CHECK_DESCRIPTORS:
DTRACE("netlib1_select",0,"check_descriptors loop",0,0);

#ifdef UDP_POLL
    /*
     * NETLIB's lack of UDP AST's:
     * check all UDP file descriptors.
     */
    if((ready_fds = sys_udpfds & all_fds) != 0)
	for (i=0; i < maxfds; i++)
	    if (FD_ISSET(i,&ready_fds))
		netlib1_check_udp_receive(i,0);
#endif

    /*
     * exception file descriptors
     */
    if (exceptfds != NULL)
	if((ready_fds = sys_exceptfds.fds_bits[0] & *exceptfds) != 0)
	    for (i=0; i < maxfds; i++)
		if (FD_ISSET(i, (fd_set *)&ready_fds))
		    readyfds++;

    /*
     * write file descriptors
     */
    if (writefds != NULL)
	if((ready_fds = sys_writefds.fds_bits[0] & *writefds) != 0)
	    for (i=0; i < maxfds; i++)
		if (FD_ISSET(i, (fd_set *)&ready_fds))
		    readyfds++;
    /*
     * read file descriptors
     */
    if (readfds != NULL)
DTRACE("netlib1_select",
	0,"sys_readfds mask: %08X",sys_readfds.fds_bits[0],0);
	if((ready_fds = sys_readfds.fds_bits[0] & *readfds) != 0)
	    for (i=0; i < maxfds; i++)
		if (FD_ISSET(i, (fd_set *)&ready_fds))
		    readyfds++;
    /*
     * See if we'er ready to exit
     */
    if (readyfds == 0)
	if (block) {
	    if (timer_ef != 0)
		if (sys$clref(timer_ef) == SS$_WASSET)
		    goto EXIT;
	    for (i=0; i < maxfds; i++)
		if (FD_ISSET(i, (fd_set *)&all_fds)) {
		    FD_SET((sd[i]->ef - EF_BASE), (fd_set *)&ef_mask);
STRACE("netlib1_select",0,i,"ef=%d, mask=%08X",sd[i]->ef-EF_BASE,ef_mask);
		}
#ifdef UDP_POLL
    	    if((sys_udpfds & all_fds) != 0) {
		sys$clref(udp_tick_ef);
DTRACE("netlib1_select",0,"sys$wflor, udp_tick_ef: %d",udp_tick_ef,0);
	        sys$setimr(udp_tick_ef,&udp_tick_time,0,udp_tick_ef,0);
	    }
#endif

DTRACE("netlib1_select",0,"sys$wflor, mask=%08X",ef_mask,0);
	    sys$wflor(EF_BASE,ef_mask);
{
unsigned long mm;
sys$readef(EF_BASE,&mm);
DTRACE("netlib1_select",0,"$READEF mask=%08X",mm,0);
}
	    goto CHECK_DESCRIPTORS;
	}

EXIT:
    if (timer_ef != 0) {
	sys$cantim(timer_ef,0);
	lib$free_ef(&timer_ef);
    }
#ifdef UDP_POLL
    if (udp_tick_ef != 0) {
	sys$cantim(udp_tick_ef,0);
	lib$free_ef(&udp_tick_ef);
    }
#endif
    if (exceptfds != NULL)
	*exceptfds = (sys_exceptfds.fds_bits[0] & maxfds_mask);
    if (writefds != NULL)
	*writefds = (sys_writefds.fds_bits[0] & maxfds_mask);
    if (readfds != NULL)
	*readfds = (sys_readfds.fds_bits[0] & maxfds_mask);
DTRACE("netlib1_select",0,"returns: %d fds ready",readyfds,0);
    return(readyfds);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int getsockname(int s, struct sockaddr *name, int *namelen)
 *
 * Description:
 *	Returns the name associated with a socket
 *
 * s	   - a valid socket descriptor
 * name	   - sockaddr address structure where data is to be written
 * namelen - number of bytes written to name
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_getsockname(s,name,namelen)
int	s;
struct	sockaddr *name;
int	*namelen;
{
int	ret_size;
struct sockaddr_in loc;
struct sockaddr_in rem;
struct sockaddr_in *local = &loc;
struct sockaddr_in *remote = &rem;
struct dsc$descriptor host_dsc =
	{sizeof(inet_name), DSC$K_DTYPE_T, DSC$K_CLASS_S, inet_name};

/* int i; */
/* char buf[1024]; */
/* unsigned short int iosb[4]; */

int rport;					/* AH 2-JUN-1997 */
int lport;					/* AH 2-JUN-1997 */

    STRACE("netlib1_getsockname",0,s,"",0,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

/*    local		= &sd[s]->my; */ /**/
    local->sin_addr.s_addr = 0;
    local->sin_port    = 0;
    sd[s]->mylen = sizeof(struct sockaddr);

    vaxc$errno = net_get_info(&sd[s]->ctx,
		&remote->sin_addr.s_addr,	/* foreign address */
		&rport,				/* foreign port */
		&local->sin_addr.s_addr,	/* local address */
		&lport);			/* local port */


    remote->sin_port = rport;			/* AH 2-JUN-1997 */
    local->sin_port  = lport;			/* AH 2-JUN-1997 */

    if (vaxc$errno != SS$_NORMAL) {
	errno = netlib1_get_errno(vaxc$errno);
        DTRACE("  --",0,"ERROR status:%08X, errno: %d",vaxc$errno,errno);
    }
    else {
#if 0	/* NETLIB Versions before 1.6 have a bug... */
	if (netlib_loaded == CMU) {
	    remote->sin_port = htons(remote->sin_port);
	    local->sin_port = htons(local->sin_port);
	}
#endif
	((struct sockaddr_in *)&sd[s]->my)->sin_port = local->sin_port;
    }
    /*
     * net_get_info does not always give us the local address.
     * if 0 get the address via gethostname/gethostbyname.
     */
    if (local->sin_addr.s_addr == 0) {
	inet_name[0] = '\0';
	vaxc$errno = net_get_hostname(&host_dsc,&ret_size);
	inet_name[ret_size] = '\0';
        DTRACE("  --",0,"net_get_hostname: {%s}",inet_name,0);
/*	if (vaxc$errno == SS$_NORMAL) { */ /**/
	    vaxc$errno = net_get_address(&sd[s]->ctx,
		&host_dsc,		/* host name descriptor */
		1,			/* maximum number of addresses */
		&local->sin_addr,   /* array to be written with addresses */
		&ret_size);		/* returned number of addresses */
            DTRACE("  --",0,"net_get_address: status: %08X",vaxc$errno,0);
/*	} */ /**/
    }

    if (local->sin_addr.s_addr != 0) {
	((struct sockaddr_in *)&sd[s]->my)->sin_addr.s_addr =
						local->sin_addr.s_addr;
    }

    local = (struct sockaddr_in *)&sd[s]->my;

    local->sin_family	= AF_INET;

    STRACE("netlib1_getsockname",0,s,"-- Local  Port: %d, Local  Addr: %08X",
	ntohs(local->sin_port),ntohl(local->sin_addr.s_addr));
    STRACE("netlib1_getsockname",0,s,"-- Remote Port: %d, Remote Addr: %08X",
	ntohs(remote->sin_port),ntohl(remote->sin_addr.s_addr));

    /*
     * determine amount of data to return to caller
     */
    ret_size = (sizeof(struct sockaddr) < *namelen)
				? sizeof(struct sockaddr) : *namelen;
    /*
     * make the copy
     */
    memcpy(name, &sd[s]->my, ret_size);
    *namelen = ret_size;

    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int getpeername(int s, struct sockaddr *name, int *namelen)
 *
 * Description:
 *	Returns the name associated with a socket
 *
 * s	   - a valid socket descriptor
 * name	   - sockaddr address structure where data is to be written
 * namelen - number of bytes written to name
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_getpeername(s,name,namelen)
int	s;
struct	sockaddr *name;
int	*namelen;
{
int	ret_size;
struct sockaddr_in loc;
struct sockaddr_in rem;
struct sockaddr_in *local = &loc;
struct sockaddr_in *remote = &rem;

int rport;						/* AH 2-JUN-1997 */
int lport;						/* AH 2-JUN-1997 */

    STRACE("netlib1_getpeername",0,s,"",0,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

/*     local		= &sd[s]->my; */
     local->sin_family	= AF_INET;
     remote->sin_family	= AF_INET;
     sd[s]->mylen = sizeof(struct sockaddr);

		/* Need to cheat a bit with the port numbers as netlib requires
		  them to be long words rather than words */
     vaxc$errno = net_get_info(&sd[s]->ctx,
		&remote->sin_addr.s_addr,	/* foreign address */
		&rport,				/* foreign port */
		&local->sin_addr.s_addr,	/* local address */
		&lport);			/* local port */


     remote->sin_port = rport;			/* AH 2-JUN-1997 */
     local->sin_port  = lport;			/* AH 2-JUN-1997 */

     if (vaxc$errno != SS$_NORMAL) {
	errno = netlib1_get_errno(vaxc$errno);
        DTRACE("  --",0,"ERROR status:%08X, errno: %d",vaxc$errno,errno);
     }
     else {
#if 0
	if (netlib_loaded == CMU) {
	    remote->sin_port = htons(remote->sin_port);
	    local->sin_port = htons(local->sin_port);
	}
#endif
	memcpy(&sd[s]->my, local, sizeof(struct sockaddr));
     }
     local = (struct sockaddr_in *)&sd[s]->my;

    STRACE("netlib1_getpeername",0,s,"-- Local  Port: %d, Local  Addr: %08X",
	ntohs(local->sin_port),ntohl(local->sin_addr.s_addr));
    STRACE("netlib1_getpeername",0,s,"-- Remote Port: %d, Remote Addr: %08X",
	ntohs(remote->sin_port),ntohl(remote->sin_addr.s_addr));

    /*
     * determine amount of data to return to caller
     */
    ret_size = (sizeof(struct sockaddr) < *namelen)
				? sizeof(struct sockaddr) : *namelen;
    /*
     * make the copy
     */
    memcpy(name, remote, ret_size);
    *namelen = ret_size;

    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int getsockopt(int s, int level, int optname,
 *			char *optval, int *optlen)
 *
 * Description:
 *	Returns the options set on a socket.
 *
 * Note:
 *	This routine does nothing.  Always returns -1 with errno set to
 * ENOPROTOOPT.
 *
 * Returns: (if it did do something)
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_getsockopt(s,level,optname,optval,optlen)
int	s, level, optname;
char	*optval;
int	*optlen;
{
    STRACE("netlib1_getsockopt",0,s,"optname: %d",optname,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * there are no socket level options at this time.
     */
    errno = ENOPROTOOPT;
    return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int setsockopt(int s, int level, int optname,
 *			char *optval, int *optlen)
 *
 * Description: (if it did do something)
 *	Set the options on a socket.
 *
 * Note:
 *	This routine does nothing.  Always returns -1 with errno set to
 * ENOPROTOOPT.
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_setsockopt(s,level,optname,optval,optlen)
int	s, level, optname;
char	*optval;
int	optlen;
{
    STRACE("netlib1_setsockopt",0,s,"optname: %d",optname,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * there are no socket level options at this time.
     */
    errno = ENOPROTOOPT;
    return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int gethostname(char *name, int namelen)
 *
 * Description:
 *	Returns the name currently associated to the host.
 *
 * name	   - address of buffer to write name of host
 * namelen - length of name buffer
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_gethostname(name, namelen)
char	*name;
int	namelen;
{
int len;
struct dsc$descriptor host_dsc =
	{namelen, DSC$K_DTYPE_T, DSC$K_CLASS_S, name};

    DTRACE("netlib1_gethostname",0,"",0,0);
    vaxc$errno = net_get_hostname(&host_dsc,&len);
    if (vaxc$errno != SS$_NORMAL) {
	errno = netlib1_get_errno(vaxc$errno);
        DTRACE("  --",0,"ERROR status: %08X, errno: %d",vaxc$errno,errno);
#if 0
	return(-1);
#endif
    }
    name[len] = '\0';
    DTRACE("netlib1_gethostname",0,"returned hostname: {%s}, len: %d",name,len);
    return(0);
}

#if 0	/* no longer used, see [-]READDB.C */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct servent *getservbyname(char *name, char *proto);
 *
 * Description:
 *	returns a servent structure filled in with information about the
 * requested service.
 *
 * name  - pointer to character string of service to search for
 * proto - pointer to character string of protocol type desired (tcp, udp)
 *
 * Notes:
 *	The usual services text file is not supported.  Instead, services are
 * are logical names defined in the system logical name table.  Services
 * logical names are defined as follows:
 *
 *	$ DEFINE /SYSTEM INET$SERVICE_service_name_protocol value
 *
 * Example:
 *   To define the service telnet, protocol tcp, port 23 use the following
 * statement:
 *	$ DEFINE/SYSTEM INET$SERVICE_TELNET_TCP 23
 *
 * Returns:
 *	Returns the address of a servent structure on success, the NULL pointer
 * on error (see __trnlnm).
 */

static struct servent serv;

struct servent *netlib1_getservbyname(name, proto)
char	*name;
char	*proto;
{
char	logical[256];
char	port[16];


    sprintf(logical,"INET$SERVICE_%s_%s",name,proto);

    vaxc$errno = __trnlnm( "LNM$SYSTEM", &logical, "Super", &port, 16);
    if (vaxc$errno == 0) {
	serv.s_name = name;
	serv.s_port = htons(atoi(&port));
	serv.s_proto = proto;
        DTRACE("netlib1_getservbyname",0,"returned name: %s, port: %d",serv.s_name,ntohs(serv.s_port));
	return (&serv);
    }
    else
	return(NULL);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * struct servent *getservbyport(int port, char *proto);
 *
 * Description:
 *	returns a servent structure filled in with information about the
 * requested port.
 *
 * port  - int value of port number to search for
 * proto - pointer to character string of protocol type desired (tcp, udp)
 *
 * Notes:
 *	The usual services text file is not supported.  Instead, services are
 * are logical names defined in the system logical name table.  Services
 * logical names are defined as follows:
 *
 *	$ DEFINE /SYSTEM INET$SERVICE_port_number_protocol service
 *
 * Example:
 *   To define the service telnet, protocol tcp, port 23 use the following
 * statement:
 *	$ DEFINE/SYSTEM INET$SERVICE_23_TCP TELNET
 *
 * Returns:
 *	Returns the address of a servent structure on success, the NULL pointer
 * on error (see __trnlnm).
 */
struct servent *netlib1_getservbyport(port, proto)
int	port;
char	*proto;
{
char	logical[256];
char	name[32];

    sprintf(logical,"INET$SERVICE_%d_%s",port,proto);

    vaxc$errno = __trnlnm( "LNM$SYSTEM", &logical, "Super", &name, 32);
    if (vaxc$errno == 0) {
	serv.s_name = &name;
	serv.s_port = htons(port);
	serv.s_proto = proto;
        DTRACE("netlib1_getservbyname",0,"returned name: %s, port: %d",serv.s_name,ntohs(serv.s_port));
	return (&serv);
    }
    else
	return(NULL);
}
#endif

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct hostent *gethostbyname(char *name)
 *
 * Description:
 *	returns the host address associated with the given name.
 *
 * name - pointer to a character string that contains the host/domain name to
 *        search for.
 *
 * Return:
 *	Returns the address of a hostent structure on success, the NULL pointer
 * on error.  Additional error information is returned in the global variable
 * errno.
 */
struct hostent *netlib1_gethostbyname(name)
char	*name;
{
void	*namctx;
int	alcount;
int	i;
unsigned long	laddr;
struct dsc$descriptor host_dsc =
	{0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};

    DTRACE("netlib1_gethostbyname",0,"name: %s",name,0);
    /*
     * If the first character of the hostname is numeric then quit (must be dot
     * notation address).
     */

    if (isdigit(name[0])  ||  strlen(name) == 0) {
	errno = EBADF;
	return(NULL);
    }

    net_assign(&namctx);

    i = strlen(name);
    str$copy_r(&host_dsc, (unsigned short *)&i, name);
    alcount = 0;
DTRACE("  --",0,"**1**",0,0);
    vaxc$errno = net_get_address(&namctx,
		&host_dsc,		/* host name descriptor */
		MAX_INADDR,		/* maximum number of addresses */
		inet_list,		/* array to be written with addresses */
		&alcount);		/* returned number of addresses */
DTRACE("  --",0,"**2**",0,0);


    if (vaxc$errno != SS$_NORMAL) {
	errno = netlib1_get_errno(vaxc$errno);
	net_deassign(&namctx);
	return(NULL);
    }

    /*
     * Get the official name of the host we just got the address for
     */
    memcpy(&laddr, &inet_list[0], sizeof(struct in_addr));
    vaxc$errno = net_addr_to_name(&namctx,
		laddr,			/* address */
		&host_dsc);		/* dynamic descriptor for host name */

    net_deassign(&namctx);

    if (vaxc$errno != SS$_NORMAL) {
	errno = netlib1_get_errno(vaxc$errno);
	return(NULL);
    }

    memcpy(inet_name,host_dsc.dsc$a_pointer,host_dsc.dsc$w_length);
    inet_name[host_dsc.dsc$w_length] = '\0';

    inet_host.h_name     = inet_name;
    inet_host.h_aliases  = &null_ptr;
    inet_host.h_addrtype = AF_INET;
    inet_host.h_length   = sizeof(struct in_addr);
    for (i=0; i<alcount; i++) addrlist[i] = &inet_list[i];
    addrlist[alcount] = NULL;
    inet_host.h_addr_list = (char **)addrlist;
    str$free1_dx(&host_dsc);
    DTRACE("netlib1_gethostbyname",0,"returned name: %s, first addr: %s",
	inet_host.h_name,INTOA(inet_host.h_addr));
    return(&inet_host);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * struct hostent *gethostbyaddr(char *addr, int len, int type)
 *
 * Description:
 *	Return official name of host given the host address.
 *
 * addr - a pointer to a series of bytes in network order specifying the
 *        address of the host to search for.
 * len  - number of bytes in the addr
 * type - address format. Only AF_INET is supported.
 *
 * Return:
 *	Returns the address of a hostent structure on success, the NULL pointer
 * on error.  Additional error information is returned in the global variable
 * errno.
 */

struct hostent *netlib1_gethostbyaddr(addr,len,type)
char	*addr;
int	len, type;
{
void	*namctx;
struct dsc$descriptor host_dsc =
	{0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};
int	alcount;
int	i;
unsigned long	laddr;

    DTRACE("netlib1_gethostbyaddr",0,"addr: %08X",*((long *)addr),0);
    if (type != AF_INET) {
	errno = EAFNOSUPPORT;
	return(NULL);
    }

    net_assign(&namctx);

    memcpy(&laddr, addr, sizeof(struct in_addr));

    vaxc$errno = net_addr_to_name(&namctx,
		laddr,			/* address */
		&host_dsc);		/* dynamic descriptor for host name */

    net_deassign(&namctx);

    if (vaxc$errno != SS$_NORMAL) {
	errno = netlib1_get_errno(vaxc$errno);
	return(NULL);
    }

    memcpy(inet_name,host_dsc.dsc$a_pointer,host_dsc.dsc$w_length);
    inet_name[host_dsc.dsc$w_length] = '\0';

    alcount = 1;
    memcpy(&inet_list[0], addr, sizeof(struct in_addr));

    inet_host.h_name     = inet_name;
    inet_host.h_aliases  = &null_ptr;
    inet_host.h_addrtype = AF_INET;
    inet_host.h_length   = sizeof(struct in_addr);
    for (i=0; i<alcount; i++) addrlist[i] = &inet_list[i];
    addrlist[alcount] = NULL;
    inet_host.h_addr_list = (char **)addrlist;
    str$free1_dx(&host_dsc);
    DTRACE("netlib1_gethostbyaddr",0,"returned name: %s, addr: %s",
	inet_host.h_name,INTOA(inet_host.h_addr));
    return(&inet_host);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <ioctl.h>
 *
 * int ioctl(int s, int request, char *argp)
 *
 * Description:
 *	control device.
 *
 * s       - a valid socket descriptor
 * request - one of:
 *		FIOCLEX		- set exclusive use on socket
 *		FIONCLEX	- remove exclusive use
 *		FIONBIO		- set/clear non-blocking i/o
 *		FIONREAD	- get # bytes ready to be read
 * argp    - address of buffer for return information
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
static int sign(val)
{
    if (val == 0) return(0);
    return( (val > 0) ? 1 : -1);
}

int netlib1_ioctl(s,request,argp)
int	s;
int	request;
void	*argp;
{
/* int	i; */
unsigned long *n;

    STRACE("netlib1_ioctl",0,s,"request: %08X",request,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    errno = 0;

    switch ((request & 0x0000ff00) >> 8) {
	case 't' :
	case 's' :
	case 'r' :
	case 'i' :
	    errno = EIO;
	    break;
	case 'f' :
	    n  = (unsigned long *)argp;
	    switch (request & 0x000000ff) {
		case 1 :	/* FIOCLEX - set exclusive use on socket */
		case 2 :	/* FIONCLEX - remove exclusive use	 */
		    sd[s]->ioctl_opts &= ~FEXLOCK;
		    sd[s]->ioctl_opts |= FEXLOCK * sign(*n);
		    break;
		case 126 :	/* FIONBIO - set/clear non-blocking i/o  */
		    sd[s]->ioctl_opts &= ~O_NDELAY;
		    sd[s]->ioctl_opts |= O_NDELAY * sign(*n);
		    break;
		case 127:	/* FIONREAD - get # bytes ready to be read */
		    /*
		     * if the socket read blocked return an error
		     */
		    if ((sd[s]->ioctl_opts & FREAD) != 0)
			errno = EPIPE;
		    if (FD_ISSET(s,&sys_readfds))
			*n = sd[s]->read_iosb.iosb_byte_count;
		    else
			*n = 0;
		    break;
		default:
		    errno = EIO;
		    break;
	    }
	    break;
    }

    return ( errno == 0 ? 0 : -1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <file.h>
 *
 * int fcntl(int s, int request, int arg)
 *
 * Description:
 *	file control.
 *
 * s       - a valid socket descriptor
 * request - One of:
 *		F_GETFL - get file flags
 *		F_SETFL - set file flags
 * arg     - flags to set
 *
 * Returns:
 *	Value of flags or -1 on error.  Addition error information is
 * specified in the global variable errno.
 */
int netlib1_fcntl(s,request,arg)
int s, request, arg;
{
    STRACE("netlib1_fcntl",0,s,"request: %08X, arg: %d",request,arg);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    switch (request) {
	case F_GETFL :
	    return(sd[s]->ioctl_opts);
	case F_SETFL :
	    if (arg == O_NDELAY) {
		sd[s]->ioctl_opts = arg;
		return(0);
            }
	    return(-1);
	default :
	    errno = EINVAL;
	    return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int read(int s, char *buf, int len)
 *
 * Description:
 *	Receive bytes from a connected socket.
 *
 * s   - a valid socket descriptor
 * buf - address of buffer to where input data is placed
 * len - max size of buf
 *
 * Returns:
 *	Number of bytes read from the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int netlib1_read(s,buf,len)
int	s;
char	*buf;
int	len;
{
    STRACE("netlib1_read",0,s,"%d bytes",len,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0)
	return(netlib1_recvfrom(s,buf,len,0,0,0));
    else {
	errno = EBADF;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int write(int s, char *msg, int len, int flags)
 *
 * Description:
 *	Send bytes through a socket to its connected peer.
 *
 * s   - a valid socket descriptor
 * buf - address of buffer of data to be sent
 * len - size of buf
 *
 * Returns:
 *	Number of bytes written to the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int netlib1_write(s,msg,len)
int	s;
char	*msg;
int	len;
{
    STRACE("netlib1_write",0,s,"%d bytes",len,0);
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0)
	return(netlib1_sendto(s,msg,len,0,0,0));
    else {
	errno = EBADF;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int close(int s)
 *
 * Description:
 *	Closes a connection and deletes a socket descriptor.
 *
 * To avoid confusion and link warnings regarding the `close' routine
 * for other file descriptors this routine name is `netlib1_close'.
 * A "#define close(s) netlib1_close(s)" or direct call to netlib1_close(s) is needed in routines
 * close sockets managed by this library.   If this routine is called with a socket
 * descriptor that is not managed by this library it will pass that descriptor
 * to the system close routine.
 *
 * s - an open socket descriptor
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_close(s)
int	s;
{
struct	backlogEntry *entry;

    STRACE("netlib1_close",0,s,"",0,0);
    /*
     * if this is not one of our channels then pass it on to the systems
     * close routine.
     */
    if (sd[s] == NULL)
	return(close(s));

    /*
     * Verify that the channel is indeed open -- in some cases it may not be.
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0) {
	/*
	 * close/deallocate the I/O channel
	 */
	tcp_disconnect(&sd[s]->ctx);
    }

    /*
     * Clear all the associated fd bits
     */
    FD_CLR(s,&sys_validfds);
    FD_CLR(s,&sys_readfds);
    FD_CLR(s,&sys_writefds);
    FD_CLR(s,&sys_exceptfds);
#ifdef UDP_POLL
    FD_CLR(s,&sys_udpfds);
#endif

    /*
     * free the various resources that we have accumulated
     */
    if (sd[s]->ef != 0)
	lib$free_ef(&sd[s]->ef);

#if 0
    if (sd[s]->protocol == IPPROTO_UDP) {
	if (sd[s]->rcvbuf != NULL)
	    free(sd[s]->rcvbuf);
    }
#endif
    if (sd[s]->read_dsc.dsc$a_pointer != NULL)
	str$free1_dx(&sd[s]->read_dsc);
    /*
     * was this an `accept'ed socket?
     */
    if (sd[s]->listen_socket != 0  &&  sd[sd[s]->listen_socket] != NULL) {
	sd[sd[s]->listen_socket]->backlogSize--;
	/*
	 * requeue the listen if it was shutdown
	 */
	if ((sd[sd[s]->listen_socket]->flags & SD_LISTENING) == 0)
	    netlib1_queue_listen(sd[s]->listen_socket);
    }

    /*
     * purge the listen queue if this was a listen socket
     */
    if ((sd[s]->sock_opts & SO_ACCEPTCONN) != 0) {
	while (sd[s]->backlogQueue != NULL) {
	    entry = sd[s]->backlogQueue;
	    sd[s]->backlogQueue = entry->flink;
	    cfree(entry);
	}
    }

    net_deassign(&sd[s]->ctx);

    cfree(sd[s]);
    sd[s] = 0;
    close(s);	/* this should free up the duped file descriptor */
    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include "netlib.h"
 *
 * int *netlib1_stdin_open(char *name)
 *
 * Description:
 *	To have stdin a device that can be selected the descriptor needs to be
 * allocated within the domain of these routines.  This routine allocates the
 * file descriptor and returns the address of the FD_ENTRY so the user can
 * control reads/writes and still use select to see if the descriptor is ready.
 * Optionally the user can use the following routine to queue reads on stdin.
 *
 * name - character string device name of the device to assign a channel to.
 *
 * Returns:
 *	Address of an FD_ENTRY structure on success, -1 on error.  Addition
 * error information is specified in the global variable errno.
 */
int *netlib1_stdin_open(name)
char	*name;
{
struct	dsc$descriptor_s dev_name;

    if (sd[0] != NULL) {
	errno = EBADF;
	return (int *)(-1);
    }
    sd[0] = (struct FD_ENTRY *)calloc( 1, sizeof(struct FD_ENTRY));
    FD_CLR(0,&sys_readfds);
    FD_SET(0,&sys_validfds);

    /*
     * copy the `name' to the `my' sockaddr area (seems like a logical place)
     */
    if (strlen(name) <= 14) {
	sd[0]->mylen = strlen(name);
	sd[0]->my.sa_family = 0;
	memcpy(&sd[0]->my.sa_data, name, strlen(name));
    }
    else {
	errno = ENAMETOOLONG;
	goto ERROR_RETURN;
    }

    /*
     * setup a descriptor for the device and assign a channel to it
     */
    dev_name.dsc$w_length  = sd[0]->mylen;
    dev_name.dsc$b_dtype   = DSC$K_DTYPE_T;
    dev_name.dsc$b_class   = DSC$K_CLASS_S;
    dev_name.dsc$a_pointer = (char *)&sd[0]->my.sa_data;

    vaxc$errno = sys$assign(&dev_name, &sd[0]->chan, 0, 0, 0);

    if (vaxc$errno != SS$_NORMAL) {
	errno = EVMSERR;
	goto ERROR_RETURN;
    }

    lib$get_ef(&sd[0]->ef);
    sys$clref(sd[0]->ef);
    /*
     * return the address of the socket descriptor so the user can play with
     * it.
     */
    return((int *)sd[0]);

ERROR_RETURN:
    if (sd[0] != NULL)
	cfree(sd[0]);
    sd[0] = NULL;
    return((int *)-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int netlib1_stdin_read(int flags, char *buf, int len, char *prompt, int mask)
 *
 * Description:
 *	queue an i/o to the descriptor described in sd[0].  Since this read is
 * ment to be used with the select call a non-blocking qio is done.  The
 * completion routine will set the file descriptor and event flag on read
 * completion.
 *
 * flags  - qio read modifiers
 * buf    - to receive characters read
 * len    - max characters to read
 * prompt - prompt string for read
 * mask   - read terminator mask
 *
 * Returns:
 *	Status from the SYS$QIO system service call.
 */
int netlib1_stdin_read(flags,buf,len,prompt,mask)
int	flags;
char	*buf;
int	len;
char	*prompt;
int	mask;
{

    /*
     * Clear these system things.
     */
    sys$clref(sd[0]->ef);
    FD_CLR(0, &sys_readfds);

    if (qioef == 0)
	lib$get_ef(&qioef);

    if (prompt != NULL)
	vaxc$errno = sys$qio(qioef, sd[0]->chan, flags, &sd[0]->read_iosb,
			netlib1_read_ast,0,
			buf, len,
			0, mask, prompt, strlen(prompt));
    else
	vaxc$errno = sys$qio(qioef, sd[0]->chan, flags, &sd[0]->read_iosb,
			netlib1_read_ast,0,
			buf, len,
			0, 0, 0, 0);
    return(vaxc$errno);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int netlib1_get_sdc(int s)
 *
 * Description:
 *	Returns the assigned VMS channel for a particular file descriptor
 *
 * s - a valid socket descriptor
 *
 * Returns:
 *  If successful a VMS i/o channel number is returned otherwise a 0 is returned and
 *  errno is set.
 *
 */
short int netlib1_get_sdc(s)
int	s;
{
    /*
     * check for valid socket.
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(0);
    }
    else
	return(sd[s]->chan);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int netlib1_get_errno(long status)
 *
 * Description:
 *	returns best guess value for errno.
 *
 * status - status code
 *
 * Returns:
 *	Returns an errno value.
 */
int netlib1_get_errno(status)
long status;
{
int netlib1_errno = 0;

#define ERRMAP(vms,unix) \
	case vms:\
		netlib1_errno = unix;\
		break;
    /*
     * Choose errno based on status.
     */
    switch (status) {

	ERRMAP(SS$_NORMAL	, 0)
	ERRMAP(0		, EVMSERR)	/* use vaxc$errno */
	ERRMAP(SS$_ABORT	, ECONNABORTED)	/* INETACP has died */
	ERRMAP(SS$_MEDOFL	, ENETDOWN)	/* INETACP has died */
	ERRMAP(SS$_BADPARAM	, EINVAL)	/* parameter not valid for call */
	ERRMAP(SS$_EXQUOTA	, EMSGSIZE)	/* buffer allocation problems */
	ERRMAP(SS$_INSFMEM	, ENOMEM)	/* buffer allocation problems */
	ERRMAP(SS$_NOPRIV	, EACCES)	/* no privilege */
	ERRMAP(SS$_ACCVIO	, EFAULT)	/* buffer access problems */
	ERRMAP(SS$_ILLCNTRFUNC	, EOPNOTSUPP)	/* operation not supported */
	ERRMAP(SS$_CONNECFAIL	, ECONNRESET)	/* conn reset by peer */
	ERRMAP(SS$_DEVINACT	, ENETDOWN)
	ERRMAP(SS$_DEVNOTMOUNT	, ENETDOWN)
	ERRMAP(SS$_DUPLNAM	, EADDRINUSE)	/* bind failed, port in use */
	ERRMAP(SS$_FILALRACC	, EEXIST)	/* file exists */
	ERRMAP(SS$_IVADDR	, EADDRNOTAVAIL)/* invalid address */
	ERRMAP(SS$_IVBUFLEN	, EINVAL)	/* invalid address buffer */
	ERRMAP(SS$_NOLICENSE	, ECONNREFUSED)	/* no license */
	ERRMAP(SS$_NOLINKS	, ENOTCONN)	/* not connected */
	ERRMAP(SS$_NOOPER	, EACCES)	/* OPER priv required */
	ERRMAP(SS$_LINKABORT	, ECONNABORTED)	/* software caused conn abort */
	ERRMAP(SS$_LINKDISCON	, EPIPE)	/* socket closed by remote */
	ERRMAP(SS$_PROTOCOL	, EAFNOSUPPORT)	/* TCP/IP and UDP/IP only */
	ERRMAP(SS$_REJECT	, ECONNREFUSED)	/* many reasons for this */
	ERRMAP(SS$_SHUT		, ENETDOWN)	/* network is shutting down */
	ERRMAP(SS$_SUSPENDED	, EWOULDBLOCK)	/* accept with no request */
	ERRMAP(SS$_TIMEOUT	, ETIMEDOUT)	/* operation timed out */
	ERRMAP(SS$_TOOMUCHDATA	, EMSGSIZE)	/* IP packet size problem */
	ERRMAP(SS$_UNREACHABLE	, ENETUNREACH)	/* network not reachable */
	ERRMAP(SS$_BUFFEROVF	, EMSGSIZE)	/* buffer allocation problems */
	ERRMAP(SS$_RESULTOVF	, EMSGSIZE)	/* buffer allocation problems */
	ERRMAP(SS$_NOTNETDEV	, ENOTSOCK)	/* socket op. on a non-socket */
	ERRMAP(SS$_NOSUCHNODE	, EDESTADDRREQ)	/* dest addr required */
	ERRMAP(SS$_RESET	, ENETRESET)	/* network dropped conn. */
	ERRMAP(SS$_NOSUCHDEV	, ENXIO)	/* no such dev or addr */

	default:
/*		netlib1_errno = EIO; */ /**/
		netlib1_errno = EVMSERR; /**/
	}
	return(netlib1_errno);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int netlib1_listen_accept(int s)
 *
 * Description:
 *	AST completion routine called when a connection completes on an open
 *	listening socket.
 *
 * s - valid socket descriptor
 *
 * Returns:
 *	None.
 */
int netlib1_listen_accept(s)
int	s;	/* socket descriptor */
{
int	ns;
/* int	tmp; */
struct	backlogEntry *backlogQueue;
/* struct	sockaddr_in *to; */

    STRACE("netlib1_listen_accept",0,s,"status: %08X",sd[s]->read_iosb.iosb_status,0);
    /*
     * check the stats of the completed open
     */
    if (sd[s]->read_iosb.iosb_status != SS$_NORMAL) {
	/*
	 * failed; set exceptfds and event
	 */
	FD_SET(s,&sys_exceptfds);
	sys$setef(sd[s]->ef);
	return(SS$_NORMAL);
    }

    /*
     * get a new clean socket like the one we have now
     */
    ns = netlib1_alloc_socket(sd[s]->domain, sd[s]->type, sd[s]->protocol);
    sd[ns]->chan = sd[s]->accept_chan;
    sd[ns]->accept_chan = 0;
    sd[ns]->ctx = sd[s]->accept_ctx;
    sd[ns]->accept_ctx = 0;

    /*
     * Get connection information
     */
    sd[ns]->tolen = sizeof(struct sockaddr);
    netlib1_getpeername(ns, &sd[ns]->to, &sd[ns]->tolen);

    /*
     * copy the local port information
     */
    memcpy(&sd[ns]->my, &sd[s]->my,   sizeof(struct sockaddr));

    /*
     * Set the bound and connected flags.
     */
    sd[ns]->flags = (SD_BIND | SD_CONNECTED);

    /*
     * indicate which socket this accept came from.
     */
    sd[ns]->listen_socket = s;

    /*
     * switch the assigned I/O channels
     */
/*    tmp		 = sd[s]->chan; */
/*    sd[s]->chan  = sd[ns]->chan; */
/*    sd[ns]->chan = tmp;    */

    /*
     * insert this new socket into the backlog queue
     */
    if (sd[s]->backlogQueue == NULL) {
	sd[s]->backlogQueue =
		(struct backlogEntry *)calloc(1,sizeof(struct backlogEntry));
	backlogQueue = sd[s]->backlogQueue;
    }
    else {
	backlogQueue = sd[s]->backlogQueue;
	while (backlogQueue->flink != NULL)
	    backlogQueue = backlogQueue->flink;
	backlogQueue->flink = (struct backlogEntry *)
			calloc(1,sizeof(struct backlogEntry));
	backlogQueue = backlogQueue->flink;
    }
    backlogQueue->sock = ns;

    /*
     * Increment the count of accepted requests.
     */
    sd[s]->backlogSize++;

    /*
     * tell someone how cares that something came in...
     */
    FD_SET(s,&sys_readfds);
/*    sys$setef(accept_net_event); */
    sys$setef(sd[s]->ef);

    /*
     * queue another listen
     */
    netlib1_queue_listen(s);

    return(SS$_NORMAL);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int netlib1_queue_listen(int s)
 *
 * Description:
 *	Queue a read that will accept a TCP connection
 *
 * s - a valid socket descriptor
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_queue_listen(s)
int	s;	/* socket descriptor to listen on */
{
/* struct	sockaddr_in *my; */
/* int ns; */
$DESCRIPTOR(dev,"NLA0:");

    STRACE("netlib1_queue_listen",0,s,"",0,0);
    /*
     * if reads are shutdown on this socket then don't queue the listen
     */
    if ((sd[s]->ioctl_opts & FREAD) != 0) {
	errno = ESHUTDOWN;
	return(-1);
    }

    /*
     * if there are already 'backlog' number of connections then exit without
     * posting the listen.  The close routine will requeue the listen.
     */
    if (sd[s]->backlogSize < sd[s]->backlog) {
	sd[s]->flags |= SD_LISTENING;

    /*
     * get a new NETLIB context for accept socket
     * same hack here as in socket()
     */
	sys$assign(&dev, &sd[s]->accept_chan, 0, 0, 0);
	sys$dassgn(sd[s]->accept_chan);
	vaxc$errno = net_assign(&sd[s]->accept_ctx);
	if (vaxc$errno != SS$_NORMAL) {
	    errno = netlib1_get_errno(vaxc$errno);
	    return(-1);
	}

    /*
     * now post the listen
     */
	vaxc$errno = tcp_accept
		       (&sd[s]->ctx,
			&sd[s]->accept_ctx,
			&sd[s]->read_iosb,
			netlib1_listen_accept,
			s);	/* ast completion routine and param */

	if (vaxc$errno != SS$_NORMAL) {
	    errno = netlib1_get_errno(sd[s]->read_iosb.iosb_status);
	    return(-1);
	}
	sd[s]->flags |= SD_CONNECTED;
    }
    else {
	sd[s]->flags ^= SD_LISTENING;
    }

    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int netlib1_read_ast(int s)
 *
 * Description:
 *	AST read completion routine.  Sets the global sys_readfds to
 * indicate the socket is ready to read or sys_exceptfds if there was an
 * error.
 *
 * s - valid socket descriptor
 *
 * Returns:
 *	Nothing.
 */
void netlib1_read_ast(s)
int	s;	/* file descriptor that is ready. */
{
struct sockaddr_in *rcv;

    STRACE("netlib1_read_ast",0,s,"iosb: %08X",sd[s]->read_iosb.iosb_status,0);
    switch (sd[s]->read_iosb.iosb_status) {
	case SS$_NORMAL:
	    FD_SET(s,&sys_readfds);
	case SS$_ABORT:
	case SS$_CANCEL:
	    /*
	     * if the status code is SS$_CANCEL or SS$_ABORT then I/O on the
	     * channel was cancled.  Don't set the exception flag in this case.
	     */
	    break;
	default:
	    /*
	     * all other status returns indicate an error
	     */
	    FD_SET(s,&sys_exceptfds);
	    FD_SET(s,&sys_readfds);	/* ???? */
    }


    /*
     * check for socket closed by remote. If it was then make if no-read,
     * no-write, and invalid.
     */
/*
 *    if ((sd[s]->read_iosb.X.STATUS & 0x0000ff00) == 0x0600) {
 *	sd[s]->ioctl_opts |= (FREAD & FWRITE);
 *	FD_CLR(s,&sys_validfds);
 *    }
 */
#ifdef UDP_POLL
    FD_CLR(s,&sys_udpfds);	/* don't wait for UDP read any longer */
#endif
    if (sd[s]->protocol == IPPROTO_UDP) {
	rcv = (struct sockaddr_in *)&sd[s]->rcvfrom;
	rcv->sin_port = htons(rcv->sin_port);
	rcv->sin_family = AF_INET;
	DTRACE("  --",0,"received addr: %s, port: %d",
		inet_ntoa(rcv->sin_addr),ntohs(rcv->sin_port));
    }
    /*
     * now set the event flag to signal the read has completed.
     */
    sys$setef(sd[s]->ef);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * void netlib1_write_ast(int s)
 *
 * Description:
 *	AST write completion routine.  Sets the global sys_writefds to
 * indicate the socket is ready to be written or sys_exceptfds if there was an
 * error.
 *
 * s - valid socket descriptor
 *
 * Returns:
 *	Nothing.
 */
void netlib1_write_ast(s)
int	s;	/* file descriptor that is ready. */
{
    if (sd[s]->write_iosb.iosb_status == SS$_NORMAL)
	FD_SET(s,&sys_writefds);
    else
	FD_SET(s,&sys_exceptfds);

}

#ifdef UDP_POLL
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int netlib1_check_udp_receive(int s, int flag);
 *
 * Description:
 *	Check UDP read on socket `s'.
 * On immediate timeout return SS$_NORMAL;
 * On completion call netlib1_read_ast. This replaced the NETLIB lack
 * of AST's on UDP receive.
 *
 * s - valid socket descriptor
 * flag - wait flag: 0 return immediately, 1 wait for something received
 *
 * Returns:
 *	VMS status.
 * the global variable errno.
 */
int netlib1_check_udp_receive(int s, int wait)
{
long timeout[2];
struct sockaddr_in *rcv;
struct sockaddr_in *my;
int i;

    STRACE("netlib1_check_udp_receive",0,s,"wait: %d",wait,0);
    /*
     * Check if something has been received already. If so return.
     */
    if (sd[s]->read_iosb.iosb_status != 0) return(SS$_NORMAL);

    /*
     * Setup timeout: immediately or never.
     */
    if (wait) {
	timeout[0] = 0;
	timeout[1] = 0xFFF00000;	/* That's 5212 days and a few hours..*/
    }
    else {
	timeout[0] = -1;
	timeout[1] = -1;		/* That's 1 nano-second... immediately*/
    }

    /*
     * now try to read...
     */
    my = &sd[s]->my;
    rcv = &sd[s]->rcvfrom;
    DTRACE("  --",0,"rcvbuf=%08X, size=%d",sd[s]->rcvbuf,sd[s]->rcvbufsize);
    i = 0;
    vaxc$errno = udp_receive(&sd[s]->ctx,
	sd[s]->rcvbuf,		/* receive buffer*/
	sd[s]->rcvbufsize,	/* receive buffer size*/
	&sd[s]->read_iosb.iosb_byte_count,	/* received bytes */
	&i,			/* receive from address */
	&rcv->sin_port,		/* receive from port */
	timeout,
	0,
	0,
	0); /**/
    rcv->sin_addr.s_addr = i;

    /*
     * On timeout return normally. iosb is untouched.
     */
    if (vaxc$errno == SS$_TIMEOUT) {
    DTRACE("  --",0,"UDP read timeout - nothing to receive",0,0);
/*
 * Here is the next hack: on CMU/IP the UDP socket disappears when
 * the read is cancelled. Just setup a new socket and don't care
 * for errors.
 */
	vaxc$errno = net_bind(&sd[s]->ctx, NET_K_UDP, ntohs(my->sin_port),1,1);
	if (vaxc$errno != SS$_NORMAL) {
	    DTRACE("  --",0,"ERROR status:%08X",vaxc$errno,0);
	    return(SS$_NORMAL);
	}
	else {
	    return(SS$_NORMAL);
	}
    }

    /*
     * Either something has been received or error. In any case, call
     * the AST-routine and return status.
     */
    sd[s]->read_iosb.iosb_status = vaxc$errno;
    netlib1_read_ast(s);
    return(vaxc$errno);
}
#endif	/* UPD_POLL */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int netlib1_queue_net_read(int s)
 *
 * Description:
 *	Queue a read on socket `s' to be completed by net_read_ast.  This
 * routine will also allocate an event flag and receive buffer if necessary.
 *
#ifdef UDP_POLL
 * We have some problems with UDP here: NETLIB doesn't support AST's with
 * UDP receive. So we use use a minimum timeout to return immediately.
#endif
 *
 * s - valid socket descriptor
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int netlib1_queue_net_read(s)
int	s;
{
long timeout[2];
struct sockaddr_in *rcv;

    STRACE("netlib1_queue_net_read",0,s,"",0,0);
    /*
     * clear event flag and file descriptor for this read request.
     */
    sys$clref(sd[s]->ef);
    FD_CLR(s, &sys_readfds);

    /*
     * Allocate a receive buffer for reads
     */
    if (sd[s]->rcvbuf == NULL) {
	vaxc$errno = str$get1_dx
	    ((unsigned short *)&sd[s]->rcvbufsize,&sd[s]->read_dsc);
/*	sd[s]->rcvbuf = malloc(sd[s]->rcvbufsize); */ /**/
	if (vaxc$errno != SS$_NORMAL) {
/*		How to abort ?	*/
	    errno = ENOBUFS;
	    return(-1);
	}
	sd[s]->rcvbuf = sd[s]->read_dsc.dsc$a_pointer;
    }

    sys$clref(sd[s]->ef);

    timeout[0] = 0;
    timeout[1] = 0xFFF00000;		/* That's 5212 days and a few hours..*/
    sd[s]->read_iosb.iosb_byte_count = 0;
    sd[s]->read_iosb.iosb_status = 0;
    DTRACE("  --",0,"rcvbuf=%08X, size=%d",sd[s]->rcvbuf,sd[s]->rcvbufsize);
    if (sd[s]->protocol == IPPROTO_UDP) {
#ifdef UDP_POLL
	FD_SET(s,&sys_udpfds);
	vaxc$errno = netlib1_check_udp_receive(s,0);
#else
	rcv = (struct sockaddr_in *)&sd[s]->rcvfrom;
	vaxc$errno = udp_receive(&sd[s]->ctx,
		sd[s]->rcvbuf,		/* receive buffer*/
		sd[s]->rcvbufsize,	/* receive buffer size*/
		&sd[s]->read_iosb.iosb_byte_count,	/* received bytes */
# if 1
		&rcv->sin_addr.s_addr,	/* receive from address */
		&rcv->sin_port,		/* receive from port */
		timeout,
		&sd[s]->read_iosb,
		netlib1_read_ast, s);	/* AST completion routine and arg */
# else
		&rcv->sin_addr.s_addr,	/* receive from address */
		&rcv->sin_port,		/* receive from port */
		timeout,
		0,0,0);
# endif
#endif	/* UDP_POLL */
    }
    else {
	vaxc$errno = tcp_receive(&sd[s]->ctx,
		&sd[s]->read_dsc,	/* receive buffer descriptor */
		&sd[s]->read_iosb,
		netlib1_read_ast, s,	/* AST completion routine and arg */
		timeout);
    }
    DTRACE("  --",0,"status=%08X, iosb=%08X",vaxc$errno,sd[s]->read_iosb.iosb_status);
    if (vaxc$errno == SS$_NORMAL)
	return(0);
    else {
	errno = netlib1_get_errno(sd[s]->read_iosb.iosb_status);
	FD_SET(s,&sys_exceptfds);
	sys$setef(sd[s]->ef);
/*		Abort??? */
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int isNetlibSocket(int s)
 *
 * Description:
 *	Checks if the socket is a NETLIB socket. If so return 1 if not return 0.
 *
 * s - a valid socket descriptor
 *
 * Returns:
 *  returns 1 if s is a NETLIB socket otherwise 0.
 *  Caution: Never use TRACE routines here, as this routine is used
 *  by the trace routines themself!
 *
 */
int isNetlibSocket(s)
int	s;
{
    /*
     * check for valid socket.
     */
    return( (sd[s] != NULL) ? 1 : 0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int netlib1_get_fd(fptr)
 *
 * Description:
 *	return the socket from a file pointer.
 *
 * s - a file pointer
 *
 * Returns:
 *  returns the socket. if no socket is found return -1.
 *  Caution: Never use TRACE routines here, as this routine is used
 *  by the trace routines themself!
 *
 */
int netlib1_get_fd(fptr)
FILE	*fptr;
{
int i;
    for (i=0;i<FD_SETSIZE;i++) {
	if (sd[i] != NULL  &&  sd[i]->fptr == fptr) {
	    return(i);
	}
    }
    return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * FILE *netlib1_fdopen(s)
 *
 * Description:
 *      return the file pointer.
 *
 * s - a valid socket descriptor
 *
 * Returns:
 *  returns the file pointer for the socket. if the socket in not a
 *  netlib socket, return NULL.
 *
 * Note that creating more than one file pointer for one socket is
 * not supported! fdopen always returns the same pointer. This pointer
 * is never used for stream I/O, it only identifies the socket.
 */
FILE *netlib1_fdopen(s)
int s;
{
    STRACE("netlib1_fdopen",0,s,"",0,0);
    if (sd[s] == NULL) {
	errno = EBADF;
	return(NULL);
    }
    DTRACE("netlib1_fdopen",0,"return fptr=%08X",sd[s]->fptr,0);
    return(sd[s]->fptr);
}

#if 0	/* This routine is in SI_SOCKET.C */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int netlib1_writev(s,iov,iovcnt)
 *
 * Description:
 *      return the file pointer.
 *
 * s - a valid socket descriptor
 * iov - IO vector
 * iovcnt - number of elements in vector
 *
 * Returns:
 *  returns the number of bytes actually written.
 *
 * Note that creating more than one file pointer for one socket is
 * not supported! fdopen always returns the same pointer. This pointer
 * is never used for stream I/O, it only identifies the socket.
 */
int netlib1_writev(s,iov,iovcnt)
int s;
struct iovec *iov;
int iovcnt;
{
int i;
int k;
int cnt = 0;

    STRACE("writev",0,s,"iovcnt: %d",iovcnt,0);
	for(i=0;i<iovcnt;i++) {
/* fprintf
      (stderr,"  %d: len = %d\n{%s}\n",i,iov[i].iov_len,iov[i].iov_base);*/ /**/
		k = (sd[s] != NULL ?
			send(s, iov[i].iov_base, iov[i].iov_len, 0) :
			write(s, iov[i].iov_base, iov[i].iov_len) );
		if (k == -1) return(-1);
		cnt += k;
	}
	return(cnt);
}
#endif

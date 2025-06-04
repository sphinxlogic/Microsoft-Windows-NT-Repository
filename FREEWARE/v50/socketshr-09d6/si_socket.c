/*	SI_SOCKET.C
 *	V1.5			12-Jan-1995		IfN/Mey
 *+
 * Here are the entry points.
 *
 * Compile this module with /DEFINE=package (e.g. /DEFINE=NETLIB)
 *-
 */
/*
** 01-DEC-1998	J. Malmberg	Minor changes so ANSI compilers will accept.
**
**		  **** 		BUG when UDP_POLL or TRACE is defined as true.
**				getenv() can not be used inside of a loop.
**				it does a malloc() each time it is called.
**
** 12-Mar-1999	J. Malmberg	DECC has problem with __inet_ntoa prototype.
**				h_errno defined differently than GCC.
**
** 15-Mar-1999	J. Malmberg	Compilation errors and warnings when TRACE is
**				defined and DECC.
**
** 05-Nov-2000	T. Dickey	fix a byte-order dependency in si_fputc()
**				Use socketprv.h
**
** 06-Nov-2000	T. Dickey	correct misplaced ifdef __DECC_VER that made
**				trace__ and ftrace__ undefined at link time.
*/

/*
 * NOTE: Never include socketshr.h, since we call the original routines here!
 */

#ifdef LIBCMU
#define isSocket isLibcmuSocket
#define get_fd cmu_get_fd
#ifdef __DECC
#define PACK(pack) cmu_ ## pack
#else
#define PACK(pack) cmu_\
pack
#endif
#endif

#ifdef NETLIB1
#define isSocket isNetlibSocket
#define get_fd netlib1_get_fd
#ifndef VAXC
#define PACK(pack) netlib1_ ## pack
#else
#define PACK(pack) netlib1_\
pack
#endif
#endif

#ifdef UCX
/*
 * UCX entry points call the DEC RTL routines directly with a few exceptions.
 */
#define isSocket isUCXSocket
#define get_fd ucx_get_fd
#ifdef __DECC
#define PACK(pack) decc$ ## pack
/* Routines not supported by DEC C */
#define decc$ioctl ucx_ioctl
#define decc$fcntl ucx_fcntl
#define decc$fdopen ucx_fdopen
#else
#define PACK(pack) vaxc$\
pack
/* Routines not supported by VAXC */
#define vaxc$ioctl ucx_ioctl
#define vaxc$fcntl ucx_fcntl
#define vaxc$fdopen ucx_fdopen
/* vaxcrtl routines other than socket routines does not have the vaxc$ prefix */
#define vaxc$close close
#define vaxc$read read
#define vaxc$write write
#endif
void ucx_socket_fptr(int s);
void ucx_close_fptr(int s);
#endif

#ifdef __GNUC__
#include <stddef.h>
/* #do not include <arpa/inet.h>  it maps to DECC$RTL/UCX$RTL Only */
/* Socketshr is now only needed for CMU-IP/VAX */
/* Here is the stuff that will work from the GCC headers. */
/*
 * Internet address (a structure for historical reasons)
 */
struct in_addr {
    union {
	struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
	struct { unsigned short s_w1,s_w2; } S_un_w;
	unsigned long S_addr;
    } S_un;
#define s_addr	S_un.S_addr	/* can be used for most tcp & ip code */
#define s_host	S_un.S_un_b.s_b2	/* host on imp */
#define s_net	S_un.S_un_b.s_b1	/* network */
#define s_imp	S_un.S_un_w.s_w2	/* imp */
#define s_impno S_un.S_un_b.s_b4	/* imp # */
#define s_lh	S_un.S_un_b.s_b3	/* logical host */
};
/*
 * Socket address, internet style.
 */
struct sockaddr_in {
	short	sin_family;
	unsigned short sin_port;
	struct	in_addr sin_addr;
	char	sin_zero[8];
};
char * inet_ntoa(struct in_addr in);

#include <sys/uio.h>
#endif

#include <stdio.h>
#include <unixio.h>
#include <time.h>
#include <socket.h>
#include <stdlib.h>
#ifndef __GNUC__
#include <in.h>
#endif
#include <signal.h>
#include <string.h>
#include <errno.h>

/*
 * Common include
 */
#include "[-]socketprv.h"

/* #define FTRACE__ stdout */ /**/
#include "[-]si_trace.h"

int isSocket(int s);
int get_fd(FILE *fptr);

FILE *PACK(fdopen(int s, char *a_mode));

#ifndef VAXC
 /* GCC and DECC can use prototypes */
/*---------------------------------*/
#if 0
short int PACK(get_sdc(int s));
#endif
int PACK(socket(int domain, int type, int protocol));
int PACK(bind(int s, struct sockaddr * name, int namelen));
int PACK(connect(int s, struct sockaddr * name, int namelen));
int PACK(listen(int s, int backlog));
int PACK(accept(int s, struct sockaddr * addr, int *addrlen));
int PACK(recv(int s, char * buf, int len, int flags));
int PACK(recvfrom(int s, char *buf, int len, int flags, \
	struct sockaddr *from, int * fromlen));
int PACK(send(int s, char *msg, int len, int flags));
int PACK(sendto(int s, char *msg, int len, int flags, \
	struct sockaddr *to, int tolen));
int PACK(shutdown(int s, int how));
int PACK(select(int nfds, int *readfds, int *writefds, \
	int *execptfds, struct timeval *timeout));
int PACK(getsockname(int s, struct sockaddr *name, int *namelen));
int PACK(getpeername(int s, struct sockaddr *name, int *namelen));
int PACK(getsockopt(int s, int level, int optname, char *optval, int *optlen));
int PACK(setsockopt(int s, int level, int optname, char *optval, int optlen));
int PACK(gethostname(char * name, int namelen));
struct servent * socket_getservbyname(char * name, char * prot);
struct servent * socket_getservbyport(int port, char * prot);
struct servent * socket_getservent(void);
struct protoent * socket_getprotobyname(char * name);
struct protoent * socket_getprotobynumber(int prot);
struct protoent * socket_getprotoent(void);
int socket_setprotoent(int stayopen);
int socket_endprotoent(void);
struct hostent * PACK(gethostbyname(char * name));
struct hostent * PACK(gethostbyaddr(char * addr, int len, int type));
int PACK(read(int s, void *buf, int l));
int PACK(ioctl(int s, long req, void *argp));
int PACK(fcntl(int s, int cmd, int arg));
int socket_setservent(int stayopen);
int socket_endservent(void);
unsigned long PACK(close(int s));
unsigned long PACK(write(int s, void * buffer, int nbytes));
unsigned long PACK(writeev(int s, struct iovec *iov, int iovcnt));
#endif

#ifndef VAXC
#define noshare
#endif

noshare FILE* ftrace__ = (FILE *)NULL;
noshare int trace__ = 0;

#ifndef __DECC_VER
noshare int h_errno = 0;
#endif

#ifdef __DECC_VER
char * __inet_ntoa(struct in_addr __in);
#endif

/*
 * Locally used variables.
 */
static struct sockaddr_in *saddr;
static struct hostent *inet_host;
static struct servent *inet_serv;
static struct protoent *inet_proto;

/*
 * Routines used only with the socket library.
 */

/*
 *	short int si_get_sdc(int s)
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
 *			char *optval, int optlen)
 *
 *	int gethostname(char *name, int namelen)
 *	struct servent *getservbyname(char *name, char *proto)
 *	struct servent *getservbyport(int port, char *proto)
 *	struct servent *getservent(void)
 *	int setservent(int stayopen)
 *	int endservent(void)
 *	struct protoent *getprotobyname(char *name)
 *	struct protoent *getprotobynumber(int proto)
 *	struct protoent *getprotoent(void)
 *	int setprotoent(int stayopen)
 *	int endprotoent(void)
 *	struct hostent *gethostbyname(char *name)
 *	struct hostent *gethostbyaddr(char *addr, int len, int type)
 *	struct hostent *gethostent(void)	[dummy]
 *	int sethostent(int stayopen)		[dummy]
 *	int endhostent(void)			[dummy]
 */

/*
 * ======================================
 * si_get_sdc get VMS channel
 * ======================================
 */

short int
si_get_sdc(int s)
{
	STRACE("si_get_sdc",0,s,"",0,0);
	return( PACK(get_sdc(s)) );
}


/*
 * ======================================
 * socket
 * ======================================
 */
int
si_socket(int domain, int type, int protocol)
{
	int s;
	DTRACE("si_socket",0,"domain: %d, type: %d",domain,type);
	s = PACK(socket(domain, type, protocol));
#ifdef UCX
	ucx_socket_fptr(s);
#endif
	STRACE("si_socket",0,s,"domain: %d, type: %d",domain,type);
	return(s);
}
/*
 * ======================================
 * bind
 * ======================================
 */
int
si_bind(int s, struct sockaddr *name, int namelen)
{
	int i;
	saddr = (struct sockaddr_in *)name;
	STRACE("si_bind",0,s,"addr: %s, port: %d",
		inet_ntoa(saddr->sin_addr),
			   ntohs(saddr->sin_port));
/*	return( PACK(bind(s, name, namelen)) ); */
	i = PACK(bind(s, name, namelen));
	DTRACE("si_bind",0,"return: %d",i,0);
	return(i);
}
/*
 * ======================================
 * connect
 * ======================================
 */
int
si_connect(int s, struct sockaddr *name, int namelen)
{
	saddr = (struct sockaddr_in *)name;
	STRACE("si_connect",0,s,"addr: %s, port: %d",
		inet_ntoa(saddr->sin_addr),
		ntohs(saddr->sin_port));
	return (PACK(connect(s, name, namelen)) );
}
/*
 * ======================================
 * listen
 * ======================================
 */
int
si_listen (int s, int backlog)
{
	STRACE("si_listen",0,s,"backlog: %d",backlog,0);
	return( PACK(listen(s, backlog)) );
}
/*
 * ======================================
 * accept
 * ======================================
 */
int
si_accept(int s, struct sockaddr *addr, int *addrlen)
{
	int i;
	STRACE("si_accept",0,s,"",0,0);
	i = PACK(accept(s, addr, addrlen));
	saddr = (struct sockaddr_in *)addr;
	DTRACE("si_accept",0,"returned: addr: %s, port: %d",
		inet_ntoa(saddr->sin_addr), ntohs(saddr->sin_port));
	return(i);
}
/*
 * ======================================
 * recv
 * ======================================
 */
int
si_recv(int s, char *buf, int len, int flags)
{
	int i;
	STRACE("si_recv",0,s,"%d bytes",len,0);
	i = PACK(recv(s, buf, len, flags));
	DTRACE("si_recv",0,"received: %d bytes",i,0);
	DMPHX(0, buf, i);
	return(i);
}
/*
 * ======================================
 * recvfrom
 * ======================================
 */
int
si_recvfrom(int s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{
	int i;
	STRACE("si_recvfrom",0,s,"%d bytes",len,0);
	i = PACK(recvfrom(s, buf, len, flags, from, fromlen));
#ifdef TRACE
	DTRACE("si_recvfrom",0,"received: %d bytes",i,0);
	if (from != (struct sockaddr *)0) {
		saddr = (struct sockaddr_in *)from;
		DTRACE("  --",0,"returned: addr: %s, port: %d",
			inet_ntoa(saddr->sin_addr), ntohs(saddr->sin_port));
	}
	DMPHX(0, buf, i);
#endif
	return(i);
}
/*
 * ======================================
 * send
 * ======================================
 */
int
si_send(int s, char *msg, int len, int flags)
{
	STRACE("si_send",0,s,"%d bytes",len,0);
	DMPHX(0, msg, len);
	return( PACK(send(s, msg, len, flags)) );
}
/*
 * ======================================
 * sendto
 * ======================================
 */
int
si_sendto(int s, char *msg, int len, int flags, struct sockaddr *to, int tolen)
{
	STRACE("si_sendto",0,s,"%d bytes",len,0);
#ifdef TRACE
	if (to != (struct sockaddr *)0) {
		saddr = (struct sockaddr_in *)to;
		DTRACE("  --",0,"To: addr: %s, port: %d",
			inet_ntoa(saddr->sin_addr),
			ntohs(saddr->sin_port));
	}
	DMPHX(0, msg, len);
#endif
	return( PACK(sendto(s, msg, len, flags, to, tolen)) );
}
/*
 * ======================================
 * shutdown
 * ======================================
 */
int
si_shutdown(int s, int how)
{
	STRACE("si_shutdown",0,s,"how: %d",how,0);
	return( PACK(shutdown(s, how)) );
}
/*
 * ======================================
 * select
 * ======================================
 */
int
si_select(int nfds, int *readfds, int *writefds, int *execptfds,
			struct timeval *timeout)
{
	DTRACE("si_select",0,"",0,0);
	if (timeout != NULL) {
		DTRACE("  --",0,"Timeout: %d sec, %d usec",
			timeout->tv_sec,timeout->tv_usec);
	}
	return( PACK(select(nfds, readfds, writefds, execptfds, timeout)) );
}
/*
 * ======================================
 * getsockname
 * ======================================
 */
int
si_getsockname(int s, struct sockaddr *name, int *namelen)
{
	int i;
	STRACE("si_getsockname",0,s,"",0,0);
	i = PACK(getsockname(s, name, namelen));
	saddr = (struct sockaddr_in *)name;
	DTRACE("si_getsockname",0,"-- Local  Port: %d, Local  Addr: %s",
		ntohs(saddr->sin_port),
		inet_ntoa(saddr->sin_addr));
	return(i);
}
/*
 * ======================================
 * getpeername
 * ======================================
 */
int
si_getpeername(int s, struct sockaddr *name, int *namelen)
{
	int i;
	STRACE("si_getpeername",0,s,"",0,0);
	i = PACK(getpeername(s, name, namelen));
	saddr = (struct sockaddr_in *)name;
	DTRACE("si_getpeername",0,"-- Remote Port: %d, Remote Addr: %s",
		ntohs(saddr->sin_port),
		inet_ntoa(saddr->sin_addr));
	return(i);
}
/*
 * ======================================
 * getsockopt
 * ======================================
 */
int
si_getsockopt(int s, int level, int optname, char *optval, int *optlen)
{
	STRACE("si_getsockopt",0,s,"opt: %d",optname,0);
	return( PACK(getsockopt(s, level, optname, optval, optlen)) );
}
/*
 * ======================================
 * setsockopt
 * ======================================
 */
int
si_setsockopt(int s, int level, int optname, char *optval, int optlen)
{
	STRACE("si_setsockopt",0,s,"opt: %d",optname,0);
	return( PACK(setsockopt(s, level, optname, optval, optlen)) );
}
/*
 * ======================================
 * gethostname
 * ======================================
 */
int
si_gethostname(char *name, int namelen)
{
	int i;
	DTRACE("si_gethostname",0,"",0,0);
	i = PACK(gethostname(name, namelen));
	DTRACE("si_gethostname",0,"returned name: %s",name,0);
	return(i);
}
/*
 * ======================================
 * getservbyname
 * ======================================
 */
struct servent *
si_getservbyname(char *name, char *prot)
{
#if 0 /* !defined(__DECC) && defined(UCX) */ /**/
/* Here is a very mysterious thing: When calling 'getservbyname' in VAXC/UCX,
 * a any DTRACE in this routine (even after the call) crashes the program.
 * I've not the slightest idea what happens. The work around is not to
 * use any of the trace routines. Another bug is that UCX returns -1 instead
 * of 0 if the name is not found.
 */
	inet_serv = PACK(getservbyname(name, prot));
	if (inet_serv == (struct servent *)-1) inet_serv = (struct servent *)0;
#else
	DTRACE("si_getservbyname",0,"name: %s",name,0);
	if (prot != NULL) {
		DTRACE("  --",0,"protocol: %s",prot,0);
	}
/*	inet_serv = PACK(getservbyname(name, prot)); */ /**/
	inet_serv = socket_getservbyname(name, prot); /**/
#ifdef TRACE
	if (inet_serv != (struct servent *)0) {
	char **pp;
		DTRACE("si_getservbyname",0,"returned name: %s, port: %d",
			inet_serv->s_name,ntohs(inet_serv->s_port));
		DTRACE("  --",0,"returned protocol: %s", inet_serv->s_proto,0);
		for (pp=inet_serv->s_aliases; *pp; pp++) {
			DTRACE("  --",0,"alias: %s",*pp,0);
		}
	}
#endif
#endif
	return(inet_serv);
}
/*
 * ======================================
 * getservbyport
 * ======================================
 */
struct servent *
si_getservbyport(int port, char *prot)
{
	DTRACE("si_getservbyport",0,"port: %d",port,0);
	if (prot != NULL) {
		DTRACE("  --",0,"protocol: %s",prot,0);
	}
/*	inet_serv = PACK(getservbyport(port, prot)); */ /**/
	inet_serv = socket_getservbyport(port, prot); /**/
#ifdef TRACE
	if (inet_serv != (struct servent *)NULL) {
	char **pp;
		DTRACE("si_getservbyport",0,"returned name: %s, port: %d",
			inet_serv->s_name,ntohs(inet_serv->s_port));
		DTRACE("  --",0,"returned protocol: %s", inet_serv->s_proto,0);
		for (pp=inet_serv->s_aliases; *pp; pp++) {
			DTRACE("  --",0,"alias: %s",*pp,0);
		}
	}
#endif
	return(inet_serv);
}
/*
 * ======================================
 * getservent
 * ======================================
 */
struct servent *
si_getservent(void)
{
	DTRACE("si_getservent",0,"",0,0);
	inet_serv = socket_getservent();
#ifdef TRACE
	if (inet_serv != (struct servent *)NULL) {
	char **pp;
		DTRACE("si_getservent",0,"returned name: %s, port: %d",
			inet_serv->s_name,ntohs(inet_serv->s_port));
		DTRACE("  --",0,"returned protocol: %s", inet_serv->s_proto,0);
		for (pp=inet_serv->s_aliases; *pp; pp++) {
			DTRACE("  --",0,"alias: %s",*pp,0);
		}
	}
#endif
	return(inet_serv);
}
/*
 * ======================================
 * setservent
 * ======================================
 */
int
si_setservent(int stayopen)
{
	DTRACE("si_setservent",0,"stayopen: %d",stayopen,0);
	return(socket_setservent(stayopen));
}
/*
 * ======================================
 * endservent
 * ======================================
 */
int
si_endservent(void)
{
	DTRACE("si_endservent",0,"",0,0);
	return(socket_endservent());
}
/*
 * ======================================
 * getprotobyname
 * ======================================
 */
struct protoent *
si_getprotobyname(char *name)
{
	DTRACE("si_getprotobyname",0,"name: %s",name,0);
/*	inet_proto = PACK(getprotobyname(name)); */ /**/
	inet_proto = socket_getprotobyname(name); /**/
#ifdef TRACE
	if (inet_proto != (struct protoent *)NULL) {
	char **pp;
		DTRACE("si_getprotobyname",0,"returned name: %s, protocol: %d",
			inet_proto->p_name,inet_proto->p_proto);
		for (pp=inet_proto->p_aliases; *pp; pp++) {
			DTRACE("  --",0,"alias: %s",*pp,0);
		}
	}
#endif
	return(inet_proto);
}
/*
 * ======================================
 * getprotobynumber
 * ======================================
 */
struct protoent *
si_getprotobynumber(int prot)
{
	DTRACE("si_getprotobynumber",0,"protocol: %d",prot,0);
/*	inet_proto = PACK(getprotobynumber(prot)); */ /**/
	inet_proto = socket_getprotobynumber(prot); /**/
#ifdef TRACE
	if (inet_proto != (struct protoent *)NULL) {
	char **pp;
		DTRACE("si_getprotobynumber",0,"returned name: %s, protocol: %d",
			inet_proto->p_name,inet_proto->p_proto);
		for (pp=inet_proto->p_aliases; *pp; pp++) {
			DTRACE("  --",0,"alias: %s",*pp,0);
		}
	}
#endif
	return(inet_proto);
}
/*
 * ======================================
 * getprotoent
 * ======================================
 */
struct protoent *
si_getprotoent(void)
{
	DTRACE("si_getprotoent",0,"",0,0);
	inet_proto = socket_getprotoent();
#ifdef TRACE
	if (inet_proto != (struct protoent *)NULL) {
	char **pp;
		DTRACE("si_getprotoent",0,"returned name: %s, protocol: %d",
			inet_proto->p_name,inet_proto->p_proto);
		for (pp=inet_proto->p_aliases; *pp; pp++) {
			DTRACE("  --",0,"alias: %s",*pp,0);
		}
	}
#endif
	return(inet_proto);
}
/*
 * ======================================
 * setprotoent
 * ======================================
 */
int
si_setprotoent(int stayopen)
{
	DTRACE("si_setprotoent",0,"stayopen: %d",stayopen,0);
	return(socket_setprotoent(stayopen));
}
/*
 * ======================================
 * endprotoent
 * ======================================
 */
int
si_endprotoent(void)
{
	DTRACE("si_endprotoent",0,"",0,0);
	return(socket_endprotoent());
}
/*
 * ======================================
 * gethostbyname
 * ======================================
 */
struct hostent *
si_gethostbyname(char *name)
{
	DTRACE("si_gethostbyname",0,"name: %s",name,0);
	inet_host = PACK(gethostbyname(name));
#ifdef TRACE
	if (inet_host != (struct hostent *)NULL) {
	char **pp;
		DTRACE("si_gethostbyname",0,"returned name: %s, addr: %s",
			inet_host->h_name,INTOA(inet_host->h_addr));
		for (pp=inet_host->h_aliases; *pp; pp++) {
			DTRACE("  --",0,"alias: %s",*pp,0);
		}
	}
#endif
	return(inet_host);
}
/*
 * ======================================
 * gethostbyaddr
 * ======================================
 */
struct hostent *
si_gethostbyaddr(char *addr, int len, int type)
{
	DTRACE("si_gethostbyaddr",0,"addr: %08X",*((long *)addr),0);
	inet_host = PACK(gethostbyaddr(addr, len, type));
#ifdef TRACE
	if (inet_host != (struct hostent *)NULL) {
	char **pp;
		DTRACE("si_gethostbyaddr",0,"returned name: %s, addr: %s",
			inet_host->h_name,INTOA(inet_host->h_addr));
		for (pp=inet_host->h_aliases; *pp; pp++) {
			DTRACE("  --",0,"alias: %s",*pp,0);
		}
	}
#endif
	return(inet_host);
}
/*
 * ======================================
 * gethostent
 * ======================================
 */
struct hostent *
si_gethostent(void)
{
	DTRACE("si_gethostent",0,"--not supported--",0,0);
	return((struct hostent *)0);
}
/*
 * ======================================
 * sethostent
 * ======================================
 */
int
si_sethostent(int stayopen)
{
	DTRACE("si_sethostent",0,"stayopen: %d, --not supported--",stayopen,0);
	return(0);
}
/*
 * ======================================
 * endhostent
 * ======================================
 */
int
si_endhostent(void)
{
	DTRACE("si_endhostent",0,"--not supported--",0,0);
	return(0);
}

/*
 * Routines shared with unix I/O.
 *
 *	int close(int s)
 *	int read(int s, char *buf, int len)
 *	int write(int s, char *msg, int len, int flags)
 */

/*
 * ======================================
 * si_close
 * ======================================
 */
int
si_close(int s)
{
	STRACE("si_close",0,s,"",0,0);
#ifdef UCX
	ucx_close_fptr(s);
#endif
	return( isSocket(s) ? PACK(close(s)) : close(s) );
}

/*
 * ======================================
 * si_write
 * ======================================
 */
int
si_write (int s, void *buf, int l)
{
	STRACE("si_write",0,s,"%d bytes",l,0);
	DMPHX(0, buf, l);
	return( isSocket(s) ? PACK(write(s, buf, l)) : write(s, buf, l) );
}

/*
 * ======================================
 * si_read
 * ======================================
 */
int
si_read (int s, void *buf, int l)
{
	int i;
	STRACE("si_read",0,s,"%d bytes",l,0);
	if ( !isSocket(s) ) return(read(s, buf, l));
	i = PACK(read(s, buf, l));
	STRACE("si_read",0,s,"return - %d bytes received, errno = %d",i,errno);
	DMPHX(0, buf, i);
	return(i);
}

/*
 * Routines that should work with non-socket fd's, but are
 * still not present in the RTL (not even in DEC-C).
 */

/*
 * ======================================
 * si_writev
 * ======================================
 */


int
si_writev (int s, struct iovec *iov, int iovcnt)
{
	int i;
	int k;
	int cnt = 0;

	STRACE("si_writev",0,s,"",0,0);
	for(i=0;i<iovcnt;i++) {
/* fprintf(stderr,"  %d: len = %d\n{%s}\n",i,iov[i].iov_len,iov[i].iov_base); */
		k = (isSocket(s) ?
			si_send(s, iov[i].iov_base, iov[i].iov_len, 0) :
			write(s, iov[i].iov_base, iov[i].iov_len) );
		if (k == -1) return(-1);
		cnt += k;
	}
	return(cnt);
}

/*
 * ======================================
 * si_ioctl
 * ======================================
 */
int
si_ioctl(int s, long req, void *argp)
{
	STRACE("ioctl",0,s,"request: %d",req,0);
	return( isSocket(s) ? PACK(ioctl(s, req, argp)) : -1 );
}

/*
 * ======================================
 * si_fcntl
 * ======================================
 */
int
si_fcntl(int s, int cmd, int arg)
{
	STRACE("si_fcntl",0,s,"cmd: %d, arg: %d",cmd,arg);
	return( isSocket(s) ? PACK(fcntl(s, cmd, arg)) : -1 );
}

/*
 * ======================================
 * si_fdopen
 * ======================================
 */
FILE *
si_fdopen (int s, char *a_mode)
{
	STRACE("si_fdopen",0,s,"mode: %s",a_mode,0);
	return( isSocket(s) ? PACK(fdopen(s, a_mode)) : fdopen(s, a_mode) );
}

/*
 * Now the standard I/O functions. Shared with file standard I/O.
 *
 *	int fileno(FILE * fptr)
 *	int fread(char *buf, int nsize, int num, FILE *fptr)
 *	int fwrite(char *buf, int nsize, int num, FILE *fptr)
 *	int fgetc(FILE *fptr)
 *	char *fgets(char *buf, int num, FILE *fptr)
 *	int fputc(int c, FILE *fptr)
 *	int fputs(char *buf, FILE *fptr)
 *	int fclose(FILE *fptr)
 *	int rewind(FILE *fptr)
 *	int fflush(FILE *fptr)
 *	int fprintf(FILE *fptr, char *fmt, ...)
 *
 *	Note that feof() and ferror() are implemented through macros.
 */

/*
 * ======================================
 * si_fileno
 * ======================================
 */
int
si_fileno(FILE * fptr)
{
	int s;
	FTRACE("si_fileno",0,fptr,"",0,0);
	if ( (s=get_fd(fptr)) != -1) return(s);
	return(fileno(fptr));
}

/*
 * ======================================
 * si_fread
 * ======================================
 */
int
si_fread(char *buf, int nsize, int num, FILE *fptr)
{
	int s;
	int i;

	FTRACE("si_fread",0,fptr,"%d units of %d bytes",num,nsize);
	if ( (s=get_fd(fptr)) == -1) return( fread(buf, nsize, num, fptr) );
	i = si_recv(s,buf,nsize*num,0);
	return( (i>0) ? i : 0 );
}


/*
 * ======================================
 * si_fwrite
 * ======================================
 */
int
si_fwrite(char *buf, int nsize, int num, FILE *fptr)
{
	int s;
	int i;

	FTRACE("si_fwrite",0,fptr,"%d units of %d bytes",num,nsize);
	if ( (s=get_fd(fptr)) == -1) return( fwrite(buf, nsize, num, fptr) );
	i = si_send(s,buf,nsize*num,0);
	return( (i>0) ? i : 0 );
}
/*
 * ======================================
 * si_fgetc
 * ======================================
 */
int
si_fgetc(FILE *fptr)
{
	int s;
	char cbuf[1];

	FTRACE("si_fgetc",0,fptr,"",0,0);
	if ( (s=get_fd(fptr)) == -1) return( fgetc(fptr) );
        if (si_recv(s,cbuf,1,0) <= 0) return(EOF);
	return((int)((*cbuf) & 0xFF));
}

/*
 * ======================================
 * si_fgets
 * ======================================
 */
char *
si_fgets(char *buf, int num, FILE *fptr)
{
	int s, i;
	char cbuf[1], *t;

	FTRACE("si_fgets",0,fptr,"%d bytes max",num,0);
/*
 * we need to read a character string from the socket which is
 * terminated by a nl. Do this the hard way by reading 1 char
 * at a time and testing it.
 */
	if ( (s=get_fd(fptr)) == -1) return( fgets(buf,num,fptr) );
	t = buf;
	for (i=1;i<num;i++) {
		if (si_recv(s,cbuf,1,0) <= 0) {
			return(NULL);
		}
		*t++ = cbuf[0];
		if (cbuf[0] == '\n') break;
	}
	*t = '\0';
	return(buf);
}

/*
 * ======================================
 * si_fputc
 * ======================================
 */
int
si_fputc(int c, FILE *fptr)
{
	int s;
	char ch = c;

	FTRACE("si_fputc",0,fptr,"char: {%c} (0x%02X)",c,c);
	if ( (s=get_fd(fptr)) == -1) return( fputc(c,fptr) );
	return( si_send(s, &ch, 1, 0) );
}

/*
 * ======================================
 * si_fputs
 * ======================================
 */
int
si_fputs(char *buf, FILE *fptr)
{
	int s;
	int num;

	num = strlen(buf);
	FTRACE("si_fputs",0,fptr,"%d bytes",num,0);
	if ( (s=get_fd(fptr)) == -1) return( fputs(buf,fptr) );
	return( si_send(s,buf,num,0) );
}


/*
 * ======================================
 * si_fclose
 * ======================================
 */
int
si_fclose(FILE *fptr)
{
	int s;

	FTRACE("si_fclose",0,fptr,"",0,0);
	if ( (s=get_fd(fptr)) == -1) return( fclose(fptr) );
	return( si_close(s) );
}

/*
 * ======================================
 * si_rewind
 * ======================================
 */
int
si_rewind(FILE *fptr)
{
	int s;

	FTRACE("si_rewind",0,fptr,"",0,0);
/* don't do anything if its one of ours */
	if ( (s=get_fd(fptr)) == -1) return( rewind(fptr) );
	return(0);
}

/*
 * ======================================
 * si_fflush
 * ======================================
 */
int
si_fflush(FILE *fptr)
{
	int s;

	FTRACE("si_fflush",0,fptr,"",0,0);
/* don't do anything if its one of ours */
	if ( (s=get_fd(fptr)) == -1) return( fflush(fptr) );
	return(0);
}

/*
 * ======================================
 * si_fprintf
 * ======================================
 */
int
si_fprintf(fptr,format,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16)
FILE *fptr;
char *format;
int *a1,*a2,*a3,*a4,*a5,*a6,*a7,*a8,*a9,*a10,*a11,*a12,*a13,*a14,*a15,*a16;
{
	char buf[1024];
	int s;

	FTRACE("si_fprintf",0,fptr,"format: {%s}",format,0);
/*
 * this is not pretty but how else can we handle variable number of
 * arguments except declare lots of them, no checking is done if we
 * supply too few. Of course if more than 16 are passed we get into
 * awful trouble.
 */
	if ( (s=get_fd(fptr)) == -1) {
		return(fprintf(fptr,format,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16));
	}
	sprintf(buf,format,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16);
	return( si_send(s,buf,strlen(buf),0) );
}

/*
 * Unsupported functions.
 * These functions have entry points for future use of them.
 *
 *	void signal(int sig, void (*func)() )
 *	int sigvec(int sig, struct sigvec *vec, struct sigvec *ovec)
 *	int alarm(int i)
 */

/*
 * ======================================
 * si_signal
 * ======================================
 */
void
si_signal(int sig, void (*func)(int))
{
	DTRACE("si_signal",0,"%d",sig,0);
	signal(sig,func);
}

/*
 * ======================================
 * si_sigvec
 * ======================================
 */
int
si_sigvec(int sig, struct sigvec *vec, struct sigvec *ovec)
{
	DTRACE("si_sigvec",0,"%d",sig,0);
	return(sigvec(sig,vec,ovec));
}

/*
 * ======================================
 * si_alarm
 * ======================================
 */
int
si_alarm(int i)
{
	DTRACE("si_alarm",0,"",0,0);
	return(alarm(i));
}

/*
 * Conversion routines.
 *
 *	unsigned long si_inet_addr(char *str)
 *	char *si_inet_ntoa(struct in_addr in)
 *	unsigned long int si_ntohl(unsigned long int x)
 *	unsigned short int si_ntohs(unsigned short int x)
 *	unsigned long int si_htonl(unsigned long int x)
 *	unsigned short int si_htons(unsigned short int x)
 */

unsigned long
si_inet_addr(char *str)
{
	XTRACE("inet_addr",0,"",0,0);
	return( inet_addr(str) );
}

char *
si_inet_ntoa(struct in_addr in)
{
char * chptr;

	XTRACE("inet_ntoa",0,"",0,0);
	chptr = inet_ntoa(in);
	return chptr;
}

unsigned long int
si_ntohl(unsigned long int x)
{
	XTRACE("ntohl",0,"",0,0);
	return( ntohl(x) );
}

unsigned short int
si_ntohs(unsigned short int x)
{
	XTRACE("ntohs",0,"",0,0);
	return( ntohs(x) );
}

unsigned long int
si_htonl(unsigned long int x)
{
	XTRACE("htonl",0,"",0,0);
	return( htonl(x) );
}

unsigned short int
si_htons(unsigned short int x)
{
	XTRACE("htons",0,"in: %d %08X",x,x);
	return( htons(x) );
}


#ifdef TRACE
static int trace_flag = 0;
static char *timestr = NULL;
/************************************************************/
/* trace_open						    */
/************************************************************/
void _trace_open(char *str)
{
	time_t bintim;
	char *p;
	char *logfile = NULL;

	time(&bintim);
	timestr = ctime(&bintim) + 11;
	timestr[8] = '\0';
	if (trace_flag == 0) {
		p = getenv("SOCKETSHR_DEBUG");
		if (p != NULL) {
			trace__ = atoi(p);
		}
		p = getenv("SOCKETSHR_LOGFILE");
		if (p != NULL) {
			logfile = p;
		}
		trace_flag = 1;
	}
	if ( (trace__ != 0) && (FTRACE__ == (FILE*)NULL) ) {
		if (logfile != NULL) {
			ftrace__ = fopen(logfile,"w");
		}
		else {
			ftrace__ = stdout;
		}
		fprintf(ftrace__,"\nOpened by %s\n\n",str);
	}
}
/************************************************************/
/* strace - debug output with socket number		    */
/************************************************************/
void _strace(char *str, int flag, int s, char *fmt, char *a1, char *a2)
{
	_trace_open(str);
	if ( (trace__ > 0 && isSocket(s)) || (trace__ == -1) || flag) {
		fprintf(FTRACE__,"%s %s: s=%d ",timestr,str,s);
		fprintf(FTRACE__,fmt,a1,a2);
		fprintf(FTRACE__,"\n");
	}
}
/************************************************************/
/* ftrace - debug output with file pointer		    */
/************************************************************/
void _ftrace(char *str, int flag, FILE *fptr, char *fmt, char *a1, char *a2)
{
	_trace_open(str);
	if ( (trace__ > 0 && (get_fd(fptr) != -1)) || (trace__ == -1) || flag ) {
		fprintf(FTRACE__,"%s %s: fptr=%08X ",timestr,str,fptr);
		fprintf(FTRACE__,fmt,a1,a2);
		fprintf(FTRACE__,"\n");
	}
}
/************************************************************/
/* dtrace - debug output without socket or file pointer ;-) */
/************************************************************/
void _dtrace(char *str, int flag, char *fmt, char *a1, char *a2)
{
	_trace_open(str);
	if ( (trace__ > 0) || (trace__ == -1) || flag) {
		fprintf(FTRACE__,"%s %s ",timestr,str);
		fprintf(FTRACE__,fmt,a1,a2);
		fprintf(FTRACE__,"\n");
	}
}

/************************************************************/
/* dmphx - dump a buffer to the debug log in hex and ascii  */
/************************************************************/
void
_dmphx(FILE *f, char *msg, int len)
{
  int i, j;
  unsigned char *v, *w;

  v = w = (unsigned char *)msg;
  j = len;
  while (j>0) {
     if (j>16) i = 16; else i=j;
     for(;i>0;i--,v++) fprintf(f," %02x",*v);
     if (j<16) for(i=16-j;i>0;i--) fprintf(f,"   ");
     fprintf(f,"        ");
     if (j>16) i = 16; else i=j;
     for(;i>0;i--,w++) fprintf(f,"%c",(*w>0x1f && *w<0x80)? *w : '.');
     fprintf(f,"\n");
     j -= 16;
  }
}

#endif	/* TRACE */

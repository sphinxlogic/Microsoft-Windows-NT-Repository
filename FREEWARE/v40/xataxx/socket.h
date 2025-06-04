/*
**  FILE:   Header <socket.h> for Inet socket support
**  IDENT:  X-8
**
*****************************************************************************
**									    *
**  Copyright © Digital Equipment Corporation, 1992, 1993	    	    *
**  All Rights Reserved.  Unpublished rights reserved under  the  copyright *
**  laws of the United States.						    *
**									    *
**  The software contained on this media is proprietary to and embodies the *
**  confidential  technology of Digital Equipment Corporation.  Possession, *
**  use,  duplication  or  dissemination  of  the  software  and  media  is *
**  authorized  only  pursuant  to  a  valid  written  license from Digital *
**  Equipment Corporation.						    *
**									    *
**  RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure by the  U.S. *
**  Government  is  subject  to  restrictions  as set forth in Subparagraph *
**  (c)(1)(ii) of DFARS 252.227-7013, or in FAR 52.227-19, as applicable.   *
**									    *
*****************************************************************************
*/

#ifndef __SOCKET_LOADED
#define __SOCKET_LOADED 1

# define _NGROUPS_MAX_		8
# define _TMP_MAX_		10000
# define _OPEN_MAX_		64
# define _STREAM_MAX_		64
# define _TZNAME_MAX_		10
# define _LINK_MAX_		32767
# define _MAX_CANON_		511
# define _MAX_INPUT_		511
# define _NAME_MAX_		255
# define _PATH_MAX_		(1023+1)
# define _PIPE_BUF_		4096
# define _LINE_MAX_		2048
# define _BC_BASE_MAX_		SHRT_MAX
# define _BC_DIM_MAX_		(SHRT_MAX+1)
# define _BC_SCALE_MAX_		SHRT_MAX
# define _BC_STRING_MAX_	_LINE_MAX_
# define _EQUIV_CLASS_MAX_	2
# define _EXPR_NEST_MAX_	200
# define _RE_DUP_MAX_		2048
# define _MAX_NICE_		31
# define _MIN_NICE_		-32
# define _NZERO_	        32
# define _PASS_MAX_		31
# define _NL_ARGMAX_		9
# define _NL_LANGMAX_		39
# define _NL_MSGMAX_		32767
# define _NL_SETMAX_		255
# define _NL_TEXTMAX_		255
# define _NL_NMAX_		32
# define _EVTCLASS_MIN_		0
# define _EVTCLASS_MAX_		31
# define _EVTCLASS_SIG_		32
# define _DELAYTIMERMAX_	INT_MAX
# define _AIO_PRIO_DFL_		0
# define _AIO_PRIO_MIN_		0
# define _AIO_PRIO_MAX_		0
# define _AIO_LISTIO_MAX_SIZE_	10

#ifdef __cplusplus
extern "C" {
#endif

# if __DECC || __DECCXX
#   pragma __nostandard
#   pragma __member_alignment __save
#   pragma __nomember_alignment
# elif VAXC
#   pragma nostandard
# endif

#ifndef __SOCKET_TYPEDEFS
#define __SOCKET_TYPEDEFS 1

typedef unsigned short u_short;

typedef unsigned long u_long;

typedef unsigned char u_char;

#endif

/*
 * Definitions related to sockets: types, address families, options.
 */

/*
 * Types
 */
#define	SOCK_STREAM	1		/* stream socket */
#define	SOCK_DGRAM	2		/* datagram socket */
#define	SOCK_RAW	3		/* raw-protocol interface */
#define	SOCK_RDM	4		/* reliably-delivered message */
#define	SOCK_SEQPACKET	5		/* sequenced packet stream */

/*
 * Option flags per-socket.
 */
#define	SO_DEBUG	0x01		/* turn on debugging info recording */
#define	SO_ACCEPTCONN	0x02		/* socket has had listen() */
#define	SO_REUSEADDR	0x04		/* allow local address reuse */
#define	SO_KEEPALIVE	0x08		/* keep connections alive */
#define	SO_DONTROUTE	0x10		/* just use interface addresses */
#define	SO_BROADCAST	0x20		/* permit sending of broadcast msgs */
#define	SO_USELOOPBACK	0x40		/* bypass hardware when possible */
#define	SO_LINGER	0x80		/* linger on close if data present */
#define SO_OOBINLINE	0x100		/* leave received OOB data in line */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF	0x1001		/* send buffer size */
#define SO_RCVBUF	0x1002		/* receive buffer size */
#define SO_SNDLOWAT	0x1003		/* send low-water mark */
#define SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define SO_SNDTIMEO	0x1005		/* send timeout */
#define SO_RCVTIMEO	0x1006		/* receive timeout */
#define SO_ERROR	0x1007		/* get error status and clear */
#define SO_TYPE		0x1008		/* get socket type */

/*
 * Structure used for manipulating linger option.
 */
struct	linger {
	int	l_onoff;		/* option on/off */
	int	l_linger;		/* linger time */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define	SOL_SOCKET	0xffff		/* options for socket level */

/*
 * Address families.
 */
#define	AF_UNSPEC	0		/* unspecified */
#define	AF_UNIX		1		/* local to host (pipes, portals) */
#define	AF_INET		2		/* internetwork: UDP, TCP, etc. */
#define	AF_IMPLINK	3		/* arpanet imp addresses */
#define	AF_PUP		4		/* pup protocols: e.g. BSP */
#define	AF_CHAOS	5		/* mit CHAOS protocols */
#define	AF_NS		6		/* XEROX NS protocols */
#define	AF_NBS		7		/* nbs protocols */
#define	AF_ECMA		8		/* european computer manufacturers */
#define	AF_DATAKIT	9		/* datakit protocols */
#define	AF_CCITT	10		/* CCITT protocols, X.25 etc */
#define	AF_SNA		11		/* IBM SNA */
#define AF_DECnet	12		/* DECnet */
#define AF_DLI		13		/* Direct data link interface */
#define AF_LAT		14		/* LAT */
#define	AF_HYLINK	15		/* NSC Hyperchannel */
#define AF_APPLETALK	16		/* Apple talk */
#define AF_BSC		17		/* BISYNC 2780/3780 */
#define AF_DSS		18		/* Distributed system services */

#define	AF_MAX		19

/*
 * Structure used by kernel to store most
 * addresses.
 */
struct sockaddr {
	u_short	sa_family;		/* address family */
	char	sa_data[14];		/* up to 14 bytes of direct address */
};

/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct sockproto {
	u_short	sp_family;		/* address family */
	u_short	sp_protocol;		/* protocol */
};

/*
 * Protocol families, same as address families for now.
 */
#define	PF_UNSPEC	AF_UNSPEC
#define	PF_UNIX		AF_UNIX
#define	PF_INET		AF_INET
#define	PF_IMPLINK	AF_IMPLINK
#define	PF_PUP		AF_PUP
#define	PF_CHAOS	AF_CHAOS
#define	PF_NS		AF_NS
#define	PF_NBS		AF_NBS
#define	PF_ECMA		AF_ECMA
#define	PF_DATAKIT	AF_DATAKIT
#define	PF_CCITT	AF_CCITT
#define	PF_SNA		AF_SNA
#define PF_DECnet	AF_DECnet
#define PF_DLI		AF_DLI
#define PF_LAT		AF_LAT
#define	PF_HYLINK	AF_HYLINK
#define PF_APPLETALK	AF_APPLETALK
#define PF_BSC		AF_BSC
#define PF_DSS		AF_DSS

#define	PF_MAX		AF_MAX

/*
 * Maximum queue length specifiable by listen.
 */
#define	SOMAXCONN	5

/*
 * I/O buffer element.
 */
struct iovec {
	caddr_t	iov_base;
	int	iov_len;
};

/*
 * Message header for recvmsg and sendmsg calls.
 */
struct msghdr {
	caddr_t	msg_name;		/* optional address */
	int	msg_namelen;		/* size of address */
	struct	iovec *msg_iov;		/* scatter/gather array */
	int	msg_iovlen;		/* # elements in msg_iov */
	caddr_t	msg_accrights;		/* access rights sent/received */
	int	msg_accrightslen;
};

#define	MSG_OOB		0x1		/* process out-of-band data */
#define	MSG_PEEK	0x2		/* peek at incoming message */
#define	MSG_DONTROUTE	0x4		/* send without using routing tables */

#define	MSG_MAXIOVLEN	16

/*  From Ultrix's time.h */
struct timeval {
    long tv_sec;
    long tv_usec;
};

/*  FD_XXX macros are similar to Ultrix types.h ones */

/*
 * Select uses bit masks of file descriptors in "int" (32 bits).
 * These macros manipulate such bit fields (the filesystem macros use chars).
 */
typedef int    	fd_mask;
#define NFDBITS (sizeof(fd_mask) * 8)	/* (byte number in an int) * (bit number in a byte) = (32 bits) */
typedef struct fd_set {
        fd_mask fds_bits[(((_OPEN_MAX_)+((NFDBITS)-1))/(NFDBITS))];
} fd_set;

#define FD_SET(n, p)    ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)    ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)      bzero((char *)(p), sizeof(*(p)))

/* Function prototypes */

int socket	(int __af, int __mess_type, int __prot_type);
int accept	(int __sd, struct sockaddr * __S_addr, int * __addrlen);
int bind	(int __sd, const struct sockaddr * __name, int __namelen );
int listen	(int __sd, int __backlog);
int connect	(int __sd, const struct sockaddr * __name, int __namelen);
int send	(int __sd, const char * __buf, int __nbyte, int __flags);
int sendto	(int __sd, const char * __buf, int __nbyte, int __flags, 
			const struct sockaddr * __to, int __tolen);
int sendmsg	(int __sd, const struct msghdr * __msg, int __flags);
int recv	(int __sd, char * __buf, int __nbyte, int __flags);
int recvfrom	(int __sd, char * __buf, int __nbyte, int __flags, 
			struct sockaddr * __from, int * __fromlen);
int recvmsg	(int __sd, struct msghdr * __msg, int __flags);
int shutdown	(int __sd, int __mode);
int select	(int __nfds, fd_set * __readfds, fd_set * __writefds, 
		    fd_set * __exceptfds, const struct timeval * __timeout);
int getpeername	(int __sd, struct sockaddr * __name, int * __namelen);
int getsockname	(int __sd, struct sockaddr * __name, int * __namelen);
int getsockopt	(int __sd, int __level, int __optname, char * __optval, 
			int * __optlen);
int setsockopt	(int __sd, int __level, int __optname, const char * __optval, 
			int __optlen);

int gethostname	(char * __name, int __namelen);
int gethostaddr	(char * __addr);

int bcopy	(const char * __str1, char * __str2, int __len);
int bcmp	(const char * __str1, const char * __str2, int __len);
int bzero	(char * __bufptr, int __buflength);

# if __DECC || __DECCXX
#   pragma __member_alignment __restore
#   pragma __standard
# elif VAXC
#   pragma standard
# endif

#ifdef __cplusplus
}
#endif

#endif					/* __SOCKET_LOADED */


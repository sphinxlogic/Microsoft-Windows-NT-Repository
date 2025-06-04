#ifndef __TYPES_LOADED

/*
 *  <types.h> - RTL Typedef Definitions
 *
 *  This file is distributed with SOCKETSHR. It replaces VMS' TYPES.H
 *  (which contains only the definition of time_t) to add the
 *  bit field macros which are used with select().
 */


#include <sys/types.h>
/* #define __TYPES_LOADED	1 */

#ifdef __cplusplus
    extern "C" {
#endif

/* #ifndef __TIME_T */
#ifndef __TYPES_LOADED
# define __TIME_T 1
  typedef unsigned long int	time_t;
#endif

#define makedev yes	/* this is not a proper definition, however it is used
			 * to test if types is loaded.
			 */

#ifndef __DEV_T
typedef unsigned int uid_t;
typedef unsigned int gid_t;
#endif

#ifndef __SOCKET_TYPEDEFS
#ifndef CADDR_T
#define CADDR_T
typedef char * caddr_t;
#endif
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
typedef unsigned char	u_char;
#define __SOCKET_TYPEDEFS
#endif


/*
 *  VAX C does not define these at all
 *  DEC C defines them in socket.h; protected via __FD_SET
 */


#ifndef __FD_SET

#if defined(VAXC) && !defined(__DECC)
#define __FD_SET 1
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */


#define MAX_NOFILE	32
#define	NBBY		 8		/* number of bits in a byte	*/

#ifndef	FD_SETSIZE
#define	FD_SETSIZE	MAX_NOFILE
#endif	/* FD_SETSIZE */

/* How many things we'll allow select to use. 0 if unlimited */
#define MAXSELFD	MAX_NOFILE
typedef int	fd_mask;	/* int here! VMS prototypes int, not long */
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask (power of 2!)*/
#define NFDSHIFT 5				/* Shift based on above */

#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif /* howmany */

/*
 * fd setup for:
 *	- VAXCRTL and DEC TCP/IP Services for OpenVMS
 *	- LIBCMU and CMU-OpenVMS/IP v6.6
 * Both use one longword bitmask for 32 file descriptors.
 * Note: Wollongong WIN/TCP is different...  DEFINE WINTCP for WIN/TCP support
 */
#ifndef WINTCP
typedef	fd_mask fd_set;

#define	FD_SET(n, p)	(*(p) |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	(*(p) &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	(*(p) & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	memset((char *)(p), 0, sizeof(*(p)))
#endif

#ifdef WINTCP
/*
 * fd setup for Wollongong's WIN/TCP
 */
typedef	unsigned short fd_channel;

typedef	struct fd_set {
	fd_mask		fds_bits[howmany(FD_SETSIZE, NFDBITS)];
	fd_channel	fds_chan[howmany(FD_SETSIZE, NFDBITS)][NFDBITS];
} fd_set;

#define	FD_SET(n, p)	_$fdset(n, p)
#define	FD_CLR(n, p)	_$fdclr(n, p)
#define	FD_ISSET(n, p)	(n == 0 ? n : _$fdisset(n, p))
#define FD_ZERO(p)	_$fdzero(p)

#endif /* WINTCP */

#ifndef __FD_SET
#define fd_set "fd_set not defined in header file sys/types.h"
#endif
#endif /* VAXC */

#endif /* __FD_SET */


#ifdef __cplusplus
    }
#endif

#endif					/* __TYPES_LOADED */

/*	@(#)types.h 2.22 88/01/13 SMI; from UCB 7.1 6/4/86	*/

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ifndef	_UNIX_TYPES_
#define	_UNIX_TYPES_

/*
 * Basic system types.
 */

/*
This is not relevant on VMS:
#include <sys/sysmacros.h>
*/

#include <time.h>
#include <types.h>

#ifndef  __SOCKET_TYPEDEFS
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned long	u_long;
#define __SOCKET_TYPEDEFS 1
#endif

#ifndef __U_INT
typedef	unsigned int	u_int;
typedef	unsigned short	ushort;		/* System V compatibility */
typedef	unsigned int	uint;		/* System V compatibility */
#endif

typedef	struct	_physadr { int r[1]; } *physadr;
typedef	struct	label_t	{
	int	val[14];
} label_t;

typedef	struct	_quad { long val[2]; } quad;
typedef	long	daddr_t;

/* This appears elsewhere, e.g., in X Windows include files */
#if !defined(CADDR_T) && !defined(__CADDR_T)
typedef	char	*caddr_t;
#define CADDR_T
#define __CADDR_T
#endif

#ifndef __STAT
#include <stat.h>
#endif /* __STAT */
typedef	long	swblk_t;


/* For VMS we need to avoid a couple of multiple definitions */

#if !defined (__TYPES) && !defined (__TIME_T) && !defined (__TYPES_LOADED)
/* TYPES.H	*/
/*	TYPES - RTL Typedef Definitions 	*/
typedef long int time_t;
#define __TIME_T
#endif

#ifndef __STDDEF
/* STDDEF.H	*/
#if defined(vax11c) || defined(__DECC)
#include <stddef.h>
#else
#define __STDDEF
typedef int ptrdiff_t;
typedef int size_t;
#define NULL		(void *) 0
#define offsetof( type, identifier)			\
     	((size_t)(&((type*) NULL)->identifier))
extern volatile int noshare errno;	/* UNIX style error code */
#endif /* vax11c */
#endif


#ifndef __DEV_T                   /* This started to appear with DECC 5.0 */
typedef	u_short	uid_t;
typedef	u_short	gid_t;
typedef int pid_t;
typedef	long	key_t;
#endif

#define	NBBY	8		/* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	256
#endif

#ifndef __FD_SET
#ifndef _FD_MASK_
#define _FD_MASK_
typedef	long	fd_mask;
#endif /*_FD_MASK_*/

#define	NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */
#ifndef	howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

typedef	char *	addr_t;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define	FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))

#endif

#if !defined(F_OK) || !defined(X_OK) || !defined(W_OK) || !defined(R_OK)
#define	F_OK	0
#define	X_OK	1
#define	W_OK	2
#define R_OK	4
#endif 

#define MAXPATHLEN	256
#endif	/*_UNIX_TYPES_*/

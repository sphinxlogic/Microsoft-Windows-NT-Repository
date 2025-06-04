/*
 *	This is the UNIX-compatible Types definition
 */

#ifndef _TYPES_
#define	_TYPES_
/*
 * Basic system types
 */
#ifndef  __SOCKET_TYPEDEFS
#define  __SOCKET_TYPEDEFS
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
#ifndef __DECC
typedef	unsigned long	u_long;
#endif /* __DECC */
#endif

#ifdef __DECC
#ifdef __DECC_VER
#if (__DECC_VER < 50200000)
typedef  unsigned int   u_int;
#endif /* __DECC_VER < 50200000 , that is DEC C 5.0 or 5.1 */
#else    /* DEC C 4.0 , __DECC_VER is not defined */
typedef  unsigned int   u_int;
#endif /* __DECC_VER */
#endif /* __DECC */

#ifndef __DECC
typedef	unsigned short	ushort;		/* sys III compat */
#endif /* __DECC */
typedef	struct	_quad { long val[2]; } quad;
typedef	long	daddr_t;
#ifndef CADDR_T
#define CADDR_T
#ifndef __DECC
typedef	char *	caddr_t;
#endif /* __DECC */
#endif

#ifndef __DECC
typedef	u_short	ino_t;
typedef	char	*dev_t;
typedef	unsigned off_t;
#endif /* __DECC */

typedef	long	swblk_t;
#ifndef _SIZE_T
#define _SIZE_T
#ifndef __DECC
typedef	unsigned int	size_t;
#endif /* __DECC */
#endif
#ifndef	TYPES_H_DEFINED
#ifndef __TIME_T
# define __TIME_T 1

/*
 *  <types.h> - VAX C RTL Typedef Definitions
 */


  typedef unsigned long int     time_t;


#endif /* __TIME_T */
 
/* #include <types.h>	 This gets us time_t    */
#endif	/* TYPES_H_DEFINED */

#ifndef __DECC
typedef	u_short	uid_t;
typedef	u_short	gid_t;
#endif /* __DECC */

#define	NBBY	8		/* number of bits in a byte */

#endif /*	_TYPES_   */

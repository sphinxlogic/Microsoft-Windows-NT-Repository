#define	HAVE_STDLIB_H
#define	HAVE_UNISTD_H
#define	HAVE_STRING_H
#define	HAVE_MEMSET
#define	HAVE_MEMCPY
#define HAVE_MEMMOVE
#define	HAVE_STRERROR
#define	HAVE_FCNTL_H
#define	HAVE_STRDUP
#define	HAVE_STRPBRK
#define	HAVE_STRSPN
#define	HAVE_STRCSPN
#define	HAVE_STRTOUL
#define	HAVE_STRCASECMP
#define	HAVE_STRCASECMP
#define	HAVE_ATEXIT

/*
** 13-Mar-2000	Sergey Tikhonov
**	Added defines specific to OpenVMS
*/

#ifdef VMS

#define SYSCONFDIR "sys$login:"
#define inline	   /* */

#define	USE_CLI

#endif

/* Emulate getpagesize on systems that lack it.  */

#ifndef HAVE_GETPAGESIZE

#ifdef VMS

#include <syidef.h>
#ifdef SYI$_PAGE_SIZE
extern int getpagesize ();
extern int getpagelog ();
#else
#define getpagelog() 9
#define getpagesize() (1<<(getpagelog()))
#endif /* SYI$_PAGE_SIZE */

#else /* not VMS */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _SC_PAGESIZE
#define getpagesize() sysconf(_SC_PAGESIZE)
#else

#include <sys/param.h>

#ifdef EXEC_PAGESIZE
#define getpagesize() EXEC_PAGESIZE
#else
#ifdef NBPG
#define getpagesize() NBPG * CLSIZE
#ifndef CLSIZE
#define CLSIZE 1
#endif /* no CLSIZE */
#else /* no NBPG */
#ifdef NBPC
#define getpagesize() NBPC
#endif /* NBPC */
#endif /* no NBPG */
#endif /* no EXEC_PAGESIZE */
#endif /* no _SC_PAGESIZE */

#endif /* VMS */

#endif /* not HAVE_GETPAGESIZE */

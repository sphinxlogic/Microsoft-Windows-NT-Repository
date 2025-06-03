/* term.h - defs for handling termio inclusion - painful and getting worse */

/* Written by: Don Libes, NIST, 2/6/90

Design and implementation of this program was paid for by U.S. tax
dollars.  Therefore it is public domain.  However, the author and NIST
would appreciate credit if this program or parts of it are used.
*/

#ifdef SYSV3
#define INCLUDE_TERMIO
#if defined(CRAY) && CRAY>=60
#undef INCLUDE_TERMIO
#define INCLUDE_TERMIOS
#endif
#endif

#ifdef POSIX
#include <termios.h>
#endif

#ifdef INCLUDE_TERMIO
#include <sys/termio.h>
#endif

#ifdef INCLUDE_TERMIOS
#include <sys/termios.h>
#endif

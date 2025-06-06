/*

    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999,2000 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    support.c - Define missing function
                Written by Masanao Izumo <mo@goice.co.jp>
*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <stdarg.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifndef NO_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#ifdef __W32__
#include <windows.h>
#endif /* __W32__ */

#ifdef VMS
#include <sys/time.h>
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif /* HAVE_SYS_PARAM_H */
#include <ctype.h>

#include "timidity.h"
#include "mblock.h"
#ifdef __MACOS__
#include <Threads.h>
#endif

#ifndef HAVE_VSNPRINTF
/* From glib-1.1.13:gstrfuncs.c
 * Modified by Masanao Izumo <mo@goice.co.jp>
 */
static int printf_string_upper_bound (const char* format,
			     va_list      args)
{
  int len = 1;

  while (*format)
    {
      int long_int = 0;
      int extra_long = 0;
      char c;
      
      c = *format++;
      
      if (c == '%')
	{
	  int done = 0;
	  
	  while (*format && !done)
	    {
	      switch (*format++)
		{
		  char *string_arg;
		  
		case '*':
		  len += va_arg (args, int);
		  break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		  /* add specified format length, since it might exceed the
		   * size we assume it to have.
		   */
		  format -= 1;
		  len += strtol (format, (char**) &format, 10);
		  break;
		case 'h':
		  /* ignore short int flag, since all args have at least the
		   * same size as an int
		   */
		  break;
		case 'l':
		  if (long_int)
		    extra_long = 1; /* linux specific */
		  else
		    long_int = 1;
		  break;
		case 'q':
		case 'L':
		  long_int = 1;
		  extra_long = 1;
		  break;
		case 's':
		  string_arg = va_arg (args, char *);
		  if (string_arg)
		    len += strlen (string_arg);
		  else
		    {
		      /* add enough padding to hold "(null)" identifier */
		      len += 16;
		    }
		  done = 1;
		  break;
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
#ifdef	G_HAVE_GINT64
		  if (extra_long)
		    (void) va_arg (args, gint64);
		  else
#endif	/* G_HAVE_GINT64 */
		    {
		      if (long_int)
			(void) va_arg (args, long);
		      else
			(void) va_arg (args, int);
		    }
		  len += extra_long ? 64 : 32;
		  done = 1;
		  break;
		case 'D':
		case 'O':
		case 'U':
		  (void) va_arg (args, long);
		  len += 32;
		  done = 1;
		  break;
		case 'e':
		case 'E':
		case 'f':
		case 'g':
#ifdef HAVE_LONG_DOUBLE
		  if (extra_long)
		    (void) va_arg (args, long double);
		  else
#endif	/* HAVE_LONG_DOUBLE */
		    (void) va_arg (args, double);
		  len += extra_long ? 64 : 32;
		  done = 1;
		  break;
		case 'c':
		  (void) va_arg (args, int);
		  len += 1;
		  done = 1;
		  break;
		case 'p':
		case 'n':
		  (void) va_arg (args, void*);
		  len += 32;
		  done = 1;
		  break;
		case '%':
		  len += 1;
		  done = 1;
		  break;
		default:
		  /* ignore unknow/invalid flags */
		  break;
		}
	    }
	}
      else
	len += 1;
    }

  return len;
}

void vsnprintf(char *buff, size_t bufsiz, const char *fmt, va_list ap)
{
    MBlockList pool;
    char *tmpbuf = buff;

    init_mblock(&pool);
    tmpbuf = new_segment(&pool, printf_string_upper_bound(fmt, ap));
    vsprintf(tmpbuf, fmt, ap);
    strncpy(buff, tmpbuf, bufsiz);
    reuse_mblock(&pool);
}
#endif /* HAVE_VSNPRINTF */


#ifndef HAVE_SNPRINTF
void snprintf(char *buff, size_t bufsiz, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buff, bufsiz, fmt, ap);
    va_end(ap);
}
#endif /* HAVE_VSNPRINTF */

#ifndef VMS
#ifndef HAVE_GETOPT
/*
	Copyright (c) 1986,1992 by Borland International Inc.
	All Rights Reserved.
*/
#ifdef HAVE_DOS_H
#include <dos.h>
#endif /* HAVE_DOS_H */

int	optind	= 1;	/* index of which argument is next	*/
char   *optarg;		/* pointer to argument of current option */
int	opterr	= 1;	/* allow error message	*/

static	char   *letP = NULL;	/* remember next option char's location */

#if 0
static	char	SW = 0;		/* DOS switch character, either '-' or '/' */
#endif
#define SW '-' /* On Win32 can't call DOS! */
/*
  Parse the command line options, System V style.

  Standard option syntax is:

    option ::= SW [optLetter]* [argLetter space* argument]

  where
    - SW is either '/' or '-', according to the current setting
      of the MSDOS switchar (int 21h function 37h).
    - there is no space before any optLetter or argLetter.
    - opt/arg letters are alphabetic, not punctuation characters.
	 - optLetters, if present, must be matched in optionS.
    - argLetters, if present, are found in optionS followed by ':'.
    - argument is any white-space delimited string.  Note that it
      can include the SW character.
    - upper and lower case letters are distinct.

  There may be multiple option clusters on a command line, each
  beginning with a SW, but all must appear before any non-option
  arguments (arguments not introduced by SW).  Opt/arg letters may
  be repeated: it is up to the caller to decide if that is an error.

  The character SW appearing alone as the last argument is an error.
  The lead-in sequence SWSW ("--" or "//") causes itself and all the
  rest of the line to be ignored (allowing non-options which begin
  with the switch char).

  The string *optionS allows valid opt/arg letters to be recognized.
  argLetters are followed with ':'.  Getopt () returns the value of
  the option character found, or EOF if no more options are in the
  command line.	 If option is an argLetter then the global optarg is
  set to point to the argument string (having skipped any white-space).

  The global optind is initially 1 and is always left as the index
  of the next argument of argv[] which getopt has not taken.  Note
  that if "--" or "//" are used then optind is stepped to the next
  argument before getopt() returns EOF.

  If an error occurs, that is an SW char precedes an unknown letter,
  then getopt() will return a '?' character and normally prints an
  error message via perror().  If the global variable opterr is set
  to false (zero) before calling getopt() then the error message is
  not printed.

  For example, if the MSDOS switch char is '/' (the MSDOS norm) and

    *optionS == "A:F:PuU:wXZ:"

  then 'P', 'u', 'w', and 'X' are option letters and 'F', 'U', 'Z'
  are followed by arguments.  A valid command line may be:

    aCommand  /uPFPi /X /A L someFile

  where:
    - 'u' and 'P' will be returned as isolated option letters.
    - 'F' will return with "Pi" as its argument string.
    - 'X' is an isolated option.
    - 'A' will return with "L" as its argument.
    - "someFile" is not an option, and terminates getOpt.  The
      caller may collect remaining arguments using argv pointers.
*/

int	getopt(int argc, char *argv[], char *optionS)
{
	unsigned char ch;
	char *optP;

#if 0
	if (SW == 0) {
		/* get SW using dos call 0x37 */
		_AX = 0x3700;
		geninterrupt(0x21);
		SW = _DL;
	}
#endif
	if (argc > optind) {
		if (letP == NULL) {
			if ((letP = argv[optind]) == NULL ||
				*(letP++) != SW)  goto gopEOF;
			if (*letP == SW) {
				optind++;  goto gopEOF;
			}
		}
		if (0 == (ch = *(letP++))) {
			optind++;  goto gopEOF;
		}
		if (':' == ch  ||  (optP = strchr(optionS, ch)) == NULL)
			goto gopError;
		if (':' == *(++optP)) {
			optind++;
			if (0 == *letP) {
				if (argc <= optind)  goto  gopError;
				letP = argv[optind++];
			}
			optarg = letP;
			letP = NULL;
		} else {
			if (0 == *letP) {
				optind++;
				letP = NULL;
			}
			optarg = NULL;
		}
		return ch;
	}
gopEOF:
	optarg = letP = NULL;
	return EOF;

gopError:
        if (argc > optind)
                optind++;
        optarg = letP = NULL;
	errno  = EINVAL;
	if (opterr)
		perror ("get command line option");
	return ('?');
}
#endif /* HAVE_GETOPT */


#ifndef HAVE_STRERROR
#ifndef HAVE_ERRNO_H
char *strerror(int errnum) {
    static char s[32];
    sprintf(s, "ERROR %d", errnum);
    return s;
}
#else

char *strerror(int errnum)
{
    int i;
    static char s[32];
    struct {
	int id;
	char *str;
    } error_list[] = {
#ifdef EPERM
    {EPERM, "Not super-user"},
#endif /* EPERM */
#ifdef ENOENT
    {ENOENT, "No such file or directory"},
#endif /* ENOENT */
#ifdef ESRCH
    {ESRCH, "No such process"},
#endif /* ESRCH */
#ifdef EINTR
    {EINTR, "interrupted system call"},
#endif /* EINTR */
#ifdef EIO
    {EIO, "I/O error"},
#endif /* EIO */
#ifdef ENXIO
    {ENXIO, "No such device or address"},
#endif /* ENXIO */
#ifdef E2BIG
    {E2BIG, "Arg list too long"},
#endif /* E2BIG */
#ifdef ENOEXEC
    {ENOEXEC, "Exec format error"},
#endif /* ENOEXEC */
#ifdef EBADF
    {EBADF, "Bad file number"},
#endif /* EBADF */
#ifdef ECHILD
    {ECHILD, "No children"},
#endif /* ECHILD */
#ifdef EAGAIN
    {EAGAIN, "Resource temporarily unavailable"},
#endif /* EAGAIN */
#ifdef EWOULDBLOCK
    {EWOULDBLOCK, "Resource temporarily unavailable"},
#endif /* EWOULDBLOCK */
#ifdef ENOMEM
    {ENOMEM, "Not enough core"},
#endif /* ENOMEM */
#ifdef EACCES
    {EACCES, "Permission denied"},
#endif /* EACCES */
#ifdef EFAULT
    {EFAULT, "Bad address"},
#endif /* EFAULT */
#ifdef ENOTBLK
    {ENOTBLK, "Block device required"},
#endif /* ENOTBLK */
#ifdef EBUSY
    {EBUSY, "Mount device busy"},
#endif /* EBUSY */
#ifdef EEXIST
    {EEXIST, "File exists"},
#endif /* EEXIST */
#ifdef EXDEV
    {EXDEV, "Cross-device link"},
#endif /* EXDEV */
#ifdef ENODEV
    {ENODEV, "No such device"},
#endif /* ENODEV */
#ifdef ENOTDIR
    {ENOTDIR, "Not a directory"},
#endif /* ENOTDIR */
#ifdef EISDIR
    {EISDIR, "Is a directory"},
#endif /* EISDIR */
#ifdef EINVAL
    {EINVAL, "Invalid argument"},
#endif /* EINVAL */
#ifdef ENFILE
    {ENFILE, "File table overflow"},
#endif /* ENFILE */
#ifdef EMFILE
    {EMFILE, "Too many open files"},
#endif /* EMFILE */
#ifdef ENOTTY
    {ENOTTY, "Inappropriate ioctl for device"},
#endif /* ENOTTY */
#ifdef ETXTBSY
    {ETXTBSY, "Text file busy"},
#endif /* ETXTBSY */
#ifdef EFBIG
    {EFBIG, "File too large"},
#endif /* EFBIG */
#ifdef ENOSPC
    {ENOSPC, "No space left on device"},
#endif /* ENOSPC */
#ifdef ESPIPE
    {ESPIPE, "Illegal seek"},
#endif /* ESPIPE */
#ifdef EROFS
    {EROFS, "Read only file system"},
#endif /* EROFS */
#ifdef EMLINK
    {EMLINK, "Too many links"},
#endif /* EMLINK */
#ifdef EPIPE
    {EPIPE, "Broken pipe"},
#endif /* EPIPE */
#ifdef EDOM
    {EDOM, "Math arg out of domain of func"},
#endif /* EDOM */
#ifdef ERANGE
    {ERANGE, "Math result not representable"},
#endif /* ERANGE */
#ifdef ENOMSG
    {ENOMSG, "No message of desired type"},
#endif /* ENOMSG */
#ifdef EIDRM
    {EIDRM, "Identifier removed"},
#endif /* EIDRM */
#ifdef ECHRNG
    {ECHRNG, "Channel number out of range"},
#endif /* ECHRNG */
#ifdef EL2NSYNC
    {EL2NSYNC, "Level 2 not synchronized"},
#endif /* EL2NSYNC */
#ifdef EL3HLT
    {EL3HLT, "Level 3 halted"},
#endif /* EL3HLT */
#ifdef EL3RST
    {EL3RST, "Level 3 reset"},
#endif /* EL3RST */
#ifdef ELNRNG
    {ELNRNG, "Link number out of range"},
#endif /* ELNRNG */
#ifdef EUNATCH
    {EUNATCH, "Protocol driver not attached"},
#endif /* EUNATCH */
#ifdef ENOCSI
    {ENOCSI, "No CSI structure available"},
#endif /* ENOCSI */
#ifdef EL2HLT
    {EL2HLT, "Level 2 halted"},
#endif /* EL2HLT */
#ifdef EDEADLK
    {EDEADLK, "Deadlock condition."},
#endif /* EDEADLK */
#ifdef ENOLCK
    {ENOLCK, "No record locks available."},
#endif /* ENOLCK */
#ifdef ECANCELED
    {ECANCELED, "Operation canceled"},
#endif /* ECANCELED */
#ifdef ENOTSUP
    {ENOTSUP, "Operation not supported"},
#endif /* ENOTSUP */
#ifdef EDQUOT
    {EDQUOT, "Disc quota exceeded"},
#endif /* EDQUOT */
#ifdef EBADE
    {EBADE, "invalid exchange"},
#endif /* EBADE */
#ifdef EBADR
    {EBADR, "invalid request descriptor"},
#endif /* EBADR */
#ifdef EXFULL
    {EXFULL, "exchange full"},
#endif /* EXFULL */
#ifdef ENOANO
    {ENOANO, "no anode"},
#endif /* ENOANO */
#ifdef EBADRQC
    {EBADRQC, "invalid request code"},
#endif /* EBADRQC */
#ifdef EBADSLT
    {EBADSLT, "invalid slot"},
#endif /* EBADSLT */
#ifdef EDEADLOCK
    {EDEADLOCK, "file locking deadlock error"},
#endif /* EDEADLOCK */
#ifdef EBFONT
    {EBFONT, "bad font file fmt"},
#endif /* EBFONT */
#ifdef ENOSTR
    {ENOSTR, "Device not a stream"},
#endif /* ENOSTR */
#ifdef ENODATA
    {ENODATA, "no data (for no delay io)"},
#endif /* ENODATA */
#ifdef ETIME
    {ETIME, "timer expired"},
#endif /* ETIME */
#ifdef ENOSR
    {ENOSR, "out of streams resources"},
#endif /* ENOSR */
#ifdef ENONET
    {ENONET, "Machine is not on the network"},
#endif /* ENONET */
#ifdef ENOPKG
    {ENOPKG, "Package not installed"},
#endif /* ENOPKG */
#ifdef EREMOTE
    {EREMOTE, "The object is remote"},
#endif /* EREMOTE */
#ifdef ENOLINK
    {ENOLINK, "the link has been severed"},
#endif /* ENOLINK */
#ifdef EADV
    {EADV, "advertise error"},
#endif /* EADV */
#ifdef ESRMNT
    {ESRMNT, "srmount error"},
#endif /* ESRMNT */
#ifdef ECOMM
    {ECOMM, "Communication error on send"},
#endif /* ECOMM */
#ifdef EPROTO
    {EPROTO, "Protocol error"},
#endif /* EPROTO */
#ifdef EMULTIHOP
    {EMULTIHOP, "multihop attempted"},
#endif /* EMULTIHOP */
#ifdef EBADMSG
    {EBADMSG, "trying to read unreadable message"},
#endif /* EBADMSG */
#ifdef ENAMETOOLONG
    {ENAMETOOLONG, "path name is too long"},
#endif /* ENAMETOOLONG */
#ifdef EOVERFLOW
    {EOVERFLOW, "value too large to be stored in data type"},
#endif /* EOVERFLOW */
#ifdef ENOTUNIQ
    {ENOTUNIQ, "given log. name not unique"},
#endif /* ENOTUNIQ */
#ifdef EBADFD
    {EBADFD, "f.d. invalid for this operation"},
#endif /* EBADFD */
#ifdef EREMCHG
    {EREMCHG, "Remote address changed"},
#endif /* EREMCHG */
#ifdef ELIBACC
    {ELIBACC, "Can't access a needed shared lib."},
#endif /* ELIBACC */
#ifdef ELIBBAD
    {ELIBBAD, "Accessing a corrupted shared lib."},
#endif /* ELIBBAD */
#ifdef ELIBSCN
    {ELIBSCN, ".lib section in a.out corrupted."},
#endif /* ELIBSCN */
#ifdef ELIBMAX
    {ELIBMAX, "Attempting to link in too many libs."},
#endif /* ELIBMAX */
#ifdef ELIBEXEC
    {ELIBEXEC, "Attempting to exec a shared library."},
#endif /* ELIBEXEC */
#ifdef EILSEQ
    {EILSEQ, "Illegal byte sequence."},
#endif /* EILSEQ */
#ifdef ENOSYS
    {ENOSYS, "Unsupported file system operation"},
#endif /* ENOSYS */
#ifdef ELOOP
    {ELOOP, "Symbolic link loop"},
#endif /* ELOOP */
#ifdef ERESTART
    {ERESTART, "Restartable system call"},
#endif /* ERESTART */
#ifdef ESTRPIPE
    {ESTRPIPE, "if pipe/FIFO, don't sleep in stream head"},
#endif /* ESTRPIPE */
#ifdef ENOTEMPTY
    {ENOTEMPTY, "directory not empty"},
#endif /* ENOTEMPTY */
#ifdef EUSERS
    {EUSERS, "Too many users (for UFS)"},
#endif /* EUSERS */
#ifdef ENOTSOCK
    {ENOTSOCK, "Socket operation on non-socket"},
#endif /* ENOTSOCK */
#ifdef EDESTADDRREQ
    {EDESTADDRREQ, "Destination address required"},
#endif /* EDESTADDRREQ */
#ifdef EMSGSIZE
    {EMSGSIZE, "Message too long"},
#endif /* EMSGSIZE */
#ifdef EPROTOTYPE
    {EPROTOTYPE, "Protocol wrong type for socket"},
#endif /* EPROTOTYPE */
#ifdef ENOPROTOOPT
    {ENOPROTOOPT, "Protocol not available"},
#endif /* ENOPROTOOPT */
#ifdef EPROTONOSUPPORT
    {EPROTONOSUPPORT, "Protocol not supported"},
#endif /* EPROTONOSUPPORT */
#ifdef ESOCKTNOSUPPORT
    {ESOCKTNOSUPPORT, "Socket type not supported"},
#endif /* ESOCKTNOSUPPORT */
#ifdef EOPNOTSUPP
    {EOPNOTSUPP, "Operation not supported on socket"},
#endif /* EOPNOTSUPP */
#ifdef EPFNOSUPPORT
    {EPFNOSUPPORT, "Protocol family not supported"},
#endif /* EPFNOSUPPORT */
#ifdef EAFNOSUPPORT
    {EAFNOSUPPORT, "Address family not supported by"},
#endif /* EAFNOSUPPORT */
#ifdef EADDRINUSE
    {EADDRINUSE, "Address already in use"},
#endif /* EADDRINUSE */
#ifdef EADDRNOTAVAIL
    {EADDRNOTAVAIL, "Can't assign requested address"},
#endif /* EADDRNOTAVAIL */
#ifdef ENETDOWN
    {ENETDOWN, "Network is down"},
#endif /* ENETDOWN */
#ifdef ENETUNREACH
    {ENETUNREACH, "Network is unreachable"},
#endif /* ENETUNREACH */
#ifdef ENETRESET
    {ENETRESET, "Network dropped connection because"},
#endif /* ENETRESET */
#ifdef ECONNABORTED
    {ECONNABORTED, "Software caused connection abort"},
#endif /* ECONNABORTED */
#ifdef ECONNRESET
    {ECONNRESET, "Connection reset by peer"},
#endif /* ECONNRESET */
#ifdef ENOBUFS
    {ENOBUFS, "No buffer space available"},
#endif /* ENOBUFS */
#ifdef EISCONN
    {EISCONN, "Socket is already connected"},
#endif /* EISCONN */
#ifdef ENOTCONN
    {ENOTCONN, "Socket is not connected"},
#endif /* ENOTCONN */
#ifdef ESHUTDOWN
    {ESHUTDOWN, "Can't send after socket shutdown"},
#endif /* ESHUTDOWN */
#ifdef ETOOMANYREFS
    {ETOOMANYREFS, "Too many references: can't splice"},
#endif /* ETOOMANYREFS */
#ifdef ETIMEDOUT
    {ETIMEDOUT, "Connection timed out"},
#endif /* ETIMEDOUT */
#ifdef ECONNREFUSED
    {ECONNREFUSED, "Connection refused"},
#endif /* ECONNREFUSED */
#ifdef EHOSTDOWN
    {EHOSTDOWN, "Host is down"},
#endif /* EHOSTDOWN */
#ifdef EHOSTUNREACH
    {EHOSTUNREACH, "No route to host"},
#endif /* EHOSTUNREACH */
#ifdef EALREADY
    {EALREADY, "operation already in progress"},
#endif /* EALREADY */
#ifdef EINPROGRESS
    {EINPROGRESS, "operation now in progress"},
#endif /* EINPROGRESS */
#ifdef ESTALE
    {ESTALE, "Stale NFS file handle"},
#endif /* ESTALE */
    {0, NULL}};

    for(i = 0; error_list[i].str != NULL; i++)
	if(error_list[i].id == errnum)
	    return error_list[i].str;
    sprintf(s, "ERROR %d", errnum);
    return s;
}
#endif /* HAVE_ERRNO_H */
#endif /* HAVE_STRERROR */
#endif /* VMS */

#ifndef HAVE_USLEEP
#ifdef __W32__
int usleep(unsigned int usec)
{
    Sleep(usec / 1000);
    return 0;
}
#elif __MACOS__
int usleep(unsigned int /*usec*/)
{
    YieldToAnyThread();
    return 0;
}
#else
int usleep(unsigned int usec)
{
    struct timeval tv;
    tv.tv_sec  = usec / 1000000;
    tv.tv_usec = usec % 1000000;
    select(0, NULL, NULL, NULL, &tv);
    return 0;
}
#endif /* __W32__ */
#endif /* HAVE_USLEEP */

#ifndef VMS
#ifndef HAVE_STRDUP
char *strdup(const char *s)
{
    size_t len;
    char *p;

    len = strlen(s);
    if((p = (char *)malloc(len + 1)) == NULL)
	return NULL;
    return strcpy(p, s);
}
#endif /* HAVE_STRDUP */


#ifndef HAVE_GETCWD
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024	/* It must be defined in <sys/param.h> */
#endif /* MAXPATHLEN */

char *getcwd(char *buf, size_t size)
{
    char path[MAXPATHLEN+1];
    if(getwd(path) == NULL)
	strcpy(path, ".");
    if(buf != NULL)
	return strncpy(buf, path, size);
    return safe_strdup(path);
}
#endif /* HAVE_GETCWD */


#ifndef HAVE_STRNCASECMP
int strncasecmp(char *s1, char *s2, unsigned int len) {
  int dif;
  while (len-- > 0) {
	if ((dif =
		 (unsigned char)tolower(*s1) - (unsigned char)tolower(*s2++)) != 0)
	  return(dif);
	if (*s1++ == '\0')
	  break;
  }
  return (0);
}
#endif /* HAVE_STRNCASECMP */
#endif /* VMS */

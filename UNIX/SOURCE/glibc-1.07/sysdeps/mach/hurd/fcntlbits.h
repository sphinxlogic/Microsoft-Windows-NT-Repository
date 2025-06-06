/* O_*, F_*, FD_* bit values for GNU.
Copyright (C) 1993 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#ifndef	_FCNTLBITS_H

#define	_FCNTLBITS_H	1


/* File access modes.  These are understood by io servers; they can be
   passed in `dir_pathtrans', and are returned by `io_get_openmodes'.
   Consequently they can be passed to `open', `hurd_path_lookup', and
   `path_lookup'; and are returned by `fcntl' with the F_GETFL command.  */

/* In GNU, read and write are bits (unlike BSD).  */
#ifdef __USE_GNU
#define	O_READ		O_RDONLY /* Open for reading.  */
#define O_WRITE		O_WRONLY /* Open for writing.  */
#define	O_EXEC		0x0004	/* Open for execution.  */
#endif
/* POSIX.1 standard names.  */
#define	O_RDONLY	0x0001	/* Open read-only.  */
#define	O_WRONLY	0x0002	/* Open write-only.  */
#define	O_RDWR		(O_RDONLY|O_WRONLY) /* Open for reading and writing. */
#define	O_ACCMODE	O_RDWR	/* Mask for file access modes.  */



/* File name translation flags.  These are understood by io servers;
   they can be passed in `dir_pathtrans', and consequently to `open',
   `hurd_path_lookup', and `path_lookup'.  */

#define	O_CREAT		0x0010	/* Create file if it doesn't exist.  */
#define	O_EXCL		0x0020	/* Fail if file already exists.  */
#ifdef __USE_GNU
#define	O_NOLINK	0x0040	/* No name mappings on final component.  */
#define	O_NOTRANS	0x0080	/* No translator on final component. */
#endif


/* File status flags.  These are understood by io servers; they can be
   passed in `dir_pathtrans' and set or fetched with `io_*_openmodes'.
   Consequently they can be passed to `open', `hurd_path_lookup',
   `path_lookup', and `fcntl' with the F_SETFL command; and are returned
   by `fcntl' with the F_GETFL command.  */

#define	O_APPEND	0x0100	/* Writes always append to the file.  */
#define	O_ASYNC		0x0200	/* Send SIGIO to owner when data is ready.  */
#define	O_FSYNC		0x0400	/* Synchronous writes.  */
#define	O_SYNC		O_FSYNC
#ifdef __USE_GNU
#define	O_NOATIME	0x0800	/* Don't set access time on read by owner. */
#endif


/* The name O_NONBLOCK is unfortunately overloaded; it is both a file name
   translation flag and a file status flag.  O_NDELAY is the deprecated BSD
   name for the same flag, overloaded in the same way.

   When used in `dir_pathtrans' (and consequently `open', `hurd_path_lookup',
   or `path_lookup'), O_NONBLOCK says the open should fail with EAGAIN
   instead of blocking for any significant length of time (e.g., to wait for
   DTR on a serial line).

   When used in `io_*_openmodes' (and consequently `fcntl' with the F_SETFL
   command), the O_NONBLOCK flag means to do nonblocking i/o: any i/o
   operation that would block for any significant length of time will instead
   fail with EAGAIN.  */

#define	O_NONBLOCK	0x0008	/* Non-blocking open or non-blocking I/O.  */
#ifdef __USE_BSD
#define	O_NDELAY	O_NONBLOCK
#endif


#ifdef __USE_GNU
/* Mask of bits which are understood by io servers.  */
#define O_HURD		0xffff	/* XXX name? want this? */
#endif


/* Open-time action flags.  These are understood by `hurd_path_lookup'
   and consequently by `open' and `path_lookup'.  They are not preserved
   once the file has been opened.  */

#define	O_TRUNC		0x00010000 /* Truncate file to zero length.  */
#ifdef	__USE_MISC
#define	O_SHLOCK	0x00020000 /* Open with shared file lock.  */
#define	O_EXLOCK	0x00040000 /* Open with shared exclusive lock.  */
#endif


/* Controlling terminal flags.  These are understood only by `open',
   and are not preserved once the file has been opened.  */

#ifdef __USE_GNU
#define	O_IGNORE_CTTY	0x00080000 /* Don't do any ctty magic at all.  */
#endif
/* `open' never assigns a controlling terminal in GNU.  */
#define	O_NOCTTY	0	/* Don't assign a controlling terminal.  */


#ifdef __USE_BSD
/* Bits in the file status flags returned by F_GETFL.  */
#define FREAD		O_RDONLY
#define	FWRITE		O_WRONLY

/* Traditional BSD names the O_* bits.  */
#define FASYNC		O_ASYNC
#define FCREAT		O_CREAT
#define FEXCL		O_EXCL
#define FTRUNC		O_TRUNC
#define FNOCTTY		O_NOCTTY
#define FFSYNC		O_FSYNC
#define FSYNC		O_SYNC
#define FAPPEND		O_APPEND
#define FNONBLOCK	O_NONBLOCK
#define FNDELAY		O_NDELAY
#endif


/* Values for the second argument to `fcntl'.  */
#define	F_DUPFD	  	0	/* Duplicate file descriptor.  */
#define	F_GETFD		1	/* Get file descriptor flags.  */
#define	F_SETFD		2	/* Set file descriptor flags.  */
#define	F_GETFL		3	/* Get file status flags.  */
#define	F_SETFL		4	/* Set file status flags.  */
#ifdef __USE_BSD
#define	F_GETOWN	5	/* Get owner (receiver of SIGIO).  */
#define	F_SETOWN	6	/* Set owner (receiver of SIGIO).  */
#endif
#define	F_GETLK		7	/* Get record locking info.  */
#define	F_SETLK		8	/* Set record locking info (non-blocking).  */
#define	F_SETLKW	9	/* Set record locking info (blocking).  */

/* File descriptor flags used with F_GETFD and F_SETFD.  */
#define	FD_CLOEXEC	1	/* Close on exec.  */


#include <gnu/types.h>

/* The structure describing an advisory lock.  This is the type of the third
   argument to `fcntl' for the F_GETLK, F_SETLK, and F_SETLKW requests.  */
struct flock
  {
    int l_type;		/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.  */
    int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
    __off_t l_start;	/* Offset where the lock begins.  */
    __off_t l_len;	/* Size of the locked area; zero means until EOF.  */
    __pid_t l_pid;	/* Process holding the lock.  */
  };

/* Values for the `l_type' field of a `struct flock'.  */
#define	F_RDLCK	1	/* Read lock.  */
#define	F_WRLCK	2	/* Write lock.  */
#define	F_UNLCK	3	/* Remove lock.  */


#endif	/* fcntlbits.h */

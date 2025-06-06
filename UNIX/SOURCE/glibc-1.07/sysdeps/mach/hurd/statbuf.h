/* Copyright (C) 1992, 1993 Free Software Foundation, Inc.
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

#ifndef	_STATBUF_H

#define	_STATBUF_H	1

#include <gnu/types.h>

struct stat
  {
    int st_fstype;		/* File system type.  */
    __fsid_t st_fsid;		/* File system ID.  */
#define	st_dev	st_fsid

    __ino_t st_ino;		/* File number.  */
    unsigned int st_gen;	/* To detect reuse of file numbers.  */
    __dev_t st_rdev;		/* Device if special file.  */
    __mode_t st_mode;		/* File mode.  */
    __nlink_t st_nlink;		/* Number of links.  */

    __uid_t st_uid;		/* Owner.  */
    __gid_t st_gid;		/* Owning group.  */

    __off_t st_size;		/* Size in bytes.  */

    __time_t st_atime;		/* Access time, seconds */
    unsigned long int st_atime_usec; /* and microseconds.  */
    __time_t st_mtime;		/* Modification time, seconds */
    unsigned long int st_mtime_usec; /* and microseconds.  */
    __time_t st_ctime;		/* Status change time, seconds */
    unsigned long int st_ctime_usec; /* and microseconds.  */

    unsigned int st_blksize;	/* Optimal size for I/O.  */

#define	_STATBUF_ST_BLKSIZE	/* Tell code we have this member.  */

    unsigned int st_blocks;	/* Number of 512-byte blocks allocated.
				   Not related to `st_blksize'.  */

    __uid_t st_author;		/* File author.  */

    int st_spare[8];		/* Reserved for future use.  */
  };

/* Encoding of the file mode.  */

#define	__S_IFMT	0170000	/* These bits determine file type.  */

/* File types.  */
#define	__S_IFDIR	0040000	/* Directory.  */
#define	__S_IFCHR	0020000	/* Character device.  */
#define	__S_IFBLK	0060000	/* Block device.  */
#define	__S_IFREG	0100000	/* Regular file.  */
#define	__S_IFLNK	0120000	/* Symbolic link.  */
#define	__S_IFSOCK	0140000	/* Socket.  */
#define	__S_IFIFO	0010000	/* FIFO.  */

/* Protection bits.  */

#define	__S_ISUID	04000	/* Set user ID on execution.  */
#define	__S_ISGID	02000	/* Set group ID on execution.  */
#define	__S_ISVTX	01000	/* Save swapped text after use (sticky).  */
#define	__S_IREAD	0400	/* Read by owner.  */
#define	__S_IWRITE	0200	/* Write by owner.  */
#define	__S_IEXEC	0100	/* Execute by owner.  */


/* Default file creation mask (umask).  */
#ifdef	__USE_BSD
#define	CMASK		0022
#endif


#endif /* statbuf.h */

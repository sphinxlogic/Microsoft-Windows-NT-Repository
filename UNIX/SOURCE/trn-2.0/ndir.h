/*
 * This file was produced by running the ndir.h.SH script, which
 * gets its values from config.sh, which is generally produced by
 * running Configure.
 */
/* $Id: ndir.h.SH,v 1.2 92/01/11 16:05:37 usenet Exp $
 *
 * $Log:	ndir.h.SH,v $
 * Revision 1.2  92/01/11  16:05:37  usenet
 * header twiddling, bug fixes
 *
 * Revision 4.4  1991/09/09  20:23:31  sob
 * release 4.4
 *
 *
 * 
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */

#ifdef LIBNDIR
#   include <ndir.h>
#else
#   ifndef USENDIR
#	include <dirent.h>
#   else

#ifndef DEV_BSIZE
#define	DEV_BSIZE	512
#endif
#define DIRBLKSIZ	DEV_BSIZE
#define	MAXNAMLEN	255

struct	direct {
	long	d_ino;			/* inode number of entry */
	short	d_reclen;		/* length of this record */
	short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
};

/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in struct direct
 * without the d_name field, plus enough space for the name with a terminating
 * null byte (dp->d_namlen+1), rounded up to a 4 byte boundary.
 */
#undef DIRSIZ
#define DIRSIZ(dp)     ((sizeof (struct direct) - (MAXNAMLEN+1)) + (((dp)->d_namlen+1 + 3) &~ 3))

/*
 * Definitions for library routines operating on directories.
 */
typedef struct _dirdesc {
	int	dd_fd;
	long	dd_loc;
	long	dd_size;
	char	dd_buf[DIRBLKSIZ];
} DIR;
#ifndef NULL
#define NULL 0
#endif
extern	DIR *opendir ANSI((char *));
extern	struct direct *readdir ANSI((DIR *));
extern	long telldir ANSI((DIR *));
extern	void seekdir ANSI((DIR *));
#define rewinddir(dirp)	seekdir((dirp), (long)0)
extern	void closedir ANSI((DIR *));

#   endif
#endif

/* Copyright (C) 1992 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it without restriction.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  */

#include <stdio.h>

/* Values of special characters. */
#define TAB '\t'
#define EOL '\n'
#define BACKSLASH '\\'

#ifdef DEBUG
extern int debug;
#endif

#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif

#ifdef VMS
# define ONE_DOT_PER_FILENAME 1
# define NODIR 1
# define PROFILE_FORMAT "%s%s"
# define BACKUP_SUFFIX_STR    "_"
# define BACKUP_SUFFIX_CHAR   '_'
# define BACKUP_SUFFIX_FORMAT "%s._%d_"
# define SYS_READ vms_read	/* Defined in io.c */
# ifdef VAXC
#  include <unixio.h>
# endif
#endif /* VMS */

#ifdef __MSDOS__
#define ONE_DOT_PER_FILENAME 1
#define USG   1
#define NODIR 1
#endif /* __MSDOS__ */

/* configure defines USG if it can't find bcopy */

#ifndef USG
#define memcpy(dest,src,len) bcopy((src),(dest),len)
#endif

struct file_buffer
{
  char *name;
  unsigned long size;
  char *data;
};

extern struct file_buffer *read_file (), *read_stdin ();

/* Standard memory allocation routines.  */
char *malloc ();
char *realloc ();

/* Similar, but abort with an error if out of memory (see globs.c).  */
char *xmalloc ();
char *xrealloc ();

/*
 * config.h
 *
 * Edit this file to match your site.
 *
 * Copyright (c) 1991, John W. Eaton.
 *
 * You may distribute under the terms of the GNU General Public
 * License as specified in the README file that comes with the man 1.0
 * distribution.  
 *
 * John W. Eaton
 * jwe@che.utexas.edu
 * Department of Chemical Engineering
 * The University of Texas at Austin
 * Austin, Texas  78712
 *
 * Modified by Greg Lehey, LEMIS, 1 December 1993:
 *
 * - Remove absolute dependencies between manual section and man page extension
 * - Process packed, compressed and gzipped man pages
 * - display the name of the man page in the pager prompt where this is supported
 *   (previously, names of piped man pages were omitted)
 * - removed debug calls where I found them (use gdb instead)
 */

/* Extensions for compressed page types. These can be overridden in the Makefile */
#ifndef ISPACKED
#define ISPACKED	".z"
#endif
#ifndef ISCOMPRESSED
#define ISCOMPRESSED	".Z"
#endif
#ifndef ISGZIPPED
#define ISGZIPPED	".gz"
#endif

/* This string should appear in the pathname of any formatted man page */
#ifndef ISFORMATTED
#define ISFORMATTED	"/cat"
#endif

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 512
#endif

#ifndef MAXDIRS
#define MAXDIRS 64
#endif

/*
 * These are the programs man(1) execs with the -k and -f options.
 */

#define APROPOS "/opt/bin/apropos"
#define WHATIS  "/opt/bin/whatis"

/*
 * This might also be "/usr/ucb/more -s", though I prefer less(1)
 * because it allows one to backup even when reading from pipes.
 */

#define PAGER   "/opt/bin/less -sC -e"

static char config_file[] = "/opt/lib/man/manpath.config";

#ifdef HAS_GROFF
static char nroff_command[] = "/opt/bin/groff -Tascii -man";
#ifdef HAS_TROFF
static char troff_command[] = "/opt/bin/groff -Tps -man";
#endif
#else							    /* no groff */
static char nroff_command[] = "/usr/bin/nroff -man";

#ifdef HAS_TROFF
static char troff_command[] = "/usr/bin/troff -man";
#endif
#endif

/*
 * Define the valid manual sections.  For example, if your man
 * directory tree has subdirectories man1, man2, man3, mann,
 * and man3foo, valid_sections[] would have "1", "2", "3", "n", and
 * "3foo".  Directories are searched in the order they appear.  Having
 * extras isn't fatal, it just slows things down a bit.
 *
 * Note that this is just for directories to search.  If you have
 * files like .../man3/foobar.3Xtc, you don't need to have "3Xtc" in
 * the list below -- this is handled separately, so that `man 3Xtc foobar',
 * `man 3 foobar', and `man foobar' should find the file .../man3/foo.3Xtc,
 * (assuming, of course, that there isn't a .../man1/foo.1 or somesuch
 * that we would find first).
 *
 * Note that this list should be in the order that you want the
 * directories to be searched.  Is there a standard for this?  What is
 * the normal order?  If anyone knows, please tell me!
 */

static char *valid_sections[] = 
{
  "0",							    /* Sun has a 0 */
  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",
  ".ADM", ".C", ".CP", ".CT", ".DOS", ".F", ".HW", ".M", ".S",	    /* and SCO has all this crap */
  NULL
};

/*
 * Not all systems define these in stat.h.
 */

#ifndef S_IRUSR
#define	S_IRUSR	00400		/*  read permission: owner */
#endif
#ifndef S_IWUSR
#define	S_IWUSR	00200		/*  write permission: owner */
#endif
#ifndef S_IRGRP
#define	S_IRGRP	00040		/*  read permission: group */
#endif
#ifndef S_IWGRP
#define	S_IWGRP	00020		/*  write permission: group */
#endif
#ifndef S_IROTH
#define	S_IROTH	00004		/*  read permission: other */
#endif
#ifndef S_IWOTH
#define	S_IWOTH	00002		/*  write permission: other */
#endif

/*
 * This is the mode used for formatted pages that we create.
 */

#ifndef CATMODE
#define CATMODE S_IRUSR | S_IRGRP | S_IROTH
#endif

#ifdef SUPPORT_CAT_DIRS
static const int cat_support = 1;
#else
static const int cat_support = 0;
#endif


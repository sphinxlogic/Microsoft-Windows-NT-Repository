/* DJGPP configuration */

#include <pc/config.h>

#define HAVE_ST_BLKSIZE 1

#define _POSIX_SOURCE 1

#include <errno.h>
#ifndef EISDIR
#define EISDIR EACCES
#endif

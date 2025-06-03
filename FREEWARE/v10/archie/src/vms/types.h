/* sys/types.h */
#ifndef _types_
#define _types_

#ifndef __GNUC__
# include <sys$library:stddef.h>
#endif /* not Gnu C */

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef long daddr_t;
typedef char *caddr_t;

#include <sys$library:types.h>

typedef unsigned short ino_t;
typedef char *dev_t;
typedef unsigned int off_t;
typedef long key_t;

#include "[.vms]fd.h"

#endif /* _types */

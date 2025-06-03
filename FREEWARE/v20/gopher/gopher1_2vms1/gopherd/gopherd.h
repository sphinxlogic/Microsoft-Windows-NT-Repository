#ifdef VMS
#define GOPHERD_VERSION "1.2VMS"
char *VMS$wild_search(char *pathname, int sockfd);
#define ropendir(a) VMS$wild_search(a,sockfd)
#define readdir(a)  VMS$wild_search(NULL,sockfd)
#endif

#define PATCH_LEVEL "1"

#ifndef SERVER
#define SERVER
#endif

#include "conf.h"

#include <ctype.h>
#include <stdio.h>
#include <perror.h>
#ifdef __GNUC__
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vms/psldef.h>
#include <vms/lnmdef.h>
#else
#include <file.h>
#include <types.h>
#include <socket.h>
#include <in.h>
#include <netdb.h>
#include <psldef.h>
#include <lnmdef.h>
#endif

#include <errno.h>
#include <string.h>
#include <stat.h>
#include <time.h>

#include "GDgopherdir.h"

#ifdef __GNUC__
#include <vms/descrip.h>
#include <vms/rms.h>
#include <vms/iodef.h>
#include <vms/ssdef.h>
#else
#include <descrip.h>
#include <rms.h>
#include <iodef.h>
#include <ssdef.h>
#endif

#define TRUE 1
#define FALSE 0

#define MAXLST 4


static struct itmlst {
  unsigned short int length;
  unsigned short int code;
  char *bufadr;
  int *retlen;
} lnmlst[MAXLST];

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN                (64)
#endif

#ifndef NOFILE
#define NOFILE (100)
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN (512)
#endif

#include "compatible.h"
#include "util.h"
#include "gopherdconf.h"

/*** This one must be last ***/

#include "globals.h"

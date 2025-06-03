/* $Id: server.c,v 1.2 92/01/11 16:08:40 usenet Exp $
**
** $Log:	server.c,v $
 * Revision 1.2  92/01/11  16:08:40  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:21  davison
** Trn Release 2.0
** 
*/

#include "EXTERN.h"
#include "common.h"
#include "threads.h"

#ifdef SERVER

#include "server.h"

char *
get_server_name(errflag)
int errflag;
{
    char *name;
    extern char *getenv();

    if (SERVER_FILE[0] == '/') {
	name = getserverbyfile(SERVER_FILE);
    } else {
	if ((name = getenv("NNTPSERVER")) == NULL)
	    name = SERVER_FILE;
    }
    if (name == NULL && errflag) {
	fprintf(stderr, "Couldn't get name of news server from %s\n",
	    SERVER_FILE);
	fprintf(stderr,
	    "Either fix this file, or put NNTPSERVER in your environment.\n");
    }
    return name;
}

#endif /* SERVER */


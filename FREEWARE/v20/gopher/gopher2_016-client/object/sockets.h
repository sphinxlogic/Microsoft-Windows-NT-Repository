/********************************************************************
 * lindner
 * 3.5
 * 1994/04/08 20:05:54
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/Sockets.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: Sockets.h
 * Socket protos etc.
 *********************************************************************
 * Revision History:
 * Sockets.h,v
 * Revision 3.5  1994/04/08  20:05:54  lindner
 * gcc -Wall fixes
 *
 * Revision 3.4  1994/03/31  21:03:36  lindner
 * Use better, more descriptive return codes for socket routines
 *
 * Revision 3.3  1994/03/17  04:35:04  lindner
 * Move definitions from gopherd.h here
 *
 * Revision 3.2  1994/01/21  04:14:31  lindner
 * Add forward declaration for SOCKnetnames()
 *
 * Revision 3.1  1993/07/07  19:27:27  lindner
 * Socket functions
 *
 *
 *********************************************************************/


#ifndef SOCKETS_H
#define SOCKETS_H

/* Socket specific stuff, ugh! */
#ifdef VMS
#  if !defined(CMUIP) && !defined(NETLIB)
#    include <socket.h>
#    include <in.h>
#    include <file.h>
#    include <inet.h>
#    include <netdb.h>
#    include iodef
#    include ssdef
#  else
#    include iodef
#    include ssdef
#    include errno
#    include perror
#  endif        /* if !CMUIP && !NETLIB */

#else  /* VMS */
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <sys/file.h>
#  ifndef hpux
#    include <arpa/inet.h>
#  endif
#  include <netdb.h>
#endif  /* not VMS */


#ifndef MAXHOSTNAMELEN
#  define MAXHOSTNAMELEN                (64)
#endif




void SOCKlinger();
int  SOCKbind_to_port();
int  SOCKconnect();
int  SOCKlisten();
int  SOCKaccept();
void SOCKnetnames();
char *SOCKgetDNSname();
int  SOCKgetPort();

#define ErrSocketGetService 	-1
#define ErrSocketGetHost	-2
#define ErrSocketCall		-3
#define ErrSocketConnect	-4
#define ErrSocketNullHost	-5
#define ErrSocketInterrupted	-6

#endif  /* SOCKETS_H */

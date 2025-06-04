#define	readonly	ReadOnly	/*~~~~*/
/*
 * Program:	Operating-system dependent routines -- VMS version.
 *
 * Author:	Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *
 * Date:	11 May 1989
 * Last Edited:	3 October 1993
 *
 * Copyright 1993 by the University of Washington
 *
 *  Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * above copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Washington not be
 * used in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  This software is made
 * available "as is", and
 * THE UNIVERSITY OF WASHINGTON DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND IN
 * NO EVENT SHALL THE UNIVERSITY OF WASHINGTON BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE) OR STRICT LIABILITY, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef __OSDEP_H
#define	__OSDEP_H
/* TCP input buffer */

#define BUFLEN 8192


/* TCP I/O stream (must be before osdep.h is included) */

#define TCPSTREAM struct tcp_stream
TCPSTREAM {
  char *host;			/* host name */
  char *localhost;		/* local host name */
  int tcpsi;			/* input socket */
  int tcpso;			/* output socket */
  int ictr;			/* input counter */
  char *iptr;			/* input pointer */
  char ibuf[BUFLEN];		/* input buffer */
};

#include <types.h>
#include <time.h>
#include <timeb.h>
#include <sys/types.h>
#include <sys/socket.h>
#ifdef MULTINET
#include <netinet/in.h>
#include <netdb.h>
#endif	/* MULTINET */
#ifdef __ALPHA
#include <ctype.h>
#endif
#include <errno.h>
extern int errno;		/* just in case */
#include "../c-client/mail.h"
#include "../c-client/misc.h"

#define MAILFILE "/usr/spool/mail/%s"
#define ACTIVEFILE "/usr/lib/news/active"
#define NEWSSPOOL "/usr/spool/news"
#define NEWSRC strcat (strcpy (tmp,myhomedir ()),".newsrc")


void rfc822_date  ();
void *fs_get  ();
void fs_resize  ();
void fs_give  ();
unsigned long strcrlfcpy  ();
unsigned long strcrlflen  ();
long server_login  ();
char *myusername ();
char *myhomedir ();
char *lockname  ();
TCPSTREAM *tcp_open  ();
TCPSTREAM *tcp_aopen  ();
char *tcp_getline  ();
long tcp_getbuffer  ();
long tcp_getdata  ();
long tcp_soutr  ();
long tcp_sout  ();
void tcp_close  ();
char *tcp_host  ();
char *tcp_localhost  ();
char *memmove  ();
char *strstr  ();
char *strerror  ();
#endif	/* OSDEP_H */

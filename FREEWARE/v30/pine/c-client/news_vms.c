/*
 * Program:	Netnews mail routines
 *
 * Author:	Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *		Internet: MRC@CAC.Washington.EDU
 *
 * Date:	4 September 1991
 * Last Edited:	18 November 1993
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

#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <errno.h>
extern int errno;		/* just in case */
#include <types.h>
#include "osdep.h"
#include "mail.h"
#include <file.h>
#include <stat.h>
#include <time.h>
#include "news.h"
#include "rfc822.h"
#include "misc.h"

/* Netnews mail routines */
/* Read netnews database
 * Accepts: pointer to netnews database handle (handle NIL if first time)
 * Returns: character string for netnews database or NIL if done
 * Note - uses strtok() mechanism
 */
char *news_read (sdb)
	void **sdb;
{
  int fd;
  char *s,*t,tmp[MAILTMPLEN];
  struct stat sbuf;
  if (!*sdb) {			/* first time through? */
    if ((fd = open (NEWSRC,O_RDONLY,NIL)) < 0) return NIL;
    fstat (fd,&sbuf);		/* get file size and read data */
    read (fd,s = (char *) (*sdb = fs_get (sbuf.st_size + 1)),sbuf.st_size);
    close (fd);			/* close file */
    s[sbuf.st_size] = '\0';	/* tie off string */
    if (t = strtok (s,"\n")) return t;
  }
				/* subsequent times through database */
  else if (t = strtok (NIL,"\n")) return t;
  fs_give (sdb);		/* free database */
  return NIL;			/* all done */
}

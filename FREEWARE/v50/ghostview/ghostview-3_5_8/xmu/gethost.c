/*
 * $XConsortium: GetHost.c,v 1.2 90/12/14 19:24:38 converse Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 *
 * _XGetHostname - similar to gethostname but allows special processing.
 */

#ifdef VMS
#   include <X11_DIRECTORY/Intrinsic.h>  /*###jp### inserted 20.10.94 */
/* #define HAVE_GETHOSTNAME */
#   ifdef HAVE_GETHOSTNAME
       int gethostname( char *name, int namelen);
#   else
#      include <unixlib.h>  /* for getenv */
#   endif
#endif

int XmuGetHostname (buf, maxlen)
    char *buf;
    int maxlen;
{
    int len;

#ifdef USG
#define NEED_UTSNAME
#endif

#ifdef NEED_UTSNAME
#include <sys/utsname.h>
    /*
     * same host name crock as in server and xinit.
     */
    struct utsname name;

    uname (&name);
    len = strlen (name.nodename);
    if (len >= maxlen) len = maxlen - 1;
    strncpy (buf, name.nodename, len);
    buf[len] = '\0';
#else
    buf[0] = '\0';
#   ifdef VMS   /*###jp###*/
#      ifdef HAVE_GETHOSTNAME
          (void) gethostname (buf, maxlen); /* 06/18/95 */
#      else
          {
             char hn[100];
             char *tmp;
             int len;
             tmp = getenv("SYS$NODE");
             if (tmp) {
                strcpy(hn,tmp);
                tmp = strchr(hn,':');
                if (tmp) *tmp = '\0';
             }
             else hn[0] = '\0';
             len = strlen (hn);
             if (len) {
                if (len >= maxlen) len = maxlen - 1;
                strncpy (buf, hn, len);
                buf[len] = '\0';
             }
          }
#      endif /* HAVE_GETHOSTNAME */
#   else
       (void) gethostname (buf, maxlen);
#   endif /* VMS */
    buf [maxlen - 1] = '\0';
    len = strlen(buf);
#endif /* hpux */
    return len;
}


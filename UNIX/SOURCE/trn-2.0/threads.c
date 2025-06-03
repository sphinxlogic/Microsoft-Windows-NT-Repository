/* $Id: threads.c,v 1.2 92/01/11 16:09:11 usenet Exp $
**
** $Log:	threads.c,v $
 * Revision 1.2  92/01/11  16:09:11  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:21  davison
** Trn Release 2.0
** 
*/

#include "EXTERN.h"
#include "common.h"
#include "INTERN.h"
#include "threads.h"

#ifdef USETHREADS

extern char *threaddir;

/* Change a newsgroup name into the name of the thread data file.  We
** subsitute any '.'s in the group name into '/'s (unless LONG_THREAD_NAMES
** is defined), prepend the path, and append the '/.thread' (or '.th') on to
** the end.
*/
char *
thread_name(group)
char *group;
{
    static char name_buff[MAXFILENAME];
#ifndef LONG_THREAD_NAMES
    char group_buff[512];
    register char *ptr;

    strcpy(group_buff, group);
    ptr = group = group_buff;
    while ((ptr = index(ptr, '.'))) {
	*ptr = '/';
    }
#endif
#ifdef SUFFIX
    sprintf(name_buff, "%s/%s%s", threaddir, group, SUFFIX);
#else
    sprintf(name_buff, "%s/%s", threaddir, group);
#endif

    return name_buff;
}

/* Determine this machine's byte map for WORDs and LONGs.  A byte map is an
** array of BYTEs (sizeof (WORD) or sizeof (LONG) of them) with the 0th BYTE
** being the byte number of the high-order byte in my <type>, and so forth.
*/
void
mybytemap(map)
BMAP *map;
{
    union {
	BYTE b[sizeof (LONG)];
	WORD w;
	LONG l;
    } u;
    register BYTE *mp;
    register int i, j;

    mp = &map->w[sizeof (WORD)];
    u.w = 1;
    for (i = sizeof (WORD); i > 0; i--) {
	for (j = 0; j < sizeof (WORD); j++) {
	    if (u.b[j] != 0) {
		break;
	    }
	}
	if (j == sizeof (WORD)) {
	    goto bad_news;
	}
	*--mp = j;
	while (u.b[j] != 0 && u.w) {
	    u.w <<= 1;
	}
    }

    mp = &map->l[sizeof (LONG)];
    u.l = 1;
    for (i = sizeof (LONG); i > 0; i--) {
	for (j = 0; j < sizeof (LONG); j++) {
	    if (u.b[j] != 0) {
		break;
	    }
	}
	if (j == sizeof (LONG)) {
	  bad_news:
	    /* trouble -- set both to *something* consistent */
	    for (j = 0; j < sizeof (WORD); j++) {
		map->w[j] = j;
	    }
	    for (j = 0; j < sizeof (LONG); j++) {
		map->l[j] = j;
	    }
	    return;
	}
	*--mp = j;
	while (u.b[j] != 0 && u.l) {
	    u.l <<= 1;
	}
    }
}

/* Transform each WORD's byte-ordering in a buffer of the designated length.
*/
void
wp_bmap(buf, len)
WORD *buf;
int len;
{
    union {
	BYTE b[sizeof (WORD)];
	WORD w;
    } in, out;
    register int i;

    if (word_same) {
	return;
    }
    while (len--) {
	in.w = *buf;
	for (i = 0; i < sizeof (WORD); i++) {
	    out.b[my_bmap.w[i]] = in.b[mt_bmap.w[i]];
	}
	*buf++ = out.w;
    }
}

/* Transform each LONG's byte-ordering in a buffer of the designated length.
*/
void
lp_bmap(buf, len)
LONG *buf;
int len;
{
    union {
	BYTE b[sizeof (LONG)];
	LONG l;
    } in, out;
    register int i;

    if (long_same) {
	return;
    }
    while (len--) {
	in.l = *buf;
	for (i = 0; i < sizeof (LONG); i++) {
	    out.b[my_bmap.l[i]] = in.b[mt_bmap.l[i]];
	}
	*buf++ = out.l;
    }
}

#endif /* USETHREADS */

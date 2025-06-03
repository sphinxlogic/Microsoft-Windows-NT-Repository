/* $Id: rthreads.c,v 1.3 92/01/11 16:30:37 usenet Exp $
**
** $Log:	rthreads.c,v $
 * Revision 1.3  92/01/11  16:30:37  usenet
 * Lots of bug fixes:
 * 
 *    o    More upgrade notes for trn 1.x users (see ** LOOK ** in NEW).
 *    o    Enhanced the article-reading code to not remember our side-trip
 *         to the end of the group between thread selections (to fix '-').
 *    o    Extended trrn's handling of new articles (we fetch the active file
 *         more consistently, and listen to what the GROUP command tells us).
 *    o    Enhanced the thread selector to mention when new articles have
 *         cropped up since the last visit to the selector.
 *    o    Changed strftime to use size_t and added a check for size_t in
 *         Configure to make sure it is defined.  Also made it a bit more
 *         portable by using gettimeofday() and timezone() in some instances.
 *    o    Fixed a problem with the "total" structure not getting zero'ed in
 *         mthreads (causing bogus 'E'rrors on null groups).
 *    o    Fixed a reference to tmpbuf in intrp.c that was bogus.
 *    o    Fixed a problem with using N and Y with the newgroup code and then
 *         trying to use the 'a' command.
 *    o    Fixed an instance where having trrn get ahead of the active file
 *         might declare a group as being reset when it wasn't.
 *    o    Reorganized the checks for Apollo's C library to make sure it doesn't
 *         find the Domain OS version.
 *    o    Added a check for Xenix 386's C library.
 *    o    Made the Configure PATH more portable.
 *    o    Fixed the arguments prototypes to int_catcher() and swinch_catcher().
 *    o    Fixed the insert-my-subject-before-my-sibling code to not do this
 *         when the sibling's subject is the same as the parent.
 *    o    Fixed a bug in the RELAY code (which I'm suprised is still being used).
 *    o    Twiddled the mthreads.8 manpage.
 *    o    mthreads.8 is now installed and the destination is prompted for in
 *         Configure, since it might be different from the .1 destination.
 *    o    Fixed a typo in newsnews.SH and the README.
 * 
 * Revision 1.2  92/01/11  16:07:05  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:18  davison
** Trn Release 2.0
** 
*/

#include "EXTERN.h"
#include "common.h"
#include "intrp.h"
#include "rn.h"
#include "ngdata.h"
#include "rcln.h"
#include "bits.h"
#include "util.h"
#ifndef USETMPTHREAD
#include "rcstuff.h"
#endif
#ifdef SERVER
#include "server.h"
#endif

#ifdef USETHREADS
#include "threads.h"
#include "INTERN.h"
#include "rthreads.h"

static FILE *fp;

static char *strings = Nullch;

static bool tmpthread_created = FALSE;

static int read_item();
static void safefree();

/* Initialize our thread code by determining the byte-order of the thread
** files and our own current byte-order.  If they differ, set flags to let
** the read code know what we'll need to translate.
*/
void
thread_init()
{
    int i;
#ifdef XTHREAD
    long size;
#endif

    tmpthread_file = savestr(filexp("/tmp/thread.%$"));

    word_same = long_same = TRUE;
#ifdef XTHREAD
    sprintf(ser_line, "XTHREAD DBINIT");
# ifdef DEBUGGING
    if (debug & DEB_NNTP) {
	printf(">%s\n", ser_line) FLUSH;
    }
# endif
    put_server(ser_line);
    rawcheck_server(ser_line, sizeof ser_line);
# ifdef DEBUGGING
    if (debug & DEB_NNTP) {
	printf("<%s\n", ser_line) FLUSH;
    }
# endif
    size = rawget_server((char*)&mt_bmap, sizeof (BMAP));
    if (size >= sizeof (BMAP) - 1) {
#else /* !XTHREAD */
    if ((fp = fopen(filexp(DBINIT), FOPEN_RB)) != Nullfp
     && fread((char*)&mt_bmap, 1, sizeof(BMAP), fp) >= sizeof (BMAP) - 1) {
#endif
	if (mt_bmap.version != DB_VERSION) {
	    printf("\nThread database is the wrong version -- ignoring it.\n")
		FLUSH;
	    use_threads = FALSE;
	}
	mybytemap(&my_bmap);
	for (i = 0; i < sizeof (LONG); i++) {
	    if (i < sizeof (WORD)) {
		if (my_bmap.w[i] != mt_bmap.w[i]) {
		    word_same = FALSE;
		}
	    }
	    if (my_bmap.l[i] != mt_bmap.l[i]) {
		long_same = FALSE;
	    }
	}
    } else {
	printf("\ndb.init read failed -- assuming no byte-order translations.\n\n") FLUSH;
    }
#ifdef XTHREAD
    while (rawget_server(ser_line, sizeof ser_line)) {
	;		/* trash any extraneous bytes */
    }
#else
    if (fp != Nullfp) {
	fclose(fp);
    }
#endif
}

/* Open a thread file to make use of the data it contains.  Everything is
** slurped into arrays and buffers, and some minor digesting of the data
** is performed to make it more palatable.  Be sure to call unuse_data()
** before calling this again.
*/
int
use_data(thread_if_empty)
bool_int thread_if_empty;
{
    register int i, j, k;
    register char *ptr;
    char *threadname;
    bool already_tried = FALSE;
    ART_NUM last, first;

    last = getngsize(ng);
    first = getabsfirst(ng, last);
    if (last < first) {
	return 0;
    }
    if (tmpthread_group && strEQ(ngname, tmpthread_group)) {
	threadname = tmpthread_file;
    } else {
#ifdef XTHREAD		/* use remote thread file? */
	long size;

	threadname = Nullch;		/* assume we fail */
	sprintf(ser_line, "XTHREAD THREAD");
# ifdef DEBUGGING
	if (debug & DEB_NNTP) {
	    printf(">%s\n", ser_line) FLUSH;
	}
# endif
	put_server(ser_line);
	size = rawcheck_server(ser_line, sizeof ser_line);
# ifdef DEBUGGING
	if (debug & DEB_NNTP) {
	    printf("<%s\n", ser_line) FLUSH;
	}
# endif
	if (size < 0) {
	    fp = Nullfp;
	} else {
	    char *tmpbuf;
	    long bufsize;

# ifdef SPEEDOVERMEM
	    bufsize = size ? size : 1;
# else
	    bufsize = 16384;
# endif
	    fp = fopen(tmpthread_file, "w");
	    tmpbuf = safemalloc(bufsize);
	    while ((size = rawget_server(tmpbuf, bufsize)) != 0) {
		if (fp != Nullfp) {
		    /* write it out unbuffered */
		    write(fileno(fp), tmpbuf, size);
		}
	    }
	    free(tmpbuf);
	    if (fp != Nullfp) {
		fclose(fp);
		growstr(&tmpthread_group, &tmpthread_glen,
			strlen(ngname) + 1);
		strcpy(tmpthread_group, ngname);
		tmpthread_created = FALSE;
		threadname = tmpthread_file;
	    }
	}
#else /* !XTHREAD */
	threadname = thread_name(ngname);
#endif
    }
try_to_use:
#ifdef XTHREAD
    if (!threadname || (fp = fopen(threadname, FOPEN_RB)) == Nullfp) {
	if (threadname && errno != ENOENT) {
#else
    if ((fp = fopen(threadname, FOPEN_RB)) == Nullfp) {
	if (errno != ENOENT) {
#endif
	    printf("\n\nOpen failed for thread data.\n");
	}
	bzero(&total, sizeof (TOTAL));
	if (!thread_if_empty || already_tried) {
	    return 0;
	}
    } else if (fread((char*)&total, 1, sizeof(TOTAL), fp) < sizeof(TOTAL)) {
	fclose(fp);
	bzero(&total, sizeof (TOTAL));
	if (already_tried) {
	    return 0;
	}
    } else if (threadname != tmpthread_file || !tmpthread_created) {
	lp_bmap(&total.first, 4);
	wp_bmap(&total.root, 5);
    } else if (!total.root) {
	fclose(fp);
	return 0;
    }

    if (total.last < last) {
#ifdef USETMPTHREAD
	char cmd[512];
	ART_NUM max = last - first + 1;

	if (fp) {
	    fclose(fp);
	}
	sprintf(cmd, "tmpthread %s %s %ld %ld %ld %s",
#ifdef XTHREAD
		threadname == tmpthread_file ?
			(tmpthread_created ? "-t" : "-T") : nullstr,
#else
		threadname == tmpthread_file ? "-t" : nullstr,
#endif
		ngname, (long)last, (long)first, (long)max, tmpthread_file);
	if (system(filexp(cmd))) {
	    printf("\n\nFailed to thread data -- continuing unthreaded.\n");
	    if (tmpthread_group) {
		*tmpthread_group = '\0';
	    }
	    return 0;
	}
	growstr(&tmpthread_group, &tmpthread_glen, strlen(ngname) + 1);
	strcpy(tmpthread_group, ngname);
	threadname = tmpthread_file;
	tmpthread_created = TRUE;
	already_tried = TRUE;
	goto try_to_use;
#else /* !USETMPTHREAD */
	if (lastart > total.last) {
	    tobethreaded = last - total.last;
	    toread[ng] -= tobethreaded;
	    lastart = total.last;
	}
#endif /* !USETMPTHREAD */
    }

    if (total.last > lastart) {
#ifdef SERVER
	if (time(Null(time_t*)) - lastactfetch > MINFETCHTIME) {
	    fclose(actfp);
	    ngdata_init();	/* re-grab the active file */
	}
#endif
	grow_ctl(total.last);	/* sets lastart */
	ngmax[ng] = lastart;	/* ensure getngsize() knows the new maximum */
    }

    if (!read_item(&author_cnts, (MEM_SIZE)total.author * sizeof (WORD))
     || !read_item(&strings, (MEM_SIZE)total.string1) 
     || !read_item(&subject_cnts, (MEM_SIZE)total.subject * sizeof (WORD))
     || !read_item(&p_roots, (MEM_SIZE)total.root * sizeof (PACKED_ROOT))
     || !read_item(&p_articles, (MEM_SIZE)total.article * sizeof (PACKED_ARTICLE))) {
	printf("\n\nRead failed for thread data -- continuing unthreaded.\n");
	fclose(fp);
	unuse_data(0);
	return 0;
    }
    fclose(fp);

    if ((threadname != tmpthread_file || !tmpthread_created)
     && (!word_same || !long_same)) {
	wp_bmap(author_cnts, total.author);
	wp_bmap(subject_cnts, total.subject);
	for (i = 0; i < total.root; i++) {
	    lp_bmap(&p_roots[i].root_num, 1);
	    wp_bmap(&p_roots[i].articles, 3);
	}
	for (i = 0; i < total.article; i++) {
	    lp_bmap(&p_articles[i].num, 2);
	    wp_bmap(&p_articles[i].subject, 8);
	}
    }

#ifndef lint
    author_ptrs = (char **)safemalloc(total.author * sizeof (char **));
    subject_ptrs = (char **)safemalloc(total.subject * sizeof (char **));
    root_subjects = (WORD *)safemalloc(total.root * sizeof (WORD));
    root_article_cnts = (WORD *)safemalloc(total.root * sizeof (WORD));
#endif
    selected_roots = safemalloc(total.root * sizeof (char));

    bzero(root_article_cnts, total.root * sizeof (WORD));
    bzero(selected_roots, total.root * sizeof (char));

    for (i = 0, ptr = strings; i < total.author; i++) {
	author_ptrs[i] = ptr;
	ptr += strlen(ptr) + 1;
    }

    for (i = 0, j = 0; i < total.root; i++) {
	root_subjects[i] = j;
	k = p_roots[i].subject_cnt;
	while (k--) {
	    root_article_cnts[i] += subject_cnts[j];
	    subject_ptrs[j++] = ptr;
	    ptr += strlen(ptr) + 1;
	}
	if (saved_selections) {
	    for (k = 0; k < selected_root_cnt; k++) {
		if (p_roots[i].root_num == saved_selections[k]) {
		    selected_roots[i] = 1;
		    break;
		}
	    }
	}
    }
    count_roots(!saved_selections);

    /* Try to clean up the bitmap if articles are missing. */
    if (unthreaded) {
      char *newarea, *oldarea = ctlarea;
      extern MEM_SIZE ctlsize;

	newarea = ctlarea = safemalloc(ctlsize);
	bzero(ctlarea, ctlsize);
	for (i = total.article, p_art = p_articles; i--; p_art++) {
	    if (p_art->num >= firstbit) {
		ctl_set(p_art->num);
	    }
	}
	for (i = firstbit; i <= lastart; i++) {
	    if (!ctl_read(i)) {
		ctlarea = oldarea;
		oneless(i);
		ctlarea = newarea;
	    }
	}
	ctlarea = oldarea;
	free(newarea);
	p_art = Nullart;
	count_roots(FALSE);
    }
    safefree(&saved_selections);
    select_page = 0;
    return 1;
}

/* A shorthand for reading a chunk of the file into a malloced array.
*/
static int
read_item(dest, len)
char **dest;
MEM_SIZE len;
{
    int ret;

    *dest = safemalloc(len);
    ret = fread(*dest, 1, (int)len, fp);
    if (ret != len) {
	free(*dest);
	*dest = Nullch;
	return 0;
    }
    return 1;
}

/* Free some memory if it hasn't already been freed.
*/
static void
safefree(pp)
char **pp;
{
    if (*pp) {
	free(*pp);
	*pp = Nullch;
    }
}

/* Discard the thread data that we received through the use_data() call.
** If "save_selections" is non-zero, we'll try to remember which roots
** are currently selected long enough for the use_data() call to re-use
** them.  Only do this when you are going to re-open the same data file
** immediately with use_data() (presumably because the data has been
** updated while we were using it).
*/
void
unuse_data(save_selections)
bool_int save_selections;
{
    int i, j;

    if (save_selections) {
#ifndef lint
	saved_selections
	  = (ART_NUM *)safemalloc(selected_root_cnt * sizeof (ART_NUM));
#endif
	for (i = 0, j = 0; i < total.root; i++) {
	    if (selected_roots[i]) {
		saved_selections[j++] = p_roots[i].root_num;
	    }
	}
    } else {
	selected_root_cnt = selected_count = 0;
    }
    safefree(&p_roots);
    safefree(&root_subjects);
    safefree(&author_cnts);
    safefree(&subject_cnts);
    safefree(&author_ptrs);
    safefree(&subject_ptrs);
    safefree(&root_article_cnts);
    safefree(&selected_roots);
    safefree(&p_articles);
    safefree(&strings);

    p_art = curr_p_art = Nullart;
    init_tree();		/* free any tree lines */

    bzero(&total, sizeof (TOTAL));
}

#endif /* USETHREADS */

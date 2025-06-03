/* $Id: mt-write.c,v 1.3 92/01/11 16:29:08 usenet Exp $
**
** $Log:	mt-write.c,v $
 * Revision 1.3  92/01/11  16:29:08  usenet
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
 * Revision 1.2  92/01/11  16:05:18  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:15  davison
** Trn Release 2.0
** 
*/

#include "EXTERN.h"
#include "common.h"
#include "threads.h"
#include "mthreads.h"

static FILE *fp_out;
static int seq;
static int article_seq;

static int failure;

void write_subjects(), write_authors(), write_roots(), write_ids();
void write_articles(), write_thread(), write_item();
void enumerate_articles(), enumerate_thread();
void free_leftovers();

/* Write out all the data in a packed format that is easy for our newsreader
** to use.  We free things as we go, when we don't need them any longer.  If
** we encounter any write errors, the write_item routine sets a failure flag
** to halt our writing of the file, but we keep on plugging away to free
** everything up.
*/
int
write_data(filename)
char *filename;
{
    if (filename == Nullch) {
	failure = 2;	/* A NULL filename indicates just free the data */
    } else if ((fp_out = fopen(filename, FOPEN_WB)) == Nullfp) {
	if (ensure_path(filename)) {
	    if ((fp_out = fopen(filename, FOPEN_WB)) == Nullfp) {
		log_error("Unable to create file: `%s'.\n", filename);
		failure = 2;
	    }
	} else {
	    log_error("Unable to create path: `%s'.\n", filename);
	    failure = 2;
	}
    } else {
	failure = 0;
#ifdef SETBUFFER
	setbuffer(fp_out, rwbuf, RWBUFSIZ);
#else
# ifdef SETVBUF
	setvbuf(fp_out, rwbuf, _IOFBF, RWBUFSIZ);
# endif
#endif
    }

    /* If there's no roots, there's no data.  Leave the file with no length. */
    if (!total.root && !failure) {
	failure = -1;
    }

    write_item(&total, sizeof (TOTAL));

    enumerate_articles();

    write_authors();
    write_subjects();
    write_roots();
    write_articles();
    write_ids();
    free_leftovers();
    bzero(&total, sizeof (TOTAL));

    if (failure != 2) {
	fclose(fp_out);
    }
    if (failure == 1) {
	log_error("Write failed!  Removing `%s'.\n", filename);
	unlink(filename);
    }
    return failure <= 0;
}

/* Recursively descend the article tree, enumerating the articles as we go.
** This way we can output the article sequence numbers into the data file.
*/
void
enumerate_articles()
{
    register ROOT *root;

    seq = article_seq = 0;

    for (root = root_root; root; root = root->link) {
	root->seq = seq++;
	if (!root->articles) {
	    log_error("** No articles on this root??\n");
	    continue;
	}
	enumerate_thread(root->articles);
    }
    if (seq != total.root) {
	log_error("** Wrote %d roots instead of %d **\n", seq, total.root);
    }
    if (article_seq != total.article) {
	log_error("** Wrote %d articles instead of %d **\n", article_seq, total.article);
    }
}

/* Recursive routine for above-mentioned enumeration. */
void
enumerate_thread(article)
ARTICLE *article;
{
    while (article) {
	article->seq = article_seq++;
	if (article->children) {
	    enumerate_thread(article->children);
	}
	article = article->siblings;
    }
}

#define write_and_free(str_ptr)	/* Comment for makedepend to	 \
					** ignore the backslash above */ \
{\
    register int len = strlen(str_ptr) + 1;\
    write_item(str_ptr, len);\
    free(str_ptr);\
    string_offset += len;\
}

MEM_SIZE string_offset;

/* Write out the author information:  first the use-counts, then the
** name strings all packed together.
*/
void
write_authors()
{
    register AUTHOR *author;

    seq = 0;
    for (author = author_root; author; author = author->link) {
	write_item(&author->count, sizeof (WORD));
	author->seq = seq++;
    }
    if (seq != total.author) {
	log_error("** Wrote %d authors instead of %d **\n",
		seq, total.author);
    }

    string_offset = 0;

    for (author = author_root; author; author = author->link) {
	write_and_free(author->name);
    }
}

/* Write out the subject information: first the packed string data, then
** the use-counts.  The order is important -- it is the order required
** by the roots for their subject structures.
*/
void
write_subjects()
{
    register ROOT *root;
    register SUBJECT *subject;

    for (root = root_root; root; root = root->link) {
	for (subject = root->subjects; subject; subject = subject->link) {
	    write_and_free(subject->str);
	}
    }
    if (string_offset != total.string1) {
	log_error("** Author/subject strings were %ld bytes instead of %ld **\n",
		string_offset, total.string1);
    }

    seq = 0;
    for (root = root_root; root; root = root->link) {
	for (subject = root->subjects; subject; subject = subject->link) {
	    write_item(&subject->count, sizeof (WORD));
	    subject->seq = seq++;
	}
    }
    if (seq != total.subject) {
	log_error("** Wrote %d subjects instead of %d **\n",
		seq, total.subject);
    }
}

/* Write the roots in a packed format.  Interpret the pointers into
** sequence numbers as we go.
*/
void
write_roots()
{
    register ROOT *root;

    for (root = root_root; root; root = root->link) {
	p_root.articles = root->articles->seq;
	p_root.root_num = root->root_num;
	p_root.thread_cnt = root->thread_cnt;
	p_root.subject_cnt = root->subject_cnt;
	write_item(&p_root, sizeof (PACKED_ROOT));
    }
}

#define rel_article(article, rseq)	((article)? (article)->seq - (rseq) : 0)
#define valid_seq(ptr)		((ptr)? (ptr)->seq : -1)

/* Write all the articles in the same order that we sequenced them. */
void
write_articles()
{
    register ROOT *root;

    for (root = root_root; root; root = root->link) {
	write_thread(root->articles);
    }
}

/* Recursive routine to write the articles in thread order.  We depend on
** the fact that our first child is the very next article written (if we
** have children).
*/
void
write_thread(article)
register ARTICLE *article;
{
    while (article) {
	p_article.num = article->num;
	p_article.date = article->date;
	p_article.subject = valid_seq(article->subject);
	p_article.author = valid_seq(article->author);
	p_article.flags = article->flags;
	p_article.child_cnt = article->child_cnt;
	p_article.parent = rel_article(article->parent, article->seq);
	p_article.siblings = rel_article(article->siblings, article->seq);
	p_article.root = article->root->seq;
	write_item(&p_article, sizeof (PACKED_ARTICLE));
	if (article->children) {
	    write_thread(article->children);
	}
	article = article->siblings;
    }
}

WORD minus_one = -1;

/* Write the message-id strings:  each domain name (not including the
** ".unknown." domain) followed by all of its associated unique ids.
** Then output the article sequence numbers they belong to.  This stuff
** is last because the newsreader doesn't need to read it.
*/
void
write_ids()
{
    register DOMAIN *domain;
    register ARTICLE *id;
    register DOMAIN *next_domain;
    register ARTICLE *next_id;

    string_offset = 0;

    for (domain = &unk_domain; domain; domain = domain->link) {
	if (domain != &unk_domain) {
	    write_and_free(domain->name);
	    if (!domain->ids) {
		log_error("** Empty domain name!! **\n");
	    }
	}
	for (id = domain->ids; id; id = id->id_link) {
	    write_and_free(id->id);
	}
    }
    if (string_offset != total.string2) {
	log_error("** Message-id strings were %ld bytes (%ld) **\n",
		string_offset, total.string2);
    }
    for (domain = &unk_domain; domain; domain = next_domain) {
	next_domain = domain->link;
	for (id = domain->ids; id; id = next_id) {
	    next_id = id->id_link;
	    write_item(&id->seq, sizeof (WORD));
	    free(id);
	}
	write_item(&minus_one, sizeof (WORD));
	if (domain != &unk_domain) {
	    free(domain);
	}
    }
    unk_domain.ids = Nullart;
    unk_domain.link = Null(DOMAIN*);
}

/* Free everything that's left to free.
*/
void
free_leftovers()
{
    register ROOT *root, *next_root;
    register SUBJECT *subj, *next_subj;
    register AUTHOR *author, *next_author;

    for (root = root_root; root; root = next_root) {
	next_root = root->link;
	for (subj = root->subjects; subj; subj = next_subj) {
	    next_subj = subj->link;
	    free(subj);
	}
	free(root);
    }
    for (author = author_root; author; author = next_author) {
	next_author = author->link;
	free(author);
    }
    root_root = Null(ROOT*);
    author_root = Null(AUTHOR*);
}

/* This routine will check to be sure that the required path exists for
** the data file, and if not it will attempt to create it.
*/
int
ensure_path(filename)
register char *filename;
{
    int status, pid, w;
    char tmpbuf[1024];
#ifdef MAKEDIR
    register char *cp, *last;
    register char *tbptr = tmpbuf+5;

    if (!(last = rindex(filename, '/'))) {	/* find filename portion */
	return 1;				/* no path, we're fine */
    }
    *last = '\0';				/* truncate path at filename */
    strcpy(tmpbuf, "mkdir");

    for (cp = last;;) {
	if (stat(filename, &filestat) >= 0 && (filestat.st_mode & S_IFDIR)) {
	    *cp = '/';
	    break;
	}
	if (!(cp = rindex(filename, '/'))) {/* find something that exists */
	    break;
	}
	*cp = '\0';
    }
    
    for (cp = filename; cp <= last; cp++) {
	if (!*cp) {
	    sprintf(tbptr, " %s", filename);
	    tbptr += strlen(tbptr);		/* set up for mkdir call */
	    *cp = '/';
	}
    }
    if (tbptr == tmpbuf+5) {
	return 1;
    }
#else
    sprintf(tmpbuf,"%s %s %d", file_exp(DIRMAKER), filename, 1);
#endif

    if ((pid = vfork()) == 0) {
	execl(SH, SH, "-c", tmpbuf, Nullch);
	_exit(127);
    }
    while ((w = wait(&status)) != pid && w != -1) {
	;
    }
    if (w == -1) {
	status = -1;
    }
    return !status;
}

/* A simple routine to output some data only if we haven't failed any
** previous writes.
*/
void
write_item(buff, len)
char *buff;
int len;
{
    if (!failure) {
	if (fwrite(buff, 1, len, fp_out) < len) {
	    failure = 1;
	}
    }
}

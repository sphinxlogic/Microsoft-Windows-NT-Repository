/* $Id: mt-read.c,v 1.2 92/01/11 16:05:16 usenet Exp $
**
** $Log:	mt-read.c,v $
 * Revision 1.2  92/01/11  16:05:16  usenet
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

extern char *lib, *rnlib, *mtlib, *spool, *threaddir, *homedir;
extern long first;

static FILE *fp_in;

void tweak_roots ANSI((void));
void wrap_it_up ANSI((int));

/* Attempt to open the thread file.  If it's there, only grab the totals
** from the start of the file.  This should give them enough information
** to decide if they need to read the whole thing into memory.
*/
int
init_data(filename)
char *filename;
{
    root_root = Null(ROOT*);
    author_root = Null(AUTHOR*);
    unk_domain.ids = Nullart;
    unk_domain.link = Null(DOMAIN*);

    if (filename && (fp_in = fopen(filename, FOPEN_RB)) != Nullfp) {
#ifdef SETBUFFER
	setbuffer(fp_in, rwbuf, RWBUFSIZ);
#else
# ifdef SETVBUF
	setvbuf(fp_in, rwbuf, _IOFBF, RWBUFSIZ);
# endif
#endif
	if (fread((char*)&total,1,sizeof (TOTAL),fp_in) == sizeof (TOTAL)) {
#ifdef TMPTHREAD
	    lp_bmap(&total.first, 4);
	    wp_bmap(&total.root, 5);
#endif

	    /* If the data looks ok, return success. */
	    if (total.last - total.first >= 0 && total.author > 0
	     && total.article > 0 && total.subject > 0 && total.domain > 0
	     && total.subject <= total.article && total.author <= total.article
	     && total.root <= total.article && total.domain <= total.article
	     && total.string1 > 0 && total.string2 > 0) {
		return 1;
	    }
	}
	bzero(&total, sizeof (TOTAL));
	total.first = first;
	total.last = first - 1;
	return 1;
    }
    bzero(&total, sizeof (TOTAL));
    return 0;
}

/* They want everything.  Read in the packed information and transform it
** into a set of linked structures that is easily manipulated.
*/
int
read_data()
{
    /* If this is an empty thread file, simply return success. */
    if (!total.root) {
	fclose(fp_in);
	return 1;
    }

    if (read_authors()
     && read_subjects()
     && read_roots()
     && read_articles()
     && read_ids())
    {
	tweak_roots();
	fclose(fp_in);
	return 1;
    }
    /* Something failed.  Safefree takes care of checking if some items
    ** were already freed.  Any partially-allocated structures were freed
    ** before we got here.  All other structures are cleaned up.
    */
    if (root_root) {
	register ROOT *root, *next_root;
	register SUBJECT *subj, *next_subj;

	for (root = root_root; root; root = next_root) {
	    for (subj = root->subjects; subj; subj = next_subj) {
		next_subj = subj->link;
		free(subj->str);
		free(subj);
	    }
	    next_root = root->link;
	    free(root);
	}
	root_root = Null(ROOT*);
    }
    if (author_array) {
	register int count;

	for (count = total.author; count--;) {
	    free(author_array[count]->name);
	    free(author_array[count]);
	}
	safefree(&author_array);
	author_root = Null(AUTHOR*);
    }
    if (article_array) {
	register int count;

	for (count = total.article; count--;) {
	    free(article_array[count]);
	}
	safefree(&article_array);
    }
    safefree(&strings);
    safefree(&subject_cnts);
    safefree(&subject_array);
    safefree(&author_cnts);
    safefree(&root_array);
    safefree(&ids);
    unk_domain.ids = Nullart;
    unk_domain.link = Null(DOMAIN*);
    fclose(fp_in);
    return 0;
}

/* They don't want to read the data.  Close the file if we opened it.
*/
void
dont_read_data(open_flag)
int open_flag;		/* 0 == not opened, 1 == open failed, 2 == open */
{
    if (open_flag == 2) {
	fclose(fp_in);
	bzero(&total, sizeof (TOTAL));
    }
}

#define give_string_to(dest)	/* Comment for makedepend to	 \
				** ignore the backslash above */ \
{\
    register MEM_SIZE len = strlen(string_ptr) + 1;\
    dest = safemalloc(len);\
    bcopy(string_ptr, dest, (int)len);\
    string_ptr += len;\
}

char *subject_strings, *string_end;

/* The author information is an array of use-counts, followed by all the
** null-terminated strings crammed together.  The subject strings are read
** in at the same time, since they are appended to the end of the author
** strings.
*/
int
read_authors()
{
    register int count, author_tally;
    register char *string_ptr;
    register WORD *authp;
    register AUTHOR *author, *last_author, **author_ptr;

    if (!read_item(&author_cnts, (MEM_SIZE)total.author * sizeof (WORD))
     || !read_item(&strings, total.string1)) {
	/* (Error already logged.) */
	return 0;
    }
#ifdef TMPTHREAD
    wp_bmap(author_cnts, total.author);
#endif

    string_ptr = strings;
    string_end = string_ptr + total.string1;
    if (string_end[-1] != '\0') {
	log_error("first string table is invalid.\n");
	return 0;
    }

    /* We'll use this array to point each article at its proper author
    ** (packed values are saved as indexes).
    */
    author_array = (AUTHOR**)safemalloc(total.author * sizeof (AUTHOR*));
    author_ptr = author_array;

    authp = author_cnts;

    author_tally = 0;
#ifndef lint
    last_author = (AUTHOR*)&author_root;
#else
    last_author = Null(AUTHOR*);
#endif
    for (count = total.author; count; count--) {
	if (string_ptr >= string_end) {
	    break;
	}
	*author_ptr++ = author = (AUTHOR*)safemalloc(sizeof (AUTHOR));
	last_author->link = author;
	give_string_to(author->name);
	author_tally += *authp;
	author->count = *authp++;
	last_author = author;
    }
    last_author->link = Null(AUTHOR*);

    subject_strings = string_ptr;

    safefree(&author_cnts);

    if (count || author_tally > total.article) {
	log_error("author unpacking failed.\n");
	for (; count < total.author; count++) {
	    free((*--author_ptr)->name);
	    free(*author_ptr);
	}
	safefree(&author_array);
	return 0;
    }
    return 1;
}

/* The subject values consist of the crammed-together null-terminated strings
** (already read in above) and the use-count array.  They were saved in the
** order that the roots require while being unpacked.
*/
int
read_subjects()
{
    if (!read_item(&subject_cnts, (MEM_SIZE)total.subject * sizeof (WORD))) {
	/* (Error already logged.) */
	return 0;
    }
#ifdef TMPTHREAD
    wp_bmap(subject_cnts, total.subject);
#endif
    return 1;
}

/* Read in the packed root structures and recreate the linked list versions,
** processing each root's subjects as we go.  Defer interpretation of article
** offsets until we unpack the article structures.
*/
int
read_roots()
{
    register int count, subj_tally;
    register char *string_ptr;
    register WORD *subjp;
    ROOT *root, *last_root, **root_ptr;
    SUBJECT *subject, *last_subject, **subj_ptr;
    int ret;

    /* Use this array when unpacking the article's subject offset. */
    subject_array = (SUBJECT**)safemalloc(total.subject * sizeof (SUBJECT*));
    subj_ptr = subject_array;
    /* And this array points the article's root offset at the right spot. */
    root_array = (ROOT**)safemalloc(total.root * sizeof (ROOT*));
    root_ptr = root_array;

    subjp = subject_cnts;
    string_ptr = subject_strings;	/* string_end is already set */

    subj_tally = 0;
#ifndef lint
    last_root = (ROOT*)&root_root;
#else
    last_root = Null(ROOT*);
#endif
    for (count = total.root; count--;) {
	ret = fread((char*)&p_root, 1, sizeof (PACKED_ROOT), fp_in);
	if (ret != sizeof (PACKED_ROOT)) {
	    log_error("failed root read -- %d bytes instead of %d.\n",
		ret, sizeof (PACKED_ROOT));
	    return 0;
	}
#ifdef TMPTHREAD
	lp_bmap(&p_root.root_num, 1);
	wp_bmap(&p_root.articles, 3);
#endif
	if (p_root.articles < 0 || p_root.articles >= total.article
	 || subj_ptr - subject_array + p_root.subject_cnt > total.subject) {
	    log_error("root has invalid values.\n");
	    return 0;
	}
	*root_ptr++ = root = (ROOT*)safemalloc(sizeof (ROOT));
	root->link = Null(ROOT*);
	root->articles = Nullart;
	root->seq = p_root.articles;
	root->root_num = p_root.root_num;
	root->thread_cnt = p_root.thread_cnt;
	root->subject_cnt = p_root.subject_cnt;
	last_root->link = root;
	last_root = root;

#ifndef lint
	last_subject = (SUBJECT*)&root->subjects;
#else
	last_subject = Null(SUBJECT*);
#endif
	while (p_root.subject_cnt--) {
	    if (string_ptr >= string_end) {
		log_error("error unpacking subject strings.\n");
		last_subject->link = Null(SUBJECT*);
		return 0;
	    }
	    *subj_ptr++ = subject = (SUBJECT*)safemalloc(sizeof (SUBJECT));
	    last_subject->link = subject;
	    give_string_to(subject->str);
	    subj_tally += *subjp;
	    subject->count = *subjp++;
	    last_subject = subject;
	}
	last_subject->link = Null(SUBJECT*);
    }
    if (subj_ptr != subject_array + total.subject
     || subj_tally > total.article
     || string_ptr != string_end) {
	log_error("subject data is invalid.\n");
	return 0;
    }
    safefree(&subject_cnts);
    safefree(&strings);

    return 1;
}

bool invalid_data;

/* A simple routine that checks the validity of the article's subject value.
** A -1 means that it is NULL, otherwise it should be an offset into the
** subject array we just unpacked.
*/
SUBJECT *
valid_subject(num, art_num)
WORD num;
long art_num;
{
    if (num == -1) {
	return Null(SUBJECT*);
    }
    if (num < 0 || num >= total.subject) {
	log_error("invalid subject in thread file: %d [%ld]\n", num, art_num);
	invalid_data = TRUE;
	return Null(SUBJECT*);
    }
    return subject_array[num];
}

/* Ditto for author checking. */
AUTHOR *
valid_author(num, art_num)
WORD num;
long art_num;
{
    if (num == -1) {
	return Null(AUTHOR*);
    }
    if (num < 0 || num >= total.author) {
	log_error("invalid author in thread file: %d [%ld]\n", num, art_num);
	invalid_data = TRUE;
	return Null(AUTHOR*);
    }
    return author_array[num];
}

/* Our parent/sibling information is a relative offset in the article array.
** zero for none.  Child values are always found in the very next array
** element if child_cnt is non-zero.
*/
ARTICLE *
valid_node(relative_offset, num)
WORD relative_offset;
int num;
{
    if (!relative_offset) {
	return Nullart;
    }
    num += relative_offset;
    if (num < 0 || num >= total.article) {
	log_error("invalid node offset in thread file.\n");
	invalid_data = TRUE;
	return Nullart;
    }
    return article_array[num];
}

/* Read the articles into their linked lists.  Point everything everywhere. */
int
read_articles()
{
    register int count;
    register ARTICLE *article, **article_ptr;
    int ret;

    /* Build an array to interpret interlinkages of articles. */
    article_array = (ARTICLE**)safemalloc(total.article * sizeof (ARTICLE*));
    article_ptr = article_array;

    /* Allocate all the structures up-front so that we can point to unread
    ** siblings as we go.
    */
    for (count = total.article; count--;) {
	*article_ptr++ = (ARTICLE*)safemalloc(sizeof (ARTICLE));
    }
    invalid_data = FALSE;
    article_ptr = article_array;
    for (count = 0; count < total.article; count++) {
	ret = fread((char*)&p_article, 1, sizeof (PACKED_ARTICLE), fp_in);
	if (ret != sizeof (PACKED_ARTICLE)) {
	    log_error("failed article read -- %d bytes instead of %d.\n", ret, sizeof (PACKED_ARTICLE));
	    return 0;
	}
#ifdef TMPTHREAD
	lp_bmap(&p_article.num, 2);
	wp_bmap(&p_article.subject, 8);
#endif

	article = *article_ptr++;
	article->num = p_article.num;
	article->date = p_article.date;
	article->subject = valid_subject(p_article.subject, p_article.num);
	article->author = valid_author(p_article.author, p_article.num);
	article->flags = p_article.flags;
	article->child_cnt = p_article.child_cnt;
	article->parent = valid_node(p_article.parent, count);
	article->children = valid_node(article->child_cnt ? 1 : 0, count);
	article->siblings = valid_node(p_article.siblings, count);
	if (p_article.root < 0 || p_article.root >= total.root) {
	    log_error("invalid root offset in thread file.\n");
	    return 0;
	}
	article->root = root_array[p_article.root];
	if (invalid_data) {
	    /* (Error already logged.) */
	    return 0;
	}
    }

    /* We're done with most of the pointer arrays. */
    safefree(&root_array);
    safefree(&subject_array);

    return 1;
}

/* Read the message-id strings and attach them to each article.  The data
** format consists of the mushed-together null-terminated strings (a domain
** name followed by all its unique-id prefixes) and then the article offsets
** to which they belong.  The first domain name was omitted, as it is the
** ".unknown." domain for those truly weird message-id's without '@'s.
*/
int
read_ids()
{
    register DOMAIN *domain, *last;
    register ARTICLE *article;
    register char *string_ptr;
    register int i, count;

    if (!read_item(&strings, total.string2)
     || !read_item(&ids,
		(MEM_SIZE)(total.article+total.domain+1) * sizeof (WORD))) {
	return 0;
    }
#ifdef TMPTHREAD
    wp_bmap(ids, total.article + total.domain + 1);
#endif

    string_ptr = strings;
    string_end = string_ptr + total.string2;

    if (string_end[-1] != '\0') {
	log_error("second string table is invalid.\n");
	return 0;
    }

    last = &unk_domain;
    for (i = 0, count = total.domain + 1; count--; i++) {
	if (i) {
	    if (string_ptr >= string_end) {
		log_error("error unpacking domain strings.\n");
	      free_partial:
		last->link = Null(DOMAIN*);
		article = unk_domain.ids;
		while (article) {
		    safefree(article->id);
		    article = article->id_link;
		}
		domain = unk_domain.link;
		while (domain) {
		    free(domain->name);
		    article = domain->ids;
		    while (article) {
			safefree(article->id);
			article = article->id_link;
		    }
		    last = domain;
		    domain = domain->link;
		    free(last);
		}
		return 0;
	    }
	    domain = (DOMAIN*)safemalloc(sizeof (DOMAIN));
	    give_string_to(domain->name);
	    last->link = domain;
	} else {
	    domain = &unk_domain;
	}
	if (ids[i] == -1) {
	    domain->ids = Nullart;
	} else {
	    if (ids[i] < 0 || ids[i] >= total.article) {
	      id_error:
		log_error("error in id array.\n");
		domain->ids = Nullart;
		goto free_partial;
	    }
	    article = article_array[ids[i]];
	    domain->ids = article;
	    for (;;) {
		if (string_ptr >= string_end) {
		    log_error("error unpacking domain strings.\n");
		    article->id = Nullch;
		    article->id_link = Nullart;
		    goto free_partial;
		}
		give_string_to(article->id);
		article->domain = domain;
		if (++i >= total.article + total.domain + !count) {
		    log_error("overran id array unpacking domains.\n");
		    article->id_link = Nullart;
		    goto free_partial;
		}
		if (ids[i] != -1) {
		    if (ids[i] < 0 || ids[i] >= total.article) {
			goto id_error;
		    }
		    article = article->id_link = article_array[ids[i]];
		} else {
		    article->id_link = Nullart;
		    break;
		}
	    }
	}
	last = domain;
    }
    last->link = Null(DOMAIN*);
    safefree(&ids);
    safefree(&strings);

    return 1;
}

/* And finally, point all the roots at their root articles and get rid
** of anything left over that was used to aid our unpacking.
*/
void
tweak_roots()
{
    register ROOT *root;

    for (root = root_root; root; root = root->link) {
	root->articles = article_array[root->seq];
    }
    safefree(&author_array);
    safefree(&article_array);
}

/* A shorthand for reading a chunk of the file into a malloc'ed array.
*/
int
read_item(dest, len)
char **dest;
MEM_SIZE len;
{
    int ret;

    *dest = safemalloc(len);
    ret = fread(*dest, 1, (int)len, fp_in);
    if (ret != len) {
	log_error("only read %ld bytes instead of %ld.\n",
		(long)ret, (long)len);
	free(*dest);
	*dest = Nullch;
	return 0;
    }
    return 1;
}

/* $Id: mt-process.c,v 1.3 92/01/11 16:29:02 usenet Exp $
**
** $Log:	mt-process.c,v $
 * Revision 1.3  92/01/11  16:29:02  usenet
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
 * Revision 1.2  92/01/11  16:05:13  usenet
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
#include "ndir.h"
#ifdef SERVER
#include "server.h"
#endif
#include "INTERN.h"
#include "bits.h"

#include <time.h>
#ifndef TZSET
# include <sys/timeb.h>
#endif

#if defined(SERVER) && !defined(USLEEP)
# include <sys/time.h>
#endif

#define buff buf

char references[1024];

char subject_str[80];
bool found_Re;

char author_str[20];

extern int log_verbosity, slow_down;

long num;

DOMAIN *next_domain;

void insert_article(), expire(), trim_roots(), order_roots(), trim_authors();
void make_root(), use_root(), merge_roots(), set_root(), unlink_root();
void link_child(), unlink_child();
void free_article(), free_domain(), free_subject(), free_root(), free_author();
void get_subject_str(), get_author_str();
ARTICLE *get_article();
SUBJECT *new_subject();
AUTHOR *new_author();

#ifdef TZSET
extern time_t tnow;
extern long timezone;
#else
extern struct timeb ftnow;
#endif

#ifndef SERVER
static FILE *fp_article;
#endif

/* Given the upper/lower bounds of the articles in the current group, add all
** the ones that we don't know about and remove all the ones that have expired.
** The current directory must be the newsgroup's spool directory.
*/
void
process_articles(first_article, last_article)
ART_NUM first_article, last_article;
{
    register char *cp, *str;
    register ARTICLE *article;
    register ART_NUM i;
    time_t date;
    bool has_xrefs;
    int len;
#ifdef SERVER
    bool orig_extra = extra_expire;
#endif
#ifdef TMPTHREAD
    extern int start;
#else
    int start = total.last + 1;
#endif
    extern int errno;
    extern int sys_nerr;
    extern char *sys_errlist[];

    if (first_article > start) {
	start = first_article;
    }
    added_count = last_article - start + 1;
    if (added_count < 0) {
	added_count = 0;
    } else if (added_count > 1000) {
	/* Don't overwork ourselves the first time */
	added_count = 1000;
	start = last_article - 1000 + 1;
    }
    expired_count = 0;

#ifdef TMPTHREAD
    if (added_count) {
	printf("\nThreading %d article%s...", added_count,
		added_count == 1 ? nullstr : "s"), fflush(stdout);
    }
#endif

    for (i = start; i <= last_article; i++) {
#ifdef TMPTHREAD
	if ((i - start) % 20 == 0) {
	    if (i - start) {
		printf("%d...", i - start), fflush(stdout);
	    }
	}
#endif
#ifdef SERVER
	if (slow_down) {
	    usleep(slow_down);
	}
	sprintf(buff, "HEAD %ld", (long)i);
	put_server(buff);
	if (get_server(buff, sizeof buff) < 0 || *buff == CHAR_FATAL) {
	    last_article = i - 1;
	    extra_expire = FALSE;
	    break;
	}
	if (*buff != CHAR_OK) {
	    added_count--;
	    continue;
	}
#else
	/* Open article in current directory. */
	sprintf(buff, "%ld", (long)i);
	/* Set errno for purely paranoid reasons */
	errno = 0;
	if ((fp_article = fopen(buff, "r")) == Nullfp) {
	    /* Missing files are ok -- they've just been expired or canceled */
	    if (errno != 0 && errno != ENOENT) {
		if (errno < 0 || errno > sys_nerr) {
		    log_error("Can't open `%s': Error %d.\n", buff, errno);
		} else {
		    log_error("Can't open `%s': %s.\n", buff,
		      sys_errlist[errno]);
		}
	    }
	    added_count--;
	    continue;
	}
#endif

	article = Nullart;
	*references = '\0';
	*author_str = '\0';
	*subject_str = '\0';
	found_Re = 0;
	date = 0;
	has_xrefs = FALSE;

#ifdef SERVER
	while (get_server(cp = buff, sizeof buff) == 0) {
	  process_line:
	    if (*cp == '.') {
		if (cp[1]) {
		    log_error("Header line starts with '.'! [%ld].\n",
				(long)i);
		    continue;
		}
		break;
	    }
#else
	while ((cp = fgets(buff, sizeof buff, fp_article)) != Nullch) {
	  process_line:
	    if (*cp == '\n') {		/* check for end of header */
		break;			/* break out when found */
	    }
#endif
	    if ((unsigned char)*cp <= ' ') {	 /* skip continuation lines */
		continue;		/* (except references -- see below) */
	    }
	    if ((str = index(cp, ':')) == Nullch) {
#ifdef SERVER
		if (log_verbosity) {
		    log_error("Header line missing colon! [%ld].\n", (long)i);
		}
		continue;		/* skip bogus header line */
#else
		break;			/* end of header if no colon found */
#endif
	    }
	    if ((len = str - cp) > 10) {
		continue;		/* skip keywords > 10 chars */
	    }
#ifndef SERVER
	    cp[strlen(cp)-1] = '\0';	/* remove newline */
#endif
	    while (cp < str) {		/* lower-case the keyword */
		if ((unsigned char)*cp <= ' ') { /* stop at any whitespace */
		    break;
		}
		if (isupper(*cp)) {
		    *cp = tolower(*cp);
		}
		cp++;
	    }
	    *cp = '\0';
	    cp = buff;
	    if (len == 4 && strEQ(cp, "date")) {
#ifdef TZSET
	        date = get_date(str + 1, tnow, timezone);
#else
		date = get_date(str + 1, ftnow.time, (long) ftnow.timezone);
#endif
	    } else
	    if (len == 4 && strEQ(cp, "from")) {
		get_author_str(str + 1);
	    } else
	    if (len == 4 && strEQ(cp, "xref")) {
		has_xrefs = TRUE;
	    } else
	    if (len == 7 && strEQ(cp, "subject")) {
		get_subject_str(str + 1);
	    } else
	    if (len == 10 && strEQ(cp, "message-id")) {
		if (!article) {
		    article = get_article(str + 1);
		} else {
		    if (log_verbosity) {
			log_error("Found multiple Message-IDs! [%ld].\n",
				(long)i);
		    }
		}
	    } else
	    if (len == 10 && strEQ(cp, "references")) {
		/* include preceding space in saved reference */
		len = strlen(str + 1);
		bcopy(str + 1, references, len + 1);
		str = references + len;
		/* check for continuation lines */
#ifdef SERVER
		while (get_server(cp = buff, sizeof buff) == 0) {
#else
		while ((cp = fgets(buff, sizeof buff, fp_article)) != Nullch) {
#endif
		    if (*cp != ' ' && *cp != '\t') {
			goto process_line;
		    }
		    while (*++cp == ' ' || *cp == '\t') {
			;
		    }
		    *--cp = ' ';
		    /* If the references are too long, shift them over to
		    ** always save the most recent ones.
		    */
		    if ((len += strlen(cp)) > 1023) {
			strcpy(buff, buff + len - 1023);
			str -= len - 1023;
			len = 1023;
		    }
		    strcpy(str, cp);
		}/* while */
		break;
	    }/* if */
	}/* while */
	if (article) {
	    num = i;
	    insert_article(article, date);
	    if (has_xrefs) {
		article->flags |= HAS_XREFS;
	    }
	} else {
	    if (log_verbosity) {
		log_error("Message-ID line missing! [%ld].\n", (long)i);
	    }
	}
#ifndef SERVER
	fclose(fp_article);
#endif
    }

    if (extra_expire || first_article > total.first) {
	absfirst = first_article;
	lastart = last_article;
	expire(first_article <= last_article ? extra_expire : FALSE);
    }
    trim_roots();
    order_roots();
    trim_authors();

    total.first = first_article;
    total.last = last_article;
#ifdef SERVER
    extra_expire = orig_extra;
#endif
}

/* Search all articles for numbers less than new_first.  Traverse the list
** using the domain links so we don't have to deal with the tree structure.
** If extra is true, list all articles in the directory to setup a bitmap
** with the existing articles marked as 'read', and drop everything that
** isn't there.
*/
void
expire(extra)
bool_int extra;
{
    register DOMAIN *domain;
    register ARTICLE *article, *next_art, *hold;
#ifndef TMPTHREAD
    register ART_NUM art;
#ifndef SERVER
    register DIR *dirp;
#endif
#endif

#ifdef TMPTHREAD
    extra = FALSE;
#else
    if (extra) {
      MEM_SIZE ctlsize;

	/* Allocate a bitmap large enough for absfirst thru lastart. */
#ifndef lint
	ctlsize = (MEM_SIZE)(OFFSET(lastart)/BITSPERBYTE+20);
#endif
	ctlarea = safemalloc(ctlsize);
	bzero(ctlarea, ctlsize);

	/* List all articles and use ctl_set() to keep track of what's there. */
#ifdef SERVER
	sprintf(buff, "XHDR message-id %ld-%ld", (long)absfirst, (long)lastart);
	put_server(buff);
	if (get_server(buff, sizeof buff) == 0 && *buff == CHAR_OK) {
	    while (1) {
		if (get_server(buff, sizeof buff) < 0) {
		    extra = 0;
		    break;
		}
		if (*buff == '.') {
		    break;
		}
		art = atol(buff);
		if (art >= absfirst && art <= lastart) {
		    ctl_set(art);
		}
	    }
	} else {
	    extra = 0;
	}
#else
	if ((dirp = opendir(".")) != 0) {
	  register struct DIRTYPE *dp;

	    while ((dp = readdir(dirp)) != Null(struct DIRTYPE *)) {
	      register char *p;

		for (p = dp->d_name; *p; p++) {
		    if (!isdigit(*p)) {
			goto nope;
		    }
		}
		art = atol(dp->d_name);
		if (art >= absfirst && art <= lastart) {
		    ctl_set(art);
		}
	  nope: ;
	    }
	    closedir(dirp);
	} else {
	    extra = 0;
	}
#endif
    } else {
	ctlarea = Nullch;
    }
#endif /* TMPTHREAD */

    for (domain = &unk_domain; domain; domain = next_domain) {
	next_domain = domain->link;
	for (article = domain->ids; article; article = next_art) {
	    next_art = article->id_link;
	    if (!article->subject) {
		continue;
	    }
	    if (article->num < absfirst
#ifndef TMPTHREAD
	     || (extra && !ctl_read(article->num))
#endif
	   ) {
		article->subject->count--;
		article->subject = 0;
		article->flags &= ~HAS_XREFS;
		article->author->count--;
		article->author = 0;
		/* Free expired article if it has no children.  Then check
		** if the parent(s) are also fake and can be freed.  We'll
		** free any empty roots later.
		*/
		while (!article->children) {
		    hold = article->parent;
		    unlink_child(article);
		    free_article(article);
		    if (hold && !hold->subject) {
			if ((article = hold) == next_art) {
			    next_art = next_art->id_link;
			}
		    } else {
			break;
		    }
		}
		expired_count++;
	    }/* if */
	}/* for */
    }/* for */
    next_domain = Null(DOMAIN*);

#ifndef TMPTHREAD
    safefree(&ctlarea);
#endif
}

/* Trim the article chains down so that we don't have more than one faked
** article between the root and any real ones.
*/
void
trim_roots()
{
    register ROOT *root, *last_root;
    register ARTICLE *article, *next;
    register SUBJECT *subject, *last_subj;
    register int found;

#ifndef lint
    last_root = (ROOT *)&root_root;
#else
    last_root = Null(ROOT*);
#endif
    for (root = root_root; root; root = last_root->link) {
	for (article = root->articles; article; article = article->siblings) {
	    /* If an article has no subject, it is a "fake" reference node.
	    ** If all of its immediate children are also fakes, delete it
	    ** and graduate the children to the root.  If everyone is fake,
	    ** the chain dies.
	    */
	    while (!article->subject) {
		found = 0;
		for (next = article->children; next; next = next->siblings) {
		    if (next->subject) {
			found = 1;
			break;
		    }
		}
		if (!found) {
		    /* Remove this faked article and move all its children
		    ** up to the root.
		    */
		    next = article->children;
		    unlink_child(article);
		    free_article(article);
		    for (article = next; article; article = next) {
			next = article->siblings;
			article->parent = Nullart;
			link_child(article);
		    }
		    article = root->articles;	/* start this root over */
		} else {
		    break;			/* else, on to next article */
		}
	    }
	}
	/* Free all unused subject strings.  Begin by trying to find a
	** subject for the root's pointer.
	*/
	for (subject = root->subjects; subject && !subject->count; subject = root->subjects) {
	    root->subjects = subject->link;
	    free_subject(subject);
	    root->subject_cnt--;
	}
	/* Then free up any unused intermediate subjects.
	*/
	if ((last_subj = subject) != Null(SUBJECT*)) {
	    while ((subject = subject->link) != Null(SUBJECT*)) {
		if (!subject->count) {
		    last_subj->link = subject->link;
		    free_subject(subject);
		    root->subject_cnt--;
		    subject = last_subj;
		} else {
		    last_subj = subject;
		}
	    }
	}
	/* Now, free all roots without articles.  Flag unexpeced errors.
	*/
	if (!root->articles) {
	    if (root->subjects) {
		log_error("** Empty root still had subjects remaining! **\n");
	    }
	    last_root->link = root->link;
	    free_root(root);
	} else {
	    last_root = root;
	}
    }
}

/* Descend the author list, find any author names that aren't used
** anymore and free them.
*/
void
trim_authors()
{
    register AUTHOR *author, *last_author;

#ifndef lint
    last_author = (AUTHOR *)&author_root;
#else
    last_author = Null(AUTHOR*);
#endif
    for (author = author_root; author; author = last_author->link) {
	if (!author->count) {
	    last_author->link = author->link;
	    free_author(author);
	} else {
	    last_author = author;
	}
    }
}

/* Reorder the roots to place the oldest ones first (age determined by
** date of oldest article).
*/
void
order_roots()
{
    register ROOT *root, *next, *search;

    /* If we don't have at least two roots, we're done! */
    if (!(root = root_root) || !(next = root->link)) {
	return;						/* RETURN */
    }
    /* Break the old list off after the first root, and then start
    ** inserting the roots into the list by date.
    */
    root->link = Null(ROOT*);
    while ((root = next) != Null(ROOT*)) {
	next = next->link;
	if ((search = root_root)->articles->date >= root->articles->date) {
	    root->link = root_root;
	    root_root = root;
	} else {
	    while (search->link
	     && search->link->articles->date < root->articles->date) {
		search = search->link;
	    }
	    root->link = search->link;
	    search->link = root;
	}
    }
}

#define EQ(x,y) ((isupper(x) ? tolower(x) : (x)) == (y))

/* Parse the subject into 72 characters or less.  Remove any "Re[:^]"s from
** the front (noting that it's there), and any "(was: old)" stuff from
** the end.  Then, compact multiple whitespace characters into one space,
** trimming leading/trailing whitespace.  If it's still too long, unmercifully
** cut it off.  We don't bother with subject continuation lines either.
*/
void
get_subject_str(str)
register char *str;
{
    register char *cp;
    register int len;

    while (*str && (unsigned char)*str <= ' ') {
	str++;
    }
    if (!*str) {
	bcopy("<None>", subject_str, 7);
	return;						/* RETURN */
    }
    cp = str;
    while (EQ(cp[0], 'r') && EQ(cp[1], 'e')) {	/* check for Re: */
	cp += 2;
	if (*cp == '^') {				/* allow Re^2: */
	    while (*++cp <= '9' && *cp >= '0') {
		;
	    }
	}
	if (*cp != ':') {
	    break;
	}
	while (*++cp == ' ') {
	    ;
	}
	found_Re = 1;
	str = cp;
    }
    /* Remove "(was: oldsubject)", because we already know the old subjects.
    ** Also match "(Re: oldsubject)".  Allow possible spaces after the ('s.
    */
    for (cp = str; (cp = index(cp+1, '(')) != Nullch;) {
	while (*++cp == ' ') {
	    ;
	}
	if (EQ(cp[0], 'w') && EQ(cp[1], 'a') && EQ(cp[2], 's')
	 && (cp[3] == ':' || cp[3] == ' '))
	{
	    *--cp = '\0';
	    break;
	}
	if (EQ(cp[0], 'r') && EQ(cp[1], 'e')
	 && ((cp[2]==':' && cp[3]==' ') || (cp[2]=='^' && cp[4]==':'))) {
	    *--cp = '\0';
	    break;
	}
    }
    /* Copy subject to a temporary string, compacting multiple spaces/tabs */
    for (len = 0, cp = subject_str; len < 72 && *str; len++) {
	if ((unsigned char)*str <= ' ') {
	    while (*++str && (unsigned char)*str <= ' ') {
		;
	    }
	    *cp++ = ' ';
	} else {
	    *cp++ = *str++;
	}
    }
    if (cp[-1] == ' ') {
	cp--;
    }
    *cp = '\0';
}

/* Try to fit the author name in 16 bytes.  Use the comment portion in
** parenthesis if present.  Cut off non-commented names at the '@' or '%'.
** Then, put as many characters as we can into the 16 bytes, packing multiple
** whitespace characters into a single space.
** We might want to implement a nice name shortening algorithm sometime.
*/
void
get_author_str(str)
char *str;
{
    register char *cp, *cp2;

    if ((cp = index(str, '(')) != Nullch) {
	str = cp+1;
	if ((cp = rindex(str, ')')) != Nullch) {
	    *cp = '\0';
	}
    } else {
	if ((cp = index(str, '@')) != Nullch) {
	    *cp = '\0';
	}
	if ((cp = index(str, '%')) != Nullch) {
	    *cp = '\0';
	}
    }
    for (cp = str, cp2 = author_str; *cp && cp2-author_str < 16;) {
	/* Pack white space and turn ctrl-chars into spaces. */
	if (*cp <= ' ') {
	    while (*++cp && *cp <= ' ') {
		;
	    }
	    if (cp2 != author_str) {
		*cp2++ = ' ';
	    }
	} else {
	    *cp2++ = *cp++;
	}
    }
    *cp2 = '\0';
}

/* Take a message-id and see if we already know about it.  If so, return it.
** If not, create it.  We separate the id into its id@domain parts, and
** link all the unique ids to one copy of the domain portion.  This saves
** a bit of space.
*/
ARTICLE *
get_article(msg_id)
char *msg_id;
{
    register DOMAIN *domain;
    register ARTICLE *article;
    register char *cp, *after_at;

    /* Take message id, break it up into <id@domain>, and try to match it.
    */
    while (*msg_id == ' ') {
	msg_id++;
    }
    cp = msg_id + strlen(msg_id) - 1;
    if (msg_id >= cp) {
	if (log_verbosity) {
	    log_error("Message-ID is empty! [%ld]\n", num);
	}
	return Nullart;
    }
    if (*msg_id++ != '<') {
	if (log_verbosity) {
	    log_error("Message-ID doesn't start with '<' [%ld]\n", num);
	}
	msg_id--;
    }
    if (*cp != '>') {
	if (log_verbosity) {
	    log_error("Message-ID doesn't end with '>' [%ld]\n", num);
	}
	cp++;
    }
    *cp = '\0';
    if (msg_id == cp) {
	if (log_verbosity) {
	    log_error("Message-ID is null! [%ld]\n", num);
	}
	return Nullart;
    }

    if ((after_at = index(msg_id, '@')) == Nullch) {
	domain = &unk_domain;
    } else {
	*after_at++ = '\0';
	for (cp = after_at; *cp; cp++) {
	    if (isupper(*cp)) {
		*cp = tolower(*cp);		/* lower-case domain portion */
	    }
	}
	*cp = '\0';
	/* Try to find domain name in database. */
	for (domain = unk_domain.link; domain; domain = domain->link) {
	    if (strEQ(domain->name, after_at)) {
		break;
	    }
	}
	if (!domain) {		/* if domain doesn't exist, create it */
	  register int len = cp - after_at + 1;
	    domain = (DOMAIN *)safemalloc(sizeof (DOMAIN));
	    total.domain++;
	    domain->name = safemalloc(len);
	    total.string2 += len;
	    bcopy(after_at, domain->name, len);
	    domain->ids = Nullart;
	    domain->link = unk_domain.link;
	    unk_domain.link = domain;
	}
    }
    /* Try to find id in this domain. */
    for (article = domain->ids; article; article = article->id_link) {
	if (strEQ(article->id, msg_id)) {
	    break;
	}
    }
    if (!article) {		/* If it doesn't exist, create an article */
      register int len = strlen(msg_id) + 1;
	article = (ARTICLE *)safemalloc(sizeof (ARTICLE));
	bzero(article, sizeof (ARTICLE));
	total.article++;
	article->num = 0;
	article->id = safemalloc(len);
	total.string2 += len;
	bcopy(msg_id, article->id, len);
	article->domain = domain;
	article->id_link = domain->ids;
	domain->ids = article;
    }
    return article;
}

/* Take all the data we've accumulated about the article and shove it into
** the article tree at the best place we can possibly imagine.
*/
void
insert_article(article, date)
ARTICLE *article;
time_t date;
{
    register ARTICLE *node, *last;
    register char *cp, *end;
    int len;

    if (article->subject) {
	if (log_verbosity) {
	    log_error("We've already seen article #%ld (%s@%s)\n",
		num, article->id, article->domain->name);
	}
	return;						/* RETURN */
    }
    article->date = date;
    article->num = num;
    article->flags = 0;

    if (!*references && found_Re) {
	if (log_verbosity > 1) {
	    log_error("Missing reference line!  [%ld]\n", num);
	}
    }
    /* If the article has a non-zero root, it is already in a thread somewhere.
    ** Unlink it to try to put it in the best possible spot.
    */
    if (article->root) {
	/* Check for a real or shared-fake parent.  Articles that have never
	** existed have a num of 0.  Expired articles that remain as references
	** have a valid num.  (Valid date too, but no subject.)
	*/
	for (node = article->parent;
	     node && !node->num && node->child_cnt == 1;
	     node = node->parent)
	{
	    ;
	}
	unlink_child(article);
	if (node) {			/* do we have decent parents? */
	    /* Yes: assume that our references are ok, and just reorder us
	    ** with our siblings by date.
	    */
	    link_child(article);
	    use_root(article, article->root);
	    /* Freshen the date in any faked parent articles. */
	    for (node = article->parent;
		 node && !node->num && date < node->date;
		 node = node->parent)
	    {
		node->date = date;
		unlink_child(node);
		link_child(node);
	    }
	    return;					/* RETURN */
	}
	/* We'll assume that this article has as good or better references
	** than the child that faked us initially.  Free the fake reference-
	** chain and process our references as usual.
	*/
	for (node = article->parent; node; node = node->parent) {
	    unlink_child(node);
	    free_article(node);
	}
	article->parent = Nullart;		/* neaten up */
	article->siblings = Nullart;
    }
  check_references:
    if (!*references) {	/* If no references but "Re:" in subject, */
	if (found_Re) {	/* search for a reference in any cited text */
#ifndef SERVER
	    for (len = 4; len && fgets(buff, sizeof buff, fp_article); len--) {
		if ((cp = index(buff, '<')) && (end = index(cp, ' '))) {
		    if (end[-1] == ',') {
			end--;
		    }
		    *end = '\0';
		    if ((end = index(cp, '>')) == Nullch) {
			end = cp + strlen(cp) - 1;
		    }
		    if (valid_message_id(cp, end)) {
			strcpy(references+1, cp);
			*references = ' ';
			if (log_verbosity > 2) {
			    log_error("Found cited-text reference: '%s' [%ld]\n",
				references+1, num);
			}
			break;
		    }
		}
	    }
#endif
	} else {
	    article->flags |= ROOT_ARTICLE;
	}
    }
    /* If we have references, process them from the right end one at a time
    ** until we either run into somebody, or we run out of references.
    */
    if (*references) {
	last = article;
	node = Nullart;
	end = references + strlen(references) - 1;
	while ((cp = rindex(references, '<')) != Nullch) {
	    while (end >= cp && ((unsigned char)*end <= ' ' || *end == ',')) {
		end--;
	    }
	    end[1] = '\0';
	    /* Quit parsing references if this one is garbage. */
	    if (!valid_message_id(cp, end)) {
		if (log_verbosity) {
		    log_error("Bad ref '%s' [%ld]\n", cp, num);
		}
		break;
	    }
	    /* Dump all domains that end in '.', such as "..." & "1@DEL." */
	    if (end[-1] == '.') {
		break;
	    }
	    node = get_article(cp);
	    *cp = '\0';

	    /* Check for duplicates on the reference line.  Brand-new data has
	    ** no date.  Data we just allocated earlier on this line has a
	    ** date but no root.  Special-case the article itself, since it
	    ** MIGHT have a root.
	    */
	    if ((node->date && !node->root) || node == article) {
		if (log_verbosity) {
		    log_error("Reference line contains duplicates [%ld]\n",
			num);
		}
		if ((node = last) == article) {
		    node = Nullart;
		}
		continue;
	    }
	    last->parent = node;
	    link_child(last);
	    if (node->root) {
		break;
	    }
	    node->date = date;
	    last = node;
	    end = cp-1;
	}
	if (!node) {
	    *references = '\0';
	    goto check_references;
	}
	/* Check if we ran into anybody that was already linked.  If so, we
	** just use their root.
	*/
	if (node->root) {
	    /* See if this article spans the gap between what we thought
	    ** were two different roots.
	    */
	    if (article->root && article->root != node->root) {
		merge_roots(node->root, article->root);
		/* Set the roots of any children we brought with us. */
		set_root(article, node->root);
	    }
	    use_root(article, node->root);
	} else {
	    /* We didn't find anybody we knew, so either create a new root or
	    ** use the article's root if it was previously faked.
	    */
	    if (!article->root) {
		make_root(node);
		use_root(article, node->root);
	    } else {
		node->root = article->root;
		link_child(node);
		use_root(article, article->root);
	    }
	}
	/* Set the roots of the faked articles we created as references. */
	for (node = article->parent; node && !node->root; node = node->parent) {
	    node->root = article->root;
	}
	/* Make sure we didn't circularly link to a child article(!), by
	** ensuring that we run into the root before we run into ourself.
	*/
	while (node && node->parent != article) {
	    node = node->parent;
	}
	if (node) {
	    /* Ugh.  Someone's tweaked reference line with an incorrect
	    ** article-order arrived first, and one of our children is
	    ** really one of our ancestors. Cut off the bogus child branch
	    ** right where we are and link it to the root.
	    */
	    if (log_verbosity) {
		log_error("Found ancestral child -- fixing.\n");
	    }
	    unlink_child(node);
	    node->parent = Nullart;
	    link_child(node);
	}
    } else {
	/* The article has no references.  Either turn it into a new root, or
	** re-attach fleshed-out (previously faked) article to its old root.
	*/
	if (!article->root) {
	    make_root(article);
	} else {
	    link_child(article);
	    use_root(article, article->root);
	}
    }
}

/* Check if the string we've found looks like a valid message-id reference.
*/
int
valid_message_id(start, end)
register char *start, *end;
{
    char *mid;

    if (start == end) {
	return 0;
    }

    if (*end != '>') {
	/* Compensate for space cadets who include the header in their
	** subsitution of all '>'s into another citation character.
	*/
	if (*end == '<' || *end == '-' || *end == '!' || *end == '%'
	 || *end == ')' || *end == '|' || *end == ':' || *end == '}'
	 || *end == '*' || *end == '+' || *end == '#' || *end == ']'
	 || *end == '@' || *end == '$') {
	    if (log_verbosity) {
		log_error("Reference ended in '%c' [%ld]\n", *end, num);
	    }
	    *end = '>';
	}
    } else if (end[-1] == '>') {
	if (log_verbosity) {
	    log_error("Reference ended in '>>' [%ld]\n", num);
	}
	*(end--) = '\0';
    }
    /* Id must be "<...@...>" */
    if (*start != '<' || *end != '>' || (mid = index(start, '@')) == Nullch
     || mid == start+1 || mid+1 == end) {
	return 0;					/* RETURN */
    }
    return 1;
}

/* Remove an article from its parent/siblings.  Leave parent pointer intact.
*/
void
unlink_child(child)
register ARTICLE *child;
{
    register ARTICLE *last;

    if (!(last = child->parent)) {
	child->root->thread_cnt--;
	if ((last = child->root->articles) == child) {
	    child->root->articles = child->siblings;
	} else {
	    goto sibling_search;
	}
    } else {
	last->child_cnt--;
	if (last->children == child) {
	    last->children = child->siblings;
	} else {
	    last = last->children;
	  sibling_search:
	    while (last->siblings != child) {
		last = last->siblings;
	    }
	    last->siblings = child->siblings;
	}
    }
}

/* Link an article to its parent article.  If its parent pointer is zero,
** link it to its root.  Sorts siblings by date.
*/
void
link_child(child)
register ARTICLE *child;
{
    register ARTICLE *node;
    register ROOT *root;

    if (!(node = child->parent)) {
	root = child->root;
	root->thread_cnt++;
	node = root->articles;
	if (!node || child->date < node->date) {
	    child->siblings = node;
	    root->articles = child;
	} else {
	    goto sibling_search;
	}
    } else {
	node->child_cnt++;
	node = node->children;
	if (!node || child->date < node->date) {
	    child->siblings = node;
	    child->parent->children = child;
	} else {
	  sibling_search:
	    for (; node->siblings; node = node->siblings) {
		if (node->siblings->date > child->date) {
		    break;
		}
	    }
	    child->siblings = node->siblings;
	    node->siblings = child;
	}
    }
}

/* Create a new root for the specified article.  If the current subject_str
** matches any pre-existing root's subjects, we'll instead add it on as a
** parallel thread.
*/
void
make_root(article)
ARTICLE *article;
{
    register ROOT *new, *node;
    register SUBJECT *subject;

#ifndef NO_SUBJECT_MATCHING
    /* First, check the other root's subjects for a match. */
    for (node = root_root; node; node = node->link) {
	for (subject = node->subjects; subject; subject = subject->link) {
	    if (subject_equal(subject->str, subject_str)) {
		use_root(article, node);		/* use it instead */
		link_child(article);
		return;					/* RETURN */
	    }
	}
    }
#endif

    /* Create a new root. */
    new = (ROOT *)safemalloc(sizeof (ROOT));
    total.root++;
    new->articles = article;
    new->root_num = article->num;
    new->thread_cnt = 1;
    if (article->num) {
	article->author = new_author();
	new->subject_cnt = 1;
	new->subjects = article->subject = new_subject();
    } else {
	new->subject_cnt = 0;
	new->subjects = Null(SUBJECT*);
    }
    article->root = new;
    new->link = root_root;
    root_root = new;
}

/* Add this article's subject onto the indicated root's list.  Point the
** article at the root.
*/
void
use_root(article, root)
ARTICLE *article;
ROOT *root;
{
    register SUBJECT *subject;
    register ROOT *root2;
    SUBJECT *hold, *child_subj = Null(SUBJECT*), *sib_subj = Null(SUBJECT*);
    ARTICLE *node;

    article->root = root;

    /* If it's a fake, there's no subject to add. */
    if (!article->num) {
	return;						/* RETURN */
    }

    /* If we haven't picked a unique message number to represent this root,
    ** use the first non-zero number we encounter.  Which one doesn't matter.
    */
    if (!root->root_num) {
	root->root_num = article->num;
    }
    article->author = new_author();

    /* Check if the new subject matches any of the other subjects in this root.
    ** If so, we just update the count.  If not, check all the other roots for
    ** a match.  If found, the new subject is common between the two roots, so
    ** we merge the two roots together.
    */
    root2 = root;
#ifndef NO_SUBJECT_MATCHING
    do {
#endif
	for (subject = root2->subjects; subject; subject = subject->link) {
	    if (subject_equal(subject->str, subject_str)) {
		article->subject = subject;
		subject->count++;
#ifndef NO_SUBJECT_MATCHING
		if (root2 != root) {
		    merge_roots(root, root2);
		}
#endif
		return;					/* RETURN */
	    }
	}
#ifndef NO_SUBJECT_MATCHING
	if ((root2 = root2->link) == Null(ROOT*)) {
	    root2 = root_root;
	}
    } while (root2 != root);
#endif

    article->subject = hold = new_subject();
    root->subject_cnt++;

    /* Find the subject of any pre-existing children or siblings.  We want
    ** to insert the new subject before one of these to keep the numbering
    ** intuitive in the newsreader.  Never insert prior to our parent's
    ** subject, however.
    */
    for (node = article->children; node; node = node->children) {
	if (node->subject) {
	    child_subj = node->subject;
	    break;
	}
    }
    for (node = article->siblings; node; node = node->siblings) {
	if (node->subject) {
	    sib_subj = node->subject;
	    break;
	}
    }
    if (article->parent) {
	if (article->parent->subject == child_subj) {
	    child_subj = Null(SUBJECT*);
	}
	if (article->parent->subject == sib_subj) {
	    sib_subj = Null(SUBJECT*);
	}
    }
    if (!(subject = root->subjects)
     || subject == child_subj || subject == sib_subj) {
	hold->link = root->subjects;
	root->subjects = hold;
    } else {
	while (subject->link
	 && subject->link != child_subj && subject->link != sib_subj) {
	    subject = subject->link;
	}
	hold->link = subject->link;
	subject->link = hold;
    }
}

/* Check subjects in a case-insignificant, punctuation-ignoring manner.
*/
int
subject_equal(str1, str2)
register char *str1, *str2;
{
    register char ch1, ch2;

    while ((ch1 = *str1++)) {
	if (ch1 == ' ' || ispunct(ch1)) {
	    while (*str1 && (*str1 == ' ' || ispunct(*str1))) {
		str1++;
	    }
	    ch1 = ' ';
	} else if (isupper(ch1)) {
	    ch1 = tolower(ch1);
	}
	if (!(ch2 = *str2++)) {
	    return 0;
	}
	if (ch2 == ' ' || ispunct(ch2)) {
	    while (*str2 && (*str2 == ' ' || ispunct(*str2))) {
		str2++;
	    }
	    ch2 = ' ';
	} else if (isupper(ch2)) {
	    ch2 = tolower(ch2);
	}
	if (ch1 != ch2) {
	    return 0;
	}
    }
    if (*str2) {
	return 0;
    }
    return 1;
}

/* Create a new subject structure. */
SUBJECT *
new_subject()
{
    register int len = strlen(subject_str) + 1;
    register SUBJECT *subject;

    subject = (SUBJECT *)safemalloc(sizeof (SUBJECT));
    total.subject++;
    subject->count = 1;
    subject->link = Null(SUBJECT*);
    subject->str = safemalloc(len);
    total.string1 += len;
    bcopy(subject_str, subject->str, len);

    return subject;
}

/* Create a new author structure. */
AUTHOR *
new_author()
{
    register len = strlen(author_str) + 1;
    register AUTHOR *author, *last_author;

    last_author = Null(AUTHOR*);
    for (author = author_root; author; author = author->link) {
#ifndef DONT_COMPARE_AUTHORS	/* might like to define this to save time */
	if (strEQ(author->name, author_str)) {
	    author->count++;
	    return author;				/* RETURN */
	}
#endif
	last_author = author;
    }

    author = (AUTHOR *)safemalloc(sizeof (AUTHOR));
    total.author++;
    author->count = 1;
    author->link = Null(AUTHOR*);
    author->name = safemalloc(len);
    total.string1 += len;
    bcopy(author_str, author->name, len);

    if (last_author) {
	last_author->link = author;
    } else {
	author_root = author;
    }
    return author;
}

/* Insert all of root2 into root1, setting the proper root values and
** updating subject counts.
*/
void
merge_roots(root1, root2)
ROOT *root1, *root2;
{
    register ARTICLE *node, *next;
    register SUBJECT *subject;

    /* Remember whoever's root num is lower.  This could screw up a
    ** newsreader's kill-thread code if someone already saw the roots as
    ** being separate, but it must be done.  The newsreader code will have
    ** to handle this as best as it can.
    */
    if (root1->root_num > root2->root_num) {
	root1->root_num = root2->root_num;
    }

    for (node = root2->articles; node; node = next) {
	/* For each article attached to root2: detach it, set the branch's
	** root pointer to root1, and then attach it to root1.
	*/
	next = node->siblings;
	unlink_child(node);
	node->siblings = Nullart;
	set_root(node, root1);		/* sets children too */
	/* Link_child() depends on node->parent being null and node->root
	** being set.
	*/
	link_child(node);
    }
    root1->subject_cnt += root2->subject_cnt;
    if (!(subject = root1->subjects)) {
	root1->subjects = root2->subjects;
    } else {
	while (subject->link) {
	    subject = subject->link;
	}
	subject->link = root2->subjects;
    }
    unlink_root(root2);
    free_root(root2);
}

/* When merging roots, we need to reset all the root pointers.
*/
void
set_root(node, root)
ARTICLE *node;
ROOT *root;
{
    do {
	node->root = root;
	if (node->children) {
	    set_root(node->children, root);
	}
    } while (node = node->siblings);
}

/* Unlink a root from its neighbors. */
void
unlink_root(root)
register ROOT *root;
{
    register ROOT *node;

    if ((node = root_root) == root) {
	root_root = root->link;
    } else {
	while (node->link != root) {
	    node = node->link;
	}
	node->link = root->link;
    }
}

/* Free an article and its message-id string.  All other resources must
** already be free, and it must not be attached to any threads.
*/
void
free_article(this)
ARTICLE *this;
{
    register ARTICLE *art;

    if ((art = this->domain->ids) == this) {
	if (!(this->domain->ids = this->id_link)) {
	    free_domain(this->domain);
	}
    } else {
	while (this != art->id_link) {
	    art = art->id_link;
	}
	art->id_link = this->id_link;
    }
    total.string2 -= strlen(this->id) + 1;
    free(this->id);
    free(this);
    total.article--;
}

/* Free the domain only when its last unique id has been freed. */
void
free_domain(this)
DOMAIN *this;
{
    register DOMAIN *domain;

    if (this == (domain = &unk_domain)) {
	return;
    }
    if (this == next_domain) {	/* help expire routine skip freed domains */
	next_domain = next_domain->link;
    }
    while (this != domain->link) {
	domain = domain->link;
    }
    domain->link = this->link;
    total.string2 -= strlen(this->name) + 1;
    free(this->name);
    free(this);
    total.domain--;
}

/* Free the subject structure and its string. */
void
free_subject(this)
SUBJECT *this;
{
    total.string1 -= strlen(this->str) + 1;
    free(this->str);
    free(this);
    total.subject--;
}

/* Free a root.  It must already be unlinked. */
void
free_root(this)
ROOT *this;
{
    free(this);
    total.root--;
}

/* Free the author structure when it's not needed any more. */
void
free_author(this)
AUTHOR *this;
{
    total.string1 -= strlen(this->name) + 1;
    free(this->name);
    free(this);
    total.author--;
}

#if defined(SERVER) && !defined(USLEEP)
usleep(usec)
long usec;
{
# ifndef USELECT
    if (usec /= 1000000) {
	sleep((int)usec);
    }
# else
    struct timeval t;

    if (usec <= 0) {
	return;
    }
    t.tv_usec = usec % 1000000;
    t.tv_sec  = usec / 1000000;
    (void) select(1, 0, 0, 0, &t);
# endif
}
#endif

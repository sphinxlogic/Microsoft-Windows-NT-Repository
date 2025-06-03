/* $Id: rt-rn.c,v 1.2 92/01/11 16:06:55 usenet Exp $
**
** $Log:	rt-rn.c,v $
 * Revision 1.2  92/01/11  16:06:55  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:18  davison
** Trn Release 2.0
** 
*/

#include "EXTERN.h"
#include "common.h"
#include "term.h"
#include "final.h"
#include "util.h"
#include "bits.h"
#include "artio.h"
#include "ng.h"
#include "ngdata.h"
#include "search.h"
#include "artstate.h"
#include "backpage.h"

#ifdef USETHREADS

#include "threads.h"
#include "rthreads.h"

static void find_depth(), cache_tree(), display_tree();
static char letter();

/* Find the article structure information based on article number.
*/
void
find_article(artnum)
ART_NUM artnum;
{
    register PACKED_ARTICLE *article;
    register int i;

    if (!p_articles) {
	p_art = Nullart;
	return;
    }

    if (!p_art) {
	p_art = p_articles;
    }
    /* Start looking for the article num from our last known spot in the array.
    ** That way, if we already know where we are, we run into ourselves right
    ** away.
    */
    for (article=p_art, i=p_art-p_articles; i < total.article; article++,i++) {
	if (article->num == artnum) {
	    p_art = article;
	    return;
	}
    }
    /* Didn't find it, so search the ones before our current position.
    */
    for (article = p_articles; article != p_art; article++) {
	if (article->num == artnum) {
	    p_art = article;
	    return;
	}
    }
    p_art = Nullart;
}

static char tree_indent[] = {
    ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0,
    ' ', ' ', ' ', ' ', 0,   ' ', ' ', ' ', ' ', 0
};

char letters[] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz?";

static PACKED_ARTICLE *tree_article;

static int max_depth, max_line = -1;
static int first_depth, first_line;
static int my_depth, my_line;
static bool node_on_line;
static int node_line_cnt;

static int line_num;
static int header_indent;

static char *tree_lines[11];
static char tree_buff[128], *str;

/* Prepare tree display for inclusion in the article header.
*/
void
init_tree()
{
    register PACKED_ARTICLE *article;

    while (max_line >= 0) {		/* free any previous tree data */
	free(tree_lines[max_line--]);
    }
    tree_article = curr_p_art;

    if (!curr_p_art) {
	return;
    }
    article = p_articles + p_roots[curr_p_art->root].articles;

    max_depth = max_line = my_depth = my_line = node_line_cnt = 0;
    find_depth(article, 0);

    if (max_depth <= 5) {
	first_depth = 0;
    } else {
	if (my_depth+2 > max_depth) {
	    first_depth = max_depth - 5;
	} else if ((first_depth = my_depth - 3) < 0) {
	    first_depth = 0;
	}
	max_depth = first_depth + 5;
    }
    if (--max_line < max_tree_lines) {
	first_line = 0;
    } else {
	if (my_line + max_tree_lines/2 > max_line) {
	    first_line = max_line - (max_tree_lines-1);
	} else if ((first_line = my_line - (max_tree_lines-1)/2) < 0) {
	    first_line = 0;
	}
	max_line = first_line + max_tree_lines-1;
    }

    str = tree_buff;		/* initialize first line's data */
    *str++ = ' ';
    node_on_line = FALSE;
    line_num = 0;
    /* cache our portion of the tree */
    cache_tree(article, 0, tree_indent);

    max_depth = (max_depth-first_depth) * 5;	/* turn depth into char width */
    max_line -= first_line;			/* turn max_line into count */
    /* shorten tree if lower lines aren't visible */
    if (node_line_cnt < max_line) {
	max_line = node_line_cnt + 1;
    }
}

/* A recursive routine to find the maximum tree extents and where we are.
*/
static void
find_depth(article, depth)
PACKED_ARTICLE *article;
int depth;
{
    if (depth > max_depth) {
	max_depth = depth;
    }
    for (;;) {
	if (article == tree_article) {
	    my_depth = depth;
	    my_line = max_line;
	}
	if (article->child_cnt) {
	    find_depth(article+1, depth+1);
	} else {
	    max_line++;
	}
	if (!article->siblings) {
	    break;
	}
	article += article->siblings;
    }
}

/* Place the tree display in a maximum of 11 lines x 6 nodes.
*/
static void
cache_tree(article, depth, cp)
PACKED_ARTICLE *article;
int depth;
char *cp;
{
    int depth_mode;

    cp[1] = ' ';
    if (depth >= first_depth && depth <= max_depth) {
	cp += 5;
	depth_mode = 1;
    } else if (depth+1 == first_depth) {
	depth_mode = 2;
    } else {
	cp = tree_indent;
	depth_mode = 0;
    }
    for (;;) {
	switch (depth_mode) {
	case 1: {
	    char ch;

	    *str++ = (article->flags & ROOT_ARTICLE)? ' ' : '-';
	    if (article == tree_article) {
		*str++ = '*';
	    }
	    if (was_read(article->num)) {
		*str++ = '(';
		ch = ')';
	    } else {
		*str++ = '[';
		ch = ']';
	    }
	    if (article == recent_p_art && article != tree_article) {
		*str++ = '@';
	    }
	    *str++ = letter(article);
	    *str++ = ch;
	    if (article->child_cnt) {
		*str++ = (article->child_cnt == 1)? '-' : '+';
	    }
	    if (article->siblings) {
		*cp = '|';
	    } else {
		*cp = ' ';
	    }
	    node_on_line = TRUE;
	    break;
	}
	case 2:
	    *tree_buff = (!article->child_cnt)? ' ' :
		(article->child_cnt == 1)? '-' : '+';
	    break;
	default:
	    break;
	}
	if (article->child_cnt) {
	    cache_tree(article+1, depth+1, cp);
	    cp[1] = '\0';
	} else {
	    if (!node_on_line && first_line == line_num) {
		first_line++;
	    }
	    if (line_num >= first_line) {
		if (str[-1] == ' ') {
		    str--;
		}
		*str = '\0';
		tree_lines[line_num-first_line]
			= safemalloc(str-tree_buff + 1);
		strcpy(tree_lines[line_num - first_line], tree_buff);
		if (node_on_line) {
		    node_line_cnt = line_num - first_line;
		}
	    }
	    line_num++;
	    node_on_line = FALSE;
	}
	if (!article->siblings || line_num > max_line) {
	    break;
	}
	article += article->siblings;
	if (!article->siblings) {
	    *cp = '\\';
	}
	if (!first_depth) {
	    tree_indent[5] = ' ';
	}
	strcpy(tree_buff, tree_indent+5);
	str = tree_buff + strlen(tree_buff);
    }
}

/* Output a header line with possible tree display on the right hand side.
** Does automatic wrapping of lines that are too long.
*/
int
tree_puts(orig_line, header_line, use_underline)
char *orig_line;
ART_LINE header_line;
int use_underline;
{
    char *buf;
    register char *line, *cp, *end;
    int pad_cnt, wrap_at;
    ART_LINE start_line = header_line;
    int i;
    char ch;

    /* Make a modifiable copy of the line */
    buf = safemalloc(strlen(orig_line) + 2);  /* yes, I mean "2" */
    strcpy(buf, orig_line);
    line = buf;

    /* Change any embedded control characters to spaces */
    for (end = line; *end && *end != '\n'; end++) {
	if ((unsigned char)*end < ' ') {
	    *end = ' ';
	}
    }
    *end = '\0';

    if (!*line) {
	strcpy(line, " ");
	end = line+1;
    }

    /* If this is the first subject line, output it with a preceeding [1] */
    if (use_underline && curr_p_art && (unsigned char)*line > ' ') {
#ifdef NOFIREWORKS
	no_sofire();
#endif
	standout();
	putchar('[');
	putchar(letter(curr_p_art));
	putchar(']');
	un_standout();
	putchar(' ');
	header_indent = 4;
	line += 9;
	i = 0;
    } else {
	if (*line != ' ') {
	    /* A "normal" header line -- output keyword and set header_indent
	    ** _except_ for the first line, which is a non-standard header.
	    */
	    if (!header_line || !(cp = index(line, ':')) || *++cp != ' ') {
		header_indent = 0;
	    } else {
		*cp = '\0';
		fputs(line, stdout);
		putchar(' ');
		header_indent = ++cp - line;
		line = cp;
	    }
	    i = 0;
	} else {
	    /* Skip whitespace of continuation lines and prepare to indent */
	    while (*++line == ' ') {
		;
	    }
	    i = header_indent;
	}
    }
    for (; *line; i = header_indent) {
#ifdef CLEAREOL
	maybe_eol();
#endif
	if (i) {
	    putchar('+');
	    while (--i) {
		putchar(' ');
	    }
	}
	/* If no (more) tree lines, wrap at COLS-1 */
	if (max_line < 0 || header_line > max_line+1) {
	    wrap_at = COLS-1;
	} else {
	    wrap_at = COLS - max_depth - 5 - 3;
	}
	/* Figure padding between header and tree output, wrapping long lines */
	pad_cnt = wrap_at - (end - line + header_indent);
	if (pad_cnt <= 0) {
	    cp = line + wrap_at - header_indent - 1;
	    pad_cnt = 1;
	    while (cp > line && *cp != ' ') {
		if (*--cp == ',' || *cp == '.' || *cp == '-' || *cp == '!') {
		    cp++;
		    break;
		}
		pad_cnt++;
	    }
	    if (cp == line) {
		cp += wrap_at - header_indent;
		pad_cnt = 0;
	    }
	    ch = *cp;
	    *cp = '\0';
	    /* keep rn's backpager happy */
	    vwtary(artline, vrdary(artline - 1));
	    artline++;
	} else {
	    cp = end;
	    ch = '\0';
	}
	if (use_underline) {
	    underprint(line);
	} else {
	    fputs(line, stdout);
	}
	*cp = ch;
	/* Skip whitespace in wrapped line */
	while (*cp == ' ') {
	    cp++;
	}
	line = cp;
	/* Check if we've got any tree lines to output */
	if (wrap_at != COLS-1 && header_line <= max_line) {
	    char *cp1, *cp2;

	    do {
		putchar(' ');
	    } while (pad_cnt--);
	    /* Check string for the '*' flagging our current node
	    ** and the '@' flagging our prior node.
	    */
	    cp = tree_lines[header_line];
	    cp1 = index(cp, '*');
	    cp2 = index(cp, '@');
	    if (cp1 != Nullch) {
		*cp1 = '\0';
	    }
	    if (cp2 != Nullch) {
		*cp2 = '\0';
	    }
	    fputs(cp, stdout);
	    /* Handle standout output for '*' and '@' marked nodes, then
	    ** continue with the rest of the line.
	    */
	    while (cp1 || cp2) {
		standout();
		if (cp1 && (!cp2 || cp1 < cp2)) {
		    cp = cp1;
		    cp1 = Nullch;
		    *cp++ = '*';
		    putchar(*cp++);
		    putchar(*cp++);
		} else {
		    cp = cp2;
		    cp2 = Nullch;
		    *cp++ = '@';
		}
		putchar(*cp++);
		un_standout();
		if (*cp) {
		    fputs(cp, stdout);
		}
	    }/* while */
	}/* if */
	putchar('\n') FLUSH;
	header_line++;
    }/* for remainder of line */

    /* free allocated copy of line */
    free(buf);

    /* return number of lines displayed */
    return header_line - start_line;
}

/* Output any parts of the tree that are left to display.  Called at the
** end of each header.
*/
int
finish_tree(last_line)
ART_LINE last_line;
{
    ART_LINE start_line = last_line;

    while (last_line <= max_line) {
	artline++;
	last_line += tree_puts("+", last_line, 0);
	vwtary(artline, artpos);	/* keep rn's backpager happy */
    }
    return last_line - start_line;
}

/* Output the entire article tree for the user.
*/
void
entire_tree()
{
    int j, root;

    if (!ThreadedGroup) {
	ThreadedGroup = use_data(TRUE);
	find_article(art);
	curr_p_art = p_art;
    }
    if (check_page_line()) {
	return;
    }
    if (!p_art) {
#ifdef VERBOSE
	IF (verbose)
	    fputs("\nNo article tree to display.\n", stdout);
	ELSE
#endif
#ifdef TERSE
	    fputs("\nNo tree.\n", stdout);
#endif
    } else {
	root = p_art->root;
#ifdef NOFIREWORKS
	no_sofire();
#endif
	standout();
	printf("T%ld:\n", (long)p_roots[root].root_num);
	un_standout();
	if (check_page_line()) {
	    return;
	}
	putchar('\n');
	for (j = 0; j < p_roots[root].subject_cnt; j++) {
	    sprintf(buf, "[%c] %s\n", letters[j > 9+26+26 ? 9+26+26 : j],
		subject_ptrs[root_subjects[root]+j]);
	    if (check_page_line()) {
		return;
	    }
	    fputs(buf, stdout);
	}
	if (check_page_line()) {
	    return;
	}
	putchar('\n');
	if (check_page_line()) {
	    return;
	}
	putchar(' ');
	buf[3] = '\0';
	display_tree(p_articles+p_roots[p_art->root].articles, tree_indent);

	if (check_page_line()) {
	    return;
	}
	putchar('\n');
    }
}

/* A recursive routine to output the entire article tree.
*/
static void
display_tree(article, cp)
PACKED_ARTICLE *article;
char *cp;
{
    if (cp - tree_indent > COLS || page_line < 0) {
	return;
    }
    cp[1] = ' ';
    cp += 5;
    for (;;) {
	putchar((article->flags & ROOT_ARTICLE)? ' ' : '-');
	if (was_read(article->num)) {
	    buf[0] = '(';
	    buf[2] = ')';
	} else {
	    buf[0] = '[';
	    buf[2] = ']';
	}
	buf[1] = letter(article);
	if (article == curr_p_art) {
	    standout();
	    fputs(buf, stdout);
	    un_standout();
	} else if (article == recent_p_art) {
	    putchar(buf[0]);
	    standout();
	    putchar(buf[1]);
	    un_standout();
	    putchar(buf[2]);
	} else {
	    fputs(buf, stdout);
	}

	if (article->siblings) {
	    *cp = '|';
	} else {
	    *cp = ' ';
	}
	if (article->child_cnt) {
	    putchar((article->child_cnt == 1)? '-' : '+');
	    display_tree(article+1, cp);
	    cp[1] = '\0';
	} else {
	    putchar('\n') FLUSH;
	}
	if (!article->siblings) {
	    break;
	}
	article += article->siblings;
	if (!article->siblings) {
	    *cp = '\\';
	}
	tree_indent[5] = ' ';
	if (check_page_line()) {
	    return;
	}
	fputs(tree_indent+5, stdout);
    }
}

int
check_page_line()
{
    if (page_line < 0) {
	return -1;
    }
    if (page_line >= LINES || int_count) {
      register int cmd = -1;
	if (int_count || (cmd = get_anything())) {
	    page_line = -1;		/* disable further printing */
	    if (cmd > 0) {
		pushchar(cmd);
	    }
	    return cmd;
	}
    }
    page_line++;
    return 0;
}

/* Calculate the subject letter representation.  "Place-holder" nodes
** are marked with a ' ', others get a letter in the sequence:
**	' ', '1'-'9', 'A'-'Z', 'a'-'z', '?'
*/
static char
letter(article)
PACKED_ARTICLE *article;
{
    register int subj = article->subject;

    if (subj < 0) {
	return ' ';
    }
    subj -= root_subjects[article->root];
    if (subj < 9+26+26) {
	return letters[subj];
    }
    return '?';
}

/* Find the first unread article in the (possibly selected) root order.
*/
void
first_art()
{
    if (!ThreadedGroup) {
	art = firstart;
	return;
    }
    p_art = Nullart;
    art = lastart+1;
    follow_thread('n');
}

/* Perform a command over all or a section of the article tree.  Most of
** the option letters match commands entered from article mode:
**   n - find the next unread article after current article.
**  ^N - find the next unread article with the same subject.
**   N - goto the next article in the thread.
**   j - junk the entire thread.
**   J - junk the entire thread, chasing xrefs.
**   k - junk all articles with this same subject, chasing xrefs.
**   K - kill all this article's descendants, chasing xrefs (we know that
**	 the caller killed the current article on the way here).
**   u - mark entire thread as "unread".
**   U - mark this article and its descendants as "unread".
**   x - go through the unread articles and just chase their xrefs.
**   f - follow the thread (like 'n'), but don't attempt to find a new thread
**	 if we run off the end.
*/
void
follow_thread(cmd)
char_int cmd;
{
    int curr_subj = -1, selected;
    PACKED_ARTICLE *root_limit, *p_art_old = Nullart;
    bool subthread_flag, chase_flag;

    reread = FALSE;

    if (cmd == 'N') {
	reread = TRUE;
    }
    if (!p_art) {
	if (ThreadedGroup && art > lastart) {
	    p_art = root_limit = p_articles;
	    goto follow_root;
	}
	art++;
	return;
    }
    if (cmd == 'k' || cmd == Ctl('n')) {
	if ((curr_subj = p_art->subject) == -1) {
	    return;
	}
	p_art_old = p_art;
    }
    selected = (selected_roots[p_art->root] & 1);
    if (cmd == 'U' || cmd == 'K') {
	subthread_flag = TRUE;
	p_art_old = p_art;
    } else {
	subthread_flag = FALSE;
    }
    chase_flag = (!olden_days && (cmd == 'J' || cmd == 'k' || cmd == 'K'));

    /* Some commands encompass the entire thread */
    if (cmd == 'k' || cmd == 'j' || cmd == 'J' || cmd == 'u' || cmd == 'x') {
	p_art = p_articles + p_roots[p_art->root].articles;
	art = p_art->num;
    }
    /* The current article is already marked as read for 'K' */
    if (cmd == 'k' || cmd == 'j' || cmd == 'J') {
	if (!was_read(art) && (curr_subj < 0 || curr_subj == p_art->subject)) {
	    set_read(art, selected, chase_flag);
	}
	cmd = 'K';
    }
    if (cmd == 'u') {
	p_art_old = p_art;
	cmd = 'U';
    }
    if (cmd == 'U') {
	if (p_art->subject != -1) {
	    set_unread(art, selected);
	}
	root_article_cnts[p_art->root] = 1;
	scan_all_roots = FALSE;
    }
    if (cmd == 'x') {
	if (olden_days) {
	    return;
	}
	if ((p_art->flags & HAS_XREFS) && !was_read(art)) {
	    chase_xrefs(art, TRUE);
	}
    }
  follow_again:
    selected = (selected_roots[p_art->root] & 1);
    root_limit = upper_limit(p_art, subthread_flag);
    for (;;) {
	if (++p_art == root_limit) {
	    break;
	}
	if (!(art = p_art->num)) {
	    continue;
	}
	if (cmd == 'K' || p_art->subject == -1) {
	    if (!was_read(art)
	     && (curr_subj < 0 || curr_subj == p_art->subject)) {
		set_read(art, selected, chase_flag);
	    }
	} else if (cmd == 'U') {
	    set_unread(art, selected);
	} else if (cmd == 'x') {
	    if ((p_art->flags & HAS_XREFS) && !was_read(art)) {
		chase_xrefs(art, TRUE);
	    }
	} else if (!was_read(art)
		&& (curr_subj < 0 || curr_subj == p_art->subject)) {
	    return;
	} else if (cmd == 'N') {
	    return;
	}
    }/* for */
    if (p_art_old) {
	p_art = p_art_old;
	if (cmd == 'U' && p_art->subject != -1) {
	    art = p_art->num;
	    return;
	}
	p_art_old = Nullart;
	cmd = 'n';
	curr_subj = -1;
	subthread_flag = FALSE;
	goto follow_again;
    }
    if (cmd == 'f') {
	p_art = Nullart;
	art = lastart+1;
	return;
    }
  follow_root:
    if (root_limit != p_articles + total.article) {
	register int r;

	for (r = p_art->root; r < total.root; r++) {
	    if (!selected_root_cnt || selected_roots[r]) {
		p_art = p_articles + p_roots[r].articles;
		art = p_art->num;
		if (p_art->subject == -1 || (cmd != 'N' && was_read(art))) {
		    if (cmd != 'N') {
			cmd = 'n';
		    }
		    curr_subj = -1;
		    subthread_flag = FALSE;
		    goto follow_again;
		}
		return;
	    }
	}
    }
    if (!count_roots(FALSE) && unthreaded) {
	/* No threaded articles left -- blow everything else away */
	for (art = firstbit; art <= lastart; art++) {
	    oneless(art);
	}
	unthreaded = 0;
    }
    p_art = Nullart;
    art = lastart+1;
    if (cmd == 'N') {
	forcelast = TRUE;
	reread = FALSE;
    }
}

/* Go backward in the article tree.  Options match commands in article mode:
**    p - previous unread article.
**   ^P - previous unread article with same subject.
**    P - previous article.
*/
void
backtrack_thread(cmd)
char_int cmd;
{
    int curr_subj = -1, selected;
    PACKED_ARTICLE *root_limit, *p_art_old = Nullart;

    if (art > lastart) {
	p_art = p_articles + total.article - 1;
	root_limit = Nullart;
	goto backtrack_root;
    }
    if (!p_art) {
	art--;
	return;
    }
    if (cmd == Ctl('p')) {
	if ((curr_subj = p_art->subject) == -1) {
	    return;
	}
	p_art_old = p_art;
    }
  backtrack_again:
    selected = (selected_roots[p_art->root] & 1);
    root_limit = p_articles + p_roots[p_art->root].articles;
    for (;;) {
	if (p_art-- == root_limit) {
	    break;
	}
	if (!(art = p_art->num)) {
	    continue;
	}
	if (p_art->subject == -1) {
	    set_read(art, selected, FALSE);
	} else if (!was_read(art)
		&& (curr_subj < 0 || curr_subj == p_art->subject)) {
	    return;
	} else if (cmd == 'P') {
	    reread = TRUE;
	    return;
	}
    }/* for */
    if (p_art_old) {
	p_art = p_art_old;
	p_art_old = Nullart;
	curr_subj = -1;
	goto backtrack_again;
    }
  backtrack_root:
    if (root_limit != p_articles) {
	register int r;

	for (r = p_art->root; r >= 0; r--) {
	    if (!selected_root_cnt || selected_roots[r]) {
		art = p_art->num;
		if (cmd != 'P' && was_read(art)) {
		    goto backtrack_again;
		}
		return;
	    }
	    p_art = p_articles + p_roots[r].articles - 1;
	}
    }
    p_art = Nullart;
    art = absfirst-1;
}

/* Find the next root (first if p_art == NULL).  If roots are selected,
** only choose from selected roots.
*/
void
next_root()
{
    register int r;

    reread = FALSE;

    if (p_art) {
	r = p_art->root+1;
    } else {
	r = 0;
    }
    for (; r < total.root; r++) {
	if (!selected_root_cnt || selected_roots[r]) {
	  try_again:
	    p_art = p_articles + p_roots[r].articles;
	    art = p_art->num;
	    if (p_art->subject == -1 || (!reread && was_read(art))) {
		follow_thread(reread ? 'N' : 'f');
		if (art == lastart+1) {
		    if (scan_all_roots || selected_root_cnt
		     || root_article_cnts[r]) {
			reread = TRUE;
			goto try_again;
		    }
		    continue;
		}
	    }
	    return;
	}
    }
    p_art = Nullart;
    art = lastart+1;
    forcelast = TRUE;
}

/* Find previous root (or last if p_art == NULL).  If roots are selected,
** only choose from selected roots.
*/
void
prev_root()
{
    register int r;

    reread = FALSE;

    if (p_art) {
	r = p_art->root - 1;
    } else {
	r = total.root - 1;
    }
    for (; r >= 0; r--) {
	if (!selected_root_cnt || selected_roots[r]) {
	  try_again:
	    p_art = p_articles + p_roots[r].articles;
	    art = p_art->num;
	    if (p_art->subject == -1 || (!reread && was_read(art))) {
		follow_thread(reread ? 'N' : 'f');
		if (art == lastart+1) {
		    if (scan_all_roots || selected_root_cnt
		     || root_article_cnts[r]) {
			reread = TRUE;
			goto try_again;
		    }
		    continue;
		}
	    }
	    return;
	}
    }
    p_art = Nullart;
    art = lastart+1;
    forcelast = TRUE;
}

/* Return a pointer value that we will equal when we've reached the end of
** the current (sub-)thread.
*/
PACKED_ARTICLE *
upper_limit(artp, subthread_flag)
PACKED_ARTICLE *artp;
bool_int subthread_flag;
{
    if (subthread_flag) {
	for (;;) {
	    if (artp->siblings) {
		return artp + artp->siblings;
	    }
	    if (!artp->parent) {
		break;
	    }
	    artp += artp->parent;
	}
    }
    return p_articles + (artp->root == total.root-1 ?
	total.article : p_roots[artp->root+1].articles);
}

#endif /* USETHREADS */

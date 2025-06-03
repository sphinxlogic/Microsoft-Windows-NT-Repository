/* $Id: rt-select.c,v 1.3 92/01/11 16:30:33 usenet Exp $
**
** $Log:	rt-select.c,v $
 * Revision 1.3  92/01/11  16:30:33  usenet
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
 * Revision 1.2  92/01/11  16:07:02  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:18  davison
** Trn Release 2.0
** 
*/

#include "EXTERN.h"
#include "common.h"
#include "rn.h"
#include "rcstuff.h"
#include "term.h"
#include "final.h"
#include "util.h"
#include "help.h"
#include "bits.h"
#include "artsrch.h"
#include "ng.h"
#include "ngdata.h"
#include "ngstuff.h"

#ifdef USETHREADS

#include "threads.h"
#include "rthreads.h"

static int count_subj_lines();
static void display_subj();

/* When display mode is 'l', each author gets a separate line; when 'm', up to
** three authors share a line; when 's', no authors are displayed.
*/
static char *display_mode = select_order;
static ART_NUM article_count;
static int author_line;
static char first_two_chars[3] = { ' ', ' ', '\0' }, mask = 1;

#define MAX_SEL 64

/* Display a menu of roots for the user to choose from.  If cmd is '+'
** we display all the unread roots and allow the user to mark roots as
** selected and perform various commands upon the articles.  If cmd is
** 'U' we display all the previously read roots and allow the user to
** select which ones should be marked as unread.
*/
char
select_thread(cmd)
char_int cmd;
{
    register int i, j, cnt;
    ART_NUM art_hold = art;
    int line_cnt, screen_line, subj_line_cnt;
    int cur_root, page_root, last_root = -1;
    ART_LINE running_total, last_running;
    int last_line, got_dash;
    int max_root;
    int first, last;
    int root_line[MAX_SEL], root_hold[MAX_SEL];
    int ch, action;
    char page_char, end_char;
    char promptbuf[80];
    bool etc, clean_screen, empty_ok, displayed_status;
    char oldmode = mode;
#ifndef CONDSUB
    char tmpbuf[2];
#endif
    char *select_chars, *in_select;
    int max_cnt;

    mode = 't';
    unread_selector = (cmd == 'U');
    clear_on_stop = TRUE;
    empty_ok = FALSE;

    if (added_articles > 0)
	select_page = 0;

  select_threads:
    /* Setup for selecting articles to read or set unread */
    scan_all_roots = FALSE;
    if (unread_selector) {
	page_char = '>';
	end_char = 'Z';
	page_root = 0;
	last_root = -1;
	cmd = 0;
    } else {
	page_char = page_select;
	end_char = end_select;
	page_root = select_page;
	if (curr_p_art) {
	    last_root = curr_p_art->root;
	}
    }
    mask = unread_selector+1;

    /* Leave empty roots selected for a short time to give them a chance
    ** to Esc out of the selector if they got here by mistake.
    */
    max_root = count_roots(FALSE);

    /* If nothing to display, we're done. */
    if (!article_count && !empty_ok) {
     all_empty:
	clear_on_stop = FALSE;
	mode = oldmode;
	putchar('\n');
	if (unread_selector) {
#ifdef VERBOSE
	    IF (verbose)
		fputs("\nNo articles to set unread.\n", stdout);
	    ELSE
#endif
#ifdef TERSE
		fputs("\nNo articles.\n", stdout) FLUSH;
#endif
	    unread_selector = 0;
	    mask = 1;
	} else {
#ifdef VERBOSE
	    IF (verbose)
		fputs("\nNo unread articles to select.", stdout);
	    ELSE
#endif
#ifdef TERSE
		fputs("\nNo articles.", stdout);
#endif
#ifndef USETMPTHREAD
	    if (tobethreaded) {
		printf("  (%d article%s not yet threaded)",
		    tobethreaded, tobethreaded == 1 ? nullstr : "s") FLUSH;
	    }
#endif
	    putchar('\n');	/* let "them" FLUSH */
	}
	(void) count_roots(TRUE);
	art = art_hold;
	p_art = curr_p_art;
	return '\033';
    }
    if (unread_selector) {
	for (j = 0; j < total.root; j++) {
	    selected_roots[j] |= 4;
	}
    }
    if (page_root >= max_root) {
	ch = '<';
    } else {
	ch = '>';
    }
    cur_root = 0;
    running_total = 0;
    for (i = 0; i < page_root; i++) {
	running_total += root_article_cnts[i];
    }
    do {
	select_chars = getval("SELECTCHARS", SELECTCHARS);
	max_cnt = strlen(select_chars);
	if (max_cnt > MAX_SEL) {
	    max_cnt = MAX_SEL;
	}
	if (ch == '<' && i) {
	    screen_line = 2;
	    cnt = 0;
	    /* Scan the roots in reverse to go back a page */
	    do {
		if (!root_article_cnts[--i]) {
		    continue;
		}
		first = root_subjects[i];
		last = first + p_roots[i].subject_cnt;
		line_cnt = 0;
		for (j = first; j < last; j++) {
		    line_cnt += count_subj_lines(i, j);
		}
		if (line_cnt > LINES - 5) {
		    line_cnt = LINES - 5;
		}
		screen_line += line_cnt;
		if (screen_line > LINES - 3) {
		    i++;
		    break;
		}
		running_total -= root_article_cnts[i];
		cnt++;
	    } while (i > 0 && cnt < max_cnt);
	}

	/* Present a page of subjects to the user */
#ifndef CLEAREOL
	clear();
#else
	if (can_home_clear) {
	    home_cursor();
	    maybe_eol();
	} else {
	    clear();
	}
#endif
	carriage_return();
	page_root = i;
	last_running = running_total;
#ifdef NOFIREWORKS
	no_sofire();
#endif
	standout();
	fputs(ngname, stdout);
	un_standout();
	printf("          %ld %sarticle%s%s\n", (long)article_count,
	    unread_selector? "read " : nullstr,
	    article_count == 1 ? nullstr : "s", moderated);
#ifdef CLEAREOL
	maybe_eol();
#endif
	putchar('\n') FLUSH;
	screen_line = 2;
	for (cnt = 0; i < max_root && cnt < max_cnt; i++) {
	    if (last_root == i) {
		cur_root = cnt;
	    }
	    /* Check each root for articles to list */
	    if (!root_article_cnts[i]) {
		continue;
	    }
	    first = root_subjects[i];
	    last = first + p_roots[i].subject_cnt;

	    /* Compute how many lines we need to display the subjects/authors */
	    etc = FALSE;
	    line_cnt = 0;
	    for (j = first; j < last; j++) {
		subj_line_cnt = count_subj_lines(i, j);
		line_cnt += subj_line_cnt;
		/* If this root is too long to fit on the screen all by
		** itself, trim it to fit and set the "etc" flag.
		*/
		if (line_cnt > LINES - 5) {
		    last = j;
		    line_cnt -= subj_line_cnt;
		    if (line_cnt != LINES - 5) {
			last++;
			line_cnt = LINES - 5;
		    }
		    if (screen_line == 2) {
			etc = TRUE;
		    }
		    break;
		}
	    }
	    /* If it doesn't fit, save it for the next page */
	    if (screen_line + line_cnt > LINES - 3) {
		break;
	    }
	    /* Output the subjects, with optional authors */
	    root_line[cnt] = screen_line;
	    running_total += root_article_cnts[i];
	    first_two_chars[0] = select_chars[cnt];
	    first_two_chars[1] = (selected_roots[i] & 4) ? '-' :
				 (selected_roots[i] & mask) ? '+' : ' ';
	    author_line = screen_line;
	    for (j = first; j < last; j++) {
		display_subj(i, j);
	    }
	    screen_line += line_cnt;
	    root_hold[cnt++] = i;
	    if (etc) {
		fputs("      ...etc.", stdout);
		i++;
		break;
	    }
	}/* for */
	last_root = -1;
	if (cur_root && cur_root >= cnt) {
	    cur_root = cnt - 1;
	}

	/* Check if there is really anything left to display. */
	if (!running_total && !empty_ok) {
	    goto all_empty;
	}
	empty_ok = FALSE;

	last_line = screen_line+1;
#ifdef CLEAREOL
	maybe_eol();
#endif
	putchar('\n') FLUSH;
	/* Prompt the user */
#ifdef MAILCALL
	setmail();
#endif
	if (i != max_root) {
	    sprintf(promptbuf, "%s-- Select threads -- %s%ld%% [%c%c] --",
		mailcall, (!page_root? "Top " : nullstr),
		(long)(running_total*100 / article_count),
		page_char, end_char);
	} else {
	    sprintf(promptbuf, "%s-- Select threads -- %s [%c%c] --",
		mailcall, (!page_root? "All" : "Bot"), end_char, page_char);
	}
	if (cur_root > cnt) {
	    cur_root = 0;
	}
	screen_line = root_line[cur_root];
#ifdef CLEAREOL
	if (erase_screen && can_home_clear) {
	    clear_rest();
	}
#endif
	if (added_articles > 0) {
	    printf("\n** %d article%s been added **", added_articles,
		added_articles == 1 ? " has" : "s have");
	    displayed_status = TRUE;
	    if (can_home) {
		carriage_return();
		goto_line(screen_line+1, screen_line);
	    } else {
		putchar('\n');
	    }
	} else {
	    displayed_status = FALSE;
	}
	added_articles = 0;
      prompt_select:
	standout();
	fputs(promptbuf, stdout);
	un_standout();
	if (can_home) {
	    carriage_return();
	    goto_line(last_line, screen_line);
	}
	got_dash = 0;
	/* Grab some commands from the user */
	for (;;) {
	    fflush(stdout);
	    eat_typeahead();
#ifdef CONDSUB
	    getcmd(buf);
	    ch = *buf;
#else
	    getcmd(tmpbuf);	/* If no conditionals, don't allow macros */ 
	    ch = *tmpbuf;
	    buf[0] = ch;
	    buf[1] = FINISHCMD;
#endif
	    if (errno) {
		ch = Ctl('l');
	    }
	    in_select = index(select_chars, ch);
	    /* Plaster any inherited empty roots on first command if not Esc. */
	    if (cmd && (in_select || (ch != '\033' && ch != '+'))) {
		max_root = count_roots(TRUE);
		cmd = 0;
	    }
	    if (displayed_status && can_home) {
		goto_line(screen_line, last_line+1);
		erase_eol();
		screen_line = last_line+1;
		displayed_status = FALSE;
	    }
	    if (ch == '-') {
		got_dash = 1;
		if (!can_home) {
		    putchar('-');
		    fflush(stdout);
		}
		continue;
	    }
	    if (ch == ' ') {
		if (i == max_root) {
		    ch = end_char;
		} else {
		    ch = page_char;
		}
	    }
	    if (!in_select && (index("<+>^$!?&:/hDJLNPqQTUXZ\n\r\t\033", ch)
	     || ch == Ctl('l') || ch == Ctl('r') || ch == Ctl('k'))) {
		break;
	    }
	    if (in_select) {
		j = in_select - select_chars;
		if (j >= cnt) {
		    dingaling();
		    j = -1;
		} else if (got_dash) {
		    ;
		} else if (selected_roots[root_hold[j]] & mask) {
		    action = (unread_selector ? 'k' : '-');
		} else {
		    action = '+';
		}
	    } else if (ch == 'y' || ch == '.') {
		j = cur_root;
		if (selected_roots[root_hold[j]] & mask) {
		    action = (unread_selector ? 'k' : '-');
		} else {
		    action = '+';
		}
	    } else if (ch == 'k' || ch == 'j' || ch == ',') {
		j = cur_root;
		action = 'k';
	    } else if (ch == 'm' || ch == '\\') {
		j = cur_root;
		action = 'm';
	    } else if (ch == '@') {
		cur_root = 0;
		j = cnt-1;
		got_dash = 1;
		action = '@';
	    } else if (ch == '[' || ch == 'p') {
		if (--cur_root < 0) {
		    cur_root = cnt ? cnt-1 : 0;
		}
		j = -1;
	    } else if (ch == ']' || ch == 'n') {
		if (++cur_root >= cnt) {
		    cur_root = 0;
		}
		j = -1;
	    } else {
		if (can_home) {
		    goto_line(screen_line, last_line+1);
		    screen_line = last_line+1;
		} else {
		    putchar('\n');
		}
		printf("Type ? for help.");
		settle_down();
		displayed_status = TRUE;

		if (can_home) {
		    carriage_return();
		} else {
		    putchar('\n');
		}
		j = -1;
	    }
	    if (j >= 0) {
		if (!got_dash) {
		    cur_root = j;
		} else {
		    got_dash = 0;
		    if (j < cur_root) {
			ch = cur_root-1;
			cur_root = j;
			j = ch;
		    }
		}
		if (++j == cnt) {
		    j = 0;
		}
		do {
		  register int r;
		  register char maskr = mask;
		    r = root_hold[cur_root];
		    if (can_home) {
			goto_line(screen_line, root_line[cur_root]);
			screen_line = root_line[cur_root];
		    }
		    putchar(select_chars[cur_root]);
		    if (action == '@') {
			if (selected_roots[r] & 4) {
			    ch = (unread_selector ? '+' : ' ');
			} else if (unread_selector) {
			    ch = 'k';
			} else
			if (selected_roots[r] & maskr) {
			    ch = '-';
			} else {
			    ch = '+';
			}
		    } else {
			ch = action;
		    }
		    switch (ch) {
		    case '+':
			if (!(selected_roots[r] & maskr)) {
			    selected_roots[r] |= maskr;
			    selected_root_cnt++;
			    selected_count += root_article_cnts[r];
			    putchar('+');
			}
			/* FALL THROUGH */
		    case 'm':
			if (selected_roots[r] & 4) {
			    selected_roots[r] &= ~4;
			    if (ch == 'm') {
				putchar(' ');
			    }
			} else if (ch == 'm') {
			    goto unsel;
			}
			break;
		    case 'k':
			if (!(selected_roots[r] & 4)) {
			    selected_roots[r] |= 4;
			    putchar('-'), fflush(stdout);
			    p_art = p_articles + p_roots[r].articles;
			    art = 0;
			    follow_thread('x');
			}
			/* FALL THROUGH */
		    case '-':
		    unsel:
			if (selected_roots[r] & maskr) {
			    selected_roots[r] &= ~maskr;
			    selected_root_cnt--;
			    selected_count -= root_article_cnts[r];
			    if (ch != 'k') {
				putchar(' ');
			    }
			}
			break;
		    }
		    fflush(stdout);
		    if (++cur_root == cnt) {
			cur_root = 0;
		    }
		    if (can_home) {
			carriage_return();
		    }
		} while (cur_root != j);
	    } else {
		got_dash = FALSE;
	    }
	    if (can_home) {
		goto_line(screen_line, root_line[cur_root]);
		screen_line = root_line[cur_root];
	    }
	}/* for */
	if (can_home) {
	    goto_line(screen_line, last_line);
	}
	clean_screen = TRUE;
      do_command:
	output_chase_phrase = TRUE;
	if (ch == 'L') {
	    if (!*++display_mode) {
		display_mode = select_order;
	    }
	    ch = Ctl('l');
	    cur_root = 0;
	} else if (ch == '$') {
	    ch = '<';
	    page_root = max_root;
	    last_running = article_count;
	    cur_root = 0;
	} else if (ch == '^' || ch == Ctl('r')) {
	    ch = '>';
	    i = 0;
	    running_total = 0;
	    cur_root = 0;
	} else if (ch == 'h' || ch == '?') {
	    putchar('\n');
	    if ((ch = help_select()) || (ch = pause_getcmd())) {
		goto got_cmd;
	    }
	    ch = Ctl('l');
	} else if (index(":/&!", ch)) {
	    erase_eol();		/* erase the prompt */
	    if (!finish_command(TRUE)) {	/* get rest of command */
		if (clean_screen) {
		    screen_line = root_line[cur_root];
		    goto prompt_select;
		}
		goto extend_done;
	    }
	    if (ch == '&' || ch == '!') {
		one_command = TRUE;
		perform(buf, FALSE);
		one_command = FALSE;

		putchar('\n') FLUSH;
		clean_screen = FALSE;
	    } else {
		int selected_save = selected_root_cnt;

		if (ch == ':') {
		    clean_screen = (use_selected() == 2) && clean_screen;
		    if (!unread_selector) {
			for (j = 0; j < total.root; j++) {
			    if (selected_roots[j] & 4) {
				selected_roots[j] = 0;
				p_art = p_articles + p_roots[j].articles;
				art = 0;
				follow_thread('j');
			    }
			}
		    }
		} else {
		    /* Force the search to begin at absfirst or firstart,
		    ** depending upon whether they specified the 'r' option.
		    */
		    art = lastart+1;
		    page_line = 1;
		    switch (art_search(buf, sizeof buf, FALSE)) {
		    case SRCH_ERROR:
		    case SRCH_ABORT:
		    case SRCH_INTR:
			fputs("\nInterrupted\n", stdout) FLUSH;
			break;
		    case SRCH_DONE:
		    case SRCH_SUBJDONE:
			fputs("Done\n", stdout) FLUSH;
			break;
		    case SRCH_NOTFOUND:
			fputs("\nNot found.\n", stdout) FLUSH;
			break;
		    case SRCH_FOUND:
			break;
		    }
		    clean_screen = FALSE;
		}
		/* Recount, in case something has changed. */
		max_root = count_roots(!unread_selector);

		running_total = 0;
		if (article_count) {
		    for (j = 0; j < page_root; j++) {
			running_total += root_article_cnts[j];
		    }
		}
		cur_root = 0;

		if ((selected_save -= selected_root_cnt) != 0) {
		    putchar('\n');
		    if (selected_save < 0) {
			fputs("S", stdout);
			selected_save *= -1;
		    } else {
			fputs("Des", stdout);
		    }
		    printf("elected %d thread%s.", selected_save,
			selected_save == 1 ? nullstr : "s");
		    clean_screen = FALSE;
		}
		if (!clean_screen) {
		    putchar('\n') FLUSH;
		}
	    }/* if !& or :/ */

	    if (clean_screen) {
		carriage_return();
		up_line();
		erase_eol();
		screen_line = root_line[cur_root];
		goto prompt_select;
	    }
	  extend_done:
	    if ((ch = pause_getcmd())) {
	      got_cmd:
		if (ch > 0) {
		    /* try to optimize the screen update for some commands. */
		    if (!index(select_chars, ch)
		     && (index("<+>^$!?&:/hDJLNPqQTUXZ\n\r\t\033", ch)
		      || ch == Ctl('k'))) {
			buf[0] = ch;
			buf[1] = FINISHCMD;
			goto do_command;
		    }
		    pushchar(ch | 0200);
		}
	    }
	    ch = Ctl('l');
	} else if (ch == Ctl('k')) {
	    edit_kfile();
	    ch = Ctl('l');
	} else if (!unread_selector && (ch == 'X' || ch == 'D' || ch == 'J')) {
	    if (ch == 'D') {
		j = page_root;
		last = i;
	    } else {
		j = 0;
		last = max_root;
	    }
	    for (; j < last; j++) {
		if (((!(selected_roots[j] & 1) ^ (ch == 'J'))
		 && (cnt = root_article_cnts[j])) || (selected_roots[j] & 4)) {
		    p_art = p_articles + p_roots[j].articles;
		    art = 0;
		    follow_thread((selected_roots[j] & 4) ? 'j' : 'J');
		}
	    }
	    max_root = count_roots(TRUE);
	    if (article_count
	     && (ch == 'J' || (ch == 'D' && !selected_root_cnt))) {
		last_running = 0;
		for (i = 0; i < page_root; i++) {
		    last_running += root_article_cnts[i];
		}
		ch = Ctl('l');
		cur_root = 0;
	    } else {
		if (!output_chase_phrase)
		    ch = 'o';
		break;
	    }
	} else if (ch == 'J') {
	    for (j = 0; j < max_root; j++) {
		selected_roots[j] = (selected_roots[j] & ~2) | 4;
	    }
	    selected_root_cnt = selected_count = 0;
	    ch = Ctl('l');
	} else if (ch == 'T') {
	  register int r;

	    erase_eol();		/* erase the prompt */
	    r = root_hold[cur_root];
	    p_art = p_articles + p_roots[r].articles;
	    art = p_art->num;
	    if (p_art->subject == -1) {
		follow_thread('N');
	    }
	    perform("T", FALSE);
	    max_root = count_roots(TRUE);
	    if (article_count) {
		ch = Ctl('l');
	    }
	}
	if (ch == '>') {
	    cur_root = 0;
	    page_root = i;
	} else if (ch == '<' || (page_root && page_root >= max_root)) {
	    cur_root = 0;
	    running_total = last_running;
	    if (!(i = page_root) || !max_root) {
		ch = '>';
	    } else {
		ch = '<';
	    }
	} else if (ch == Ctl('l')) {
	    i = page_root;
	    running_total = last_running;
	    ch = '>';
	} else if (ch == '\r' || ch == '\n') {
	    if (!selected_root_cnt) {
	      register r = root_hold[cur_root];
		selected_roots[r] = mask;
		selected_root_cnt++;
		selected_count += root_article_cnts[r];
	    }
	}
    } while ((ch == '>' && i < max_root) || ch == '<');
    if (ch != 'o') {
	putchar('\n') FLUSH;
    }

    if (unread_selector) {
	/* Turn selections into unread selected roots.  Let count_roots()
	** fix the counts after we're through.
	*/
	last_root = -1;
	for (j = 0; j < total.root; j++) {
	    if (!(selected_roots[j] & 4)) {
		if (selected_roots[j] & 2) {
		    selected_roots[j] = 1;
		}
		p_art = p_articles + p_roots[j].articles;
		art = 0;
		follow_thread('u');
	    } else {
		selected_roots[j] &= ~4;
	    }
	}
    } else {
	select_page = page_root;
	for (j = 0; j < total.root; j++) {
	    if (selected_roots[j] & 4) {
		selected_roots[j] = 0;
		p_art = p_articles + p_roots[j].articles;
		art = 0;
		follow_thread('j');
	    }
	}
    }
    if (ch == 'U') {
	unread_selector = !unread_selector;
	empty_ok = TRUE;
	goto select_threads;
    }

    if (unread_selector) {
	unread_selector = 0;
	mask = 1;
	(void) count_roots(FALSE);
    }
    if (ch == 'N' || ch == 'P' || Ctl(ch) == Ctl('q') || ch == '\033') {
	art = art_hold;
	p_art = curr_p_art;
    } else {
	first_art();
    }
    clear_on_stop = FALSE;
    mode = oldmode;
    return ch;
}

static int author_cnt, first_author;

/* Counts the number of lines needed to output a subject, including optional
** authors.
*/
static int
count_subj_lines(root, subj)
int root;
int subj;
{
    PACKED_ARTICLE *artp, *root_limit;
    int author_subj;

    author_cnt = 0;
    author_subj = subj;
    first_author = -1;

    if (!subject_cnts[subj]) {
	return 0;
    }
    if (*display_mode == 's') {	/* no-author mode takes one line */
	return ++author_cnt;
    }
    bzero(author_cnts, total.author * sizeof (WORD));

    /* Count authors associated with this subject.  Increments author_cnts. */
    artp = p_articles + p_roots[root].articles;
    root_limit = upper_limit(artp, FALSE);
    for (; artp != root_limit; artp++) {
	if (artp->subject == author_subj
	 && (!was_read(artp->num) ^ unread_selector)) {
	    if (artp->author < 0 || artp->author >= total.author) {
		printf("\
Found invalid author (%d) with valid subject (%d)! [%ld]\n",
			artp->author, artp->subject, artp->num);
		artp->author = 0;
	    } else {
		if (first_author < 0) {
		    first_author = artp->author;
		}
		if (!author_cnts[artp->author]++) {
		    author_cnt++;
		}
	    }
	}
    }

    if (*display_mode == 'm') {
	return (author_cnt+4)/3;
    } else {
	return author_cnt;
    }
}

static void
display_subj(root, subj)
int root;
int subj;
{
    PACKED_ARTICLE *artp, *root_limit;
    char *str;

    count_subj_lines(root, subj);
    if (!author_cnt) {
	return;
    }
    artp = p_articles + p_roots[root].articles;
    if (artp->subject != -1 && (artp->flags & ROOT_ARTICLE)
     && (!was_read(artp->num) ^ unread_selector)) {
	str = nullstr;
    } else {
	str = ">";
    }
#ifdef CLEAREOL
    maybe_eol();
#endif
    if (*display_mode == 's') {
	printf("%s%3d  %s%.71s\n", first_two_chars,
		subject_cnts[subj], str, subject_ptrs[subj]) FLUSH;
    } else {
	printf("%s%-16.16s%3d  %s%.55s", first_two_chars,
		author_ptrs[first_author],
		subject_cnts[subj], str, subject_ptrs[subj]);
	if (author_cnt > 1) {
	    author_cnts[first_author] = 0;
	    author_cnt = 0;
	    root_limit = upper_limit(artp, FALSE);
	    for (; artp != root_limit; artp++) {
		if (artp->author >= 0 && author_cnts[artp->author]) {
		    switch (author_cnt % 3) {
		    case 0:
			putchar('\n') FLUSH;
			if (++author_line >= LINES - 3) {
			    return;
			}
#ifdef CLEAREOL
			maybe_eol();
#endif
			putchar(' ');
			putchar(' ');
			break;
		    case 1:
			putchar('\t');
			putchar('\t');
			break;
		    case 2:
			putchar('\t');
			break;
		    }
		    author_cnt += (*display_mode == 'm');
		    printf("%-16.16s", author_ptrs[artp->author]);
		    author_cnts[artp->author] = 0;
		}/* if */
	    }/* for */
	}/* if */
	putchar('\n') FLUSH;
	author_line++;
    }/* if */
    first_two_chars[0] = first_two_chars[1] = ' ';
}

/* Get each root's article count, and subject count(s); count total
** articles and selected articles (use unread_selector to determine
** whether to count read or unread articles); deselect any roots we
** find that are empty (if do_unselect is TRUE); find the last non-
** empty root, and return its count (the index+1).
*/
int
count_roots(do_unselect)
bool_int do_unselect;
{
    register int count;
    register PACKED_ARTICLE *artp, *root_limit, *art_limit;
    int last_root = -1;

    article_count = selected_count = selected_root_cnt = 0;

    if (!(artp = p_articles)) {
	return 0;
    }
    art_limit = artp + total.article;
    root_limit = upper_limit(artp, 0);

    bzero(subject_cnts, total.subject * sizeof (WORD));
    count = 0;

    for (;;) {
	if (artp->subject == -1) {
	    if (!was_read(artp->num)) {
		oneless(artp->num);
	    }
	} else if ((!was_read(artp->num) ^ unread_selector)) {
	    count++;
	    subject_cnts[artp->subject]++;
	}
	if (++artp == root_limit) {
	    register int root_num = artp[-1].root;
	    register char maskr = mask;

	    root_article_cnts[root_num] = count;
	    if (count) {
		article_count += count;
		if (selected_roots[root_num] & maskr) {
		    selected_roots[root_num] &= ~4;
		    selected_root_cnt++;
		    selected_count += count;
		}
		last_root = root_num;
	    } else if (do_unselect) {
		selected_roots[root_num] &= ~maskr;
	    } else if (selected_roots[root_num] & maskr) {
		selected_roots[root_num] &= ~4;
		selected_root_cnt++;
	    }
	    if (artp == art_limit) {
		break;
	    }
	    root_limit = upper_limit(artp, 0);
	    count = 0;
	}
    }
    if (do_unselect) {
	scan_all_roots = !article_count;
    }
    unthreaded = toread[ng] - article_count;

    return last_root+1;
}

/* Count the unread articles attached to the given root number.
*/
int
count_one_root(root_num)
int root_num;
{
    int last = (root_num == total.root-1 ? total.article
					 : p_roots[root_num+1].articles);
    register int count = 0, i;

    for (i = p_roots[root_num].articles; i < last; i++) {
	if (p_articles[i].subject != -1 && !was_read(p_articles[i].num)) {
	    count++;
	}
    }
    root_article_cnts[root_num] = count;

    return count;
}

#endif /* USETHREADS */

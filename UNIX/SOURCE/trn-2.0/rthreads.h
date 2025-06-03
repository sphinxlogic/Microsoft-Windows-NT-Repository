/* $Id: rthreads.h,v 1.3 92/01/11 16:30:39 usenet Exp $
**
** $Log:	rthreads.h,v $
 * Revision 1.3  92/01/11  16:30:39  usenet
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
 * Revision 1.2  92/01/11  16:08:33  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:18  davison
** Trn Release 2.0
** 
*/

EXT TOTAL total;

EXT PACKED_ROOT *p_roots INIT(0);
EXT WORD *root_subjects INIT(0);
EXT WORD *author_cnts INIT(0);
EXT WORD *subject_cnts INIT(0);
EXT char **author_ptrs INIT(0);
EXT char **subject_ptrs INIT(0);
EXT PACKED_ARTICLE *p_articles INIT(0);
EXT WORD *root_article_cnts INIT(0);
EXT char *selected_roots INIT(0);
EXT ART_NUM *saved_selections INIT(0);
EXT bool unread_selector INIT(0);

EXT PACKED_ARTICLE *p_art INIT(0);
EXT PACKED_ARTICLE *curr_p_art INIT(0);
EXT PACKED_ARTICLE *recent_p_art INIT(0);

EXT int selected_root_cnt INIT(0);
EXT ART_NUM selected_count INIT(0);
EXT int added_articles INIT(0);
EXT int unthreaded INIT(0);
EXT int select_page;
EXT bool scan_all_roots;
EXT bool output_chase_phrase;

#ifndef USETMPTHREAD
EXT int tobethreaded INIT(0);
#endif

void thread_init ANSI((void));
char *safemalloc ANSI((MEM_SIZE));
int use_data ANSI((bool_int));
void unuse_data ANSI((bool_int));
void find_article ANSI((ART_NUM));
void init_tree ANSI((void));
void entire_tree ANSI((void));
int tree_puts ANSI((char *,ART_LINE,int));
int finish_tree ANSI((ART_LINE));
void first_art ANSI((void));
void follow_thread ANSI((char_int));
void backtrack_thread ANSI((char_int));
void next_root ANSI((void));
void prev_root ANSI((void));
char select_thread ANSI((char_int));
int count_roots ANSI((bool_int));
int count_one_root ANSI((int));
PACKED_ARTICLE *upper_limit ANSI((PACKED_ARTICLE *,bool_int));

#define Nullart Null(PACKED_ARTICLE*)

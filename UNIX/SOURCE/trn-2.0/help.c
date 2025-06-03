/* $Id: help.c,v 1.1 92/01/11 15:50:32 usenet Stab $
 *
 * $Log:	help.c,v $
 * Revision 1.1  92/01/11  15:50:32  usenet
 * USENET News Software
 * 
 * Revision 4.4  1991/09/09  20:18:23  sob
 * release 4.4
 *
 *
 * 
 */
/* This software is Copyright 1991 by Stan Barber. 
 *
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 * is at the user's own risk. 
 */

#include "EXTERN.h"
#include "common.h"
#include "rn.h"
#include "term.h"
#include "INTERN.h"
#include "help.h"

void
help_init()
{
    ;
}

int
help_page()
{
    int cmd;

#ifdef PAGERHELP
    doshell(sh,filexp(PAGERHELP));
#else
    page_init();
    if ((cmd = print_lines("\
Paging commands:\n\
",STANDOUT)) ||
    (cmd = print_lines("\n\
SP	Display the next page.\n\
x	Display the next page decrypted (rot13).\n\
d	Display half a page more.\n\
CR	Display one more line.\n\
^R,v,^X	Restart the current article (v=verbose header, ^X=rot13).\n\
",NOMARKING)) ||
    (cmd = print_lines("\
b	Back up one page.\n\
^L,X	Refresh the screen (X=rot13).\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
t	Display the entire article tree and all its subjects.\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
g pat	Go to (search forward within article for) pattern.\n\
G	Search again for current pattern within article.\n\
^G	Search for next line beginning with \"Subject:\".\n\
TAB	Search for next line beginning with a different character.\n\
q	Quit the pager, go to end of article.  Leave article read or unread.\n\
j	Junk this article (mark it read).  Goes to end of article.\n\
\n\
",NOMARKING)) ||
    (cmd = print_lines("\
The following commands skip the rest of the current article, then behave\n\
just as if typed to the 'What next?' prompt at the end of the article:\n\
",STANDOUT)) ||
    (cmd = print_lines("\n\
n	Scan forward for next unread article.\n\
N	Go to next article.\n\
^N	Scan forward for next unread article with same title.\n\
p,P,^P	Same as n,N,^N, only going backwards.\n\
-	Go to previously displayed article.\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
<,>	Browse the previous/next selected thread.  If no threads are selected,\n\
	all threads that had unread news upon entry to the group are considered\n\
	selected for browsing.  Entering an empty group browses all threads.\n\
[,],{,} Go to parent/child/root/leaf in thread.\n\
\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
The following commands also take you to the end of the article.\n\
Type h at end of article for a description of these commands:\n\
",STANDOUT)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
	# $ & / = ? c C f F k K ^K J , m M number e r R ^R s S u U v w W Y ^ |\n\
\n\
(To return to the middle of the article after one of these commands, type ^L.)\n\
",NOMARKING)) )
#else
    (cmd = print_lines("\
	# $ & / = ? c C f F k K ^K m M number e r R ^R s S u v w W Y ^ |\n\
\n\
(To return to the middle of the article after one of these commands, type ^L.)\n\
",NOMARKING)) )
#endif
	return cmd;
#endif
    return 0;
}

int
help_art()
{
    int cmd;
#ifdef ARTHELP
    doshell(sh,filexp(ARTHELP));
#else
    page_init();
    if ((cmd = print_lines("\
Article Selection commands:\n\
",STANDOUT)) ||
#ifdef USETHREADS
    (cmd = print_lines("\n\
n,SP	Find next unread article (follows discussion-tree in threaded groups).\n\
",NOMARKING)) ||
#else
    (cmd = print_lines("\n\
n,SP	Scan forward for next unread article.\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
N	Go to next article.\n\
^N	Scan forward for next unread article with same subject.\n\
p,P,^P	Same as n,N,^N, only going backwards.\n\
-	Go to previously displayed article.\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
<,>	Browse the previous/next selected thread.  If no threads are selected,\n\
	all threads that had unread news upon entry to the group are considered\n\
	selected for browsing.  Entering an empty group browses all threads.\n\
[,]	Go to article's parent/child.\n\
{,}	Go to tree's root/leaf.\n\
t	Display the entire article tree and all its subjects.\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
number	Go to specified article.\n\
range{,range}:command{:command}\n\
	Apply one or more commands to one or more ranges of articles.\n\
	Ranges are of the form: number | number-number.  You may use . for\n\
	the current article, and $ for the last article.\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
 	Valid commands are: e, j, m, M, s, S, t, T, |, +, and -.\n\
:cmd	Perform a command on all the selected articles.\n\
",NOMARKING)) ||
#else
    (cmd = print_lines("\
 	Valid commands are: e, j, m, M, s, S, and |.\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
/pattern/modifiers\n\
	Scan forward for article containing pattern in the subject line.\n\
	(Use ?pat? to scan backwards; append h to scan headers, a to scan\n\
	entire articles, r to scan read articles, c to make case sensitive.)\n\
",NOMARKING)) ||
    (cmd = print_lines("\
/pattern/modifiers:command{:command}\n\
	Apply one or more commands to the set of articles matching pattern.\n\
	Use a K modifier to save entire command to the KILL file for this\n\
	newsgroup.  Commands m and M, if first, imply an r modifier.\n\
 	Valid commands are the same as for the range command.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
f,F	Submit a followup article (F = include this article).\n\
r,R	Reply through net mail (R = include this article).\n\
e dir{|command}\n\
	Extract to directory using /bin/sh, uudecode, unship, or command.\n\
s ...	Save to file or pipe via sh.\n\
S ...	Save via preferred shell.\n\
w,W	Like s and S but save without the header.\n\
| ...	Same as s|...\n\
",NOMARKING)) ||
    (cmd = print_lines("\
C	Cancel this article, if yours.\n\
^R,v	Restart article (v=verbose).\n\
^X	Restart article, rot13 mode.\n\
c	Catch up (mark all articles as read).\n\
b	Back up one page.\n\
^L	Refresh the screen.  You can get back to the pager with this.\n\
X	Refresh screen in rot13 mode.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
^	Go to first unread article.  Disables subject search mode.\n\
$	Go to end of newsgroup.  Disables subject search mode.\n\
",NOMARKING)) ||
    (cmd = print_lines("#       Print last article number.\n\
&	Print current values of command-line switches.\n\
&switch {switch}\n\
	Set or unset more switches.\n\
&&	Print current macro definitions.\n\
&&def	Define a new macro.\n\
j	Junk this article (mark it read).  Stays at end of article.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
m	Mark article as still unread.\n\
M	Mark article as still unread upon exiting newsgroup or Y command.\n\
Y	Yank back articles marked temporarily read via M.\n\
k	Kill current subject (mark articles as read).\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
,	Mark current article and its replies as read.\n\
J	Junk entire thread (mark all subjects as read in this thread).\n\
T	Trash current thread (like 'J'), and save command in KILL file.\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
K	Mark current subject as read, and save command in KILL file.\n\
^K	Edit local KILL file (the one for this newsgroup).\n\
=	List subjects of unread articles.\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
+	Enter thread selection mode.\n\
U	Unread some news -- prompts for thread, subthread, all, or select.\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
u	Unsubscribe from this newsgroup.\n\
q	Quit this newsgroup for now.\n\
Q	Quit newsgroup, staying at current newsgroup.\n\
",NOMARKING)) )
	return cmd;
#endif
    return 0;
}

int
help_ng()
{
    int cmd;
#ifdef NGHELP
    doshell(sh,filexp(NGHELP));
#else
    page_init();
    if (cmd = print_lines("\
Newsgroup Selection commands:\n\
",STANDOUT) )
	return cmd;
    if (ng != nextrcline) {
	if ((cmd = print_lines("\
\n\
y,SP	Do this newsgroup now.\n\
.cmd	Do this newsgroup, executing cmd as first command.\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
+	Enter this newsgroup through the thread selector (like typing .+<CR>).\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
=	Start this newsgroup, but list subjects before reading articles.\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
U	Enter this newsgroup by way of the \"Set unread?\" prompt.\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
u	Unsubscribe from this newsgroup.\n\
",NOMARKING)) )
	    return cmd;
    }
    if ((cmd = print_lines("\
c	Catch up (mark this newsgroup all read).\n\
A	Abandon read/unread changes to this newsgroup since you started trn.\n\
\n\
n	Go to the next newsgroup with unread news.\n\
N	Go to the next newsgroup.\n\
p	Go to the previous newsgroup with unread news.\n\
P	Go to the previous newsgroup.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
-	Go to the previously displayed newsgroup.\n\
1	Go to the first newsgroup.\n\
^	Go to the first newsgroup with unread news.\n\
$	Go to the last newsgroup.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
g name	Go to the named newsgroup.  Subscribe to new newsgroups this way too.\n\
/pat	Search forward for newsgroup matching pattern.\n\
?pat	Search backward for newsgroup matching pattern.\n\
	(Use * and ? style patterns.  Append r to include read newsgroups.)\n\
",NOMARKING)) ||
    (cmd = print_lines("\
l pat	List unsubscribed newsgroups containing pattern.\n\
m name	Move named newsgroup elsewhere (no name moves current newsgroup).\n\
o pat	Only display newsgroups matching pattern.  Omit pat to unrestrict.\n\
a pat	Like o, but also scans for unsubscribed newsgroups matching pattern.\n\
L	List current .newsrc.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
&	Print current command-line switch settings.\n\
&switch {switch}\n\
	Set (or unset) more command-line switches.\n\
&&	Print current macro definitions.\n\
&&def	Define a new macro.\n\
!cmd	Shell escape.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
q	Quit trn.\n\
x	Quit, restoring .newsrc to its state at startup of trn.\n\
^K	Edit the global KILL file.  Use commands like /pattern/j to suppress\n\
	pattern in every newsgroup.\n\
v	Print version.\n\
",NOMARKING)) )
	return cmd;
#endif
#ifdef PUSHBACK
    if (cmd = get_anything())
	return cmd;
    show_macros();
#endif
    return 0;
}

#ifdef ESCSUBS
int
help_subs()
{
    int cmd;
#ifdef SUBSHELP
    doshell(sh,filexp(SUBSHELP));
#else
    page_init();
    if ((cmd = print_lines("\
Valid substitutions are:\n\
",STANDOUT)) ||
    (cmd = print_lines("\
\n\
a	Current article number\n\
A	Full name of current article (%P/%c/%a)\n\
b	Destination of last save command, often a mailbox\n\
B	Bytes to ignore at beginning of last saved article\n\
",NOMARKING)) ||
    (cmd = print_lines("\
c	Current newsgroup, directory form\n\
C	Current newsgroup, dot form\n\
d	Full name of newsgroup directory (%P/%c)\n\
D	Distribution line from current article\n\
e	The last command executed to extract data from an article\n\
E	The last extraction directory\n\
",NOMARKING)) ||
    (cmd = print_lines("\
f	Who the current article is from\n\
F	Newsgroups to followup to (from Newsgroups and Followup-To)\n\
h	(This help message)\n\
H	Host name (yours)\n\
i	Message-I.D. line from current article, with <>\n\
I	Reference indicator mark (see -F switch)\n\
",NOMARKING)) ||
    (cmd = print_lines("\
l	News administrator's login name, if any\n\
L	Login name (yours)\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
m	Current mode, first letter of (init,newsgroup,thread,article,pager,\n\
		unread,Add,Catchup,Delete-bogus,Mailbox,Resubscribe)\n\
",NOMARKING)) ||
#else
    (cmd = print_lines("\
m	Current mode, first letter of (init, newsgroup, article, pager,\n\
		Add, Catchup, Delete bogus, Mailbox, Resubscribe)\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
M	Number of article marked with M\n\
n	Newsgroups from current article\n\
N	Full name (yours)\n\
o	Organization (yours)\n\
O	Original working directory (where you ran trn from)\n\
",NOMARKING)) ||
    (cmd = print_lines("\
p	Your private news directory (from -d)\n\
P	Public news spool directory\n\
r	Last reference (parent article id)\n\
R	References list for followup article\n\
",NOMARKING)) ||
    (cmd = print_lines("\
s	Subject, with all Re's and (nf)'s stripped off\n\
S	Subject, with one Re stripped off\
t	New To line derived from From and Reply-To (Internet format)\n\
T	New To line derived from Path\n\
u	Number of unread articles\n\
",NOMARKING)) ||
#ifdef USETHREADS
    (cmd = print_lines("\
U	Number of unread articles not counting the current article (when\n\
	threads are selected, the count only reflects selected articles)\n\
v	The number of extra (unselected) articles, not counting the current\n\
	one if it is unselected\n\
w	Mthreads' tmp directory\n\
W	Where thread files are saved\n\
x	News library directory\n\
X	Trn library directory\n\
z	Length of current article in bytes\n\
Z	Number of selected threads\n\
",NOMARKING)) ||
#else
    (cmd = print_lines("\
U	Number of unread articles not counting the current article\n\
x	News library directory\n\
X	Trn library directory\n\
z	Length of current article in bytes\n\
",NOMARKING)) ||
#endif
    (cmd = print_lines("\
~	Your home directory\n\
.	Directory containing . files\n\
#	A counter in multi-article saves\n\
$	Current process number\n\
/	Last search string\n\
ESC	Run preceding command through % interpretation\n\
",NOMARKING)) )
	return cmd;
#endif
    return 0;
}
#endif

#ifdef USETHREADS
int
help_select()
{
    int cmd;

    page_init();
    if ((cmd = print_lines("\
Thread selection commands:\n\
",STANDOUT)) ||
    (cmd = print_lines("\n\
a-z,0-9	Select/deselect the discussion thread by its letter or number.  By\n\
	default the letters h, k, m, n, p, q and y are omitted.\n\
SP	Perform the default command (usually > or Z).\n\
CR	Start reading.  Selects the current thread if none are selected.\n\
Z,TAB	Start reading.  If no articles are selected, read everything.\n\
y, '.'	Toggle the current thread's selection.\n\
k, ','	Mark the current thread as killed.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
m, \\	Unmark the current thread.\n\
-	Set a range, as in 2 - 5.  Repeats the last marking action.\n\
@	Toggle all visible thread selections.\n\
n, ]	Move down to the next thread.\n\
p, [	Move up to the previous thread.\n\
<, >	Go to previous/next page.\n\
^, $	Go to first/last page.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
X	Mark all unselected articles as read and start reading.\n\
D	Mark unselected articles on the current page as read.  Start\n\
	reading if articles were selected, else go to next page.\n\
J	Junk all selected articles (mark them as read).\n\
^K	Edit local KILL file (the one for this newsgroup).\n\
:cmd	Perform a command on all the selected articles.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
/pattern/modifiers\n\
	Scan all articles for a subject containing pattern.\n\
	(Append h to scan headers, a to scan entire articles, c to make case\n\
	sensitive, r to scan read articles (assumed when you are selecting\n\
	read articles to set unread.)\n\
/pattern/modifiers:command{:command}\n\
	Apply one or more commands to the set of articles matching pattern.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
	Use a K modifier to save entire command to the KILL file for this\n\
	newsgroup.  Commands m and M, if first, imply an r modifier.\n\
 	Valid commands are: e, E, j, m, M, s, S, t, T, !, =, ',' and the\n\
	thread selection commands: + and -.\n\
N	Leave this group as-is and go on to the next.\n\
U	Switch between selecting read/unread articles.\n\
L	Switch the current display mode between a terse mode without\n\
	authors and a short and long mode with authors.\n\
",NOMARKING)) ||
    (cmd = print_lines("\
&	View or set command line switches.\n\
&&	View or set macro definitions.\n\
!cmd	Escape to a subshell.\n\
q	Quit selection mode.\n\
Q	Quit group and return to news group selection prompt for this group.\n\
",NOMARKING)) )
	return cmd;
    return 0;
}
#endif

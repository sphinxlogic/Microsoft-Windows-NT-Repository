
/* $Id: filter.h,v 5.3 1992/11/15 01:42:57 syd Exp $ */

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.3 $   $State: Exp $
 *
 * 			Copyright (c) 1988-1992 USENET Community Trust
 * 			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein, Elm Coordinator
 *	elm@DSI.COM			dsinc!elm
 *
 *******************************************************************************
 * $Log: filter.h,v $
 * Revision 5.3  1992/11/15  01:42:57  syd
 * Add regexp processing to filter.
 * Add execc operator
 * From: Jan Djarv <Jan.Djarv@sa.erisoft.se>
 *
 * Revision 5.2  1992/10/24  14:20:24  syd
 * remove the 25 (MAXRULES) limitation.
 * Basically it mallocs rules in hunks of RULESINC (25) as it goes along.
 * From: Jan Djarv <Jan.Djarv@sa.erisoft.se>
 *
 * Revision 5.1  1992/10/03  22:34:39  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/

/** Headers for the filter program.

**/

#include "regexp.h"

#ifdef   BSD
# undef  tolower
#endif

/** define a few handy macros for later use... **/

#define  the_same(a,b)	(strncmp(a,b,strlen(b)) == 0)

#define relationname(x)  (x == 1?"<=":x==2?"<":x==3?">=":x==4?">":x==5?"!=":"=")

#define quoteit(x)	 (x == LINES? "" : "\"")

#define remove_return(s)	{ if (s[strlen(s)-1] == '\n') \
				    s[strlen(s)-1] = '\0';    \
			   	}

/** some of the files we'll be using, where they are, and so on... **/

#define  filter_temp	"/tmp/filter"
#define  FILTERFILE	".elm/filter-rules"
#define  filterlog	".elm/filterlog"
#define  filtersum	".elm/filtersum"

#define  EMERGENCY_MAILBOX	"EMERGENCY_MBOX"
#define  EMERG_MBOX		"MBOX.EMERGENCY"

/** an arbitrary number for allocating purposes **/

#define  RULESINC	25		/* alloc this many rules at a time */

/** some random defines for mnemonic stuff in the program... **/

#ifdef SUBJECT
# undef SUBJECT
#endif
#ifdef SENDER
# undef SENDER
#endif

#define  TO		1
#define  FROM		2
#define  LINES		3
#define  SUBJECT	4
#define  CONTAINS	5
#define  ALWAYS		6

#define  DELETE_MSG 	7
#define  SAVE		8
#define  SAVECC		9
#define  FORWARD	10
#define  LEAVE		11
#define  EXEC		12
#define  FORWARDC	13
#define  SENDER		14
#define  EXECC		15

#define  FAILED_SAVE	20

/** Some conditionals... **/

#define LE		1
#define LT		2
#define GE		3
#define GT		4
#define NE		5
#define EQ		6
#define RE		7

/** A funky way to open a file using open() to avoid file locking hassles **/

#define  FOLDERMODE	O_WRONLY | O_APPEND | O_CREAT | O_SYNCIO

/** cheap but easy way to have two files share the same #include file **/

#ifdef MAIN_ROUTINE

char home[SLEN],				/* the users home directory */
     hostname[SLEN],			/* the machine name...      */
     username[SLEN];			/* the users login name...  */

char to[VERY_LONG_STRING], 
     from[LONG_STRING], 
     subject[LONG_STRING],		/* from current message     */
     sender[LONG_STRING];		/* from current message     */

FILE *outfd;
char outfname[SLEN];
char filterfile[SLEN];
nl_catd elm_msg_cat = 0;			/* message catalog          */

int  total_rules = 0,				/* how many rules to check  */
     sizeof_rules = 0,				/* how big is rules array?  */
     show_only = FALSE,				/* just for show?           */
     long_summary = FALSE,			/* what sorta summary??     */
     verbose   = FALSE,				/* spit out lots of stuff   */
     lines     = 0,				/* lines in message..       */
     clear_logs = FALSE,			/* clear files after sum?   */
     already_been_forwarded = FALSE,		/* has this been filtered?  */
     log_actions_only = FALSE,			/* log actions | everything */
     logging   = TRUE,				/* should we log actions    */
     printing_rules = FALSE,			/* are we just using '-r'?  */
     rule_choosen,				/* which one we choose      */
     user_uid = -1,                             /* actual user id           */
     user_gid = -1;                             /* actual group id          */
  
#else

extern char home[SLEN],				/* the users home directory */
            hostname[SLEN],			/* the machine name...      */
            username[SLEN];			/* the users login name...  */

extern char to[VERY_LONG_STRING], 
            from[LONG_STRING], 
	    subject[LONG_STRING],		/* from current message     */
	    sender[LONG_STRING];		/* from current message     */

extern FILE *outfd;
extern char outfname[SLEN];
extern char filterfile[SLEN];
extern nl_catd elm_msg_cat;			/* message catalog          */

extern int total_rules,				/* how many rules to check  */
           sizeof_rules,			/* how big is rules array?  */
           show_only,				/* just for show?           */
           long_summary,			/* what sorta summary??     */
           verbose,				/* spit out lots of stuff   */
           lines,				/* lines in message..       */
           clear_logs,			        /* clear files after sum?   */
	   already_been_forwarded,		/* has this been filtered?  */
           log_actions_only,			/* log actions | everything */
	   logging,				/* should we log actions    */
           printing_rules,			/* are we just using '-r'?  */
           rule_choosen,			/* which one we choose      */
           user_uid,                            /* actual user id           */
           user_gid;                            /* actual group id          */
#endif

/** and our ruleset record structure... **/

struct condition_rec {
	int     matchwhat;			/* type of 'if' clause      */
	int     relation;			/* type of match (eq, etc)  */
	char    argument1[SLEN];		/* match against this       */
	regexp	*regex;				/* compiled regexp	    */
	struct  condition_rec  *next;		/* next condition...	    */
      };

struct ruleset_record {
	int	line;				/* line in rules file	    */
	struct  condition_rec  *condition;
	int     action;				/* what action to take      */
	char    argument2[SLEN];		/* argument for action      */
      };

#ifdef MAIN_ROUTINE
  struct ruleset_record *rules;
#else
  extern struct ruleset_record *rules;
#endif

/** finally let's keep LINT happy with the return values of all these pups! ***/

char *itoa();

#ifdef	_POSIX_SOURCE		/*{_POSIX_SOURCE*/
#include <string.h>
#include <unistd.h>
#else				/*}_POSIX_SOURCE{*/
unsigned short getuid();

unsigned long sleep();

char *strcpy(), *strcat();

#ifdef BSD
	
  FILE *popen();

#ifdef MAIN_ROUTINE
  char  _vbuf[5*BUFSIZ];              /* space for file buffering */
#else
  extern char  _vbuf[5*BUFSIZ];		/* space for file buffering */
#endif

#ifndef _IOFBF
# define _IOFBF		0		/* doesn't matter - ignored */
#endif

# define setvbuf(fd,a,b,c)	setbuffer(fd, _vbuf, 5*BUFSIZ)

#endif
#endif				/*}_POSIX_SOURCE*/

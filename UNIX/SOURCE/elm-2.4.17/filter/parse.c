
static char rcsid[] ="@(#)$Id: parse.c,v 5.6 1992/12/11 01:45:04 syd Exp $";

/*******************************************************************************
 *  The Elm Mail System  -  $Revision: 5.6 $   $State: Exp $
 *
 * 			Copyright (c) 1988-1992 USENET Community Trust
 * 			Copyright (c) 1986,1987 Dave Taylor
 *******************************************************************************
 * Bug reports, patches, comments, suggestions should be sent to:
 *
 *	Syd Weinstein - elm@DSI.COM
 *			dsinc!elm
 *
 *******************************************************************************
 * $Log: parse.c,v $
 * Revision 5.6  1992/12/11  01:45:04  syd
 * remove sys/types.h include, it is now included by defs.h
 * and this routine includes defs.h or indirectly includes defs.h
 * From: Syd
 *
 * Revision 5.5  1992/12/07  05:08:03  syd
 * add sys/types for time_t
 * From: Syd
 *
 * Revision 5.4  1992/11/15  01:40:43  syd
 * Add regexp processing to filter.
 * Add execc operator
 * From: Jan Djarv <Jan.Djarv@sa.erisoft.se>
 *
 * Revision 5.3  1992/10/28  14:52:25  syd
 * fix compiler warning
 * From: steve@nshore.org (Stephen J. Walick)
 *
 * Revision 5.2  1992/10/24  14:20:24  syd
 * remove the 25 (MAXRULES) limitation.
 * Basically it mallocs rules in hunks of RULESINC (25) as it goes along.
 * From: Jan Djarv <Jan.Djarv@sa.erisoft.se>
 *
 * Revision 5.1  1992/10/03  22:18:09  syd
 * Initial checkin as of 2.4 Release at PL0
 *
 *
 ******************************************************************************/


/** This is the parser for the filter program.  It accepts a wide variety of
    constructs, building the ruleset table as it goes along.  Check the 
    data structure in filter.h for more information on how the rules are
    stored.  The parser is a cunning state-table based program.

**/

#include <stdio.h>
#include <ctype.h>

#include "defs.h"
#include "filter.h"
#include "s_filter.h"

#define NONE			0
#define AND			10

#define NEXT_CONDITION		0
#define GETTING_OP		1
#define READING_ARGUMENT	2
#define READING_ACTION		3
#define ACTION_ARGUMENT		4

char *strtok(), *whatname(), *actionname();

static int
grow(ptr, oldsize, incr)
struct ruleset_record **ptr;
int oldsize, incr;
{
	struct ruleset_record *newptr;
	int newsize;

	if (ptr == 0) return 0;

	newsize = oldsize + incr;
	if (oldsize == 0)
	  newptr = (struct ruleset_record *)
	        malloc(sizeof(struct ruleset_record)*newsize);
	else
	  newptr = (struct ruleset_record *)
	        realloc((char *) *ptr, sizeof(struct ruleset_record)*newsize);

	if (newptr == NULL){
	  if (outfd != NULL)
	    fprintf(outfd,catgets(elm_msg_cat,
				  FilterSet,FilterCantAllocRules,
			"filter (%s): Can't alloc memory for %d rules!\n"),
			username, newsize);
	  if (outfd != NULL) fclose(outfd);
	  exit(1);
	}

	*ptr = newptr;
	return newsize;
}

int
get_filter_rules()
{
	/** Given the users home directory, open and parse their rules table,
	    building the data structure as we go along.
	    returns -1 if we hit an error of any sort...
	**/

	FILE
	     *fd;				/* the file descriptor     */
	char
	     buffer[SLEN], 			/* fd reading buffer       */
	     *str, 				/* ptr to read string      */
	     *word,				/* ptr to 'token'          */
	     filename[SLEN], 			/* the name of the ruleset */
	     action_argument[SLEN], 		/* action arg, per rule    */
	     cond_argument[SLEN];		/* cond arg, per condition */
	int
	     not_condition = FALSE, 		/* are we in a "not" ??    */
	     type=NONE, 			/* what TYPE of condition? */
	     lasttype=NONE, 			/* and the previous TYPE?  */
	     state = NEXT_CONDITION,		/* the current state       */
	     in_single, in_double, 		/* for handling spaces.    */
	     i, 				/* misc integer for loops  */
	     relop = NONE,			/* relational operator     */
	     action, 				/* the current action type */
	     buflen,				/* the length of buffer    */
	     line = 0;				/* line number we're on    */
	struct condition_rec
	     *cond, *newcond;

	strcpy(filename,filterfile);

	if ((fd = fopen(filename,"r")) == NULL) {
	  if (outfd != NULL)
	   fprintf(outfd,catgets(elm_msg_cat,FilterSet,FilterCouldntReadRules,
	   "filter (%s): Couldn't read filter rules file \"%s\"!\n"),username,
		  filename);
	  return(-1);
	}

	if (sizeof_rules == 0)
	  sizeof_rules = grow(&rules, sizeof_rules, RULESINC);

	cond_argument[0] = action_argument[0] = '\0';

	/* Now, for each line... **/

	if ((cond = (struct condition_rec *) 
		     malloc(sizeof(struct condition_rec))) == NULL) {
	  if (outfd != NULL)
	    fprintf(outfd,catgets(elm_msg_cat,
				  FilterSet,FilterCouldntMallocFirst,
		  "filter (%s): couldn't malloc first condition rec!\n"),
		    username);
	  return(-1);
	}
	
	rules[total_rules].condition = cond;	/* hooked in! */

	while (fgets(buffer, SLEN, fd) != NULL) {
	  line++;

	  if (buffer[0] == '#' || (buflen = strlen(buffer)) < 2)
	    continue;		/* nothing to look at! */

	  in_single = in_double = 0;

	  for (i=0; i < buflen; i++) {
	    if (buffer[i] == '"') 
	      in_double = ! in_double;
	    else if (buffer[i] == '\'')
	      in_single = ! in_single;
	    if ((in_double || in_single) && buffer[i] == ' ')
	      buffer[i] = '_';
	  }

	  if (lasttype != AND && lasttype != NONE) {
	    lasttype = type;
	    type = NONE;
	  }
	  str = (char *) buffer;

	  /** Three pieces to this loop - get the `field', the 'relop' (if
	      there) then, if needed, get the argument to check against (not 
	      needed for errors or the AND, of course)
	  **/

	  while ((word = strtok(str, " ()[]:\t\n")) != NULL) {

	    str = (char *) NULL;		/* we can start stomping! */
	  
	    lowercase(word);

	    if (strcmp(word, "if") == 0) {	/* only ONE 'if' allowed */
	      if ((word = strtok(str, " ()[]:\t\n")) == NULL)	/* NEXT! */
	        continue;
	      lowercase(word);
	    }
	
	    if (state == NEXT_CONDITION) {
	      lasttype = type;
	      type = NONE;

	      if (the_same(word, "not") || the_same(word, "!")) {
	        not_condition = TRUE;
	        if ((word = strtok(str, " ()[]'\"\t\n")) == NULL)
	          continue;
	      }

	      if (the_same(word, "from"))
		   type = FROM;
	      else if (the_same(word, "to"))
		   type = TO;
	      else if (the_same(word, "subject"))
		   type = SUBJECT;
	      else if (the_same(word, "sender"))
		   type = SENDER;
	      else if (the_same(word, "lines"))
		   type = LINES;
	      else if (the_same(word, "contains"))
		   type = CONTAINS;
	      else if (the_same(word, "and") || 
	               the_same(word, "&&"))
		   type = AND;
	      else if (the_same(word,"?") ||
		       the_same(word, "then") || 
		       the_same(word, "always")) {

		/** shove THIS puppy into the structure and let's continue! **/

	        if (lasttype == AND) {
		  if (outfd != NULL)
	            fprintf(outfd,catgets(elm_msg_cat,
					  FilterSet,FilterErrorReadingRules1,
       "filter (%s): Error reading line %d of rules - badly placed \"and\"\n"),
		    username, line);
		  return(-1);
	        }

	        if (the_same(word, "always"))
		  cond->matchwhat = ALWAYS;	/* so it's a hack... */
		else
		  cond->matchwhat = lasttype;

	        if (relop == NONE) relop = EQ;	/* otherwise can't do -relop */
	        cond->relation  = (not_condition? - (relop) : relop);
		cond->regex = NULL;

		if (relop != RE)
		  for (i=strlen(cond_argument); --i >= 0;)
	            if (cond_argument[i] == '_') cond_argument[i] = ' ';

		strcpy(cond->argument1, cond_argument);
	        if ((newcond = (struct condition_rec *)
		     malloc(sizeof(struct condition_rec))) == NULL) {
		  if (outfd != NULL)
	            fprintf(outfd,catgets(elm_msg_cat,
					  FilterSet,FilterCouldntMallocNew ,
		     	    "filter (%s): Couldn't malloc new cond rec!!\n"),
		            username);
		  return(-1);
	        }
	        cond->next = NULL;

	        relop = EQ;	/* default relational condition */

	        state = READING_ACTION;
	        if ((word = strtok(str, " ()[]'\"\t\n")) == NULL)
	          continue;
	        goto get_outta_loop;
	      }

	      if (type == NONE) {
		if (outfd != NULL)
	          fprintf(outfd,catgets(elm_msg_cat,
					FilterSet,FilterErrorReadingRules2,
      "filter (%s): Error reading line %d of rules - field \"%s\" unknown!\n"),
		     username, line, word);
		return(-1);
	      }

	      if (type == AND) {

		/** shove THIS puppy into the structure and let's continue! **/

		cond->matchwhat = lasttype;
	        cond->relation  = (not_condition? - (relop) : relop);
		cond->regex = NULL;
		strcpy(cond->argument1, cond_argument);
	        if ((newcond = (struct condition_rec *)
	             malloc(sizeof(struct condition_rec))) == NULL) {
		  if (outfd != NULL)
	            fprintf(outfd,catgets(elm_msg_cat,
					  FilterSet,FilterCouldntMallocNew ,
			"filter (%s): Couldn't malloc new cond rec!!\n"),
			username);
		  return(-1);
	        }
	        cond->next = newcond;
		cond = newcond;
		cond->next = NULL;

	        not_condition = FALSE;
	        state = NEXT_CONDITION;
	      }
	      else {
	        state = GETTING_OP;
	      }
	    }

get_outta_loop: 	/* jump out when we change state, if needed */

	    if (state == GETTING_OP) {

	       if ((word = strtok(str, " ()[]'\"\t\n")) == NULL)
	         continue;

	       lowercase(word);

	       relop = NONE;

	       if (the_same(word, "=") || the_same(word, "in") || 
                   the_same(word, "contains")) {
                 state = READING_ARGUMENT;
	         relop = EQ;
	       }
	       else {
	         if (the_same(word, "<=")) 	relop = LE;
	         else if (the_same(word, ">="))	relop = GE;
	         else if (the_same(word, ">"))	relop = GT;
	         else if (the_same(word, "<>")||
		          the_same(word, "!="))	relop = NE;
	         else if (the_same(word, "<"))	relop = LT;
	         else if (the_same(word, "~")||
			  the_same(word, "matches")) relop = RE;

		 /* maybe there isn't a relop at all!! */

		 state=READING_ARGUMENT;

	       }
	    }
		 
	    if (state == READING_ARGUMENT) {
	      if (relop == RE){
		/* Special for regular expressions (enclosed between //) */
		cond_argument[0] = '\0';
		for (;;) {
	          if ((word = strtok(str, "/")) == NULL)
	            break;
		  strcat(cond_argument, word);
		  if (word[strlen(word)-1] == '\\') /* If / was escaped ... */
		    strcat(cond_argument, "/");
		  else
		    break;
		}
		if (word == NULL)
		  continue;

	      } else {
		if (relop != NONE) {
	          if ((word = strtok(str, " ()[]'\"\t\n")) == NULL)
	            continue;
	        }
	        for (i=strlen(word); --i>=0;)
	          if (word[i] == '_') word[i] = ' ';

	        strcpy(cond_argument, word);
	      }
	      state = NEXT_CONDITION;
	    }

	    if (state == READING_ACTION) {
	      action = NONE;

	      not_condition = FALSE;

	      if (the_same(word, "delete"))       action = DELETE_MSG;
	      else if (the_same(word, "savec"))   action = SAVECC;
	      else if (the_same(word, "save"))    action = SAVE;
	      else if (the_same(word, "forwardc")) action = FORWARDC;
	      else if (the_same(word, "forward")) action = FORWARD;
	      else if (the_same(word, "executec")) action = EXECC;
	      else if (the_same(word, "exec"))    action = EXEC;
	      else if (the_same(word, "leave"))   action = LEAVE;
	      else {
		if (outfd != NULL)
	          fprintf(outfd,catgets(elm_msg_cat,
					FilterSet,FilterErrorReadingRules3 ,
	"filter (%s): Error on line %d of rules - action \"%s\" unknown\n"),
			username, line, word);
	      }

	      if (action == DELETE_MSG || action == LEAVE) {
	        /** add this to the rules section and alloc next... **/

	        rules[total_rules].action = action;
		rules[total_rules].argument2[0] = '\0';	/* nothing! */
	        if (++total_rules >= sizeof_rules)
		  sizeof_rules = grow(&rules, sizeof_rules, RULESINC);
	         
	        if ((cond = (struct condition_rec *)
		     malloc(sizeof(struct condition_rec))) == NULL) {
		  if (outfd != NULL)
	            fprintf(outfd,catgets(elm_msg_cat,
					  FilterSet,FilterCouldntMallocFirst,
			"filter (%s): couldn't malloc first condition rec!\n"),
			username);
	          return(-1);
	        }
	
	        rules[total_rules].condition = cond;	/* hooked in! */
	        state = NEXT_CONDITION;	
	      }
	      else {
	        state = ACTION_ARGUMENT;
	      }

	      if ((word = strtok(str, " ()[]'\"\t\n")) == NULL)
	        continue;

	    }
	
	    if (state == ACTION_ARGUMENT) {
	      strcpy(action_argument, word);

	      /** add this to the rules section and alloc next... **/

	      rules[total_rules].action = action;
	      rules[total_rules].line = line;

	      /** if we are not printing rules we can't expand macros
		  until after we applied the rule, due to the regexp macros
	      **/
	      if (printing_rules)
	        expand_macros(action_argument, rules[total_rules].argument2,
			      line, printing_rules);
	      else
		strcpy(rules[total_rules].argument2, action_argument);

	      if (++total_rules >= sizeof_rules)
	        sizeof_rules = grow(&rules, sizeof_rules, RULESINC);

	      if ((cond = (struct condition_rec *)
		     malloc(sizeof(struct condition_rec))) == NULL) {
		if (outfd != NULL)
	          fprintf(outfd,catgets(elm_msg_cat,
					FilterSet,FilterCouldntMallocFirst,
      		    "filter (%s): couldn't malloc first condition rec!\n"),
			  username);
	        return(-1);
	      }
	
	      rules[total_rules].condition = cond;	/* hooked in! */

	      state = NEXT_CONDITION;
	      if ((word = strtok(str, " ()[]'\"\t\n")) == NULL)
	        continue;
	    }
	  }
	}

	return(0);
}

/****************************************************************************/
/*									    */
/*  FACILITY:	Generic Support Library					    */
/*									    */
/*  MODULE:	Command Line Keyword and Option Processing		    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains routines to process command line	    */
/*  arguments, given a dispatch table of keywords and handlers. This can be */
/*  used for main program command lines, or internal command lines parsed   */
/*  in a similar manner.						    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/*  V0.2-00 23-SEP-1994 Steve Branam					    */
/*									    */
/*	Addded keyword translation code.				    */
/*									    */
/****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "cmdopt.h"

static KEYWORD_DEFINITION			/* Option dispatch table,   */
	    *mOptionTable;			/* used by options file	    */
						/* processing.		    */

/*************************************************************************++*/
int ustrncmp(
/* Compares strings in upper-case, serving as a case-insensitive string	    */
/* comparison; otherwise identical to strncmp.				    */

    char    *aStr1,
	    /* (READ, BY ADDR):						    */
	    /* First string to compare.					    */

    char    *aStr2,
	    /* (READ, BY ADDR):						    */
	    /* Second string to compare.				    */

    int	    vLength
	    /* (READ, BY VAL):						    */
	    /* Maximum number of characters to compare.			    */

)	/* Returns status value indicating comparison results:		    */
	/*    0	- Strings are equal.					    */
	/*  < 0 - String aStr1 is less than aStr2.			    */
	/*  > 0 - String aStr1 is greater than aStr2.			    */
	/*****************************************************************--*/

{
    /*+									    */
    /*	Compare upper-case version of each char in strings until unequal    */
    /*	chars ard found, end of string is found, or maximum number of	    */
    /*	characters exceeded.						    */
    /*- 								    */

    for (; toupper(*aStr1) == toupper(*aStr2); aStr1++, aStr2++) {
	if (*aStr1 == '\0' || --vLength == 0) {
	    return 0;			    /* Strings are "equal".	    */
	}
    }					    /* String are not equal.	    */
    return toupper(*aStr1) - toupper(*aStr2);
}

/*************************************************************************++*/
static int cmdopt_cmp(
/* Compares an argument string to a command line option using a case-	    */
/* insensitive string comparison.  The argument matches the option if the   */
/* keyword portion is at least as long as the minimum option keyword	    */
/* length, and matches the keyword for all specified characters.	    */

    KEYWORD_DEFINITION
	    *aOption,
	    /* (READ, BY ADDR):						    */
	    /* Command line option keyword definition to compare.	    */

    char    *aArgStr
	    /* (READ, BY ADDR):						    */
	    /* Argument string to compare. Options are assumed to be	    */
	    /* specified in one of two forms, either just the option switch */
	    /* char followed by the keyword (for toggle options), or the    */
	    /* option switch char and keyword followed by an equal sign and */
	    /* the option value string (for value options).		    */

)	/* Returns length of keyword portion of aArgstr (including switch   */
	/* char) to indicate successful match, or 0 if argument does not    */
	/* match option.						    */
	/********************************************************************/

{
    int	    kwlen;			    /* Length of argument keyword   */
					    /* portion (including switch    */
					    /* char).			    */

    /*+									    */
    /*	Find length of argument keyword portion. If it is less than minimum */
    /*	required length, no match. Otherwise, compare argument to option    */
    /*	keyword to determine match.					    */
    /*-									    */

    if ((kwlen = kwstrlen(&aArgStr[1])) < kwdef_minlen(aOption)) {  
	return 0;			    /* Too short, no match.	    */
    }
    else if (ustrncmp(kwdef_keyword(aOption), &aArgStr[1],
		max(kwlen, kwdef_minlen(aOption))) == 0) {
	return kwlen + 1;			    /* Argument matches option.	    */
    }
    else {
	return 0;			    /* No match.		    */
    }
}

/*************************************************************************++*/
static int kwdef_cmp(
/* Compares a string to a keyword using a case- insensitive string	    */
/* comparison.  The string matches the keyword if it is at least as long as */
/* the minimum keyword length, and matches the keyword for all specified    */
/* characters.								    */

    KEYWORD_DEFINITION
	    *aKwDef,
	    /* (READ, BY ADDR):						    */
	    /* Keyword definition to compare.				    */

    char    *aKwStr,
	    /* (READ, BY ADDR):						    */
	    /* Keyword string to compare.				    */

    int	    vLength
	    /* (READ, BY VAL):						    */
	    /* Keyword string length.					    */

)	/* Returns status flag:						    */
	/*  1 - Successful match					    */
	/*  0 - String does not match keyword.				    */
	/* ******************************************************************	*/

{
    /*+									    */
    /*	If string length is less than minimum required length, no match.    */
    /*	Otherwise, compare argument to option keyword to determine match.   */
    /*									    */

    if (vLength < kwdef_minlen(aKwDef)) {  
	return 0;			    /* Too short, no match.	    */
    }
    else if (ustrncmp(kwdef_keyword(aKwDef), aKwStr,
		max(vLength, kwdef_minlen(aKwDef))) == 0) {
	return 1;			    /* String matches keyword.	    */
    }
    else {
	return 0;			    /* No match.		    */
    }
}

/*************************************************************************++*/
int kwstrlen(
/* Finds the length of a keyword string. A keyword is a contiguous string   */
/* of alphanumerics and the underscore character '_'.			    */

    char    *aKwStr
	    /* (READ, BY ADDR):						    */
	    /* Keyword string to find length of. It is assumed to contain   */
	    /* no leading non-keyword characters (if it does, the length    */
	    /* returned will be 0).					    */

)	/* Returns length of string, or 0 if the first character is not a   */
	/* valid keyword string character.				    */
	/********************************************************************/

{
    int	    kwlen;				/* Length of keyword.	    */

    for (kwlen = 0;
	isalnum(aKwStr[kwlen]) || aKwStr[kwlen] == '_';
	kwlen++);
    return kwlen;
}

/*************************************************************************++*/
int kwstrleadlen(
/* Finds the length of leading non-keyword characters in a keyword string,  */
/* which may subsequently be treated as the offset to the beginning of the  */
/* keyword.								    */

    char    *aKwStr
	    /* (READ, BY ADDR):						    */
	    /* Keyword string that may contain leading non-keyword	    */
	    /* characters.						    */

)	/* Returns length of leading characters, or 0 if the string	    */
	/* contains no leading characters (or is a null string).	    */
	/********************************************************************/

{
    int	    leadlen;				/* Length of leading chars. */

    for (leadlen = 0;
	!isalnum(aKwStr[leadlen])
	&& aKwStr[leadlen] != '_' && aKwStr[leadlen] != '\0';
	leadlen++);
    return leadlen;
}

/*************************************************************************++*/
int process_options(
/* Parses user command line arguments for options and dispatches option	    */
/* handler routines. Processing will terminate if any handler indicates	    */
/* failure.								    */

    int	    vArgc,
	    /* (READ, BY VAL):						    */
	    /* Number of program argument strings in aArgv.		    */

    char    *aArgv[],
	    /* (READ, BY ADDR):						    */
	    /* List of program argument strings.			    */

    int	    vFirstOption,
	    /* (READ, BY VAL):						    */
	    /* Starting argument number for option. All preceding arguments */
	    /* are assumed to be required, not options.			    */

    KEYWORD_DEFINITION
	    *aOptionTable
	    /* (READ, BY ADDR):						    */
	    
	    /* Option definition table, containing option keywords (not	    */
	    /* including option switch character), handler routine ptrs,    */
	    /* and keyword translation codes.  The table is terminated by   */
	    /* an entry containing a NULL keyword ptr. The interface for a  */
	    /* handler routine is:					    */
	    /*								    */
	    /*	    int handler(code, valstr, n)			    */
	    /*								    */
	    /* where code is the translation code for the matched keyword,  */
	    /* valstr is the ptr to the remainder of the argument string    */
	    /* following the matched argument characters, and n is the	    */
	    /* option's argument position on the command line; however, a   */
	    /* handler is free to ignore (and therefore not declare) any    */
	    /* trailing subset of these parameters. The handler returns 1   */
	    /* if the argument is handled successfully, or 0 otherwise.	    */
	    /* This interface supports two styles of usage: common handlers */
	    /* may handle several different options, discriminating the	    */
	    /* matched option via the translation code; or individual	    */
	    /* handlers may be used for each option, ignoring the	    */
	    /* translation codes.					    */

)	/* Returns status indicating processing success:		    */
	/*  1	- All options handled successfully (any unrecognized	    */
	/*	  arguments/options ignored).				    */
	/*  0	- An argument could not be processed.			    */
	/********************************************************************/

{
    int	    arg;				/* Argument number.	    */
    KEYWORD_DEFINITION				/* Current keyword def.	    */
	    *kwdef;
    int	    kwlen;				/* Length of argument	    */
						/* keyword portion	    */
						/* (including switch char). */

    /*+									    */
    /*	Save option table ptr in case we run into an option to process an   */
    /*	options file, then attempt to process each command line argument    */
    /*	following the required ones as an option.			    */
    /*-									    */

    mOptionTable = aOptionTable;
    for (arg = vFirstOption; arg < vArgc; arg++) {

	/*+								    */
	/*  If argument starts with option switch character, go through	    */
	/*  option table trying to match it. On match, call option handler  */
	/*  and skip the rest of the option table; if the handler return    */
	/*  failure, abort. If no match, issue warning and ignore the	    */
	/*  argument. If it did not start with switch char, issue warning   */
	/*  and ignore it.						    */
	/*-								    */
						
	if (*aArgv[arg] == CMDLINE_OPTION_SWITCH) {
	    for (kwdef = aOptionTable; kwdef_keyword(kwdef) != NULL; kwdef++) {
		if ((kwlen = cmdopt_cmp(kwdef, aArgv[arg]))) {

						/* Match, call handler.	    */
		    if (!(kwdef_handler(kwdef))(kwdef_code(kwdef),
			    &aArgv[arg][kwlen], arg)) {
			return 0;		/* Abort, handler failed!   */
		    }
		    break;			/* Skip rest of table.	    */
		}
	    }
	    if (!kwlen) {			/* No option matches.	    */
		printf("ERROR: Unrecognized/ambiguous option %s\n", aArgv[arg]);
		return 0;
	    }
	}
	else {					/* Not an option argument.  */
	    printf("ERROR: Unexpected command line argument %s\n", aArgv[arg]);
	    return 0;
	}
    }						/* All options processed    */
    return 1;					/* successfully.	    */
}

/*************************************************************************++*/
int process_keyword(
/* Parses keyword strings and dispatches to keyword handler routines.	    */
/* Processing will terminate if any handler indicates failure. 		    */

    char    *aKwStr,
	    /* (READ, BY ADDR):						    */
	    /* Keyword string, with no leading, trailing, or enclosed	    */
	    /* whitespace. May be a single keyword, or a list of keywords,  */
	    /* separated by commas.					    */

    KEYWORD_DEFINITION
	    *aKwTable
	    /* (READ, BY ADDR):						    */
	    /* Keyword definition table, containing keywords, handler	    */
	    /* routine ptrs, and keyword translation codes. The table is    */
	    /* terminated by an entry containing a NULL keyword ptr. The    */
	    /* interface for a handler routine is:			    */
	    /*								    */
	    /*	    int handler(code)					    */
	    /*								    */
	    /* where code is the keyword translation code.  (there are no   */
	    /* arguments to the handler).  The handler returns 1 if the	    */
	    /* argument is handled successfully, or 0 otherwise. This	    */
	    /* interface supports two styles of usage: common handlers may  */
	    /* handle several different keywords, discriminating the	    */
	    /* matched keyword via the translation code; or individual	    */
	    /* handlers may be used for each keyword, ignoring the	    */
	    /* translation codes.					    */

)	/* Returns status indicating processing success:		    */
	/*  1	- All keywords handled successfully.			    */
	/*  0	- A keyword could not be processed.			    */
	/********************************************************************/

{
    KEYWORD_DEFINITION				/* Current keyword def.	    */
	    *kwdef;
    int	    kwlen;				/* Length of keyword.	    */
    int	    found;				/* Flag indicating match.   */
    char    *savestr = aKwStr;			/* Saved string ptr.	    */

						/* For each keyword in list */
    while (*aKwStr != '\0') {			/* (or just one)...	    */

	if ((kwlen = kwstrlen(aKwStr)) == 0) {
	    printf("ERROR: Missing keyword %s\n", savestr);
	    return 0;
	}
	else {					/* Scan keyword table for   */
						/* match.		    */
	    for (kwdef = aKwTable; kwdef_keyword(kwdef) != NULL; kwdef++) {
		if ((found = kwdef_cmp(kwdef, aKwStr, kwlen))) {

						/* Match, call handler with */
						/* translation code.	    */
		    if (!(kwdef_handler(kwdef))(kwdef_code(kwdef))) {
			return 0;		/* Abort, handler failed!   */
		    }
		    break;			/* Skip rest of table.	    */
		}
	    }
	    if (!found) {
		printf("ERROR: Unrecognized/ambiguous keyword %s\n", aKwStr);
		return 0;
	    }
	}
	if (aKwStr[kwlen] == KEYWORD_LIST_SEPARATOR) {
	    kwlen++;				/* Account for separator.   */
	}
	aKwStr += kwlen;			/* Advance past keyword.    */
    }						
    return 1;					/* All keywords processed   */
}						/* successfully.	    */

/*************************************************************************++*/
int translate_keyword(
/* Parses a single keyword string and returns a translation code.	    */

    char    *aKwStr,
	    /* (READ, BY ADDR):						    */
	    /* Keyword string. If it contains any leading non-keyword	    */
	    /* characters, they will be ignored.  If it contains more than  */
	    /* one keyword, only the first one will be translated.	    */

    KEYWORD_DEFINITION
	    *aKwTable
	    /* (READ, BY ADDR):						    */
	    /* Keyword definition table, containing keywords, handler	    */
	    /* routine ptrs, and keyword translation codes. The table is    */
	    /* terminated by an entry containing a NULL keyword ptr. The    */
	    /* handler routine is called only if its ptr is non-NULL. The   */
	    /* interface for a handler routine is:			    */
	    /*								    */
	    /*	    int handler(code)					    */
	    /*								    */
	    /* where code is the keyword translation code.  (there are no   */
	    /* arguments to the handler).  The handler returns 1 if the	    */
	    /* argument is handled successfully, or 0 otherwise. This	    */
	    /* interface supports two styles of usage: common handlers may  */
	    /* handle several different keywords, discriminating the	    */
	    /* matched keyword via the translation code; or individual	    */
	    /* handlers may be used for each keyword, ignoring the	    */
	    /* translation codes.					    */

)	/* Returns matching keyword translation code, or 0 if no match	    */
	/* found or the matching keyword handler returns 0.		    */
	/********************************************************************/

{
    KEYWORD_DEFINITION				/* Current keyword def.	    */
	    *kwdef;
    int	    kwlen;				/* Length of keyword.	    */
    char    *savestr = aKwStr;			/* Saved string ptr.	    */

    if ((kwlen = kwstrlen(aKwStr)) == 0) {
	return 0;
    }
    else {					/* Scan keyword table for   */
						/* match.		    */
	for (kwdef = aKwTable; kwdef_keyword(kwdef) != NULL; kwdef++) {
	    if (kwdef_cmp(kwdef, aKwStr, kwlen)) {

						/* Match, if handler	    */
						/* defined, call it with    */
						/* translation code.	    */
		if (kwdef_handler(kwdef) != NULL
		    && !(kwdef_handler(kwdef))(kwdef_code(kwdef))) {
		    return 0;			/* Abort, handler failed!   */
		}
		else {				/* Handler ok (or not	    */
		    return kwdef_code(kwdef);	/* defined), return code.   */
		}
	    }
	}
	return 0;				/* No match.		    */
    }
}

/*************************************************************************++*/
int process_options_file(
/* Command line option handler to read an options file, where each line is  */
/* a single command line option, just as if it were parsed from the command */
/* line. Option files may be nested, just be careful of looped files or	    */
/* running out of resources due to excessive nesting. Comments lines and    */
/* trailing comment are allowed.					    */

    int	    vKwCode,
	    /* (READ, BY ADDR):						    */
	    /* Option keyword translation code, ignored by this routine.    */

    char    *aValStr
	    /* (READ, BY ADDR):						    */
	    /* Option value string, preceded by equal sign.		    */
    
)	/* Returns status code:						    */
	/*  1	- Successful processing of this option.			    */
	/*  0	- Option file name missing, or file cannot be opened.	    */
	/*****************************************************************--*/

{
    FILE    *optfile;			    /* Options file ptr.	    */
    int	    rcount;			    /* Routine name count.	    */
    char    option[512];		    /* Option string buffer.	    */
    char    *optend;			    /* End-of-option ptr.	    */
    char    *optv;			    /* Option buffer ptr.	    */

					    /* Check for equal sign and	    */
					    /* make sure there is a file.   */
    if (*aValStr++ == CMDLINE_OPTION_SEPARATOR && *aValStr != '\0') {
	if ((optfile = fopen(aValStr, "r")) != NULL) {
	
					    /* For each line, locate	    */
					    /* beginning of text.	    */
	    while (fgets(option, sizeof(option), optfile) != NULL) {
		for (optv = option;
		    *optv != '\0' && *optv != CMDLINE_OPTION_SWITCH &&
		    *optv != CMDLINE_OPTION_COMMENT;
		    optv++);
					    /* Ignore comment lines. Find   */
					    /* end of option field and	    */
					    /* process option.		    */
		if (*optv != CMDLINE_OPTION_COMMENT) {
		    for (optend = optv;
			*optend > ' ' && *optend != CMDLINE_OPTION_COMMENT;
			optend++);
		    *optend = '\0';
		    if (!process_options(1, &optv, 0, mOptionTable)) {
			printf(
			    "ERROR: Failure processing option \"%s\" in %s\n",
			    option, aValStr);
			fclose(optfile);
			return 0;
		    }
		}
	    }
	    fclose(optfile);
	    return 1;
	}
	else {
	    printf("ERROR: Unable to open options file %s for input\n",
		aValStr);
	    return 0;
	}
    }
    else {
	printf("ERROR: %coptions option requires options file name\n",
	    CMDLINE_OPTION_SWITCH);
	return 0;
    }
}


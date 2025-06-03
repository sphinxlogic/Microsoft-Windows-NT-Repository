/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	DCL Language Parser					    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains the source parser for DCL language    */
/*  source files. Note that this particular implementation is just a dummy  */
/*  parser. The structure exists to create a more realistic state-driven    */
/*  parser, but for now it only differentiates between comments, strings,   */
/*  and statements.							    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 27-SEP-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "ranalyzer.h"
#include "parser.h"

#define MAX_QUOTED_LEN		1024		/* Just a guess...	    */

typedef enum
{
    FIND_START,
    FIND_END_ALNUM,
    FIND_END_NUMBER,
    FIND_END_SPACE,
    FIND_END_DQUOTED,
    FIND_END_TRAILING_COMMENT
} dcl_scanner_states;

typedef enum					/* Dummy states.	    */
{
    DUMMY
} dcl_parser_states;

static char
	*mPSNames[] = {
	    "DUMMY"
	};

typedef enum
{
    END_DCL_SOURCE,				/* EOF			    */
    IDENTIFIER,
    KEYWORD,
    SPACE,
    OTHER
} dcl_token_types;

static	char					/* Dummy keyword table.	    */
	*keywords[] = { NULL };

static  int				    /* Statement char count.	    */
	statement;
static  int				    /* Comment char count.	    */
	comment;

/*************************************************************************++*/
static void new_source_line(
/* Updates source line counters when a new line is found.		    */

    SOURCEFILE
	    *aSourceRecord
		/* (MODIFY, BY ADDR):					    */
		/* Source file information record. The line count	    */
		/* statistics will be updated.				    */

)	/* No return value						    */
	/*****************************************************************--*/

{
    /*
    ** Classify the source line just completed as either mixed
    ** statements/comments, statements only, comments only, or blank, and
    ** increment the appropriate source record counters.
    */
    
    if (statement && comment) {
	inc_source_mixed(aSourceRecord);
    }
    else if (statement){
	inc_source_statements(aSourceRecord);
    }
    else if (comment) {
	inc_source_comments(aSourceRecord);
    }
    else {
	inc_source_empty(aSourceRecord);
    }
    
    statement = 0;			    /* Reset counters for next	    */
    comment   = 0;			    /* line.			    */

    new_list_line(aSourceRecord);
}

/*************************************************************************++*/
static int iskeyword(
/* Determines whether or not an alphanumeric token is a source language	    */
/* keyword.								    */

    char    *aKeywords[],
	    /* (READ, BY ADDR):						    */
	    /* List of known source language keyword string pointers, in    */
	    /* alphabetical order, terminated by NULL entry.		    */

    char    *aToken
	    /* (READ, BY ADDR):						    */
	    /* Token string to check.					    */

)	/* Returns status of comparison:				    */
	/*	1   - Token is a keyword.				    */
	/*	0   - Token is not a keyword.				    */
	/*****************************************************************--*/

{
    return 0;	/* No keywords to recognize. */
}

/*************************************************************************++*/
static get_token(
/* Source file input scanner. Reads the next lexical token from the source  */
/* file and accumulates source line statistics.				    */

    FILE    *aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file containing C language.			    */

    SOURCEFILE
	    *aSourceRecord,
		/* (MODIFY, BY ADDR):					    */
		/* Source file information record. The line count	    */
		/* statistics will be updated.				    */

    char    *aToken
		/* (WRITE, BY ADDR):					    */
		/* String buffer to receive token.			    */

)	/* Returns code indicating which type of token was found:	    */
	/*     END_DCL_SOURCE - End of the source file.			    */
	/*     IDENTIFIER   - An identifier.				    */
	/*     KEYWORD	    - A language keyword.			    */
	/*     SPACE	    - Whitespace.				    */
	/*     OTHER	    - Some other type of token.			    */
	/*****************************************************************--*/

{
    int	    ch;				    /* Input character.		    */
    dcl_scanner_states			    /* Scanner state.		    */
	    state = FIND_START;
    char    *nextchar = aToken;		    /* Pointer to next char	    */
					    /* position in aToken.	    */
    int	    quoted_len;			    /* Length of quoted string, for */
					    /* catching unterminated	    */
					    /* strings.			    */
    long    quoted_line;		    /* Line where literal started.  */

    do {
	ch = fgetc(aSourceFile);
	switch (state) {
	case FIND_START:
	    list_char(ch);
	    if (isalpha(ch) || ch == '_' || ch == '$') {
		state = FIND_END_ALNUM;
		*nextchar++ = ch;
		statement++;
	    }
	    else if (isdigit(ch)) {
		state = FIND_END_NUMBER;
		*nextchar++ = ch;
		statement++;
	    }
	    else if (isspace(ch)) {
		if (ch == '\n') {
		    new_source_line(aSourceRecord);
		}
		state = FIND_END_SPACE;
	    }
	    else {
		switch (ch) {
		case '"':
		    statement++;
		    state = FIND_END_DQUOTED;
		    quoted_len = 0;
		    quoted_line = source_line(aSourceRecord);
		    break;
		case '!':
		    comment++;
		    state = FIND_END_TRAILING_COMMENT;
		    break;
		default:
		    if (ch != EOF) {
			*nextchar++ = ch;
			*nextchar   = '\0';
			statement++;
			return OTHER;
		    }
		}
	    }
	    break;
	case FIND_END_ALNUM:
	    if (isalnum(ch) || ch == '_' || ch == '$') {
		list_char(ch);
		*nextchar++ = ch;
		statement++;
	    }
	    else {
		ungetc(ch, aSourceFile);
		*nextchar = '\0';
		if (iskeyword(keywords, aToken)) {
		    return KEYWORD;
		}
		else {
		    return IDENTIFIER;
		}
	    }
	    break;
	case FIND_END_NUMBER:
	    if (isdigit(ch)) {
		list_char(ch);
		*nextchar++ = ch;
		statement++;
	    }
	    else {
		ungetc(ch, aSourceFile);
		*nextchar = '\0';
		return OTHER;
	    }
	    break;
	case FIND_END_SPACE:
	    if (isspace(ch)) {
		list_char(ch);
		if (ch == '\n') {
		    new_source_line(aSourceRecord);
		}
	    }
	    else {
		ungetc(ch, aSourceFile);
		*nextchar = '\0';
		return SPACE;
	    }
	    break;
	case FIND_END_DQUOTED:
	    list_char(ch);
	    if (quoted_len++ == MAX_QUOTED_LEN) {
		printf(
		"WARNING: Suspected unterminated string literal at line %d\n",
		    quoted_line);
	    }
	    if (ch == '"') {
		ch = fgetc(aSourceFile);
		if (ch == '"') {
		    *nextchar++ = ch;
		    statement++;
		    list_char(ch);
		}
		else {
		    ungetc(ch, aSourceFile);
		}
		*nextchar++ = ch;
		*nextchar   = '\0';
		statement++;
		return OTHER;
	    }
	    else if (ch == '\n') {
		new_source_line(aSourceRecord);
	    }
	    else {
		statement++;
	    }
	    break;
	case FIND_END_TRAILING_COMMENT:
	    list_char(ch);
	    if (ch == '\n') {
		new_source_line(aSourceRecord);
		state = FIND_START;
	    }
	    else {
		comment++;
	    }
	    break;
	}
    } while (ch != EOF);
    return END_DCL_SOURCE;
}

/*************************************************************************++*/
language_element dcl_parser(
/* Parses DCL source language statements. This is just a dummy parser, it   */
/* does not recognize any actual syntax.				    */

    FILE    *aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file containing DCL language. Must be opened by   */
		/* caller.						    */

    SOURCEFILE
	    *aSourceRecord,
		/* (READ, BY ADDR):					    */
		/* Source file information record.			    */

    char    *aElement,
		/* (WRITE, BY ADDR):					    */
		/* String buffer that will receive the recognized source    */
		/* language element.					    */

    long    *aSourceLine
		/* (WRITE, BY ADDR):					    */
		/* Buffer that will receive the line number of aElement.    */

)	/* Returns one of the following values indicating the type of	    */
	/* element output in aElement:					    */
	/*      PARSE_ERROR	    - An error was detected in the input    */
	/*			      stream.				    */
	/*	END_OF_SOURCE	    - The normal end of file was found.	    */
	/*	ROUTINE_DEF_BEGIN   - The beginning of a routine definition */
	/*			      was found.			    */
	/*	ROUTINE_DEF_END	    - The end of the current routine	    */
	/*			      definition was found.		    */
	/*	ROUTINE_REF	    - A routine reference (call) was found. */
	/*****************************************************************--*/

{
    static dcl_parser_states		    /* Parser state.		    */
	    state = DUMMY;
    static int				    /* Nested block level.	    */
	    blevel;
    static char				    /* Name of current routine.	    */
	    curdefname[MAX_ROUTINE_NAME + 1];
    int	    plevel;			    /* Nested parenthesis level.    */
    dcl_token_types			    /* Type of source token.	    */
	    tokentype;
    char    token[MAX_ROUTINE_NAME + 1];    /* Source token buffer.	    */

    do {
	tokentype = get_token(aSourceFile, aSourceRecord, token);
	switch (state) {
	case DUMMY:
	    break;
	}
    } while (tokentype != END_DCL_SOURCE);
    change_pstate(DUMMY);
    return END_OF_SOURCE;
}



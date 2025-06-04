/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	BLISS Language Parser					    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains the source parser for BLISS language  */
/*  source files. Note that this particular implementation is a very	    */
/*  rudimentary state-driven parser. While it is reasonably functional, it  */
/*  is possible that it may become confused by unusual but otherwise valid  */
/*  language constructs.						    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 19-SEP-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "ranalyzer.h"
#include "parser.h"

#define BLISS_KEYWORD_BEGIN	"BEGIN"
#define BLISS_KEYWORD_END	"END"
#define BLISS_KEYWORD_ROUTINE	"ROUTINE"
#define BLISS_KEYWORD_FORWARD	"FORWARD"
#define BLISS_KEYWORD_EXTERNAL	"EXTERNAL"
#define MAX_QUOTED_LEN		1024		/* Just a guess...	    */

typedef enum
{
    NO_MACRO,
    IN_MACRO
} c_macro_states;

typedef enum
{
    FIND_START,
    FIND_END_ALNUM,
    FIND_END_NUMBER,
    FIND_END_SPACE,
    FIND_END_SQUOTED,
    FIND_END_TRAILING_COMMENT,
    FIND_END_EMBEDDED_COMMENT
} bliss_scanner_states;

typedef enum
{
    FIND_ROUTINE,
    FIND_IDENT,
    FIND_DEF_EQUALS,
    FIND_DEF_RPAREN,
    FIND_DEF_ATTR,
    FIND_LBRACE,
    IN_ROUTINE,
    FIND_REF_LPAREN
} bliss_parser_states;

static char
	*mPSNames[] = {
	    "FIND_ROUTINE",
	    "FIND_IDENT",
	    "FIND_DEF_EQUALS",
	    "FIND_DEF_RPAREN",
	    "FIND_DEF_ATTR",
	    "FIND_LBRACE",
	    "IN_ROUTINE",
	    "FIND_REF_LPAREN"
	};

typedef enum
{
    END_BLISS_SOURCE,				/* EOF			    */
    LPAREN,					/* '('			    */
    RPAREN,					/* ')'			    */
    BEGIN_BLOCK,				/* "BEGIN"		    */
    END_BLOCK,					/* "END"		    */
    ROUTINE_DECL,				/* "ROUTINE"		    */
    FORWARD_DECL,				/* "FORWARD"		    */
    EXTERNAL_DECL,				/* "EXTERNAL"		    */
    EQUALS,					/* '='			    */
    COLON,					/* ':'			    */
    SEMICOLON,					/* ';'			    */
    IDENTIFIER,
    KEYWORD,
    MACBEGIN,
    SPACE,
    OTHER
} bliss_token_types;

static	char
	*keywords[] = { "addressing_mode",
			"align",
			"always",
			"and",
			"begin",
			"bind",
			"bit",
			"builtin",
			"by",
			"byte",
			"case",
			"codecomment",
			"compiletime",
			"decr",
			"decru",
			"do",
			"else",
			"eludom",
			"enable",
			"eql",
			"eqla",
			"eqlu",
			"eqv",
			"exitloop",
			"external",
			"field",
			"forward",
			"from",
			"geq",
			"geqa",
			"gequ",
			"global",
			"gtr",
			"gtra",
			"gtru",
			"if",
			"incr",
			"incra",
			"incru",
			"initial",
			"inrange",
			"iopage",
			"keywordmacro",
			"label",
			"leave",
			"leq",
			"leqa",
			"lequ",
			"library",
			"linkage",
			"literal",
			"local",
			"long",
			"lss",
			"lssa",
			"lssu",
			"macro",
			"map",
			"mod",
			"module",
			"neq",
			"neqa",
			"nequ",
			"not",
			"novalue",
			"of",
			"or",
			"otherwise",
			"outrange",
			"own",
			"plit",
			"preset",
			"psect",
			"record",
			"ref",
			"register",
			"rep",
			"require",
			"return",
			"routine",
			"select",
			"selecta",
			"selectone",
			"selectonea",
			"selectoneu",
			"selectu",
			"set",
			"show",
			"signed",
			"stacklocal",
			"structure",
			"switches",
			"tes",
			"then",
			"to",
			"undeclare",
			"unsigned",
			"until",
			"uplit",
			"volatile",
			"weak",
			"while",
			"with",
			"word",
			"xor",
			NULL };

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
    int	    cmpstat;			    /* Comparison status.	    */
    int	    length;			    /* Token length.		    */

    length = strlen(aToken);
    while (*aKeywords != NULL
	&& (cmpstat = ustrncmp(*aKeywords, aToken,
	max(length, strlen(*aKeywords)))) < 0) {
	aKeywords++;
    }
    return !cmpstat;
}

/*************************************************************************++*/
char *ustrcpy(
/* Copies a string in uppercase.					    */

    char    *aDest,
	    /* (WRITE, BY ADDR):					    */
	    /* Destination string buffer into which all-uppercase string    */
	    /* will be written. It is assumed to be long enough to hold the */
	    /* entire contents of aSrc with null termination.		    */

    char    *aSrc
	    /* (READ, BY ADDR):  					    */
	    /* Source string, of any case.                     		    */

)	/* Returns aDest, the destination buffer.			    */
	/*****************************************************************--*/

{
    char    *deststr = aDest;			/* Save dest string ptr.    */

    while (*aSrc != '\0') {
	*aDest++ = toupper(*aSrc++);
    }
    *aDest = '\0';
    return deststr;
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
	/*     END_BLISS_SOURCE - End of the source file.		    */
	/*     LPAREN	    - Left parenthesis.				    */
	/*     RPAREN	    - Right parenthesis.			    */
	/*     BEGIN_BLOCK  - "BEGIN" keyword.				    */
	/*     END_BLOCK    - "END" keyword.				    */
	/*     EQUALS	    - Equals sign.				    */
	/*     COLON	    - Colon.					    */
	/*     SEMICOLON    - Semi-colon.				    */
	/*     IDENTIFIER   - Routine or data identifier		    */
	/*     KEYWORD	    - C language keyword.			    */
	/*     MACBEGIN	    - Beginning of macro.			    */
	/*     SPACE	    - Whitespace.				    */
	/*     OTHER	    - Some other type of token.			    */
	/*****************************************************************--*/

{
    int	    ch;				    /* Input character.		    */
    bliss_scanner_states		    /* Scanner state.		    */
	    state = FIND_START;
    char    *nextchar = aToken;		    /* Pointer to next char	    */
					    /* position in aToken.	    */
    static c_macro_states		    /* Macro state.		    */
	    macro = NO_MACRO;
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
		case '(':
		    statement++;
		    return LPAREN;
		    break;
		case ')':
		    statement++;
		    return RPAREN;
		    break;
		case '=':
		    statement++;
		    return EQUALS;
		    break;
		case ':':
		    statement++;
		    return COLON;
		    break;
		case ';':
		    statement++;
		    return SEMICOLON;
		    break;
		case '\'':
		    statement++;
		    state = FIND_END_SQUOTED;
		    quoted_len = 0;
		    quoted_line = source_line(aSourceRecord);
		    break;
		case '!':
		    comment++;
		    state = FIND_END_TRAILING_COMMENT;
		    break;
		case '%':
		    ch = fgetc(aSourceFile);
		    if (ch == '(') {
			list_char(ch);
			state = FIND_END_EMBEDDED_COMMENT;
			comment += 2;
		    }
		    else {
			ungetc(ch, aSourceFile);
			state = FIND_END_ALNUM;
			*nextchar++ = ch;
			statement++;
		    }
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
		if (ustrncmp(aToken, BLISS_KEYWORD_BEGIN,
		    max(strlen(BLISS_KEYWORD_BEGIN), strlen(aToken))) == 0) {
		    return BEGIN_BLOCK;
		}
		else if (ustrncmp(aToken, BLISS_KEYWORD_END,
		    max(strlen(BLISS_KEYWORD_END), strlen(aToken))) == 0) {
		    return END_BLOCK;
		}
		else if (ustrncmp(aToken, BLISS_KEYWORD_ROUTINE,
		    max(strlen(BLISS_KEYWORD_ROUTINE), strlen(aToken))) == 0) {
		    return ROUTINE_DECL;
		}
		else if (ustrncmp(aToken, BLISS_KEYWORD_FORWARD,
		    max(strlen(BLISS_KEYWORD_ROUTINE), strlen(aToken))) == 0) {
		    return FORWARD_DECL;
		}
		else if (ustrncmp(aToken, BLISS_KEYWORD_EXTERNAL,
		    max(strlen(BLISS_KEYWORD_ROUTINE), strlen(aToken))) == 0) {
		    return EXTERNAL_DECL;
		}
		else if (iskeyword(keywords, aToken)) {
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
	case FIND_END_SQUOTED:
	    list_char(ch);
	    if (quoted_len++ == MAX_QUOTED_LEN) {
		printf(
		"WARNING: Suspected unterminated string literal at line %d\n",
		    quoted_line);
	    }
	    if (ch == '\'') {
		ch = fgetc(aSourceFile);
		if (ch == '\'') {
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
	case FIND_END_EMBEDDED_COMMENT:
	    list_char(ch);
	    if (ch == ')') {
		ch = fgetc(aSourceFile);
		if (ch == '%') {
		    list_char(ch);
		    state = FIND_START;
		    comment += 2;
		}
		else {
		    ungetc(ch, aSourceFile);
		    comment++;
		}
	    }
	    else if (ch == '\n') {
		new_source_line(aSourceRecord);
	    }
	    else {
		comment++;
	    }
	    break;
	}
    } while (ch != EOF);
    return END_BLISS_SOURCE;
}

/*************************************************************************++*/
language_element bliss_parser(
/* Parses BLISS source language statements, looking for routine definition  */
/* begin and end, and routine references. Retrieves the next language	    */
/* element in the source file.						    */
/*									    */
/* Note that this version is a very simple-minded parser, and has several   */
/* limitations.  It is not able to identify function pointer usages as	    */
/* routine references. It may also be confused by other legal constructs.   */

    FILE    *aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file containing BLISS language. Must be opened by */
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
    static bliss_parser_states		    /* Parser state.		    */
	    state = FIND_ROUTINE;
    static int				    /* Nested block level.	    */
	    blevel;
    static char				    /* Name of current routine.	    */
	    curdefname[MAX_ROUTINE_NAME + 1];
    int	    plevel;			    /* Nested parenthesis level.    */
    bliss_token_types			    /* Type of source token.	    */
	    tokentype;
    char    token[MAX_ROUTINE_NAME + 1];    /* Source token buffer.	    */
    int	    forward_flag = 0;		    /* Indicates FORWARD or	    */
					    /* EXTERNAL keyword seen.	    */

    /*+									    */
    /*	This function operates as a state machine. The states represent the */
    /*	various tokens expected next in the token stream, according to	    */
    /*	BLISS syntax. Whenever a routine definition beginning or end, or    */
    /*	routine reference, is recognized, the parser returns to the caller. */
    /*	However, context is maintained between calls to the parser via	    */
    /*	static state variables.						    */
    /*-									    */

    do {
	tokentype = get_token(aSourceFile, aSourceRecord, token);
	switch (state) {
	case FIND_ROUTINE:
	    if (forward_flag && tokentype != SPACE) {
		forward_flag = 0;
	    }
	    else if (tokentype == ROUTINE_DECL) {
		*aSourceLine = source_line(aSourceRecord);
		change_pstate(FIND_IDENT);
	    }
	    else if (tokentype == FORWARD_DECL || tokentype == EXTERNAL_DECL) {
		forward_flag = 1;
		trace_msg(
	    "\nTRACE: Parser will ignore token after FORWARD or EXTERNAL\n");
	    }
	    break;
	case FIND_IDENT:
	    if (tokentype == IDENTIFIER) {
		ustrcpy(aElement, token);
		change_pstate(FIND_DEF_EQUALS);
	    }
	    else if (tokentype == END_BLISS_SOURCE) {
		printf("ERROR: Unexpected end of file %s\n",
		    source_name(aSourceRecord));
		return PARSE_ERROR;
	    }
	    else if (tokentype != SPACE) {
		change_pstate(FIND_ROUTINE);
	    }
	    break;
	case FIND_DEF_EQUALS:
	    if (tokentype == EQUALS) {
		change_pstate(IN_ROUTINE);
		block_level_zero();
		ustrcpy(curdefname, token);
		return ROUTINE_DEF_BEGIN;
	    }
	    else if (tokentype == LPAREN) {
		change_pstate(FIND_DEF_RPAREN);
		paren_level_zero();
	    }
	    else if (tokentype == COLON) {
		change_pstate(FIND_DEF_ATTR);
puts("*** WARNING: FIND_DEF_ATTR state not fully implemented ***");
	    }
	    else if (tokentype != SPACE) {
		change_pstate(FIND_ROUTINE);
	    }
	    break;
	case FIND_DEF_RPAREN:
	    if (tokentype == RPAREN) {
		if (plevel) {
		    paren_level_dec();
		}
		else {
		    change_pstate(FIND_DEF_EQUALS);
		}
	    }
	    else if (tokentype == LPAREN) {
		paren_level_inc();
	    }
	    break;
	case FIND_DEF_ATTR:
puts(token);	
	    if (tokentype == EQUALS) {
		change_pstate(IN_ROUTINE);
		block_level_zero();
		ustrcpy(curdefname, token);
		return ROUTINE_DEF_BEGIN;
	    }
	    break;
	case IN_ROUTINE:
	    if (tokentype == BEGIN_BLOCK) {
		block_level_inc();
	    }
	    else if (tokentype == END_BLOCK) {
		block_level_dec();
	    }
	    else if (tokentype == SEMICOLON) {
		if (blevel == 0) {
		    trace_blmsg(BLEND);
		    change_pstate(FIND_ROUTINE);
		    *aSourceLine = source_line(aSourceRecord);
		    ustrcpy(aElement, curdefname);
		    return ROUTINE_DEF_END;
		}
	    }
	    else if (tokentype == IDENTIFIER) {
		ustrcpy(aElement, token);
		*aSourceLine = source_line(aSourceRecord);
		change_pstate(FIND_REF_LPAREN);
	    }
	    else if (tokentype == END_BLISS_SOURCE) {
		printf("ERROR: Unexpected end of file %s\n",
		    source_name(aSourceRecord));
		return PARSE_ERROR;
	    }
	    break;
	case FIND_REF_LPAREN:
	    if (tokentype != SPACE) {
		if (tokentype == END_BLOCK) {
		    block_level_dec();
		}
		change_pstate(IN_ROUTINE);
	    }
	    if (tokentype == LPAREN) {
		return ROUTINE_REF;
	    }
	    break;
	}
    } while (tokentype != END_BLISS_SOURCE);
    change_pstate(FIND_ROUTINE);
    return END_OF_SOURCE;
}


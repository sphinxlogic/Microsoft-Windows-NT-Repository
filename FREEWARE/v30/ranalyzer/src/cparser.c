/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	C Language Parser					    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains the source parser for C language	    */
/*  source files. Note that this particular implementation is a very	    */
/*  rudimentary state-driven parser. While it is reasonably functional, it  */
/*  is possible that it may become confused by unusual but otherwise valid  */
/*  language constructs.						    */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-01 10-FEB-1995 Steve Branam					    */
/*									    */
/*	Allow spaces between # and keyword in preprocessor directives.      */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "ranalyzer.h"
#include "parser.h"

#define MAX_DQUOTE_LEN	    512			/* Just a guess.	    */
#define MAX_SQUOTE_LEN	    4			/* '\123'		    */


#define cond_token(t)	(copy ? t : SPACE)

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
    FIND_END_DIRECTIVE,
    FIND_END_DQUOTED,
    FIND_END_SQUOTED,
    FIND_END_COMMENT
} c_scanner_states;

typedef enum
{
    FIND_IDENT,
    FIND_DEF_LPAREN,
    FIND_DEF_RPAREN,
    FIND_LBRACE_OR_SEMICOLON,
    FIND_LBRACE,
    FIND_MACRO_IDENT,
    FIND_MACRO_LPAREN,
    IN_ROUTINE,
    FIND_REF_LPAREN
} c_parser_states;

static char
	*mPSNames[] = {
	    "FIND_IDENT",
	    "FIND_DEF_LPAREN",
	    "FIND_DEF_RPAREN",
	    "FIND_LBRACE_OR_SEMICOLON",
	    "FIND_LBRACE",
	    "FIND_MACRO_IDENT",
	    "FIND_MACRO_LPAREN",
	    "IN_ROUTINE",
	    "FIND_REF_LPAREN"
	};
	
typedef enum
{
    END_C_SOURCE,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
    IDENTIFIER,
    KEYWORD,
    MACBEGIN,
    SPACE,
    OTHER
} c_token_types;

static	char
	*keywords[] = { "auto",
			"break",
			"case",
			"char",
			"continue",
			"default",
			"do",
			"double",
			"else",
			"entry",
			"enum",
			"extern",
			"float",
			"for",
			"goto",
			"if",
			"int",
			"long",
			"register",
			"return",
			"sizeof",
			"short",
			"static",
			"struct",
			"switch",
			"typedef",
			"union",
			"unsigned",
			"void",
			"while",
			NULL };

static  int				    /* Statement char count.	    */
	statement;
static  int				    /* Comment char count.	    */
	comment;

/*************************************************************************++*/
void trace_parser(
/* Write a parser trace message to the listing file. Listing must be	    */
/* enabled. Parser tracing is assumed to be enabled.			    */

    char    *aTraceStr
	    /* (READ, BY ADDR):  					    */
	    /* String to write to listing file.				    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fputs(aTraceStr, list_file());
	restore_list_column();
    }
}

/*************************************************************************++*/
void trace_parser_int(
/* Write a parser trace message containing one integer value to the listing */
/* file. Listing must be enabled. Parser tracing is assumed to be enabled.  */

    char    *aTraceStr,
	    /* (READ, BY ADDR):  					    */
	    /* String (including one integer printf format control) to	    */
	    /* write to listing file.					    */
	    
    int	    vInt
	    /* (READ, BY VAL):						    */
	    /* Integer value to write in string.			    */

)	/* No return value.      					    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), aTraceStr, vInt);
	restore_list_column();
    }
}

/*************************************************************************++*/
void trace_parser_state(
/* Write a parser state change trace message to the listing file. Listing   */
/* must be enabled. Parser tracing is assumed to be enabled.		    */

    char    *aFromName,
	    /* (READ, BY ADDR):  					    */
	    /* Name string for state changing from.			    */
	    
    char    *aToName
	    /* (READ, BY ADDR):  					    */
	    /* Name string for state changing to.			    */
	    
)	/* No return value.      					    */
	/*****************************************************************--*/

{
    if (list_enabled()) {
	fprintf(list_file(), "\nTRACE: Parser state change: %s to %s\n",
	    aFromName, aToName);
	restore_list_column();
    }
}

/*************************************************************************++*/
void new_source_line(
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

    while (*aKeywords != NULL && (cmpstat = strcmp(*aKeywords++, aToken)) < 0);
    return !cmpstat;
}

/*************************************************************************++*/
static int evaluate_if(
/* Evaluates a #if conditional compilation directive.			    */
/*									    */
/* WARNING: This is just a dummy version for now.			    */

    char    *aCondition
	    /* (READ, BY ADDR):  					    */
	    /* Condition string to be evaluated.			    */

)	/* Returns boolean flag to indicate results of evaluation.	    */
	/*****************************************************************--*/

{
    return atoi(aCondition);	/* Handle simple numeric constant */
}

/*************************************************************************++*/
static int evaluate_ifdef(
/* Evaluates a #ifdef conditional compilation directive.		    */
/*									    */
/* WARNING: This is just a dummy version for now.			    */

    char    *aCondition
	    /* (READ, BY ADDR):  					    */
	    /* Condition string to be evaluated.			    */

)	/* Returns boolean flag to indicate results of evaluation.	    */
	/*****************************************************************--*/

{
    return 0; /* Nothing is defined right now. */
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
	/*     END_C_SOURCE - End of the source file.			    */
	/*     LPAREN	    - Left parenthesis.				    */
	/*     RPAREN	    - Right parenthesis.			    */
	/*     LBRACE	    - Left brace.				    */
	/*     RBRACE	    - Right brace.				    */
	/*     SEMICOLON    - Semicolon.				    */
	/*     IDENTIFIER   - Routine or data identifier		    */
	/*     KEYWORD	    - C language keyword.			    */
	/*     MACBEGIN	    - Beginning of macro.			    */
	/*     SPACE	    - Whitespace.				    */
	/*     OTHER	    - Some other type of token.			    */
	/*****************************************************************--*/

{
    int	    ch;				    /* Input character.		    */
    int	    quotelen;			    /* Length of quoted token.	    */
    c_scanner_states			    /* Scanner state.		    */
	    state = FIND_START;
    char    *nextchar = aToken;		    /* Pointer to next char	    */
					    /* position in aToken.	    */
    char    condbuf[256];		    /* Conditional directive buf.   */
    static int				    /* Conditional compilation copy */
	    copy = 1;			    /* flag.			    */
    static c_macro_states		    /* Macro state.		    */
	    macro = NO_MACRO;

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
		    if (macro == IN_MACRO) {/* Special case: if end of line */
			macro = NO_MACRO;   /* in a macro, this is really   */
			ungetc(ch, aSourceFile); 
			return cond_token(RBRACE); /* end of a "routine"    */
		    }			    /* definition.		    */
		    else {
			new_source_line(aSourceRecord);
		    }
		}
		state = FIND_END_SPACE;
	    }
	    else {
		switch (ch) {
		case '(':
		    statement++;
		    return cond_token(LPAREN);
		    break;
		case ')':
		    statement++;
		    return cond_token(RPAREN);
		    break;
		case '{':
		    statement++;
		    return cond_token(LBRACE);
		    break;
		case '}':
		    statement++;
		    return cond_token(RBRACE);
		    break;
		case ';':
		    statement++;
		    return cond_token(SEMICOLON);
		    break;
		case '#':
		    statement++;
		    *nextchar++ = ch;
		    state = FIND_END_DIRECTIVE;
		    break;
		case '"':
		    statement++;
		    state = FIND_END_DQUOTED;
		    quotelen = 0;
		    break;
		case '\'':
		    statement++;
		    state = FIND_END_SQUOTED;
		    quotelen = 0;
		    break;
		case '/':
		    ch = fgetc(aSourceFile);
		    if (ch == '*') {
			list_char(ch);
			state = FIND_END_COMMENT;
			comment += 2;
		    }
		    else {
			ungetc(ch, aSourceFile);
			statement++;
		    }
		    break;
		default:
		    if (ch != EOF) {
			*nextchar++ = ch;
			*nextchar   = '\0';
			statement++;
			return cond_token(OTHER);
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
		    return cond_token(KEYWORD);
		}
		else {
		    return cond_token(IDENTIFIER);
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
		return cond_token(OTHER);
	    }
	    break;
	case FIND_END_SPACE:
	    if (isspace(ch)) {
		list_char(ch);
		if (ch == '\n') {
		    if (macro == IN_MACRO) {/* Special case: if end of line */
			macro = NO_MACRO;   /* in a macro, this is really   */
			ungetc(ch, aSourceFile);
			return cond_token(RBRACE);	    /* the end of a "routine"	    */
		    }			    /* definition.		    */
		    else {
			new_source_line(aSourceRecord);
		    }
		}
	    }
	    else {
		ungetc(ch, aSourceFile);
		*nextchar = '\0';
		return cond_token(SPACE);
	    }
	    break;
	case FIND_END_DIRECTIVE:
	    if (isalpha(ch)) {
		list_char(ch);
		*nextchar++ = ch;
		statement++;
	    }
	    else if (*(nextchar - 1) == '#' && isspace(ch)) {
	        list_char(ch);         /* Ignore space between # and keyword */
		statement++;
	    }
	    else {
		ungetc(ch, aSourceFile);
		*nextchar = '\0';
		if (!strcmp(aToken, "#define")) {
		    macro = IN_MACRO;
		    return cond_token(MACBEGIN);
		}
		else if (!strcmp(aToken, "#if")) {
		    fgets(condbuf, sizeof(condbuf), aSourceFile);
		    if (list_enabled()) {
			fputs(condbuf, list_file());
		    }
		    new_source_line(aSourceRecord);
		    copy = evaluate_if(condbuf);
		    return SPACE;
		}
		else if (!strcmp(aToken, "#ifdef")) {
		    fgets(condbuf, sizeof(condbuf), aSourceFile);
		    if (list_enabled()) {
			fputs(condbuf, list_file());
		    }
		    new_source_line(aSourceRecord);
		    copy = evaluate_ifdef(condbuf);
		    return SPACE;
		}
		else if (!strcmp(aToken, "#ifndef")) {
		    fgets(condbuf, sizeof(condbuf), aSourceFile);
		    if (list_enabled()) {
			fputs(condbuf, list_file());
		    }
		    new_source_line(aSourceRecord);
		    copy = !evaluate_ifdef(condbuf);
		    return SPACE;
		}
		else if (!strcmp(aToken, "#else")) {
		    copy = !copy;
		    return SPACE;
		}
		else if (!strcmp(aToken, "#endif")) {
		    copy = 1;
		    return SPACE;
		}
		else
		{
		    return cond_token(OTHER);
		}
	    }
	    break;
	case FIND_END_DQUOTED:
	    list_char(ch);
	    if (ch == '"') {
		statement++;
		return cond_token(OTHER);
	    }
	    else if (ch == '\\') {	    /* Check for escape seq. */
		statement++;
		quotelen++;
	        ch = fgetc(aSourceFile);
		list_char(ch);
		statement++;
		quotelen++;
	    }
	    else if (ch == '\n') {
		new_source_line(aSourceRecord);
	    }
	    else if (quotelen > MAX_DQUOTE_LEN) {
		printf(
	    "WARNING: Suspected unterminated double quote at line %d of %s\n",
		    source_line(aSourceRecord), source_name(aSourceRecord));
		return cond_token(OTHER);
	    }
	    else {
		statement++;
		quotelen++;
	    }
	    break;
	case FIND_END_SQUOTED:
	    list_char(ch);
	    if (ch == '\'') {
		ch = fgetc(aSourceFile);
		if (ch == '\'') {
		    statement++;
		    list_char(ch);
		}
		else {
		    ungetc(ch, aSourceFile);
		}
		statement++;
		return cond_token(OTHER);
	    }
	    else if (ch == '\n' || quotelen > MAX_SQUOTE_LEN) {
		new_source_line(aSourceRecord);
		printf(
	    "WARNING: Suspected unterminated single quote at line %d of %s\n",
		    source_line(aSourceRecord), source_name(aSourceRecord));
		return cond_token(OTHER);
	    }
	    else {
		statement++;
		quotelen++;
	    }
	    break;
	case FIND_END_COMMENT:
	    list_char(ch);
	    if (ch == '*') {
		ch = fgetc(aSourceFile);
		if (ch == '/') {
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
    copy = 1;
    return END_C_SOURCE;
}
    
/*************************************************************************++*/
language_element c_parser(
/* Parses C source language statements, looking for routine definition	    */
/* begin and end, and routine references. Retrieves the next language	    */
/* element in the source file.						    */
/*									    */
/* Note that this version is a very simple-minded parser, and has several   */
/* limitations.  It is not able to identify function pointer usages as	    */
/* routine references. It may also be confused by other legal constructs.   */

    FILE    *aSourceFile,
		/* (READ, BY ADDR):					    */
		/* Source file containing C language. Must be opened by	    */
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
    static c_parser_states		    /* Parser state.		    */
	    state = FIND_IDENT;
    static int				    /* Nested braces level.	    */
	    blevel;
    static char				    /* Name of current routine.	    */
	    curdefname[MAX_ROUTINE_NAME + 1];
    int	    plevel;			    /* Nested parenthesis level.    */
    c_token_types			    /* Type of source token.	    */
	    tokentype;
    char    token[MAX_ROUTINE_NAME + 1];    /* Source token buffer.	    */

    /*
    ** This function operates as a state machine. The states represent the
    ** various tokens expected next in the token stream, according to C syntax.
    ** Whenever a routine definition beginning or end, or routine reference, is
    ** recognized, the parser returns to the caller. However, context is
    ** maintained between calls to the parser via static state variables.
    */

    do {
	tokentype = get_token(aSourceFile, aSourceRecord, token);
	switch (state) {
	case FIND_IDENT:
	    if (tokentype == IDENTIFIER) {
		strcpy(aElement, token);
		*aSourceLine = source_line(aSourceRecord);
		change_pstate(FIND_DEF_LPAREN);
	    }
	    else if (tokentype == MACBEGIN) {
		change_pstate(FIND_MACRO_IDENT);
	    }
	    break;
	case FIND_DEF_LPAREN:
	    if (tokentype == LPAREN) {
		change_pstate(FIND_DEF_RPAREN);
		paren_level_zero();
	    }
	    else if (tokentype == IDENTIFIER) {
		strcpy(aElement, token);
		*aSourceLine = source_line(aSourceRecord);
	    }
	    else if (tokentype != SPACE) {
		change_pstate(FIND_IDENT);
	    }		
	    break;
	case FIND_DEF_RPAREN:
	    if (tokentype == RPAREN) {
		if (plevel) {
		    paren_level_dec();
		}
		else {
		    change_pstate(FIND_LBRACE_OR_SEMICOLON);
		}
	    }
	    else if (tokentype == LPAREN) {
		paren_level_inc();
	    }
	    break;
	case FIND_LBRACE_OR_SEMICOLON:
	    if (tokentype == LBRACE) {
		change_pstate(IN_ROUTINE);
		block_level_zero();
		strcpy(curdefname, token);
		return ROUTINE_DEF_BEGIN;
	    }					/* Forward or external	    */
	    else if (tokentype == SEMICOLON) {	/* declaration.		    */
		change_pstate(FIND_IDENT);
	    }
	    else if (tokentype != SPACE) {	/* Parameter declarations.  */
		change_pstate(FIND_LBRACE);
	    }
	    break;
	case FIND_LBRACE:
	    if (tokentype == LBRACE) {		/* Keep grabbing tokens	    */
		change_pstate(IN_ROUTINE);	/* until left brace.	    */
		block_level_zero();
		strcpy(curdefname, token);
		return ROUTINE_DEF_BEGIN;
	    }
#if 0
	    else if (tokentype != SPACE) {
		change_pstate(FIND_IDENT);
	    }
#endif
	    break;
	case FIND_MACRO_IDENT:
	    if (tokentype == IDENTIFIER) {
		strcpy(aElement, token);
		*aSourceLine = source_line(aSourceRecord);
		change_pstate(FIND_MACRO_LPAREN);
	    }
	    else if (tokentype != SPACE) {
		change_pstate(FIND_IDENT);
	    }
	    break;
	case FIND_MACRO_LPAREN:
	    if (tokentype == LPAREN) {
		change_pstate(IN_ROUTINE);
		strcpy(curdefname, token);
		return ROUTINE_DEF_BEGIN;
	    }
	    else {			    /* Cannot tolerate SPACE here */
		change_pstate(FIND_IDENT);
	    }
	    break;
	case IN_ROUTINE:
	    if (tokentype == LBRACE) {
		block_level_inc();
	    }
	    else if (tokentype == RBRACE) {
		if (blevel == 0) {
		    trace_blmsg(BLEND);
		    change_pstate(FIND_IDENT);
		    *aSourceLine = source_line(aSourceRecord);
		    strcpy(aElement, curdefname);
		    return ROUTINE_DEF_END;
		}
		else {
		    block_level_dec();
		}
	    }
	    else if (tokentype == IDENTIFIER) {
		strcpy(aElement, token);
		*aSourceLine = source_line(aSourceRecord);
		change_pstate(FIND_REF_LPAREN);
	    }
	    else if (tokentype == END_C_SOURCE) {
		printf("ERROR: Unexpected end of file %s\n",
		    source_name(aSourceRecord));
		return PARSE_ERROR;
	    }
	    break;
	case FIND_REF_LPAREN:
	    if (tokentype != SPACE) {
		change_pstate(IN_ROUTINE);
		if (tokentype == RBRACE) {
					    /* Must be scanner finding end  */
					    /* of macro, calling it rbrace. */
		    if (blevel == 0) {	    /* Treat as end of routine.	    */
			trace_blmsg(
		    "\nTRACE: brace level already 0 (assuming end of macro)\n");
			change_pstate(FIND_IDENT);
			*aSourceLine = source_line(aSourceRecord);
			strcpy(aElement, curdefname);
			return ROUTINE_DEF_END;
		    }
		    else {		    /* Must be end of data	    */
			block_level_dec();  /* initializer list.	    */
			trace_blmsg("(assuming end of data initializer)\n");
		    }
		}
	    }
	    if (tokentype == LPAREN) {
		return ROUTINE_REF;
	    }
	    break;
	}
    } while (tokentype != END_C_SOURCE);
    change_pstate(FIND_IDENT);
    return END_OF_SOURCE;
}

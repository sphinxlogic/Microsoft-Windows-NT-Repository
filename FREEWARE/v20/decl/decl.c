/*
 * Program to convert a C declaration into a word
 * description, as in:
 *
 *	Input:			Output:
 *
 *	int *daytab[13]		daytab: array[13] of pointer to int
 * 	void (*comp)()		comp: pointer to function returning void
 *
 * Copied more-or-less verbatim from 'The C Programming Language' (2nd ed)
 * by Brian Kernighan and Dennis Ritchie.  See pp122-126.
 *
 * Typed up by Dave Porter, 18-Jan-1991.
 * Halfway-reasonable error recovery added.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>

#define MAXTOKEN 100

enum {NAME, PARENS, BRACKETS};

void dcl(void);
void dirdcl(void);
int gettoken(void);

int tokentype;				/* type of last token */
char token[MAXTOKEN];			/* last token string */
char name[MAXTOKEN];			/* identifier name */
char datatype[MAXTOKEN];		/* data type */
char out[1000];				/* output string */

jmp_buf jumper;				/* for error recovery */

#define error(s) longjmp(jumper, (int)(s))

int getch(void);
void ungetch(int);

#define BUFSIZE 100

char buf[BUFSIZE];			/* buffer for ungetch */
int bufp = 0;				/* next free position in buf */
int eofflag = 0;			/* eof seen */

/*
 * main routine
 */
main()
{
    int err;

    while (err = setjmp(jumper)) {
	printf("error - %s\n", (char *) err);
	while (tokentype != '\n' && tokentype != EOF)
	    gettoken();
    }

    while (gettoken() != EOF) {
	strcpy(datatype, token);	/* 1st token is datatype */
	out[0] = '\0';
	dcl();				/* parse rest of line */
	if (tokentype != '\n') 
	    error("syntax error");
	printf("%s: %s %s\n", name, out, datatype);
    }

    return (1);
}

/* 
 * dcl: parse a declarator 
 */
void dcl(void)
{
    int ns = 0;
    while (gettoken() == '*')
	ns++;
    dirdcl();
    while (ns-- > 0)
	strcat(out, " pointer to");
}

/*
 * dirdcl: parse a direct declarator
 */
void dirdcl(void)
{
    int type;

    if (tokentype == '(') {		/* ( dcl ) */
	dcl();
	if (tokentype != ')')
	    error("misplaced )");
    } 

    else if (tokentype == NAME)       /* variable name */
	strcpy(name, token);

    else
	error("unexpected token; expected name or (dcl)");

    while (type = gettoken(), type == PARENS || type == BRACKETS)
	if (type == PARENS)
	    strcat(out, " function returning");
	else {
	    strcat(out, " array");
	    strcat(out, token);
	    strcat(out, " of");
	}
}

/*
 * return next token in input: a name, a pair of parentheses,
 * a pair of brackets optionally containing a number, or any 
 * other single character.
 */
int gettoken(void)
{
    int c;
    char *p = token;

    while (c = getch(), c == ' ' || c == '\t')
	;

    if (c == '(') {
	if ((c = getch()) == ')') {
	    strcpy(token, "()");
	    return (tokentype = PARENS);
	} else {
	    ungetch(c);
	    return (tokentype = '(');
	}
    } 

    else if (c == '[') {
	*p++ = c;
	while ((*p++ = getch()) != ']')
	    ;
	*p = '\0';
	return (tokentype = BRACKETS);
    } 

    else if (isalpha(c)) {
	*p++ = c;
	while (isalnum(c = getch()))
	    *p++ = c;
	*p = '\0';
	ungetch(c);
	return (tokentype = NAME);
    }

    else
	return (tokentype = c);

}

/*
 * Get character, possibly pushed back
 */
int getch(void)
{
    if (bufp > 0) 
	return (buf[--bufp]);
    else if (eofflag) 
	return (EOF);
    else {
	int c = getchar();
	if (c == EOF) eofflag = 1;
	return (c);
    }	
}

/*
 * Push character back on input
 */
void ungetch(int c)
{
    if (c != EOF && bufp < BUFSIZE) 
	buf[bufp++] = c;
}

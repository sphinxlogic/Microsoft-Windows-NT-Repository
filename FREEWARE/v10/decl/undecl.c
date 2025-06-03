/*
 * Program to convert a data type description into a C
 * declaration, as in:
 *
 *	Input:						Output:
 *
 *	daytab: array[13] of pointer to int		int *daytab[13]	       
 *    	comp: pointer to function returning void	void (*comp)()
 *
 * Based on the example in 'The C Programming Language' (2nd ed)
 * by Brian Kernighan and Dennis Ritchie, pp122-126, but modified to
 * take an English language description rather than the symbolic
 * notation used in the example.
 *
 * Dave Porter, 18-Jan-1991.
 *
 * Warning: the error detection is rudimentary, to say the least.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>

#define MAXTOKEN 100

enum {TYPE, FUNC, ARRAY, PTR};

int gettoken(void);
int match(char *);

int tokentype;				/* type of last token */
char token[MAXTOKEN];			/* last token string */
char out[1000];				/* output string */

int getch(void);
void ungetch(int);

#define BUFSIZE 100

char buf[BUFSIZE];			/* buffer for ungetch */
int bufp = 0;				/* next free position in buf */
int eofflag = 0;			/* eof seen */

jmp_buf jumpy;

#define error(s) longjmp(jumpy, (int)(s))

/*
 * main routine
 */
main()
{
    int err;
    char temp[MAXTOKEN];

    while (err = setjmp(jumpy)) {
	printf("error - %s\n", (char *) err);
	while (tokentype != '\n' && tokentype != EOF)
	    gettoken();
    }

    while (gettoken() != EOF) {
	strcpy(out, token);  	/* identifier */

	if (!match(":"))
	    error("expected 'identifier: description'");

	while (gettoken(), tokentype != '\n' && tokentype != EOF)
	    if (tokentype == FUNC || tokentype == ARRAY)
		strcat(out, token);
	    else if (tokentype == PTR) {
		sprintf(temp, "(*%s)", out);
                strcpy(out, temp);
	    } else if (tokentype == TYPE) {
		sprintf(temp, "%s %s", token, out);
		strcpy(out, temp);
	    } else {
		sprintf(temp, "unexpected token at '%c'", tokentype);		
		error(temp);
	    }

	printf("  %s\n", out);
    }

    return (1);
}

/*
 * return next token in input:
 *	function {returning}
 *	array {[]} {of}
 *	array {[NN]} {of}
 *	pointer {to}
 *	TYPENAME
 *	ANYCHAR
 */
int gettoken(void)
{
    int c, n;
    char *p = token;

    if (match("function")) {
	match("returning");
	strcpy(token, "()");
	return (tokentype = FUNC);
    }

    else if (match("pointer")) {
	match("to");
	strcpy(token, "*");
	return (tokentype = PTR);
    }

    else if (match("array")) {
	if (match("[")) {
	    *p++ = '[';
	    while ((*p++ = getch()) != ']')
		;
	    *p++ = '\0';
	}
	else
            strcpy(token, "[]");
        match("of");
	return (tokentype = ARRAY);
    }

    else {
        c = getch();
	if (isalpha(c)) {
	    while (isalnum(c)) {
	    	*p++ = c;
	    	c = getch();
	    }
       	    *p = '\0';
            ungetch(c);
	    return (tokentype = TYPE);
	}
	return (tokentype = c);
    }

}

/*
 * Examines the input stream to see if it matches a certain
 * string.   Leading whitespace is unconditionally skipped.
 * If the token-to-be-matched ends with an alphanumeric,
 * then the input stream must have a nonalphanumeric char
 * after the matched part - this prevents input of "foobar"
 * matching "foo".
 */
int match(char *str)
{
    int i, len, c, ok;

    while (c = getch(), c == ' ' || c == '\t')
	;
    ungetch(c);

    len = strlen(str);
    ok = 1;

    for (i = 0; i < len; i++) {         /* match token */
	c = getch();
	if (c != str[i]) {
	    ungetch(c);
	    ok = 0;
	    break;
	}
    }

    if (ok && isalnum(c)) {		/* peek ahead at delimiter */
	c = getch();
	ungetch(c);
	if (isalnum(c)) 
	    ok = 0;
    }

    if (!ok) {				/* replace input if failed */
	while (--i >= 0)
	    ungetch(str[i]);
    }

    return (ok);
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

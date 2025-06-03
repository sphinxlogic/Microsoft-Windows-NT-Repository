%{
#include <stdio.h>
#include <signal.h>
#include "addr.h"
#include "faces.h"

/*  @(#)address.y 1.4 91/05/06
 *
 *  Copyright (c) 1986, 1987, 1988, 1989, 1990, 1991, by:
 *
 *      Bill Nesheim
 *      Daniel Karrenberg
 *      David Herron
 *      Rich Salz
 *      John Mackin
 *
 *  All rights reserved.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged.
 *
 *  No responsibility is taken for any errors inherent either
 *  to the comments or the code of this program, but if reported
 *  (see README file), then an attempt will be made to fix them.
 */

/* A note on memory allocation.  It's difficult to change the code in this
 * parser to prevent memory leakage; it doesn't seem to have been planned
 * with that in mind.  So, rather than rewrite it all, I have changed all
 * references to malloc() in this file to use Amalloc() instead.  It allocates
 * out of a fixed arena, all allocations in which can be discarded and re-used
 * by one call to Afreeall().  Afree() is a null operation.
 *
 * This is a touch gross, but much easier than changing all the code, which
 * clearly wasn't written to work in a daemon-type application.
 */

#define	Afree(x)	/* nothing */
#define	AHEAPSIZE	10240		/* should be plenty */

static char *	Aheap = (char *)NULL;
static int	Aheapsize = 0;
static char *	Aptr;

extern char *	Malloc();

#define	ADDRSIZE	512

static char	*errstr;
static char	*comstr;
static char	*cp;
static char	*savecp;
static char	*saveline;
static char	 errbuf[256];
static char	 combuf[256];
static int	 iseol;

Addr		*adrlist;
Addr		*errlist;
%}

%union {
   char	 yChar;
   char	*yString;
   Dom	*yDom;
   Addr	*yAddr;
}

%token	EOL ATOM LIT_DOMAIN QUOTED_STRING

%type	<yString>	word domain_ref sub_domain local_part phrase
%type	<yDom>		domain route_list route
%type	<yAddr>		addr_spec non_local_addr_spec route_addr mailbox mbox_list group address

%start	addr_list

%%
addr_list:	addr_lel
       | addr_list addr_lel
       ;

addr_lel: address EOL {
	    $1->comment = comstr;
	    $1->error = errstr;
	    comstr = NULL;
	    errstr = NULL;
	    appAddr(&adrlist, $1);
	}
	| address ',' {
	    $1->comment = comstr;
	    $1->error = errstr;
	    comstr = NULL;
	    errstr = NULL;
	    appAddr(&adrlist, $1);
	}
	| error {
	    register Addr	*ap;

	    ap = newAddr();
	    errbuf[0] = '\0';
	    if (savecp > saveline) {
		if (cp && *cp)
		    Sprintf(errbuf, " after \"%.*s\", before \"%s\"\n", savecp - saveline, saveline, cp);
		else
		    Sprintf(errbuf, " after \"%.*s\"\n", savecp - saveline, saveline);
	    }
	    else {
		if (cp && *cp)
		    Sprintf(errbuf, " before \"%s\"\n", cp);
		else
		    strcpy(errbuf, "\n");
	    }
	    if (errbuf[0])
		errstr = newstring2(errstr, errbuf);
	    ap->error = errstr;
	    errstr = NULL;
	    comstr = NULL;
	    appAddr(&errlist, ap);
	}
	;

address: mailbox {
	    $$ = $1;
	}
	| group {
	    $$ = $1;
	}
	;

group	: phrase ':' mbox_list ';' {
	    register Addr	*a;

	    for (a = $3; a; a = a->next)
		a->group = $1;
	    $$ = $3;
	}
	;

mbox_list: mailbox {
	    $$ = $1;
	}
	| mbox_list ',' mailbox {
	    $3->comment = comstr;
	    $3->error = errstr;
	    comstr = NULL;
	    errstr = NULL;
	    appAddr(&($1), $3);
	    $$ = $1;
	}
	;

mailbox: addr_spec {
	    $$ = $1;
	}
	| route_addr {
	    $$ = $1;
	}
	| phrase route_addr {
	    $2->name = $1;
	    $$ = $2;
	}
	;

phrase	: word {
	    $$ = $1;
	}
	| phrase word {
	    $$ = newstring3($1, " ", $2);
	    Afree($1);
	    Afree($2);
       }
       ;

route_addr: '<' addr_spec '>' {
	   $$ = $2;
       }
       | '<' route non_local_addr_spec '>' {
	   prepDom(&($3->route), $2);
	   $$ = $3;
       }
       ;

route	: route_list ':' {
	    $$ = $1;
	}
	;

route_list: '@' domain {
	    $$ = $2;
	}
	| route_list ',' '@' domain {
	    appDom(&($1), $4);
	    $$ = $1;
	}
	;

addr_spec: non_local_addr_spec
	| local_part {
	    register Addr	*ap;

	    $$ = ap = newAddr();
	    ap->localp = $1;
	    ap->destdom = NULL;
	}
	;

non_local_addr_spec: local_part '@' domain {
	    register Addr	*ap;

	    $$ = ap = newAddr();
	    ap->localp = $1;
	    ap->destdom = $3;
	    ap->route = $3;
	}
	;

local_part: word {
		$$ = $1;
	    }
	    | local_part '.' word {
		$$ = newstring3($1, ".", $3);
		Afree($1);
		Afree($3);
	    }
	    | local_part '%' word {
		$$ = newstring3($1, "%", $3);
		Afree($1);
		Afree($3);
	    }
	    ;

domain	: sub_domain {
	    register Dom	*dp;

	    dp = newDom();
	    dp->sub[0] = $1;
	    dp->top = dp->sub;
	    $$ = dp;
	}
	| domain '.' sub_domain {
	    ($1->top)++;
	    *($1->top) = $3;
	    $$ = $1;
	}
	;

sub_domain: domain_ref {
	    $$ = $1;
	}
	| LIT_DOMAIN {
	    $$ = yylval.yString;
	}
	;

domain_ref: ATOM {
	    $$ = yyval.yString;
	}
	;

word	: ATOM {
	    $$ = yylval.yString;
	}
	| QUOTED_STRING {
	    $$ = yylval.yString;
	}
	;

%%

#include <stdio.h>
#include <ctype.h>

#define ERROR	-2


static char *
newstring3(a, b, c)
    char	*a;
    char	*b;
    char	*c;
{
    char	*p;
    char	*q;
    int		 i;

    i = strlen(a) + strlen(b) + strlen(c) + 1;
    if ((p = Amalloc((MALLOCT)i)) == NULL)
	nomem();
    q = p + strlen(strcpy(p, a));
    q += strlen(strcpy(q, b));
    Strcpy(q, c);
    return(p);
}


static char *
newstring2(a, b)
    char		*a;
    char		*b;
{
    char		*p;
    int			 i;

    if (a == (char *)0)
	a = "";
    i = strlen(a) + strlen(b) + 1;
    if ((p = Amalloc((MALLOCT)i)) == NULL)
	nomem();
    Strcpy(p, a);
    Strcat(p, b);
    return(p);
}


static void
yyerror(s)
    char	*s;
{
   switch(yychar) {
       default:
	   Sprintf(errbuf, "%s: \"%c\" unexpected", s, yylval.yChar);
	   errstr = newstring2(errstr, errbuf);
	   break;
       case LIT_DOMAIN:
       case QUOTED_STRING:
       case ATOM:
	   Sprintf(errbuf, "%s: \"%s\" unexpected", s, yylval.yString);
	   errstr = newstring2(errstr, errbuf);
	   break;
       case EOL:
       case 0: /* EOF */
	   Sprintf(errbuf, "%s: unexpected end-of-header", s);
	   errstr = newstring2(errstr, errbuf);
	   break;
   }
}


parseit(line)
    char	*line;
{
    Afreeall();
    saveline = cp = line;
    adrlist = NULL;
    errlist = NULL;
    (void)yyparse();
}


char *
eatcomment(s)
register char *s;
{
	register int parencount;

	parencount = 0;
	for (;;) {
		if (*s == '\\') {	/* quoted-pair */
			s++;
			if (*s == '\0')
				return ((char *)0);
			s++;
			if (*s == '\0')
				return ((char *)0);
			continue;
		}
		if (*s == '(')
			parencount++;
		else if (*s == ')')
			parencount--;

		if (parencount == 0)
			return (++s);
		else if (parencount < 0)
			panic("eatcomment botch");

		if (*++s == '\0')
			return ((char *)0);
	}
}


yylex()
{
    register char	*p;

    savecp = cp;
    while (isascii(*cp) && (isspace(*cp) || (*cp == '('))) {
	if (*cp == '(') {
	    p = eatcomment(cp);
	    if (p == (char *)0)
		return (EOF);
	    Strncpy(combuf, cp + 1, (p - 2) - cp);
	    combuf[(p - 2) - cp] = '\0';
	    if (comstr == NULL) {
		if ((comstr = Amalloc((MALLOCT)(strlen(combuf) + 1))) == NULL)
		    nomem();
		Strcpy(comstr, combuf);
	    }
	    else
		comstr = newstring3(comstr, ", ", combuf);
	    cp = p;
	}
	else
	    cp++;
    }

    if (!isascii(*cp))
	return(ERROR);

    switch (*cp) {
	case '\0':
	    if (iseol) {
		iseol = 0;
		return(EOF);
	    }
	    iseol = 1;
	    return(EOL);
	case ',':
	case ':':
	case ';':
	case '.':
	case '@':
	case '%':
	case '<':
	case '>':
	case '(':
	case ')':
	case ']':
	    yylval.yChar = *cp;
	    return(*cp++);
	case '[':       /* LIT_DOMAIN */
	    for (p = cp + 1; *p && *p != ']'; ) {
		if (*p == '\\') {
		    p++;
		    if (*p == '\0')
			return(EOF);
		}
		p++;
	    }
	    if (*p == '\0')
		return(EOF);
	    if ((yylval.yString = Amalloc((MALLOCT)(p - cp + 2))) == NULL)
		nomem();
	    Strncpy(yylval.yString, cp, p - cp + 1);
	    yylval.yString[p - cp + 1] = '\0';
	    cp = ++p;
	    return(LIT_DOMAIN);
	case '"':       /* QUOTED_STRING */
	    for (p = cp + 1; *p && *p != '"'; ) {
		if (*p == '\\') {
		    p++;
		    if (*p == '\0')
			return(EOF);
		}
		p++;
	    }
	    if (*p == '\0')
		return(EOF);
	    if ((yylval.yString = Amalloc((MALLOCT)(p - cp))) == NULL)
		nomem();
	    Strncpy(yylval.yString, cp + 1, p - cp);
	    yylval.yString[p - cp - 1] = '\0';
	    cp = ++p;
	    return(QUOTED_STRING);
    }
    for (p = cp; ; p++)
	switch (*p) {
	    case ',':
	    case ':':
	    case ';':
	    case '.':
	    case '@':
	    case '%':
	    case '<':
	    case '>':
	    case '(':
	    case ')':
	    case '[':
	    case ']':
	    case '"':
	    case '\0':
		goto out;
	    default:
		if (isspace(*p))
		    goto out;
	}
out:
    if ((yylval.yString = Amalloc((MALLOCT)(p - cp + 1))) == NULL)
	nomem();
    Strncpy(yylval.yString, cp, p - cp);
    yylval.yString[p - cp] = '\0';
    cp = p;
    return(ATOM);
}


/*
**  Create and initialize a new address.
*/
Addr *
newAddr()
{
    register Addr	*ap;

    if ((ap = (Addr *)Amalloc((MALLOCT)sizeof *ap)) == NULL)
	nomem();
    CLEARMEM((char *)ap, sizeof *ap);
    return(ap);
}


/*
**  Append addresslist "addr" to addresslist "head".
*/
appAddr(head, addr)
    Addr		**head;
    Addr		 *addr;
{
    register Addr	*ap;

    if (*head) {
	for (ap = *head; ap->next; ap = ap->next)
	    ;
	ap->next = addr;
    }
    else
	*head = addr;
}



/*
**  Create and initialize a new domain.
*/
Dom *
newDom()
{
    register Dom	*dp;

    if ((dp = (Dom *)Amalloc((MALLOCT)sizeof *dp)) == NULL)
	nomem();
    CLEARMEM((char *)dp, sizeof *dp);
    dp->top = dp->sub;
    return(dp);
}


/*
**  Append domainlist "dom" to domainlist "head".
*/
appDom(head, dom)
    Dom			**head;
    Dom			*dom;
{
    register Dom	*dp;

    if (*head) {
	for (dp = *head; dp->next; dp = dp->next)
	    ;
	dp->next = dom;
    }
    else
    *head = dom;
}


/*
**  Prepend domainlist "dom" before domainlist "head".
*/
prepDom(head, dom)
    Dom			**head;
    Dom			 *dom;
{
    register Dom	 *dp;

    for (dp = dom; dp->next; dp = dp->next)
	;
    dp->next = *head;
    *head = dom;
}

/*
 * Translate an Addr into a textual address.  In the simplest case,
 * this would reduce to simply localp[@destdom].  But we know
 * about source routing and do the right thing with it.
 */

#if 0
char *
makeaddress(a, flag)
register Addr *a;
int flag;
{
	char *newstring();
	char *putdom();
	static char buf[ADDRSIZE];
	register char *p;
	register Dom *d;
	extern Addr *Here;
	int sourcerouted;

	if (Here == (Addr *)0)
		panic("null Here in makeaddress");
	if (a->localp == (char *)0 || a->localp[0] == '\0')
		panic("null local-part in makeaddress");
	p = buf;
	d = a->route;
	sourcerouted = 0;
	while (d && d->next) {	/* source route */
		sourcerouted++;
		*p++ = '@';
		p = putdom(p, d);
		*p++ = d->next->next ? ',' : ':';
		d = d->next;
	}
	strcpy(p, a->localp);
	p += strlen(p);
	if (!flag && !sourcerouted && (d == (Dom *)0 || domcmp(d, Here->destdom) == 0)) {	/* local address */
		*p = '\0';
		return (newstring(buf));
	}
	*p++ = '@';
	p = putdom(p, d);
	*p = '\0';
	return (newstring(buf));
}

int
domcmp(d, e)
register Dom *d, *e;
{
	register char **p, **q;

	while (d && e) {
		p = d->sub;
		q = e->sub;
		while (p <= d->top && q <= e->top) {
			if (strcasecmp(*p, *q))
				return (1);
			p++;
			q++;
		}
		if (p <= d->top || q <= e->top)
			return (1);
		d = d->next;
		e = e->next;
	}
	if (d || e)
		return (1);

	return (0);
}

int
strcasecmp(s1, s2)
register char *s1, *s2;
{
	register int c1, c2;

#ifndef	_tolower
#define	_tolower	tolower
#endif

	for (;;) {
		c1 = *s1++;
		c2 = *s2++;
		if (c1 == '\0' && c2 == '\0')
			return (0);
		if (isupper(c1))
			c1 = _tolower(c1);
		if (isupper(c2))
			c2 = _tolower(c2);
		if (c1 != c2)
			return (c1 - c2);
	}
}

char *
newstring(sp)
	char *sp;
{
	char *np;

	np = Amalloc(strlen(sp)+1);
	if (np == NULL)
		nomem();
	strcpy(np, sp);
	return np;
}
#endif

char *
putdom(s, d)
register char *s;
register Dom *d;
{
	register char **p;
	register char *t;

	for (p = d->sub; p != d->top; p++) {
		t = *p;
		while (*s++ = *t++)
			;
		s[-1] = '.';
	}
	t = *p;
	while (*s++ = *t++)
		;
	return (--s);
}

/* print error and core dump */
char *panicstr;

extern int
panic(s)
	char *s;
{
	panicstr = s;
	fprintf(stderr, "panic: %s\n", s);
	signal(SIGQUIT, SIG_DFL);
	kill(getpid(), SIGQUIT);
	exit(-1);
}

nomem()
{
	fprintf(stderr, "out of memory\n");
	exit(1);
}

Afreeall()
{
	if (Aheap == (char *)NULL) {
		Aheap = Malloc(AHEAPSIZE);
		Aheapsize = AHEAPSIZE;
	}
	Aptr = Aheap;
}

/*
 * Note: this routine is machine dependent.  It assumes that anything can be
 * stored on a boundary that is aligned to the nearest "sizeof (long)"
 * bytes.  This should be just about universally true, but... caveat
 * executor.
 */

static char *
Amalloc(nbytes)
MALLOCT nbytes;
{
	register long align;
	register char *p;

	align = nbytes % sizeof (long);
	if (align > 0)
		nbytes += sizeof (long) - align;
	if ((Aptr + nbytes) - Aheap > Aheapsize) {
		/* Whoops!  Need more heap. */
		Aheapsize *= 2;
		Aheap = realloc(Aheap, (MALLOCT)Aheapsize);
		if (Aheap == (char *)NULL)
			nomem();
	}
	p = Aptr;
	Aptr += nbytes;
	return (p);
}

int
parse822from(line, user, host)
char *line;
char **user;
char **host;
{
	register char *p;
	static char buf[ADDRSIZE];
	extern char *hostname;

	if (!is822header(line, "From"))
		panic("bad argument to parse822from");
	p = line + sizeof "From" - 1;
	while (LWSP(*p))
		p++;
	if (*p++ != ':')
		panic("can't happen in parse822from");
	parseit(p);
	if (errlist != (Addr *)NULL)
		return (0);		/* error */
	/*
	 * This should never happen, per the grammar, but a bit of defensive
	 * programming never hurt anyone.
	 */
	if (adrlist == (Addr *)NULL)
		return (0);

	/*
	 * This is where we should be handling multiple From: addresses.
	 * They'll manifest themselves as multiple elements in adrlist.
	 * For the moment, we just take the first element of adrlist (we've
	 * just verified that it has at least one) and if there are any
	 * more, they are silently ignored.
	 *
	 * Note that creatively source-routed addresses will cause problems
	 * (uncreative ones won't: as long as the destdom is complete, all is
	 * OK, but if the destdom relies on the route, we're losing: e.g.
	 * <@ai.mit.edu:person@video> should, ideally, give a host of
	 * "video.mit.edu", not "video".  That problem is insoluble, though,
	 * even in theory; and we're certainly doing much better than
	 * parsefrom()).
	 *
	 * The same problem exists with routing kludges; if they use
	 * incomplete domains, we're screwed, even in theory.  Note that,
	 * again, we're doing no worse than parsefrom() used to.  A real-world
	 * example: <urban%hercules@rand.org>.  Of course, the message here is
	 * that people shouldn't use routing kludges, especially when (as
	 * in this example) they don't have to.
	 *
	 * Hopefully, this will become less of a problem when we change over
	 * to using the person's full name, rather than a (user, host)
	 * tuple, as the primary means of face location.
	 */

	*user = adrlist->localp;
	if (adrlist->destdom != (Dom *)NULL) {
		p = putdom(buf, adrlist->destdom);
		*p = '\0';
		*host = buf;
	}
	else
		*host = hostname;

	return (1);			/* OK */
}

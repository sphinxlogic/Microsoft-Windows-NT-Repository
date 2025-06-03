/* $Id: sw.c,v 1.2 92/01/11 16:08:43 usenet Exp $
 *
 * $Log:	sw.c,v $
 * Revision 1.2  92/01/11  16:08:43  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4  1991/09/09  20:27:37  sob
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
#include "util.h"
#include "head.h"
#include "only.h"
#include "term.h"
#include "ng.h"
#include "intrp.h"
#include "INTERN.h"
#include "sw.h"

void
sw_init(argc,argv,tcbufptr)
int argc;
char *argv[];
char **tcbufptr;
{
    register int i;

    if (argc >= 2 && strEQ(argv[1],"-c"))
	checkflag=TRUE;			/* so we can optimize for -c */
    interp(*tcbufptr,1024,GLOBINIT);
    sw_file(tcbufptr,FALSE);
#ifdef USETHREADS
    if (use_threads)
	safecpy(*tcbufptr,getval("TRNINIT",getenv("RNINIT")),1024);
    else
#endif
	safecpy(*tcbufptr,getenv("RNINIT"),1024);
    if (**tcbufptr) {
	if (**tcbufptr == '/') {
	    sw_file(tcbufptr,TRUE);
	}
	else
	    sw_list(*tcbufptr);
    }

    for (i = 1; i < argc; i++)
	decode_switch(argv[i]);
}

void
sw_file(tcbufptr,bleat)
char **tcbufptr;
bool_int bleat;
{
    int initfd = open(*tcbufptr,0);
	
    if (initfd >= 0) {
	fstat(initfd,&filestat);
	if (filestat.st_size > 1024)
	    *tcbufptr = saferealloc(*tcbufptr,(MEM_SIZE)filestat.st_size);
	if (filestat.st_size) {
	    read(initfd,*tcbufptr,(int)filestat.st_size);
	    (*tcbufptr)[filestat.st_size-1] = '\0';
				/* wipe out last newline */
	    sw_list(*tcbufptr);
	}
	else
	    **tcbufptr = '\0';
	close(initfd);
    }
    else {
	if (bleat)
	    printf(cantopen,*tcbufptr) FLUSH;
	**tcbufptr = '\0';
    }
}

/* decode a list of space separated switches */

void
sw_list(swlist)
char *swlist;
{
    char *tmplist = safemalloc((MEM_SIZE) strlen(swlist) + 2);
					/* semi-automatic string */
    register char *p, inquote = 0;

    strcpy(tmplist,swlist);
    for (p=tmplist; isspace(*p); p++) ;	/* skip any initial spaces */
    while (*p) {			/* "String, or nothing" */
	if (!inquote && isspace(*p)) {	/* word delimiter? */
	    *p++ = '\0';		/* chop here */
	    while (isspace(*p))		/* these will be ignored later */
		p++;
	}
	else if (inquote == *p) {
	    strcpy(p,p+1);		/* delete trailing quote */
	    inquote = 0;		/* no longer quoting */
	}
	else if (!inquote && *p == '"' || *p == '\'') {
					/* OK, I know when I am not wanted */
	    inquote = *p;		/* remember single or double */
	    strcpy(p,p+1);		/* delete the quote */
	}				/* (crude, but effective) */
	else if (*p == '\\') {		/* quoted something? */
	    if (p[1] == '\n')		/* newline? */
		strcpy(p,p+2);		/* "I didn't see anything" */
	    else {
		strcpy(p,p+1);		/* delete the backwhack */
		p++;			/* leave the whatever alone */
	    }
	}
	else
	    p++;			/* normal char, leave it alone */
    }
    *++p = '\0';			/* put an extra null on the end */
    if (inquote)
	printf("Unmatched %c in switch\n",inquote) FLUSH;
    for (p = tmplist; *p; /* p += strlen(p)+1 */ ) {
	decode_switch(p);
	while (*p++) ;			/* point at null + 1 */
    }
    free(tmplist);			/* this oughta be in Ada */
}

/* decode a single switch */

void
decode_switch(s)
register char *s;
{
    while (isspace(*s))			/* ignore leading spaces */
	s++;
#ifdef DEBUGGING
    if (debug)
	printf("Switch: %s\n",s) FLUSH;
#endif
    if (*s != '-' && *s != '+') {	/* newsgroup pattern */
	setngtodo(s);
    }
    else {				/* normal switch */
	bool upordown = *s == '-' ? TRUE : FALSE;
	char tmpbuf[LBUFLEN];

	s++;
	switch (*s) {
#ifdef TERMMOD
	case '=': {
	    char *beg = s+1;

	    while (*s && *s != '-' && *s != '+') s++;
	    cpytill(tmpbuf,beg,*s);
	    if (upordown ? strEQ(getenv("TERM"),tmpbuf)
	    		 : strNE(getenv("TERM"),tmpbuf) ) {
		decode_switch(s);
	    }
	    break;
	}
#endif
#ifdef BAUDMOD
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	    if (upordown ? (just_a_sec*10 <= atoi(s))
	    		 : (just_a_sec*10 >= atoi(s)) ) {
		while (isdigit(*s)) s++;
		decode_switch(s);
	    }
	    break;
#endif
	case '/':
	    if (checkflag)
		break;
#ifdef SETENV
	    setenv("SAVEDIR",  upordown ? "%p/%c" : "%p" );
	    setenv("SAVENAME", upordown ? "%a"    : "%^C");
#else
	    notincl("-/");
#endif
	    break;
	case 'a':
#ifdef USETHREADS
	    thread_always = upordown;
#else
	    notincl("-a");
#endif
	    break;
	case 'c':
	    checkflag = upordown;
	    break;
	case 'C':
	    s++;
	    if (*s == '=') s++;
	    docheckwhen = atoi(s);
	    break;
	case 'd': {
	    if (checkflag)
		break;
	    s++;
	    if (*s == '=') s++;
	    if (cwd) {
		chdir(cwd);
		free(cwd);
	    }
	    cwd = savestr(s);
	    break;
	}
#ifdef DEBUGGING
	case 'D':
	    s++;
	    if (*s == '=') s++;
	    if (*s)
		if (upordown)
		    debug |= atoi(s);
		else
		    debug &= ~atoi(s);
	    else
		if (upordown)
		    debug |= 1;
		else
		    debug = 0;
	    break;
#endif
	case 'e':
	    erase_screen = upordown;
	    break;
	case 'E':
#ifdef SETENV
	    s++;
	    if (*s == '=')
		s++;
	    strcpy(tmpbuf,s);
	    s = index(tmpbuf,'=');
	    if (s) {
		*s++ = '\0';
		setenv(tmpbuf,s);
	    }
	    else
		setenv(tmpbuf,nullstr);
#else
	    notincl("-E");
#endif
	    break;
	case 'F':
	    s++;
	    indstr = savestr(s);
	    break;
#ifdef INNERSEARCH
	case 'g':
	    gline = atoi(s+1)-1;
	    break;
#endif
	case 'H':
	case 'h': {
	    register int len, i;
	    char *t;
	    int flag = (*s == 'h' ? HT_HIDE : HT_MAGIC);
	    
	    if (checkflag)
		break;
	    s++;
	    len = strlen(s);
	    for (t=s; *t; t++)
		if (isupper(*t))
		   *t = tolower(*t);
	    for (i=HEAD_FIRST; i<HEAD_LAST; i++)
		if (!len || strnEQ(s,htype[i].ht_name,len))
		    if (upordown)
			htype[i].ht_flags |= flag;
		    else
			htype[i].ht_flags &= ~flag;
	    break;
	}
	case 'i':
	    s++;
	    if (*s == '=') s++;
	    initlines = atoi(s);
	    initlines_specified = TRUE;
	    break;
	case 'l':
	    muck_up_clear = upordown;
	    break;
	case 'L':
#ifdef CLEAREOL
	    can_home_clear = upordown;
#else
	    notincl("-L");
#endif
	    break;
	case 'M':
	    mbox_always = upordown;
	    break;
	case 'm':
	    s++;
	    if (*s == '=') s++;
	    if (!upordown)
		marking = NOMARKING;
	    else if (*s == 'u')
		marking = UNDERLINE;
	    else {
		marking = STANDOUT;
	    }
	    break;
	case 'N':
	    norm_always = upordown;
	    break;
#ifdef VERBOSE
	case 'n':
	    fputs("This isn't readnews.  Don't use -n.\n\n",stdout) FLUSH;
	    break;
#endif
	case 'o':
	    s++;
	    if (*s == '=') s++;
	    if (*s <= '9' && *s >= '0') {
		olden_days = atoi(s);
		do {
		    s++;
		} while (*s <= '9' && *s >= '0');
	    } else
		olden_days = upordown;
	    break;
	case 'r':
	    findlast = upordown;
	    break;
	case 's':
	    s++;
	    if (*s == '=') s++;
	    if (*s) {
		countdown = atoi(s);
		suppress_cn = FALSE;
	    }
	    else {
		if (!upordown)
		    countdown = 5;
		suppress_cn = upordown;
	    }
	    break;
	case 'S':
#ifdef ARTSEARCH
	    s++;
	    if (*s == '=') s++;
	    if (*s)
		scanon = atoi(s);
	    else
		scanon = upordown*3;
#else
	    notincl("-S");
#endif
	    break;
	case 't':
#ifdef VERBOSE
#ifdef TERSE
	    verbose = !upordown;
#else
	    notincl("+t");
#endif
#else
	    notincl("+t");
#endif
	    break;
	case 'T':
	    typeahead = upordown;
	    break;
	case 'v':
#ifdef VERIFY
	    verify = upordown;
#else
	    notincl("-v");
#endif
	    break;
	case 'x':
#ifdef USETHREADS
	    s++;
	    if (*s == '=') s++;
	    if (*s <= '9' && *s >= '0') {
		if ((max_tree_lines = atoi(s)) > 11)
		    max_tree_lines = 11;
		do {
		    s++;
		} while (*s <= '9' && *s >= '0');
	    } else
		max_tree_lines = 6;
	    if (*s)
		strncpy(select_order, s, 3);
	    use_threads = upordown;
#else
	    notincl("-x");
#endif
	    break;
	case 'X':
#ifdef USETHREADS
	    s++;
	    if (*s == '=') s++;
	    if (*s <= '9' && *s >= '0') {
		select_on = atoi(s);
		do {
		    s++;
		} while (*s <= '9' && *s >= '0');
	    } else
		select_on = upordown;
	    if (*s)
		end_select = *s++;
	    if (*s)
		page_select = *s;
#else
	    notincl("-X");
#endif
	    break;
	/*
	 * People want a way to avoid checking for new newsgroups on startup.
	 */
	case 'q':
		quickstart = upordown;
		break;
	default:
#ifdef VERBOSE
	    IF(verbose)
		printf("\nIgnoring unrecognized switch: -%c\n", *s) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		printf("\nIgnoring -%c\n", *s) FLUSH;
#endif
	    break;
	}
    }
}

/* print current switch values */

void
pr_switches()
{
    static char mp[2] = {'+','-'};
    register int i;
    
    fputs("\nCurrent switch settings:\n",stdout);
    printf("%c/ ", mp[strEQ(getval("SAVEDIR",SAVEDIR),"%p/%c")]);
    printf("%ca ", mp[thread_always]);
    printf("%cc ", mp[checkflag]);
    printf("-C%d ", docheckwhen);
    printf("-d%s ", cwd);
#ifdef DEBUGGING
    if (debug)
	printf("-D%d ", debug);
#endif
    printf("%ce ", mp[erase_screen]);
    printf("-F\"%s\" ", indstr);
#ifdef INNERSEARCH
    printf("-g%d", gline);
#endif
    putchar('\n');
#ifdef VERBOSE
    if (verbose) {
	for (i=HEAD_FIRST; i<HEAD_LAST; i++)
	    printf("%ch%s%c",
		mp[htype[i].ht_flags & HT_HIDE], htype[i].ht_name,
		(! (i % 5) ? '\n' : ' ') );
    }
#endif
    printf("-i%d ", initlines);
    printf("%cl ", mp[muck_up_clear]);
#ifdef CLEAREOL
    printf("%cL ", mp[can_home_clear]);
#endif /* CLEAREOL */
    if (marking)
	printf("-m%c ",marking==UNDERLINE?'u':'s');
    else
	printf("+m ");
    printf("%cM ", mp[mbox_always]);
    printf("%cN ", mp[norm_always]);
    if (olden_days)
	printf("-o%d ", olden_days);
    else
	printf("+o ");
    printf("%cr ", mp[findlast]);
    if (countdown)
	printf("-s%d ", countdown);
    else
	printf("%cs ", mp[suppress_cn]);
#ifdef ARTSEARCH
    if (scanon)
	printf("-S%d ",scanon);
    else
	printf("+S ");
#endif
#ifdef VERBOSE
#ifdef TERSE
    printf("%ct ", mp[!verbose]);
#endif
#endif
    printf("%cT ", mp[typeahead]);
#ifdef VERIFY
    printf("%cv ", mp[verify]);
#endif
#ifdef USETHREADS
    if (use_threads)
	printf("-x%d%s ",max_tree_lines,select_order);
    else
	printf("+x ");
    if (select_on)
	printf("-X%d%c%c ",select_on,end_select,page_select);
    else
	printf("+X ");
#endif
    fputs("\n\n",stdout) FLUSH;
#ifdef ONLY
    if (maxngtodo) {
#ifdef VERBOSE
	IF(verbose)
	    fputs("Current restriction:",stdout);
	ELSE
#endif
#ifdef TERSE
	    fputs("Only:",stdout);
#endif
	for (i=0; i<maxngtodo; i++)
	    printf(" %s",ngtodo[i]);
	fputs("\n\n",stdout) FLUSH;
    }
#ifdef VERBOSE
    else if (verbose)
	fputs("No restriction.\n\n",stdout) FLUSH;
#endif
#endif
}

void
cwd_check()
{
    char tmpbuf[LBUFLEN];

    if (!cwd)
	cwd = savestr(filexp("~/News"));
    strcpy(tmpbuf,cwd);
    if (chdir(cwd)) {
	safecpy(tmpbuf,filexp(cwd),sizeof tmpbuf);
	if (makedir(tmpbuf,MD_DIR) < 0 || chdir(tmpbuf) < 0) {
	    interp(cmd_buf, (sizeof cmd_buf), "%~/News");
	    if (makedir(cmd_buf,MD_DIR) < 0)
		strcpy(tmpbuf,homedir);
	    else
		strcpy(tmpbuf,cmd_buf);
	    chdir(tmpbuf);
#ifdef VERBOSE
	    IF(verbose)
		printf("\
Cannot make directory %s--\n\
	articles will be saved to %s\n\
\n\
",cwd,tmpbuf) FLUSH;
	    ELSE
#endif
#ifdef TERSE
		printf("\
Can't make %s--\n\
	using %s\n\
\n\
",cwd,tmpbuf) FLUSH;
#endif
	}
    }
    free(cwd);
    getwd(tmpbuf);
    if (eaccess(tmpbuf,2)) {
#ifdef VERBOSE
	IF(verbose)
	    printf("\
Current directory %s is not writeable--\n\
	articles will be saved to home directory\n\n\
",tmpbuf) FLUSH;
	ELSE
#endif
#ifdef TERSE
	    printf("%s not writeable--using ~\n\n",tmpbuf) FLUSH;
#endif
	strcpy(tmpbuf,homedir);
    }
    cwd = savestr(tmpbuf);
}

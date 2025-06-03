/* $Id: artio.c,v 1.2 92/01/11 16:03:42 usenet Exp $
 *
 * $Log:	artio.c,v $
 * Revision 1.2  92/01/11  16:03:42  usenet
 * header twiddling, bug fixes
 * 
 * Revision 4.4  1991/09/09  20:18:23  sob
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
#ifdef SERVER
#include "server.h"
#endif
#include "INTERN.h"
#include "artio.h"

void
artio_init()
{
    ;
}

/* open an article, unless it's already open */

FILE *
artopen(artnum)
ART_NUM artnum;
{
#ifdef SERVER
    return nntpopen(artnum,GET_ARTICLE);
#else
    char artname[MAXFILENAME];		/* filename of current article */

    if (artnum < 1)
	return Nullfp;
    if (openart == artnum) {		/* this article is already open? */
	fseek(artfp,0L,0);		/* just get to the beginning */
	return artfp;			/* and say we succeeded */
    }
    if (artfp != Nullfp) {		/* it was somebody else? */
	fclose(artfp);			/* put them out of their misery */
	openart = 0;			/* and remember them no more */
    }
    sprintf(artname,"%ld",(long)artnum);
					/* produce the name of the article */
    if (artfp = fopen(artname,"r"))	/* if we can open it */
	openart = artnum;		/* remember what we did here */
#ifdef LINKART
    {
	char tmpbuf[256];
	char *s;

	if (fstat(fileno(artfp),&filestat))
	    return artfp;
	if (filestat.st_size < (sizeof tmpbuf)) {
	    fgets(tmpbuf,(sizeof tmpbuf),artfp);
	    if (*tmpbuf == '/') {	/* is a "link" to another article */
		fclose(artfp);
		if (s=index(tmpbuf,'\n'))
		    *s = '\0';
		if (!(artfp = fopen(tmpbuf,"r")))
		    openart = 0;
		else {
		    if (*linkartname)
			free(linkartname);
		    linkartname = savestr(tmpbuf);
		}
	    }
	    else
		fseek(artfp,0L,0);		/* get back to the beginning */
	}
    }
#endif
    return artfp;			/* and return either fp or NULL */
#endif /* SERVER */
}

#ifdef SERVER
static long our_pid=0;

FILE *
nntpopen(artnum,function)
ART_NUM artnum;
ART_PART function;
{
    char artname[MAXFILENAME];		/* filename of current article */
    char intrpwork[MAXFILENAME];	/* filename of current article */
    if (our_pid == 0)
	our_pid = getpid();
    if (artnum < 1)
	return Nullfp;
    if ((openart == artnum) && (openpart >= function))
    {					/* this article is already open? */
	fseek(artfp,0L,0);		/* just get to the beginning */
	return artfp;			/* and say we succeeded */
    }
    if (artfp != Nullfp) {		/* it was somebody else? */
	fclose(artfp);			/* put them out of their misery */
	nntpclose();
	openart = 0;			/* and remember them no more */
    }
    interp(intrpwork,MAXFILENAME-1,"%P");
    sprintf(artname,"%s/rrn%ld.%ld", intrpwork,(long) artnum, our_pid);
    artfp = fopen(artname, "w+");	/* create the temporary article */
    if (artfp == Nullfp) {
	UNLINK(artname);
	return Nullfp;
    }
    switch (function){
	    case GET_STATUS:
		function = GET_HEADER;	/* fall through */
	    case GET_HEADER:
		sprintf(ser_line, "HEAD %ld", (long)artnum);
		break;
	    case GET_ARTICLE:
		sprintf(ser_line, "ARTICLE %ld", (long)artnum);
		break;
    }	    
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf(">%s\n", ser_line) FLUSH;
#endif
    put_server(ser_line);		/* ask the server for the article */
    if (nntp_get(ser_line, sizeof(ser_line)) < 0) {
	fprintf(stderr, "\nrrn: Unexpected close of server socket.\n");
	finalize(1);
    }
#ifdef DEBUGGING
    if (debug & DEB_NNTP)
	printf("<%s\n", ser_line) FLUSH;
#endif
    if (*ser_line == CHAR_FATAL) {	/* Fatal error */
		fprintf(stderr,"\nrrn: %s\n",ser_line);
		finalize(1);
	}
    if (*ser_line != CHAR_OK) {		/* and get it's reaction */
	fclose(artfp);
	artfp = Nullfp;
	UNLINK(artname);
 	errno = ENOENT;		/* Simulate file-not-found */
        return Nullfp;
    }

    for (;;) {
        if (nntp_get(ser_line, sizeof(ser_line)) < 0) {
	    fprintf(stderr, "\nrrn: Unexpected close of server socket.\n");
	    finalize(1);
	}
	if (ser_line[0] == '.' && ser_line[1] == '\0')
		break;
	fputs((ser_line[0] == '.' ? ser_line + 1 : ser_line), artfp);
	putc('\n', artfp);
    }
    openpart = function;
    if (function == GET_HEADER)
	 putc('\n', artfp); /* req'd blank line after header */
    fseek(artfp, 0L, 0);		/* Then get back to the start */
    openart = artnum;
    return artfp;			/* and return either fp or NULL */
}

void
nntpclose()
{
    char artname[MAXFILENAME];		/* filename of current article */
    char intrpwork[MAXFILENAME];	/* filename of current article */
    if (our_pid == 0)
	our_pid = getpid();
    interp(intrpwork,MAXFILENAME-1,"%P");
    sprintf(artname, "%s/rrn%ld.%ld", intrpwork,(long) openart, our_pid);
    UNLINK(artname);
}
#endif

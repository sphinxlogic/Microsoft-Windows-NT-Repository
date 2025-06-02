27-Aug-85 19:17:33-MDT,1774;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Tue 27 Aug 85 19:17:29-MDT
Received: from usenet by TGR.BRL.ARPA id a001107; 27 Aug 85 20:58 EDT
From: "Larry J. Barello" <larry@tikal.uucp>
Newsgroups: net.sources
Subject: Son of 'which'
Message-ID: <225@tikal.UUCP>
Date: 26 Aug 85 14:40:19 GMT
To:       unix-sources@BRL-TGR.ARPA

A week or so I posted a quick C version of the UCB shell script
"which".  It takes as arguments command names and searches your path
for instances of them.  The version I posted had some cute bugs: it
didn't work if you didn't have a path or if there were a null
component in the path (Thanks to Tom Truscott).  To the folks with v8
shell: wish I had it, sounds like it make a lot of utilities, like
this one, useless.

Here is a fancified version of the original one.

	..!uw-beaver!teltone!larry

-------- cut here (duh) -------

#include <stdio.h>

char *getenv();
char *index();

int
main(ac,av)
char **av;
{
    char *origpath, *path, *cp;
    char buf[200];
    char patbuf[512];
    int quit, found;

    if (ac < 2) {
	fprintf(stderr, "Usage: %s cmd [cmd, ..]\n", *av);
	exit(1);
    }
    if ((origpath = getenv("PATH")) == 0)
	origpath = ".";

    av[ac] = 0;
    for(av++ ; *av; av++) {

	strcpy(patbuf, origpath);
	cp = path = patbuf;
	quit = found = 0;

	while(!quit) {
	    cp = index(path, ':');
	    if (cp == NULL) 
		quit++;
	    else
		*cp = '\0';

	    sprintf(buf, "%s/%s", (*path ? path:"."), *av);
	    path = ++cp;

	    if (access(buf, 1) == 0) {
		printf("%s\n", buf);
		found++;
	    }
	}
	if (!found) 
	    printf("No %s in %s\n", *av, origpath);
    }
    exit(0);
}

------------------

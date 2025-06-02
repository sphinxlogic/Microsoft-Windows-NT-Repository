Article 2755 of net.sources:
Path: brl-smoke!brl-adm!rutgers!clyde!watmath!sunybcs!uggworek
From: uggworek@sunybcs.UUCP (Don Gworek)
Newsgroups: net.sources
Subject: cn -- a checknews script
Message-ID: <1549@sunybcs.UUCP>
Date: 1 Dec 86 03:31:17 GMT
Sender: nobody@sunybcs.UUCP
Reply-To: uggworek@sunybcs.UUCP (Don Gworek)
Distribution: na
Organization: SUNY/Buffalo Computer Science
Lines: 31
Keywords: awk, rn

Here's a cute little shell script that lists available news
articles in multicolumn form, such as:

[31] comp.lang.c        [ 1] wny.yumyum         [ 2] mod.sources        
[ 4] comp.unix.questio  [ 1] comp.unix.wizards  [ 1] mod.computers.sun  
[ 2] mod.computers.wor  


The script uses rn's -c and -s options.  It's based on an
alias by Jeff Forys at CU/Boulder.

--------------------------- cut here --------------------------------
#! /bin/sh

# Sun Nov 30 22:34:48 EST 1986	(...!sunybcs!gworek / ...!boulder!forys)
#
#	cn - Check for available news, multicolumn style.
#

	rn -s250 -c | \
		awk '{ \
			printf "[%2.2s] %-17.17s  ", $5, $4; \
			if ((NR % 3) == 0) \
				print ""; \
		} END { \
			if ((NR) && (NR % 3)) \
				print ""; \
		}'
--------------------------- cut here --------------------------------



10-Dec-85 04:19:35-MST,1975;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Tue 10 Dec 85 04:19:30-MST
Received: from usenet by TGR.BRL.ARPA id a023483; 10 Dec 85 5:43 EST
From: garcia <amg22@aluxz.uucp>
Newsgroups: net.sources,net.micro.att
Subject: batch command for AT&T UNIX PC 7300
Message-ID: <287@aluxz.UUCP>
Date: 9 Dec 85 18:12:43 GMT
Xref: seismo net.sources:4005 net.micro.att:795
To:       unix-sources@BRL-TGR.ARPA



	I ran across the following shell script which does a nice job
of providing the much missed batch command on the AT&T UNIX PC.  The
original file (below the cut line) is pretty much self explanatory.

	I hope others find it as useful as I do.  I would also be
interested in any feedback on this file or the availability of other
goodies for the PC 7300.

			Gus Garcia
			AT&T Bell Laboratories
			555 Union Blvd
			Allentown, PA  18103

			aluxz!amg22

------------------------- Cut along this line -------------------------
#........................................................................
#:									:
#: batch -- Run a background shell script independent of login status.	:
#:	Copyright 1985 by G.R.A.M. Associates - For the Public Domain	:
#:									:
#:	Usage:	batch [-] [file ...]					:
#:		where:	file	is 0 or more files containing shell	:
#:				scripts 				:
#:			- 	indicates standard input.		:
#:									:
#:	Files:	$HOME/mmddhhmmss (temporary file removed after use)	:
#:									:
#:  G.R.A.M. Associates assumes no responsibility for any use or misuse	:
#: of this program.  Please don't remove this header.			:
#:......................................................................:

FILE=$HOME/`date '+%m%d%H%M%S'`		# Make a unique file name
( cat $* ; echo rm -f $FILE ) > $FILE	# Need a separate process for input
nohup sh -c "sh $FILE 2>&1 | mail $LOGNAME" 2>&1 >/dev/null &	# Run it
echo batch: Executing $FILE		# Let 'em know its running

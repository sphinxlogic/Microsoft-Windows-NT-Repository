18-Jan-86 18:11:47-MST,1404;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Sat 18 Jan 86 18:11:39-MST
Received: from usenet by TGR.BRL.ARPA id a021441; 18 Jan 86 19:46 EST
From: Robert Virding <robert@erix.uucp>
Newsgroups: net.sources
Subject: Re: Using MH as the mail interface to rn
Message-ID: <1039@erix.UUCP>
Date: 17 Jan 86 17:15:06 GMT
To:       unix-sources@BRL-TGR.ARPA

There's an easier way to do it which also allows using draft folders
in MH.  In .login (or whatever, .rninit is also a good place):

setenv EMAILPOSTER "~/bin/mh comp -form %h"
setenv EMAILHEADER \
'To: %f\\n\
Cc: %L\\n\
Subject: Re: %S\\n\
In-Reply-To: Your message of %[date]\\n\
             %i\\n\
%(%[references]!=^\$?References\\: %[references]\\n)\
Newsgroups: %n\\n-------\\n'

~/bin/mh is defined as:

#! /bin/sh
#	A small shell script to run the MH-system.
#	Includes the mh command directory in the PATH
#	If a (mh) command is given this is run instead.
#
export PATH; PATH=/usr/new/mh:$PATH	# the mh-system commands

if [ $# = 0 ]
then
	export PS1; PS1="MH> "		# an informative prompt used by sh
	exec $SHELL			# use the default shell
else
	exec sh -c "$*"			# run through shell
fi

This file is fine for running a subshell for MH as well.

			Robert Virding  @ L M Ericsson, Stockholm
			UUCP: {decvax,philabs,seismo}!mcvax!enea!erix!robert

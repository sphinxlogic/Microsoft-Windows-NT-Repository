19-Sep-85 13:29:56-MDT,3094;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Thu 19 Sep 85 13:29:48-MDT
Received: from usenet by TGR.BRL.ARPA id a006005; 18 Sep 85 19:41 EDT
From: Glenn Reid <greid@adobe.uucp>
Newsgroups: net.sources
Subject: Great tool for pre-processing KILL files with 'rn'
Message-ID: <676@adobe.UUCP>
Date: 16 Sep 85 20:03:16 GMT
Keywords: rn KILL fun
To:       unix-sources@BRL-TGR.ARPA

--------------------- cut here ---------------------
# Startup macros for 'rn'.  Should reside in the file ".rnmac"
# in your home directory (or your DOT directory, if you do it 
# that way).  These macros allow pre-processing of KILL files,
# among other things.

# Glenn Reid {glacier,decwrl}!adobe!greid  Mon Sep 16 12:56:23 1985

# This is best invoked by running 'rn' and piping the letter 'z'
# into the thing until it pukes:
#	yes z | rn    >& /dev/null

# The idea is that by always typing 'z', you will always enter each
# newsgroup that has unread news (which will cause the KILL file to
# be processed for that group), but will exit immediately after that,
# and then duck into the next newsgroup.  This will continue until
# the last newsgroup is entered and exited, at which point you will
# quit 'rn'.  The point of having the 'z' key do all these different
# things is so that you don't have to pay attention to where you are,
# and you can use the above-mentioned command line to process the KILL
# files *before* you read the news, so you save lots of search time.

# I originally intended this to be run in the background at some
# random time, so that I would never have to know about the KILLed
# articles, but I have discovered that is great fun to just type:
#	yes | rn
# and watch the thing cycle through the newsgroups, blowing away all
# those hated articles.  In fact, it is arguably much more fun than
# actually reading the news.

# NOTE: These macro names are ridiculous and are chosen
#	so that they do not conflict with any standard 'rn' bindings.
#	The 'driver' macro is the 'z' macro (last here), and you should
#	really start reading there, since it calls the other macros as
#	necessary.

 # the '*' macro is called by 'Z', quits if last newsgroups, otherwise 'y'..
 # the condition %C=$ returns TRUE if the current newsgroup (in dot mode)
 # is the last newsgroup (i.e. "End of newsgroups--what next? [npq]")

*	%(%C=$?q:y)

 # the 'Z' macro is called by 'z', calls '*' if there are unread articles...
 # the %u=0 condition returns TRUE if there are no 'unread articles' in
 # the current newsgroup.

Z	%(%u=0?q:^(*^))

 # the 'z' macro returns 'q' in any mode other than newsgroup selection mode,
 # wherein it calls the 'Z' macro...
 # the %m!=n means: "if mode is not 'newsgroup selection' mode"
 # the ? clause is executed if condition is TRUE, the : clause otherwise
 # the godawful ^( ^) syntax is required by 'rn' to be able to call a
 # macro by name on the right-hand side of another macro.  Otherwise no
 # expansion is done.

z	%(%m!=n?q:^(Z^))

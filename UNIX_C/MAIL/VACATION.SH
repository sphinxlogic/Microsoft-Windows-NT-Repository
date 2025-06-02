19-Dec-85 00:18:00-MST,3330;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Thu 19 Dec 85 00:17:54-MST
Received: from usenet by TGR.BRL.ARPA id a007919; 19 Dec 85 1:47 EST
From: Eric Mazur <mazur@harvard.uucp>
Newsgroups: net.sources
Subject: vacation.sh
Message-ID: <558@harvard.UUCP>
Date: 18 Dec 85 21:39:58 GMT
Keywords: mail-answering, vacation, shell script, Venix
To:       unix-sources@BRL-TGR.ARPA

A little while ago I requested from the net a program to answer incoming
mail during my absence (No, I don't have sendmail; can't use .forward...).
Since I received no reply, I started writing my own little shell script.
Even though I probably re-invented the wheel, it was nice, and I think the
result is neat! Besides, some people might not have seen this wheel yet.
So here we go. The header explains it all. Happy holidays!

Eric Mazur

P.S.:	I would very much appreciate receiving any comments, suggestions,
	improvements and bug reports you have. It runs fine on Venix/PRO,
	so (with all respect for Venix) it should really work on any
	other Unix version.

ARPA-NET:	mazur@harvard.harvard.EDU
BITNET:		mazur@harvunxh.bitnet
UUCP:							 /-----	mazur
					 		/  /---	dasy!mazur
{seismo,harpo,ihnp4,linus,allegra,ut-sally}!harvard! ------
							\  \___	lasex!mazur
							 \_____	molphy!mazur

------------------------------- vacation(.sh) ---------------------------------
: Shell script to answer your mail while you are gone. Will only work on
: Usenet, probably not to other networks. Will not work if you don't have
: the UCB "from" utility. You might try "grep From" on your mailbox instead,
: but that is not water-tight.

: USAGE:
: 1. Put a ".vacation" note in your home directory - this is the file that
:    is going to be sent in reply to your incoming mail.
: 2. Type "vacation [-c]" and have a nice vacation. The -c option will
:    cause the program to keep a ".vmbox.log" of answered mail.
: 3. When you come back - heaven forbid - remove the ".vacation" file.
: 4. Read your mail with "mail -f vmbox".

: AUTHOR: Eric Mazur, 17 December 1985 - about to leave for a vacation.
:	  Mail improvements and bugs to: mazur@harvard.uucp

: BUG: Relies heavily on the fact that the basename of your home directory
:      is the same as your login name, but unfortunately "who am i"
:      yield the desired answer when executed by "at".

case $1 in
"")	;;
-c)	check=1 ;;
*)	echo Usage: vacation [-c]; exit ;;
esac

if test -f $HOME/.vacation		: vacation will die if file removed
then
	me=`basename $HOME`
	mybox='/usr/spool/mail/'$me
	to=`from | awk '{printf("%s ",$2)}END{printf("\n")}'`
	cat $mybox >> $HOME/vmbox
	cp /dev/null $mybox
	mail -s "Re: Your message" $to < $HOME/.vacation
	if test $check
	then
		/bin/echo -n "Replied to: $to on " >> $HOME/vmbox.log
		date >> $HOME/vmbox.log
	fi
	echo vacation $1 | at 1am	: do it again tomorrow
	exit
else					: vacation over; the boss is back
	echo "Couldn't find $HOME/.vacation."
	exit
fi

-- 
Eric Mazur

ARPA-NET:	mazur@harvard.harvard.EDU
BITNET:		mazur@harvunxh.bitnet
UUCP:							 /-----	mazur
					 		/  /---	dasy!mazur
{seismo,harpo,ihnp4,linus,allegra,ut-sally}!harvard! ------
							\  \___	lasex!mazur
							 \_____	molphy!mazur

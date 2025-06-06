26-Sep-85 03:19:53-MDT,9618;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Thu 26 Sep 85 03:19:41-MDT
Received: from usenet by TGR.BRL.ARPA id a005987; 26 Sep 85 4:45 EDT
From: "c.bamford" <cjb@mtuxo.uucp>
Newsgroups: net.sources
Subject: cd commands that remember where you've been
Message-ID: <989@mtuxo.UUCP>
Date: 23 Sep 85 01:08:48 GMT
To:       unix-sources@BRL-TGR.ARPA

If you spend a lot of time prowling directory trees, the following Korn
shell functions are very handy. They provide a version of the "cd"
command that remembers which directories you've visited since
login. Remembered directories are maintained in a stack and labelled
with small integers; to cd to a directory you've already visited,
it is only necessary to say something like:
	r 3
which makes the 3rd directory in the stack the current working directory,
avoiding the necessity of retyping /horribly/long/directory/names.

These functions were inspired by the Csh pushd/popd/dirs commands, and
go much further than the Ksh "cd -" command. Although they are written
for the Korn shell, I have versions for Csh and /bin/sh if anyone
has trouble with the conversions involved. Overly verbose documentation
(including a little wall chart) appears in the shell file itself.

Cliff Bamford
Consultant to AT&T Lincroft NJ room 113A-3L210 phone (210) 576 2133
... ihnp4!mtuxo!cjb

Note: this is NOT an installation shell script - just put it in a file
------------------cut here--cut here--cut here------------------------
#
# Ksh versions of the directory-stack management functions by bamford 21Sep85
#
# The o,p,q,r,s commands replace cd. They maintain a stack of directories
# visited by you since login. There are commands to visit (cd to) a new
# directory, display the stack, reorder the stack, etc.
#
# To use these functions: copy this text to a file in your home directory
# then insert the following lines in your .profile:
#	unalias r	     # ksh has a hardwired alias for r='fc -e -'
#	alias -x x='fc -e -' # now you can say "x ect=etc g" to fix last grep
#	. <this file name>   # define the functions
#	alias cd=p           # experts prefer alias cd=s
# 
# The q command displays the directory stack. If your userid is foobar, and
# your $HOME directory is /u/foobar, then this is what the q command displays
# immediately after you logon:
#	q
#	0=/u/foobar
#
# This means that the stack consists of a single directory numbered 0.
# Directories on the stack are always numbered for easy reference, directory
# number 0 is ALWAYS your current working directory. Now say you do two
# cds in a row:
#	cd /usr/games
#	0=/usr/games  1=/u/foobar
#	cd /news/spool/net
#	0=/news/spool/net  1=/usr/games  2=/u/foobar
#
# Since cd is aliased to p (the PUSH directory command), your movement
# thru the directory tree causes the stack to grow, always keeping the
# current working directory at the top of the stack (position 0) and 
# pushing previous directories down the stack for possible future reference.
# All the usual magic stuff works:
#	cd ~henry
#	0=/v/henry  1=/news/spool/net  2=/usr/games  3=/u/foobar
#	cd 
#	0=/u/foobar  1=/v/henry  2=/news/spool/net  3=/usr/games
#
# Notice that last cd -- the opqrs commands never put a directory on the
# stack if it's already there (it just moves the requested directory to
# the top of the stack, making it the current working directory).
# The r command is another way to reorder the stack. Without operands 
# it interchanges the top two directories:
#	r
#	0=/v/henry  1=/u/foobar  2=/news/spool/net  3=/usr/games
# 
# You can tell r to reach down further into the stack:
#	r 3
#	0=/usr/games  1=/v/henry  2=/u/foobar  3=/news/spool/net
#	r 3
#	0=/news/spool/net  1=/usr/games  2=/v/henry  3=/u/foobar
#
# The s command SWITCHES (replaces) the directory at the top of
# the stack with the one you name:
#	s ..
#	0=/news/spool  1=/usr/games  2=/v/henry  3=/u/foobar
#	s
#	0=/u/foobar  1=/news/spool  2=/usr/games  3=/v/henry
#
# As shown in the last example, s without an operand means switch me
# to my home directory -- which, in this case was already in the stack
# so it got moved to the 0 (current working directory) position.
# Finally, the o command POPs (removes directories) off the stack:
#	o
#	0=/news/spool  1=/usr/games  3=/v/henry
# 
# You can tell o to keep popping until the nth directory is at the top:
#	o 3
#	0=/v/henry
#
# None of the commands let you create an empty stack:
#	o 99
#	0=/u/foobar
#
# Which is an easy way to clean up the stack completely.
# All of this is much harder to explain than it is to use. The following
# chart really tells the whole story:
#
# command	description
# -------	-----------
# p X		push directory X (default $HOME) onto top of stack
# s X		switch (replace) top of stack with X (default $HOME)
# q		display the stack (no operands allowed)
# r n		reorder - make the nth directory (default 1) top of stack
# o n		pop directories until the nth (default 1) is top of stack
#
#
####################End of Documentation#################################
#
# CDS is the directory stack CDSN is the highest valid index thereto
#
CDS[0]=$HOME
let CDSN=0
export CDS
export CDSN

function tellq		##### describe the q command
{
	print "The q command displays the directory stack (queue)."
	print "It ignores all operands except ?, which produces this note."
}

function q		##### display the queue
{
	if [ $1xx = \?xx ]
	   then tellq
	   return
	fi
	let i=0
	while [ $i -le $CDSN ]
	      do
	      if [ ${CDS[$i]} ]
		 then print -n $i"="${CDS[$i]}"  "
	      fi
	      let i=i+1
	      done
	print -n \\n
}

function oo		##### internal - pop 1 item off queue
{
	if [ $CDSN -eq 0 ]
	   then CDS[0]=$HOME
	   return
	fi
	let i=0
	let j=$CDSN
	while [ $i -le $j ]
	      do
	      let k=$i+1
	      CDS[$i]=${CDS[$k]}
	      let i=$i+1
	      done
	unset CDS[$i]
	unset CDS[$j]
	let CDSN=${CDSN}-1
}

function tello		##### explain how to use o 
{
	print "The o n command pops the stack until the nth directory is top."
	print "If this leaves the stack empty, \$HOME becomes single top."
	print "Usage: o      (pop once)"
	print "   or: o 1    (ditto)"
	print "   or: o n    (pop until the nth directory is at top)" 
	print "   or: o 99   (clean up stack completely)"
}

function o		##### pop n items off queue (default is 1)
{
	if [ $# -lt 1 ] 
	   then oo
	   cd ${CDS[0]}
	   q
	   return
	fi
	if [ $# -gt 1 ]
	   then print "o: too many parms [$*]"
	   return 1
	fi
	if [ $1xx = ?xx ]
	   then tello
           return 1
	fi
	case $1 in
	   [!1-9] ) print "o: parm [$*] must be 1-9"; return 1;;
	esac
	let n=$1
	while [ $n -gt 0 ]
	      do
	      oo
	      let n=$n-1
	      done
	cd ${CDS[0]}
	q	
}

function chekstak	##### if stack contains $1 set $in to its index
{
	let in=0
	while [ $in -le $CDSN  ]
	      do
	      if [ $1 = ${CDS[$in]} ]
                 then return 0
	         fi
              let in=$in+1
	      done
        return 1
}
function rr		##### internal - switch top and CDS[$1], $1 is good
{
	if [ $1 -gt $CDSN ]
	   then q
	   return
	fi
	cd ${CDS[$1]}	##### if cd fails, we abort w/o changing CDS
	rrx=${CDS[0]}
	CDS[0]=${CDS[$1]}
	let rri=$1
	while [ $rri -gt 1 ]
	      do
	      let rrj=$rri-1
	      CDS[$rri]=${CDS[rrj]}
	      let rri=$rri-1
	      done
	CDS[1]=$rrx
	q
}

function r		##### bring CDS[$1] to top of q
{
	if [ $# -lt 1 ]
	   then rr 1
	   return  
	fi
	if [ $# -gt 1 ]
	   then print "r: too many parms [$*]"
	   return 1
	fi
	if [ $1xx = ?xx ]
	   then print "The r n command reorders the stack, bringing"
	        print "the nth directory to the top."
	        print "Usage: r 1  (interchange top and 1st directory)"
	        print "   or: r    (ditto)"
		print "   or: r n  (bring nth directory to top)"
		return
	fi
	if [ $1xx = 0xx ]
	   then q
	   return
	fi
	case $1 in
	     [!1-9] ) print "r: parms [$*] must be 0-9";return 1;;
	esac
	if [ $1 -lt 1 -o $1 -gt $CDSN ]
	   then print "r: parm [$*] out of range - max is $CDSN"
	   return 1
	fi
	rr $1
}

function p		#### push directory x onto stack
{
	if [ $# -gt 1 ]
	   then print "p: too many parms [$*]"
	   return 1
	fi
	if [ $1xx = ?xx ]
	   then print "The p xxx command puts directory xxx on top."
		print "Usage: p /usr/bin  (puts /usr/bin on top)"
		print "   or: p           (go to HOME directory)"
		print "   or: p \~	  (ditto)"
		print "   or: p \`pwd\`     (synch opqrs to real world)"
	   return
	fi
	cd $1		#### if cd fails, exit w/o change
	pd=`pwd`
	if chekstak $pd
	   then r $in
	   return
	fi
	let pi=$CDSN+1
	while [ $pi -gt 0 ]
	      do
	      let pj=$pi-1
	      CDS[$pi]=${CDS[pj]}
	      let pi=$pi-1
	      done
	CDS[0]=$pd
	let CDSN=$CDSN+1
	q
}

function s		#### switch $1 onto top of stack
{
	if [ $# -gt 1 ]
	   then print "s: too may parms [$*]"
	   return 1
	fi
	if [ $1xx = ?xx ]
	   then print "The s xxx command replaces the top of stack with"
		print "directory xxx. A missing operand means HOME."
	        print "Usage: s ..	(cd to the parent of this directory)"
		print "   or: s bin     (go one directory lower)"
	        print "   or: s \~       (go to home directory)"
		print "   or: s         (ditto)"
		return
	fi
	cd $1		#### if cd fails exit without prejudice
	sd=`pwd`
	if chekstak $sd
	   then r $in
	   return
	fi
	CDS[0]=$sd
	q
}

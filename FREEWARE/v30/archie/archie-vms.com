$ write sys$output "Unpacking..."
$ if f$search("archie.dir") .eqs. "" then create/dir [.ARCHIE]
$ if f$search("[.ARCHIE]vms.dir") .eqs. "" then create/dir [.ARCHIE.VMS]
$ if f$search("[.ARCHIE]msdos.dir") .eqs. "" then create/dir [.ARCHIE.MSDOS]
$ set noverify
$! This is a DCL shar-type archive created by Unix dclshar.
$!
$CREATE [.archie]INSTALL
$DECK
[Last changed: 04/14/92 v1.3.2]

 This is the minimal set of things you need to build an Archie client
that takes advantage of the Prospero system.  It is derived from the
4.2beta version of Prospero, currently available as the file prospero.tar.Z
on the host cs.washington.edu.

 * To make the client under Unix:
   * edit the Makefile, changing CC, OPTIONS, LDFLAGS, and RM if
     necessary (odds are you won't have to do this..see below)
   * edit the DEFINEs as necessary
   * if archie.ans.net isn't "close" to you netwise, please edit
     ARCHIE in the Makefile appropriately.  This is particularly important
     if you're not in the US.  You don't need to touch archie.h anymore.
   * type `make'

 I've tried to make this as portable as possible.  I'd encourage you
to try doing a "make" without any changes to the Makefile, and see how
it goes.  If it pukes, and you have to define something, please drop
me a line---my goal is for anybody to just type "make" and not have to
worry about things like what functions their system supports.

 * To make the client under VMS, just type @MAKE and it'll ask you
   which of the available Archie servers is closest.  That's it!

 * To make the client under MSDOS or OS/2:
   * with FTP Software's PC/TCP for MSDOS (version 2.1 or higher) and
     OS/2 (version 1.1 and higher), use makefile.os2 or makefile.dos
   * with CUTCP (NCSA telnet), use makefile.cut

-- Brendan Kehoe (brendan@cs.widener.edu)
$EOD
$!
$CREATE [.archie]Makefile
$DECK
#
# Last changed: 4/14/92, v1.3.2
#
# Makefile for the minimal build for an archie Prospero client.

# Your C compiler:
CC=cc

# For most systems, these OPTIONS will suffice.  Exceptions:
#
#	* If you're on a Stardent, add  -43
#	* If you're running EP/IX, you may need to add  -systype bsd43
#	  but try it without it first.
OPTIONS= -O -I. -I/usr/netinclude -I/usr/ucbinclude

# For this, DEFINES is usually ok as-is.  Try it without any of these
# first; if some stuff fails or shows up undefined, then come back and
# add 'em.  Also please drop me a line if you had to add anything...ideally
# things will reach a point where this whole section will be unnecessary.
#
#     * if you want to include the debugging code (so you
#       can help with problem-solving if any crop up), add    -DDEBUG
#     * if you're running Interactive Unix, add		      -DISC
#     * if you're running System V, add			      -DSYSV
#     * if you're running a USG (System V.2) system, add      -DUSG
#     * if you're running UTS, add			      -DUTS
#     * if your system doesn't have the functions index(),
#       rindex(), bcopy(), or bzero(), add                    -DFUNCS
#     * if your system is missing the getenv(3) routine, add  -DGETENV
#     * if your system doesn't have the re_comp/regcmp or re_exec/regex
#       routines (no regex(3)/regcmp(3X) library), then add   -DNOREGEX
#     * if your system is lacking strspn(), add               -DSTRSPN
DEFINES= -DDEBUG

# The default Archie server; choose one of:
#    archie.ans.net		(USA [NY])
#    archie.rutgers.edu		(USA [NJ])
#    archie.sura.net		(USA [MD])
#    archie.unl.edu		(USA [NE])
#    archie.mcgill.ca		(Canada)
#    archie.funet.fi		(Finland/Mainland Europe)
#    archie.au			(Australia)
#    archie.doc.ic.ac.uk	(Great Britain/Ireland)
#
ARCHIE= archie.sura.net

#	Usually LDFLAGS is empty; if, after you build this, archie
#	 complains that it can't resolve ARCHIE.ANS.NET (or whatever
#	 you defined ARCHIE_HOST as), you need to add `-lresolv'.
#	* If you need the PW library (e.g. A/UX), add -lPW
#	* If you're using ISC, add -linet
#	* If you're using Wollongong TCP/IP on an AT&T box, use the
#	  arguments -lnet -lnsl_s .
#	* If you're using UTS, add -lsocket
#	* If you're using System V Release 4, add -L/usr/ucblib etc.
LDFLAGS=
#LDFLAGS= -lresolv
#LDFLAGS= -lnet -lnsl_s
#LDFLAGS= -lsocket
#LDFLAGS= -L/usr/ucblib -lucb -lsockdns -lnsl

# Change this if necessary.
RM=/bin/rm

# =========================
# Yer' done....make archie.
# =========================
#
CFLAGS=$(OPTIONS) $(DEFINES) -DARCHIE_HOST=\"$(ARCHIE)\"

VERSION=1.3.2
VMS_VERSION=`echo $(VERSION) | sed -e 's,\\.,_,g'`

OBJS=	aquery.o archie.o atalloc.o dirsend.o get_pauth.o get_vdir.o \
	perrmesg.o procquery.o ptalloc.o regex.o stcopy.o support.o \
	vlalloc.o vl_comp.o

all: archie

archie: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): archie.h pmachine.h pcompat.h Makefile

procquery.o: copyright.h

clean:
	$(RM) -f *.o archie archie.doc

gclean: clean
	$(RM) -f *~ archie.0* archie$(VMS_VERSION).com archie.tar* gmon.out archie.doc

unx-deadly:
	unifdef -UXARCHIE xprocquery.c > procquery.c

udp: udp.o
	$(CC) -o $@ udp.o

udptest: udp
	@echo "This should print the date, if UDP's enabled, or hang if not:"
	@./udp

FILES=	INSTALL Makefile Prospero README README.dos archie.c archie.h \
	archie.lnk archie.doc archie.man aquery.c atalloc.c dirsend.c \
	get_pauth.c get_vdir.c make.com makefile.cut makefile.dos makefile.os2 \
	msdos/cutcp.h msdos/hostform.h msdos/netevent.h \
	perrmesg.c patchlevel.h pauthent.h pcompat.h perrno.h pfs.h pmachine.h \
	pprot.h procquery.c ptalloc.c rdgram.h regex.c regex.h stcopy.c \
	support.c copyright.h udp.c vl_comp.c vlalloc.c vms.h vms_support.c \
	vms/fd.h vms/in.h vms/pseudos.h vms/signal.h vms/socket.h vms/time.h \
	vms/types.h vms/network.h vms/multi.opt vms/ucx.opt vms/woll.opt

BFILES= $(FILES) xprocquery.c

dist: tar shar dcl

shar: archie.doc
	makekit -narchie-$(VERSION). archie-$(VERSION) `echo $(FILES) | \
		tr ' ' '\012' | sed -e "s/^/archie-$(VERSION)\//g"` \
		vms msdos > Manifest

archie.doc: archie.man
	nroff -man archie.man | tr '\010' _ | sed -e s/__//g > archie.doc

dcl: archie.doc
	if [ ! -d archie$(VMS_VERSION)/ ]; then ln -s . archie$(VMS_VERSION); fi
	echo '$$ write sys$$output "Unpacking..."' > archie$(VMS_VERSION).com
	echo '$$ if f$$search("archie.dir") .eqs. "" then create/dir [.ARCHIE]' >> archie$(VMS_VERSION).com
	echo '$$ if f$$search("[.ARCHIE]vms.dir") .eqs. "" then create/dir [.ARCHIE.VMS]' >> archie$(VMS_VERSION).com
	echo '$$ if f$$search("[.ARCHIE]msdos.dir") .eqs. "" then create/dir [.ARCHIE.MSDOS]' >> archie$(VMS_VERSION).com
	echo '$$ set noverify' >> archie$(VMS_VERSION).com
	dclshar `echo "$(FILES)" | tr ' ' '\012' | \
		sed -e "s/^/archie\//g"` >> archie$(VMS_VERSION).com
	echo '$$ write sys$$output "Ok, now enter the ARCHIE directory, look at MAKE.COM, then type @MAKE ."' >> archie$(VMS_VERSION).com

tar: archie.doc
	if [ ! -d archie-$(VERSION)/ ]; then ln -s . archie-$(VERSION); fi
	tar cvf archie-$(VERSION).tar `echo $(FILES) |\
	 tr ' ' '\012' | sed -e "s/^/archie-$(VERSION)\//g"`
	compress -f archie-$(VERSION).tar
$EOD
$!
$CREATE [.archie]Prospero
$DECK
Prospero also allows users to access Archie as if it were part of a
file system.  Here is an example of how Prospero can be used to access
Archie in this manner.

  Script started on Mon Jul  1 22:36:42 1991
  % source /home/ftp/archie/pfs/bin/vfsetup.source
  % vfsetup guest
  % venable
  % cd /archive-sites/archie/regex
  % cd prospero (This command specifies the query)
  % ls -l
  total 0
  -r--r--r--   0 -               0 -            info-prospero.arc
  dr-xr-xr-x   0 -               0 -            prospero
  dr-xr-xr-x   0 -               0 -            prospero-papers
  -r--r--r--   0 -               0 -            prospero.arc
  -r--r--r--   0 -               0 -            prospero.tar.Z
  (Note that the "vls" command could have been used)
  (to show where the files were actually stored    )
  % ls prospero (list a result if it is a directory)
  prog.tar.Z      prospero.tar.Z
  % cat info-prospero.arc  (The file is automatically retrieved and displayed)
  >From bcn@n1dmm  Tue Dec  4 02:33:36 1990
  Received: from n1dmm.cs.washington.edu by june.cs.washington.edu (5.64/7.0jh)
          id AA24763; Tue, 4 Dec 90 02:33:36 -0800
  Received: by n1dmm.cs.washington.edu (5.64/7.0h)
          id AA08497; Tue, 4 Dec 90 02:33:31 -0800
  Date: Tue, 4 Dec 90 02:33:31 -0800
  From: bcn@cs.washington.edu (Clifford Neuman)
  ...
  % vdisable
  % exit
  script done on Mon Jul  1 22:39:33 1991
$EOD
$!
$CREATE [.archie]README
$DECK
[Last changed 04/14/92 v1.3.2]

 Enclosed you'll find a Prospero client for the archie service.  It'll
let you query the archie databases without actually using an
interactive process on the remote server's machine (e.g., archie.ans.net),
resulting in a MUCH better response time.  It also helps lessen the
load on the archie server itself.

 What's Archie?  It's a system that will let you check a database containing
thousands of entries for the files that're available at FTP sites around
the world.

 This is a third child of Clifford Neuman's Prospero project.  It's really
the Archie client that's included in the prospero stuff, but I've taken out
everything that's unnecessary for this client to work.  (Aka, you don't
have to build all of Prospero to get the Archie client.)  Khun Yee Fung
wrote an archie client in Perl, George Ferguson has written a client
for use with XWindows, based in part upon this code.  Also, Scott
Stark wrote a NeXT-Step client for the NeXT.

 Using the Archie Prospero interface in its true form will probably be of
interest---check out the file `Prospero' for an example of its interface.
If you find it intriguing, you should probably get the full prospero kit
from the University of Washington on cs.washington.edu in pub/prospero.tar.Z.

 Suffice to say, there are now a number of ways to query Archie without
bogging a server down with your logins.

 Check out the man page (or archie.doc, if you're using VMS or DOS)
for instructions on how to use this archie client.  VMS users please
note that you have to put quotes around args that are capital letters;
for example,  $ ARCHIE "-L"  to list the available servers.

 Please check to make sure you don't have "archie" aliased or modified
in some way to do a telnet or rlogin (which you may've done before
this command-line ability came into being).

 If Archie consistently hangs (at different times of day with
different queries), it's possible that your site has UDP traffic on
ports > 1000 blocked, for security reasons.  Type `make udptest' to
check---if it prints out the date, the Archie server is probably down;
if it doesn't print out the date, either Widener's computer is down
(God forbid ;-) ) or you do indeed have UDP blocked.  See your system
administrator in this case.  If the problem persists and haven't the
vaguest, then write me describing the situation and what machine/OS
you're using.

 Write to archie-group@cs.mcgill.ca with questions about Archie itself.
 Write to info-prospero@isi.edu about the Prospero protocol.
 Write to brendan@cs.widener.edu with questions about this specific package.
$EOD
$!
$CREATE [.archie]README.dos
$DECK
11/22/91  Readme.dos -- Readme file for CUTCP DOS version of Archie.

--
This version requires the use of a `standard' NCSA or CUTCP compatible
CONFIG.TEL file.  If you are running NCSA telnet or CUTCP/CUTE telnet,
you should be able to run this program without modification.

As with all CUTCP programs, you may set a DOS environment variable to
point to the location of your config.tel file.

	set CONFIGTEL=C:\net\config.tel

You may also set the $CUTCPnn dos environment variables to include other
'local' information (such as client IP address on a Novell lan).

This version has been compiled with the US (archie.sura.net) Prospero/Archie
server as the default. This may not be appropriate for your location. Here's
how to change it.


	1. Run the archie program with the -L argument to list known
	   archie/prospero servers.

	2. Select the server name that is closest to your site. For this example
	   we'll pick archie.funet.fi

	3. Edit your config.tel file and add the following lines at the end
	   of the file.

			name=archie
			host=archie.funet.fi	# actually substitute your select
						# server name here

	4. If you happen to know the IP address of the server, you may also
	   add the appropriate

			hostip=<insert IP address here>

	5. If you don't enter an IP address, archie will perform a DNS lookup
	   use the domain nameserver information that is in your config.tel 
	   file.


An additional command line option (-H) has been added (vs -h) to allow the
user to specify the config.tel file location on the command line. 

	archie -H c:\net\config.tel emacs

During a search, you may press the <ESCAPE> key to abort the query.



If you have problems, try running archie with the -D9 option (debug).

As usual, bugs/comments to:

		cutcp-bugs@omnigate.clarkson.edu

Brad Clements, Sr. Network Engineer
Educational Resources Center
Clarkson University 
Potsdam, NY 13699

bkc@draco.erc.clarkson.edu

-----------
Sample Config.tel file 
------------------------------
#  Example host file for ARCHIE/ CUTCP version
#  11/21/91
#
#      Bugs to  cutcp-bugs@omnigate.clarkson.edu

#---------------------------------------------------------------------------#
#									    #
#** ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION ATTENTION **#
#									    #
#   This sample config.tel file contains every single option that you could #
#   possibly use. Its meant as an example only. Please Please PLEASE don't  #
#   use all these options in your working config.tel file, just the ones    #
#   that you actually need. Many options can be left out, the default action#
#   (if defined) is shown in parens () next to the option. 		    #
#									    #
#                         Thank you for your support			    #
#---------------------------------------------------------------------------#

#
#  This example file is for my=bootp, in which case the following items are
#  determined from the bootp server: (and are thus commented out in this file)
#       1. This machine's IP Address
#       2. The network NETMASK
#       3. The default gateway (one or more)
#       4. Nameservers (one or more)
#       5. The domain search list (if this clients hostname is in the bootp packet)
#
#  Your BOOTP server needs to be RFC 1048 compliant for this to work
#  
#  If you have nameservers, gateways or a domainslist already specified
#  in this file, then the file's entry takes precedence.
#  Extra gateways and nameservers will be added by BOOTP if found, however.
#----------------------------------------------------------------------------
#  You can set a DOS environment variable  CONFIGTEL to point to this file
#  then you don't need batch files or the -h option.
#
#   	C> set CONFIGTEL=C:\net\myconfig.tel
#
#  You may also use environment variables to include config.tel options,
#  such as:
#	C> set $CUTCP1=myip~197.001.42.98;netmask~255.255.252.0
#	C> set $CUTCP2=name~x;hostip~128.163.298.2
#
#  and so on up to $CUTCP99. Note that you use a tilde (~) instead of (=)
#  in the dos set command because two ='s are not allowed by Dos.
#
#  Additionally, there is a new config.tel option called include= which
#  allows the nesting (up to 3) configuration files deep.
#
#
#  You can use these new options on networks to make your configuration job
#  easier. CUTCP always opens config.tel files as read-only, so you can mark
#  your files as sharable, read only. Also, you can use the include= command
#  in a $CUTCP environment variable, and on the command line.

#  *Note* that you can not include a machine specific parameter before
#  name=default... This used to work in old versions, but is strictly
#  enforced in this version.
# --------------------------------------------------------------------------  
#  This file is free form
#  Separators are any char <33 and :;=
#
#  The form is keyword=value for each parameter.
#  The first set of parameters refer to the whole program's defaults.
#  These parameter values can be in any order.
#  Following this are the individual machine specs.
#

myip=128.153.28.65                      #  (bootp)
				#  myip types are:
                                #  bootp        - sends out a BOOTP request for your IP
                                #  rarp         - Uses reverse ARp to get your IP
                                #  xx.yy.zz.qq  - your IP address

vjc=no				# (no)
				# set  vjc=yes if you're running slip and
				# you want to use Van Jacobson TCP header
				# compression

splayc=no			# (no) ack. splay compression w/ vjc.. don't
				# use it, not standard, development option only

myname=userid			# put your userid here, used for Rlogin
				# PC-NFS version ignores this value and uses the name
				# that you specified to your pcnfsd.

netmask=255.255.252.0           #  needed if not using BOOTP. 
                                #  otherwise not needed because Bootp gets your netmask
				#  for you

hardware=packet			# (packet)
                                # harware choices
                                #  3com         - 3com 3c501
                                #  3c523        - 3com 3c523
                                #  wd800        - Western Digitial 800E
                                #  nicpc        - 
                                #  nicps        -
                                #  ni5210       - Micom Interlan NI5210 card
                                #  packet       - FTP packet Driver spec 
                                # (currently only Ethernet and Slip class devices are supported)

interrupt=2                     # hardware IRQ interrupt
address=0                       # (0)
				# base memory address or packet driver class
                                # if using packet driver (0 == default == ethernet)
                                # or class=6 for slip

ioaddr=0                        # (0)
				# I/O address  or packet int vector if using packet driver
                                # If = 0 and packet driver, telbin looks for first
                                # packet driver found between 60H and 7FH

noarpme=yes			# Don't arp for myself before running? (no)
				# if yes, inhibits the initial arp request
				# for this IP address. However, if yes, won't
				# warn you if another user is using this IP
				# address. For some token ring systems, you'll
				# need noarpme=yes.

include="nul"			# if you want to include another file as part
				# of this one (you can nest up to three includes)
				# otherwise, don't use this option

domaintime=4			# domain name lookup timeout (first retry)
domainretry=4			# domain name max number of retries 
domainslist="clarkson.edu,aux.clarkson.edu" # domain name search path
                                # domain name lookup will attach each of these
                                # comma seperated suffixes to
                                # the end of the name you are looking for to
                                # discover the full name and IP address.
                                # so, looking for x tries x.clarkson.edu and x.aux.clarkson.edu then just x
                                # unless you use  x.  which ONLY looks for x

	## Also, the fewer suffixes in the domainslist
	## the less time you will have to wait if you mis-type an entry.
	## Finally, try not to use a suffix like   .  or .edu or .com this will
	## waste a lot of time looking for a bogus host name if you mis-type an entry.

## ----------------------------------------------------------------- ##
##   BEGIN MACHINE SPECIFIC PARAMETERS...			     ##
## ----------------------------------------------------------------- ##
 
name=default			# default entry sets defaults for all following
				# machines.

arptime=10			# arp timeout in seconds
				#

retrans=1                       # starting retransmit time out in ticks
				#   1/18ths of sec MAX 100, min 1

mtu=1024                        # maximum transmit unit in bytes
				#   outgoing packet size, MAX=1500

maxseg=1024			# largest segment we can receive
				#   whatever the hardware can take, MAX=4096
rwin=2048                       # most bytes we can receive without ACK
				#   =TCP window size, MAX=4096

contime=20                      # timeout in seconds to try connection
				#   before returning error to user

#  Following are individual machine specifications
#  Gateways are used in order that they appear in the file
#  Nameservers rotate, #1, #2, #3, #1, #2 when a request fails
#

# ********************************************************************* ###
# Note: If you are using BOOTP above for myip=, then you do not need
#       to enter a default gateway, nameserver, netmask etc (unless you want to)
#       only IF your BOOTP server is rfc_1048 compliant. (Ask your Net Manager)
#       You can use both bootp and entries here, in which case the entries in
#       this file over-ride what BOOTP discovers. (however, bootp nameservers
#       and gateways are ADDED to this file's list of entries
# ********************************************************************* ####

#  Below this line, most of the communication parameters are obtained
#  from the "default" host entry.  Those parameters listed for a host 
#  override the default host values.
#
#  These are examples, replace them with the correct values for your site.
#name=mynameserver
#host=omnigate.clarkson.edu
#hostip=197.001.4.2
#nameserver=1

#name=backupserver
#host=clutx.clarkson.edu
#hostip=197.001.4.3
#nameserver=2

#name=lastserver
#host=n2ngw.nyser.net
#hostip=128.145.198.2
#nameserver=3

#name=mygateway
#host=nysernet.clarkson.edu
#hostip=197.001.4.1
#gateway=1


name=archie
host=archie.funet.fi
$EOD
$!
$CREATE [.archie]archie.c
$DECK
/*
 * Copyright (c) 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

/*
 * Archie client using the Prospero protocol.
 *
 * Suggestions and improvements to Brendan Kehoe (brendan@cs.widener.edu).
 */

#include <stdio.h>
#if defined(OS2)
# include <pctcp.h>
#endif
#ifdef MSDOS
# include <string.h>
# include <stdlib.h>
# ifdef CUTCP
#  include <msdos/cutcp.h>
#  include <msdos/hostform.h>
/* The default stack size for a BC program is 4k; jack it up to 16 and add the
   Check for Stack Overflow option to the compiler.  */
extern unsigned _stklen = 16 * 1024;
# endif
#endif

#include <pfs.h>
#include <rdgram.h>
#include <archie.h>
#include <pmachine.h>

int		listflag = 0;
int		sortflag = 0;   /* 1 = by date                    */
char		*progname;
#ifdef DEBUG
extern int	pfs_debug;
#endif
extern int	rdgram_priority;

/* To keep the code clean.. */
#ifdef VMS
# define NFLAG	"\"N\""
# define LFLAG	"\"L\""
# define A_EXIT	SS$_NORMAL
#else
# define NFLAG	"N"
# define LFLAG	"L"
# define A_EXIT	1
#endif
#ifdef CUTCP
# define HFLAG	"[H config.tel]]"
#else
# define HFLAG	"]"
#endif

main(argc,argv)
    int		argc;
    char	*argv[];
    {
	char		*cur_arg;
	char		qtype = '=';    /* Default to exact string match  */
	char		etype = '=';	/* Type if only -e is specified   */
	int		eflag = 0;	/* Exact flag specified		  */
	int		max_hits = MAX_HITS;
	int		offset = 0;
	int		exitflag = 0;	/* Display release identifier     */
	int		tmp;
	char		*host = ARCHIE_HOST;
	char		*p;
	static char *archies[] = { ARCHIES };
#ifdef CUTCP
	int		hostset = 0;
#endif
	char		*getenv();

	progname = *argv;
	argc--; argv++;

#ifdef	CUTCP
	if(getenv("CONFIGTEL"))
		if(Shostfile(getenv("CONFIGTEL")) < 0) {
			fprintf(stderr,"Error, couldn't open configtel file %s\n",
				getenv("CONFIGTEL"));
			exit(1);
		}
#endif

	if ((p = getenv("ARCHIE_HOST")) != (char *)NULL)
	  host = p;
	    
	while (argc > 0 && **argv == '-') {
	    cur_arg = argv[0]+1;

	    /* If a - by itself, or --, then no more arguments */
	    if(!*cur_arg || ((*cur_arg == '-') && (!*(cur_arg+1)))) {
	        argc--, argv++;
		goto scandone;
	    }

	    while (*cur_arg) {
		switch (*cur_arg++) {
#ifdef DEBUG		
		case 'D':  /* Debug level */
		    pfs_debug = 1; /* Default debug level */
		    if(*cur_arg && index("0123456789",*cur_arg)) {
			sscanf(cur_arg,"%d",&pfs_debug);
			cur_arg += strspn(cur_arg,"0123456789");
		    }
		    else if(argc > 2) {
		        tmp = sscanf(argv[1],"%d",&pfs_debug);
			if (tmp == 1) {argc--;argv++;}
		    }
		    break;
#endif
#ifdef	CUTCP
		case 'H' :
			if(Shostfile(argv[1]) < 0) {
				fprintf(stderr,"Error, couldn't open configtel file %s\n",argv[1]);
				exit(1);
			}
		 	argc--;argv++;
			break;
#endif
#ifndef XARCHIE
		case 'L':
		    printf("Known archie servers:\n");
		    for (tmp = 0; tmp < NARCHIES; tmp++)
			printf("\t%s\n", archies[tmp]);
		    printf(" * %s is the default Archie server.\n", ARCHIE_HOST);
		    printf(" * For the most up-to-date list, write to an Archie server and give it\n   the command `servers'.\n");
		    exitflag = 1;
		    break;
#endif

		case 'N':  /* Priority (nice) */
		    rdgram_priority = RDGRAM_MAX_PRI; /* Use this if no # */
		    if(*cur_arg && index("-0123456789",*cur_arg)) {
			sscanf(cur_arg,"%d",&rdgram_priority);
			cur_arg += strspn(cur_arg,"-0123456789");
		    }
		    else if(argc > 2) {
		        tmp = sscanf(argv[1],"%d",&rdgram_priority);
			if (tmp == 1) {argc--;argv++;}
		    }
		    if(rdgram_priority > RDGRAM_MAX_SPRI) 
			rdgram_priority = RDGRAM_MAX_PRI;
		    if(rdgram_priority < RDGRAM_MIN_PRI) 
			rdgram_priority = RDGRAM_MIN_PRI;
  		    break;

		case 'c':  /* substring (case sensitive) */
		    qtype = 'C';
		    etype = 'c';
		    break;

		case 'e':  /* Exact match */
		    /* If -e specified by itself, then we use the  */
		    /* default value of etype which must be '='    */
		    eflag++;
		    break;

		case 'h':  /* Host */
		    host = argv[1];
#ifdef CUTCP
		    hostset++;
#endif
		    argc--; argv++;
		    break;

		case 'l':  /* List one match per line */
		    listflag++;
		    break;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		    cur_arg--;
		case 'm':  /* Max hits */
		    max_hits = -1;  
		    if(*cur_arg && index("0123456789",*cur_arg)) {
			sscanf(cur_arg,"%d",&max_hits);
			cur_arg += strspn(cur_arg,"0123456789");
		    }
		    else if(argc > 1) {
		        tmp = sscanf(argv[1],"%d",&max_hits);
			if (tmp == 1) {argc--;argv++;}
		    }
		    if (max_hits < 1) {
			fprintf(stderr, "%s: -m option requires a value for max hits (>= 1)\n",
				progname);
			exit(A_EXIT);
		    }
		    break;

		case 'o':  /* Offset */
		    if(argc > 1) {
		      tmp = sscanf(argv[1],"%d",&offset);
		      if (tmp != 1)
			argc = -1;
		      else {
			argc--; argv++;
		      }
		    }
		    break;

		case 'r':  /* Regular expression search */
		    qtype = 'R';
		    etype = 'r';
		    break;

		case 's':  /* substring (case insensitive) */
		    qtype = 'S';
		    etype = 's';
		    break;

		case 't':  /* Sort inverted by date */
		    sortflag = 1;
		    break;

		case 'v':  /* Display version */
		    fprintf(stderr,
			"Client version %s based upon Prospero version %s\n",
			    CLIENT_VERSION, PFS_RELEASE);
		    exitflag++;
		    break;

		default:
		    fprintf(stderr,"Usage: %s [-[cers][l][t][m#][h host][%s][%s#]%s string\n", progname, LFLAG, NFLAG, HFLAG);
		    exit(A_EXIT);
		}
	    }
	    argc--; argv++;
	}

      scandone:

	if (eflag) qtype = etype;

	if ((argc != 1) && exitflag) exit(0);

	if (argc != 1) {
	    fprintf(stderr, "Usage: %s [-[cers][l][t][m#][h host][%s][%s#]%s string\n", progname, LFLAG, NFLAG, HFLAG);
	    fprintf(stderr,"       -c : case sensitive substring search\n");
	    fprintf(stderr,"       -e : exact string match (default)\n");
	    fprintf(stderr,"       -r : regular expression search\n");
	    fprintf(stderr,"       -s : case insensitive substring search\n");
	    fprintf(stderr,"       -l : list one match per line\n");
	    fprintf(stderr,"       -t : sort inverted by date\n");
	    fprintf(stderr,"      -m# : specifies maximum number of hits to return (default %d)\n", max_hits);
	    fprintf(stderr,"  -h host : specifies server host\n");
	    fprintf(stderr,"       -%s : list known servers and current default\n", LFLAG);
	    fprintf(stderr,"      -%s# : specifies query niceness level (0-35765)\n", NFLAG);
#ifdef CUTCP
	    fprintf(stderr,"-H config.tel: specify location of config.tel file\n");
#endif
	    exit(A_EXIT);
	}

#ifdef	CUTCP
	if(argc = Snetinit()) {
	       	fprintf(stderr,"Error %d from SNetinit (bad or missing config.tel ?)\n",argc);
		if(argc == -2)
		       	netshut();	/* rarp lookup failure */
		exit(1);
	}
	if(!hostset) {		/* if no host on command line, look in config.tel file
	       			   for name=archie */
		struct machinfo *mp;

	 	mp = Shostlook("archie");
		if(mp) {
		       	host = mp->hname ? mp->hname : mp->sname;
		}
       }
#endif

	procquery(host, argv[0], max_hits, offset, qtype, sortflag, listflag);

#ifdef CUTCP
	netshut();
#endif
	exit(0);
    }
$EOD
$!
$CREATE [.archie]archie.h
$DECK
/*
 * archie.h : Definitions for the programmatic Prospero interface to Archie
 *
 *     Written by Brendan Kehoe (brendan@cs.widener.edu), 
 *                George Ferguson (ferguson@cs.rochester.edu), and
 *                Clifford Neuman (bcn@isi.edu).
 */

/* You can't touch this.  */
#ifndef XARCHIE
# define ARCHIES	"archie.ans.net (USA [NY])","archie.rutgers.edu (USA [NJ])","archie.sura.net (USA [MD])","archie.unl.edu (USA [NE])","archie.mcgill.ca (Canada)","archie.funet.fi (Finland/Mainland Europe)","archie.au (Australia)","archie.doc.ic.ac.uk (Great Britain/Ireland)"
# define NARCHIES	8
#endif

/*
 * Default value for max hits.  Note that this is normally different
 * for different client implementations.  Doing so makes it easier to
 * collect statistics on the use of the various clients.
 */
#ifdef VMS
# define	MAX_HITS	98	/* VMS Client */
#else
# ifdef XARCHIE
#  define	MAX_HITS	99	/* X Client */
# else
#  define	MAX_HITS	95	/* Normal client */
# endif
#endif

/*
 * CLIENT_VERSION may be used to identify the version of the client if 
 * distributed separately from the Prospero distribution.  The version
 * command should then identify both the client version and the Prospero
 * version identifiers.   
 */
#ifdef XARCHIE
# define CLIENT_VERSION	"1.3-X"
#else
# define CLIENT_VERSION "1.3.2"
#endif

/* Procedures from user/aquery.c */

/* archie_query(host,string,max_hits,offset,query_type,cmp_proc,flags) */
extern VLINK archie_query(); 

/* defcmplink(p,q) and invdatecmplink(p,q)                             */
extern int defcmplink();	/* Compare by host then by filename    */
extern int invdatecmplink();	/* Compare links inverted by date      */

/* Definitions for the comparison procedures                           */
#define AQ_DEFCMP	defcmplink
#define AQ_INVDATECMP	invdatecmplink

/* Flags                                                               */
#define AQ_NOSORT	0x01	/* Don't sort                          */
#define AQ_NOTRANS	0x02	/* Don't translate Archie responses    */
$EOD
$!
$CREATE [.archie]archie.lnk
$DECK
aquery.lo archie.lo atalloc.lo dirsend.lo+
Get_pauth.lo get_vdir.lo perrmesg.lo procquery.lo+
ptalloc.lo regex.lo stcopy.lo support.lo+
vlalloc.lo vl_comp.lo
archie.unp
archie/map/noi/co/li/stack:45000
lsocket lnetlib lconfig lpc llibce
$EOD
$!
$CREATE [.archie]archie.doc
$DECK



ARCHIE(1)                USER COMMANDS                  ARCHIE(1)



NAME
     archie - query the  Archie  anonymous  FTP  databases  using
     Prospero

SYNOPSIS
     archie [ -cers ] [ -l ] [ -t ] [ -m# ] [ -N# ] [ -h hostname ]
            [ -L ] string

DESCRIPTION
     archie queries an archie anonymous FTP database looking  for
     the  specified  string  using  the  Prospero protocol.  This
     client is based on Prospero version Beta.4.2 and is provided
     to  encourage non-interactive use of the Archie servers (and
     subsequently better performance on both  sides).   This  man
     page describes version 1.3 of the client.

     The general method of use is of the form

          % archie string

     This will go to the archie server and ask it to look for all
     known  systems  that have a file named `string' in their FTP
     area.  archie will wait, and print out any matches.

     For example,

          % archie emacs

     will find all anonymous FTP sites  in  the  archie  database
     that  have  files  named  emacs somewhere in their FTP area.
     (This particular query would probably return a lot of direc-
     tories.)  If you want a list of every filename that contains
     emacs anywhere in it, you'd use

          % archie -c emacs

     Regular expressions, such as

          % archie -r '[xX][lL]isp'

     may also be used for searches.  (See the manual of a reason-
     ably good editor, like GNU Emacs or vi, for more information
     on using regular expressions.)


OPTIONS
     The options currently available to this archie client are:

     -c          Search substrings paying attention  to  upper  &
                 lower case.
     -e          Exact string match.  (This is the default.)
     -r          Search using a regular expression.



Archie (Prospero)  Last change: 9 January 1992                  1






ARCHIE(1)                USER COMMANDS                  ARCHIE(1)



     -s          Search  substrings  ignoring  the  case  of  the
                 letters.
     -l          Output results in a form suitable for parsing by
                 programs.
     -t          Sort the results inverted by date.
     -m#         Specifies the maximum number of  hits  (matches)
                 to return (default of 95).
     -N#         Sets the niceness of a query; by  default,  it's
                 set  to 0.  Without an argument, ``-N'' defaults
                 to 35765.   If  you  use  -N  with  an  argument
                 between 0 and 35765, it'll adjust itself accord-
                 ingly.  (Note: VMS users will have to put quotes
                 around  this  argument, and -L, like "-N45"; VMS
                 will otherwise convert it to lowercase.)
     -h hostname Tells the client  to  query  the  Archie  server
                 hostname.
     -L          Lists the Archie servers known  to  the  program
                 when it was compiled, as well as the name of the
                 default Archie server.  For an up-to-date  list,
                 write  to  ``archie@archie.mcgill.ca''  (or  any
                 Archie  server)  with  the  single  command   of
                 servers.

     The three search-modifying arguments  (``-c'',  ``-r'',  and
     ``-s'')  are  all  mutually  exclusive;  only  the  last one
     counts.  If you specify -e with any of  ``-c'',  ``-r'',  or
     ``-s'', the server will first check for an exact match, then
     fall back to the case-sensitive, case-insensitive, or  regu-
     lar expression search.  This is so if there are matches that
     are particularly obvious, it will take a minimal  amount  of
     time to satisfy your request.

     If you list a single `-' by itself,  any  further  arguments
     will  be  taken  as  part  of  the  search  string.  This is
     intended to enable searching for strings that begin  with  a
     `-'; for example:

          % archie -s - -old

     will search for all filenames that contain the string `-old'
     in them.

RESPONSE
     Archie servers are set up to respond to a number of requests
     in  a  queued fashion.  That is, smaller requests get served
     much more quickly than do large requests.  As a result,  the
     more  often  you query the Archie server, or the larger your
     requests, the longer the queue will become, resulting  in  a
     longer  waiting  period  for everyone's requests.  Please be
     frugal when possible, for your benefit as well  as  for  the
     other users.




Archie (Prospero)  Last change: 9 January 1992                  2






ARCHIE(1)                USER COMMANDS                  ARCHIE(1)



QUERY PRIORITY
     Please use the  ``-N''  option  whenever  you  don't  demand
     immediacy,  or when you're requesting things that could gen-
     erate large responses.  Even when using the nice option, you
     should  still  try  to  avoid  big jobs during busy periods.
     Here is a list of what we consider to be  nice  values  that
     accurately reflect the priority of a job to the server.

          Normal              0
          Nice                500
          Nicer               1000
          Very Nice           5000
          Extremely Nice      10000
          Nicest              32765

     The last priority, Nicest, would be used when a  job  should
     wait  until  the  queue is essentially empty before running.
     You should pick one of these values to use, possibly modify-
     ing  it  slightly depending on where you think your priority
     should land.  For example, 32760 would mean wait  until  the
     queue  is  empty,  but  jump  ahead  of other jobs that have
     selected Nicest.

     There are certain types of  things  that  we  suggest  using
     Nicest  for,  irregardless.  In particular, any searches for
     which you would have a hard time justifying the use of  any-
     thing but extra resources.  (We all know what those searches
     would be for.)

ENVIRONMENT
     ARCHIE_HOST
             This will change the host archie will  consult  when
             making  queries.   (The default value is what's been
             compiled in.)  The ``-h'' option will override this.
             If  you're  running  VMS,  create  a  symbol  called
             ARCHIE_HOST.

SEE ALSO
     For more information on regular expressions, see the  manual
     pages on:

     regex(3), ed(1)

     Also   read    the    file    archie/doc/whatis.archie    on
     archie.mcgill.ca for a detailed paper on Archie as a whole.

AUTHORS
     The archie service was conceived  and  implemented  by  Alan
     Emtage       (bajan@cs.mcgill.ca),       Peter       Deutsch
     (peterd@cs.mcgill.ca),        and        Bill         Heelan
     (wheelan@cs.mcgill.ca).   The  entire  Internet  is in their
     debt.



Archie (Prospero)  Last change: 9 January 1992                  3






ARCHIE(1)                USER COMMANDS                  ARCHIE(1)



     The  Prospero  system  was  created   by   Clifford   Neuman
     (bcn@isi.edu);   write  to  info-prospero@isi.edu  for  more
     information on the protocol and its use.

     This stripped client  was  put  together  by  Brendan  Kehoe
     (brendan@cs.widener.edu),  with  modifications  by  Clifford
     Neuman and George Ferguson (ferguson@cs.rochester.edu).

BUGS
     There are none; only a few unexpected features.













































Archie (Prospero)  Last change: 9 January 1992                  4



$EOD
$!
$CREATE [.archie]archie.man
$DECK
.\" Originally by Jeff Kellem (composer@chem.bu.edu).
.\"
.\" This is from rn (1):
.de Ip
.br
.ie \\n.$>=3 .ne \\$3
.el .ne 3
.IP "\\$1" \\$2
..
.\"
.TH ARCHIE 1 "9 January 1992" "Archie (Prospero)"
.SH NAME
archie \- query the Archie anonymous FTP databases using Prospero
.SH SYNOPSIS
.in +\w'\fBarchie \fR'u
.ti -\w'\fBarchie \fR'u
.B archie\
\ [\ \fB\-cers\fR\ ]\
\ [\ \fB\-l\fR\ ]\ [\ \fB\-t\fR\ ]\
\ [\ \fB\-m#\fR\ ]\ [\ \fB\-N#\fR\ ]\
\ [\ \fB\-h\fR\ \fIhostname\fR\ ]
[\ \fB\-L\fR\ ]\ \fIstring\fR
.SH DESCRIPTION
.B archie
queries an archie anonymous FTP database looking for the specified
.I string
using the
.B Prospero
protocol.  This client is based on
.B Prospero
version Beta.4.2 and is provided to encourage non-interactive use of
the Archie servers (and subsequently better performance on both
sides).  This man page describes version 1.3 of the client.

The general method of use is of the form

.RS
%
.B archie string
.RE
.PP

This will go to the archie server and ask it to look for all known
systems that have a file named `string' in their FTP area.  \fBarchie\fP
will wait, and print out any matches.

For example,

.RS
%
.B archie emacs
.RE
.PP

will find all anonymous FTP sites in the archie database that have files
named
.B emacs
somewhere in their FTP area.  (This particular query would probably
return a lot of directories.)  If you want a list of every filename
that contains \fBemacs\fR \fIanywhere\fR in it, you'd use

.RS
%
.B archie -c emacs
.RE
.PP

Regular expressions, such as

.RS
%
.B archie -r '[xX][lL]isp'
.RE
.PP

may also be used for searches.  (See the manual of a reasonably good
editor, like GNU Emacs or vi, for more information on using regular
expressions.)

.SH OPTIONS
The options currently available to this
.B archie
client are:

.PD 0
.TP 12
.BR \-c
Search substrings paying attention to upper & lower case.
.TP
.BR \-e
Exact string match.  (This is the default.)
.TP
.BR \-r
Search using a regular expression.
.TP
.BR \-s
Search substrings ignoring the case of the letters.
.TP
.BR \-l
Output results in a form suitable for parsing by programs.
.TP
.BR \-t
Sort the results inverted by date.
.TP
.BI \-m#
Specifies the maximum number of hits (matches) to return (default of 
\fB95\fR).
.TP
.BI \-N#
Sets the \fIniceness\fR of a query; by default, it's set to 0.
Without an argument, ``\-N'' defaults to \fB35765\fR.  If you use
\fB\-N\fR with an argument between 0 and 35765, it'll adjust itself
accordingly.  (\fBNote\fR: VMS users will have to put quotes around
this argument, and \fB\-L\fR, like "\fB\-N45\fR"; VMS will otherwise convert
it to lowercase.)
.TP
.BI \-h\ \fIhostname\fR
Tells the client to query the Archie server \fIhostname\fR.
.TP
.BI \-L
Lists the Archie servers known to the program when it was compiled, as
well as the name of the default Archie server.  For an up-to-date
list, write to ``archie@archie.mcgill.ca'' (or any Archie server) with
the single command of \fIservers\fR.

.PP
The three search-modifying arguments (``\-c'', ``\-r'', and ``\-s'')
are all mutually exclusive; only the last one counts.  If you specify
\fB\-e\fR with any of ``\-c'', ``\-r'', or ``\-s'',
the server will first check for an exact match, then fall back to the
case-sensitive, case-insensitive, or regular expression search.  This is
so if there are matches that are particularly obvious, it will take a
minimal amount of time to satisfy your request.

If you list a single `\-' by itself, any further arguments will be
taken as part of the search string.  This is intended to enable
searching for strings that begin with a `\-'; for example:

.RS
%
.B archie \-s \- \-old
.RE

will search for all filenames that contain the string `\-old' in them.

.SH RESPONSE
Archie servers are set up to respond to a number of requests in a
queued fashion.  That is, smaller requests get served much more
quickly than do large requests.  As a result, the more often you query
the Archie server, or the larger your requests, the longer the queue
will become, resulting in a longer waiting period for everyone's
requests.  Please be frugal when possible, for your benefit as well as
for the other users.

.SH QUERY PRIORITY
Please use the ``-N'' option whenever you don't demand immediacy, or
when you're requesting things that could generate large responses.
Even when using the nice option, you should still try to avoid big
jobs during busy periods.  Here is a list of what we consider to be
nice values that accurately reflect the priority of a job to the server.

.RS
.TP 20
.B Normal
0
.TP
.B Nice
500
.TP
.B Nicer
1000
.TP
.B Very Nice
5000
.TP
.B Extremely Nice
10000
.TP
.B Nicest
32765
.RE

The last priority, \fBNicest\fR, would be used when a job should wait until
the queue is essentially empty before running.  You should pick one of
these values to use, possibly modifying it slightly depending on where
you think your priority should land.  For example, 32760 would mean
wait until the queue is empty, but jump ahead of other jobs that have
selected \fBNicest\fR.

There are certain types of things that we suggest using \fBNicest\fR
for, irregardless.  In particular, any searches for which you would
have a hard time justifying the use of anything but extra resources.
(We all know what those searches would be for.)

.SH ENVIRONMENT
.Ip "ARCHIE_HOST" 8
This will change the host
.IR archie
will consult when making queries.  (The default value is what's been
compiled in.)  The ``\-h'' option will override this.  If you're
running VMS, create a symbol called ARCHIE_HOST.

.SH SEE ALSO
For more information on regular expressions, see the manual pages on:

.BR regex (3) ,
.BR ed (1)

Also read the file \fBarchie/doc/whatis.archie\fR on
\fBarchie.mcgill.ca\fR for a detailed paper on Archie as a whole.

.SH AUTHORS
The 
.B archie
service was conceived and implemented by Alan Emtage (\fBbajan@cs.mcgill.ca\fR),
Peter Deutsch (\fBpeterd@cs.mcgill.ca\fR), and Bill Heelan
(\fBwheelan@cs.mcgill.ca\fR).  The entire Internet is in their debt.

The \fBProspero\fR system was created by Clifford Neuman
(\fBbcn@isi.edu\fR); write to \fBinfo\-prospero@isi.edu\fR for more
information on the protocol and its use.

This stripped client was put together by Brendan Kehoe
(\fBbrendan@cs.widener.edu\fR), with modifications by
Clifford Neuman and George Ferguson (\fBferguson@cs.rochester.edu\fR).

.SH BUGS
There are none; only a few unexpected features.

$EOD
$!
$CREATE [.archie]aquery.c
$DECK
/*
 * aquery.c : Programmatic Prospero interface to Archie
 *
 * Copyright (c) 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 * Originally part of the Prospero Archie client by Clifford 
 * Neuman (bcn@isi.edu).  Modifications, addition of programmatic interface,
 * and new sorting code by George Ferguson (ferguson@cs.rochester.edu) 
 * and Brendan Kehoe (brendan@cs.widener.edu).  MSDOS and OS2 modifications
 * to use with PC/TCP by Mark Towfiq (towfiq@FTP.COM).
 */

#include <copyright.h>

#include <stdio.h>

#include <pfs.h>
#include <perrno.h>
#include <archie.h>

#include <pmachine.h>
#ifdef NEED_STRING_H
# include <string.h>			/* for char *index() */
#else
# include <strings.h>			/* for char *index() */
#endif

static void translateArchieResponse();

extern int pwarn;
extern char p_warn_string[];

/*
 * archie_query : Sends a request to _host_, telling it to search for
 *                _string_ using _query_type_ as the search method.
 *                No more than _max_hits_ matches are to be returned
 *                skipping over _offset_ matches.
 *
 *		  archie_query returns a linked list of virtual links. 
 *                If _flags_ does not include AQ_NOTRANS, then the Archie
 *                responses will be translated. If _flags_ does not include 
 *                AQ_NOSORT, then the list will be sorted using _cmp_proc_ to
 *                compare pairs of links.  If _cmp_proc_ is NULL or AQ_DEFCMP,
 *                then the default comparison procedure, defcmplink(), is used
 *                sorting by host, then filename. If cmp_proc is AQ_INVDATECMP
 *                then invdatecmplink() is used, sorting inverted by date.
 *                otherwise a user-defined comparison procedure is called.
 *
 *                archie_query returns NULL and sets perrno if the query
 *                failed. Note that it can return NULL with perrno == PSUCCESS
 *                if the query didn't fail but there were simply no matches.
 *
 *   query_type:  S  Substring search ignoring case   
 *                C  Substring search with case significant
 *                R  Regular expression search
 *                =  Exact String Match
 *            s,c,e  Tries exact match first and falls back to S, C, or R 
 *                   if not found.
 *
 *     cmp_proc:  AQ_DEFCMP      Sort by host, then filename
 *                AQ_INVDATECMP  Sort inverted by date
 *
 *        flags:  AQ_NOSORT      Don't sort results
 *                AQ_NOTRANS     Don't translate results
 */
VLINK 
archie_query(host,string,max_hits,offset,query_type,cmp_proc,flags)
    char	*host,*string;
    int		max_hits,offset;
    char	query_type;
    int		(*cmp_proc)();
    int		flags;
    {
	char qstring[MAX_VPATH];    /* For construting the query  */
	VLINK	links;		/* Matches returned by server */
	VDIR_ST	dir_st;         /* Filled in by get_vdir      */
	PVDIR	dir= &dir_st;
	
	VLINK	p,q,r,lowest,nextp,pnext,pprev;
	int	tmp;

	/* Set the cmp_proc if not given */
	if (cmp_proc == NULL) cmp_proc = defcmplink;

	/* Make the query string */
	sprintf(qstring,"ARCHIE/MATCH(%d,%d,%c)/%s",
		max_hits,offset,query_type,string);

	/* Initialize Prospero structures */
	perrno = PSUCCESS; *p_err_string = '\0';
	pwarn = PNOWARN; *p_warn_string = '\0';
	vdir_init(dir);
	
	/* Retrieve the list of matches, return error if there was one */
#if defined(MSDOS)
	if(tmp = get_vdir(host, qstring, "", dir, (long)GVD_ATTRIB|GVD_NOSORT,
		NULL, NULL)) {
#else
	if(tmp = get_vdir(host,qstring,"",dir,GVD_ATTRIB|GVD_NOSORT,NULL,NULL)) {
# endif
	    perrno = tmp;
	    return(NULL);
	}

	/* Save the links, and clear in dir in case it's used again   */
	links = dir->links; dir->links = NULL;

	/* As returned, list is sorted by suffix, and conflicting     */
	/* suffixes appear on a list of "replicas".  We want to       */
	/* create a one-dimensional list sorted by host then filename */
	/* and maybe by some other parameter                          */

	/* First flatten the doubly-linked list */
	for (p = links; p != NULL; p = nextp) {
	    nextp = p->next;
	    if (p->replicas != NULL) {
		p->next = p->replicas;
		p->next->previous = p;
		for (r = p->replicas; r->next != NULL; r = r->next)
		    /*EMPTY*/ ;
		r->next = nextp;
		nextp->previous = r;
		p->replicas = NULL;
	    }
	}

	/* Translate the filenames unless NOTRANS was given */
	if (!(flags & AQ_NOTRANS))
	    for (p = links; p != NULL; p = p->next)
		translateArchieResponse(p);

	/* If NOSORT given, then just hand it back */
	if (flags & AQ_NOSORT) {
	    perrno = PSUCCESS;
	    return(links);
	}

	/* Otherwise sort it using a selection sort and the given cmp_proc */
	for (p = links; p != NULL; p = nextp) {
	    nextp = p->next;
	    lowest = p;
	    for (q = p->next; q != NULL; q = q->next)
		if ((*cmp_proc)(q,lowest) < 0)
		    lowest = q;
	    if (p != lowest) {
		/* swap the two links */
		pnext = p->next;
		pprev = p->previous;
		if (lowest->next != NULL)
		    lowest->next->previous = p;
		p->next = lowest->next;
		if (nextp == lowest) {
		    p->previous = lowest;
		} else {
		    lowest->previous->next = p;
		    p->previous = lowest->previous;
		}
		if (nextp == lowest) {
		    lowest->next = p;
		} else {
		    pnext->previous = lowest;
		    lowest->next = pnext;
		}
		if (pprev != NULL)
		    pprev->next = lowest;
		lowest->previous = pprev;
		/* keep the head of the list in the right place */
		if (links == p)
		    links = lowest;
	    }
	}

	/* Return the links */
	perrno = PSUCCESS;
	return(links);
    }

/*
 * translateArchieResponse: 
 *
 *   If the given link is for an archie-pseudo directory, fix it. 
 *   This is called unless AQ_NOTRANS was given to archie_query().
 */
static void
translateArchieResponse(l)
    VLINK l;
    {
	char *slash;

	if (strcmp(l->type,"DIRECTORY") == 0) {
	    if (strncmp(l->filename,"ARCHIE/HOST",11) == 0) {
		l->type = stcopyr("EXTERNAL(AFTP,DIRECTORY)",l->type);
		l->host = stcopyr(l->filename+12,l->host);
		slash = (char *)index(l->host,'/');
		if (slash) {
		    l->filename = stcopyr(slash,l->filename);
		    *slash++ = '\0';
		} else
		    l->filename = stcopyr("",l->filename);
	    }
	}
    }

/*
 * defcmplink: The default link comparison function for sorting. Compares
 *	       links p and q first by host then by filename. Returns < 0 if p
 *             belongs before q, > 0 if p belongs after q, and == 0 if their
 *             host and filename fields are identical.
 */
int
defcmplink(p,q)
    VLINK p,q;
    {
	int result;

	if ((result=strcmp(p->host,q->host)) != 0)
	    return(result);
	else
	    return(strcmp(p->filename,q->filename));
    }

/*
 * invdatecmplink: An alternative comparison function for sorting that
 *	           compares links p and q first by LAST-MODIFIED date,
 *                 if they both have that attribute. If both links
 *                 don't have that attribute or the dates are the
 *                 same, it then calls defcmplink() and returns its 
 *		   value.
 */
int
invdatecmplink(p,q)
    VLINK p,q;
    {
	PATTRIB pat,qat;
	char *pdate,*qdate;
	int result;
	
	pdate = qdate = NULL;
	for (pat = p->lattrib; pat; pat = pat->next)
	    if(strcmp(pat->aname,"LAST-MODIFIED") == 0)
		pdate = pat->value.ascii;
	for (qat = q->lattrib; qat; qat = qat->next)
	    if(strcmp(qat->aname,"LAST-MODIFIED") == 0)
		qdate = qat->value.ascii;
	if(!pdate && !qdate) return(defcmplink(p,q));
	if(!pdate) return(1); 
	if(!qdate) return(-1);
	if((result=strcmp(qdate,pdate)) == 0) return(defcmplink(p,q));
	else return(result);
    }
$EOD
$!
$CREATE [.archie]atalloc.c
$DECK
/*
 * Copyright (c) 1989, 1990 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <stdio.h>

#include <pfs.h>
#include <pmachine.h> /* for correct definition of ZERO */

static PATTRIB	lfree = NULL;
int		pattrib_count = 0;
int		pattrib_max = 0;

/*
 * atalloc - allocate and initialize vlink structure
 *
 *    ATALLOC returns a pointer to an initialized structure of type
 *    PATTRIB.  If it is unable to allocate such a structure, it
 *    returns NULL.
 */
PATTRIB
atalloc()
    {
	PATTRIB	at;
	if(lfree) {
	    at = lfree;
	    lfree = lfree->next;
	}
	else {
	    at = (PATTRIB) malloc(sizeof(PATTRIB_ST));
	    if (!at) return(NULL);
	    pattrib_max++;
	}

	pattrib_count++;

	ZERO(at);
	/* Initialize and fill in default values; all items are
	   0 [or NULL] save precedence */
	at->precedence = ATR_PREC_OBJECT;

	return(at);
    }

/*
 * atfree - free a PATTRIB structure
 *
 *    ATFREE takes a pointer to a PATTRRIB structure and adds it to
 *    the free list for later reuse.
 */
void
atfree(at)
    PATTRIB	at;
    {
	if(at->aname) stfree(at->aname);

	if((strcmp(at->avtype,"ASCII") == 0) && at->value.ascii) 
	    stfree(at->value.ascii);
	if((strcmp(at->avtype,"LINK") == 0) && at->value.link) 
	    vlfree(at->value.link);
	
	if(at->avtype) stfree(at->avtype);

	at->next = lfree;
	at->previous = NULL;
	lfree = at;
	pattrib_count--;
    }

/*
 * atlfree - free a PATTRIB structure
 *
 *    ATLFREE takes a pointer to a PATTRIB structure frees it and any linked
 *    PATTRIB structures.  It is used to free an entrie list of PATTRIB
 *    structures.
 */
void
atlfree(at)
    PATTRIB	at;
    {
	PATTRIB	nxt;

	while(at != NULL) {
	    nxt = at->next;
	    atfree(at);
	    at = nxt;
	}
    }

$EOD
$!
$CREATE [.archie]dirsend.c
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

/* If you're going to hack on this, I'd suggest using unifdef with -UCUTCP
   and possibly -UVMS, for your working copy.  When you've got your changes
   done, come back and add them into this main file.  It's getting pretty
   nasty down there.  */

#include <copyright.h>
#include <stdio.h>
#include <errno.h>

#ifdef VMS
# ifdef WOLLONGONG
#  include "twg$tcp:[netdist.include]netdb.h"
# else /* not Wollongong */
#  ifdef UCX
#   include netdb
#  else /* Multinet */
#   include "multinet_root:[multinet.include]netdb.h"
#  endif
# endif
# include <vms.h>
#else /* not VMS */
# include <sys/types.h> /* this may/will define FD_SET etc */
# ifdef u3b2
#  include <sys/inet.h> /* THIS does FD_SET etc on AT&T 3b2s.  */
# endif /* u3b2 */
# include <pmachine.h>
# if defined(NEED_TIME_H) && !defined(AUX)
#  include <time.h>
# else
#  include <sys/time.h>
# endif
# ifdef WANT_BOTH_TIME
#  include <sys/time.h>
# endif
# ifdef NEED_STRING_H
#  include <string.h>
# else
#  include <strings.h>
# endif
# ifdef CUTCP
#  include <msdos/cutcp.h>
#  include <msdos/netevent.h>
#  include <msdos/hostform.h>
# else /* not CUTCP */
#  include <netdb.h>
#  include <sys/socket.h>
# endif
# ifdef NEED_SELECT_H
#  include <sys/select.h>
# endif /* NEED_SELECT_H */
# ifndef IN_H
#  include <netinet/in.h>
#  define IN_H
# endif
# ifndef hpux
#  include <arpa/inet.h>
# endif
#endif /* !VMS */

/* Interactive UNIX keeps some of the socket definitions in funny places.  */
#ifdef ISC
# include <net/errno.h>
#endif /* ISC */

#include <pfs.h>
#include <pprot.h>
#include <pcompat.h>
#include <perrno.h>

/* Gnu C currently fails to pass structures on Sparcs properly.  This directly
   effects the calling of inet_ntoa().  To get around it, we use this hack;
   take the address of what's being called to inet_ntoa, so it gets it
   properly.  This won't be necessary with gcc 2.0.  */
#if defined(sun) && defined(__GNUC__) && !defined(__GNU_LIBRARY__)
# define SUN_GNU_FIX &
#else
# define SUN_GNU_FIX
#endif

static int notprived = 0;
#ifndef MSDOS
extern int errno;
#endif
extern int perrno;
extern int rdgram_priority;
#ifdef DEBUG
extern int pfs_debug;
#endif
extern int pfs_disable_flag;

char	*nlsindex();

#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

static int		dir_udp_port = 0;	/* Remote UDP port number */

#ifdef CUTCP
# define	NS_TIMEOUT	15
#endif

static unsigned short	next_conn_id = 0;

static int client_dirsrv_timeout = CLIENT_DIRSRV_TIMEOUT;
static int client_dirsrv_retry = CLIENT_DIRSRV_RETRY; 

/* These were parameters to dirsend() */
static PTEXT pkt;
static char *hostname;
static struct sockaddr_in *hostaddr;

/* These were locals in dirsend(). Note that the initializations here
 * are really meaningless since we have to redo them for each call to
 * dirsend() since they were formerly automatically initialized.
 */
static PTEXT		first = NULL;	/* First returned packet	 */
static PTEXT		next;		/* The one we are waiting for 	 */
static PTEXT		vtmp;           /* For reorganizing linked list  */
static PTEXT		comp_thru;	/* We have all packets though    */
static int		lp = -1;	/* Opened UDP port	         */
static int		hdr_len;	/* Header Length                 */
static int		nd_pkts;	/* Number of packets we want     */
static int		no_pkts;	/* Number of packets we have     */
static int		pkt_cid;        /* Packet connection identifier  */
static unsigned short	this_conn_id;	/* Connection ID we are using    */
static unsigned short	recvd_thru;	/* Received through              */
static short		priority;	/* Priority for request          */
static short		one = 0;	/* Pointer to value 1            */
static short		zero = 0;	/* Pointer to value 0		 */
static char		*seqtxt;	/* Pointer to text w/ sequence # */
static struct sockaddr_in  us;		/* Our address                   */
static struct sockaddr_in  to;		/* Address to send query	 */
static struct sockaddr_in  from;	/* Reply received from		 */
static int		from_sz;	/* Size of from structure	 */
static struct hostent	*host;		/* Host info from gethostbyname  */
static long		newhostaddr;    /* New host address from *host   */
static int		req_udp_port=0; /* Requested port (optional)     */
static char		*openparen;	/* Delimits port in name         */
static char		hostnoport[500];/* Host name without port        */
static int		ns;		/* Number of bytes actually sent */
static int		nr;		/* Number of bytes received      */
static SELECTARG	readfds;	/* Used for select		 */
static int		tmp;
static char		*ctlptr;	/* Pointer to control field      */
static short		stmp;		/* Temp short for conversions    */
static int		backoff;	/* Server requested backoff      */
static unsigned char	rdflag11;	/* First byte of flags (bit vect)*/
static unsigned char	rdflag12;	/* Second byte of flags (int)    */
static int		scpflag = 0;	/* Set if any sequencd cont pkts */
static int		ackpend = 0;    /* Acknowledgement pending      */
static int		gaps = 0;	/* Gaps present in recvd pkts   */
static struct timeval	timeout;	/* Time to wait for response    */
static struct timeval	ackwait;	/* Time to wait before acking   */
static struct timeval	gapwait;	/* Time to wait b4 filling gaps */
static struct timeval	*selwait;	/* Time to wait for select      */
static int		retries;	/* was = client_dirsrv_retry    */
char   to_hostname[512];		/* lmjm: saves inet_ntoa() str  */

/* These are added so dirsend() "blocks" properly */
static PTEXT dirsendReturn;
static int dirsendDone;

/* And here are the values for dirsendDone */
#define DSRET_DONE		1
#define DSRET_SEND_ERROR	-1
#define DSRET_RECV_ERROR	-2
#define DSRET_SELECT_ERROR	-3
#define DSRET_TIMEOUT		-4
#define DSRET_ABORTED		-5

/* New procedures to break up dirsend() */
static int initDirsend();
static void retryDirsend(), keepWaitingDirsend();
static void timeoutProc();
static void readProc();

/* Wrappers around X calls to allow non-X usage */
static void processEvent();

/* Extra stuff for the asynchronous X version of dirsend() */
typedef char *XtPointer;
typedef char *XtInputId;
typedef char *XtIntervalId;

static XtInputId inputId;
static XtIntervalId timerId = (XtIntervalId)0;

/*
 * dirsend - send packet and receive response
 *
 *   DIRSEND takes a pointer to a structure of type PTEXT, a hostname,
 *   and a pointer to a host address.  It then sends the supplied
 *   packet off to the directory server on the specified host.  If
 *   hostaddr points to a valid address, that address is used.  Otherwise,
 *   the hostname is looked up to obtain the address.  If hostaddr is a
 *   non-null pointer to a 0 address, then the address will be replaced
 *   with that found in the hostname lookup.
 *
 *   DIRSEND will wait for a response and retry an appropriate
 *   number of times as defined by timeout and retries (both static
 *   variables).  It will collect however many packets form the reply, and
 *   return them in a structure (or structures) of type PTEXT.
 *
 *   DIRSEND will free the packet that it is presented as an argument.
 *   The packet is freed even if dirsend fails.
 */
PTEXT
dirsend(pkt_p,hostname_p,hostaddr_p)
    PTEXT pkt_p;
    char *hostname_p;
    struct sockaddr_in	*hostaddr_p;
{
    /* copy parameters to globals since other routines use them */
    pkt = pkt_p;
    hostname = hostname_p;
    hostaddr = hostaddr_p;
    /* Do the initializations of formerly auto variables */
    first = NULL;
    lp = -1;
    one = 0;
    zero = 0;
    req_udp_port=0;
    scpflag = 0;
    ackpend = 0;
    gaps = 0;
    retries = client_dirsrv_retry;

    if (initDirsend() < 0)
	return(NULL);

    /* set the first timeout */
    retryDirsend();

    dirsendReturn = NULL;
    dirsendDone = 0;
    /* Until one of the callbacks says to return, keep processing events */
    while (!dirsendDone)
	processEvent();

    /* Return whatever we're supposed to */
    return(dirsendReturn);
}


/*	-	-	-	-	-	-	-	-	*/
/* This function does all the initialization that used to be done at the
 * start of dirsend(), including opening the socket descriptor "lp". It
 * returns the descriptor if successful, otherwise -1 to indicate that
 * dirsend() should return NULL immediately.
 */
static int
initDirsend()
{
    if(one == 0) one = htons((short) 1);

    priority = htons(rdgram_priority);

    timeout.tv_sec = client_dirsrv_timeout;
    timeout.tv_usec = 0;

    ackwait.tv_sec = 0;
    ackwait.tv_usec = 500000;

    gapwait.tv_sec = (client_dirsrv_timeout < 5 ? client_dirsrv_timeout : 5);
    gapwait.tv_usec = 0;

    comp_thru = NULL;
    perrno = 0;
    nd_pkts = 0;
    no_pkts = 0;
    pkt_cid = 0;

    /* Find first connection ID */
    if(next_conn_id == 0) {
	srand(getpid()+time(0)); /* XXX: arg ok, but not right type. */
	next_conn_id = rand();
    }


    /* If necessary, find out what udp port to send to */
    if (dir_udp_port == 0) {
        register struct servent *sp;
	tmp = pfs_enable; pfs_enable = PMAP_DISABLE;
#ifdef USE_ASSIGNED_PORT
	/* UCX needs 0 & -1 */
        if ((sp = getservbyname("prospero","udp")) <= 0) {
#ifdef DEBUG
	    if (pfs_debug)
		fprintf(stderr, "dirsrv: udp/prospero unknown service - using %d\n", 
			PROSPERO_PORT);
#endif
	    dir_udp_port = htons((u_short) PROSPERO_PORT);
        }
#else
	/* UCX needs 0 & -1 */
        sp = getservbyname("dirsrv","udp");
	if (sp == (struct servent *)0 || sp == (struct servent *)-1) {
#ifdef DEBUG
	    if (pfs_debug)
		fprintf(stderr, "dirsrv: udp/dirsrv unknown service - using %d\n", 
			DIRSRV_PORT);
#endif
	    dir_udp_port = htons((u_short) DIRSRV_PORT);
        }
#endif
	else dir_udp_port = sp->s_port;
	pfs_enable = tmp;
#ifdef DEBUG
        if (pfs_debug > 3)
            fprintf(stderr,"dir_udp_port is %d\n", ntohs(dir_udp_port));
#endif
    }

    /* If we were given the host address, then use it.  Otherwise  */
    /* lookup the hostname.  If we were passed a host address of   */
    /* 0, we must lookup the host name, then replace the old value */
    if(!hostaddr || (hostaddr->sin_addr.s_addr == 0)) {
	/* I we have a null host name, return an error */
	if((hostname == NULL) || (*hostname == '\0')) {
#ifdef DEBUG
            if (pfs_debug)
                fprintf(stderr, "dirsrv: Null hostname specified\n");
#endif
	    perrno = DIRSEND_BAD_HOSTNAME;
	    ptlfree(pkt);
            /* return(NULL); */
	    return(-1);
	}
	/* If a port is included, save it away */
	if(openparen = index(hostname,'(')) {
	    sscanf(openparen+1,"%d",&req_udp_port);
	    strncpy(hostnoport,hostname,400);
	    if((openparen - hostname) < 400) {
		*(hostnoport + (openparen - hostname)) = '\0';
		hostname = hostnoport;
	    }
	}
	tmp = pfs_enable; pfs_enable = PMAP_DISABLE;
	if((host = gethostbyname(hostname)) == NULL) {
	    pfs_enable = tmp;
	    /* Check if a numeric address */
	    newhostaddr = inet_addr(hostname);
	    if(newhostaddr == -1) {
#ifdef DEBUG
		if (pfs_debug)
		  fprintf(stderr, "dirsrv: Can't resolve host %s\n",hostname);
#endif
		perrno = DIRSEND_BAD_HOSTNAME;
		ptlfree(pkt);
		/* return(NULL); */
		return(-1);
	    }
	    bzero((char *)&to, S_AD_SZ);
	    to.sin_family = AF_INET;
	    bcopy((char *) &newhostaddr, (char *)&to.sin_addr, 4);
	    if(hostaddr) bcopy(&to,hostaddr, S_AD_SZ);
	}
	else {
	    pfs_enable = tmp;
	    bzero((char *)&to, S_AD_SZ);
	    to.sin_family = host->h_addrtype;
#ifdef CUTCP
	    bcopy((char *) &host->h_addr, (char *)&to.sin_addr, host->h_length);
#else
	    bcopy(host->h_addr, (char *)&to.sin_addr, host->h_length);
#endif
	    if(hostaddr) bcopy(&to,hostaddr, S_AD_SZ);
	}
    }
    else bcopy(hostaddr,&to, S_AD_SZ);
    /* lmjm: Save away the hostname */
    strncpy(to_hostname,
	    inet_ntoa(SUN_GNU_FIX *(struct in_addr *)&to.sin_addr),
	    sizeof(to_hostname)-1);

    if(req_udp_port) to.sin_port = htons(req_udp_port);
    else to.sin_port = dir_udp_port;

    /* If a port was specified in hostaddr, use it, otherwise fill it in */
    if(hostaddr) {
	if(hostaddr->sin_port) to.sin_port = hostaddr->sin_port;
	else hostaddr->sin_port = to.sin_port;
    }

#ifndef CUTCP
    /* Must open a new port each time. we do not want to see old */
    /* responses to messages we are done with                    */
    if ((lp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
#ifdef DEBUG
        if (pfs_debug)
            fprintf(stderr,"dirsrv: Can't open socket\n");
#endif
	perrno = DIRSEND_UDP_CANT;
	ptlfree(pkt);
        /* return(NULL); */
	return(-1);
    }
#endif /* not CUTCP */

    /* Try to bind it to a privileged port - loop through candidate */
    /* ports trying to bind.  If failed, that's OK, we will let the */
    /* system assign a non-privileged port later                    */
#ifndef CUTCP
    if(!notprived) {
	for(tmp = PROS_FIRST_PRIVP; tmp < PROS_FIRST_PRIVP+PROS_NUM_PRIVP; 
	    tmp++) {
#endif
	    bzero((char *)&us, sizeof(us));
	    us.sin_family = AF_INET;
#ifndef CUTCP
	    us.sin_port = htons((u_short) tmp);
	    if (bind(lp, (struct sockaddr *)&us, sizeof(us))) {
		if(errno != EADDRINUSE) {
		    notprived++;
		    break;
		}
	    }
	    else break;
	}
    }
#else
    us.sin_port = htons(PROS_FIRST_PRIVP);
    netulisten(PROS_FIRST_PRIVP);
#endif

#ifndef USE_V3_PROT
    /* Add header */
    if(rdgram_priority) {
	pkt->start -= 15;
	pkt->length += 15;
	*(pkt->start) = (char) 15;
	bzero(pkt->start+9,4);
	*(pkt->start+11) = 0x02;
	bcopy(&priority,pkt->start+13,2);
    }
    else {
	pkt->start -= 9;
	pkt->length += 9;
	*(pkt->start) = (char) 9;
    }
    this_conn_id = htons(next_conn_id++);
    if(next_conn_id == 0) next_conn_id++;
    bcopy(&this_conn_id,pkt->start+1,2);
    bcopy(&one,pkt->start+3,2);
    bcopy(&one,pkt->start+5,2);
    bzero(pkt->start+7,2);
#endif

#ifdef DEBUG
    if (pfs_debug > 2) {
#ifndef USE_V3_PROT
        if (to.sin_family == AF_INET) {
	    if(req_udp_port) 
		fprintf(stderr,"Sending message to %s+%d(%d)...",
			to_hostname, req_udp_port, ntohs(this_conn_id));
	    else fprintf(stderr,"Sending message to %s(%d)...",
			 to_hostname, ntohs(this_conn_id));
	}
#else
        if (to.sin_family == AF_INET) 
	    fprintf(stderr,"Sending message to %s...", to_hostname);
#endif /* USE_V3_PROT */
        else
            fprintf(stderr,"Sending message...");
        (void) fflush(stderr);
    }
#endif /* DEBUG */

    first = ptalloc();
    next = first;

#ifndef CUTCP
    return(lp);
#else
    return(1);
#endif /* CUTCP */
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * This used to be a label to goto to retry the last packet. Now we resend
 * the packet and call keepWaitingDirsend() to wait for a reply. (We
 * call keepWaitingDirsend() because formerly the code dropped through
 * the keep_waiting label.)
 */
static void
retryDirsend()
{
#ifdef CUTCP
    int lretry = 3;
#endif
    gaps = ackpend = 0;

#ifndef CUTCP
    ns = sendto(lp,(char *)(pkt->start), pkt->length, 0, (struct sockaddr *)&to, S_AD_SZ);
#else
    while(--lretry) {
	    ns = netusend(&to.sin_addr,ntohs(to.sin_port),ntohs(us.sin_port),
			  (char *) pkt->start, pkt->length);
	    if(!ns)
		break;
	    Stask();
	    Stask();
	    Stask();
    }
#endif /* CUTCP */

#ifndef CUTCP
    if(ns != pkt->length) {
#else
    if(ns != 0) {
#endif
#ifdef DEBUG
	if (pfs_debug) {
    fprintf(stderr,"\nsent only %d/%d: ",ns, pkt->length);
	    perror("");
	}
#endif
	close(lp);
	perrno = DIRSEND_NOT_ALL_SENT;
	ptlfree(first);
	ptlfree(pkt);
	/* return(NULL); */
	dirsendReturn = NULL;
	dirsendDone = DSRET_SEND_ERROR;
    }
#ifdef DEBUG
    if (pfs_debug > 2) fprintf(stderr,"Sent.\n");
#endif
    keepWaitingDirsend();
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * This used to be a label to goto to set the appropriate timeout value
 * and blocked in select(). Now we set selwait and the SELECTARGs to the
 * appropriate values, and in X register a new timeout, then return to
 * allow event processing.
 */
static void
keepWaitingDirsend()
{
    /* We come back to this point (by a goto) if the packet */
    /* received is only part of the response, or if the     */
    /* response came from the wrong host		    */

#ifdef DEBUG
    if (pfs_debug > 2) fprintf(stderr,"Waiting for reply...");
#endif

#ifndef CUTCP
    FD_ZERO(&readfds);
    FD_SET(lp, &readfds);
#endif

    if(ackpend) selwait = &ackwait;
    else if(gaps) selwait = &gapwait;
    else selwait = &timeout;
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * This routine is called when a timeout occurs. It includes the code that
 * was formerly used when select() returned 0 (indicating a timeout).
 */
/*ARGSUSED*/
static void
timeoutProc(client_data,id)
XtPointer client_data;
XtIntervalId *id;
{
    if (gaps || ackpend) { /* Send acknowledgment */
	/* Acks are piggybacked on retries - If we have received */
	/* an ack from the server, then the packet sent is only  */
	/* an ack and the rest of the message will be empty      */
#ifdef DEBUG
	if (pfs_debug > 2) {
            fprintf(stderr,"Acknowledging (%s).\n",
		    (ackpend ? "requested" : "gaps"));
	}	
#endif
	retryDirsend();
	return;
    }

    if (retries-- > 0) {
	timeout.tv_sec = CLIENT_DIRSRV_BACKOFF(timeout.tv_sec);
#ifdef DEBUG
	if (pfs_debug > 2) {
            fprintf(stderr,"Timed out.  Setting timeout to %d seconds.\n",
		    timeout.tv_sec);
	}
#endif
	retryDirsend();
	return;
    }

#ifdef DEBUG
    if (pfs_debug) {
	fprintf(stderr, "select failed(timeoutProc): readfds=%x ",
		readfds);
	perror("");
    }
#endif
#ifndef CUTCP
    close(lp);
#endif
    perrno = DIRSEND_SELECT_FAILED;
    ptlfree(first);
    ptlfree(pkt);
    /* return(NULL); */
    dirsendReturn = NULL;
    dirsendDone = DSRET_TIMEOUT;
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * This function is called whenever there's something to read on the
 * connection. It includes the code that was run when select() returned
 * greater than 0 (indicating read ready).
 */
/*ARGSUSED*/
static void
readProc(client_data,source,id)
XtPointer client_data;
int *source;
XtInputId *id;
{
#ifdef CUTCP
    int lretry = 3;
#endif

    from_sz = sizeof(from);
    next->start = next->dat;

#ifndef CUTCP
    if ((nr = recvfrom(lp, next->start, sizeof(next->dat), 0, (struct sockaddr *)&from, &from_sz)) < 0) {
#else
    nr = neturead(next->start);
    if (nr < 1) {
#endif
#ifdef DEBUG
        if (pfs_debug) perror("recvfrom");
#endif
#ifndef CUTCP
	close(lp);
#endif
	perrno = DIRSEND_BAD_RECV;
	ptlfree(first);
	ptlfree(pkt);
	/* return(NULL) */
	dirsendReturn = NULL;
	dirsendDone = DSRET_RECV_ERROR;
        return;
    }

    next->length = nr;
    next->start[next->length] = 0;

#ifdef DEBUG
    if (pfs_debug > 2)
        fprintf(stderr,"Received packet from %s\n",
		inet_ntoa(SUN_GNU_FIX *(struct in_addr *)&from.sin_addr));
#endif


    /* For the current format, if the first byte is less than             */
    /* 20, then the first two bits are a version number and the next six  */
    /* are the header length (including the first byte).                  */
    if((hdr_len = (unsigned char) *(next->start)) < 20) {
	ctlptr = next->start + 1;
	next->seq = 0;
	if(hdr_len >= 3) { 	/* Connection ID */
	    bcopy(ctlptr,&stmp,2);
	    if(stmp) pkt_cid = ntohs(stmp);
	    ctlptr += 2;
	}
	if(pkt_cid && this_conn_id && (pkt_cid != ntohs(this_conn_id))) {
	    /* The packet is not for us */
	    /* goto keep_waiting; */
	    keepWaitingDirsend();
	    return;
	}
	if(hdr_len >= 5) {	/* Packet number */
	    bcopy(ctlptr,&stmp,2);
	    next->seq = ntohs(stmp);
	    ctlptr += 2;
	}
	else { /* No packet number specified, so this is the only one */
	    next->seq = 1;
	    nd_pkts = 1;
	}
	if(hdr_len >= 7) {	    /* Total number of packets */
	    bcopy(ctlptr,&stmp,2);  /* 0 means don't know      */
	    if(stmp) nd_pkts = ntohs(stmp);
	    ctlptr += 2;
	}
	if(hdr_len >= 9) {	/* Receievd through */
	    bcopy(ctlptr,&stmp,2);  /* 1 means received request */
#ifndef USE_V3_PROT
	    if((stmp) && (ntohs(stmp) == 1)) {
		/* Future retries will be acks only */
		pkt->length = 9;
		bcopy(&zero,pkt->start+3,2);
#ifdef DEBUG
		if(pfs_debug > 2) 
		    fprintf(stderr,"Server acked request - retries will be acks only\n");
#endif
	    }
#endif
	    ctlptr += 2;
	}
	if(hdr_len >= 11) {	/* Backoff */
	    bcopy(ctlptr,&stmp,2);
	    if(stmp) {
		backoff = ntohs(stmp);
#ifdef DEBUG
		if(pfs_debug > 2) 
		    fprintf(stderr,"Backing off to %d seconds\n", backoff);
#endif
		timeout.tv_sec = backoff;
		if ((backoff > 60) && (first == next) && (no_pkts == 0)) {
		    /* Probably a long queue on the server - don't give up */
		    retries = client_dirsrv_retry;
		}
	    }
	    ctlptr += 2;
	}
	if(hdr_len >= 12) {	/* Flags (1st byte) */
	    bcopy(ctlptr,&rdflag11,1);
	    if(rdflag11 & 0x80) {
#ifdef DEBUG
		if(pfs_debug > 2) 
		    fprintf(stderr,"Ack requested\n");
#endif
		ackpend++;
	    }
	    if(rdflag11 & 0x40) {
#ifdef DEBUG
		if(pfs_debug > 2) 
		    fprintf(stderr,"Sequenced control packet\n");
#endif
		next->length = -1;
		scpflag++;
	    }
	    ctlptr += 1;
	}
	if(hdr_len >= 13) {	/* Flags (2nd byte) */
	    /* Reserved for future use */
	    bcopy(ctlptr,&rdflag12,1);
	    ctlptr += 1;
	}
	if(next->seq == 0) {
	    /* goto keep_waiting; */
	    keepWaitingDirsend();
	    return;
	}
	if(next->length >= 0) next->length -= hdr_len;
	next->start += hdr_len;
	goto done_old;
    }

    pkt_cid = 0;

    /* if intermediate format (between old and new), then process */
    /* and go to done_old                                         */
    ctlptr = next->start + max(0,next->length-20);
    while(*ctlptr) ctlptr++;
    /* Control fields start after the terminating null */
    ctlptr++;
    /* Until old version are gone, must be 4 extra bytes minimum */
    /* When no version 3 servers, can remove the -4              */
    if(ctlptr < (next->start + next->length - 4)) {
	/* Connection ID */
	bcopy(ctlptr,&stmp,2);
	if(stmp) pkt_cid = ntohs(stmp);
	ctlptr += 2;
	if(pkt_cid && this_conn_id && (pkt_cid != ntohs(this_conn_id))) {
	    /* The packet is not for us */
	    /* goto keep_waiting; */
	    keepWaitingDirsend();
	    return;
	}
	/* Packet number */
	if(ctlptr < (next->start + next->length)) {
	    bcopy(ctlptr,&stmp,2);
	    next->seq = ntohs(stmp);
	    ctlptr += 2;
	}
	/* Total number of packets */
	if(ctlptr < (next->start + next->length)) {
	    bcopy(ctlptr,&stmp,2);
	    if(stmp) nd_pkts = ntohs(stmp);
	    ctlptr += 2;
	}
	/* Receievd through */
	if(ctlptr < (next->start + next->length)) {
	    /* Not supported by clients */
	    ctlptr += 2;
	}
	/* Backoff */
	if(ctlptr < (next->start + next->length)) {
	    bcopy(ctlptr,&stmp,2);
	    backoff = ntohs(stmp);
#ifdef DEBUG
	    if(pfs_debug > 2) 
		fprintf(stderr,"Backing off to %d seconds\n", backoff);
#endif
	    if(backoff) timeout.tv_sec = backoff;
	    ctlptr += 2;
	}
	if(next->seq == 0) {
	    /* goto keep_waiting; */
	    keepWaitingDirsend();
	    return;
	}
	goto done_old;

    }

    /* Notes that we have to start searching 11 bytes before the    */
    /* expected start of the MULTI-PACKET line because the message  */
    /* might include up to 10 bytes of data after the trailing null */
    /* The order of those bytes is two bytes each for Connection ID */
    /* Packet-no, of, Received-through, Backoff                     */
    seqtxt = nlsindex(next->start + max(0,next->length - 40),"MULTI-PACKET"); 
    if(seqtxt) seqtxt+= 13;

    if((nd_pkts == 0) && (no_pkts == 0) && (seqtxt == NULL)) goto all_done;

    tmp = sscanf(seqtxt,"%d OF %d", &(next->seq), &nd_pkts);
#ifdef DEBUG    
    if (pfs_debug && (tmp == 0)) 
	fprintf(stderr,"Cant read packet sequence number: %s", seqtxt);    
#endif
 done_old:
#ifdef DEBUG
    if(pfs_debug > 2) fprintf(stderr,"Packet %d of %d\n",next->seq,nd_pkts);
#endif
    if ((first == next) && (no_pkts == 0)) {
	if(first->seq == 1) {
	    comp_thru = first;
	    /* If only one packet, then return it */
	    if(nd_pkts == 1) goto all_done;
	}
	else gaps++;
	no_pkts = 1;
	next = ptalloc();
	/* goto keep_waiting; */
	keepWaitingDirsend();
	return;
    }
	
    if(comp_thru && (next->seq <= comp_thru->seq))
	ptfree(next);
    else if (next->seq < first->seq) {
	vtmp = first;
	first = next;
	first->next = vtmp;
	first->previous = NULL;
	vtmp->previous = first;
	if(first->seq == 1) comp_thru = first;
	no_pkts++;
    }
    else {
	vtmp = (comp_thru ? comp_thru : first);
	while (vtmp->seq < next->seq) {
	    if(vtmp->next == NULL) {
		vtmp->next = next;
		next->previous = vtmp;
		next->next = NULL;
		no_pkts++;
		goto ins_done;
	    }
	    vtmp = vtmp->next;
	}
	if(vtmp->seq == next->seq)
	    ptfree(next);
	else {
	    vtmp->previous->next = next;
	    next->previous = vtmp->previous;
	    next->next = vtmp;
	    vtmp->previous = next;
	    no_pkts++;
	}
    }   

ins_done:
	
    while(comp_thru && comp_thru->next && 
	  (comp_thru->next->seq == (comp_thru->seq + 1))) {
	comp_thru = comp_thru->next;
#ifndef USE_V3_PROT
	recvd_thru = htons(comp_thru->seq);
	bcopy(&recvd_thru,pkt->start+7,2); /* Let server know we got it */
#endif
	/* We've made progress, so reset retry count */
	retries = client_dirsrv_retry;
	/* Also, next retry will be only an acknowledgement */
	/* but for now, we can't fill in the ack field      */
#ifdef DEBUG
	if(pfs_debug > 2) 
	    fprintf(stderr,"Packets now received through %d\n",comp_thru->seq);
#endif
    }

    /* See if there are any gaps */
    if(!comp_thru || comp_thru->next) gaps++;
    else gaps = 0;

    if ((nd_pkts == 0) || (no_pkts < nd_pkts)) {
	next = ptalloc();
	/* goto keep_waiting; */
	keepWaitingDirsend();
	return;
    }

 all_done:
    if(ackpend) { /* Send acknowledgement if requested */
#ifdef DEBUG
	if (pfs_debug > 2) {
	    if (to.sin_family == AF_INET)
		fprintf(stderr,"Acknowledging final packet to %s(%d)\n",
			to_hostname, ntohs(this_conn_id));
            else
                fprintf(stderr,"Acknowledging final packet\n");
	    (void) fflush(stderr);
	}
#endif
#ifndef CUTCP
	ns = sendto(lp,(char *)(pkt->start), pkt->length, 0, (struct sockaddr *)&to, S_AD_SZ);
#else
	while(--lretry) {
	    ns = netusend(&to.sin_addr, ntohs(to.sin_port), ntohs(us.sin_port),(char *) pkt->start, pkt->length);
		if(!ns)
			break;
		Stask();
		Stask();
	}
#endif

#ifndef CUTCP
	if(ns != pkt->length) {
#else
	if(ns != 0) {
#endif

#ifdef DEBUG
	    if (pfs_debug) {
		fprintf(stderr,"\nsent only %d/%d: ",ns, pkt->length);
		perror("");
	    }
#endif
	}

    }
#ifndef CUTCP
    close(lp);
#endif
    ptlfree(pkt);

    /* Get rid of any sequenced control packets */
    if(scpflag) {
	while(first && (first->length < 0)) {
	    vtmp = first;
	    first = first->next;
	    if(first) first->previous = NULL;
	    ptfree(vtmp);
	}
	vtmp = first;
	while(vtmp && vtmp->next) {
	    if(vtmp->next->length < 0) {
		if(vtmp->next->next) {
		    vtmp->next = vtmp->next->next;
		    ptfree(vtmp->next->previous);
		    vtmp->next->previous = vtmp;
		}
		else {
		    ptfree(vtmp->next);
		    vtmp->next = NULL;
		}
	    }
	    vtmp = vtmp->next;
	}
    }

    /* return(first); */
    dirsendReturn = first;
    dirsendDone = DSRET_DONE;

}

static void
processEvent()
{
#ifdef CUTCP
    unsigned long now;
#endif
    /* select - either recv is ready, or timeout */
    /* see if timeout or error or wrong descriptor */
#ifndef CUTCP
    tmp = select(lp + 1, &readfds, (SELECTARG *)0, (SELECTARG *)0, selwait);
    if (tmp == 0) {
	timeoutProc(NULL,&timerId);
    } else if ((tmp < 0) || !FD_ISSET(lp,&readfds)) {
#ifdef DEBUG
	if (pfs_debug) {
	    fprintf(stderr, "select failed(processEvent): readfds=%x ",
		    readfds);
	    perror("");
	}
#endif
	close(lp);
#else /* CUTCP's flood. */
	/* while not timeout in selwait loop, stask looking for uevents */
	now = time(NULL) + selwait->tv_sec;
#ifdef	DEBUG
 	if(pfs_debug) {
		fprintf(stderr,"Waiting %d seconds\n",selwait->tv_sec);
	}

#endif
	while(now > time(NULL)) {
		int	i, cl, dat;

		Stask();
		if (0 < (i = Sgetevent(USERCLASS, &cl, &dat))) {
			/* got a user class event */
			if(cl == USERCLASS &&
				i == UDPDATA) {
					readProc(NULL,&lp,&inputId);
					return;
			}
		}
		if(kbhit()) {
			int c = getch();
			if(c == 27 || c == 3)
				break;
			fprintf(stderr,"Press <ESCAPE> to abort\n");
		}
	}
	if(now <= time(NULL)) {	/* timeout */
		timeoutProc(NULL,&timerId);
		 return;
	}

#endif /* CUTCP */
	perrno = DIRSEND_SELECT_FAILED;
	ptlfree(first);
	ptlfree(pkt);
	/* return(NULL); */
	dirsendReturn = NULL;
	dirsendDone = DSRET_SELECT_ERROR;
#ifndef CUTCP
    } else {
	readProc(NULL,&lp,&inputId);
    }
#endif /* CUTCP */
}

void
abortDirsend()
{
    if (!dirsendDone) {
#ifndef CUTCP
	close(lp);
#endif
	ptlfree(first);
	ptlfree(pkt);
	dirsendReturn = NULL;
	dirsendDone = DSRET_ABORTED;
    }
    return;
}
$EOD
$!
$CREATE [.archie]get_pauth.c
$DECK
/*
 * Copyright (c) 1989, 1990 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <stdio.h>
#ifndef VMS
# include <sys/types.h> /* this may/will define FD_SET etc */
# include <pmachine.h>
#endif

#ifdef NEED_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#ifndef VMS
# if defined(MSDOS) && !defined(OS2)
#  ifndef CUTCP
#   include <rwconf.h>
#  endif
# else
#  include <pwd.h>
# endif
#else
# include <jpidef.h>
# include <vms.h>
#endif

#include <pcompat.h>
#include <pauthent.h>

PAUTH
get_pauth(type)
    int		type;
    {
	static PAUTH_ST   no_auth_st;
	static PAUTH		  no_auth = NULL;
#if !defined(VMS) && !defined(MSDOS) || defined(OS2)
	struct passwd *whoiampw;
#else
	char username[13];
	unsigned short usernamelen;
	struct {
	    unsigned short buflen;
	    unsigned short itmcod;
	    char *bufadr;
	    unsigned short *retlenadr;
	    unsigned long null;
	} jpi_itemlist;
#endif

	if(no_auth == NULL) {
	    no_auth = &no_auth_st;
	    strcpy(no_auth->auth_type,"UNAUTHENTICATED");

	    /* find out who we are */
#ifndef VMS
#if defined(MSDOS) && !defined(OS2)
#ifndef CUTCP
	    if (!getconf("general", "user", no_auth->authenticator, 250)
		|| (strlen (no_auth->authenticator) == 0))
#endif
	      strcpy(no_auth->authenticator,"nobody");
#else /* not MSDOS */
	    DISABLE_PFS(whoiampw = getpwuid(getuid()));
	    if (whoiampw == 0) strcpy(no_auth->authenticator,"nobody");
	    else strcpy(no_auth->authenticator, whoiampw->pw_name);
#endif /* not MSDOS */
#else
	    jpi_itemlist.buflen = sizeof(username);
	    jpi_itemlist.itmcod = JPI$_USERNAME;
	    jpi_itemlist.bufadr = &username;
	    jpi_itemlist.retlenadr = &usernamelen;
	    jpi_itemlist.null = 0;
	    if (SYS$GETJPI(0, 0, 0, &jpi_itemlist, 0, 0, 0) & 0x1)
	    {
		username[usernamelen] = 0;
		strcpy(no_auth->authenticator, username);
	    } else
		strcpy(no_auth->authenticator, "nobody");
#endif
	}
	return(no_auth);
    }
$EOD
$!
$CREATE [.archie]get_vdir.c
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <stdio.h>

#include <pfs.h>
#include <pprot.h>
#include <perrno.h>
#include <pcompat.h>
#include <pauthent.h>
#include <pmachine.h>

#ifdef NEED_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#ifdef DEBUG
extern int	pfs_debug;
#endif

extern int	pwarn;
extern char	p_warn_string[];
extern int	perrno;
extern char	p_err_string[];

/*
 * get_vdir - Get contents of a directory given its location
 *
 *	      GET_VDIR takes a directory location, a list of desired
 *	      components, a pointer to a directory structure to be 
 *	      filled in, and flags.  It then queries the appropriate 
 *	      directory server and retrieves the desired information.
 *
 *      ARGS:   dhost       - Host on which directory resides
 *              dfile       - Directory on that host
 *              components  - The names from the directory we want
 *		dir	    - Structure to be filled in
 *	        flags       - Options.  See FLAGS
 *		filters     - filters to be applied to result 
 *              acomp       - Pointer to remaining components
 *
 *     FLAGS:	GVD_UNION   - Do not expand union links
 *		GVD_EXPAND  - Expand union links locally
 *		GVD_REMEXP  - Request remote expansion (& local if refused)
 *		GVD_LREMEXP - Request remote expansion of local union links
 *		GVD_VERIFY  - Only verify that args are for a directory
 *              GVD_ATTRIB  - Request attributes from directory server
 *              GVD_NOSORT  - Do not sort links when adding to directory
 *
 *   RETURNS:   PSUCCESS (0) or error code
 *		On some codes addition information in p_err_string
 *
 *     NOTES:   If acomp is non-null the string it points to might be modified
 *
 *              If the directory passed as an argument already has
 *		links or union links, then those lists will be freed
 *              before the new contents are filled in.
 *
 *              If a filter is passed to the procedure, and application of
 *              the filter results in additional union link, then those links
 *              will (or will not) be expanded as specified in the FLAGS field.
 *
 *              If the list of components in NULL, or the null string, then
 *              get_vdir will return all links in the requested directory.
 *
 *      BUGS:   Doesn't process union links yet
 *              Doesn't process errors returned from server
 *		Doesn't expand union links if requested to
 */
int
get_vdir(dhost,dfile,components,dir,flags,filters,acomp)
    char	*dhost;		/* Host on which directory resides           */
    char	*dfile;		/* Name of file on that host	             */
    char	*components;	/* Component name (wildcards allowed)        */
    PVDIR	dir;		/* Structure to be filled in		     */
    long	flags;		/* Flags			             */
    VLINK	filters;	/* Filters to be applied to result           */
    char	*acomp;		/* Components left to be resolved            */
    {
        PTEXT	request;	/* Text of request to dir server             */
	PTEXT	resp;	    	/* Response from dir server	             */

	char	ulcomp[MAX_VPATH];/* Work space for new current component    */
	char	*comp = components;

	VLINK	cur_link = NULL;/* Current link being filled in              */
	VLINK 	exp = NULL; 	/* The current ulink being expanded	     */
	VLINK	pul = NULL;     /* Prev union link (insert new one after it) */
	VLINK	l;		/* Temp link pointer 			     */
	int	mcomp;		/* Flag - check multiple components          */
	int	unresp;		/* Flag - received unresolved response       */
	int	getattrib = 0;  /* Get attributes from server                */
	int	vl_insert_flag; /* Flags to vl_insert                        */

	int	fwdcnt = MAX_FWD_DEPTH;

	int	no_links = 0;   /* Count of number of links found	     */

	char	options[40];    /* LIST option                               */
	char	*opt;           /* After leading +                           */

	PAUTH	authinfo;

	/* Treat null string like NULL (return entire directory) */
	if(!components || !*components) comp = NULL;

	if(acomp && !filters) mcomp = 1;
	else mcomp = 0;

	if(flags&GVD_ATTRIB) {
	    getattrib++;
	    flags &= (~GVD_ATTRIB);
	}

	if(flags&GVD_NOSORT) vl_insert_flag = VLI_NOSORT;
	else vl_insert_flag = VLI_ALLOW_CONF;
	flags &= (~GVD_NOSORT);

	if(filters) comp = NULL;

	perrno = 0;

	authinfo = get_pauth(PFSA_UNAUTHENTICATED);

	*options = '\0';

	if(getattrib) {
	    strcat(options,"+ATTRIBUTES");
	    flags &= (~GVD_ATTRIB);
	}

	if(!filters) { /* Can't do remote expansion if filters to be applied */
	    if(flags == GVD_REMEXP) strcat(options,"+EXPAND");
	    if(flags == GVD_LREMEXP) strcat(options,"+LEXPAND");
	}

	/* If all we are doing is verifying that dfile is a directory */
	/* then we do not want a big response from the directory      */
	/* server.  A NOT-FOUND is sufficient.			      */
	if(flags == GVD_VERIFY)
#ifdef NEWVERIFYOPT
	    strcat(options,"+VERIFY");
#else
	comp = "%#$PRobably_nOn_existaNT$#%";
#endif

	if(*options) opt = options+1;
	else opt = "''";

    startover:
	request = ptalloc();

	sprintf(request->start,
		"VERSION %d %s\nAUTHENTICATOR %s %s\nDIRECTORY ASCII %s\nLIST %s COMPONENTS %s%s%s\n",
		VFPROT_VNO, PFS_SW_ID, authinfo->auth_type,
		authinfo->authenticator, dfile, opt,
		(comp ? comp : ""), (mcomp ? "/" : ""),
		(mcomp ? acomp : ""));

	request->length = strlen(request->start);

#ifdef DEBUG
	if(pfs_debug > 2)
	    fprintf(stderr,"Sending message to dirsrv:\n%s",request->start);
#endif

#if defined(MSDOS)
	resp = dirsend(request,dhost,0L);
#else
	resp = dirsend(request,dhost,0);
#endif

#ifdef DEBUG
	if(pfs_debug && (resp == NULL)) {
	    fprintf(stderr,"Dirsend failed: %d\n",perrno);
	}
#endif

	/* If we don't get a response, then if the requested       */
	/* directory, return error, if a ulink, mark it unexpanded */
	if(resp == NULL) {
	    if(exp) exp->expanded = FAILED;
	    else return(perrno);
	}

	unresp = 0;

	/* Here we must parse reponse and put in directory */
	/* While looking at each packet 		   */
	while(resp) {
	    PTEXT		vtmp;
	    char		*line;

	    vtmp = resp;
#ifdef DEBUG
	    if(pfs_debug > 3) fprintf(stderr,"%s\n",resp->start);
#endif
	    /* Look at each line in packet */
	    for(line = resp->start;line != NULL;line = nxtline(line)) {
		switch (*line) {
		    
		    /* Temporary variables to hold link info */
		    char	l_linktype;
		    char 	l_name[MAX_DIR_LINESIZE];
		    char	l_type[MAX_DIR_LINESIZE];
		    char 	l_htype[MAX_DIR_LINESIZE];
		    char 	l_host[MAX_DIR_LINESIZE];
		    char 	l_ntype[MAX_DIR_LINESIZE];
		    char 	l_fname[MAX_DIR_LINESIZE];
		    int		l_version;
		    char 	t_unresolved[MAX_DIR_LINESIZE];
		    int		l_magic;
		    int		tmp;

		case 'L': /* LINK or LINK-INFO */
		    if(strncmp(line,"LINK-INFO",9) == 0) {
			PATTRIB		at;
			PATTRIB		last_at;
			at = parse_attribute(line);
			if(!at) break;

			/* Cant have link info without a link */
			if(!cur_link) {
			    perrno = DIRSRV_BAD_FORMAT;
			    atfree(at);
			    break;
			}
			
			if(cur_link->lattrib) {
			    last_at = cur_link->lattrib;
			    while(last_at->next) last_at = last_at->next;
			    at->previous = last_at;
			    last_at->next = at;
			}
			else {
			    cur_link->lattrib = at;
			    at->previous = NULL;
			}
			break;
		    }

		    /* Not LINK-INFO, must be LINK - if not check for error */
		    if(strncmp(line,"LINK",4) != 0) goto scanerr;

		    /* If only verifying, don't want to change dir */
		    if(flags == GVD_VERIFY) {
			break;
		    }
		    /* If first link and some links in dir, free them */
		    if(!no_links++) {
			if(dir->links) vllfree(dir->links); dir->links=NULL;
			if(dir->ulinks) vllfree(dir->ulinks); dir->ulinks=NULL;
			}
			
		    cur_link = vlalloc();

		    /* parse and insert file info */
		    tmp = sscanf(line,"LINK %c %s %s %s %s %s %s %d %d", &l_linktype,
				 l_type, l_name, l_htype, l_host, 
				 l_ntype, l_fname, &(cur_link->version),
				 &(cur_link->f_magic_no));

		    if(tmp != 9) {
			perrno = DIRSRV_BAD_FORMAT;
			vlfree(cur_link);
			break;
		    }

		    cur_link->linktype = l_linktype;
		    cur_link->type = stcopyr(l_type,cur_link->type);
		    cur_link->name = stcopyr(unquote(l_name),cur_link->name);
		    cur_link->hosttype = stcopyr(l_htype,cur_link->hosttype);
		    cur_link->host = stcopyr(l_host,cur_link->host);
		    cur_link->nametype = stcopyr(l_ntype,cur_link->nametype);
		    cur_link->filename = stcopyr(l_fname,cur_link->filename);

		    /* Double check to make sure we don't get */
		    /* back unwanted components		      */
		    /* OK to keep if special (URP) links      */
		    /* or if mcomp specified                  */
		    if(!mcomp && (cur_link->linktype == 'L') && 
		       (!wcmatch(cur_link->name,comp))) {
			vlfree(cur_link);
			break;
		    }

		    /* If other optional info was sent back, it must */
		    /* also be parsed before inserting link     ***  */
		    
		    
		    if(cur_link->linktype == 'L') 
			vl_insert(cur_link,dir,vl_insert_flag);
		    else {
			tmp = ul_insert(cur_link,dir,pul);

			/* If inserted after pul, next one after cur_link */
			if(pul && (!tmp || (tmp == UL_INSERT_SUPERSEDING)))
			    pul = cur_link;
		    }
		    
		    break;

		case 'F': /* FILTER, FAILURE or FORWARDED */
		    /* FORWARDED */
		    if(strncmp(line,"FORWARDED",9) == 0) {
			if(fwdcnt-- <= 0) {
			    ptlfree(resp);
			    perrno = PFS_MAX_FWD_DEPTH;
			    return(perrno);
			}
			/* parse and start over */

			tmp = sscanf(line,"FORWARDED %s %s %s %s %d %d", 
				     l_htype,l_host,l_ntype,l_fname,
				     &l_version, &l_magic);

			dhost = stcopy(l_host);
			dfile = stcopy(l_fname);

			if(tmp < 4) {
			    perrno = DIRSRV_BAD_FORMAT;
			    break;
			}

			ptlfree(resp);
			goto startover;
		    }
		    if(strncmp(line,"FILTER",6) != 0) goto scanerr;
		    break;


		case 'M': /* MULTI-PACKET (processed by dirsend) */
		case 'P': /* PACKET (processed by dirsend) */
		    break;

		case 'N': /* NOT-A-DIRECTORY or NONE-FOUND */
		    /* NONE-FOUND, we just have no links to insert */
		    /* It is not an error, but we must clear any   */
		    /* old links in the directory arg              */
		    if(strncmp(line,"NONE-FOUND",10) == 0) {
			/* If only verifying, don't want to change dir */
			if(flags == GVD_VERIFY) {
			    break;
			}

			/* If first link and some links in dir, free them */
			if(!no_links++) {
			    if(dir->links) vllfree(dir->links);
			    if(dir->ulinks) vllfree(dir->ulinks);
			    dir->links = NULL;
			    dir->ulinks = NULL;
			}
			break;
		    }
		    /* If NOT-A-DIRECTORY or anything else, scan error */
		    goto scanerr;

		case 'U': /* UNRESOLVED */
		    if(strncmp(line,"UNRESOLVED",10) != 0) {
			goto scanerr;
		    }
		    tmp = sscanf(line,"UNRESOLVED %s", t_unresolved);
		    if(tmp < 1) {
			perrno = DIRSRV_BAD_FORMAT;
			break;
		    }
		    /* If multiple components were resolved */
		    if(strlen(t_unresolved) < strlen(acomp)) {
			strcpy(ulcomp,acomp);
			/* ulcomp is the components that were resolved */
			*(ulcomp+strlen(acomp)-strlen(t_unresolved)-1) = '\0';
			/* Comp gets the last component resolved */
			comp = (char *) rindex(ulcomp,'/');
			if(comp) comp++;
			else comp = ulcomp;
			/* Let rd_vdir know what remains */
			strcpy(acomp,t_unresolved);
		    }
		    unresp = 1;
		    break;

		case 'V': /* VERSION-NOT-SUPPORTED */
		    if(strncmp(line,"VERSION-NOT-SUPPORTED",21) == 0) {
			perrno = DIRSRV_BAD_VERS;
			return(perrno);
		    }
		    goto scanerr;

		scanerr:
		default:
		    if(*line && (tmp = scan_error(line))) {
			ptlfree(resp);
			return(tmp);
		    }
		    break;
		}
	    }

	    resp = resp->next;

	    ptfree(vtmp);
	}

	/* We sent multiple components and weren't told any */
	/* were unresolved                                  */
	if(mcomp && !unresp) {
	    /* ulcomp is the components that were resolved */
	    strcpy(ulcomp,acomp);
	    /* Comp gets the last component resolved */
	    comp = (char *) rindex(ulcomp,'/');
	    if(comp) comp++;
	    else comp = ulcomp;
	    /* If we have union links to resolve, only one component remains */
	    mcomp = 0;
	    /* Let rd_vdir know what remains */
	    *acomp = '\0';
	}

	/* If only verifying, we already know it is a directory */
	if(flags == GVD_VERIFY) return(PSUCCESS);

	/* Don't return if matching was delayed by the need to filter    */
	/* if FIND specified, and dir->links is non null, then we have   */
	/* found a match, and should return.                             */
	if((flags & GVD_FIND) && dir->links && (!filters))
	    return(PSUCCESS);

	/* If expand specified, and ulinks must be expanded, making sure */
        /* that the order of the links is maintained properly            */

expand_ulinks:

	if((flags != GVD_UNION) && (flags != GVD_VERIFY)) {

	    l = dir->ulinks;

	    /* Find first unexpanded ulink */
	    while(l && l->expanded && (l->linktype == 'U')) l = l->next;
	    
	    /* Only expand if a FILE or DIRECTORY -  Mark as  */
            /* failed otherwise                               */
	    /* We must still add support for symbolic ulinks */
	    if(l) {
		if ((strcmp(l->type,"DIRECTORY") == 0) || 
		    (strcmp(l->type,"FILE") == 0)) {
		    l->expanded = TRUE;
		    exp = l;
		    pul = l;
		    dhost = l->host;
		    dfile = l->filename;
		    goto startover; /* was get_contents; */
		}
		else l->expanded = FAILED;
	    }
	}

	/* Double check to make sure we don't get */
	/* back unwanted components		  */
	/* OK to keep if special (URP) links      */
	if(components && *components) {
	    l = dir->links;
	    while(l) {
		VLINK	ol;
		if((l->linktype == 'L') && (!wcmatch(l->name,components))) {
		    if(l == dir->links)
			dir->links = l->next;
		    else l->previous->next = l->next;
		    if(l->next) l->next->previous = l->previous;
		    ol = l;
		    l = l->next;
		    vlfree(ol);
		}
		else l = l->next;
	    }
	}

	return(PSUCCESS);
    }
$EOD
$!
$CREATE [.archie]make.com
$DECK
$! --- MAKE.COM ---						!x='f$verify(0)'
$! Description:
$!  build the Archie client for VAX/VMS
$!
$! Written by Luke Brennan <brennan@coco.cchs.su.oz.au>
$!
$! Modifications:
$!    Date	Programmer	Reason for modification.
$! 20-Jan-92	   ldcb		Initial coding.
$!
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$ Archie_EXECUTABLE := "archie.exe"
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! add a new call here with the appropriate new Archie server host(s)
$!
$ arg == 1
$ Call AddHost "archie.ans.net"		"(USA [NY])"
$ Call AddHost "archie.rutgers.edu"	"(USA [NJ])"
$ Call AddHost "archie.sura.net"	"(USA [MD])"
$ Call AddHost "archie.unl.edu"		"(USA [NE])"
$ Call AddHost "archie.mcgill.ca"	"(Canada)"
$ Call AddHost "archie.funet.fi"	"(Finland/Mainland Europe)"
$ Call AddHost "archie.au"		"(Australia)"
$ Call AddHost "archie.doc.ic.ac.uk"	"(Great Britain/Ireland)"
$ MAXHOSTS = arg - 1
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! add a new call here with any new source file(s)
$! (P2 should be "SUPPORT" if intended for supporting an unknown TCPIP)
$!
$ arg == 1
$ Call AddSource "AQUERY"
$ Call AddSource "ARCHIE"
$ Call AddSource "ATALLOC"
$ Call AddSource "DIRSEND"
$ Call AddSource "GET_PAUTH"
$ Call AddSource "GET_VDIR"
$ Call AddSource "PERRMESG"
$ Call AddSource "PROCQUERY"
$ Call AddSource "PTALLOC"
$ Call AddSource "REGEX"
$ Call AddSource "STCOPY"
$ Call AddSource "SUPPORT"
$ Call AddSource "VLALLOC"
$ Call AddSource "VL_COMP"
$ Call AddSource "VMS_SUPPORT" "SUPPORT"
$ MAXSOURCEFILES = arg - 1
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! add a call here with any supported TCP/IP implementations
$!  P1 = name of TCP/IP product, P2 = cc/define to set,
$!  P3 = logical to look for,    P4 = location of link/OPT,
$!  P5 = MINIMUM VERSION of TCP/IP to support
$!
$!  Multinet should be last, as it can 'fake' a UCX if you want it to, so
$!  UCX would come up as the 'real' net even though Multinet is used.
$!
$ arg == 1
$ Call AddTCPIP "UCX"        "UCX"         "UCX$DEVICE" "[.vms]ucx.opt"
$ Call AddTCPIP "WOLLONGONG" "WOLLONGONG"  "TWG$TCP"    "[.vms]woll.opt"
$ Call AddTCPIP "MULTINET"   "MULTINET_30" "MULTINET"   "[.vms]multi.opt" "V3.0"
$ MAXTCPIPTYPES = arg - 1
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ YES = (1.eq.1)
$ NO  = (1.eq.0)
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ GoSub get_command_line_args
$ GoSub check_for_GNU_cc
$ GoSub check_which_TCPIP
$ GoSub ask_nearest_ARCHIE_HOST
$ GoSub check_for_strings_H
$ GoSub set_cc_defines
$ GoSub do_compiles
$ If (LINKAGE_REQUIRED)
$ Then GoSub do_link
$ Else Write Sys$OutPut "ARCHIE is up to date."
$ EndIF
$ Exit
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$get_command_line_args:
$ cmdline = P1 + P2 + P3 + P4 + P5 + P6 + P7 + P8
$ If ((f$locate("DEBUG",cmdline) .ne. f$length(cmdline)) -
 .or. (f$locate("DBG",cmdline)   .ne. f$length(cmdline)))
$ Then debug := "/DeBug"
$ Else debug := "/NOdebug"
$ EndIF
$ If (f$locate("FORCE",cmdline) .ne. f$length(cmdline))
$ Then FORCEBUILD = YES
$ Else FORCEBUILD = NO
$ EndIF
$ If (f$locate("LINK",cmdline) .ne. f$length(cmdline))
$ Then FORCELINK = YES
$ Else FORCELINK = NO
$ EndIF
$ If ((f$locate("?",cmdline) .ne. f$length(cmdline)) -
 .or. (f$locate("H",cmdline) .ne. f$length(cmdline)))
$ Then
$   Write Sys$Output "Usage:"
$   Write Sys$OutPut "     @MAKE [<debug>|<force>|<link>|<help>]
$   EXIT
$ EndIF
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$CHECK_FOR_GNU_CC:
$ If (f$trnlnm("GNU_CC") .nes. "")
$ Then
$   cc := "GCC/Optimize/Include=([])"
$   gnu_cc = YES
$ Else
$   cc := "CC/Optimize=NOinline/Include=([])"
$   gnu_cc = NO
$ EndIF
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! for product P1, cc/define=P2 if logical P3 present on system.
$! Libs/option = P4 if present. ALL get disregarded if less than version P5
$!
$CHECK_WHICH_TCPIP:
$ tcpip_flag :=
$ tcpip_libs :=
$ NO_TCPIP_SUPPORT = YES
$ i = 1
$tcp_loop:
$ If (i .gt. MAXTCPIPTYPES) Then GoTo tcp_check_done
$ If (f$type(tcpip_P2_'i') .eqs. "") Then GoTo tcp_check_done
$ If (f$type(tcpip_P3_'i') .nes. "")
$ Then
$   tcpip_logical = tcpip_P3_'i'
$   If (tcpip_logical .nes. "")
$   Then					! logical to look for
$     If (f$logical(tcpip_logical) .nes. "")
$     Then
$       tcpip_flag = tcpip_P2_'i'
$       tcpip_flag = f$fao(",!AS=1",tcpip_flag)
$       NO_TCPIP_SUPPORT = NO
$       If (f$type(tcpip_P4_'i') .nes. "")
$       Then					! link/OPT file location
$  	  tcpip_linkOPTs = tcpip_P4_'i'
$         If (tcpip_linkOPTs .nes. "")
$         Then
$           If (f$search(tcpip_linkOPTs) .nes. "")
$           Then
$             tcpip_libs = tcpip_P4_'i'
$             tcpip_libs = f$fao("!AS/Option",tcpip_libs)
$           EndIF
$         EndIF
$       EndIF
$       If (f$type(tcpip_P5_'i') .nes. "")
$       Then					! minimum version specified
$         If (tcpip_P5_'i' .nes. "")
$         Then
$           GoSub CheckIfVersionOK
$           If VERSION_TOO_EARLY
$           Then				! too early.. use SUPPORT files
$             tcpip_flag :=
$             tcpip_libs :=
$             NO_TCPIP_SUPPORT = YES
$	      tcp_ver = tcpip_P5_'i'
$             tcp_name = tcpip_P1_'i'
$             Write Sys$OutPut f$fao( -
		   "Your version of !AS is earlier than !AS.",tcp_name,tcp_ver)
	      Write Sys$OutPut "MAKE will use STD support files instead."
$           EndIF
$         EndIF
$       EndIF
$     EndIF
$   EndIF
$ EndIF
$ i = i + 1
$ Goto tcp_loop
$tcp_check_done:
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$ASK_NEAREST_ARCHIE_HOST:
$GoSub CLRSCN
$ Write Sys$OutPut f$fao("!AS!/!/", -
	"           Enter the number of the ARCHIE HOST nearest you.")
$!
$ i = 1
$_display_loop:
$ If (i .gt. MAXHOSTS) Then GoTo _display_done
$  Write Sys$OutPut f$fao("!2SL) !25AS  !AS",i,host_P1_'i',host_P2_'i')
$  i = i + 1
$ GoTo _display_loop
$_display_done:
$!
$ Assign/User_Mode/NOlog Sys$Command Sys$InPut
$_select_loop:				! get their selection
$ Read	 Sys$Command selection	-
	/End=_selection_made	-
	/Prompt="Enter number of your selection: "
$ If (selection .gt. MAXHOSTS)
$ Then
$   Write Sys$OutPut f$fao("!AS !2SL", "error: Options only go to", MAXHOSTS)
$   GoTo _select_loop
$ EndIF
$ ascii_string = f$edit(selection,"COLLAPSE,UPCASE")
$ ascii_char = f$extract(0,1,ascii_string)
$ If (.NOT. ("0" .les. ascii_char) .and. (ascii_char .les. "9"))
$ Then
$   Write Sys$OutPut -
	f$fao("error: Enter option NUMBER (up to !2SL)", MAXHOSTS)
$   Goto _select_loop
$ EndIF
$!
$ local_archie = host_P1_'selection'
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! If we're using VAXC then we need to grab STRINGS.H from SYS$LIBRARY.
$CHECK_FOR_STRINGS_H:
$ delete := delete
$ copy   := copy
$ If (f$search("strings.h") .nes. "") Then delete/nolog/noconfirm []strings.h;*
$ If .NOT. (GNU_CC) Then copy/noconfirm sys$library:string.h []strings.h
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! **NOTE** use of global symbol!!!
$! **NOTE** this is the only way I could pass it to the compile subroutine
$! **NOTE** without DCL and/or CC stripping off too many layers of quotes..
$! **NOTE** yeah.. I know.. It's ugly...  you work it out!! :-)
$SET_CC_DEFINES:
$ archie_host = " """"""ARCHIE_HOST=""""""""''local_archie'"""""""" """""" "
$ cflags :== /define=(debug=1,funcs=1,noregex=1'tcpip_flag','archie_host')
$!
$RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$DO_COMPILES:
$ GoSub CLRSCN
$ LINKAGE_REQUIRED == NO
$ If ("''f$type(Archie_EXECUTABLE)'" .nes. "")
$ Then
$   If (Archie_EXECUTABLE .nes. "")
$   Then If (f$search(Archie_EXECUTABLE) .eqs. "") Then LINKAGE_REQUIRED == YES
$   Else If (f$search("Archie.exe") .eqs. "") Then LINKAGE_REQUIRED == YES
$   EndIF
$ Else
$   If (f$search("Archie.exe") .nes. "") Then LINKAGE_REQUIRED == YES
$ EndIF
$ i = 1
$cc_loop:
$ If (i .gt. MAXSOURCEFILES) Then GoTo cc_done
$ source_file = source_P1_'i'
$ If ((f$type(source_P2_'i') .eqs. "") .or. (source_P2_'i' .eqs. ""))
$ Then Call Compile "''cc'" "''source_file'" "''debug'" 'FORCEBUILD'
$ Else
$   If ((NO_TCPIP_SUPPORT) .and. (source_P2_'i' .eqs. "SUPPORT"))
$   Then Call Compile "''cc'" "''source_file'" "''debug'" 'FORCEBUILD'
$   EndIF
$ EndIF
$ i = i + 1
$ GoTo cc_loop
$cc_done:
$ If (FORCELINK) Then LINKAGE_REQUIRED == YES
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$DO_LINK:
$ If (f$type(Archie_EXECUTABLE) .nes. "")
$ Then
$   If (Archie_EXECUTABLE .nes. "")
$   Then executable := /Exec='Archie_EXECUTABLE'
$   Else executable := /Exec=Archie.exe
$   EndIF
$ Else
$   executable := /Exec=Archie.exe
$ EndIF
$ i = 1
$ object_files :=
$object_files_loop:
$ If (i .gt. MAXSOURCEFILES) Then GoTo object_files_done
$ object_file = source_P1_'i'
$ If ((f$type(source_P2_'i') .eqs. "") .or. (source_P2_'i' .eqs. ""))
$ Then object_files := 'object_files'+'object_file'
$ Else
$   If ((NO_TCPIP_SUPPORT) .and. (source_P2_'i' .eqs. "SUPPORT"))
$   Then object_files := 'object_files'+'object_file'
$   EndIF
$ EndIF
$ i = i + 1
$ GoTo object_files_loop
$object_files_done:
$ If (f$extract(0,1,object_files) .eqs. "+")
$ Then object_files = f$extract(1,f$length(object_files),object_files)
$ EndIF
$ If (tcpip_libs .nes. "")
$ Then object_files = object_files + ","
$ EndIF
$!
$ Set Verify
$ Link'debug''executable' 'object_files''tcpip_libs'
$ x='f$verify(0)'
$!
$ Write Sys$OutPut " "
$ Write Sys$OutPut " "
$ Write Sys$OutPut "Done! Define the symbol ARCHIE & fire away."
$!
$ RETURN
$!
$! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$CLRSCN:
$If (f$GetDVI("TT:","TT_ANSICRT"))			! ANSI compatible?
$Then
$  CSI = "x["
$  CSI[0,8] = 27
$  CLS = CSI + "H" + CSI +"2J"
$  Write Sys$OutPut CLS
$Else
$  Write Sys$Output "''f$fao("!/!/!/!/!/!/!/!/!/!/!/")
$  Write Sys$Output "''f$fao("!/!/!/!/!/!/!/!/!/!/!/!/")
$EndIF
$Return
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$AddHOST: SUBROUTINE
$ host_P1_'arg' :== "''P1'"
$ host_P2_'arg' :== "''P2'"
$ arg == arg + 1	! *NOTE* global symbols used...
$ENDSUBROUTINE
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$AddSOURCE: SUBROUTINE
$ source_P1_'arg' :== "''P1'"
$ source_P2_'arg' :== "''P2'"
$ arg == arg + 1	! *NOTE* global symbols used...
$ENDSUBROUTINE
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$AddTCPIP: SUBROUTINE
$ tcpip_P1_'arg' :== "''P1'"
$ tcpip_P2_'arg' :== "''P2'"
$ tcpip_P3_'arg' :== "''P3'"
$ tcpip_P4_'arg' :== "''P4'"
$ tcpip_P5_'arg' :== "''P5'"
$ arg == arg + 1	! *NOTE* global symbols used...
$ENDSUBROUTINE
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! **NOTE** cflags is a GLOBAL symbol due to problems with quoted /Defines
$! **NOTE** not passing down correctly.. (I gave up!)
$Compile: SUBROUTINE
$ YES = (1.eq.1)
$! --- do a Make of only that source which has been modified since its
$!     object code was generated or that is missing its object code.
$ cc       = "''P1'"
$ source   = "''P2'"
$ dbg      = "''P3'"
$ FORCED   = P4
$!
$source = source - ".C" + ".C"
$ t1 = f$search("''source'")				! source exists?
$  If (t1 .eqs. "") Then GoTo _error_source_missing	! YIPE!
$   source = source - ".C"
$    if (FORCED) Then GoTo _compile_the_source		! forced to compile
$     t1 = f$search("''source'.OBJ")			! object exist?
$     If (t1 .eqs. "") Then GoTo _compile_the_source	! object missing
$     t1 = f$file_attributes("''source'.OBJ","RDT")	! when was the OBJECT
$    t1 = f$cvtime(t1)					! produced? (rev date)
$   t2 = f$file_attributes("''source'.C","RDT")		! when was source last
$  t2 = f$cvtime(t2)					! modified?
$ If (t1 .ges. t2) Then GoTo _bypass_compile		! object still current
$_compile_the_source:
$ set verify
$ 'cc -
  'cflags -
  'dbg 'source
$ x='f$verify(0)'
$ LINKAGE_REQUIRED == YES
$  GoTo _cc_done
$_bypass_compile:					! didn't need to
$  GoTo _cc_done					! generate new OBJ file
$_error_source_missing:
$ Write Sys$OutPut "ERROR: unable to locate source file ''source'.C"
$_cc_done:
$ENDSUBROUTINE
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$CHECKIFVERSIONOK:
$ required_version = tcpip_P5_'i'
$ tcpip_type = tcpip_P1_'i'
$ If ("MULTINET" .eqs. tcpip_type)
$ Then						! I know how to check MULTINET
$   If (f$search("MULTINET:MULTINET_VERSION.;") .nes. "")
$   Then
$     Open/share=READ fd MULTINET:MULTINET_VERSION.;
$     Read fd buffer
$     Close fd
$     v = buffer - "VERSION"
$     v = f$edit(V,"TRIM,COMPRESS")
$     If (v .ges. required_version)
$     Then VERSION_TOO_EARLY = NO
$     Else VERSION_TOO_EARLY = YES
$     EndIF
$   Else
$     VERSION_TOO_EARLY = YES
$   EndIF
$ Else						! don't know, so assume current
$   VERSION_TOO_EARLY = NO
$ EndIF
$!
$ RETURN
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


$EOD
$!
$CREATE [.archie]makefile.cut
$DECK
#
# CUTCP/CUTE BC++ version 11/22/91 bkc@omnigate.clarkson.edu
# Last changed: 12/03/91 1.2.1
#
# Makefile for the minimal build for an archie Prospero client.
#.INCLUDE ../../../builtins

OBJS	= aquery.obj	archie.obj	atalloc.obj \ 
	  get_pauth.obj	get_vdir.obj	perrmesg.obj	procquery.obj	\
	  ptalloc.obj	regex.obj	stcopy.obj	support.obj\
	  vlalloc.obj	vl_comp.obj 	dirsend.obj	misc.obj

# dirsend.obj

HDRS	= archie.h	pmachine.h	pcompat.h

DEFINES	= -DDEBUG -DCUTCP

CFLAGS= -k -N -ml -r -v $(DEFINES) -I.
LFLAGS= /P/m/s
CC = bccx

EXE	= archie.exe

all: $(EXE)

$(OBJS): $(HDRS)

.c.obj:
	$(CC)  $(CFLAGS) -c $<

$(EXE): $(OBJS) Makefile 
	tlink $(LFLAGS) @&&!
c0l $(OBJS)
!,archie,archie,@&&!
lib\tcp lib\sess lib\enet lib\vjc lib\over cl
!

clean:
	+-del *.obj
$EOD
$!
$CREATE [.archie]makefile.dos
$DECK
#
# Last changed: 11/20/91, v1.2
#
# Makefile for the minimal build for an archie Prospero client.
#.INCLUDE ../../../builtins

OBJS	= aquery.lo	archie.lo	atalloc.lo	dirsend.lo	\
	  get_pauth.lo	get_vdir.lo	perrmesg.lo	procquery.lo	\
	  ptalloc.lo	regex.lo	stcopy.lo	support.lo	\
	  vlalloc.lo	vl_comp.lo
HDRS	= archie.h	pmachine.h	pcompat.h

DEFINES	= -DDEBUG -DNOREGEX -DUSG

CFLAGS	= -Oeclgsz -Gs -Zi -W4 -I. $(DEFINES)

EXE	= archie.exe

all: $(EXE)

$(OBJS): $(HDRS)

$(EXE): $(OBJS) Makefile archie.lnk
	link @archie.lnk
	exepack archie.unp archie.exe

clean:
	+-del *.lo
	+-del *.exe

$EOD
$!
$CREATE [.archie]makefile.os2
$DECK
#
# Last changed: 11/20/91, v1.2
#
# Makefile for the minimal build for an archie Prospero client.
.INCLUDE ../../../builtins

OBJS	= aquery.obj	archie.obj	atalloc.obj	dirsend.obj	\
	  get_pauth.obj	get_vdir.obj	perrmesg.obj	procquery.obj	\
	  ptalloc.obj	regex.obj	stcopy.obj	support.obj	\
	  vlalloc.obj	vl_comp.obj
HDRS	= archie.h	pmachine.h	pcompat.h

DEFINES	= -DDEBUG -DNOREGEX

IFLAGS	= -I. -I../../../include $(DEFINES)

LFLAGS 	= /stack:30000/nod/noe/noi/map/CO
LIBS	= pctcp bsd ftpcrt socket os2
EXE	= archie.exe

all: $(EXE)

install: $(EXE)
	cp $[m,*.exe,$**] ..\..\..\bin
	@touch install

$(OBJS): $(HDRS)

$(EXE): $(OBJS) Makefile
	$(LD) $(LFLAGS) $(L_DEBUG) <@<
$[s,"+ \n",$[m,*.obj,$**]]
$*
$*
$(LIBS)

<
	+markexe lfns $@
	+markexe windowcompat $@

clean:
	+-del *.obj
	+-del *.exe

$EOD
$!
$CREATE [.archie.msdos]cutcp.h
$DECK
/* cutcp.h -- defs for cutcp code */


int 		netlisten(unsigned int port);
struct machinfo *Shostlook(char *name);
int 		Sdomain(char *name);
struct machinfo *Sgethost(char *name);
struct machinfo *Slookip(unsigned long *address);
void		netgetftp(unsigned int array[], unsigned int port);
int 		netopen(unsigned long *address, unsigned int port);
int 		Snetopen(struct machinfo *m, unsigned int port);
int		netqlen(int handle);
int		netroom(int handle);
int		netread(int pnum, unsigned char *buffer, unsigned int n);
int		netwrite(int pnum, unsigned char *buffer, unsigned int n);
int		netest(int pnum);
int		netpush(int pnum);
void		netulisten(unsigned int portnum);
int		netusend(unsigned long *address, unsigned destport, 
			unsigned sourceport, unsigned char *buffer, int count);
int		neturead(unsigned char	*buffer);
struct machinfo *Slooknum(int pnum);
int		Snetinit();
int		Shostfile(char *name);
int		netshut();
int		Sgetevent(int classes, int *clss, int *data);
int		netclose(int pnum);
unsigned int	intswap(unsigned int val);
unsigned long	n_clicks(void *p);
void		Stask();

#define	ntohs(a)	(intswap(a))
#define	htons(a)	(intswap(a))
$EOD
$!
$CREATE [.archie.msdos]hostform.h
$DECK
/* hfile.inc - placed into all .h files to set up for PVCS 
   $Header:   E:/pcdirs/vcs/hostform.h_v   1.0   15 Jan 1990 19:30:22   bkc  $
   Revision History ----------------------------------------------------
   $Log:   E:/pcdirs/vcs/hostform.h_v  $
 * 
 *    Rev 1.0   15 Jan 1990 19:30:22   bkc
*/


/*
*  Host and local machine configuration information.
*
****************************************************************************
*                                                                          *
*      NCSA Telnet for the PC                                              *
*      by Tim Krauskopf, VT100 by Gaige Paulsen, Tek by Aaron Contorer     *
*                                                                          *
*      National Center for Supercomputing Applications                     *
*      152 Computing Applications Building                                 *
*      605 E. Springfield Ave.                                             *
*      Champaign, IL  61820                                                *
*                                                                          *
*      This program is in the public domain.                               *
*                                                                          *
****************************************************************************
*/

/*
*  Linked list of structures which describe other machines.
*  Arranged one per session with unique session names.
*/

struct machinfo {
	unsigned char 
		*sname,					/* pointer to name of session */
		*hname,                 /* pointer to name of that machine */
		*font,					/* font name, if we can do it */
		*keymap,				/* pointer to keymap file name */
		hostip[4],				/* IP number of this machine */
		gateway,				/* gateway preference, start with 1 */
		nameserv,				/* nameserver preference, start with 1 */
		bksp,					/* backspace value */
		halfdup,				/* half duplex required */
		crmap,					/* Strange Berkeley 4.3 CR mode needed */
		vtwrap,					/* flag on when need wrap mode */
		vtmargin;				/* col to ring bell at */
	int
		clearsave,				/* whether to save cleared lines */
		fsize,					/* font size in points */
		nfcolor[3],				/* normal foreground */
		nbcolor[3],				/* normal background */
		bfcolor[3],				/* blink             */
		bbcolor[3],
		ufcolor[3],             /* underline */
		ubcolor[3],
		mno,					/* machine number for reference */
		mstat,					/* status of this machine entry */
		bkscroll,				/* how many lines to save */
		retrans,				/* initial retrans timeout */
		conto,					/* time out in seconds to wait for connect */
		window,					/* window, will be checked against buffers */
		maxseg,					/* maximum receivable segment size */
		mtu,					/* maximum transfer unit MTU (out) */
                domainsremaining,                       /* how many domain search list entries remain */
                destport,                               /* yepper, you can telnet to a different port than 23 */
		flags;					/* general flags holder */		
#define	MFLAGS_SCRIPT	0x80
	struct machinfo *next;		/* surprise, its a linked list! */
};

struct machinfo *Sgethost(),*Shostlook(),*Slooknum(),*Slookip(),*Smadd();

/*
*  status fields for mstat, what do we know about that machine?
*/
#define NOIP 1					/* we don't have IP number */
#define UDPDOM 3				/* there is a UDP request pending on it */
/*  The next 20 numbers are reserved for UDPDOM */
#define HAVEIP 50				/* at least we have the # */
#define HFILE 70				/* we have IP number from host file */
#define DOM 71					/* we have an IP number from DOMAIN */
#define FROMKIP 72				/* have IP# from KIP server */



/*
*   Configuration information which 
*   the calling program may want to obtain from the hosts file.
*   The calling program should include hostform.h and call
*   Sgetconfig(cp)
*     struct config *cp;
*   which will copy the information to the user's data structure.
*/
struct config {
	unsigned char
		netmask[4],				/* subnetting mask being used */
		havemask,				/* do we have a netmask? */
		irqnum,					/* which hardware interrupt */
		myipnum[4],				/* what is my IP #? */
		me[32],					/* my name description (first 30 chars) */
		color[3],				/* default colors to use */
		hw[10],					/* hardware type for network */
		video[10],				/* video graphics hardware available */
		bios,					/* flag, do we want to use BIOS for screen access */
		tek,					/* flag, enable tektronix graphics */
		ftp,					/* flag, enable ftp server */
		rcp,					/* flag, enable rcp server */
		comkeys,				/* flag, commandkeys=yes */
		*termtype,				/* terminal type specification */
		*zone,					/* AppleTalk zone for KIP NBP */
		*defdom,				/* default domain */
		*capture,				/* pointer to where the capture file name is */
		*pass,					/* pointer to where the password file name is */
		*hpfile,				/* HP file name */
		*psfile,				/* PS file name */
		*tekfile;				/* tek file name */
int
		nstype,					/* nameserver = 1-domain 2-IEN116  */
		domto,					/* time out for UDP domain request */
		ndom,					/* number of retries for domain requests */
		timesl,					/* time slice */
		address,				/* segment address */
		ioaddr;					/* I/O address */
char            *domainpath;                               /* domain name search path */
char            *map3270;                               /* path to 3270 map */
char            tnmode;                                 /* how we decide to connect to 3270 streams */
long		sys_flags;
#define	SYS_FLAGS_VISUAL_BELL	0x1		/* use visual bell */

};


#define TNMODE_FLAGGED  0
#define TNMODE_AUTO     1
#define TNMODE_NOTFLAGGED 2
#define TNMODE_OFF      3
$EOD
$!
$CREATE [.archie.msdos]netevent.h
$DECK
/* hfile.inc - placed into all .h files to set up for PVCS 
   $Header:   E:/pcdirs/vcs/netevent.h_v   1.0   15 Jan 1990 19:29:26   bkc  $
   Revision History ----------------------------------------------------
   $Log:   E:/pcdirs/vcs/netevent.h_v  $
 * 
 *    Rev 1.0   15 Jan 1990 19:29:26   bkc
*/


/*
*  Events for event processing in NCSA Telnet.
*  Used for netgetevent().
*/


#define USERCLASS	1	/* the user program will accept these events */
#define ICMPCLASS	2	/* ICMP in netsleep will look for these */
#define ERRCLASS    4	/* the user may or may not read these error messages */
#define SCLASS		8	/* the background server will take these */
#define CONCLASS    0x10	/* the application manages connections with these */

#define ERR1	1		/* an error message is waiting, ERRCLASS */

#define IREDIR	1		/* ICMP redirect, ICMPCLASS */

#define CONOPEN 1		/* connection has opened, CONCLASS */
#define CONDATA 2       /* there is data available on this connection */
#define CONCLOSE 3		/* the other side has closed its side of the connection */
#define CONFAIL 4		/* connection open attempt has failed */

#define UDPDATA 1		/* UDP data has arrived on listening port, USERCLASS */
#define DOMOK	2		/* domain name ready */
#define DOMFAIL 3		/* domain name lookup failed */
#define FTPCOPEN 20     /* FTP command connection has opened */
#define FTPCLOSE 21     /* FTP command connection has closed */
#define FTPBEGIN 22     /* FTP transfer beginning, dat =1 for get, 0 for put */
#define FTPEND   23     /* FTP transfer ending */
#define FTPLIST  24     /* FTP file listing taking place */
#define FTPUSER  25     /* FTP user name has been entered */
#define FTPPWOK	26		/* FTP password verified */
#define FTPPWNO 27		/* FTP password failed */
#define RCPBEGIN 30		/* RCP beginning */
#define RCPEND 31		/* RCP ending */

#define UDPTO 1			/* UDP request from DOMAIN timed out, SCLASS */
#define FTPACT 2		/* FTP transfer is active, keep sending */
#define TCPTO  3		/* TCP for DOMAIN timed out */
#define RCPACT 4		/* rcp is active, needs CPU time */
#define RETRYCON 5		/* retry connection packet, might be lost */
#define DOMNEXT 6               /* search next domain list entry */
#define E_CLOCK 7
#ifdef	SCRIPT
#define	SCRIPT_EVENT	1	/* script next step */
#define	SCRIPT_DATA	2	/* received some data */
#define	SCRIPT_PROC	3	/* just process stuff */
#define	SCRIPT_CLOSE	4	/* connection was closed */
#define	SCRIPT_FORCE	5	/* user forced connection closed */
#define	SCRIPT_DOMAIN	6	/* domain name lookup ok */
#define	SCRIPT_FUNC	8
/* int Script_Event(int type, void *twin, unsigned int data); */
#endif
$EOD
$!
$CREATE [.archie]perrmesg.c
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <perrno.h>
#include <stdio.h>

/* This file and perrno.h should always be updated simultaneously */

int	perrno = 0;
int	pwarn = 0;
char	p_err_string[P_ERR_STRING_SZ];
char	p_warn_string[P_ERR_STRING_SZ];

char	*p_err_text[256] = {
    /*   0 */ "Success (prospero)",
    /*   1 */ "Port unknown (dirsend)",
    /*   2 */ "Can't open local UDP port (dirsend)",
    /*   3 */ "Can't resolve hostname (dirsend)",
    /*   4 */ "Unable to send entire message (dirsend)",
    /*   5 */ "Timed out (dirsend)",
    /*   6 */ "Recvfrom failed (dirsend)",
    /*   7 */ "",    /*   8 */ "",    /*   9 */ "",    /*  10 */ "",
    /*  11 */ "Sendto failed (reply)",
    /*  12 */ "",    /*  13 */ "",    /*  14 */ "",    /*  15 */ "",
    /*  16 */ "",    /*  17 */ "",    /*  18 */ "",    /*  19 */ "",
    /*  20 */ "",
    /*  21 */ "Link already exists (vl_insert)",
    /*  22 */ "Link with same name already exists (vl_insert)",
    /*  23 */ "",    /*  24 */ "",
    /*  25 */ "Link already exists (ul_insert)",
    /*  26 */ "Replacing existing link (ul_insert)",
    /*  27 */ "Previous entry not found in dir->ulinks (ul_insert)",
    /*  28 */ "",    /*  29 */ "",    /*  30 */ "",    /*  31 */ "",
    /*  32 */ "",    /*  33 */ "",    /*  34 */ "",    /*  35 */ "",
    /*  36 */ "",    /*  37 */ "",    /*  38 */ "",    /*  39 */ "",
    /*  40 */ "",
    /*  41 */ "Temporary not found (rd_vdir)",
    /*  42 */ "Namespace not closed with object (rd_vdir)",
    /*  43 */ "Alias for namespace not defined (rd_vdir)",
    /*  44 */ "Specified namespace not found (rd_vdir)",
    /*  45 */ "",    /*  46 */ "",    /*  47 */ "",    /*  48 */ "",
    /*  49 */ "",    /*  50 */ "",
    /*  51 */ "File access method not supported (pfs_access)",
    /*  52 */ "",    /*  53 */ "",    /*  54 */ "",
    /*  55 */ "Pointer to cached copy - delete on close (pmap_cache)",
    /*  56 */ "Unable to retrieve file (pmap_cache)",
    /*  57 */ "",    /*  58 */ "",    /*  59 */ "",    /*  60 */ "",
    /*  61 */ "Directory already exists (mk_vdir)",
    /*  62 */ "Link with same name already exists (mk_vdir)",
    /*  63 */ "",    /*  64 */ "",
    /*  65 */ "Not a virtual system (vfsetenv)",
    /*  66 */ "Can't find directory (vfsetenv)",
    /*  67 */ "",    /*  68 */ "",    /*  69 */ "",    /*  70 */ "",
    /*  71 */ "Link already exists (add_vlink)",
    /*  72 */ "Link with same name already exists (add_vlink)",
    /*  73 */ "",    /*  74 */ "",    /*  75 */ "",    /*  76 */ "",
    /*  77 */ "",    /*  78 */ "",    /*  79 */ "",    /*  80 */ "",
    /*  81 */ "",    /*  82 */ "",    /*  83 */ "",    /*  84 */ "",
    /*  85 */ "",    /*  86 */ "",    /*  87 */ "",    /*  88 */ "",
    /*  89 */ "",    /*  90 */ "",    /*  91 */ "",    /*  92 */ "",
    /*  93 */ "",    /*  94 */ "",    /*  95 */ "",    /*  96 */ "",
    /*  97 */ "",    /*  98 */ "",    /*  99 */ "",    /* 100 */ "",
    /* 101 */ "",    /* 102 */ "",    /* 103 */ "",    /* 104 */ "",
    /* 105 */ "",    /* 106 */ "",    /* 107 */ "",    /* 108 */ "",
    /* 109 */ "",    /* 110 */ "",    /* 111 */ "",    /* 112 */ "",
    /* 113 */ "",    /* 114 */ "",    /* 115 */ "",    /* 116 */ "",
    /* 117 */ "",    /* 118 */ "",    /* 119 */ "",    /* 120 */ "",
    /* 121 */ "",    /* 122 */ "",    /* 123 */ "",    /* 124 */ "",
    /* 125 */ "",    /* 126 */ "",    /* 127 */ "",    /* 128 */ "",
    /* 129 */ "",    /* 130 */ "",    /* 131 */ "",    /* 132 */ "",
    /* 133 */ "",    /* 134 */ "",    /* 135 */ "",    /* 136 */ "",
    /* 137 */ "",    /* 138 */ "",    /* 139 */ "",    /* 140 */ "",
    /* 141 */ "",    /* 142 */ "",    /* 143 */ "",    /* 144 */ "",
    /* 145 */ "",    /* 146 */ "",    /* 147 */ "",    /* 148 */ "",
    /* 149 */ "",    /* 150 */ "",    /* 151 */ "",    /* 152 */ "",
    /* 153 */ "",    /* 154 */ "",    /* 155 */ "",    /* 156 */ "",
    /* 157 */ "",    /* 158 */ "",    /* 159 */ "",    /* 160 */ "",
    /* 161 */ "",    /* 162 */ "",    /* 163 */ "",    /* 164 */ "",
    /* 165 */ "",    /* 166 */ "",    /* 167 */ "",    /* 168 */ "",
    /* 169 */ "",    /* 170 */ "",    /* 171 */ "",    /* 172 */ "",
    /* 173 */ "",    /* 174 */ "",    /* 175 */ "",    /* 176 */ "",
    /* 177 */ "",    /* 178 */ "",    /* 179 */ "",    /* 180 */ "",
    /* 181 */ "",    /* 182 */ "",    /* 183 */ "",    /* 184 */ "",
    /* 185 */ "",    /* 186 */ "",    /* 187 */ "",    /* 188 */ "",
    /* 189 */ "",    /* 190 */ "",    /* 191 */ "",    /* 192 */ "",
    /* 193 */ "",    /* 194 */ "",    /* 195 */ "",    /* 196 */ "",
    /* 197 */ "",    /* 198 */ "",    /* 199 */ "",    /* 200 */ "",
    /* 201 */ "",    /* 202 */ "",    /* 203 */ "",    /* 204 */ "",
    /* 205 */ "",    /* 206 */ "",    /* 207 */ "",    /* 208 */ "",
    /* 209 */ "",    /* 210 */ "",    /* 211 */ "",    /* 212 */ "",
    /* 213 */ "",    /* 214 */ "",    /* 215 */ "",    /* 216 */ "",
    /* 217 */ "",    /* 218 */ "",    /* 219 */ "",    /* 220 */ "",
    /* 221 */ "",    /* 222 */ "",    /* 223 */ "",    /* 224 */ "",
    /* 225 */ "",    /* 226 */ "",    /* 227 */ "",    /* 228 */ "",
    /* 229 */ "",
    /* 230 */ "File not found (prospero)",
    /* 231 */ "Directory not found (prospero)",
    /* 232 */ "Symbolic links nested too deep (prospero)",
    /* 233 */ "Environment not initialized - source vfsetup.source then run vfsetup",
    /* 234 */ "Can't traverse an external file (prospero)",
    /* 235 */ "Forwarding chain is too long (prospero)",
    /* 236 */ "",    /* 237 */ "",    /* 238 */ "",    /* 239 */ "",
    /* 240 */ "",    /* 241 */ "",
    /* 242 */ "Authentication required (prospero server)",
    /* 243 */ "Not authorized (prospero server)",
    /* 244 */ "Not found (prospero server)",
    /* 245 */ "Bad version number (prospero server)",
    /* 246 */ "Not a directory (prospero server)",
    /* 247 */ "Already exists (prospero server)",
    /* 248 */ "Link with same name already exists (prospero server)",
    /* 249 */ "",    /* 250 */ "",
    /* 251 */ "Command not implemented on server (dirsrv)",
    /* 252 */ "Bad format for response (dirsrv)",
    /* 253 */ "Protocol error (prospero server)",
    /* 254 */ "Unspecified server failure (prospero server)",
    /* 255 */ "Generic Failure (prospero)"};

char	*p_warn_text[256] = {
    /*   0 */ "No warning",
    /*   1 */ "You are using an old version of this program",
    /*   2 */ "From server",
    /*   3 */ "Unrecognized line in response from server",
  /* 4-254 */ "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    /* 255 */ ""};

#ifndef ARCHIE
perrmesg(prefix,no,text)
    char	*prefix;
    int		no;
    char	*text;
    {
	fprintf(stderr,"%s%s%s%s\n", (prefix ? prefix : ""),
		(no ? p_err_text[no] : p_err_text[perrno]),
		((text ? (*text ? " - " : "") : 
		  (!no && *p_err_string ? " - " : ""))),
		(text ? text : (no ? "" : p_err_string)));
    }

sperrmesg(buf,prefix,no,text)
    char	*buf;
    char	*prefix;
    int		no;
    char	*text;
    {
	sprintf(buf,"%s%s%s%s\n", (prefix ? prefix : ""),
		(no ? p_err_text[no] : p_err_text[perrno]),
		((text ? (*text ? " - " : "") : 
		  (!no && *p_err_string ? " - " : ""))),
		(text ? text : (no ? "" : p_err_string)));
    }

pwarnmesg(prefix,no,text)
    char	*prefix;
    int		no;
    char	*text;
    {
	fprintf(stderr,"%s%s%s%s\n", (prefix ? prefix : ""),
		(no ? p_warn_text[no] : p_warn_text[pwarn]),
		((text ? (*text ? " - " : "") : 
		  (!no && *p_warn_string ? " - " : ""))),
		(text ? text : (no ? "" : p_warn_string)));
    }

spwarnmesg(buf,prefix,no,text)
    char	*buf;
    char	*prefix;
    int		no;
    char	*text;
    {
	sprintf(buf,"%s%s%s%s\n", (prefix ? prefix : ""),
		(no ? p_warn_text[no] : p_warn_text[pwarn]),
		((text ? (*text ? " - " : "") : 
		  (!no && *p_warn_string ? " - " : ""))),
		(text ? text : (no ? "" : p_warn_string)));
    }
#endif
$EOD
$!
$CREATE [.archie]patchlevel.h
$DECK
/*
 * Archie v1.3
 *
 * History:
 *
 * 04/14/92 v1.3.2 - Release.
 * 01/10/92 v1.3.1 - Release.
 * 01/09/92 v1.3 - Release.
 * 12/13/91      - added UCX support
 * 12/03/91      - added CUTCP support
 * 11/20/91 v1.2 - Release.
 * 11/18/91      - ported to DOS & OS/2
 * 11/12/91      - finally got to test under Multinet 3.0
 * 10/03/91      - replaced regex.c for oz
 * 09/25/91	 - added Wollongong support
 * 08/30/91      - ported to VMS
 * 08/20/91 v1.1 - Major revisions
 * 07/31/91 v1.0 - Original
 */
$EOD
$!
$CREATE [.archie]pauthent.h
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

#define PFSA_UNAUTHENTICATED		1

struct pfs_auth_info {
    char			auth_type[100];
    char			authenticator[250];
};

typedef struct pfs_auth_info *PAUTH;
typedef struct pfs_auth_info PAUTH_ST;

PAUTH get_pauth();

#ifndef VMS
# ifndef IN_H
#  include <netinet/in.h>
#  define IN_H
# endif
#else
# ifndef _ARCHIE_VMS
#  include <vms.h>
# endif
#endif

struct client_info {
    int				ainfo_type;
    char			*auth_type;
    char			*authenticator;
    char			*userid;
    short			port;
    struct in_addr		haddr;
    struct pfs_auth_info	*previous;
    struct pfs_auth_info	*next;
};

typedef struct client_info *CINFO;
typedef struct client_info CINFO_ST;
$EOD
$!
$CREATE [.archie]pcompat.h
$DECK
/*
 * Copyright (c) 1989, 1990 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

/* 
 * pcompat.h - Definitions for compatability library
 *
 * This file contains the defintions used by the compatability
 * library.  Among the definitions are the possible values for
 * pfs_disable_flag.  This file also contains the external 
 * declaration of that variable.  Note, however that the 
 * the module pfs_disable_flag.o is included in libpfs.a
 * because some of the routines in that library set it.
 * The only place it is checked, however, is in pfs_access, 
 * found in libpcompat.a
 *
 */

extern	int		pfs_default;
extern	int		pfs_enable;

/* Definitions for values of pfs_enable */
#define PMAP_DISABLE      0
#define PMAP_ENABLE       1
#define PMAP_COLON	  2
#define PMAP_ATSIGN_NF	  3
#define PMAP_ATSIGN	  4

#define DISABLE_PFS(stmt) do {int DpfStmp; DpfStmp = pfs_enable;\
			   pfs_enable = PMAP_DISABLE; \
			   stmt; \
			   pfs_enable = DpfStmp;} while (0)

/* Definitions for PFS_ACCESS */
#define PFA_MAP           0  /* Map the file name only                       */
#define PFA_CREATE        1  /* Create file if not found                     */
#define PFA_CRMAP         2  /* Map file name.  Map to new name if not found */
#define PFA_RO            4  /* Access to file is read only                  */

#define check_pfs_default() \
	do { if (pfs_default == -1) get_pfs_default(); } while (0)
$EOD
$!
$CREATE [.archie]perrno.h
$DECK
/*
 * Copyright (c) 1989, 1990 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

/* This file and perrmesg.c should be updated simultaneously.  */

/*
 * perrno.h - definitions for perrno
 *
 * This file contains the declarations and defintions of of the external
 * error values in which errors are returned by the pfs and psrv
 * libraries.
 */

#define		P_ERR_STRING_SZ 100	 /* Size of error string	    */

extern int	perrno;
extern char	p_err_string[];
extern char	*p_err_text[];

extern int	pwarn;
extern char	p_warn_string[];
extern char	*p_warn_text[];

/* Error codes returned or found in verrno */

#ifndef PSUCCESS
#define	PSUCCESS		0
#endif

/* dirsend (perrno) */
#define DIRSEND_PORT_UNKN	1	/* DIRSRV UDP port unknown      */
#define DIRSEND_UDP_CANT	2	/* Can't open local UDP port    */
#define DIRSEND_BAD_HOSTNAME	3	/* Can't resolve hostname       */
#define DIRSEND_NOT_ALL_SENT	4	/* Didn't send entire message   */
#define DIRSEND_SELECT_FAILED	5	/* Select failed	        */
#define DIRSEND_BAD_RECV	6	/* Recvfrom failed 	        */

/* reply */
#define REPLY_NOTSENT		11	/* Reply: sendto failed	        */

/* vl_insert */
#define VL_INSERT_ALREADY_THERE	21	/* Link already exists	        */
#define VL_INSERT_CONFLICT	22	/* Link exists with same name   */

/* ul_insert */
#define UL_INSERT_ALREADY_THERE 25	/* Link already exists		*/
#define UL_INSERT_SUPERSEDING   26	/* Replacing existing link	*/
#define UL_INSERT_POS_NOTFOUND  27	/* Prv entry not in dir->ulinks */

/* rd_vdir */
#define RVD_DIR_NOT_THERE	41	/* Temporary NOT_FOUND		    */
#define RVD_NO_CLOSED_NS	42	/* Namespace not closed w/ object:: */
#define RVD_NO_NS_ALIAS		43	/* No alias for namespace NS#:      */
#define RVD_NS_NOT_FOUND	44	/* Specified namespace not found    */

/* pfs_access */
#define PFSA_AM_NOT_SUPPORTED   51      /* Access method not supported  */

/* pmap_cache */
#define PMC_DELETE_ON_CLOSE     55	/* Delete cached copy on close   */
#define PMC_RETRIEVE_FAILED     56      /* Unable to retrieve file       */

/* mk_vdir */
/* #define MKVD_ALREADY_EXISTS     61	/* Directory already exists      */
/* #define MKVD_NAME_CONFLICT	62	/* Link with name already exists */

/* vfsetenv */
#define VFSN_NOT_A_VS		65	/* Not a virtual system          */
#define VFSN_CANT_FIND_DIR	66	/* Not a virtual system          */

/* add_vlink */
/* #define ADDVL_ALREADY_EXISTS    71	/* Directory already exists      */
/* #define ADDVL_NAME_CONFLICT	72	/* Link with name already exists */

/* Local error codes on server */

/* dsrdir */
#define DSRDIR_NOT_A_DIRECTORY 111	/* Not a directory name		*/
/* dsrfinfo */
#define DSRFINFO_NOT_A_FILE    121      /* Object not found             */
#define DSRFINFO_FORWARDED     122      /* Object has moved             */

/* Error codes that may be returned by various procedures               */
#define PFS_FILE_NOT_FOUND     230      /* File not found               */
#define PFS_DIR_NOT_FOUND      231      /* Directory in path not found  */
#define PFS_SYMLINK_DEPTH      232	/* Max sym-link depth exceeded  */
#define PFS_ENV_NOT_INITIALIZED	233	/* Can't read environment	*/
#define PFS_EXT_USED_AS_DIR    234	/* Can't use externals as dirs  */
#define PFS_MAX_FWD_DEPTH      235	/* Exceeded max forward depth   */

/* Error codes returned by directory server                    */
/* some of these duplicate errors from individual routines     */
/* some of those error codes should be eliminated              */
#define DIRSRV_AUTHENT_REQ     242      /* Authentication required       */
#define DIRSRV_NOT_AUTHORIZED  243      /* Not authorized                */
#define DIRSRV_NOT_FOUND       244      /* Not found                     */
#define DIRSRV_BAD_VERS        245
#define DIRSRV_NOT_DIRECTORY   246
#define DIRSRV_ALREADY_EXISTS  247	/* Identical link already exists */
#define DIRSRV_NAME_CONFLICT   248	/* Link with name already exists */

#define DIRSRV_UNIMPLEMENTED   251      /* Unimplemented command         */
#define DIRSRV_BAD_FORMAT      252
#define DIRSRV_ERROR           253
#define DIRSRV_SERVER_FAILED   254      /* Unspecified server failure    */

#ifndef PFAILURE
#define	PFAILURE 	       255
#endif

/* Warning codes */

#define PNOWARN			 0	/* No warning indicated		 */
#define PWARN_OUT_OF_DATE	 1	/* Software is out of date       */
#define PWARN_MSG_FROM_SERVER	 2      /* Warning in p_warn_string      */
#define PWARN_UNRECOGNIZED_RESP  3	/* Unrecognized line in response */
#define PWARNING	       255	/* Warning in p_warn_string      */
$EOD
$!
$CREATE [.archie]pfs.h
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

#ifdef VMS
# include <vms.h>
#else /* not VMS */
# ifndef _TYPES_
#  include <sys/types.h>
# endif /* _TYPES_ */
# ifndef IN_H
#  include <netinet/in.h> 
#  define IN_H
# endif
#endif /* VMS */

#ifndef NULL
# ifdef MSDOS
#  include <stdio.h>
# else
#  define NULL 0
# endif /* MSDOS */
#endif /* NULL */

#define		PFS_RELEASE	"Beta.4.2E"
#define		PFS_SW_ID	"B42E"

/* moved up for vdir_init */
#define ZERO(p)		bzero((char *)(p), sizeof(*(p)))

/* General Definitions */

#define		MAX_PTXT_LEN	1250     /* Max length of PTEXT structure   */
#define		MAX_PTXT_HDR	32       /* Max offset for start            */
#define		P_ERR_STRING_SZ 100	 /* Size of error string	    */
#define		MAX_VPATH	1024	 /* Max length of virtual pathname  */

/* Definition of text structure used to pass text around */

struct ptext {
    int			length;		  /* Length of text (from start)    */
    char		*start;		  /* Start of text		    */
    char		dat[MAX_PTXT_LEN+2*MAX_PTXT_HDR];/* The data itself */
    unsigned long 	mbz;		  /* ZERO to catch runaway strings  */
    struct ptext	*previous;        /* Previous element in list       */
    struct ptext	*next;		  /* Next element in linked list    */
    int			seq;		  /* Sequence Number		    */
};

typedef struct ptext *PTEXT;
typedef struct ptext PTEXT_ST;

/* Request structure: maintains information about server requests */
struct preq {
    int			cid;		  /* Connection ID                  */
    short		priority;	  /* Connection priority            */
    int			pf_priority;	  /* Priority assigned by pri_func  */
    int			recv_tot;	  /* Total # of packets received    */
    int			trns_tot;	  /* Total # of packets to transmit */
    struct ptext	*cpkt;		  /* Current packet being filled in */
    struct ptext	*recv;		  /* Received packets               */
    struct ptext	*trns;		  /* Transmitted packets            */
    int			rcvd_thru;	  /* Received all packets through # */
    struct preq		*previous;        /* Previous element in list       */
    struct preq		*next;		  /* Next element in linked list    */
    struct sockaddr_in	fromto; 	  /* Sender/Destination		    */
};

typedef struct preq *PREQ;
typedef struct preq PREQ_ST;


/* Definition of structure containing information on virtual link */

struct vlink {
    int			dontfree;	  /* Flag: don't free this link     */
    char		*name;		  /* Component of path name	    */
    char		linktype;	  /* L = Link, U = Union, N= Native */
    int			expanded;	  /* Has a union link been expanded */
    char		*type;            /* Type of object pointed to      */
    struct vlink	*filters;	  /* Filters associated with link   */
    struct vlink	*replicas;	  /* Replicas (* see comment below) */
    char		*hosttype;	  /* Type of hostname		    */
    char		*host;		  /* Files physical location	    */
    char		*nametype;	  /* Type of filename		    */
    char		*filename;	  /* System level filename	    */
    long		version;	  /* Version number of destination  */
    long		f_magic_no;	  /* File's magic number	    */
    struct acl		*acl;		  /* ACL for link		    */
    long		dest_exp;	  /* Expiration for dest of link    */
    long		link_exp;	  /* Expiration of link itself      */
    char		*args;		  /* Arguments if this is a filter  */
    struct pattrib	*lattrib;	  /* Attributes associated w/ link  */
    struct pfile	*f_info;	  /* Info to be assoicated w/ file  */
    struct vlink	*previous;        /* Previous elt in linked list    */
    struct vlink	*next;		  /* Next element in linked list    */
};

typedef struct vlink *VLINK;
typedef struct vlink VLINK_ST;

/* * Note that vlink->replicas is not really a list of replicas of the  */
/*   object.  Instead, it is a list of the objects returned during name */
/*   resolution that share the same name as the current object.  Such   */
/*   an object should only be considered a replica if it also shares    */
/*   the same non-zero magic number.                                    */

/* Definition of structure continiaing virtual directory information    */

struct vdir {
    int			version;	  /* Version of local directory fmt  */
    int			inc_native;	  /* Include the native directory    */
    long		magic_no;	  /* Magic number of current file    */
    struct acl		*dacl;            /* Default acl for links in dir    */
    struct pfile	*f_info;	  /* Directory file info             */
    struct vlink	*links;		  /* The directory entries	     */
    struct vlink	*lastlink;	  /* Last directory entry            */
    struct vlink	*ulinks;	  /* The entries for union links     */
    struct vdir		*previous;        /* Previous element in linked list */
    struct vdir		*next;		  /* Next element in linked list     */
};

typedef struct vdir *PVDIR;
typedef struct vdir VDIR_ST;

/* Initialize directory */
#define vdir_init(dir)  ZERO(dir)
/* XXX: was

  dir->version = 0;     dir->inc_native = 0; \
  dir->magic_no = 0;    dir->f_info = NULL; \
  dir->links = NULL;    dir->lastlink = NULL; \
  dir->ulinks = NULL;   dir->dacl = NULL; \
  dir->previous = NULL; dir->next = NULL;
*/

#define vdir_copy(d1,d2) d2->version = d1->version; \
                         d2->inc_native = d1->inc_native; \
                         d2->magic_no = d1->magic_no; \
    			 d2->f_info = d1->f_info; \
                         d2->links = d1->links; \
                         d2->lastlink = d1->lastlink; \
                         d2->ulinks = d1->ulinks; \
                         d2->dacl = d1->dacl; \
                         d2->previous = d1->previous; \
                         d2->next = d1->next; 
                         
/* Values of ->inc_native in vdir structure */
#define VDIN_REALONLY	-1   /* Include native files, but not . and ..       */
#define VDIN_NONATIVE	 0   /* Do not include files from native directory   */
#define VDIN_INCLNATIVE	 1   /* Include files from native directory          */
#define VDIN_NATIVEONLY  2   /* All entries in directory are from native dir */
#define VDIN_PSEUDO      3   /* Directory is not real                        */


/* Definition of structure containing information on a specific file */

union avalue {
    char		*ascii;		/* Character string                */
    struct vlink	*link;		/* A link			   */
};


struct pattrib {
    char		precedence;	/* Precedence for link attribute   */
    char		*aname;		/* Name of the attribute           */
    char		*avtype;	/* Type of the attribute value     */
    union avalue	value;		/* Attribute Value                 */
    struct pattrib	*previous;      /* Previous element in linked list */
    struct pattrib	*next;		/* Next element in linked list     */
};

typedef struct pattrib *PATTRIB;
typedef struct pattrib PATTRIB_ST;

#define 	ATR_PREC_OBJECT  'O'   /* Authoritative answer for object */
#define 	ATR_PREC_LINK    'L'   /* Authoritative answer for link   */
#define 	ATR_PREC_CACHED  'C'   /* Object info cached w/ link      */
#define 	ATR_PREC_REPLACE 'R'   /* From link (replaces O)          */
#define 	ATR_PREC_ADD     'A'   /* From link (additional value)    */

/* **** Incomplete **** */
struct pfile {
    int			version;	  /* Version of local finfo format   */
    long		f_magic_no;	  /* Magic number of current file    */
    long		exp;		  /* Expiration date of timeout      */
    long		ttl;		  /* Time to live after reference    */
    long		last_ref;	  /* Time of last reference          */
    struct vlink	*forward;	  /* List of forwarding pointers     */
    struct vlink	*backlinks;	  /* Partial list of back links      */
    struct pattrib	*attributes;	  /* List of file attributes         */
    struct pfile	*previous;        /* Previous element in linked list */
    struct pfile	*next;		  /* Next element in linked list     */
};

typedef struct pfile *PFILE;
typedef struct pfile PFILE_ST;

/* Definition of structure contining an access control list entry */

struct acl {
    int			acetype;	  /* Access Contol Entry type       */
    char		*atype;           /* Authentication type            */
    char		*rights;          /* Rights                         */
    char		*principals;      /* Authorized principals          */
    struct restrict     *restrictions;    /* Restrictions on use            */
    struct acl		*previous;        /* Previous elt in linked list    */
    struct acl		*next;		  /* Next element in linked list    */
};
typedef struct acl *ACL;
typedef struct acl ACL_ST;

#define ACL_NONE		0         /* Nobody authorized by ths entry */
#define ACL_DEFAULT		1         /* System default                 */
#define ACL_SYSTEM		2         /* System administrator           */
#define ACL_OWNER               3         /* Directory owner                */
#define ACL_DIRECTORY           4         /* Same as directory              */
#define ACL_ANY                 5         /* Any user                       */
#define ACL_AUTHENT             6         /* Authenticated principal        */
#define ACL_LGROUP              7         /* Local group                    */
#define ACL_GROUP               8         /* External group                 */
#define ACL_ASRTHOST            10        /* Check host and asserted userid */
#define ACL_TRSTHOST            11        /* ASRTHOST from privileged port  */


/* Definition of structure contining access restrictions */
/* for future extensions                                 */
struct restrict {
    struct acl		*previous;        /* Previous elt in linked list    */
    struct acl		*next;		  /* Next element in linked list    */
};

/* Definitions for send_to_dirsrv */
#define	CLIENT_DIRSRV_TIMEOUT		4	/* time between retries      */
#define CLIENT_DIRSRV_BACKOFF(x)  (2 * x)	/* Backoff algorithm         */
#define CLIENT_DIRSRV_RETRY		3	/* retry this many times     */

/* Definitions for rd_vlink and rd_vdir */
#define		SYMLINK_NESTING 10       /* Max nesting depth for sym links */

/* Definition fo check_acl */
#define		ACL_NESTING     10       /* Max depth for ACL group nesting */

/* Flags for mk_vdir */
#define	     MKVD_LPRIV     1   /* Minimize privs for creator in new ACL    */

/* Flags for get_vdir */
#define	     GVD_UNION      0	/* Do not expand union links 		     */
#define      GVD_EXPAND     1   /* Expand union links locally		     */
#define	     GVD_LREMEXP    3   /* Request remote expansion of local links   */
#define	     GVD_REMEXP     7   /* Request remote expansion of all links     */
#define	     GVD_VERIFY     8	/* Only verify args are for a directory      */
#define      GVD_FIND	   16   /* Stop expanding when match is found        */
#define	     GVD_ATTRIB    32   /* Request attributes from remote server     */
#define	     GVD_NOSORT	   64   /* Do not sort links when adding to dir      */

/* Flags for rd_vdir */
#define	     RVD_UNION      GVD_UNION
#define	     RVD_EXPAND     GVD_EXPAND 
#define	     RVD_LREMEXP    GVD_LREMEXP
#define	     RVD_REMEXP     GVD_REMEXP
#define	     RVD_DFILE_ONLY GVD_VERIFY /* Only return ptr to dir file        */
#define      RVD_FIND       GVD_FIND   
#define      RVD_ATTRIB     GVD_ATTRIB
#define	     RVD_NOSORT	    GVD_NOSORT
#define	     RVD_NOCACHE    128

/* Flags for add_vlink */
#define	     AVL_UNION      1   /* Link is a union link                      */

/* Flags for vl_insert */
#define	     VLI_NOCONFLICT 0   /* Do not insert links w/ conflicting names  */
#define      VLI_ALLOW_CONF 1   /* Allow links with conflicting names        */
#define	     VLI_NOSORT     2   /* Allow conflicts and don't sort            */

/* Flags for mapname */
#define      MAP_READWRITE  0   /* Named file to be read and written         */
#define	     MAP_READONLY   1   /* Named file to be read only                */

/* Flags for modify_acl */
#define	     MACL_NOSYSTEM   0x01
#define      MACL_NOSELF     0x02
#define      MACL_DEFAULT    0x08
#define      MACL_SET        0x0C
#define      MACL_INSERT     0x14
#define      MACL_DELETE     0x10
#define      MACL_ADD        0x1C
#define      MACL_SUBTRACT   0x18
#define      MACL_LINK       0x00
#define      MACL_DIRECTORY  0x20
#define      MACL_OBJECT     0x60
#define      MACL_INCLUDE    0x40

#define      MACL_OP    (MACL_DEFAULT|MACL_SET|MACL_INSERT|\
			 MACL_DELETE|MACL_ADD|MACL_SUBTRACT)

#define      MACL_OTYPE (MACL_LINK|MACL_DIRECTORY|MACL_OBJECT|MACL_INCLUDE)

/* Flags for dsrdir */
#define DSRD_ATTRIBUTES                      0x1 /* Fill in attributes for links */

/* Access methods returned by Pget_am */
#define P_AM_ERROR			0
#define P_AM_FTP			1
#define P_AM_AFTP			2  /* Anonymous FTP  */
#define P_AM_NFS			4
#define P_AM_KNFS			8  /* Kerberized NFS */
#define P_AM_AFS		       16

/* Return codes */

#define		PSUCCESS	0
#define		PFAILURE	255

/* Hush up warnings.  */
void vllfree();

/* Procedures in libpfs.a */

char *pget_wdhost(), *pget_wdfile(), *pget_wd(), *pget_hdhost();
char *pget_hdfile(), *pget_hd(), *pget_rdhost(), *pget_rdfile();
char *pget_dhost(), *pget_dfile(), *pget_vsname(), *nlsindex();
char *sindex(), *strtok(), *nxtline(), *unquote(), *stcopy();
char *stcopyr(), *readheader(), *month_sname();

long		asntotime();
void		procquery();

PTEXT		ptalloc();
PTEXT		dirsend();
void		ptfree();
void		ptlfree();

PREQ		pralloc();
PREQ		get_next_request();

VLINK		rd_slink();
VLINK		rd_vlink();
VLINK		vl_delete();
VLINK		vlalloc();
VLINK		vlcopy();
void		vlfree();

PFILE		pfalloc();

PATTRIB         parse_attribute();
PATTRIB         atalloc();
PATTRIB 	pget_at();
void		atfree();
void		atlfree();

ACL             acalloc();
ACL             get_acl();

void		stfree();

/* Miscellaneous useful definitions */
#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#define AUTHORIZED      1
#define NOT_AUTHORIZED  0
#define NEG_AUTHORIZED  -1

#ifndef NULL
#define NULL		0
#endif

#define FAILED		-1
$EOD
$!
$CREATE [.archie]pmachine.h
$DECK
/*
 * Miscellaneous system dependencies.
 *
 * I kept the name pmachine.h because it was already in all of the files...this
 * barely resembles the pmachine.h that comes with the real Prospero, tho.
 */

#ifdef u3b2
# define USG
# define NOREGEX
# define MAXPATHLEN 1024	/* There's no maxpathlen in any 3b2 .h file.  */
#endif

#ifdef hpux
# ifndef bcopy
#  define FUNCS			/* HP/UX 8.0 has the fns.  */
# endif
# define NOREGEX
# define NEED_STRING_H
#endif

#if defined(USG) || defined(SYSV)
# define FUNCS
#endif

#ifdef ISC
# define FUNCS
# define STRSPN
# define NOREGEX
#endif

#ifdef CUTCP
# define FUNCS
# define NOREGEX
# define NEED_STRING_H
# define SELECTARG int
# define MSDOS
#endif

#ifdef _AUX_SOURCE
# define AUX
# define NOREGEX
# define NBBY 8	/* Number of bits in a byte.  */
# define long Fd_mask;
# define NFDBITS (sizeof(Fd_mask) * NBBY)	/* bits per mask */
#endif

#ifdef OS2
# define NOREGEX
# include <pctcp.h>
#endif
#ifdef MSDOS
# define USG
# define NOREGEX
# include <string.h>
# include <stdlib.h>
#endif

#ifdef _AIX
# ifdef u370
#  define FUNCS
# endif /* AIX/370 */
# define _NONSTD_TYPES
# define _BSD_INCLUDES
# define NEED_STRING_H
# define NEED_SELECT_H
# define NEED_TIME_H
#endif

/* ==== */
#ifdef FUNCS
# define index		strchr
/* According to mycroft. */
# ifdef _IBMR2
char *strchr();
# endif
# define rindex		strrchr
# ifndef _AUX_SOURCE
#  define bcopy(a,b,n)	memcpy(b,a,n)
#  define bzero(a,n)	memset(a,0,n)
# ifdef _IBMR2
char *memset();
# endif
# endif
#endif

#if defined(_IBMR2) || defined(_BULL_SOURCE)
# define NEED_SELECT_H
#endif
#if defined(USG) || defined(UTS)
# define NEED_STRING_H
#endif
#if defined(USG) || defined(UTS) || defined(_AUX_SOURCE)
# define NEED_TIME_H
# ifdef UTS
#  define WANT_BOTH_TIME
# endif
#endif

#ifdef VMS
/* Get the system status stuff.  */
# include <ssdef.h>
#endif /* VMS */

/*
 * FD_SET: lib/pfs/dirsend.c, user/vget/ftp.c
 */
#ifndef CUTCP

#define SELECTARG fd_set
#if !defined(FD_SET) && !defined(VMS) && !defined(NEED_SELECT_H)
#define	FD_SETSIZE	32
#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif

#endif /* not CUTCP */
$EOD
$!
$CREATE [.archie]pprot.h
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

#ifndef MAXPATHLEN
# ifdef VMS
#  define MAXPATHLEN 32
# else /* not VMS */
#  if defined(MSDOS) && !defined(OS2)
#   define MAXPATHLEN 255
#  else /* not MSDOS */
#   ifdef ISC
#    define MAXPATHLEN 512
#   else /* not Interactive..normal! (gasp) */
#    include <sys/param.h>
#   endif /* ISC */
#  endif /* MSDOS && !OS2 */
# endif /* VMS */
#endif

/* Protocol Definitions */

#define	       VFPROT_VNO	1      /* Protocol Version Number           */

#define	       DIRSRV_PORT      1525   /* Server port used if not in srvtab */
#define        PROSPERO_PORT	191    /* Officially assigned prived port   */
#define	       PROS_FIRST_PRIVP 901    /* First privileged port to try      */
#define        PROS_NUM_PRIVP   20     /* Number of privileged ports to try */

#define	       MAXPKT	        1024   /* Max size of response from server  */
#define	       SEQ_SIZE		32     /* Max size of sequence text in resp */ 
#define	       MAX_DIR_LINESIZE 160+MAXPATHLEN /* Max linesize in directory */

#define	       MAX_FWD_DEPTH    20     /* Max fwd pointers to follow        */

#define S_AD_SZ		sizeof(struct sockaddr_in)

/* Replacement for strtok that doesn't keep state.  Both the variable  */
/* S and the variable S_next must be defined.  To initialize, assign   */
/* the string to be stepped through to S_next, then call get_token on  */
/* S.  The first token will be in S, and S_next will point to the next */
/* token.  Like strtok, this macro does modify the string passed to it */
#ifdef __GNUC__
#define get_token(S,C) \
  do { \
    S = S##_next; \
    if(S) { \
     while(*S == C) S++; \
     S##_next = index(S,C); \
     if(S##_next) *(S##_next++) = '\0'; \
     if(!*S) S = NULL; \
    } \
  } while (0)
#else
#define get_token(S,C) \
    S = S/**/_next; \
  do { \
    if(S) { \
     while(*S == C) S++; \
     S/**/_next = index(S,C); \
     if(S/**/_next) *(S/**/_next++) = '\0'; \
     if(!*S) S = NULL; \
    } \
  } while (0)
#endif /* __GNUC__ */
$EOD
$!
$CREATE [.archie]procquery.c
$DECK
/*
 * procquery.c : Routines for processing results from Archie
 *
 * Originally part of the Prospero Archie client by Cliff Neuman (bcn@isi.edu).
 * Modified by Brendan Kehoe (brendan@cs.widener.edu).
 * Re-modified by George Ferguson (ferguson@cs.rochester.edu).
 *
 * Copyright (c) 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 *
 */

#include <copyright.h>
#include <stdio.h>
#include <pfs.h>
#include <perrno.h>
#include <pmachine.h>
#include <archie.h>

#ifdef NEED_TIME_H
# include <time.h>
#else
# ifndef VMS
#  include <sys/time.h>
# endif
#endif

extern int client_dirsrv_timeout,client_dirsrv_retry;	/* dirsend.c */
extern char *progname;


/*
 * Functions defined here
 */
void display_link(), procquery();

/*
 * Data defined here
 */
extern int pwarn, perrno;
#ifdef DEBUG
int pfs_debug;
#endif
static struct tm *presenttime;
static char lastpath[MAX_VPATH] = "\001";
static char lasthost[MAX_VPATH] = "\001";

/*	-	-	-	-	-	-	-	-	*/
/*
 * display_link : Prints the contents of the given virtual link. If
 *	listflag is 0, then this uses last{host,path} to save state
 *	between calls for a less verbose output. If listflag is non-zero
 *	then all information is printed every time.
 */
void
display_link(l,listflag)
VLINK l;
int listflag;
{
    PATTRIB 	ap;
    char	linkpath[MAX_VPATH];
    int		dirflag = 0;
#ifdef MSDOS
    unsigned long size = 0L;
#else
    int		size = 0;
#endif
    char	*modes = "";
    char	archie_date[20];
    char	*gt_date = "";
    int		gt_year = 0;
    int		gt_mon = 0;
    int		gt_day = 0;
    int		gt_hour = 0;
    int		gt_min = 0;
    
    /* Initialize local buffers */
    *archie_date = '\0';

    /* Remember if we're looking at a directory */
    if (sindex(l->type,"DIRECTORY"))
	dirflag = 1;
    else
	dirflag = 0;
    
    /* Extract the linkpath from the filename */
    strcpy(linkpath,l->filename);
    *(linkpath + (strlen(linkpath) - strlen(l->name) - 1)) = '\0';
    
    /* Is this a new host? */
    if (strcmp(l->host,lasthost) != 0) {
	if (!listflag)
	    printf("\nHost %s\n\n",l->host);
	strcpy(lasthost,l->host);
	*lastpath = '\001';
    }
    
    /* Is this a new linkpath (location)? */
    if(strcmp(linkpath,lastpath) != 0) {
	if (!listflag)
	    printf("    Location: %s\n",(*linkpath ? linkpath : "/"));
	strcpy(lastpath,linkpath);
    }
    
    /* Parse the attibutes of this link */
    for (ap = l->lattrib; ap; ap = ap->next) {
	if (strcmp(ap->aname,"SIZE") == 0) {
#ifdef MSDOS
	    sscanf(ap->value.ascii,"%lu",&size);
#else
	    sscanf(ap->value.ascii,"%d",&size);
#endif
	} else if(strcmp(ap->aname,"UNIX-MODES") == 0) {
	    modes = ap->value.ascii;
	} else if(strcmp(ap->aname,"LAST-MODIFIED") == 0) {
	    gt_date = ap->value.ascii;
	    sscanf(gt_date,"%4d%2d%2d%2d%2d",&gt_year,
		   &gt_mon, &gt_day, &gt_hour, &gt_min);
	    if ((12 * (presenttime->tm_year + 1900 - gt_year) + 
					presenttime->tm_mon - gt_mon) > 6) 
		sprintf(archie_date,"%s %2d %4d",month_sname(gt_mon),
			gt_day, gt_year);
	    else
		sprintf(archie_date,"%s %2d %02d:%02d",month_sname(gt_mon),
			 gt_day, gt_hour, gt_min);
	}
    }
    
    /* Print this link's information */
    if (listflag)
#if defined(MSDOS)
	printf("%s %6lu %s %s%s\n",gt_date,size,l->host,l->filename,
	       (dirflag ? "/" : ""));
#else
	printf("%s %6d %s %s%s\n",gt_date,size,l->host,l->filename,
	       (dirflag ? "/" : ""));
#endif
    else
#ifdef MSDOS
	printf("      %9s %s %10lu  %s  %s\n",(dirflag ? "DIRECTORY" : "FILE"),
					modes,size,archie_date,l->name);
#else
	printf("      %9s %s %10d  %s  %s\n",(dirflag ? "DIRECTORY" : "FILE"),
					modes,size,archie_date,l->name);
#endif /* MSDOS */


    /* Free the attibutes */
    atlfree(l->lattrib);
    l->lattrib = NULL;
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * procquery : Process the given query and display the results. If
 *	sortflag is non-zero, then the results are sorted by increasing
 *	date, else by host/filename. If listflag is non-zero then each
 *	entry is printed on a separate, complete line. Note that listflag
 *	is ignored by xarchie.
 */
void
procquery(host,str,max_hits,offset,query_type,sortflag,listflag)
char *host,*str;
int max_hits,offset;
char query_type;
int sortflag,listflag;
{
    VLINK l;
    long now;
    extern int rdgram_priority;

    /* initialize data structures for this query */
    (void)time(&now);
    presenttime = localtime(&now);

    /* Do the query */
    if (sortflag == 1)
	l = archie_query(host,str,max_hits,offset,query_type,AQ_INVDATECMP,0);
    else
	l = archie_query(host,str,max_hits,offset,query_type,NULL,0);

    /* Error? */
    if (perrno != PSUCCESS) {
	if (p_err_text[perrno]) {
	    if (*p_err_string)
		fprintf(stderr, "%s: failed: %s - %s\n", progname,
		        p_err_text[perrno], p_err_string);
	    else
	        fprintf(stderr, "%s failed: %s\n", progname, p_err_text[perrno]);
	} else
	    fprintf(stderr, "%s failed: Undefined error %d (prospero)", perrno);
    }

    /* Warning? */
    if (pwarn != PNOWARN) {
	if (*p_warn_string)
	    fprintf(stderr, "%s: Warning! %s - %s\n", progname,
		p_warn_text[pwarn], p_warn_string);
	else
	    fprintf(stderr, "%s: Warning! %s\n", progname, p_warn_text[pwarn]);
    }


    /* Display the results */

    if (l == (VLINK)NULL && pwarn == PNOWARN && perrno == PSUCCESS) {
	if (! listflag) puts ("No matches.");
#ifdef CUTCP
	netshut();
#endif
	exit (1);
    }

    *lasthost = '\001';
    *lastpath = '\001';
    while (l != NULL) {
	display_link(l,listflag);
	l = l->next;
    }
}
$EOD
$!
$CREATE [.archie]ptalloc.c
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <stdio.h>

#include <pfs.h>
#include <pmachine.h> /* for correct definition of ZERO */
#ifdef MSDOS
# define free _pfree   /* otherwise we get conflicts with free() */
#endif

static PTEXT	free = NULL;
int 		ptext_count = 0;
int		ptext_max = 0;

/*
 * ptalloc - allocate and initialize ptext structure
 *
 *    PTALLOC returns a pointer to an initialized structure of type
 *    PTEXT.  If it is unable to allocate such a structure, it
 *    returns NULL.
 */
PTEXT
ptalloc()
    {
	PTEXT	vt;
	if(free) {
	    vt = free;
	    free = free->next;
	}
	else {
	    vt = (PTEXT) malloc(sizeof(PTEXT_ST));
	    if (!vt) return(NULL);
	    ptext_max++;
	}
	ptext_count++;

	/* nearly all parts are 0 [or NULL] */
	ZERO(vt);
	/* The offset is to leave room for additional headers */
	vt->start = vt->dat + MAX_PTXT_HDR;

	return(vt);
    }

/*
 * ptfree - free a VTEXT structure
 *
 *    VTFREE takes a pointer to a VTEXT structure and adds it to
 *    the free list for later reuse.
 */
void
ptfree(vt)
    PTEXT	vt;
    {
	vt->next = free;
	vt->previous = NULL;
	free = vt;
	ptext_count--;
    }

/*
 * ptlfree - free a VTEXT structure
 *
 *    VTLFREE takes a pointer to a VTEXT structure frees it and any linked
 *    VTEXT structures.  It is used to free an entrie list of VTEXT
 *    structures.
 */
void
ptlfree(vt)
    PTEXT	vt;
    {
	PTEXT	nxt;

	while(vt != NULL) {
	    nxt = vt->next;
	    ptfree(vt);
	    vt = nxt;
	}
    }

$EOD
$!
$CREATE [.archie]rdgram.h
$DECK
/*
 * Copyright (c) 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>

/* Queuing priorities for datagrams */
#define	       RDGRAM_MAX_PRI   32765  /* Maximum user proiority          */
#define	       RDGRAM_MAX_SPRI  32767  /* Maximum priority for system use */
#define	       RDGRAM_MIN_PRI  -32765  /* Maximum user proiority          */
#define	       RDGRAM_MIN_SPRI -32768  /* Maximum priority for system use */

int	rdgram_priority = 0;
$EOD
$!
$CREATE [.archie]regex.c
$DECK
#include <pmachine.h>

#ifdef NOREGEX
/*
 * These routines are BSD regex(3)/ed(1) compatible regular-expression
 * routines written by Ozan S. Yigit, Computer Science, York University.
 * Parts of the code that are not needed by Prospero have been removed,
 * but most of the accompanying information has been left intact. 
 * This file is to be included on those operating systems that do not
 * support re_comp and re_exec.
 */

/*
 * regex - Regular expression pattern matching
 *         and replacement
 *
 * by:  Ozan S. Yigit (oz@nexus.yorku.ca)
 *	Dept. of Computing Services
 *      York University
 *
 * These routines are the PUBLIC DOMAIN equivalents 
 * of regex routines as found in 4.nBSD UN*X, with minor
 * extensions.
 *
 * Modification history:
 *
 * $Log:	regex.c,v $
 * Revision 1.3  89/04/01  14:18:09  oz
 * Change all references to a dfa: this is actually an nfa.
 * 
 * Revision 1.2  88/08/28  15:36:04  oz
 * Use a complement bitmap to represent NCL.
 * This removes the need to have seperate 
 * code in the pmatch case block - it is 
 * just CCL code now.
 * 
 * Use the actual CCL code in the CLO
 * section of pmatch. No need for a recursive
 * pmatch call.
 * 
 * Use a bitmap table to set char bits in an
 * 8-bit chunk.
 * 
 * Routines:
 *      re_comp:        compile a regular expression into
 *                      a NFA.
 *
 *			char *re_comp(s)
 *			char *s;
 *
 *      re_exec:        execute the NFA to match a pattern.
 *
 *			int re_exec(s)
 *			char *s;
 *
 * Regular Expressions:
 *
 *      [1]     char    matches itself, unless it is a special
 *                      character (metachar): . \ [ ] * + ^ $
 *
 *      [2]     .       matches any character.
 *
 *      [3]     \       matches the character following it, except
 *			when followed by a left or right round bracket,
 *			a digit 1 to 9 or a left or right angle bracket. 
 *			(see [7], [8] and [9])
 *			It is used as an escape character for all 
 *			other meta-characters, and itself. When used
 *			in a set ([4]), it is treated as an ordinary
 *			character.
 *
 *      [4]     [set]   matches one of the characters in the set.
 *                      If the first character in the set is "^",
 *                      it matches a character NOT in the set, i.e. 
 *			complements the set. A shorthand S-E is 
 *			used to specify a set of characters S upto 
 *			E, inclusive. The special characters "]" and 
 *			"-" have no special meaning if they appear 
 *			as the first chars in the set.
 *                      examples:        match:
 *
 *                              [a-z]    any lowercase alpha
 *
 *                              [^]-]    any char except ] and -
 *
 *                              [^A-Z]   any char except uppercase
 *                                       alpha
 *
 *                              [a-zA-Z] any alpha
 *
 *      [5]     *       any regular expression form [1] to [4], followed by
 *                      closure char (*) matches zero or more matches of
 *                      that form.
 *
 *      [6]     +       same as [5], except it matches one or more.
 *
 *      [7]             a regular expression in the form [1] to [10], enclosed
 *                      as \(form\) matches what form matches. The enclosure
 *                      creates a set of tags, used for [8] and for
 *                      pattern substution. The tagged forms are numbered
 *			starting from 1.
 *
 *      [8]             a \ followed by a digit 1 to 9 matches whatever a
 *                      previously tagged regular expression ([7]) matched.
 *
 *	[9]	\<	a regular expression starting with a \< construct
 *		\>	and/or ending with a \> construct, restricts the
 *			pattern matching to the beginning of a word, and/or
 *			the end of a word. A word is defined to be a character
 *			string beginning and/or ending with the characters
 *			A-Z a-z 0-9 and _. It must also be preceded and/or
 *			followed by any character outside those mentioned.
 *
 *      [10]            a composite regular expression xy where x and y
 *                      are in the form [1] to [10] matches the longest
 *                      match of x followed by a match for y.
 *
 *      [11]	^	a regular expression starting with a ^ character
 *		$	and/or ending with a $ character, restricts the
 *                      pattern matching to the beginning of the line,
 *                      or the end of line. [anchors] Elsewhere in the
 *			pattern, ^ and $ are treated as ordinary characters.
 *
 *
 * Acknowledgements:
 *
 *	HCR's Hugh Redelmeier has been most helpful in various
 *	stages of development. He convinced me to include BOW
 *	and EOW constructs, originally invented by Rob Pike at
 *	the University of Toronto.
 *
 * References:
 *              Software tools			Kernighan & Plauger
 *              Software tools in Pascal        Kernighan & Plauger
 *              Grep [rsx-11 C dist]            David Conroy
 *		ed - text editor		Un*x Programmer's Manual
 *		Advanced editing on Un*x	B. W. Kernighan
 *		regexp routines			Henry Spencer
 *
 * Notes:
 *
 *	This implementation uses a bit-set representation for character
 *	classes for speed and compactness. Each character is represented 
 *	by one bit in a 128-bit block. Thus, CCL always takes a 
 *	constant 16 bytes in the internal nfa, and re_exec does a single
 *	bit comparison to locate the character in the set.
 *
 * Examples:
 *
 *	pattern:	foo*.*
 *	compile:	CHR f CHR o CLO CHR o END CLO ANY END END
 *	matches:	fo foo fooo foobar fobar foxx ...
 *
 *	pattern:	fo[ob]a[rz]	
 *	compile:	CHR f CHR o CCL bitset CHR a CCL bitset END
 *	matches:	fobar fooar fobaz fooaz
 *
 *	pattern:	foo\\+
 *	compile:	CHR f CHR o CHR o CHR \ CLO CHR \ END END
 *	matches:	foo\ foo\\ foo\\\  ...
 *
 *	pattern:	\(foo\)[1-3]\1	(same as foo[1-3]foo)
 *	compile:	BOT 1 CHR f CHR o CHR o EOT 1 CCL bitset REF 1 END
 *	matches:	foo1foo foo2foo foo3foo
 *
 *	pattern:	\(fo.*\)-\1
 *	compile:	BOT 1 CHR f CHR o CLO ANY END EOT 1 CHR - REF 1 END
 *	matches:	foo-foo fo-fo fob-fob foobar-foobar ...
 * 
 */

#define MAXNFA  1024
#define MAXTAG  10

#define OKP     1
#define NOP     0

#define CHR     1
#define ANY     2
#define CCL     3
#define BOL     4
#define EOL     5
#define BOT     6
#define EOT     7
#define BOW	8
#define EOW	9
#define REF     10
#define CLO     11

#define END     0

/*
 * The following defines are not meant
 * to be changeable. They are for readability
 * only.
 *
 */
#define MAXCHR	128
#define CHRBIT	8
#define BITBLK	MAXCHR/CHRBIT
#define BLKIND	0170
#define BITIND	07

#define ASCIIB	0177

typedef /*unsigned*/ char CHAR;

static int  tagstk[MAXTAG];             /* subpat tag stack..*/
static CHAR nfa[MAXNFA];		/* automaton..       */
static int  sta = NOP;               	/* status of lastpat */

static CHAR bittab[BITBLK];		/* bit table for CCL */
					/* pre-set bits...   */
static CHAR bitarr[] = {1,2,4,8,16,32,64,128};

static int internal_error;

static void
chset(c)
register CHAR c;
{
	bittab[((c) & BLKIND) >> 3] |= bitarr[(c) & BITIND];
}

#define badpat(x)	return (*nfa = END, x)
#define store(x)	*mp++ = x
 
char *     
re_comp(pat)
char *pat;
{
	register char *p;               /* pattern pointer   */
	register CHAR *mp = nfa;        /* nfa pointer       */
	register CHAR *lp;              /* saved pointer..   */
	register CHAR *sp = nfa;        /* another one..     */

	register int tagi = 0;          /* tag stack index   */
	register int tagc = 1;          /* actual tag count  */

	register int n;
	register CHAR mask;		/* xor mask -CCL/NCL */
	int c1, c2;
		
	if (!pat || !*pat)
		if (sta)
			return 0;
		else
			badpat("No previous regular expression");
	sta = NOP;

	for (p = pat; *p; p++) {
		lp = mp;
		switch(*p) {

		case '.':               /* match any char..  */
			store(ANY);
			break;

		case '^':               /* match beginning.. */
			if (p == pat)
				store(BOL);
			else {
				store(CHR);
				store(*p);
			}
			break;

		case '$':               /* match endofline.. */
			if (!*(p+1))
				store(EOL);
			else {
				store(CHR);
				store(*p);
			}
			break;

		case '[':               /* match char class..*/
			store(CCL);

			if (*++p == '^') {
				mask = 0377;	
				p++;
			}
			else
				mask = 0;

			if (*p == '-')		/* real dash */
				chset(*p++);
			if (*p == ']')		/* real brac */
				chset(*p++);
			while (*p && *p != ']') {
				if (*p == '-' && *(p+1) && *(p+1) != ']') {
					p++;
					c1 = *(p-2) + 1;
					c2 = *p++;
					while (c1 <= c2)
						chset(c1++);
				}
#ifdef EXTEND
				else if (*p == '\\' && *(p+1)) {
					p++;
					chset(*p++);
				}
#endif
				else
					chset(*p++);
			}
			if (!*p)
				badpat("Missing ]");

			for (n = 0; n < BITBLK; bittab[n++] = (char) 0)
				store(mask ^ bittab[n]);
	
			break;

		case '*':               /* match 0 or more.. */
		case '+':               /* match 1 or more.. */
			if (p == pat)
				badpat("Empty closure");
			lp = sp;		/* previous opcode */
			if (*lp == CLO)		/* equivalence..   */
				break;
			switch(*lp) {

			case BOL:
			case BOT:
			case EOT:
			case BOW:
			case EOW:
			case REF:
				badpat("Illegal closure");
			default:
				break;
			}

			if (*p == '+')
				for (sp = mp; lp < sp; lp++)
					store(*lp);

			store(END);
			store(END);
			sp = mp;
			while (--mp > lp)
				*mp = mp[-1];
			store(CLO);
			mp = sp;
			break;

		case '\\':              /* tags, backrefs .. */
			switch(*++p) {

			case '(':
				if (tagc < MAXTAG) {
					tagstk[++tagi] = tagc;
					store(BOT);
					store(tagc++);
				}
				else
					badpat("Too many \\(\\) pairs");
				break;
			case ')':
				if (*sp == BOT)
					badpat("Null pattern inside \\(\\)");
				if (tagi > 0) {
					store(EOT);
					store(tagstk[tagi--]);
				}
				else
					badpat("Unmatched \\)");
				break;
			case '<':
				store(BOW);
				break;
			case '>':
				if (*sp == BOW)
					badpat("Null pattern inside \\<\\>");
				store(EOW);
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				n = *p-'0';
				if (tagi > 0 && tagstk[tagi] == n)
					badpat("Cyclical reference");
				if (tagc > n) {
					store(REF);
					store(n);
				}
				else
					badpat("Undetermined reference");
				break;
#ifdef EXTEND
			case 'b':
				store(CHR);
				store('\b');
				break;
			case 'n':
				store(CHR);
				store('\n');
				break;
			case 'f':
				store(CHR);
				store('\f');
				break;
			case 'r':
				store(CHR);
				store('\r');
				break;
			case 't':
				store(CHR);
				store('\t');
				break;
#endif
			default:
				store(CHR);
				store(*p);
			}
			break;

		default :               /* an ordinary char  */
			store(CHR);
			store(*p);
			break;
		}
		sp = lp;
	}
	if (tagi > 0)
		badpat("Unmatched \\(");
	store(END);
	sta = OKP;
	return 0;
}


static char *bol;
static char *bopat[MAXTAG];
static char *eopat[MAXTAG];
char *pmatch();

/*
 * re_exec:
 * 	execute nfa to find a match.
 *
 *	special cases: (nfa[0])	
 *		BOL
 *			Match only once, starting from the
 *			beginning.
 *		CHR
 *			First locate the character without
 *			calling pmatch, and if found, call
 *			pmatch for the remaining string.
 *		END
 *			re_comp failed, poor luser did not
 *			check for it. Fail fast.
 *
 *	If a match is found, bopat[0] and eopat[0] are set
 *	to the beginning and the end of the matched fragment,
 *	respectively.
 *
 */

int
re_exec(lp)
register char *lp;
{
	register char c;
	register char *ep = 0;
	register CHAR *ap = nfa;

	bol = lp;

	bopat[0] = 0;
	bopat[1] = 0;
	bopat[2] = 0;
	bopat[3] = 0;
	bopat[4] = 0;
	bopat[5] = 0;
	bopat[6] = 0;
	bopat[7] = 0;
	bopat[8] = 0;
	bopat[9] = 0;

	switch(*ap) {

	case BOL:			/* anchored: match from BOL only */
		ep = pmatch(lp,ap);
		break;
	case CHR:			/* ordinary char: locate it fast */
		c = *(ap+1);
		while (*lp && *lp != c)
			lp++;
		if (!*lp)		/* if EOS, fail, else fall thru. */
			return 0;
	default:			/* regular matching all the way. */
		while (*lp) {
			if ((ep = pmatch(lp,ap)))
				break;
			lp++;
		}
		break;
	case END:			/* munged automaton. fail always */
		return 0;
	}
	if (!ep)
		return 0;

	if (internal_error)
		return -1;

	bopat[0] = lp;
	eopat[0] = ep;
	return 1;
}

/* 
 * pmatch: 
 *	internal routine for the hard part
 *
 * 	This code is mostly snarfed from an early
 * 	grep written by David Conroy. The backref and
 * 	tag stuff, and various other mods are by oZ.
 *
 *	special cases: (nfa[n], nfa[n+1])
 *		CLO ANY
 *			We KNOW ".*" will match ANYTHING
 *			upto the end of line. Thus, go to
 *			the end of line straight, without
 *			calling pmatch recursively. As in
 *			the other closure cases, the remaining
 *			pattern must be matched by moving
 *			backwards on the string recursively,
 *			to find a match for xy (x is ".*" and 
 *			y is the remaining pattern) where
 *			the match satisfies the LONGEST match
 *			for x followed by a match for y.
 *		CLO CHR
 *			We can again scan the string forward
 *			for the single char without recursion, 
 *			and at the point of failure, we execute 
 *			the remaining nfa recursively, as
 *			described above.
 *
 *	At the end of a successful match, bopat[n] and eopat[n]
 *	are set to the beginning and end of subpatterns matched
 *	by tagged expressions (n = 1 to 9).	
 *
 */

/*
 * character classification table for word boundary
 * operators BOW and EOW. the reason for not using 
 * ctype macros is that we can let the user add into 
 * our own table. see re_modw. This table is not in
 * the bitset form, since we may wish to extend it
 * in the future for other character classifications. 
 *
 *	TRUE for 0-9 A-Z a-z _
 */
static char chrtyp[MAXCHR] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 0, 0, 0, 0
	};

#define inascii(x)	(0177&(x))
#define iswordc(x) 	chrtyp[inascii(x)]
#define isinset(x,y) 	((x)[((y)&BLKIND)>>3] & bitarr[(y)&BITIND])

/*
 * skip values for CLO XXX to skip past the closure
 *
 */

#define ANYSKIP	2 	/* [CLO] ANY END ...	     */
#define CHRSKIP	3	/* [CLO] CHR chr END ...     */
#define CCLSKIP 18	/* [CLO] CCL 16bytes END ... */

static char *
pmatch(lp, ap)
register char *lp;
register CHAR *ap;
{
	register int op, c, n;
	register char *e;		/* extra pointer for CLO */
	register char *bp;		/* beginning of subpat.. */
	register char *ep;		/* ending of subpat..	 */
	char *are;			/* to save the line ptr. */

	while ((op = *ap++) != END)
		switch(op) {

		case CHR:
			if (*lp++ != *ap++)
				return 0;
			break;
		case ANY:
			if (!*lp++)
				return 0;
			break;
		case CCL:
			c = *lp++;
			if (!isinset(ap,c))
				return 0;
			ap += BITBLK;
			break;
		case BOL:
			if (lp != bol)
				return 0;
			break;
		case EOL:
			if (*lp)
				return 0;
			break;
		case BOT:
			bopat[*ap++] = lp;
			break;
		case EOT:
			eopat[*ap++] = lp;
			break;
 		case BOW:
			if (lp!=bol && iswordc(lp[-1]) || !iswordc(*lp))
				return 0;
			break;
		case EOW:
			if (lp==bol || !iswordc(lp[-1]) || iswordc(*lp))
				return 0;
			break;
		case REF:
			n = *ap++;
			bp = bopat[n];
			ep = eopat[n];
			while (bp < ep)
				if (*bp++ != *lp++)
					return 0;
			break;
		case CLO:
			are = lp;
			switch(*ap) {

			case ANY:
				while (*lp)
					lp++;
				n = ANYSKIP;
				break;
			case CHR:
				c = *(ap+1);
				while (*lp && c == *lp)
					lp++;
				n = CHRSKIP;
				break;
			case CCL:
				while ((c = *lp) && isinset(ap+1,c))
					lp++;
				n = CCLSKIP;
				break;
			default:
				internal_error++;
				return 0;
			}

			ap += n;

			while (lp >= are) {
				if (e = pmatch(lp, ap))
					return e;
				--lp;
			}
			return 0;
		default:
			internal_error++;
			return 0;
		}
	return lp;
}
#endif /* Need regex libraries? Compile to nothing if not.  */
$EOD
$!
$CREATE [.archie]regex.h
$DECK
/*
 * regex.h : External defs for Ozan Yigit's regex functions, for systems
 *	that don't have them builtin. See regex.c for copyright and other
 *	details.
 *
 * Note that this file can be included even if we're linking against the
 * system routines, since the interface is (deliberately) identical.
 *
 * George Ferguson, ferguson@cs.rochester.edu, 11 Sep 1991.
 */

#if defined(_AUX_SOURCE) || defined(USG)
/* Let them use ours if they wish.  */
# ifndef NOREGEX
extern char *regcmp();
extern char *regex();
#define re_comp regcmp
#define re_exec regex
# endif
#else
extern char *re_comp();
extern int re_exec();
#endif
$EOD
$!
$CREATE [.archie]stcopy.c
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <stdio.h>
#include <pmachine.h>

#ifdef NEED_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if defined(MSDOS)
# include <stdlib.h>
#endif

char	*stcopyr();

int	string_count = 0;
int	string_max = 0;

/*
 * stcopy - allocate space for and copy a string
 *
 *     STCOPY takes a string as an argument, allocates space for
 *     a copy of the string, copies the string to the allocated space,
 *     and returns a pointer to the copy.
 */

char *
stcopy(st)
    char	*st;
    {
      if (!st) return(NULL);
      if (string_max < ++string_count) string_max = string_count;

      return strcpy((char *)malloc(strlen(st) + 1), st);
    }

/*
 * stcopyr - copy a string allocating space if necessary
 *
 *     STCOPYR takes a string, S, as an argument, and a pointer to a second
 *     string, R, which is to be replaced by S.  If R is long enough to
 *     hold S, S is copied.  Otherwise, new space is allocated, and R is
 *     freed.  S is then copied to the newly allocated space.  If S is
 *     NULL, then R is freed and NULL is returned.
 *
 *     In any event, STCOPYR returns a pointer to the new copy of S,
 *     or a NULL pointer.
 */
char *
stcopyr(s,r)
    char	*s;
    char	*r;
    {
	int	sl;

	if(!s && r) {
	    free(r);
	    string_count--;
	    return(NULL);
	}
	else if (!s) return(NULL);

	sl = strlen(s) + 1;

	if(r) {
	    if ((strlen(r) + 1) < sl) {
		free(r);
		r = (char *) malloc(sl);
	    }
	}
	else {
	    r = (char *) malloc(sl);
	    string_count++;
	    if(string_max < string_count) string_max = string_count;
	}
	    
	return strcpy(r,s);
    }

/*
 * stfree - free space allocated by stcopy or stalloc
 *
 *     STFREE takes a string that was returned by stcopy or stalloc 
 *     and frees the space that was allocated for the string.
 */
void
stfree(st)
    char *st;
    {
	if(st) {
	    free(st);
	    string_count--;
	}
    }


$EOD
$!
$CREATE [.archie]support.c
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

/*
 * Miscellaneous routines pulled from ~beta/lib/pfs and ~beta/lib/filters
 */

#include <copyright.h>
#include <stdio.h>

#include <errno.h>

#ifdef VMS
# ifdef WOLLONGONG
#  include "twg$tcp:[netdist.include]netdb.h"
# else /* not Wollongong */
#  ifdef UCX
#   include netdb
#  else /* Multinet */
#   include "multinet_root:[multinet.include]netdb.h"
#  endif
# endif
# include <vms.h>
#else /* not VMS */
# include <sys/types.h>
# include <pmachine.h>
# ifdef NEED_STRING_H
#  include <string.h>
# else
#  include <strings.h>
# endif
# ifndef CUTCP
#  include <netdb.h>
# endif
# if !defined(MSDOS) || defined(OS2)
#  include <sys/file.h>
#  include <sys/param.h>
# endif
#endif /* VMS */

#include <pfs.h>
#include <pprot.h>
#include <perrno.h>
#include <pcompat.h>
#include <pauthent.h>

#include "regex.h"

int	pfs_enable = PMAP_ATSIGN;

#ifndef FALSE
# define TRUE 	1
# define FALSE   0
#endif

/* 
 * wcmatch - Match string s against template containing widlcards
 *
 *	     WCMATCH takes a string and a template, and returns
 *	     true if the string matches the template, and 
 *	     FALSE otherwise.
 *
 *    ARGS:  s        - string to be tested
 *           template - Template containing optional wildcards
 *
 * RETURNS:  TRUE (non-zero) on match.  FALSE (0) otherwise.
 *
 *    NOTE:  If template is NULL, will return TRUE.
 *
 */
int
wcmatch(s,template)
    char	*s;
    char	*template;
    {
	char	temp[200];
	char	*p = temp;

	if(!template) return(TRUE);
	*p++ = '^';

	while(*template) {
	    if(*template == '*') {*(p++)='.'; *(p++) = *(template++);}
	    else if(*template == '?') {*(p++)='.';template++;}
	    else if(*template == '.') {*(p++)='\\';*(p++)='.';template++;}
	    else if(*template == '[') {*(p++)='\\';*(p++)='[';template++;}
	    else if(*template == '$') {*(p++)='\\';*(p++)='$';template++;}
	    else if(*template == '^') {*(p++)='\\';*(p++)='^';template++;}
	    else if(*template == '\\') {*(p++)='\\';*(p++)='\\';template++;}
	    else *(p++) = *(template++);
	}
	    
	*p++ = '$';
	*p++ = '\0';

	if(re_comp(temp)) return(FALSE);

#ifdef AUX
	if (re_exec(s) == (char *)NULL)
	  return 0;
	return 1;
#else
	return(re_exec(s));
#endif
    }

/*
 * ul_insert - Insert a union link at the right location
 *
 *             UL_INSERT takes a directory and a union link to be added
 *             to a the list of union links in the directory.  It then
 *             inserts the union link in the right spot in the linked
 *             list of union links associated with that directory.
 *
 *	       If an identical link already exists, then the link which
 *             would be evaluated earlier (closer to the front of the list)
 *             wins and the other one is freed.  If this happens, an error
 *             will also be returned.
 *        
 *    ARGS:    ul    - link to be inserted
 *	       vd    - directory to get link
 *             p     - vl that this link will apper after
 *                     NULL - This vl will go at end of list
 *                     vd   - This vl will go at head of list
 *
 * RETURNS:    Success, or UL_INSERT_ALREADY_THERE or UL_INSERT_SUPERSEDING
 */
int
ul_insert(ul,vd,p)
    VLINK	ul;		/* Link to be inserted                   */
    PVDIR	vd;		/* Directory to receive link             */
    VLINK	p;		/* Union link to appear prior to new one */
    {
	VLINK	current;

	/* This is the first ul in the directory */
	if(vd->ulinks == NULL) {
	    vd->ulinks = ul;
	    ul->previous = NULL;
	    ul->next = NULL;
	    return(PSUCCESS);
	}

	/* This ul will go at the head of the list */
	if(p == (VLINK) vd) {
	    ul->next = vd->ulinks;
	    ul->next->previous = ul;
	    vd->ulinks = ul;
	    ul->previous = NULL;
	}
	/* Otherwise, decide if it must be inserted at all  */
	/* If an identical link appears before the position */
	/* at which the new one is to be inserted, we can   */
	/* return without inserting it 			    */
	else {
	    current = vd->ulinks;

	    while(current) {
		/* p == NULL means we insert after last link */
		if(!p && (current->next == NULL))
		    p = current;

		if(vl_comp(current,ul) == 0) {
		    vlfree(ul);
		    return(UL_INSERT_ALREADY_THERE);
		}

		if(current == p) break;
		current = current->next;
	    }

	    /* If current is null, p was not found */
	    if(current == NULL)
		return(UL_INSERT_POS_NOTFOUND);

	    /* Insert ul */
	    ul->next = p->next;
	    p->next = ul;
	    ul->previous = p;
	    if(ul->next) ul->next->previous = ul;
	}

	/* Check for identical links after ul */
	current = ul->next;

	while(current) {
	    if(vl_comp(current,ul) == 0) {
		current->previous->next = current->next;
		if(current->next)
		    current->next->previous = current->previous;
		vlfree(current);
		return(UL_INSERT_SUPERSEDING);
	    }
	    current = current->next;
	}
	
	return(PSUCCESS);
    }

/*
 * vl_insert - Insert a directory link at the right location
 *
 *             VL_INSERT takes a directory and a link to be added to a 
 *             directory and inserts it in the linked list of links for
 *             that directory.  
 *
 *             If a link already exists with the same name, and if the
 *             information associated with the new link matches that in
 *             the existing link, an error is returned.  If the information
 *             associated with the new link is different, but the magic numbers
 *             match, then the new link will be added as a replica of the
 *             existing link.  If the magic numbers do not match, the new
 *             link will only be added to the list of "replicas" if the
 *             allow_conflict flag has been set.
 * 
 *             If the link is not added, an error is returned and the link
 *             is freed.  Ordering for the list of links is by the link name.  
 *        
 *             If vl is a union link, then VL_INSERT calls ul_insert with an
 *	       added argument indicating the link is to be included at the
 *             end of the union link list.
 * 
 *    ARGS:    vl - Link to be inserted, vd - directory to get link
 *             allow_conflict - insert links with conflicting names
 *
 * RETURNS:    Success, or VL_INSERT_ALREADY_THERE
 */
int
vl_insert(vl,vd,allow_conflict)
    VLINK	vl;		/* Link to be inserted               */
    PVDIR	vd;		/* Directory to receive link         */
    int		allow_conflict;	/* Allow duplicate names             */
    {
	VLINK	current;	/* To step through list		     */
	VLINK	crep;		/* To step through list of replicas  */
	int	retval;		/* Temp for checking returned values */

	/* This can also be used to insert union links at end of list */
	if(vl->linktype == 'U') return(ul_insert(vl,vd,NULL));

	/* If this is the first link in the directory */
	if(vd->links == NULL) {
	    vd->links = vl;
	    vl->previous = NULL;
	    vl->next = NULL;
	    vd->lastlink = vl;
	    return(PSUCCESS);
	}

	/* If no sorting is to be done, just insert at end of list */
	if(allow_conflict == VLI_NOSORT) {
	    vd->lastlink->next = vl;
	    vl->previous = vd->lastlink;
	    vl->next = NULL;
	    vd->lastlink = vl;
	    return(PSUCCESS);
	}

	/* If it is to be inserted at start of list */
	if(vl_comp(vl,vd->links) < 0) {
	    vl->next = vd->links;
	    vl->previous = NULL;
	    vl->next->previous = vl;
	    vd->links = vl;
	    return(PSUCCESS);
	}

	current = vd->links;

	/* Otherwise, we must find the right spot to insert it */
	while((retval = vl_comp(vl,current)) > 0) {
	    if(!current->next) {
		/* insert at end */
		vl->previous = current;
		vl->next = NULL;
		current->next = vl;
		vd->lastlink = vl;
		return(PSUCCESS);
	    }
	    current = current->next;
	}

	/* If we found an equivilant entry already in list */
	if(!retval) {
	    if(vl_equal(vl,current)) {
		vlfree(vl);
		return(VL_INSERT_ALREADY_THERE);
	    }
	    if((allow_conflict == VLI_NOCONFLICT) &&
	       ((vl->f_magic_no != current->f_magic_no) ||
		(vl->f_magic_no==0)))
		return(VL_INSERT_CONFLICT);
	    /* Insert the link into the list of "replicas" */
	    /* If magic is 0, then create a pseudo magic number */
	    if(vl->f_magic_no == 0) vl->f_magic_no = -1;
	    crep = current->replicas;
	    if(!crep) {
		current->replicas = vl;
		vl->next = NULL;
		vl->previous = NULL;
	    }
	    else {
		while(crep->next) {
		    /* If magic was 0, then we need a unique magic number */
		    if((crep->f_magic_no < 0) && (vl->f_magic_no < 1))
			(vl->f_magic_no)--;
		    crep = crep->next;
		}
		/* If magic was 0, then we need a unique magic number */
		if((crep->f_magic_no < 0) && (vl->f_magic_no < 1))
		    (vl->f_magic_no)--;
		crep->next = vl;
		vl->previous = crep;
		vl->next = NULL;
	    }
	    return(PSUCCESS);
	}

	/* We found the spot where vl is to be inserted */
	vl->next = current;
	vl->previous = current->previous;
	current->previous = vl;
	vl->previous->next = vl;
	return(PSUCCESS);
    }

/*
 * nlsindex - Find first instance of string 2 in string 1 following newline
 *
 *	      NLSINDEX scans string 1 for the first instance of string
 *	      2 that immediately follows a newline.  If found, NLSINDEX
 *	      returns a pointer to the first character of that instance.
 *	      If no instance is found, NLSINDEX returns NULL (0).
 *
 *    NOTE:   This function is only useful for searching strings that
 *            consist of multiple lines.  s1 is assumed to be preceeded
 * 	      by a newline.  Thus, if s2 is at the start of s1, it will
 *	      be found.
 *    ARGS:   s1 - string to be searched
 *            s2 - string to be found
 * RETURNS:   First instance of s2 in s1, or NULL (0) if not found
 */
char *
nlsindex(s1,s2)
    char	*s1;		/* String to be searched */
    char	*s2;		/* String to be found    */
    {
	register int s2len = strlen(s2);
	char	*curline = s1;	/* Pointer to start of current line */

	/* In case s2 appears at start of s1 */
	if(strncmp(curline,s2,s2len) == 0)
	    return(curline);

	/* Check remaining lines of s1 */
	while((curline = (char *) index(curline,'\n')) != NULL) {
	    curline++;
	    if(strncmp(curline,s2,s2len) == 0)
		return(curline);
	}

	/* We didn't find it */
	return(NULL);
    }

/*
 * month_sname - Return a month name from it's number
 *
 *               MONTH_SNAME takes a number in the range 0
 *               to 12 and returns a pointer to a string
 *               representing the three letter abbreviation
 *	         for that month.  If the argument is out of 
 *		 range, MONTH_SNAME returns a pointer to "Unk".
 *
 *       ARGS:   n - Number of the month
 *    RETURNS:   Abbreviation for selected month
 */
char *month_sname(n)
    int n;		/* Month number */
{
    static char *name[] = { "Unk",
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };
    return((n < 1 || n > 12) ? name[0] : name[n]);
}

/*
 * sindex - Find first instance of string 2 in string 1 
 *
 *	      SINDEX scans string 1 for the first instance of string
 *	      2.  If found, SINDEX returns a pointer to the first
 *	      character of that instance.  If no instance is found, 
 *	      SINDEX returns NULL (0).
 *
 *    ARGS:   s1 - string to be searched
 *            s2 - string to be found
 * RETURNS:   First instance of s2 in s1, or NULL (0) if not found
 */
char *
sindex(s1,s2)
    char	*s1;		/* String to be searched   */
    char	*s2;		/* String to be found      */
    {
	register int s2len = strlen(s2);
	char	*s = s1;	/* Temp pointer to string  */

	/* Check for first character of s2 */
	while((s = (char *) index(s,*s2)) != NULL) {
	    if(strncmp(s,s2,s2len) == 0)
		return(s);
	    s++;
	}

	/* We didn't find it */
	return(NULL);
    }

int
scan_error(erst)
    char	*erst;
    {
	*p_err_string = '\0';

	if(strncmp(erst,"NOT-A-DIRECTORY",15) == 0) 
	    return(DIRSRV_NOT_DIRECTORY);

	if(strncmp(erst,"UNIMPLEMENTED",13) == 0) {
	    perrno = DIRSRV_UNIMPLEMENTED;
	    sscanf(erst+13,"%*[^\n \t\r]%*[ \t]%[^\n]",p_err_string);
	    return(perrno);
	}

	if(strncmp(erst,"WARNING ",8) == 0) {
	    erst += 8;
	    *p_warn_string = '\0';
	    sscanf(erst,"%*[^\n \t\r]%*[ \t]%[^\n]",p_warn_string);
	    /* Return values for warnings are negative */
	    if(strncmp(erst,"OUT-OF-DATE",11) == 0) {
		pwarn = PWARN_OUT_OF_DATE;
		return(PSUCCESS);
	    }
	    if(strncmp(erst,"MESSAGE",7) == 0) {
		pwarn = PWARN_MSG_FROM_SERVER;
		return(PSUCCESS);
	    }
	    pwarn = PWARNING;
	    sscanf(erst,"%[^\n]",p_warn_string);
	    return(PSUCCESS);
	}
	else if(strncmp(erst,"ERROR",5) == 0) {
	    if(*(erst+5)) sscanf(erst+6,"%[^\n]",p_err_string);
	    perrno = DIRSRV_ERROR;
	    return(perrno);
	}
	/* The rest start with "FAILURE" */
	else if(strncmp(erst,"FAILURE",7) != 0) {
	    /* Unrecognized - Give warning, but return PSUCCESS */
	    if(pwarn == 0) {
		*p_warn_string = '\0';
		pwarn = PWARN_UNRECOGNIZED_RESP;
		sscanf(erst,"%[^\n]",p_warn_string);
	    }
	    return(PSUCCESS);
	}

	if(strncmp(erst,"FAILURE ",8) != 0) {
	    perrno = PFAILURE;
	    return(perrno);
	}	
	erst += 8;
	
	sscanf(erst,"%*[^\n \t\r]%*[ \t]%[^\n]",p_err_string);

	/* Still to add               */
	/* DIRSRV_AUTHENT_REQ     242 */
	/* DIRSRV_BAD_VERS        245 */

	if(strncmp(erst,"NOT-FOUND",9) == 0) 
	    perrno = DIRSRV_NOT_FOUND;
	else if(strncmp(erst,"NOT-AUTHORIZED",13) == 0) 
	    perrno = DIRSRV_NOT_AUTHORIZED;
	else if(strncmp(erst,"ALREADY-EXISTS",14) == 0) 
	    perrno = DIRSRV_ALREADY_EXISTS;
	else if(strncmp(erst,"NAME-CONFLICT",13) == 0) 
	    perrno = DIRSRV_NAME_CONFLICT;
	else if(strncmp(erst,"SERVER-FAILED",13) == 0) 
	    perrno = DIRSRV_SERVER_FAILED;
 	/* Use it whether it starts with FAILURE or not */
	else if(strncmp(erst,"NOT-A-DIRECTORY",15) == 0) 
	    perrno = DIRSRV_NOT_DIRECTORY;
	else perrno = PFAILURE;

	return(perrno);
    }

PATTRIB 
parse_attribute(line)
    char	*line;
    {
	char	l_precedence[MAX_DIR_LINESIZE];
	char	l_name[MAX_DIR_LINESIZE];
	char	l_type[MAX_DIR_LINESIZE];
	char	l_value[MAX_DIR_LINESIZE];
	PATTRIB	at;
	int	tmp;

	tmp = sscanf(line,"OBJECT-INFO %s %s %[^\n]", l_name, l_type, l_value);
	
	if(tmp < 3) {
	    tmp = sscanf(line,"LINK-INFO %s %s %s %[^\n]", l_precedence,
			 l_name, l_type, l_value);
	    if(tmp < 4) {
		perrno = DIRSRV_BAD_FORMAT;
		return(NULL);
	    }
	}

	at = atalloc();

	if(tmp == 4) {
	    if(strcmp(l_precedence,"CACHED") == 0) 
		at->precedence = ATR_PREC_CACHED;
	    else if(strcmp(l_precedence,"LINK") == 0) 
		at->precedence = ATR_PREC_LINK;
	    else if(strcmp(l_precedence,"REPLACEMENT") == 0) 
		at->precedence = ATR_PREC_REPLACE;
	    else if(strcmp(l_precedence,"ADDITIONAL") == 0) 
		at->precedence = ATR_PREC_ADD;
	}

	at->aname = stcopy(l_name);
	at->avtype = stcopy(l_type);
	if(strcmp(l_type,"ASCII") == 0) 
	    at->value.ascii = stcopy(l_value);
	else if(strcmp(l_type,"LINK") == 0) {
	    char		ftype[MAX_DIR_LINESIZE];
	    char		lname[MAX_DIR_LINESIZE];
	    char		htype[MAX_DIR_LINESIZE];
	    char		host[MAX_DIR_LINESIZE];
	    char		ntype[MAX_DIR_LINESIZE];
	    char		fname[MAX_DIR_LINESIZE];
	    VLINK		al;

	    al = vlalloc();
	    at->value.link = al;

	    tmp = sscanf(l_value,"%c %s %s %s %s %s %s %d %d",
			 &(al->linktype),
			 ftype,lname,htype,host,ntype,fname,
			 &(al->version),
			 &(al->f_magic_no));
	    if(tmp == 9) {
		al->type = stcopyr(ftype,al->type);
		al->name = stcopyr(unquote(lname),al->name);
		al->hosttype = stcopyr(htype,al->hosttype);
		al->host = stcopyr(host,al->host);
		al->nametype = stcopyr(ntype,al->nametype);
		al->filename = stcopyr(fname,al->filename);
	    }
	    else {
		perrno = DIRSRV_BAD_FORMAT;
		return(NULL);
	    }
	    
	}

	return(at);
    }

/*
 * nxtline - Find the next line in the string
 *
 *	      NXTLINE takes a string and returns a pointer to
 *	      the character immediately following the next newline.
 *
 *    ARGS:   s - string to be searched
 *
 * RETURNS:   Next line or NULL (0) on failure
 */
char *
nxtline(s)
    char	*s;		/* String to be searched */
 {
	s = (char *) index(s,'\n');
	if(s) return(++s);
	else return(NULL);
    }


/*
 * unquote - unquote string if necessary
 *
 *	      UNQUOTE takes a string and unquotes it if it has been quoted.
 *
 *    ARGS:   s - string to be unquoted
 *            
 * RETURNS:   The original string.  If the string has been quoted, then the
 *            result appears in static storage, and must be copied if 
 *            it is to last beyond the next call to quote.
 *
 */
char *
unquote(s)
    char	*s;		/* String to be quoted */
    {
	static char	unquoted[200];
	char		*c = unquoted;

	if(*s != '\'') return(s);

	s++;

	/* This should really treat a quote followed by other */
	/* than a quote or a null as an error                 */
	while(*s) {
	    if(*s == '\'') s++;
	    if(*s) *c++ = *s++;
	}

	*c++ = '\0';

	return(unquoted);
    }

#if defined(DEBUG) && defined(STRSPN)
/* needed for -D option parsing */
/*
 * strspn - Count initial characters from chrs in s
 *
 *	      STRSPN counts the occurances of chacters from chrs
 *            in the string s preceeding the first occurance of
 *            a character not in s.
 *
 *    ARGS:   s    - string to be checked
 *            chrs - string of characters we are looking for
 *
 * RETURNS:   Count of initial characters from chrs in s
 */
strspn(s,chrs)
    char	*s;    /* String to search                         */
    char	*chrs; /* String of characters we are looking for  */
    {
	char	*cp;   /* Pointer to the current character in chrs */
	int	count; /* Count of characters seen so far          */
	
	count = 0;

	while(*s) {
	    for(cp = chrs;*cp;cp++)
		if(*cp == *s) {
		    s++;
		    count++;
		    goto done;
		}
	    return(count);
	done:
	    ;
	}
	return(count);
    }
#endif

#ifdef CUTCP
char
*inet_ntoa(struct in_addr in)
{
	static	char	buff[36];

	unsigned char	*c = (char *) &in.address;
	sprintf(buff,"%d.%d.%d.%d",*c,*(c+1),*(c+2),*(c+3));
	return(buff);
}

long
inet_addr(char *cp)
{
	long	value = 0;
	unsigned	v1,v2,v3,v4;

	v1 = v2 = v3 = v4 = 0xff;
	sscanf(cp,"%d.%d.%d.%d",&v1,&v2,&v3,&v4);
	value = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
	return(value);
}

struct	hostent
*gethostbyname(char *name)
{
	struct machinfo	*mp;
	int	mnum;
	unsigned long	now;
	static	struct hostent	ht;
	extern int pfs_debug;

	mp = Shostlook(name);
	if(!mp || (!mp->hostip[0])) {	/* DNS lookup */
#ifdef DEBUG
	    if (pfs_debug)
		fprintf(stderr, "Domain name lookup of %s\n", name);
#endif
		mnum = Sdomain(name);		/* start a DNS lookup */
		now = time(NULL) + NS_TIMEOUT;
		while(now > time(NULL)) {
			int	i, class, dat;

			Stask();
			i = Sgetevent(USERCLASS, &class, &dat);
			if(i == DOMOK) {	/* domain lookup ok */
				mp = Slooknum(mnum);
#ifdef DEBUG
	    if (pfs_debug)
		fprintf(stderr, "Domain name lookup of %s Completed OK\n", name);
#endif
				break;
			}
		}
		if(!mp)	{	/* get here if timeout */
#ifdef DEBUG
	    if (pfs_debug)
		fprintf(stderr, "Domain name lookup of %s Failed\n", name);
#endif
			return(NULL);
		}
	}
	ht.h_addr = *((unsigned long *) mp->hostip);
	ht.h_length = 4;
	ht.h_addrtype = AF_INET;
	return(&ht);

}
#endif /* CUTCP */

#ifdef GETENV
/*
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getenv.c	5.7 (Berkeley) 6/1/90";
#endif /* LIBC_SCCS and not lint */

#include <stdlib.h>
#include <stddef.h>

/*
 * getenv --
 *	Returns ptr to value associated with name, if any, else NULL.
 */
char *
getenv(name)
	char *name;
{
	int offset;
	char *_findenv();

	return(_findenv(name, &offset));
}

/*
 * _findenv --
 *	Returns pointer to value associated with name, if any, else NULL.
 *	Sets offset to be the offset of the name/value combination in the
 *	environmental array, for use by setenv(3) and unsetenv(3).
 *	Explicitly removes '=' in argument name.
 *
 *	This routine *should* be a static; don't use it.
 */
char *
_findenv(name, offset)
	register char *name;
	int *offset;
{
	extern char **environ;
	register int len;
	register char **P, *C;

	for (C = name, len = 0; *C && *C != '='; ++C, ++len);
	for (P = environ; *P; ++P)
		if (!strncmp(*P, name, len))
			if (*(C = *P + len) == '=') {
				*offset = P - environ;
				return(++C);
			}
	return(NULL);
}
#endif
$EOD
$!
$CREATE [.archie]copyright.h
$DECK
/* These are the uw-copyright.h and usc-copyright.h files that appear in
   the Prospero distribution.  */

/* 
  Copyright (c) 1989, 1990, 1991 by the University of Washington

  Permission to use, copy, modify, and distribute this software and its
  documentation for non-commercial purposes and without fee is hereby
  granted, provided that the above copyright notice appear in all copies
  and that both the copyright notice and this permission notice appear in
  supporting documentation, and that the name of the University of
  Washington not be used in advertising or publicity pertaining to
  distribution of the software without specific, written prior
  permission.  The University of Washington makes no representations
  about the suitability of this software for any purpose.  It is
  provided "as is" without express or implied warranty.

  Prospero was written by Clifford Neuman (bcn@isi.edu).

  Questions concerning this software should be directed to 
  info-prospero@isi.edu.

  */

/* 
  Copyright (c) 1991, 1992 by the University of Southern California
  All rights reserved.

  Permission to use, copy, modify, and distribute this software and its
  documentation in source and binary forms for non-commercial purposes
  and without fee is hereby granted, provided that the above copyright
  notice appear in all copies and that both the copyright notice and
  this permission notice appear in supporting documentation. and that
  any documentation, advertising materials, and other materials related
  to such distribution and use acknowledge that the software was
  developed by the University of Southern California, Information
  Sciences Institute.  The name of the University may not be used to
  endorse or promote products derived from this software without
  specific prior written permission.

  THE UNIVERSITY OF SOUTHERN CALIFORNIA makes no representations about
  the suitability of this software for any purpose.  THIS SOFTWARE IS
  PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  Other copyrights might apply to parts of the Prospero distribution
  and are so noted when applicable.

  Prospero was originally written by Clifford Neuman (bcn@isi.edu).
  Contributors include Benjamin Britt (britt@isi.edu)
  and others identified in individual modules.

  Questions concerning this software should be directed to 
  info-prospero@isi.edu.

  */
$EOD
$!
$CREATE [.archie]udp.c
$DECK
/*
 * udp - Check if UDP traffic is allowed on this host; we open port 1527 on
 *       a system (default of cs.widener.edu), which is expecting it; the
 *       date is output (e.g. very similar to the daytime service).  This
 *       will conclusively tell us if UDP traffic on ports > 1000 is allowed.
 *
 *	It should print out the date if UDP traffic's not blocked on your
 *	system.  If it just hangs, try these tests too:
 *	  a. run it with -d  (e.g. "udp -d"); that goes to the normal UDP port
 *	     to print the date.  If it works, then you can be sure that any
 *	     UDP traffic > port 1000 is blocked on your system.
 *	  b. if it hangs too, try "telnet 147.31.254.130 13" and see if
 *	     _that_ prints the date; if it doesn't, it's another problem (your
 *	     network can't get to me, e.g.).
 *
 * Compile by: cc -o udp udp.c
 *
 * Brendan Kehoe, brendan@cs.widener.edu, Oct 1991.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef hpux
# include <arpa/inet.h>
#endif

#define	SIZE	2048
#define	HOST	"147.31.254.130"	/* cs.widener.edu */
#define PORT	1527

main (argc, argv)
     int argc;
     char **argv;
{
  int s, len;
  struct sockaddr_in server, sa;
  char buf[SIZE];

  if ((s = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
      perror ("socket()");
      exit (1);
    }

  bzero ((char *) &sa, sizeof (sa));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl (INADDR_ANY);
  sa.sin_port = htons (0);

  if (bind (s, (struct sockaddr *) &sa, sizeof (sa)) < 0)
    {
      perror ("bind()");
      exit (1);
    }

  bzero ((char *) &server, sizeof (server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr (HOST);
  if (argc > 1 && strcmp(*(argv + 1), "-d") == 0)
    server.sin_port = htons ((unsigned short) 13);
  else
    server.sin_port = htons ((unsigned short) PORT);

  /* yoo hoo, we're here .. */
  if (sendto (s, "\n", 1, 0, &server, sizeof (server)) < 0)
    {
      perror ("sendto()");
      exit (1);
    }

  /* slurp */
  len = sizeof (server);
  if (recvfrom (s, buf, sizeof (buf), 0, &server, &len) < 0)
    {
      perror ("recvfrom");
      exit (1);
    }

  printf ("%s", buf);
  close (s);
}
$EOD
$!
$CREATE [.archie]vl_comp.c
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <pfs.h>

/*
 * vl_comp - compare the names of two virtual links
 *
 *           VL_COMP compares the names of two links.  It returns
 *           0 if they are equal, negative if vl1 < vl2, and positive if
 *           vl1 > vl2.
 *
 *    ARGS:  vl1,vl2 - Virtual links to be compared
 * 
 * RETURNS:  0 if equal, + is vl1 > vl2, - if vl1 < vl2
 *
 *   NOTES:  Order of significance is as follows.  Existence,
 *           name.  If names do not exist, then hosttype, host,
 *           native filenametype, native filename.  The only time
 *           the name will not exist if if the link is a union link.
 */
int
vl_comp(vl1,vl2)
    VLINK	vl1;
    VLINK	vl2;
    {
	int	retval;

	if(vl1->name && !vl2->name) return(1);
	if(!vl1->name && vl2->name) return(-1);
	if(vl1->name && vl2->name && (*(vl1->name) || *(vl2->name)))
	    return(strcmp(vl1->name,vl2->name));

	retval = strcmp(vl1->hosttype,vl2->hosttype);
	if(!retval) retval = strcmp(vl1->host,vl2->host);
	if(!retval) retval = strcmp(vl1->nametype,vl2->nametype);
	if(!retval) retval = strcmp(vl1->filename,vl2->filename);
	return(retval);
    }

/*
 * vl_equal - compare the values of two virtual links
 *
 *           VL_EQUAL compares the values of two links.  It returns
 *           1 if all important fields are the same, and 0 otherwise.
 *
 *    ARGS:  vl1,vl2 - Virtual links to be compared
 * 
 * RETURNS:  1 if equal, 0 if not equal
 *
 */
int
vl_equal(vl1,vl2)
    VLINK	vl1;
    VLINK	vl2;
    {
      return strcmp(vl1->name, vl2->name) == 0         &&
	     vl1->linktype == vl2->linktype            &&
	     strcmp(vl1->type, vl2->type) == 0         &&
	     strcmp(vl1->hosttype, vl2->hosttype) == 0 &&
	     strcmp(vl1->host, vl2->host) == 0         &&
	     strcmp(vl1->nametype, vl2->nametype) == 0 &&
	     strcmp(vl1->filename, vl2->filename) == 0 &&
	     vl1->version == vl2->version              &&
	     vl1->f_magic_no == vl2->f_magic_no        ;

    }

$EOD
$!
$CREATE [.archie]vlalloc.c
$DECK
/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <stdio.h>

#include <pfs.h>
#include <pmachine.h>

static VLINK	lfree = NULL;
int		vlink_count = 0;
int		vlink_max = 0;

/*
 * vlalloc - allocate and initialize vlink structure
 *
 *    VLALLOC returns a pointer to an initialized structure of type
 *    VLINK.  If it is unable to allocate such a structure, it
 *    returns NULL.
 */
VLINK
vlalloc()
    {
	VLINK	vl;
	if(lfree) {
	    vl = lfree;
	    lfree = lfree->next;
	}
	else {
	    vl = (VLINK) malloc(sizeof(VLINK_ST));
	    if (!vl) return(NULL);
	    vlink_max++;
	}

	vlink_count++;

	/* Initialize and fill in default values */
	/* Since all but four are set to a zero-value,
	   why not just wipe it clean?  */
	ZERO(vl);

	vl->linktype = 'L';
	vl->type = stcopy("FILE");
	vl->hosttype = stcopy("INTERNET-D");
	vl->nametype = stcopy("ASCII");

	return(vl);
    }

/*
 * vlfree - free a VLINK structure
 *
 *    VLFREE takes a pointer to a VLINK structure and adds it to
 *    the free list for later reuse.
 */
void
vlfree(vl)
    VLINK	vl;
    {
        extern int string_count;

	if(vl->dontfree) return;
	/* many of these don't need to call stfree(); since a check
	   for pointer validity's already done before even calling
	   it, we can just call free() here then do one big decrement
	   of string_count at the end.  */
	if(vl->name) free(vl->name);
	stfree(vl->type);
	if(vl->replicas) vllfree(vl->replicas);
	stfree(vl->hosttype);
	if(vl->host) free(vl->host);
	stfree(vl->nametype);
	if(vl->filename) free(vl->filename);
	if(vl->args) free(vl->args);
	if(vl->lattrib) atlfree(vl->lattrib);
	/* No allocation routines for f_info yet */
	vl->f_info = NULL;
	vl->next = lfree;
	vl->previous = NULL;
	lfree = vl;
	vlink_count--;
	string_count -= 4; /* freed name, host, filename, and args */
    }

/*
 * vllfree - free a VLINK structure
 *
 *    VLLFREE takes a pointer to a VLINK structure frees it and any linked
 *    VLINK structures.  It is used to free an entrie list of VLINK
 *    structures.
 */
void
vllfree(vl)
    VLINK	vl;
    {
	VLINK	nxt;

	while((vl != NULL) && !vl->dontfree) {
	    nxt = vl->next;
	    vlfree(vl);
	    vl = nxt;
	}
    }

$EOD
$!
$CREATE [.archie]vms.h
$DECK
#ifndef _ARCHIE_VMS
#define _ARCHIE_VMS
#include <pmachine.h>

#if !defined(MULTINET_30) && !defined(WOLLONGONG) && !defined(UCX)
#include "[.vms]pseudos.h"
#include "[.vms]types.h"
#include "[.vms]in.h"
#include "[.vms]signal.h"
#include "[.vms]socket.h"
#include "[.vms]time.h"

#else

/* time_t gets multiply defined <ekup> */
#ifndef __TYPES
#define __TYPES
#endif
#ifdef MULTINET_30
# include "multinet_root:[multinet.include.sys]types.h"
# include "multinet_root:[multinet.include.netinet]in.h"
# include "multinet_root:[multinet.include.sys]socket.h"
# include "multinet_root:[multinet.include.sys]time.h"
#endif /* MULTINET_30 */

#ifdef WOLLONGONG
/* We don't want size_t defined.  */
# ifndef __STDDEF
#  define __STDDEF
# endif
# include "twg$tcp:[netdist.include.sys]types.h"
# include "twg$tcp:[netdist.include.netinet]in.h"
# include "twg$tcp:[netdist.include.sys]socket.h"
# include "twg$tcp:[netdist.include.sys]time.h"
#endif /* WOLLONGONG */

#ifdef UCX
# include <types.h>
# include <in.h>
# include <socket.h>
# include <time.h>
# include "[.vms]fd.h"
#endif /* UCX */

#endif /* Multinet or Wallongong or UCX */

#endif /* _ARCHIE_VMS */
$EOD
$!
$CREATE [.archie]vms_support.c
$DECK
/* Emulation of 4.2 UNIX socket interface routines includes drivers for
   Wollongong, CMU-TEK, UCX tcp/ip interface and also emulates the SUN
   version of X.25 sockets.  The TWG will also work for MultiNet.  */

/* This is from unixlib, by P.Kay@massey.ac.nz; wonderful implementation.
   You can get the real thing on 130.123.1.4 as unixlib_tar.z.  */

#include <stdio.h>
#include <errno.h>
#include <ssdef.h>
#include <dvidef.h>
#include <signal.h>
#include <sys$library:msgdef.h>
#include <iodef.h>
#include <ctype.h>
#include <vms.h>
#include "[.vms]network.h"

#define QIO_FAILED (st != SS$_NORMAL || p[s].iosb[0] != SS$_NORMAL)
#define QIO_ST_FAILED (st != SS$_NORMAL)

/* Socket routine.  */
int
VMSsocket (domain, type, protocol)
     int domain, type, protocol;
{
  struct descriptor inetdesc, x25desc, mbxdesc;
  int i, st, s, p_initialise ();
  long ucx_sock_def;
  char *getenv ();

  if (!tcp_make)
    set_tcp_make ();

  if (p_initialised == 0)
    {
      for (i = 0; i < 32; i++)
	p_initialise (i);

      p_initialised = 1;
    }

  /* First of all, get a file descriptor and file ptr we can associate with
     the socket, allocate a buffer, and remember the socket details.  */
  s = dup (0);
  if (s > 31)
    {
      errno = EMFILE;
      close (s);
      return -1;
    }

  p[s].fptr = fdopen (s, "r");
  p[s].fd_buff = (unsigned char *) malloc (BUF_SIZE);
  p[s].domain = domain;
  p[s].type = type;
  p[s].protocol = protocol;

  /* Handle the case of INET and X.25 separately.  */
  if (domain == AF_INET)
    {
      if (tcp_make == NONE)
	{
	  printf ("Trying to obtain a TCP socket when we don't have TCP!\n");
	  exit (1);
	}
      if (tcp_make == CMU)
	{
	  /* For CMU we need only assign a channel.  */
	  inetdesc.size = 3;
	  inetdesc.ptr = "IP:";
	  if (sys$assign (&inetdesc, &p[s].channel, 0, 0) != SS$_NORMAL)
	    return -1;
	}
      else if (tcp_make == UCX)
	{
	  /* For UCX assign channel and associate a socket with it.  */
	  inetdesc.size = 3;
	  inetdesc.ptr = "BG:";
	  if (sys$assign (&inetdesc, &p[s].channel, 0, 0) != SS$_NORMAL)
	    return -1;

	  ucx_sock_def = (domain << 24) + (type << 16) + protocol;
	  st = sys$qiow (0, p[s].channel, IO$_SETMODE, p[s].iosb, 0, 0,
			 &ucx_sock_def, 0, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;
	}
      else
	{
	  /* For TWG we assign the channel and associate a socket with it.  */
	  inetdesc.size = 7;
	  inetdesc.ptr = "_INET0:";

	  if (sys$assign (&inetdesc, &p[s].channel, 0, 0) != SS$_NORMAL)
	    return -1;

	  st = sys$qiow (0, p[s].channel, IO$_SOCKET, p[s].iosb, 0, 0,
			 domain, type, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domains yet.  */
    return -1;

  /* For each case if we are successful we return the descriptor.  */
  return s;
}

/* Bind routine.  */
VMSbind (s, name, namelen)
     int s;
     union socket_addr *name;
     int namelen;
{
  char infobuff[1024], lhost[32];
  int st;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].domain == AF_INET)
    {
      /* One main problem with bind is that if we're given a port number
	 of 0, then we're expected to return a unique port number.  Since
	 we don't KNOW, we return 1050+s and look to Lady Luck.  */
      if (tcp_make == CMU)
	{
	  if (name->in.sin_port == 0 && p[s].type != SOCK_DGRAM)
	    name->in.sin_port = 1050 + s;
	  p[s].namelen = namelen;
	  bcopy (name, &(p[s].name), namelen);

	  if (p[s].type == SOCK_DGRAM)
	    {
	      /* Another problem is that CMU still needs an OPEN request
		 even if it's a datagram socket.  */
	      st = sys$qiow (0, p[s].channel, TCP$OPEN, p[s].iosb,
			     0, 0, 0, 0, ntohs (p[s].name.in.sin_port),
			     0, 1, 0);
	      if (QIO_ST_FAILED)
		return -1;

	      p[s].cmu_open = 1;
	      sys$qiow (0, p[s].channel, TCP$INFO, p[s].iosb,
			0, 0, &infobuff, 1024, 0, 0, 0, 0);
	      bcopy (infobuff + 264, &(p[s].name.in.sin_port), 2);
	      p[s].name.in.sin_port = htons (p[s].name.in.sin_port);

	      /* So get it another way.  */
	      bcopy (infobuff + 136, lhost, infobuff[1]);
	      lhost[infobuff[1]] = '\0';
	      sys$qiow (0, p[s].channel, GTHST, p[s].iosb,
			0, 0, &infobuff, 1024, 1, lhost, 0, 0);
	      bcopy (infobuff + 4, &(p[s].name.in.sin_addr), 4);

	      /* Be prepared to receive a message.  */
	      hang_a_read (s);
	    }
	}
      else if (tcp_make == UCX)
	{
	  /* UCX will select a prot for you.  If the port's number is 0,
	     translate "name" into an item_2 list.  */
	  struct itemlist lhost;
	  lhost.length = namelen;
	  lhost.code = 0;
	  lhost.dataptr = (char *) name;

	  st = sys$qiow (0, p[s].channel, IO$_SETMODE, p[s].iosb, 0, 0,
			 0, 0, &lhost, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  if (p[s].type == SOCK_DGRAM)
	    hang_a_read (s);

	}
      else
	{
	  /* WG is more straightforward */
	  st = sys$qiow (0, p[s].channel, IO$_BIND, p[s].iosb,
			 0, 0, name, namelen, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  /* If it's a datagram, get ready for the message.  */
	  if (p[s].type == SOCK_DGRAM)
	    hang_a_read (s);
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;

  return 0;
}

/* Connect routine.  */
VMSconnect (s, name, namelen)
     int s;
     union socket_addr *name;
     int namelen;
{
  int pr, fl, st;
  char *inet_ntoa ();
  static struct
  {
    int len;
    char name[128];
  } gethostbuf;
  extern int connect_ast ();

  if (!tcp_make)
    set_tcp_make ();

  /* For datagrams we need to remember who the name was so we can send all
     messages to that address without having to specify it all the time.  */
  if (p[s].connected)
    {
      if (p[s].connected == 1)
	errno = EISCONN;
      else
	{
	  errno = ECONNREFUSED;
	  p[s].connected = 0;
	}
      return -1;
    }

  if (p[s].connect_pending)
    {
      errno = EALREADY;
      return -1;
    }

  p[s].passive = 0;
  p[s].tolen = namelen;
  bcopy (name, &(p[s].to), namelen);

  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{

	  /* Get the info about the remote host  and open up a connection.  */
	  st = sys$qiow (0, p[s].channel, GTHST, p[s].iosb, 0, 0, &gethostbuf,
			 132, 2, name->in.sin_addr.s_addr, 0, 0);
	  if (QIO_FAILED)
	    {
	      strcpy (gethostbuf.name, inet_ntoa (name->in.sin_addr.s_addr));
	      gethostbuf.len = strlen (gethostbuf.name);
	    }
	  gethostbuf.name[gethostbuf.len] = 0;

	  /* TCP */
	  pr = 0;
	  /* Active */
	  fl = 1;

	  /* Nothing else for datagrams.  */
	  if (p[s].type == SOCK_DGRAM)
	    return (0);
	  st = sys$qio (s, p[s].channel, TCP$OPEN, p[s].iosb, connect_ast,
			&p[s], &(gethostbuf.name), ntohs (name->in.sin_port),
			ntohs (p[s].name.in.sin_port), fl, pr, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else if (tcp_make == UCX)
	{
	  /* Both UDP and TCP can use a connect - IO$_ACCESS */
	  p[s].rhost.length = namelen;
	  p[s].rhost.code = 0;
	  p[s].rhost.dataptr = (char *) name;

	  st = sys$qio (s, p[s].channel, IO$_ACCESS, p[s].iosb, connect_ast,
			&p[s], 0, 0, &p[s].rhost, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else
	{
	  /* TWG */
	  if (p[s].type == SOCK_DGRAM)
	    return (0);
	  st = sys$qio (s, p[s].channel, IO$_CONNECT, p[s].iosb, connect_ast,
			&p[s], name, namelen, 0, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;

  if (p[s].non_blocking)
    {
      if (p[s].connected)
	{
	  if (p[s].connected == 1)
	    return 0;
	  else
	    {
	      p[s].connected = 0;
	      errno = ECONNREFUSED;
	      return -1;
	    }
	}
      else
	{
	  p[s].connect_pending = 1;
	  errno = EINPROGRESS;
	  return -1;
	}
    }
  else
    {
      /* wait for the connection to occur */
      if (p[s].connected)
	{
	  if (p[s].connected == 1)
	    return 0;
	  else
	    {
	      p[s].connected = 0;
	      errno = ECONNREFUSED;
	      return -1;
	    }
	}

      /* Timed out? */
      if (wait_efn (s) == -1)
	return -1;

      if (p[s].connected != SS$_NORMAL)
	{
	  errno = ECONNREFUSED;
	  return -1;
	}

      return 0;
    }
}

/* Listen routine.  */
VMSlisten (s, backlog)
     int s;
     int backlog;
{
  int st;

  if (!tcp_make)
    set_tcp_make ();

  p[s].passive = 1;
  p[s].backlog = backlog;
  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  /* For the CMU sockets we can't do the open call in listen;
	     we have to do it in hang_an_accept, because when we close
	     off the connection we have to be ready to accept another
	     one.  accept() also calls hang_an_accept on the old
	     descriptor.  */

	  /* Nothing */
	}
      else if (tcp_make == UCX)
	{

	  /* Doc Verbage sez backlog is descriptor of byte.  Doc examples
	     and common sense say backlog is value.  Value doesn't work,
	     so let's try descriptor of byte after all.  */
	  struct descriptor bl;
	  unsigned char ucx_backlog;

	  ucx_backlog = (unsigned char) backlog;
	  bl.size = sizeof (ucx_backlog);
	  bl.ptr = (char *) &ucx_backlog;

	  st = sys$qiow (0, p[s].channel, IO$_SETMODE, p[s].iosb, 0, 0,
			 0, 0, 0, &bl, 0, 0);
	  if (QIO_FAILED)
	    return -1;
	}
      else
	{
	  /* TWG */
	  st = sys$qiow (0, p[s].channel, IO$_LISTEN, p[s].iosb, 0, 0,
			 backlog, 0, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;

  p[s].status = LISTENING;
  hang_an_accept (s);
  return 0;
}

/* Accept routine.  */
int
VMSaccept (s, addr, addrlen)
     int s;
     union socket_addr *addr;
     int *addrlen;
{
  int news, st;
  struct descriptor inetdesc;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].non_blocking && !p[s].accept_pending)
    {
      errno = EWOULDBLOCK;
      return -1;
    }

  /* hang_an_accept set up an incoming connection request so we have first
     to hang around until one appears or we time out.  */
  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  char infobuff[1024];

	  /* Timed out?  */
	  if (wait_efn (s) == -1)
	    return -1;

	  /* Ok, get a new descriptor ...  */
	  news = dup (0);
	  if (news > 31)
	    {
	      errno = EMFILE;
	      close (news);
	      return -1;
	    }

	  /* ... and copy all of our data across.  */
	  bcopy (&p[s], &p[news], sizeof (p[0]));

	  /* But not this field, of course! */
	  p[news].s = news;

	  sys$qiow (0, p[news].channel, TCP$INFO, p[news].iosb,
		    0, 0, &infobuff, 1024, 0, 0, 0, 0);

	  /* Copy across the connection info if necessary.  */
	  if (addr != 0)
	    {
	      *addrlen = sizeof (struct sockaddr_in);
	      bcopy (infobuff + 132, &(addr->in.sin_port), 2);
	      addr->in.sin_port = htons (addr->in.sin_port);
	      addr->in.sin_family = AF_INET;
	      bcopy (infobuff + 272, &(addr->in.sin_addr), 4);
	      p[news].fromlen = *addrlen;
	      bcopy (addr, &(p[news].from), *addrlen);
	    }
	  p[news].status = PASSIVE_CONNECTION;

	  /* Get a new file ptr for the socket.  */
	  p[news].fptr = fdopen (news, "r");

	  /* Reset this field.  */
	  p[news].accept_pending = 0;

	  /* Allocate a buffer.  */
	  p[news].fd_buff = (unsigned char *) malloc (BUF_SIZE);
	  p[news].fd_leftover = 0;

	  /* Be prepared to get msgs.  */
	  hang_a_read (news);

	  /* Now fix up our previous socket so it's again listening
	     for connections.  */
	  inetdesc.size = 3;
	  inetdesc.ptr = "IP:";
	  if (sys$assign (&inetdesc, &p[s].channel, 0, 0) != SS$_NORMAL)
	    return -1;
	  p[s].status = LISTENING;
	  hang_an_accept (s);

	  /* Return the new socket descriptor.  */
	  return news;
	}
      else if (tcp_make == UCX)
	{
	  /* UCX does the actual accept from hang_an_accept.  The accept info
	    is put into the data structure for the "listening" socket.
	    These just need to be copied into a newly allocated socket for
	    the connect and the listening socket re-started.  */

	  /* Wait for event flag from accept being received inside
	     of hang_an_accept().  */

	  if (wait_efn (s) == -1)
	    /* Timed out.  */
	    return -1;

	  /* Ok, get a new descriptor ...  */
	  news = dup (0);
	  if (news > 31)
	    {
	      errno = EMFILE;
	      close (news);
	      return -1;
	    }
	  /* ... and copy all of our data across.  */
	  bcopy (&p[s], &p[news], sizeof (p[0]));
	  p[news].s = news;	/* but not this field */
	  p[news].channel = p[s].ucx_accept_chan;

	  /* Initialize the remote host address item_list_3 struct.  */
	  p[news].rhost.length = sizeof (struct sockaddr_in);
	  p[news].rhost.code = 0;
	  p[news].rhost.dataptr = (char *) &p[news].from;
	  p[news].rhost.retlenptr = &p[news].fromdummy;

	  if (addr != 0)
	    {
	      /* Return the caller's info, if requested.  */
	      *addrlen = p[news].fromdummy;
	      bcopy (&p[news].from, addr, p[news].fromdummy);
	    }

	  /* Finish fleshing out the new structure.  */
	  p[news].status = PASSIVE_CONNECTION;

	  /* Get a new file pointer for the socket.  */
	  p[news].fptr = fdopen (news, "r");

	  /* Reset this field.  */
	  p[news].accept_pending = 0;

	  /* Allocate a buffer.  */
	  p[news].fd_buff = (unsigned char *) malloc (BUF_SIZE);
	  p[news].fd_leftover = 0;

	  /* Get it started reading.  */
	  hang_a_read (news);

	  p[s].status = LISTENING;
	  hang_an_accept (s);

	  return news;
	}
      else
	{
	  /* TWG */
	  struct descriptor inetdesc;
	  int size;

	  /* Time out?  */
	  if (wait_efn (s) == -1)
	    return -1;

	  /* Ok, get a new descriptor ...  */
	  news = dup (0);
	  if (news > 31)
	    {
	      errno = EMFILE;
	      close (news);
	      return -1;
	    }

	  /* Assign a new channel.  */
	  inetdesc.size = 7;
	  inetdesc.ptr = "_INET0:";
	  st = sys$assign (&inetdesc, &p[news].channel, 0, 0);
	  if (QIO_ST_FAILED)
	    {
	      p[s].accept_pending = 0;
	      sys$clref (s);
	      return -1;
	    }

	  /* From info needs an int length field! */
	  size = sizeof (p[s].from) + 4;
	  st = sys$qiow (0, p[news].channel, IO$_ACCEPT, p[news].iosb, 0, 0,
			 &p[s].fromdummy, size, p[s].channel, 0, 0, 0);

	  if (QIO_ST_FAILED || p[news].iosb[0] != SS$_NORMAL)
	    {
	      p[s].accept_pending = 0;
	      sys$clref (s);
	      return -1;
	    }

	  if (addr != 0)
	    {
	      /* Return the caller's info if requested.  */
	      *addrlen = p[s].fromdummy;
	      bcopy (&p[s].from, addr, *addrlen);
	    }

	  /* Fix up our new data structure.  */
	  p[news].status = PASSIVE_CONNECTION;
	  p[news].domain = AF_INET;
	  p[news].passive = 1;
	  p[news].fptr = fdopen (news, "r");
	  /* Allocate a buffer.  */
	  p[news].fd_buff = (unsigned char *) malloc (BUF_SIZE);

	  /* Be prepared to accept msgs.  */
	  hang_a_read (news);

	  /* Get the old descriptor back onto accepting.  */
	  hang_an_accept (s);
	  return news;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* Recv routine.  */
int
VMSrecv (s, buf, len, flags)
     int s;
     char *buf;
     int len, flags;
{
  return recvfrom (s, buf, len, flags, 0, 0);
}

/* Revfrom routine.  */
int
VMSrecvfrom (s, buf, len, flags, from, fromlen)
     int s;
     char *buf;
     int len, flags;
     union socket_addr *from;
     int *fromlen;
{
  int number;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].domain != AF_INET && p[s].domain != AF_X25)
    return -1;

  /* If we're not onto datagrams, then it's possible that a previous
     call to recvfrom didn't read all the data, and left some behind.
     So first of all, look in our data buffer for any leftovers that
     will satisfy this read.  */

  /* We couldn't satisfy the request from previous calls so we must now
     wait for a message to come through.  */
  if (wait_efn (s) == -1)
    /* Timed out.  */
    return -1;

  if (p[s].closed_by_remote == 1)
    {
      /* This could have happened! */
      errno = ECONNRESET;
      return -1;
    }

  if (from != NULL)
    {
      if (tcp_make == CMU)
	{
	  if (p[s].type == SOCK_DGRAM)
	    {
	      /* Not documented but we get the from data from the beginning of
		 the data buffer.  */
	      *fromlen = sizeof (p[s].from.in);
	      from->in.sin_family = AF_INET;
	      bcopy (&p[s].fd_buff[8], &(from->in.sin_port), 2);
	      from->in.sin_port = htons (from->in.sin_port);
	      bcopy (&p[s].fd_buff[0], &(from->in.sin_addr), 4);

	      /* Remove the address data from front of data buffer.  */
	      bcopy (p[s].fd_buff + 12, p[s].fd_buff, p[s].fd_buff_size);
	    }
	  else
	    {
	      *fromlen = p[s].fromlen;
	      bcopy (&p[s].from, from, p[s].fromlen);
	    }
	}
      else if (tcp_make == UCX)
	{
	  *fromlen = p[s].fromdummy;
	  bcopy (&p[s].from, from, p[s].fromdummy);
	}
      else
	{
	  *fromlen = p[s].fromlen;
	  bcopy (&p[s].from, from, p[s].fromlen);
	}
    }

  /* We may've received too much.  */
  number = p[s].fd_buff_size;
  if (number <= len)
    {
      /* If we haven't give back all the data available.  */
      bcopy (p[s].fd_buff, buf, number);
      p[s].fd_leftover = 0;
      hang_a_read (s);
      return (number);
    }
  else
    {
      /* If we have too much data then split it up.  */
      p[s].fd_leftover = p[s].fd_buff;
      bcopy (p[s].fd_leftover, buf, len);
      /* And change the pointers.  */
      p[s].fd_leftover += len;
      p[s].fd_buff_size -= len;
      return (len);
    }
}

/* Send routine.  */
int
VMSsend (s, msg, len, flags)
     int s;
     char *msg;
     int len, flags;
{
  return sendto (s, msg, len, flags, 0, 0);
}

/* Sendto routine.  */
int
VMSsendto (s, msg, len, flags, to, tolen)
     int s;
     unsigned char *msg;
     int len, flags;
     union socket_addr *to;
     int tolen;
{
  int i, j, st, size;
  unsigned char udpbuf[BUF_SIZE + 12];
  char infobuff[1024], lhost[32];
  unsigned short int temp;

  if (!tcp_make)
    set_tcp_make ();

  /* First remember who we sent it to and set the value of size.  */
  if (to != 0)
    {
      p[s].tolen = tolen;
      bcopy (to, &(p[s].to), tolen);
      size = tolen;
    }
  else
    size = 0;

  if (p[s].domain == AF_INET)
    {
      /* We might never have started a read for udp (socket/sendto) so
	 put one here.  */
      if (p[s].type == SOCK_DGRAM)
	hang_a_read (s);

      if (tcp_make == CMU)
	{
	  if (p[s].type == SOCK_DGRAM)
	    {
	      /* We might never have opened up a udp connection yet,
		 so check.  */
	      if (p[s].cmu_open != 1)
		{
		  st = sys$qiow (0, p[s].channel, TCP$OPEN, p[s].iosb, 0, 0,
				 0, 0, 0, 0, 1, 0);
		  if (QIO_ST_FAILED)
		    return -1;

		  p[s].cmu_open = 1;
		  sys$qiow (0, p[s].channel, TCP$INFO, p[s].iosb,
			    0, 0, &infobuff, 1024, 0, 0, 0, 0);
		  bcopy (infobuff + 264, &(p[s].name.in.sin_port), 2);
		  p[s].name.in.sin_port = htons (p[s].name.in.sin_port);
		  bcopy (infobuff + 136, lhost, infobuff[1]);
		  lhost[infobuff[1]] = '\0';
		  sys$qiow (0, p[s].channel, GTHST, p[s].iosb,
			    0, 0, &infobuff, 1024, 1, lhost, 0, 0);
		  bcopy (infobuff + 4, &(p[s].name.in.sin_addr), 4);
		}

	      /* This isn't well documented.  To send to a UDP socket, we
		 need to put the address info at the beginning of the
		 buffer.  */
	      bcopy (msg, udpbuf + 12, len);
	      bcopy (&p[s].to.in.sin_addr, udpbuf + 4, 4);
	      temp = ntohs (p[s].to.in.sin_port);
	      bcopy (&temp, udpbuf + 10, 2);
	      bcopy (&p[s].name.in.sin_addr, udpbuf, 4);
	      temp = ntohs (p[s].name.in.sin_port);
	      bcopy (&temp, udpbuf + 8, 2);
	      temp = len + 12;
	      st = sys$qiow (0, p[s].channel, TCP$SEND, p[s].iosb, 0, 0,
			     udpbuf, temp, 0, 0, 0, 0);
	      if (QIO_FAILED)
		return -1;
	    }
	  else
	    {
	      /* TCP (! UDP)  */
	      st = sys$qiow (0, p[s].channel, TCP$SEND, p[s].iosb, 0, 0,
			     msg, len, 0, 0, 0, 0);
	      if (QIO_FAILED)
		return -1;
	    }
	  return len;
	}
      else if (tcp_make == UCX)
	{
	  struct itemlist rhost;
	  rhost.length = sizeof (struct sockaddr_in);
	  rhost.code = 0;
	  rhost.dataptr = (char *) &p[s].to;

	  st = sys$qiow (0, p[s].channel, IO$_WRITEVBLK, p[s].iosb, 0, 0,
			 msg, len, &rhost, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return len;
	}
      else
	{
	  /* TWG */
	  st = sys$qiow (0, p[s].channel, IO$_WRITEVBLK, p[s].iosb,
			 0, 0, msg, len, 0, &p[s].to, size, 0);
	  if (QIO_FAILED)
	    return -1;

	  return len;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* Getsockname routine.  */
int
VMSgetsockname (s, name, namelen)
     int s;
     union socket_addr *name;
     int *namelen;
{
  int st;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  /* For CMU we just return values held in our data structure.  */
	  *namelen = p[s].namelen;
	  bcopy (&(p[s].name), name, *namelen);
	  return (0);
	}
      else if (tcp_make == UCX)
	{
	  /* An item_list_3 descriptor.  */
	  struct itemlist lhost;

	  lhost.length = *namelen;
	  lhost.code = 0;
	  lhost.dataptr = (char *) name;

	  /* Fill in namelen with actual ret len value.  */
	  lhost.retlenptr = (short int *) namelen;

	  st = sys$qiow (0, p[s].channel, IO$_SENSEMODE, p[s].iosb, 0, 0,
			 0, 0, &lhost, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return 0;
	}
      else
	{
	  /* TWG gives us the information. */
	  st = sys$qiow (0, p[s].channel, IO$_GETSOCKNAME, p[s].iosb,
			 0, 0, name, namelen, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return 0;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* Select routine.  */
int
VMSselect (nfds, readfds, writefds, exceptfds, timeout)
     int nfds;
     fd_set *readfds, *writefds, *exceptfds;
     struct timeval *timeout;
{
  int timer, fd, alarm_set, total, end;
  long mask, cluster;
  struct descriptor termdesc;
  static fd_set new_readfds, new_writefds, new_exceptfds;

  FD_ZERO (&new_readfds);
  FD_ZERO (&new_writefds);
  FD_ZERO (&new_exceptfds);
  total = 0;

  /* Assign a terminal channel if we haven't already.  */
  if (terminal.chan == -1)
    {
      termdesc.size = 10;
      termdesc.ptr = "SYS$INPUT:";
      sys$assign (&termdesc, &terminal.chan, 0, 0);
    }
  alarm_set = 0;
  if (timeout != NULL)
    {
      /* If a timeout is given then set the alarm.  */
      end = timeout->tv_sec;
      if (timer != 0)
	{
	  /* We need to reset the alarm if it didn't fire, but we set it.  */
	  alarm_set = 1;
	  si_alarm (end);
	}
    }
  else
    end = 1;

  do
    {
      if (exceptfds)
	{
	   /* Nothing */ ;
	}

      if (writefds)
	{
	  for (fd = 0; fd < nfds; fd++)
	    if (FD_ISSET (fd, writefds))
	      {
		if (p[fd].connect_pending)
		   /* Nothing */ ;
		else if ((p[fd].status == ACTIVE_CONNECTION)
			 || (p[fd].status == PASSIVE_CONNECTION))
		  {
		    FD_SET (fd, &new_writefds);
		    total++;
		  }
	      }
	}

      if (readfds)
	{
	  /* True if data pending or an accept.  */
	  for (fd = 3; fd < nfds; fd++)
	    if (FD_ISSET (fd, readfds) &&
		((p[fd].fd_buff_size != -1) || (p[fd].accept_pending == 1)))
	      {
		FD_SET (fd, &new_readfds);
		total++;
	      }
	}

      if (total || (end == 0))
	break;

      /* Otherwise, wait on an event flag.  It's possible that the wait can
	 be stopped by a spurious event flag being set -- i.e. one that's
	 got a status not normal.  So we've got to be prepared to loop
	 around the wait until a valid reason happens.  */

      /* Set up the wait mask.  */
      cluster = 0;
      mask = 0;
      for (fd = 3; fd < nfds; fd++)
	{
	  sys$clref (fd);
	  if (readfds)
	    if FD_ISSET
	      (fd, readfds) mask |= (1 << fd);
	  if (writefds)
	    if FD_ISSET
	      (fd, writefds) mask |= (1 << fd);
	  if (exceptfds)
	    if FD_ISSET
	      (fd, exceptfds) mask |= (1 << fd);
	}

      mask |= (1 << TIMER_EFN);

      /* Clear it off just in case.  */
      sys$clref (TIMER_EFN);

      /* Wait around.  */
      sys$wflor (cluster, mask);

      mask = 0;
      if (read_efn (TIMER_EFN))
	{
	  errno = EINTR;
	  break;
	}
  } while (1);
  /*NOTREACHED*/

  /* Unset the alarm if we set it.  */
  if (alarm_set == 1)
    alarm (0);

  if (readfds)
    *readfds = new_readfds;

  if (writefds)
    *writefds = new_writefds;

  if (exceptfds)
    *exceptfds = new_exceptfds;

  return total;
}

/* Shutdown routine.  */
VMSshutdown (s, how)
     int s, how;
{
  int st;
  int ucx_how;

  if (!tcp_make)
    set_tcp_make ();

  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  /* For CMU we just close off.  */
	  si_close (s);
	  return 0;
	}
      else if (tcp_make == UCX)
	{
	  st = sys$qiow (0, p[s].channel, IO$_DEACCESS | IO$M_SHUTDOWN,
			 p[s].iosb, 0, 0, 0, 0, 0, how, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return 0;
	}
      else
	{
	  /* TWG lets us do it.  */
	  st = sys$qiow (0, p[s].channel, IO$_SHUTDOWN, p[s].iosb, 0, 0, how,
			 0, 0, 0, 0, 0);
	  if (QIO_FAILED)
	    return -1;

	  return 0;
	}
    }
  else				/* it wasn't a socket */
    return -1;
}

/*  */

/* The following routines are used by the above socket calls.  */

/* hang_a_read sets up a read to be finished at some later time.  */
hang_a_read (s)
     int s;
{
  extern int read_ast ();
  int size, st;

  /* Don't bother if we already did it.  */
  if (p[s].read_outstanding == 1)
    return;

  /* Have a read outstanding.  */
  p[s].read_outstanding = 1;
  size = sizeof (p[s].from) + 4;
  sys$clref (s);

  /* Clear off the event flag just in case, and reset the buf size.  */
  p[s].fd_buff_size = -1;
  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  st = sys$qio (s, p[s].channel, TCP$RECEIVE, p[s].iosb, read_ast,
			&p[s], p[s].fd_buff, BUF_SIZE, 0, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else if (tcp_make == UCX)
	{

	  p[s].rhost.length = sizeof (struct sockaddr_in);
	  p[s].rhost.code = 0;
	  p[s].rhost.dataptr = (char *) &p[s].from;
	  p[s].rhost.retlenptr = &p[s].fromdummy;

	  st = sys$qio (s, p[s].channel, IO$_READVBLK, p[s].iosb, read_ast,
			&p[s], p[s].fd_buff, BUF_SIZE, &p[s].rhost, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else
	{
	  /* TWG */
	  st = sys$qio (s, p[s].channel, IO$_READVBLK, p[s].iosb, read_ast,
			&p[s], p[s].fd_buff, BUF_SIZE, 0, &p[s].fromlen,
			size, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* hang_an_accept waits for a connection request to come in.  */
hang_an_accept (s)
     int s;
{
  extern int accept_ast ();
  int st;

  /* Clear the event flag just in case.  */
  sys$clref (s);

  /* Reset our flag & buf size.  */
  p[s].accept_pending = 0;
  p[s].fd_buff_size = -1;
  if (p[s].domain == AF_INET)
    {
      if (tcp_make == CMU)
	{
	  st = sys$qio (s, p[s].channel, TCP$OPEN, p[s].iosb, accept_ast,
			&p[s], 0, 0, ntohs (p[s].name.in.sin_port), 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else if (tcp_make == UCX)
	{
	  struct descriptor inetdesc;

	  /* Assign channel for actual connection off listener.  */
	  inetdesc.size = 3;
	  inetdesc.ptr = "BG:";
	  if (sys$assign (&inetdesc, &p[s].ucx_accept_chan, 0,
			  0) != SS$_NORMAL)
	    return -1;

	  /* UCX's accept returns remote host info and the channel for a new
	     socket to perform reads/writes on, so a sys$assign isn't
	     really necessary.  */
	  p[s].rhost.length = sizeof (struct sockaddr_in);
	  p[s].rhost.dataptr = (char *) &p[s].from;
	  p[s].fromdummy = 0;
	  p[s].rhost.retlenptr = &p[s].fromdummy;

	  st = sys$qio (s, p[s].channel, IO$_ACCESS | IO$M_ACCEPT, p[s].iosb,
			accept_ast, &p[s], 0, 0, &p[s].rhost,
			&p[s].ucx_accept_chan, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
      else
	{
	  st = sys$qio (s, p[s].channel, IO$_ACCEPT_WAIT, p[s].iosb,
			accept_ast, &p[s], 0, 0, 0, 0, 0, 0);
	  if (QIO_ST_FAILED)
	    return -1;
	}
    }
  else
    /* We don't handle any other domain yet.  */
    return -1;
}

/* wait_efn just sets up a wait on either an event or the timer.  */
wait_efn (s)
     int s;
{
  long mask, cluster;

  cluster = 0;
  sys$clref (TIMER_EFN);
  mask = (1 << s) | (1 << TIMER_EFN);
  sys$wflor (cluster, mask);

  if (read_efn (TIMER_EFN))
    {
      errno = EINTR;
      return -1;
    }

  return 0;
}

/* read_ast is called by the system whenever a read is done.  */
read_ast (p)
     struct fd_entry *p;
{
  int i, j;
  unsigned char *v, *w;

  /* Reset the outstanding flag.  */
  p->read_outstanding = 0;
  if (p->iosb[0] == SS$_NORMAL)
    {
      /* Check no errors.  */
      p->fd_buff_size = p->iosb[1];
      if (tcp_make == CMU)
	{
	  /* fiddle for DGRMs */
	  if (p->type == SOCK_DGRAM)
	    p->fd_buff_size -= 12;
	}
      if (p->sig_req == 1)
	gsignal (SIGIO);
    }
  else if (p->iosb[0] == SS$_CLEARED)
    p->closed_by_remote = 1;
  else if (tcp_make == UCX)
    {
      if (p->iosb[0] == SS$_LINKDISCON)
	p->closed_by_remote = 1;
    }
}

/* accept_ast is called whenever an incoming call is detected.  */
accept_ast (p)
     struct fd_entry *p;
{
  if (p->iosb[0] == SS$_NORMAL)
    p->accept_pending = 1;
  else
    /* If it failed set up another listen.  */
    listen (p->s, p[p->s].backlog);
}

/* connect_ast is called whenever an async connect is made.  */
connect_ast (p)
     struct fd_entry *p;
{
  p->connect_pending = 0;
  if ((p->connected = p->iosb[0]) == SS$_NORMAL)
    {
      /* We made the connection.  */
      p->status = ACTIVE_CONNECTION;

      /* Be prepared to accept a msg.  */
      hang_a_read (p->s);
    }
}

/*  */
/* These routines handle stream I/O.  */

/* si_close -- must close off any connection in progress.  */
si_close (s)
     int s;
{
  if (!tcp_make)
    set_tcp_make ();

  if ((s < 0) || (s > 31))
    return -1;

  if (p[s].channel != 0)
    {
      /* Was it one of our descriptors? */
      if (p[s].domain == AF_INET)
	{
	  if (tcp_make == CMU)
	    sys$qiow (0, p[s].channel, TCP$CLOSE, p[s].iosb,
		      0, 0, 0, 0, 0, 0, 0, 0);
	  if (p[s].status != HANDED_OFF)
	    sys$dassgn (p[s].channel);
	  close (s);
	  free (p[s].fd_buff);
	  p_initialise (s);
	}
      return 0;
    }
  else
    {
      /* Re-initialise data structure just in case.  */
      p[s].fd_buff_size = -1;
      p[s].accept_pending = 0;
      p[s].status = INITIALISED;
      return close (s);
    }
}

/* si_alarm -- insert a call to our own alarm function.  */
si_alarm (i)
     int i;
{
  extern int pre_alarm ();

  /* Make the call to pre_alarm instead of what the user wants;
     pre_alarm will call his routine when it finishes.  */
  /* VAX needs this call each time! */
  signal (SIGALRM, pre_alarm);
  alarm (i);
}

/* pre_alarm -- gets called first on an alarm signal.  */
pre_alarm ()
{
  /* Come here first so we can set our timer event flag.  */
  sys$setef (TIMER_EFN);
  (*alarm_function) ();
}

/* p_initialise - initialise our data array.  */
p_initialise (s)
     int s;
{
  int j;
  for (j = 0; j < 4; j++)
    p[s].iosb[j] = 0;
  p[s].channel = 0;
  p[s].fd_buff_size = -1;
  p[s].accept_pending = 0;
  p[s].connect_pending = 0;
  p[s].connected = 0;
  p[s].fd_buff = NULL;
  p[s].fd_leftover = NULL;
  p[s].fptr = NULL;
  p[s].s = s;
  p[s].name.in.sin_port = 0;
  p[s].masklen = 4;
  for (j = 0; j < 16; j++)
    p[s].mask[j] = 0xff;
  p[s].need_header = 0;
  p[s].status = INITIALISED;
  p[s].read_outstanding = 0;
  p[s].cmu_open = 0;
  p[s].x25_listener = 0;
  p[s].mother = s;
  p[s].child = 0;
  p[s].no_more_accepts = 0;
  p[s].closed_by_remote = 0;
  p[s].non_blocking = 0;
  p[s].sig_req = 0;
  sys$clref (s);
}

/* read_efn -- see whether an event flag is set.  */
read_efn (i)
     int i;
{
  int j;
  sys$readef (i, &j);
  j &= (1 << i);

  return j;
}

static
set_tcp_make ()
{
  struct descriptor inetdesc;
  int channel;
  /* first try CMU */
  inetdesc.size = 3;
  inetdesc.ptr = "IP:";
  if (sys$assign (&inetdesc, &channel, 0, 0) == SS$_NORMAL)
    {
      sys$dassgn (channel);
      tcp_make = CMU;
      return;
    }

  /* next try TWG */
  inetdesc.size = 7;
  inetdesc.ptr = "_INET0:";
  if (sys$assign (&inetdesc, &channel, 0, 0) == SS$_NORMAL)
    {
      sys$dassgn (channel);
      tcp_make = WG;
      return;
    }

  /* next try UCX */
  inetdesc.size = 4;
  inetdesc.ptr = "BG0:";
  if (sys$assign (&inetdesc, &channel, 0, 0) == SS$_NORMAL)
    {
      sys$dassgn (channel);
      tcp_make = UCX;
      return;
    }

  /* nothing there oh dear!*/
  tcp_make = NONE;
  return;
}

static char *
getdevicename (channel)
     unsigned short int channel;
{
  int st;
  struct
  {
    struct itemlist id;
    int eol;
  } itmlst;
  static char name[64];
  short int lgth;

  name[0] = '\0';
  itmlst.id.code = DVI$_DEVNAM;
  itmlst.id.length = 64;
  itmlst.id.dataptr = name;
  itmlst.id.retlenptr = &lgth;
  itmlst.eol = 0;
  st = sys$getdvi (0, channel, 0, &itmlst, 0, 0, 0, 0);
  if (QIO_ST_FAILED)
    fprintf (stderr, "error getting device name %d\n", st);

  return (name);
}
$EOD
$!
$CREATE [.archie.vms]fd.h
$DECK
#ifndef _VMS_FD
#define _VMS_FD

typedef struct fd_set
{
  int fds_bits[1];
} fd_set;

#define FD_SETSIZE          (sizeof (fd_set) * 8)
#define FD_SET(f,s)         ((s)->fds_bits[0] |= (1 << (f)))
#define FD_CLR(f,s)         ((s)->fds_bits[0] &= ~(1 << (f)))
#define FD_ISSET(f,s)       ((s)->fds_bits[0] & (1 << (f)))
#define FD_ZERO(s)          ((s)->fds_bits[0] = 0)

#endif /* _VMS_FD */
$EOD
$!
$CREATE [.archie.vms]in.h
$DECK
/* netinet/in.h */
struct in_addr
{
  union
  {
    struct
    {
      unsigned char s_b1, s_b2, s_b3, s_b4;
    } S_un_b;
    struct
    {
      unsigned short s_w1, s_w2;
    } S_un_w;
    unsigned long S_addr;
  } S_un;
#define   s_addr S_un.S_addr
#define   s_host S_un.S_un_b.s_b2
#define   s_net  S_un.S_un_b.s_b1
#define   s_imp  S_un.S_un_w.s_w2
#define   s_impno S_un.S_un_b.s_b4
#define   s_lh   S_un.S_un_b.s_b3
};

#define INADDR_ANY 0x00000000
#define INADDR_BROADCAST 0xffffffff
#define INADDR_LOOPBACK 0x7f000001

struct sockaddr_in
{
  short sin_family;
  unsigned short sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

#define ntohl(x) (( (((unsigned long) x) >> 24)& 0x000000ff ) |\
                  ( (((unsigned long) x) >> 8) & 0x0000ff00 ) |\
                  ( (((unsigned long) x) << 8) & 0x00ff0000 ) |\
                  ( (((unsigned long) x) << 24)& 0xff000000 ))
#define ntohs(x) (( (((unsigned short) x) >> 8) |\
                  ( (((unsigned short) x) << 8)) & 0xffff ))
#define htonl(x) (( (((unsigned long) x) >> 24)& 0x000000ff ) |\
                  ( (((unsigned long) x) >> 8) & 0x0000ff00 ) |\
                  ( (((unsigned long) x) << 8) & 0x00ff0000 ) |\
                  ( (((unsigned long) x) << 24)& 0xff000000 ))
#define htons(x) (( (((unsigned short) x) >> 8) |\
                  ( (((unsigned short) x) << 8)) & 0xffff ))

#define IPPORT_RESERVED 1024
$EOD
$!
$CREATE [.archie.vms]pseudos.h
$DECK
/* These are so we don't end up using the MultiNet versions.  */
#define socket	VMSsocket
#define bind	VMSbind
#define connect	VMSconnect
#define listen	VMSlisten
#define accept	VMSaccept
#define select	VMSselect
#define recv	VMSrecv
#define recvfrom	VMSrecvfrom
#define send	VMSsend
#define sendto	VMSsendto
#define getsockname	VMSgetsockname
#define shutdown	VMSshutdown
#define getsockopt	VMSgetsockopt
#define setsockopt	VMSsetsockopt
$EOD
$!
$CREATE [.archie.vms]signal.h
$DECK
/* signal.h */
#define SIGURG  16
#define SIGTSTP 18
#define SIGCHLD 20
#define SIGIO   23
#define sigmask(m) (1 << ((m)-1))

#ifndef __GNUC__
# include <sys$library:signal.h>
#else /* Gnu C */
# include <gnu_cc_include:[000000]signal.h>
#endif /* not Gnu C */
$EOD
$!
$CREATE [.archie.vms]socket.h
$DECK
/* sys/socket.h */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2

#define SO_DEBUG        0x01
#define SO_ACCEPTCONN   0x02
#define SO_REUSEADDR    0x04
#define SO_KEEPALIVE    0x08
#define SO_LINGER       0x80
#define SO_DONTLINGER   (~SO_LINGER)

#define AF_UNSPEC       0
#define AF_ERROR        0
#define AF_INET         2
#define AF_CCITT        10
#define AF_X25          10

struct sockaddr
{
  unsigned short sa_family;
  char sa_data[14];
};

#define SOL_SOCKET      0xffff

#define MSG_OOB 1
#define MSG_PEEK 2
$EOD
$!
$CREATE [.archie.vms]time.h
$DECK
#ifndef __PKTIME
#define __PKTIME

struct timeval
{
  long tv_sec;
  long tv_usec;
};

struct timezone
{
  int tz_minuteswest;
  int tz_dsttime;
};

struct itimerval
{
  struct timeval it_interval;
  struct timeval it_value;
};

#define ITIMER_REAL 0
#define timerclear(x) (x)->tv_sec = (x)->tv_usec = 0

#ifndef __GNUC__
# include <sys$library:time.h>
#else /* not Gnu C */
# include <gnu_cc_include:[000000]time.h>
#endif /* Gnu C */

#endif /* __PKTIME */
$EOD
$!
$CREATE [.archie.vms]types.h
$DECK
/* sys/types.h */
#ifndef _types_
#define _types_

#ifndef __GNUC__
# include <sys$library:stddef.h>
#endif /* not Gnu C */

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef long daddr_t;
typedef char *caddr_t;

#include <sys$library:types.h>

typedef unsigned short ino_t;
typedef char *dev_t;
typedef unsigned int off_t;
typedef long key_t;

#include "[.vms]fd.h"

#endif /* _types */
$EOD
$!
$CREATE [.archie.vms]network.h
$DECK
/* Miscellaneous things for the networking library.  */

/* Actually an itemlist_3, but can be used for itemlist_2's.  */
struct itemlist
{
  short length;
  short code;
  char *dataptr;
  short *retlenptr;
};

union socket_addr
{
  struct sockaddr_in in;
};

#define   TCP$SEND        (IO$_WRITEVBLK)
#define   TCP$RECEIVE     (IO$_READVBLK)
#define   TCP$OPEN        (IO$_CREATE)
#define   TCP$CLOSE       (IO$_DELETE)
#define   TCP$ABORT       (IO$_DEACCESS)
#define   TCP$STATUS      (IO$_ACPCONTROL)
#define   TCP$INFO        (IO$_MODIFY)
#define   GTHST           (IO$_SKIPFILE)

#define   IO$_SEND        (IO$_WRITEVBLK)
#define   IO$_RECEIVE     (IO$_READVBLK)
#ifndef IO$S_FCODE
#define IO$S_FCODE 0x0006
#endif
#define   IO$_SOCKET      (IO$_ACCESS | (0 << IO$S_FCODE))
#define   IO$_BIND        (IO$_ACCESS | (1 << IO$S_FCODE))
#define   IO$_LISTEN      (IO$_ACCESS | (2 << IO$S_FCODE))
#define   IO$_ACCEPT      (IO$_ACCESS | (3 << IO$S_FCODE))
#define   IO$_CONNECT     (IO$_ACCESS | (4 << IO$S_FCODE))
#define   IO$_SETSOCKOPT  (IO$_ACCESS | (5 << IO$S_FCODE))
#define   IO$_GETSOCKOPT  (IO$_ACCESS | (6 << IO$S_FCODE))
#define   IO$_IOCTL       (IO$_ACCESS | (8 << IO$S_FCODE))
#define   IO$_ACCEPT_WAIT (IO$_ACCESS | (10 << IO$S_FCODE))
#define   IO$_NETWORK_PTY (IO$_ACCESS | (11 << IO$S_FCODE))
#define   IO$_SHUTDOWN    (IO$_ACCESS | (12 << IO$S_FCODE))
#define   IO$_GETSOCKNAME (IO$_ACCESS | (13 << IO$S_FCODE))
#define	  SETCHAR_HANDOFF (1<<2)

#define   NFB$C_DECLNAME   0x15

#define TIMER_EFN 1
#define TERM_EFN  2
#define BUF_SIZE 2000

#define INITIALISED 0
#define ACTIVE_CONNECTION 1
#define PASSIVE_CONNECTION 2
#define LISTENING 3
#define HANDED_OFF 4

static struct fd_entry
{
  unsigned short int channel;	/* vms channel assigned to this socket */
  unsigned short int iosb[4];	/* returned status block */
  int fd_buff_size;		/* number of chrs in buffer still to be read */
  int accept_pending;		/* a call is waiting to be accepted */
  int connect_pending;		/* a connect is outstanding*/
  int connected;		/* this descriptor is connected */
  unsigned char *fd_buff;	/* pointer to buffer dyn assigned */
  unsigned char *fd_leftover;	/* pointer to any chrs still to be read */
  FILE *fptr;			/* we need to assgn a file ptr for stream io */
  int s;			/* socket number - needed in the ast's */
  int namelen;			/* our socket address name */
  union socket_addr name;
  short int fromdummy;		/* wg - accept wants an int - recvfrom wants a short!!*/
  short int fromlen;		/* the from socket address name */
  union socket_addr from;
  int tolen;			/* wg - sendto wants an int*/
  union socket_addr to;		/* the to socket address name */
  int passive;			/* still needed because of x25 close ambig */
  int backlog;			/* backlog - not handled well! */
  int domain;			/* domain of socket AF_INET or AF_X25 */
  int type;			/* type of socket stream or datagram */
  int protocol;			/* protocol of socket - ignored */
  int mbx_channel;		/* mailbox channel - needed for x25 */
  unsigned char mbx_buff[255];	/* mailbox buffer */
  unsigned short int miosb[4];	/* mailbox status block */
  int ncb_size;			/* x25 connection information */
  unsigned char ncb[128];
  unsigned char masklen;	/* x25 user data mask */
  unsigned char mask[16];
  int need_header;		/* x25 header field gives data status if req*/
  int send_type;		/* x25 data packet type eg more bit set etc */
  int status;			/* status of socket */
  int closed_by_remote;		/* flag for remote dropouts */
  int read_outstanding;		/* flag so we don't hang two reads */
  int cmu_open;			/* flag to say whether a cmu open was hung */
  int x25_listener;		/* flag to say we are an x25 listener */
  int oob_type;			/* handles interrupt messages */
  int mother;			/* mother socket for X25 accepts */
  int child;			/* child socket for X25 accepts */
  int no_more_accepts;		/* don't accept anymore calls */
  char int_data;		/* interrupt data - only 1 char supported */
  int non_blocking;		/* don't block on a read if no data */
  int sig_req;			/* generate SIGIO on data ready */
  struct itemlist rhost;	/* descriptor pointing to "p[].from" info for UCX */
  unsigned short ucx_accept_chan;	/* Channel returned by a UCX accept via hang_an_accept */
} p[32];

/* So we can handle select on terminal input.  */
static struct term_entry
{
  int chan;
  short int iosb[4];
  short int char_available;
  short int read_outstanding;
  char c[1];
} terminal =

{
  -1, 0, 0, 0, 0, 0, 0
};

#define CMU 1
#define WG  2
#define NONE 3
#define TGV 4
#define UCX 5
static int tcp_make = 0;

struct descriptor
{
  int size;
  char *ptr;
};

/* Initialize certain things 1st time thru.  */
static int p_initialised = 0;

/* A routine to point SIGALRM and SIGURG at.  */
static int 
si_dummy ()
{
}
static int (*alarm_function) () = si_dummy;
static int (*sigurg_function) () = si_dummy;

FILE *fdopen ();
static set_tcp_make ();
static char *getdevicename ();
$EOD
$!
$CREATE [.archie.vms]multi.opt
$DECK
sys$library:vaxcrtl.exe/share
multinet:multinet_socket_library.exe/share
$EOD
$!
$CREATE [.archie.vms]ucx.opt
$DECK
sys$library:vaxcrtl.exe/share
sys$library:ucx$ipc/lib
$EOD
$!
$CREATE [.archie.vms]woll.opt
$DECK
sys$library:vaxcrtl.exe/share
twg$tcp:[netdist.lib]twglib.olb/lib
$EOD
$ write sys$output "Ok, now enter the ARCHIE directory, look at MAKE.COM, then type @MAKE ."

CMUIP, Networking, VAX Only TCP/IP implementation


		CMU OpenVMS TCP/IP

		Frequently Asked Questions

		Last Update: 27-OCT-1994

		FAQ Maintainer:  Andy Harper
   		                 A.Harper @ kcl.ac.uk

--------------------------------------------------------------------------------

This document is a set of Frequently Asked Questions (FAQ) About the CMU
OpenVMS TCP/IP (hereinafter referred to as OpenCMU) package originally
developed for VMS by Carnegie Mellon University and Tektronix. It is updated on
an irregular basis, as new FAQs arise, and is posted on a monthly basis to the
OpenCMU mailing list. The updated version may be obtained through anonymous FTP
from `FTP.KCL.AC.UK' in directory [.CMU-TCPIP] as file CMU.FAQ.

Each FAQ section begins with >>>> followed by the title of the section and the
source of the answer, where known. Use of SEARCH on this document allows
FAQ titles to be located.

Please notify the maintainer of any omissions, out-of-date or incorrect
information.

























Changes Since OCT 1994 Edition.
-------------------------------

27-OCT-1994	Add details of the FSP client/server software
11-OCT-1994	Rewrite section on telnet hanging
11-OCT-1994	Rewrite OpenCMU prerequisites section
11-OCT-1994	Change layout on all sections
10-OCT-1994	All relevant references to CMU changed to OpenCMU !
10-OCT-1994	Add note on OpenCMU under OpenVMS 6.1
10-OCT-1994	Reorganize software details by function; rewrite some entries
10-OCT-1994	Add details of SOCKIT - a general socket library for OpenVMS
3-OCT-1994	Add details of SOCKETSHR - socket interface to NETLIB


Changes Since AUG 1994 Edition.
-------------------------------

10-AUG-1994	Add hint on location of NETERROR.OBJ file
3-AUG-1994	Fix minor typos
2-AUG-1994	MX now at revision 4.1; updated filenames aaccordingly


Changes Since JUL 1994 Edition.
-------------------------------

1-AUG-1994	Add more info on SLIP connections
11-JUL-1994	Correct instructions for use under OpenVMS 6.x
5-JUL-1994	Corrected use of CONVERT/FDL to modify BACKUP saveset format


Changes Since JUN 1994 Edition.
-------------------------------

4-JUL-1994	Updated locations for WWW HTTP server
15-JUN-1994	Making OpenCMU work with OpenVMS 6.0
15-JUN-1994	Add details of ARCHIE software for OpenCMU
15-JUN-1994	Add details of IPADDR software for OpenCMU
3-JUN-1994	Correct typo in MGFTP description
3-JUN-1994	MGFTP is freeware, not public domain



Changes Since MAY 1994 Edition.
-------------------------------

24-MAY-1994	Add details of LYNX package
16-MAY-1994	Rewrite GOPHER details.
16-MAY-1994	Update details of FTP patch kit
16-MAY-1994	Update OpenCMU overview LPD/LPRSYMB description
16-MAY-1994	How to print OpenCMU IP Error message texts
16-MAY-1994	Info on IPACP BYTLM quotas
11-MAY-1994	Tidy up software availability tables
11-MAY-1994	Add details of MGFTP - MadGoat FTP - product
6-MAY-1994	Add details of MG_FINGER - MadGoat Finger - product



Changes Since APRIL 1994 Edition.
---------------------------------

14-APR-1994	Update Anonymous FTP address of UK mirror site
12-APR-1994	Correct directory of spanish mirror site

























--------------------------------------------------------------------------------


			C O N T E N T S


	1.0	GENERAL INFORMATION . . . . . . . . . . . . . . . .
		1.1 What is OpenCMU . . . . . . . . . . . . . . . .
		1.2 Obtaining OpenCMU . . . . . . . . . . . . . . .
		1.3 PreRequisites . . . . . . . . . . . . . . . . .
		1.4 Current Versions  . . . . . . . . . . . . . . .
		1.5 The OpenCMU Mailing List/Newsgroup  . . . . . .

	2.0	COMMONLY ASKED `SETUP' QUESTIONS  . . . . . . . . .
		2.1 Setting Up DECwindows over OpenCMU IP . . . . .
		2.2 Setting up a SLIP connection  . . . . . . . . .
		2.3 More on setting up SLIP . . . . . . . . . . . .
		2.4 Setting up an Anonymous FTP server  . . . . . .
		2.5 Setting up OpenCMU on OpenVMS 6.0 and 6.1 . . . . .

	3.0	KNOWN PROBLEMS  . . . . . . . . . . . . . . . . . .
		3.1 Why is FTP so slow  . . . . . . . . . . . . . .
		3.2 Why does FTP crash with `exceeded quota'  . . .
		3.3 Why doesn't the NFS server work . . . . . . . .
		3.4 Why does TELNET sometimes hang in `RWAST' . . .
		3.5 Why does TELNETing into cmu hang  . . . . . . .
		3.6 Name Resolver gives referral limit exceeded . .
		3.7 IPACP Issues status codes to OPCOM  . . . . . .
		3.8 IPACP crash due to insufficient quota . . . . .
		3.9 FTP of BACKUP savesets gives CRC errors . . . .
	
	4.0	PUBLIC DOMAIN SOFTWARE THAT SUPPORTS OPENCMU  . . .

		4.1 TCP/IP Transport Interface Libraries  . . . . .
		    4.1.1 NETLIB  . . . . . . . . . . . . . . . . .
		    4.1.2 SOCKETSHR . . . . . . . . . . . . . . . .
		    4.1.3 LIBCMU  . . . . . . . . . . . . . . . . .
                    4.1.4 SOCKIT  . . . . . . . . . . . . . . . . .

		4.2 Mail Applications . . . . . . . . . . . . . . .
		    4.2.1 MX  . . . . . . . . . . . . . . . . . . .
		    4.2.2 POP3 Server . . . . . . . . . . . . . . .

		4.3 News Applications . . . . . . . . . . . . . . .
		    4.3.1 ANU NEWS  . . . . . . . . . . . . . . . .
		    4.3.2 NEWSRDR . . . . . . . . . . . . . . . . .
		    4.3.3 FNEWS . . . . . . . . . . . . . . . . . .

		4.4 World Wide Web Applications . . . . . . . . . .
		    4.4.1 MOSAIC  . . . . . . . . . . . . . . . . .
		    4.4.2 LYNX  . . . . . . . . . . . . . . . . . .
		    4.4.3 HTTP_SERVER . . . . . . . . . . . . . . .

		4.5 File Transfer Applications  . . . . . . . . . .
		    4.5.1 MADGOAT FTP . . . . . . . . . . . . . . .
		    4.5.2 C-KERMIT  . . . . . . . . . . . . . . . .		
		    4.5.3 FSP . . . . . . . . . . . . . . . . . . .

		4.6 Network Archive Search Applications . . . . . .
		    4.6.1 ARCHIE  . . . . . . . . . . . . . . . . .

		4.7 Gopher Applications . . . . . . . . . . . . . .
		    4.7.1 GOPHER  . . . . . . . . . . . . . . . . .

		4.8 Finger Applications . . . . . . . . . . . . . .
		    4.8.1 MADGOAT FINGER  . . . . . . . . . . . . .

		4.9 Domain Name Server Applications . . . . . . . .
		    4.9.1 NSQUERY . . . . . . . . . . . . . . . . .
		    4.9.2 IPADDR  . . . . . . . . . . . . . . . . .

















1.0	GENERAL INFORMATION
---------------------------


1.1 >>>> WHAT IS OPENCMU				[11-OCT-1994]
------------------------

The OpenCMU software provides a full TCP/IP network transport for VAX systems
running the VMS operating system. This allows a VMS system to participate in
the world wide Internet network and take advantage of the wealth of information
and software available on it.

OpenCMU includes the following components as standard:

   o  TELNET for interactive access to other systems on the internet

   o  TELNET Server for allowing interactive login to the system running
      OpenCMU

   o  FTP for transferring files directly to and from other systems on the
      internet

   o  FTP Server for allowing file transfer to and from the system running
      OpenCMU. An ANONYMOUS login facility is included.

   o  FINGER for identifying users logged on on other systems on the internet

   o  FINGER Server for allowing others to finger users logged in to the system
      running OpenCMU

   o  TALK for interactive converstations with other users on the internet (a
      sort of network PHONE)

   o  TALK Server for allowing other users to talk to anyone logged on to the
      system running OpenCMU.

   o  LPD for processing incoming print file requests and status from clients.

   o  LPRSMB for submitting files to printers attached to other systems on the
      internet.

   o  IPNCP for monitoring and controlling the TCP/IP system

   o  UNIXSHR - A TCP/IP socket library

   o  DECW$TRANSPORT_CMU - shareable DECwindows transport for running X11
      applications over OpenCMU TCP/IP.


Support is provided for:

   o  IP over ethernet

   o  IP Over X.25

   o  IP Over DECnet

   o  IP over Serial lines (SLIP)


At this time, there is no support for:

   o  Compressed SLIP

   o  PPP

   o  Rlogin, Rexec, Rsh


Also Supported:

   o  Electronic mail is supported via the Freeware packages MX and NETLIB (see
      elsewhere in this FAQ for details).

   o  Domain Name Service is supported through the freeware DOMAIN package
      by Bruce Orchard. Usually available from the same sites as OpenCMU.


Note:

   o  Electronic mail support is also provided by the chargeable PMDF package
      available from Innosoft. Contact them for details.
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

1.2 >>>> OBTAINING THE OPENCMU SOFTWARE			[11-OCT-1994]
---------------------------------------

The OpenCMU software is entirely free of charge and may be obtained through
your local DECUS representative. The last relevant symposium tape that contains
the OpenCMU software is the Fall 1992 tape, in directory [VAX92B.CMU] and on
DECUS CDROM #12.

A number of sites on the Internet maintain up to date anonymous FTP directories
containing the OpenCMU software. These can be accessed using the FTP program or
a mail server such as FTPMAIL that can transfer files from an anonymous FTP
account back to the requestor via e-mail.

The following systems maintain up to date OpenCMU kits:

   Master Sites:

	SiteName		Directory	Maintainer
        SACUSR.MP.USBR.GOV	[.CMUIP]]	Henry Miller <HENRYM@SACTO.MP.USBR.GOV>
				[.TEKIP.FTP]
				[.TELNET]

   Mirror Sites:

	SiteName		Directory	Maintainer
	FTP.KCL.AC.UK		[.CMU-TCPIP]	Andy Harper <UDAA055@KCL.AC.UK>

	FTP.CSUS.EDU		pub/cmuip	???

	DMC.COM			[.VMS.CMUIP]	???

	marduk.iib.uam.es	/pub/VMS/cmutek-ip J.R.Valverde <sistema@biomed.iib.uam.es>

The first time you obtain OpenCMU, you will very likely not have an existing
network facility and hence will be unable to obtain the software across the
network. In this case, you should contact your local DECUS representative for a
copy of the software on a suitable media.

Subsequently, you can pick up new versions and patches using the network from
many of the sites listed above. Announcements about these are made to the
OpenCMU mailing list.
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

1.3 >>>> PREREQUISITES					[11-OCT-1994]
----------------------

Before installing OpenCMU on your system, take note of the following
requirements.

Hardware Requirements:

   A Digital VAX system.
   NOTE: Digital ALPHA systems are NOT currently supported.

   A Network connection.
   OpenCMU supports the following types of network interface:
    * Ethernet
    * Serial line (using SLIP)
    * X.25 Synchronous interface (using IP tunnelling)

   A network link to the outside world.
   The network interface must be connected to a network with at least one other
   network aware system connected to it. Consult your local site management or
   network service providers for details of how to connect to the rest of the
   IP world (the `Internet').

   Each interface to the network MUST be allocated its own unique IP address
   (your service provider will supply this) and a subnet mask.

   Although optional, it is highly likely that access to an IP router and a
   Domain Name Server will be required. Ask your service provider for the IP
   addresses of each of these.


Minimum VMS:

   The latest version of OpenCMU requires VMS 5.2 and upwards.

   There are versions of OpenCMU available that run on earlier versions of VMS.
   Poke around some of the FTP sites listed elsewhere to see what's available
   as some sites do not carry old versions.
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

1.4 >>>> CURRENT VERSIONS OF OPENCMU			[11-OCT-1994]
------------------------------------

Base Version:

	OpenCMU 6.6-5		Kit:	CMUIP066.% { % = A,B,C,D }


Update Kits:

	OpenCMU 6.6-5A		Kit:	TEKIP0665A.SAVE
	Telnet 5.0-1		Kit:    TELNETU1050.A
	FTP 2.12		Kit:    FTPU0212.A
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

1.5 >>>> THE OPENCMU MAILING LIST/NEWSGROUP		[11-OCT-1994]
-------------------------------------------

An electronic mailing list and news group exists for exchanging information
about the OpenCMU software. This is the preferred way to exchange information
about problems, and their solutions, and to announce updates to the software.
By subscribing, you gain access to a wealth of practical information from other
users and the relevant OpenCMU experts.

The address of the electronic mailing list, to which all enquiries or
announcements are directed, is:

   CMU-OpenVMS-IP@DRYCAS.CLUB.CC.CMU.EDU


Subscribe by sending a message to:

   CMU-OpenVMS-IP-Request@DRYCAS.CLUB.CC.CMU.EDU


For those with access to USENET, the world-wide electronic NEWS system, this
mailing list is automatically gatewayed to the newsgroup:

   vmsnet.networks.tcp-ip.cmu-tek

You are recommended to subscribe to the newsgroup wherever possible, in
preference to the electronic mailing list.  Details of some suitable news
readers can be found elsewhere in this document.
							<A.Harper@kcl.ac.uk>
							<Mark Berryman>
							<Tillman@swdev.si.com>











2.0	COMMONLY ASKED `SETUP' QUESTIONS
----------------------------------------

2.1 >>>> SETTING UP DEC WINDOWS OVER OPENCMU		[11-OCT-1994]
--------------------------------------------

NOTE: This summary applies to the LATEST version of OpenCMU. There may be minor
differences applying it to earlier versions.

 +===========================================================================+
 | Example of the installation of dec$transport_cmu of OpenCMU V6.6-5A       |
 +===========================================================================+
 
1.  Copy DECW$TRANSPORT_CMU.exe to sys$common:[syslib]decw$transport_cmu.exe
   ( Note:  Although the change of $ to _ is required (see 5.3 upgrade and 
   installation procedures section 10.4, p.10-)7, I didn't do so. But it works)
 
2.  Set protections on the file as -- S:rwed,O:rwed,g:rwed,w:re
 
3.  Add the following record in IP_STARTUP.COM if it is not
    already there:

       $ install create sys$share:decw$transport_cmu.exe /open/share/header/prot
 
4.  Customize decw$private_server_setup.com to have the following line:

       $ decw$server_transports == "DECNET,LOCAL,LAT,CMU"
 
    Eg. Copy decw$private_server_setup.template to *.com in Sys$common:[sysmgr]
        directory and add one record as follows;
 
       $do_default: 
  >>>  $ decw$server_transports == "DECNET,LOCAL,LAT,CMU"
       $ exit  
 
5.  Reboot workstation. If IP is already setup and running, it should be
    sufficient to merely restart DECwindows via the following command:

       $ @sys$manager:decw$startup restart
 
6. Security Entry on VAX/VMS+CMU
 
     If you are using VAX/Station(VMS+CMU) and you want to create a window on 
   the VAX/Station (i.e. you want to use it as a "server"), it is required to 
   customize security by adding as  "authorized user"  the OpenCMU transport for 
   the users and machines desired.
 
   (Select security in the setup menubar of VAX/Station and add following entry
      NODE:      IP-address or domain-name  (Eg. 134.160.1.1)
      USER nam   ? or *                     (Eg. *)
      Transport: CMU                        (Eg. CMU)
 
     If you want use your VAX only as a client or your VAX is not workstation,
   it is not necessary to define the security entry.

6.1  Security entry on UNIX workstation (eg. SUN + X11 Release 4)

     If you want to communicate with a UNIX workstation running X11R4, it is 
    also required to define the security entry of the hostname on the UNIX.
    There are two ways to define the security entry 
      (1) Write hostname in   /etc/X*.hosts  file. (* is display number)
      (2) Define hostname by "xhost" command. (this works only in the local
          terminal. This does not work on the telneted terminal)
    You can get more information by "man X" and "man xhost" command in UNIX.

    X11 Release 4 entire kit and its patch are available for anonymous 
    ftp from expo.lcs.mit.edu in pub/R4 directory 

7.  Then you can create a X-window from UNIX or on UNIX
 
      Eg. 1 (create a X-terminal on VAX/Station from SUN [SUN OS 4.0.3+X11R4] )
 
      (%  xhost  hostname              (authorize hostname, see 6.1))
       %  setenv DISPLAY hostname:0    (define display)
       %  xterm &                      (Create a X-terminal of sun on VAX/VMS)
 
      Eg. 2  (create a DEC-terminal on SUN from VAX/Station)
 
       $ set disp/cre/node="hostname"/tran=CMU
       $ cre/term/det
  
PS: And another one of Bruce's unsolicited tips:
 
   (Following is applied only for X11-server [i.e. VAX/Station] )
    After you bring up the transport with DECWindows, do a
    NETSTAT to see if the transport was initialized to wait for
    incoming connections.  You should see a TCP port at port 6000
    in the LISTEN state.  If not, you've done something wrong.
 
     Example
 
      $ IPNCP
      IPNCP> netstat
       1 TCP connection found
       IDX  Address       Local Host    Port    Foreign Host    Port  State
         2  0004C188         0.0.0.0  23.112         0.0.0.0     0.0  LISTEN
       0 UDP connections found
							<HENRYM@SACUSR.MP.USBR.GOV>
 
--------------------------------------------------------------------------------

2.2 >>>> SETTING UP A SLIP CONNECTION			[11-OCT-1994]
-------------------------------------

Ensure that the SLIP Driver (SLDRV.EXE) is in the CMUIP_ROOT:[SYSLIB]
directory.

To create a slip line include the following in your INET$CONFIG:

   Device_Init:SLDRV:dev:NONE:ip-address:ip-mask

Where:
	DEV is the terminal line to use eg. TXA1 ...
	ip-address is the Ip-address of the line.
	ip-mask is the IP-mask of the line.


Here is an example line:

   Device_Init:SLDRV:TXA0:NONE:123.123.123.123:255.255.0.0

        Defines a slip port on TXA0 having an IP address of
        123.123.123.123

Then kill and restart IPACP:

   IPNCP
   IPNCP> netexit
   IPNCP> Startup/ipacp

As far as I know there is no dynamic SLIP available.
However there is a new IPACP/IPNCP in the works that will allow you to
add/enable/disable SLIP lines on the fly.
						<CLEMENT@PHYSICS.RICE.EDU>

--------------------------------------------------------------------------------

2.2 >>>> MORE ON SETTING UP SLIP			[11-OCT-1994]
--------------------------------

The information below has been culled mostly from personal experience (i.e.,
it works for me).  All IP addresses have been changed.  The reason for this is
that OpenCMUIP SLIP doesn't require a password -- if you know the IP address of
the SLIP interface, and the phone number to dial, you can get in.  Also, any
explanations given below are not necessarily completely correct; I've tried to
put everything in simple terms that I understand.  Those caveats given, here
goes... 

First, in INET$CONFIG, you need to have *two* separate interfaces: one 
that talks to the SLIP "network", and one that talks to the Ethernet 
"network".  These two interfaces each have their own addresses.  If 
they "overlap" (this will be defined later), the interface to the more 
general network must come *last*.

Example: in my configuration (remember: *all* ip addresses have been 
changed; OpenCMUIP SLIP doesn't bother to ask for a password for 
connection purposes, so anyone who knows the phone number of your 
modem and the IP address of your SLIP interface can connect through 
your system), I have a VAX with an Ethernet interface at IP address 
128.97.101.101.  We use subnetting here at UCLA, and our network mask 
is 255.255.255.0.  This means that my VAX can directly connect to any 
computer whose address is 128.97.101.x, where x is in the range 0-255 
(although I believe the first and last are off limits).  To get to any 
computer with an address not in this range, I need to go through a 
gateway.  In my case, the gateway is at 128.97.101.105.  Note that I 
can directly connect to this computer.  This is, of course, important.  
So, I need a line in INET$CONFIG that says something like

Device_Init:ETHER:ESA0:00-00-00-00-00-00:128.97.101.101:255.255.255.0

and further down, one that reads 

Gateway:GATEWAY.PHYSICS.UCLA.EDU:128.97.101.105:0.0.0.0:0.0.0.0:

Now, I want to start a SLIP interface with a modem attached to the 
device TTA2.  I am assigned the address range 128.97.101.120 through 
128.97.101.127.  This is a total of eight addresses, but again, the 
first and last are not usable for reasons I don't understand (I think 
the operative terms are "network address" for the .0 one and 
"broadcast address" for the .127 one).  At this point, the six 
remaining addresses belong to *me*, and are mine to assign as I wish.  
I choose to assign the first one to the SLIP interface itself:

slip.physics.ucla.edu = 128.97.101.121

I have a computer at home (say it's a PC).  I choose to assign this 
the next address:

mypc.physics.ucla.edu = 128.97.101.122

These names must be established with the network administrator, but I 
am free to dole out the addresses to whomever I choose.

Note that by the rules above, the ethernet connection can speak 
directly to mypc.  This is incorrect, since that must go through the 
SLIP interface.  This is what I have called overlap above.  The range 
of SLIP addresses (.120 - .127) lies within that of Ethernet addresses 
(.0-.255).  We therefore need a way to tell OpenCMUIP to use the SLIP 
interface for the addresses .120-.127, and the Ethernet interface for 
all the others.  We do this simply by placing the SLIP device 
definition first, so that the Device_Init lines now look like:

Device_Init:SLDRV:TTA2:slip.physics.ucla.edu:128.97.101.121:255.255.255.248
Device_Init:ETHER:ESA0:00-00-00-00-00-00:128.97.101.101:255.255.255.0

The only new thing here is the mask on the SLIP interface.  That mask 
has all bits set to 1 except the last three.  This means that there 
are eight addresses (2^3) that the SLIP interface can access, which 
work out to .120-.127.  If OpenCMUIP wants to connect to one of these 
addresses, it does it through the SLIP interface.  If it wants to talk 
to anything else with an address of 128.97.101.x, it does it through 
the Ethernet interface.  Any other address is contacted via the 
gateway (which itself is contacted via the Ethernet interface).

The only other thing to do is to set the IP_Forwarding flag.  This 
allows OpenCMUIP to transmit packets from one interface to the other.  
without this, you couldn't use SLIP to get to the outside world: 

Variable:IP_Forwarding:1
 
That should be it.  Restart OpenCMUIP, and all should be well.  One other 
thing: many SLIP packages on the PC/Mac side expect to give the system 
some kind of password upon connection, and will fail if they don't 
receive a response from the VAX.  This needs to be turned off, since 
the VAX won't do a thing other than just sit there.
							<price@uclapp.physics.ucla.edu>

--------------------------------------------------------------------------------

2.4 >>>> HOW TO SET UP ANONYMOUS FTP			[11-OCT-1994]
------------------------------------
Note: following information taken from the FTP client release notes, with minor
editing to add additional info.


2.4.1 RPI Modifications

2.4.1.1	V2.7-5, 20-JUN-1989, Madison

> All FTP_ANON logical names should now be placed in the logical name
  table FTP_NAME_TABLE, to get them out of the system name table.  To
  do this, add the following lines to your IP_STARTUP.COM:

  $ CREATE/NAME_TABLE/EXEC/PROT=(S:RWED,O:RWED,G:R,W:R)-
    	/PARENT=LNM$SYSTEM_DIRECTORY FTP_NAME_TABLE
  $ FTPDEF := DEFINE/TABLE=FTP_NAME_TABLE/EXEC/NOLOG

  then use FTPDEF to define the FTP_ANON... logical names, for example:

  $ FTPDEF FTP_ANONYMOUS_DIRS USER$:[ANONYMOUS...]
  $ FTPDEF FTP_ANON_LOAD_THRESHOLD "0.5"
  $ FTPDEF FTP_ANON_PRIME_DAYS "2,3,4"  ! Tuesday, Wednesday, Thursday

> Added system load checking on anonymous logins if LAV0 device is
  available.  To enable, define the following logical names in FTP_NAME_TABLE:

    FTP_ANON_LOAD_THRESHOLD 	some floating-point number between 0.0 and 1.0.
    FTP_ANON_PRIME_DAYS	    	day-numbers -- indicate "prime time" days
    FTP_ANON_PRIMETIME_START	time-of-day -- indicates start of "prime time"
    FTP_ANON_PRIMETIME_END  	time-of-day -- indicates end of "prime time"
    FTP_ANON_TIME_ZONE	    	any character string indicating local time zone

  The only required logical is FTP_ANON_LOAD_THRESHOLD.  If that logical name
  exists and the LAV0 device exists, the load checking code is used.  The
  code does the following:

    If FTP_ANON_PRIME_DAYS is defined, it is translated.  The comma-separated
    list of numbers (where 1=Monday, 2=Tuesday, etc.) is used to identify
    the days in which "prime time" is effective.  If it does not exist,
    "prime time" is assumed to be in effect Monday through Friday.
    Note: Use ONLY numbers 1 through 7, and NO SPACES in the string.  Surround
    the string with quotation marks when defining!

    If FTP_ANON_PRIMETIME_START is defined, it is translated and converted
    into a system date-time value using LIB$CONVERT_DATE_STRING.  If not,
    then 09:00 is used as the start of "prime time".

    If FTP_ANON_PRIMETIME_END is defined, it is translated and converted
    into a system date-time value using LIB$CONVERT_DATE_STRING.  If not,
    then 17:00 is used as the end of "prime time".

    If the current time is between the prime-time start and end times,
    then the current load averages are read from the LAV device.  The
    current load is computed using the following formula:
    	    	    load = M15 * (P15 / 4.0)
    where M15 is the average load over the last 15 minutes, and P15 is
    the average priority over the last 15 minutes.  Thus, the average load
    is normalized against typical interactive priority to guard against
    low-priority batch jobs preventing guest login access.

    If the load is greater than or equal to the LOAD_THRESHOLD value, then
    the guest login is denied with a reason of "system too busy".
    If the threshold is not exceeded, then the guest login is accepted, but
    the user is warned to minimize access during prime time (with the
    start and end times displayed along with the time zone information
    [if FTP_ANON_TIME_ZONE is defined]).

    If the current time does not fall within prime time, no load checking
    is performed.

2.4.1.2   V2.7-4, 09-JUN-1989, Madison

> Added special messages to FTP server during guest (anonymous) login.
  Modified the logging of anonymous sessions slightly.

2.4.1.3   V2.7-2, 03-APR-1989, Madison

> The FTP server presents a somewhat more informative banner on connection--
  includes system name and version of FTP.

> The code that handled directory changes was really ugly, even though
  it had been modified to fix the infinite-loop problem from V2.6.  I
  replaced the code with some which makes use of available VMS system
  services, simply to satisfy my own sense of aesthetics.

> Enhanced the Anonymous FTP support provided by OpenCMU.  The enhancements
  include:

    * ANONYMOUS is never allowed privileges regardless of the contents of
      its UAF record.

    * All ANONYMOUS FTP sessions create logs.  Each session creates a
      file SYS$MANAGER:ANON_FTP_LOG.LOG.  You can put them elsewhere by
      defining ANON_FTP_LOG system-wide to a different location.  The
      password given to ANONYMOUS is logged along with the remote host's
      name and address, as well as RETR, LIST, NLST, CWD, and CDUP commands.
      The log files need not be accessible to the ANONYMOUS userid (and
      probably should not be).

      NOTE: It appears that the anonymous log file is ONLY created in
      SYS$MANAGER if the ANON_FTP_LOG logical name is explicitly defined. By
      default, no log file gets created.  Use:

	$ define/system ANON_FTP_LOG sys$manager:ANON_FTP_LOG.LOG
         
    * You can restrict the directories to which ANONYMOUS has access by
      defining the system-wide logical name FTP_ANONYMOUS_DIRS to
      a search list of device/directory specifications.  Any RETR,
      LIST, or NLST will check against this list before going through normal
      system access checks.  This prevents ANONYMOUS from gaining access
      to people's files via WORLD access.  If you do not define
      FTP_ANONYMOUS_DIRS, the extra access checks do not take place.
      You can use [directory...] notation to allow access to the entire
      subdirectory tree below the specified directory.

The steps needed to set up a controlled Anonymous FTP are:

    1. Create a UAF record for ANONYMOUS.  Set it /NOINTER/NOBATCH/NONETWORK
       to prevent logins or DECnet use.  Set /FLAG=DISMAIL to prevent mail
       from reaching it.  Assign it a UIC that is unique and outside any
       existing group.  Give it a default device and directory.

Example: UAF> ADD ANONYMOUS/PASS=JUNK/NOINTERACTIVE/NOBATCH/NONETWORK-
    	    	/FLAG=DISMAIL/UIC=[666,666]/DEV=USER$DISK/DIR=[PUBLIC]

    2. Put the definition of FTP_ANONYMOUS_DIRS in your system startup
       sequence.  Make sure it is defined before allowing Anonymous access.
       Make sure that the default device/directory in the UAF is accessible
       (not strictly necessary, but easier on the users).

Example: $ DEFINE/SYSTEM FTP_ANONYMOUS_DIRS -
    	    	USER$DISK:[PUBLIC...],-	    ! public files
    	    	USER$DISK:[NEWS...],-	    ! news archives
    	    	USER$DISK:[MAIL]    	    ! mail archives

    3. Create the directories to which ANONYMOUS will have access.  Do not
       permit ANONYMOUS to own any of the files or be in the same group
       as the owner of the files.  Set WORLD:R protection on all files
       and directories to be accessible, or use an ACL to grant access 
       specifically to ANONYMOUS.

  While these modifications were meant to enhance the security of Anonymous FTP,
  neither the author nor his employer (nor anyone else for that matter)
  guarantees that the software is secure.
						<HENRYM @ SACUSR.MP.USBR.GOV>


NOTE:
  The Load Average Driver (LAVDRIVER) referred to in this article can be
  obtained from a number of public archive sites. Here are some possibilities:



Availability via Anonymous FTP:
===============================

--------------------------------------------------------------
Site		Directory		File(s)		Format
--------------------------------------------------------------

ftp.kcl.ac.uk	[.default]		lavdriver.*	VMS_SHARE

--------------------------------------------------------------


Availability via Electronic Mail:
=================================

--------------------------------------------------------------
Address				Command to send		Format
--------------------------------------------------------------

VMSSERV @ KCL.AC.UK		SEND LAVDRIVER.PACKAGE	VMS_SHARE

--------------------------------------------------------------

--------------------------------------------------------------------------------



2.5 >>>> SETTING UP OPENCMU WITH OPENVMS 6.0 AND 6.1	[11-OCT-1994]
----------------------------------------------------

Following the upgrade to OpenVMS 6.0 or 6.1, OpenCMU 6.6-5A can be made to work
by issuing the following sequence of commands:

 $ @sys$update:register_privileged_image register cmuip_root:[sys$ldr]ipdriver.exe
 $ @sys$update:register_privileged_image register cmuip_root:[sysexe]ipacp.exe
 $ @sys$update:register_privileged_image register cmuip_root:[sysexe]lpd.exe

Failure to do this will likely result in error messages similar to:

 "System version level mismatch"

and the network will fail to come up.
							<A.Harper@kcl.ac.uk>
							<Rynda@scfe.chinalake.navy.mil>
							<henrym@sacto.mp.usbr.gov>
							<srshmgr@m2g.gns.cri.nz>






3.0 KNOWN PROBLEMS
------------------

3.1 >>>> WHY IS FTP SO SLOW				[11-OCT-1994]
---------------------------

The version of FTP supplied with the master 6.6-5 kit suffers from a number of
bugs. One of these causes excessive error rates and retransmissions resulting
in a low throughput rate.

It is STRONGLY recommended that the 6.6-5A patch kit be applied. This greatly
improves the performance.

See also the freeware MGFTP software (described in more detail in the
`Software' section elsewhere).
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

3.2 >>>> WHY DOES FTP CRASH WITH `EXCEEDED QUOTA'	[11-OCT-1994]
-------------------------------------------------

FTP (client or server) can fall over with an `exceeded quota' message if the
SYSGEN parameter `MAXBUF' is not set correctly.  The latest recommendation is
for the minimum value to be 2300. In practice, however, a much larger figure of
8300 is recommended.

Note that transferring files with large records, exceeding MAXBUF, may still
cause problems.
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

3.3 >>>> WHY DOESN'T THE NFS SERVER WORK		[11-OCT-1994]
----------------------------------------

The NFS server broke with version 6.6-5 of OpenCMU.  At this time, there is no
workable solution. IF NFS is a requirement, version 6.6-4 is the last version
in which NFS works.
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

3.4 >>>> WHY DOES TELNET SOMETIMES HANG IN `RWAST'	[11-OCT-1994]
--------------------------------------------------

TELNET clients prior to version 5.0 could, under certain conditions, lock up a
process in an RWAST state. All users are strongly recommended to upgrade to
Version 5.0-1 of TELNET in which this problem, and others, are solved.
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

3.5 >>>> WHY DOES TELNETTING INTO OPENCMU HANG		[11-OCT-1994]
----------------------------------------------

When telnetting into a OpenCMU host, the system does not prompt for a username
until an extra carriage return appears. There are three known, unrelated,
causes for this problem.

First, a bug in earlier versions of the CMU telnet software is known to cause
unexpected hangs. To fix this, All users should install the latest patches to
OpenCMU (6.6-5A) and the telnet client.

Second, some PC telnet clients are known to contain problems that prevent them
successfully interworking with CMU telnet. PC-NFS telnet versions 4.x and 5.x
suffer from this problem.  To fix, avoid these clients - there are plenty of
reasonable alternative telnet clients around.

Finally, it may be the case that some PC telnet's do not correctly negotiate
the telnet options when the call is connected. One or other end can wait
indefinitely for the opposite end to continue. At this time, no clear solution
is known but the problem can sometimes be alleviated by adding the following to
the CMU INET$CONFIG file:

      Variable:TELNET_NEG_TIMEOUT:0

This causes telnet not to wait for negotiations to timeout, and can speed up
those logins which appear to hang for a long time.
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

3.6 >>>> NAMRES GIVES DOMAIN REFERRAL EXCEEDED MESSAGES	[11-OCT-1994]
-------------------------------------------------------

The name resolver can produce the message `Maximum domain referral limit
exceeded' and fail to resolve a name into its address. This is often indicative
of incorrect configuration of the name resolver.  Ensure that the following
lines are included in the NAMRES$CONFIG file:

   Variable:TIMEOUT:5
   Variable:REFMAX:10
   Variable:RECURSE:1

You might want to also add:

   Variable:NS_RETRANS:2


(NOTE: in table 3-8 of the last official manual, the last variable, labelled
TIMEOUT, should be labelled RECURSE. TIMEOUT is given correctly as the second
entry in the table).

Restart the name resolver if necessary:

   $ IPNCP
   IPNCP> NAMRES EXIT
   ....
   IPNCP> STARTUP /NAMRES
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

3.7 >>>> IPACP ISSUES STATUS CODES TO OPCOM		[11-OCT-1994]
-------------------------------------------

When the IPACP process (which coordinates the IP traffic) has problems, it can
issue system status codes to OPCOM. Here is a typical sequence:

   %%%%%%%%%%%  OPCOM  16-AUG-1993 10:49:23.75  %%%%%%%%%%%
   Message from user SYSTEM on XYZZY
   IPACP: XE status error.  Status = 00000A00

   %%%%%%%%%%%  OPCOM  16-AUG-1993 10:49:23.83  %%%%%%%%%%%
   Message from user SYSTEM on XYZZY
   IPACP: XE retried 5 times.

   %%%%%%%%%%%  OPCOM  16-AUG-1993 10:49:23.89  %%%%%%%%%%%
   Message from user SYSTEM on XYZZY
   IPACP: XE $QIO read error (dev_inact), RC=000020D4


To determine the exact problem, it is first necessary to translate the status
codes (00000A00  and  000020D4) into the more usual text form. The DCL lexical
function F$MESSAGE will translate them for you.  Here is a little command file
to make it easier:


   $! SHOWMSG.COM
   $! Usage: @SHOWMSG 20D4
   $	WRITE SYS$OUTPUT F$MESSAGE(%X'P1')


Typically, the messages are indicative of a problem with the ethernet itself or
with the ethernet controller; the status messages may help to determine the
root cause.

The message texts from OpenCMU are not part of the standard system message
files. For a translation of the error code into the text to be possible, the
user must have issued a SET MESSAGE command on the file  NETERROR.EXE.  The
installation of OpenCMU should have placed this in the SYS$MESSAGE directory.

If not, locate the file called NETERROR.OBJ in the CMUIP_ROOT:[*...] tree and
relink it to form the NETERROR.EXE, using this command:

   $ LINK/SHARE=SYS$COMMON:[SYSMSG]NETERROR NETERROR.OBJ


Following this, the message texts can be made available to F$MESSAGE using:

   $ SET MESSAGE SYS$MESSAGE:NETERROR

[Note: if, for any reason, NETERROR.OBJ does not exist in the directory tree, 
it can be found in the second saveset of the OpenCMU kit - CMUIP066.B]
							<dragon@nscvax.princeton.edu>
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

3.8 >>>> IPACP CRASH DUE TO QUOTA EXCEEDED		[11-OCT-1994]
------------------------------------------

For systems with a high IP load, IPACP may occasionally crash with a quota
exceeded. This does not refer to disk quota, but to one of the process quota
limits. Usually, the quota in question is BYTLM.

To increase the BYTLM for the IPACP, modify the IP_STARTUP.COM procedure and
change the value of the /BUFFER_LIMIT qualifier on the RUN command that starts
the IPACP process. Then shut down and restart IPACP.

At the current time, there appears to be a memory leak in IPACP which has the
effect of gradually reducing the available BYTLM over time. When this gets
close to zero, IPACP will hang (as it retries) and then crash soon afterwards.
It is therefore desirable to give IPACP more BYTLM than the typical load might
suggest. If this sort of crash is experienced, increase the BYTLM by 50% and
restart it.
							<A.Harper@kcl.ac.uk>

--------------------------------------------------------------------------------

3.9 >>>> FTP OF BACKUP SAVESETS GIVES CRC ERRORS	[11-OCT-1994]
------------------------------------------------

One major use of FTP is in transferring BACKUP savesets to/from other systems.
Often this leads to the recipient having difficulties unpacking that saveset;
in particular, using BACKUP to list or unpack it results in a stream of
messages similar to `CRC error' to the user's terminal and to OPCOM. This
article summarizes why the error occurs and how to correct it.

When BACKUP creates a saveset, it writes the file with a fixed length record
format - the length being that specified with BACKUP's /BLOCK qualifier. For
example:

    BACKUP/BLOCK=8192 * s.bck/save
         Creates a file with fixed length records of 8192.

When FTP is used, in binary mode, the data is sent correctly but the record
structure changes; typically, it is created with 512 byte records. Thus, when
BACKUP is used to list or unpack the file contents, it finds that the record
length of the file does not match the size used originally (this value is
stored in the BACKUP saveset header as well as in the file header).

If both ends of the FTP session support the special STRUC VMS mode of transfer,
then it should be used and the file will transfer correctly. If this structure
is not supported, the record structure becomes corrupted and must be manually
`fixed up' before BACKUP can be used.

There are three ways in which this can be done.

 1.	Use the VMS CONVERT utility to alter the record structure:

	   $ CONVERT/FDL=SYS$INPUT  file.bck  file.bck
	   RECORD
	     FORMAT FIXED
             SIZE nnnn
	   ^Z

	Where 'nnn' is the record size used on the original BACKUP command.

	NOTE: a new copy of the file is made.


 2.	Use the public domain utility called FILE (courtesy of Joe Meadows):

	   $ FILE/RECORD_SIZE=nnn file.bck

	Where 'nnn' is the record size used on the original BACKUP command.

	NOTE: this utility does NOT make a copy of the file; instead it patches
	the file header directly. It is wise to make a backup copy before using
	this technique!!!



3.	Use the public domain utility called FIX_SAVESET (author unknown):

	   $ FIX_SAVESET file.bck

	This utility scans the file, on the assumption that it is a backup
	saveset; picks out the original record length from the backup saveset
	header stored in the file; and finally, patches the file header record
	size back to this length. A new copy of the file is not made.


Both the FILE and FIX_SAVESET utilities are available from a number of public
archives using anonymous FTP. Here are some possibilities:



Availability via Anonymous FTP:
===============================

--------------------------------------------------------------
Site		Directory		File(s)		Format
--------------------------------------------------------------

ftp.kcl.ac.uk	[.default]		fix_saveset.*	VMS_SHARE
ftp.kcl.ac.uk	[.joemeadows]		file.*		VMS_SHARE

--------------------------------------------------------------


Availability via Electronic Mail:
=================================

--------------------------------------------------------------
Address				Command to send		Format
--------------------------------------------------------------

VMSSERV @ KCL.AC.UK		SEND FIX_SAVESET.PACKAGE VMS_SHARE
VMSSERV @ KCL.AC.UK		SEND FILE.PACKAGE	VMS_SHARE

--------------------------------------------------------------


Finally, to summarize the correct method of transferring a BACKUP saveset using
FTP:

  1.	If both ends support STRUC VMS, then

	a.	ftp> SET STRUC VMS
	b.	ftp> GET file

	File will be stored locally with the correct attributes.

  2.	If STRUC VMS is not supported by one or both ends, then

	a.	ftp> BINARY
	b.	ftp> GET file

	Once file arrives on the VMS system:

	c.	FIX_SAVESET file
							<A.Harper@kcl.ac.uk>














4.0 >>>> PUBLIC DOMAIN SOFTWARE THAT SUPPORTS OPENCMU
-----------------------------------------------------

In this section is described a number of network applications which are known
to work with the OpenCMU transport.

Volunteers are always required to port other network applications to the latest
OpenCMU release. If you port anything, please notify the FAQ maintainer and the
OpenCMU mailing list. Also, please try to persuade the original author of any
ported application to include your OpenCMU changes in the official release.
This will greatly reduce the amount of work needed to track the latest releases
of software.



4.1 >>>> TCP/IP TRANSPORT INTERFACE LIBRARIES		[10-OCT-1994]
---------------------------------------------

There are a number of different TCP/IP transports available for OpenVMS. These
include:

   UCX			Digital's own TCP/IP
   OpenCMU		The Carnegie-Mellon University implementation
   Multinet		TGV Inc.
   Pathway Access	The Wollongong group
   TCPware		Process Software

For historical reasons, each package has a slightly different application level
interface, which makes writing portable TCP/IP applications harder than it
should be. The software in this section provide interfaces to one or more of
the available transports, providing a consistent interface for applications
irrespective of which TCP transport is in use. All adopt the widely recognized
`socket' interface used by BSD unix and others but in some cases the syntax
used is different. Using an interface with the same syntax as BSD sockets
should make it much easier to port programs between VMS and unix systems, and
any other systems that come along.

Some of the applications described in this section require a specific socket
library interface, so it may be necessary to install two or more of them on a
given system to get all the applications to run.

In terms of portability, the best socket libraries are `SOCKIT' and the
`SOCKETSHR/NETLIB' combination, as these interface to two or more of the
commonly available transports.
							<A.Harper@kcl.ac.uk>



4.1.1 >>>> NETLIB					[10-OCT-1994]
-----------------

Summary:
	A vendor independent TCP/IP programming interface.

Description:
	NETLIB solves the problem by providing a vendor-independent programming
	interface that sits between the application and the particular version
	of TCP/IP installed on the system. Thus, applications can be written in
	terms of NETLIB routines and will run over any transport supported by
	NETLIB.

Transports Supported:
	NETLIB supports OpenCMU; also, Multinet, TCPware, UCX and TWG's Win/TCP
	and Pathway Access. Anyone writing network applications should be
	strongly encouraged to use the NETLIB interface. 

Interface Type:
	Similar to BSD sockets in concept but not in syntax. See SOCKETSHR
	package.

Author:
	Matt Madison <Madison @ tgv.com>

Pre-Requisites:
	NONE, except for one of the supported TCP/Ip transports listed above.
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.spc.edu	[.macro32.savesets]	NETLIBnnn.zip	ZIP
	ftp.kcl.ac.uk	[.madgoat]		NETLIB.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND NETLIB.PACKAGE	VMS_SHARE
	FILESERV@ WKUVX1.WKU.EDU	SEND NETLIB		VMS_SHARE, MFTU, ZIP

	--------------------------------------------------------------

NOTE: `nnn' is a version number - replace with the latest version.



4.1.2 >>>> SOCKETSHR					[10-OCT-1994]
--------------------

Summary:
	A BSD sockets interface to NETLIB

Description:
	SOCKETSHR provides a complete socket library that allows applications
	to be written with complete independence of the underlying network
	transport. It is written to interface to the NETLIB software, which
	provides an interface to all the available TCP/IP transports for VMS.

	A recommended package for all users who are writing or porting network
	applications.

Transports Supported:
	All those supported by NETLIB

Interface Type:
	BSD sockets compatible

Author:
	Thanks go to Eckart Meyer for making this package available, and to
	Mike O'Malley, on whose LIBCMU package this is based.

Pre-Requisites:
	To use SOCKETSHR, the NETLIB package is a pre-requisite; to use
	SOCKETSHR with UDP applications requires NETLIB version 1.7 as a
	minimum!
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.ifn.ing.tu-bs.de
			[.VMS.SOCKETSHR]	SOCKETSHR*.ZIP	ZIP

	ftp.kcl.ac.uk	[.default]		SOCKETSHR.*	VMS_SHARE

	--------------------------------------------------------------



Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND SOCKETSHR.PACKAGE	VMS_SHARE

	--------------------------------------------------------------


4.1.3 >>>> SOCKIT					[10-OCT-1994]
-----------------

Summary:
	A socket library interface for VMS network applications

Description:
	SOCKIT provides an emulation of the BSD socket routines for VMS. The
	interesting thing about this package is that it will interface to
	several of the commonly available TCP/IP transports, OpenCMU included.

Transports Supported:
	CMU
	Wollongong
	UCX			(thus works with Multinet if UCX emulation on!)
	X.25

Interface Type:
	BSD sockets compatible

Author:
	Peter Kay

Pre-Requisites:
	NONE. The code to interface to each transport is built-in.
							<A.Harper@kcl.ac.uk>



Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.kcl.ac.uk	[.default]		SOCKIT.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND SOCKIT.PACKAGE	VMS_SHARE

	--------------------------------------------------------------




4.1.3 >>>> LIBCMU 					[10-OCT-1994]
-----------------

Summary:
	A socket library interface for OpenCMU


Description:
	LIBCMU is a purpose built library of routines for interfacing
	applications that use Berkeley sockets to the OpenCMU programming
	interface. This library allows a number of applications written for
	sockets to be easily ported to OpenCMU.

Transports Supported:
	CMU

InterFace Type:
	BSD sockets compatible

Author:
	Thanks go to Mike O'Malley for writing and maintaining the LIBCMU
	software.
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	kermit.columbia.edu
			vms-libcmu		ckvlcmu.hex	HEX of backup saveset
	ftp.kcl.ac.uk	[.cmu-tcpip]		LIBCMU.*	BACKUP

	--------------------------------------------------------------



4.2 >>>> MAIL APPLICATIONS				[10-OCT-1994]
--------------------------

Electronic mail is one of the main applications used over TCP/IP networks. It
allows messages to be sent from one user to another even though they are on
opposite sides of the world. Provided the users both have access to a computer
system running compatible mail software, messages can be sent easily.

OpenCMU does not provide any mail applications. Instead one or more of the
applications listed below are recommended.
							<A.Harper@kcl.ac.uk>




4.2.1	>>>> MX ELECTRONIC MAIL				[10-OCT-1994]
-------------------------------

Summary:
	A comprehensive SMTP based network mail system that interfaces directly
	into VMS MAIL and the underlying TCP/IP network.


Description:
	MX provides full SMTP mail support and interfaces to VMS MAIL.  It also
	provides a mailing list and file server facility.

	MX is completely free of charge and may be obtained from your local
	DECUS representative or from the sites listed below.

Pre-Requisites:
	MX requires the NETLIB interface library.

Author:
	Matt Madison <Madison @ tgv.com> and
	Hunter Goatley <goathunter@alpha.wkuvx1.wku>
	(C) MadGoat Software ltd.
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.spc.edu	[.mx.mx041]		mx041.*		BACKUP
	ftp.kcl.ac.uk	[.madgoat]		mx.*		VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND MX.PACKAGE		VMS_SHARE
	FILESERV@ WKUVX1.WKU.EDU	SEND MX041		VMS_SHARE,MFTU,ZIP

	--------------------------------------------------------------



4.2.2 >>>> IUPOP3					[10-OCT-1994]
-----------------

Summary:
	A POP3 mail server

Description:
	POP3 is a protocol that allows a PC user to download mail from a
	central mail server and read it on the PC using PC style interfaces. A
	client that understands the POP protocol must run on the PC and many
	public domain or shareware ones are available (Win/QVT, Eudora,
	PC-Eudora, POPmail, Pegasus Mail and MINUET to name but a few).

	IUPOP3 is a POP3 server that runs under a number of systems, including
	VMS, and runs over the OpenCMU TCP/IP software (also UCX and Multinet).


Pre-Requisites:
	IUPOP3 requires a specific library for CMU, INET_CMUTIL.
	Note the original Author of this software (see below) do NOT currently
	support a CMU version. The CMU port is a one off (Thanks to Brian T
	Carcich).

Author
	Indiana University
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.indiana.edu	/pub/vms/iupop3		v1.7/*		source
						v1.7-CMU-TEK/*
	ftp.kcl.ac.uk	[.iupop3]		iupop3-017.*	VMS_SHARE

	ftp.kcl.ac.uk	[.cmu-tcpip]		INET_CMUTIL.BCK	BACKUP
	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND IUPOP3-017.PACKAGE	VMS_SHARE

	--------------------------------------------------------------



4.3 >>>> NEWS APPLICATIONS				[10-OCT-1994]
--------------------------

Usenet news is a world wide distributed news system. Messages generated on one
system are passed around the world rapidly, to all other systems. To simplify
the management of news, messages are divided into `newsgroups', each newsgroup
concentrating on one general topic. Using an appropriate newsreader, a user can
`subscribe' to a particular set of newsgroups and read all the related messages
in a manner similar to mail. There are some 3000+ different user groups
currently.

There are a number of parts to the news system. Firstly, software is required
to gather batches of news from an upstream `feed' site and insert it into a
local news database; Second, software is required to allow users to read the
news database, possibly modifying it by sending new messages.

The news database can be on the same system as the user, or the news database
can be on a remote system, accessible through a client-server mechanism. The
user's news reader program becomes a client, using the network to access a news
server.
							<A.Harper@kcl.ac.uk>



4.3.1 >>>> ANU NEWS					[10-OCT-1994]
-------------------

Summary:
	A complete news system

Description:
	ANU-NEWS provides a complete package to deal with USENET news. News is
	received from an up-stream news feed site and stored in a local on-disk
	database. This database can be interrogated by local users running the
	NEWS application; A news server can be set up that provides access to
	the news databases via any convenient NEWS client running on another
	system (see NEWSRDR elsewhere in this document for one example).

	The ANU-NEWS server supports OpenCMU.


Author:
	Thanks go to Geoff Huston for writing and maintaining the ANU NEWS
	software.
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	kuhub.cc.ukans.edu
			[.ANU_Vnnn]		NEWS_Vnnn.ZIP	ZIP
	ftp.kcl.ac.uk	[.news]			news.*		VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND NEWS.PACKAGE	VMS_SHARE

	--------------------------------------------------------------

NOTE: `nnn' is a version number or version string; replace with the latest
version that appears in the directory.



4.3.2 >>>> NEWSRDR					[10-OCT-1994]
------------------

Summary:
	A VMS newsreader client


Description:
	NEWSRDR is a news client that allows the user to access the news groups
	stored on a news server system. This gives users a quick way of
	accessing all the news without the need to build a full news system.


Pre-Requisites:
	NEWSRDR requires the NETLIB library to interface to the underlying
	TCP/IP network.


Author:
	Thanks go to Matt Madison for writing and maintaining the NEWSRDR
	software. <madison @ tgv.com>
							<A.Harper@kcl.ac.uk

Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.spc.edu	[.macro32.savesets]	newsrdr.zip	ZIP
	ftp.kcl.ac.uk	[.news]			newsrdr.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND NEWSRDR.PACKAGE	VMS_SHARE
	FILESERV@ WKUVX1.WKU.EDU	SEND NEWSRDR		VMS_SHARE, MFTU, ZIP

	--------------------------------------------------------------




4.3.3 >>>> FNEWS					[10-OCT-1994]
----------------

Summary:
	A NEWS reading client for VMS

Description:

	FNEWS is another news reader client. It offers local caching of
	newsgroups to speed the downloading of messages.


Pre-Requisities:
	None: FNEWS builds for the currently installed transport.


Author:
	???
							<A.Harper@kcl.ac.uk>



Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	zephyr.grace.cri.nz
			/pub/fnews/vms		fnews.bck	BACKUP
	ftp.kcl.ac.uk	[.news]			fnews.*		VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND FNEWS.PACKAGE	VMS_SHARE

	--------------------------------------------------------------



4.4. >>>> WORLD WIDE WEB APPLICATIONS			[10-OCT-1994]
-------------------------------------

The world wide web is a distributed hypertext system that literally encompasses
the world. A document can be loaded from a remote server which contains
hypertext links to other documents anywhere in the world. Documents can be
text, graphics, sound, binary etc.

World Wide Web servers accept requests from clients to download documents, and
world wide web clients accept those documents and format them for the user's
display. A single display can be composed of a mix of text and graphics etc.

World Wide Web uses the HyperText Markup Language (HTML) to specify document
format and remote links.

World Wide Web links specify the location of the document (site, directory and
filename) as well as the protocol used to access them (ftp, wais, gopher, html
etc.). So world wide web combines the functionality of a number of client
types.
							<A.Harper@kcl.ac.uk>



4.4.1 >>>> MOSAIC					[10-OCT-1994]
-----------------

Summary:
	A graphical world wide web client that uses the X protocol over
	TCP/IP

Descrription:
	MOSAIC is a superb graphical interface for browsing through the World
	Wide Web and gopher databases on the internet. By using a hypertext
	markup language, text, images and sound can be pulled together,
	irrespective of their locations, into a single on-screen document. This
	is THE program for information seekers.


Pre-Requisites:
	X-windows is required to display the document.

	A socket library interface is required to build this program for the
	underlying network transport. For OpenCMU, the required library is
	LIBCMU.

Author:
	MOSAIC is written by the National Centre for Supercomputing
	Applications.
							<A.Harper@kcl.ac.uk>



Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.kcl.ac.uk	[.mosaic]		mosaic.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND MOSAIC.PACKAGE	VMS_SHARE

	--------------------------------------------------------------




4.4.2 >>>> LYNX						[10-OCT-1994]
---------------

Summary:
	A World Wide Web browser, designed for line mode terminals (such as
	Digital's VT series.

Description:
	LYNX is a line mode version of a World Wide Web hypertext browser. It
	combines the functions of gopher and FTP, together with WWW and allows
	access from a VT compatible terminal.  It provides similar
	functionality to that of Mosaic except that a simple terminal interface
	is all that is required.

Pre-Requisites:
	A socket library interface is required; LYNX recognizes a number of
	socket libraries. For OpenCMU, the LIBCMU socket library is required.

Author:
	Thanks go to the LYNX developers, mainly at the University of Kansas,
	for developing and maintaining this software, and for making it freely
	available.
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.kcl.ac.uk	[.lynx]			lynx.*		VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND LYNX.PACKAGE	VMS_SHARE

	--------------------------------------------------------------



4.4.3 >>> HTTP_SERVER					[10-OCT-1994]

Summary:
	A World Wide Web server conforming to the standard HTTP (HyperText
	Transfer Protocol) mechanism.

Description:
	HTTP_SERVER provides a VMS system with the ability to act as a World
	Wide Web server using the HTTP protocol. It will accept requests from
	HTTP clients (such as MOSAIC - see elsewhere in this document) and
	return the necessary information. A full description of the World Wide
	Web system is outside the scope of this summary but it is, in essence,
	a distributed hypertext system capable of mixing text, images,
	graphics, animation and sound into a single on-screen display, with
	each element being on different systems anywhere in the world. The HTML
	language is used to specify the links.

Pre-Requisites:
	None; the server contains all the necessary interfaces to work with
	OpenCMU, as well as with UCX and Multinet.

Author:
	Thanks go to David Jones for writing and maintaining this software.
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	osu.edu					http_server.tar	TAR
	ftp.spc.edu	[.macro32.savesets]	http_server.zip	ZIP
	ftp.wku.edu	[.vms.fileserv]		http_server.zip	ZIP
	ftp.kcl.ac.uk	[.default]		http_server.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND HTTP_SERVER.PACKAGE VMS_SHARE
	FILESERV @ WKUVX!.WKU.EDU	SEND HTTP_SERVER	VMS_SHARE,MFTU,ZIP
	--------------------------------------------------------------



4.5 >>> FILE TRANSFER APPLICATIONS			[10-OCT-1994]

File transfer is another major application run over the network. It allows
files to be transferred between two different systems using a simple set of
commands. It is most often used for retrieving files from one of the many
public domain archive sites around the world.

OpenCMU comes prepackaged with an FTP client and server but it is worth
considering the alternatives listed here.
							<A.Harper@kcl.ac.uk>


4.5.1 >>>> MADGOAT FTP					[10-OCT-1994]
----------------------

Summary:
	An alternative FTP client and server.

Description:
	The File Transfer program (or FTP) is an important part of the TCP/IP
	applications set. It allows files to be moved between two systems. 
	MGFTP is a file transfer program which can be used over any of the
	available TCP/IP transports, including OpenCMU.

	This client is more functional than the one provided with OpenCMU and is
	recommended. Useful enhancements include:
	  * Logging of server transactions to a file in each user's home
	    directory
	  * User control over how the server is used on an account
	  * Anonymous FTP has per-directory messages
	  * FTP client has automatic anonymous login
	  * FTP client can have aliases defined to connect/fetch from specific
	    systems/files

Pre-Requisites:
	The NETLIB interface is required.

Author:
	MGFTP is based on the OpenCMU FTP client and server, written by many
	people.

	Thanks go to Matt Madison and Hunter Goatley, of MadGoat software, and
	to Darrell Burkhead, for writing and maintaining MGFTP and making it
	available as freeware.
							<A.Harper@kcl.ac.uk>



Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.wku.edu	[.madgoat]		mgftp.zip	ZIP
	ftp.spc.edu	[.macro32.savesets]	mgftp.zip	ZIP
	ftp.kcl.ac.uk	[.madgoat]		mgftp.*		VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND MGFTP.PACKAGE	VMS_SHARE
	FILESERV@ WKUVX1.WKU.EDU	SEND MGFTP		VMS_SHARE,MFTU.ZIP

	--------------------------------------------------------------



4.5.2 >>>> C-KERMIT					[10-OCT-1994]
-------------------

Summary:
	File transfer over the current terminal connection to a host.

Description:
	The KERMIT program is a widely used way of tranferring files over
	serial lines between systems. The user's terminal temporarily becomes a
	client and the user's host session temporarily becomes a server. The
	KERMIT protocol allows switching between terminal mode and file
	transfer mode, as well as sending or requesting files to be transferred.
	In the past, kermit has been able to set up terminal sessions, and run
	the file transfers, only over serial line connections. More recent
	versions have allowed the terminal connections, and hence the file
	transfers, to take place over telnet links by having direct TCP/IP
	support built in.


	The latest version of C-kermit supports direct TCP/IP connections and
	will work over the OpenCMU package.

Pre-Requisites:
	The OpenCMU version requires a socket library specific to the transport
	on which it runs. For OpenCMU, the required socket library is LIBCMU.

Author:
	Columbia University and many contributors around the world.
							<A.Harper@kcl.ac.uk>



Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	kermit.columbia.edu
			kermit/b		ckvvcmu.hex	HEX of .exe
			kermit/test		ckvvcmu.hex	HEX of .exe

	--------------------------------------------------------------

NOTE: The OpenCMU version of kermit is currently a test version only so it is
only worth looking in the kermit/test directory.



4.5.2 >>>> FSP						[27-OCT-1994]
--------------

Summary:
	File transfer over a lightweight UDP based protocol.


Description:
	FSP is a simple file transfer protocol based around UDP rather than
	TCP protocols.  It is designed to impose minimal load on the server
	and does not require the user to log in or identify themselves.
	Essentially, the client throws UDP packets at a server asking for
	a portion of a file or info about a file, and keeps throwing the same
	request at it until the server responds. Thus an FSP transfer is, in
	principle, resilient to server failure as it will retry until the
	server comes back on-line. It is said that FSP is what anonymous FTP
	should have been.

	This package is a port of the unix FSP client and server to VMS, and
	directly supports UCX, Multinet and CMU. It also supports the vendor
	independent SOCKETSHR library.


Pre-Requisities:
	Either:
	    LIBCMU	For direct CMU support

	Or:
	    SOCKETSHR	For vendor independent TCP/IP support (recommended)
	    NETLIB	(required by SOCKETSHR)


Author:
	Various.
							<A.Harper@kcl.ac.uk>



Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.kcl.ac.uk	[.default]		fsp.*		VMS_SHARE
	
	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND FSP.PACKAGE	VMS_SHARE

	--------------------------------------------------------------





4.6 >>>> NETWORK ARCHIVE SEARCH APPLICATIONS		[10-OCT-1994]
--------------------------------------------

There are many many sites around the world that allow public access to parts of
the file system that contain freely available software. With so many sites and
so many packages available, it can often be difficult to locate the appropriate
site that holds the required software.

ARCHIE was designed to ease this problem. A large number of sites are
responsible for indexing all the other sites in the world and keeping track of
what each contains. The ARCHIE mechanism allows a user to supply a keyword to
the nearest archie host and have it return a list of software locations that
contain the keyword somewhere in the directory/filename path. This usually
results in a large list of potential places to search, which can then be
interrogated using an FTP utility.
							<A.Harper@kcl.ac.uk>


4.6.1 >>>> ARCHIE					[10-AUG-1994]
-----------------

Summary:
	A client used to interrogate the world-wide archive software database.

Description:
	ARCHIE is a client for interrogating ARCHIE servers. Such servers
	maintain up to date information about what software is available on
	various FTP archives around the world and permit the client to ask
	where a particular item can be found. Given a keyword, ARCHIE will try
	to find all archives that contain files with the keyword as part of the
	name. Once located, FTP can be used to retrieve the item from the
	nearest archive.

	ARCHIE is configurable to use any one of a number of nearby archie
	servers with one selected at compile time as the default.

Pre-Requisites:
	For OpenCMU usage, a socket library interface is required. There are
	two parallel versions or archie. One runs over the LIBCMU package, and
	the other runs over the SOCKETSHR package. Check the readme files with
	the software to see which is applicable.

Author:
	Unknown. Many contributors.
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.kcl.ac.uk	[.archie]		archie.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND ARCHIE.PACKAGE	VMS_SHARE

	--------------------------------------------------------------


4.7 >>>> GOPHER APPLICATIONS				[10-OCT-1994]
----------------------------

GOPHER is a protocol for requesting information from a remote system. GOPHER
servers run on these systems to handle the incoming requests and GOPHER clients
are necessary to interact with a user and generate the requests. Information is
presented to the user in a menu format and allows information of many different
types to be downloaded, viewed and/or saved.  One GOPHER server can send back a
pointer to a file of information that exists on a completely different system.
This provides a generalised world wide browsing system

NOTE: To a large extent, the functionality of GOPHER has been superceded by the
World Wide Web but there are still a large number of gopher servers around.
							<A.Harper@kcl.ac.uk>


4.7.1 >>>> GOPHER					[10-OCT-1994]
-----------------

Summary:
	A gopher client and server for VMS

Description:
	The gopher client allows a user to request documents from any gopher
	server in the world. The gopher server allows a site to serve documents
	to the rest of the world.

Pre-Requisites:
	The gopher client requires NETLIB, and will run over any of the
	supported TCP/IP transports. The gopher server specifically requires
	either UCX or MULTINET. There is no version for OpenCMU.


Author:
	The University of Minnesota
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	boombox.micro.umn.edu
			/pub/gopher/VMS		gopher*VMS*.zip	ZIP
	ftp.kcl.ac.uk	[.gopher]		gopher.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND GOPHER.PACKAGE	VMS_SHARE

	--------------------------------------------------------------



4.8 >>>> FINGER APPLICATIONS				[10-OCT-1994]
----------------------------

The FINGER protocol allows a client to `finger' another user on another system
to find out basic information. For instance, fingering a system will give
details of who is currently logged on. Fingering an individual username will
give selected personal details (real name, location and any immediate plans).

Note - some sites consider finger to be a security risk and do not run either
the server or the clients. Thus it may not be possible to `finbger' some
systems.
							<A.Harper@kcl.ac.uk>



4.8.1 >>>> MADGOAT FINGER				[10-AUG-1994]
-------------------------

Summary:
	A FINGER Client and Server

Description:
	FINGER provides both client and server facilities; This allows users
	to discover information about users on another system and for those
	users, in turn, to find out about local users.

Pre-Requisites:
	FINGER requires the NETLIB library

Author:
	Matt Madison <tgv.com> and Hunter Goatley <goathunter@alpha.wku.edu>
	(C) MadGoat Software ltd.
							<A.Harper@kcl.ac.uk>



Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.spc.edu	[.macro32.savesets]	mg_finger.zip	ZIP
	ftp.kcl.ac.uk	[.madgoat]		mg_finger.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND MG_FINGER.PACKAGE	VMS_SHARE
	FILESERV@ WKUVX1.WKU.EDU	SEND MG_FINGER		VMS_SHARE,MFTU,ZIP

	--------------------------------------------------------------



4.9 >>>> DOMAIN NAME SERVER APPLICATIONS		[10-OCT-1994]
----------------------------------------

The Domain Name Server (or DNS) is responsbible for mapping system names into
network addresses. It is sometimes useful to interrogate the DNS directly,
perhaps to do fault determination or to track down a system name.
							<A.Harper@kcl.ac.uk>


4.9.1 >>>> NSQUERY					[10-OCT-1994]
------------------

Summary:
	Request information from the DNS

Description:
	NSQUERY is a very useful utility that allows a user to interrogate any
	Domain Name Server for full site details.

Pre-Requisites:
	Requires the NETLIB library.

Author:
	Thanks go to Matt Madison <madison@tgv.com> for writing and mainting
	the NSQUERY software.
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.spc.edu	[.macro32.savesets]	nsquery.zip	ZIP
	ftp.kcl.ac.uk	[.news]			nsquery.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND NSQUERY.PACKAGE	VMS_SHARE
	FILESERV@ WKUVX1.WKU.EDU	SEND NSQUERY		VMS_SHARE,MFTU,ZIP
	--------------------------------------------------------------



4.9.2 >>>> IPADDR					[10-OCT-1994]
-----------------

Summary:
	Convert name to IP Address and vice versa

Description:
	IPADDR is a simple utility to map an IP address into its corresponding
	host name(s) and vice versa.

Pre-Requisites:
	Requires the NETLIB library.

Author:
	Andy Harper <A.Harper @ kcl.ac.uk>
							<A.Harper@kcl.ac.uk>


Availability via Anonymous FTP:

	--------------------------------------------------------------
	Site		Directory		File(s)		Format
	--------------------------------------------------------------

	ftp.kcl.ac.uk	[.default]		ipaddr.*	VMS_SHARE

	--------------------------------------------------------------


Availability via Electronic Mail:

	--------------------------------------------------------------
	Address				Command to send		Format
	--------------------------------------------------------------

	VMSSERV @ KCL.AC.UK		SEND IPADDR.PACKAGE	VMS_SHARE

	--------------------------------------------------------------



The Penn State Finger package contains a Finger client and server.
The Finger server process provides several services, including Finger,
an SNTP client (Simple Network Time Protocol), and several of the
trivial services.

The whole package is written in VAX MACRO and runs on either VAX or Alpha.

The Finger server is a permanent process, allowing quick response to
network connections.  Local Finger provides better performance then
DECUS Finger.

The Finger server is running on ECL.PSU.EDU, feel free to try it (if
you currently lack a client, telnet to port 79 and hit return).

The only optional VMS software required is DECnet and/or UCX.
Multinet with UCX emulation may also be used.

FINGER CLIENT
=============
The Finger client supports DECnet and UCX TCP/IP.  It requires a local
server to finger the local node (it is not itself a Finger program).

Command is:

	FINGER username@host

Any qualifiers are passed to the server, they may appear anywhere
in the line.

If @host is omitted, FINGER defaults to 0 (your own host) or to
the value of the FINGER_SERVER logical name.  For a memory starved
workstation cluster, you might want to run the Finger server only on
one node and define FINGER_SERVER on all satellites to point to this node.

FINGER first tries DECnet (unless the name obviously isn't a DECnet
node name), then it tries TCP.  You may suffix the name with .DECNET
to force DECnet only.  Use the fully qualified domain name to force TCP.

DECnet Finger is compatible with the DECUS DECnet Finger object.

Command arguments may be quoted to preserve case, otherwise the entire
line is converted to lowercase.


FINGER SERVER
=============
The Finger server is a single, multi-threaded, detached process.
The server offers fast response because the process is always present.
It has been tested with DECnet, UCX (version 1.3 and 2.0), and Multinet.
The server requires privileges, the client does not.

The following services are provided (any may be disabled through
conditional assembly).

	o  TCP/UDP/DECnet FINGER (RFC 1288)
	   The Finger server is substantially compatible with Rich
	   Garland's Finger (DECUS Finger).  The DECnet server will
	   directly replace the DECUS Finger object.

	   All Finger commands are logged to SYS$MANAGER:FINGER.LOG.

	o  TCP/UDP Echo protocol (RFC 862)

	o  TCP/UDP Discard protocol (RFC 863)

	o  TCP/UDP Daytime protocol (RFC 867)

	o  TCP/UDP CharGen protocol (RFC 864)

	o  TCP/UDP Internet Message Send Protocol (RFC 1159)
	   This is the A protocol.  The newer, B protocol
	   is not yet supported.  It has been tested with
	   the ISEND client from Hebrew University (dated
	   April 1991).

	   Messages are broadcast clusterwide to the requested
	   username (terminal name is ignored).

	o  SNTP client (Simple Network Time Protocol) (RFC 1361)
	   Given a suitable (close and reliable) NTP server, this
	   client will periodically poll the server and speed up
	   or slow down the local system time to match the server.

	   All time changes are logged to SYS$MANAGER:SETTIME.LOG.

DECUS FINGER DIFFERENCES
========================
This is a list of things I can think of which differ.

	/HELP to the server is the only documentation.
	This is a munged copy of the DECUS Finger help file.
	I haven't given documentation any serious consideration yet.

	This is a clusterwide finger (/CLUSTER is default).

	New mail message headers are never listed (I haven't yet figured
	out how to match addresses so it only displays messages from the
	person doing the Finger).

	There is no Finger database.  Names are read from the SYSUAF
	owner field.  The entire SYSUAF is read at startup, and updated
	as needed for new accounts.  There is not presently an easy way
	to disable printing the owner name.

	Added fields for account and nodename.

	No support for /SYSTEM, /IDLETIME, /TTTYPE, or /SWAPPED.

	/SORT only supports account, CPU time, Login time, and Username
	(Username is the default).

	Differs a bit in what an interactive process is.

	Only supports one filename for finger plan (default FINGER.PLN).
	Also prints a project line (FINGER.PROJECT).

	The format of the process listing (which fields can be displayed,
	which columns, and how wide the fields are) can be easily configured
	at assembly time.

	No support for Jnet, or TCP other then UCX/Multinet, at this time.

	The server will not forward Finger requests.  Commands like
	Finger @watsun@cunixc are not valid.

INSTALLATION
============
Two executables are generated, a Finger client and a Finger server.

To install:

1) Place the files in a (preferably empty) directory, into which you
   have SET DEFAULT.

2) If you will be using the Finger server, edit CONFIG.MAR and select
   services appropriate for your site.  By default, all services are
   enabled except SNTP.  To disable a service, place a ; before the
   *_ON=1 line (where * is the service).

3) Compile and link:

	$ @BUILD

   This will require about 12 minutes on an Alpha DEC 3000 model 400.
   About 15 minutes on a VAX 11/785.

   BUILD will create FINGER.EXE (client) and FINGER-DAE.EXE (server).
   Put these in an appropriate place.  There is no reason to keep
   the .OBJ files.

4) If you will be using the server, update FINGER-DAE.COM for your site.
   Put it in an appropriate place.

   Start the Finger daemon (now, and at system startup) by executing
   this file (@FINGER-DAE.COM).

   The server log files (sys$manager:finger.log and settime.log) will
   have new versions automatically created each day.  You should set
   a version limit on these files.  The files are kept open, so you
   will need to stop the server to set the version limit.  A side note,
   commands are flushed out to disk after no more than a minute.

5) Define a FINGER command:

	FINGER:==$device:[directory]FINGER

CREDITS
=======
This Finger package may be freely distributed (see copyright.txt).
Comments/suggestions are welcome.  I will attempt to include any
modifications I receive.  I plan to make substantial changes to
Finger in the future (time permitting).


			Bryan Jensen
			System Manager
			Engineering Computer Lab
			Penn State University
			bjj@ecl.psu.edu
			bjj@psuecl.bitnet

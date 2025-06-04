
README.TXT		SOCKETSHR V0.9-D			25-Jan-1995

							Eckart Meyer
							meyer@ifn.ing.tu-bs.de

NOTE: SOCKETSHR is still in beta test. Many people have tested SOCKETSHR
      meanwhile and it should be quite stable now.
      V0.9-D is only tested on UCX V3.2 and CMU/IP V6.6-5A. I've no access
      to other TCP/IP packages - I've left that for you ;-)


What is SOCKETSHR ?
-------------------

	SOCKETSHR is a TCP/IP package independent socket library for OpenVMS
	(VAX and Alpha).
	All TCP/IP packages come with their own socket library, why do we
	need another one?

	The advantage of SOCKETSHR is that it is implemented as a shared
	image which can be used with all well known TCP/IP packages.
	Programs linked to it may run on all of them *without relinking*.

	This makes it possible to use only one executable if different
	TCP/IP packages are used in a network. It makes it much easier
	to distribute a program in binary form to the public. Many programs
	like ARCHIE, GOPHER, MOSAIC, FINGER, TALK, etc. may benefit
	from being linked against SOCKETSHR and distributed as binaries.

	SOCKETSHR also supports the standard i/o routines like fputs() etc.

How to get it?
--------------

	The beta test versions can be obtained via anonymous FTP from

		'ftp.ifn.ing.tu-bs.de' in directory 'vms/socketshr':

	SOCKETSHR_BIN_???.ZIP		- shared images for OpenVMS VAX and AXP
	SOCKETSHR_SRC_???.ZIP		- sources containing the UCX and the
					  NETLIB V1 Version.
	NETLIB017.ZIP			- Matt Madisons NETLIB V1.7 kit.

	This is a site in Germany at the Technical University of Braunschweig.
	Later it may appear on other well known servers.

How to install ?
----------------

	This is really easy.

	Unzip the appropriate .ZIP file to a temporay directory and move
	the shared images to SYS$COMMON:[SYSLIB] or to any
	other directory (when moving socketshr.h into another directory,
	you should define VAXC$INCLUDE or DECC$SYSTEM_INCLUDE to also point
	to this directory). Define the logical SOCKETSHR to point to the
	image file, e.g.:

		$ define/system socketshr sys$share:socketshr_netlib1.exe

	Add this command to the system startup file
	(SYS$MANAGER:SYSTARTUP_V5.COM).

	SOCKETSHR_BIN_???.ZIP contains:

	README.TXT			This file

	SOCKETSHR.H			Header file with #define's
	SOCKETSHR_NETLIB1.EXE		VAX shared image for NETLIB V1
	SOCKETSHR_NETLIB1.ALPHA_EXE	Alpha shared image for NETLIB V1

	SOCKETSHR_UCX.EXE		VAX shared image for UCX only
	SOCKETSHR_UCX.ALPHA_EXE		Alpha shared image for UCX only

	TYPES.H, IOCTL.H and FILE.H	Header files used in programs.

	PROTOCOLS.			IP protocol definitions. Used by
					the getproto* routines.
	SERVICES.			service definitions. Used by the
					getserv* routines.

	*.OLB, *.OBJ, *.OPT		Object files, libraries and option
					files to link the shared image.
	LINK.COM			Used to link SOCKETSHR_xxx.EXE

	The Alpha versions may be renamed to .EXE.

	You may relink the shared image by typing "@LINK".

	The NETLIB version implements a socket library and network I/O is
	based on NETLIB. You must install NETLIB V1.7 or higher to use
	this version.
	** This is the recommended version **.

	The UCX version is based on the UCX socket library. SOCKETSHR_UCX
	provides the SOCKETSHR interface which additionally contains the
	standard i/o routines. It has all bugs of the UCX socket library...
	(the NETLIB version has its own bugs :-).

	You may switch between the different versions by simply redefine
	the logical SOCKETSHR to one of the shared images. No relinking
	is required.

How is it implemented?
----------------------

	SOCKETSHR comes as a shared image containing entries named
	SI_routinename, e.g. SI_SOCKET, SI_GETHOSTBYNAME. The file
	SOCKETSHR.H should be included in a C program, which contains
	Definitions like "#define socket si_socket" ("SI" stands for
	"Socket Interface"). Also, SOCKETSHR.H defines routine prototypes.

	The first approach to implement SOCKETSHR was to only have some
	jacket routines which subsequently call the package specific
	routines. A Version for CMU/IP's UNIXSHR library has been built
	some years ago (in fact, this was simply the socket handling routines
	stolen from UNIXSHR and some additional routines - even the prefix SI
	was stolen from UNIXSHR). Later, a version for UCX and one for LIBCMU
	have been built. To make it useful for other packages, the current
	version is built on Matt Madison's NETLIB V1, thus SOCKETSHR supports
	all TCP/IP packages which NETLIB supports.

	Most of the code of the NETLIB Version of SOCKETSHR is stolen from
	LIBCMU (a socket library implementation for CMU/IP by Mike O'Malley).

What is supported?
------------------

	In addition to the traditional socket routines SOCKETSHR also supports
	the standard I/O routines like fgets(), fread(), etc, including
	fdopen() and fileno(). These routines are simply jacket routines,
	which call the VAXC/DECC RTL-Routines if used with file pointers
	other than those used with sockets and send/recv with file pointers
	associated with sockets. There is no additional buffering.

	Here is a list of all implemented routines:

	Socket routines:

		socket			returns a fd, starting with 3
		bind
		connect
		listen
		accept
		recv
		recvfrom
		send
		sendto
		read
		write
		writev
		select			32 fd's, sockets and terminal only
		ioctl			only a small subset, e.g. FIONBIO
		shutdown
		close
		getsockopt		nothing implemented - NOP
		setsockopt		dito.
		si_get_sdc		returns VMS channel like vaxc$get_sdc

	For historical reasons (and may be used in future):

		signal			directly transfered to VAXC/DECC
		sigvec			dito.
		alarm			dito.

	Database routines:

		getservbyname		read file 'services.'
		getservbyport
		getservent
		setservent
		endservent
		getprotobyname		read file 'protocols.'
		getprotobynumber
		getprotoent
		setprotoent
		endprotoent
		gethostbyname
		gethostbyaddr
		gethostname
		gethostent		dummy, always returns NULL
		sethostent		dummy
		endhostent		dummy
		getsockname
		getpeername

	Conversion routines:

		inet_addr
		inet_ntoa
		ntohl
		ntohs
		htonl
		htons

	Standard I/O routines:

		fgetc
		fputc
		fgets
		fputs
		fread
		fwrite
		fprintf
		fcntl			only a small subset supported
		fdopen			only one FILE * for a socket.
		(fileno			obsolete)
		fclose
		rewind			NOP on sockets
		fflush			NOP on sockets
	
	The getserv* and getproto* routines used the standard UNIX
	files 'services' and 'protocols'. They should reside in
	SYS$LIBRARY. A logical SOCKETSHR_SERVICES or SOCKETSHR_PROTOCOLS
	can be used to point to the files, e.g.

		$ define/system SOCKETSHR_SERVICES mydev:[mydir]services.dat

Implementation Notes
--------------------

	The files types.h (added the FD_SET definitions for select()),
	ioctl.h and file.h (add definitions for fcntl()) are provided.

	No other .h files have been provided currently. Use the header files
	provided by the VAX-C or DEC-C compiler (e.g. socket.h).
	When compiling software ported from UNIX, define logicals
	SYS, NETINET etc. to point to SYS$LIBRARY.
	To link a program against SOCKETSHR, add to the option file:

	SOCKETSHR/SHARE

	(provided that the logical SOCKETSHR has been setup properly)

	VAX: SOCKETSHR has been built with VAX C. The current shared image
	ident is 1,3.
	The shared image provides two universal symbols for each routine
	not already defined in VAXCRTL. One entry has the SI_ prefix,
	the other entry is without prefix. Routines like read(), write(),
	close() and all standard I/O routines only have one universal 
	symbol with prefix SI_. Without including socketshr.h
	those routines cannot be used with sockets.
	The recommended way is to include socketshr.h.

	AXP: SOCKETSHR has been built with DEC C. The current shared image
	ident is 2,3.
	The shared image provides two universal symbols for each routine
	(with the exception of SI_GET_SDC), one with the prefix SI_ and
	one without. This does not interfere with DECC$SHR since
	this image prefixes all routines with DECC$.
	Using the compiler switch /prefix=ansi prevents the compiler from
	prefixing socket routines with DECC$, thus no #defines are
	neccessary. Of course, read(), write(), close() and all
	standard I/O routines cannot be used in this case, since they
	*are* prefixed by DEC-C. Including socketshr.h
	redefines all routines to have the SI_ prefix. In this case,
	all routines may be used. This is the recommended use.

	Since AST's with UDP are supported only from NETLIB V1.7 and
	above, you must install at least this version. You can find
	the kit in the same directory as SOCKETSHR.

	TRACE output: The logical SOCKETSHR_DEBUG may be defined as a bit mask:

		1 -	show routine entries and parameters
		2 -	show data

	Output is to the terminal, but may be redirected by defining the
	logical SOCKETSHR_LOGFILE to a valid vms file specification.


Mail list
---------

	A mail list has been created for diskussion of SOCKETSHR.
	To subscribe send a one-line message to:

		socketshr-request@ifn.ing.tu-bs.de

	The body must be one line containing

		subscribe

	The subject line is ignored. Send articles to

		socketshr@ifn.ing.tu-bs.de



Building from sources
---------------------

	MMS may be used to control the building. If you don't have MMS get
	the clone MMK from ftp.spc.edu or ftp.wku.edu or their mirrors.
	(I don't have MMS, I always used MMK).
	Alternatively use BUILD.COM

	Create a directory (for example [mydir.SOCKETSHR]).
	If building for both VAX and Alpha the following must be executed
	on both architectures.

		$ SET DEFAULT [mydir.SOCKETSHR]

	Get SOCKETSHR_SRC_???.ZIP and unzip it:

		$ UNZIP SOCKETSHR_SRC_???.ZIP

	For building the NETLIB version,

		$ SET DEFAULT [.NETLIB]
		$ @SETUP		! defines some logicals

	If building both the VAX and Alpha version:

		$ @[-]arch

	This switches from VAX to Alpha and vice versa. Then:

		$ MMS	or $ MMS/MACRO=(TRACE=1) !enable trace output (recommended)
	or	$ @BUILD	! always build with trace enabled

	SOCKETSHR_NETLIB1.EXE will be created in the the current directory.
	The same procedure applies to other SOCKETSHR implementations.

	The prebuilt objects and binaries have the trace capabilities enabled.

License
-------

	SOCKETSHR may be freely used and distributed. All copyright notices
	must be retained.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


Bug-Reports
-----------

	Please report errors and suggestions to

		meyer@ifn.ing.tu-bs.de

		Eckart Meyer
		Institute for Telecommunications
		Technical University of Braunschweig
		Schleinitzstr. 23
		38092 Braunschweig
		Germany
		Phone: +49 531 391 2454
		Fax:   +49 531 391 5192


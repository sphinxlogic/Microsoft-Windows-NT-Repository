------------
Contents
------------
Copyright
Notices
Authors
History
Synopsis
Files
Making IUFINGERD
Configuring IUFINGERD
Cache Sizes
Caveats
Comments


------------
Copyright
------------

   (C) Copyright 1992-1993 The Trustees of Indiana University

   Permission to use, copy, modify, and distribute this program for
   non-commercial use and without fee is hereby granted, provided that
   this copyright and permission notice appear on all copies and
   supporting documentation, the name of Indiana University not be used
   in advertising or publicity pertaining to distribution of the program
   without specific prior permission, and notice be given in supporting
   documentation that copying and distribution is by permission of
   Indiana University.

   Indiana University makes no representations about the suitability of
   this software for any purpose. It is provided "as is" without express
   or implied warranty.


------------
Notices
------------

The authoritative distribution site for IUFINGERD is ftp.indiana.edu.
This is where you will find the most up-to-date version.  


------------
Authors
------------

Indiana University
University Computing Services
Network Applications Group

Larry Hughes & Jim Harvey - Version 3.0
Larry Hughes  - Version 2.0
Colin Fraizer - Version 1.0


------------
History
------------

Current Version : 3.0
Release Date    : October 22, 1993
Comments        : Nearly a complete rewrite.  Create a QIO-driven asynchronous
                  event loop, to fix the bug that caused server to hang in
                  V2.x.  Add support for all CLD command line options and
                  associated features.  Support for AXP architecture.

Version         : 2.0
Release Date    : December 28, 1992
Comments        : Cache users' real names from SYSUAF to minimize CPU
                  usage.  Other miscellaneous optimizations, some
                  additional logging for debugging.

Version         : 1.0
Release Date    : September 25, 1991
Comments        : Initial release to the public.


------------
Synopsis
------------

IUFINGERD is a finger daemon for OpenVMS.  It will compile and run on most
VMS systems with Wollongong's Pathway (aka WIN/TCP for VMS), Digital's 
TCP/IP Services for OpenVMS (aka UCX)), TGV's Multinet, or Process Software's 
TCPWare.

IUFINGERD has several attractive and unusual features:

   o It is a permanent server, requiring no process creations.
   o Caching of static UAF fields, to minimize I/O to SYSUAF.DAT.
   o Caching of remote host information, to minize DNS queries.
   o Site-configurable output formats for list queries (using FAO).
   o Optional RFC931 support to determine client usernames.
   o Optional display of users' forwarding,  new mail status, and
     home directory information.
   o Optional support for multiple "plan" and "project" file names.


------------
Files
------------

README.TXT          - this file

START_IUFINGERD.COM - runs at system statup (RUN/DETACH's IUFINGERD.COM)
IUFINGERD.COM       - runs IUFINGERD.EXE

MAKE.COM            - Builds the IUFINGERD distribution
IUFINGERD.CLD       - CLD for IUFINGERD.EXE
IUFINGERD.MMS       - MMS make file used by MAKE.COM
MULTINET.OPT        - MULTINET MMS options file
UCX.OPT             - UCX MMS options file
WINS.OPT            - WINS MMS options file
TCPWARE.OPT         - TCPWARE MMS options file

ARGS.C              - IUFINGERD server C source code files
CACHE.C
FUIP.C
HASH.C
JPI.C
MAIL.C
MAIN.C
NET.C
PLAN.C
QIO.C
RFC931.C
UAI.C
UTIL.C
UCX_IOCTL.C         - a somewhat functional ioctl() for UCX

IUFINGERD.H         - header file used by C source files
UCX_COMPATIBILITY.H - header file used when built on UCX
VERSION.H           - header file containing version number


----------------
Making IUFINGERD
----------------

To build IUFINGERD, simply type this command in the IUFINGERD
distribution directory:

   $ @MAKE

The MAKE.COM accepts two optional paramters:

   $ @MAKE    [UCX | WINS | MULTINET | TCPWARE]    [MMS | NOMMS]
                        *must be P1*               *must be P2*

Although MAKE.COM should be able to automatically detect your TCP/IP
implementation and whether or not you have MMS, you may override the
automatic detection by supplying the parameters.

Successful completion of MAKE.COM will yield an executable of the form:

    IUFINGERD_'arch'_'tcpip'.EXE

where "arch" is "VAX" or "ALPHA", and "tcpip" is "UCX" or "WINS" or
"MULTINET" or "TCPWARE".  For example, IUFINGERD_VAX_UCX.EXE or
IUFINGERD_ALPHA_MULTINET.EXE, etc.

Rename or copy this file to IUFINGERD.EXE before proceeding.


---------------------
Configuring IUFINGERD
---------------------

After you execute the MAKE.COM procedure, you'll have to edit the
IUFINGERD.COM (which runs IUFINGERD.EXE), START_IUFINGERD.COM (which
runs IUFINGERD.COM as a detached process), and IUFINGERD.CLD (which
is the command language definition for IUFINGERD.EXE).

The only change you'll probably need to make to these files, is to
correct the "SYS$NOWHERE:[NOBODY]" directory specifications.  These
unusual and probably invalid directory references are used to encourage
you to correctly configure IUFINGERD for your individual site.

You might also wish to change START_IUFINGERD.COM to submit IUFINGERD.COM
to a batch queue, instead of running it as a detached process.  We
prefer the latter approach, and therefore distribute it that way.

In IUFINGERD.COM, the IUFINGERD.EXE executable can be run with any of
these command line options:

/DEBUG
/NODEBUG  (default)
   Controls whether or not profuse debugging information will be written
   to the log file.

/FAO="fao_string"
   The FAO (formatted ASCII output) string for the JPI information to be
   displayed in list queries.  If this qualifier is present, the /JPI
   qualifier must also be.  The default FAO is "!12AZ !30AZ !15AZ !AZ",
   which corresponds to the default /JPI fields.

   NOTE: You must use variations of "!AZ" for the JPI fields to be correctly
   substituted in the FAO routines.  Variations of "!AS" or "!AD" may cause
   the server to crash.

   You may specify any "!AZ" size that you wish for any of the fields.
   If the actual field length is larger than your specified size, the
   string will be truncated.  If the actual length is shorter than your
   size, it will be padded with spaces (on the right).  The maximum
   size that any field will actually achieve is shown below:

   Field      Max Size     Comment
   ----------------------------------------------------------
   IMAGNAME      39        File name minus path and extension
   LOGINTIM      11        Format is "MM/YY HH:MM"
   PID            8        Represented in hexadecimal
   PRCNAM        15        VMS limit
   REMOTE         ?        Undocumented in SYS$GETJPI()
   TERMINAL       8        VMS limit
   UAFOWNER      31        VMS limit
   USERNAME      12        VMS limit

/HEADER  (default)
/HEADER=SYSTAT
/NOHEADER
   Controls whether or not header information will be displayed in list
   queries.  Header information consists of the system nodename, date and
   uptime.  If you have Multinet or WIN/TCP, UNIX-style load averages are
   also shown.  Using /HEADER=SYSTAT displays the header in a format
   similar to the TOPS-10/20 SYSTAT program and also requires /SORT to
   be in effect.

/HOME_DIRECTORY  (default)
/NOHOME_DIRECTORY
   Enables or disables the display of users' home directories in
   single user queries.

/HOST_CACHE=(SIZE=size, TTL=ttl)
   Specifies the host cache configuration.  This is relevant only if you have
   specified one or more of the /RESOLVE_ADDRESSES and /RFC931 qualifiers.

   The information that is cached for hosts is the host name (if available
   through your DNS) and whether or not a remote system has an RFC931 daemon
   running.

   For details on the SIZE= parameter, see the section titled "Cache Sizes"
   below.  The default value is 100.

   The TTL is the "time to live" for the cache entry, in seconds.  The
   default TTL is 86400 (1 day).  A TTL of 0 represents an infinite
   value, i.e. cache entries will never time out.

/JPI=(keyword[,...])
   Specifies the JPI (job process information) fields to be displayed in
   list queries.  If this qualifier is present, /FAO must also be.

   Valid keywords are IMAGNAME, LOGINTIM, PID, PRCNAM, REMOTE, TERMINAL,
   UAFOWNER, and USERNAME.  The fields will be displayed in the order that
   you specify them, in the format specified by the /FAO qualifier.  The
   default is /JPI=(USERNAME, UAFOWNER, REMOTE, LOGINTIM).

   Technically, UAFOWNER is not JPI information, but IUFINGERD pretends
   it is.

   REMOTE is approximately equivalent to the TT_ACCPORNAM reported by
   $GETJPI.  For users connected via means other than TCP/IP, the
   REMOTE field is the unmodified TT_ACCPORNAM.  For those connected
   via TCP/IP, it will be either the DNS host name or IP network address,
   depending on the value of TT_ACCPORNAM and the presence of the
   /RESOLVE_ADDRESSES qualifier.

/LAST_LOGIN  (default)
/NOLAST_LOGIN
   Enables or disables the display of "Last login:" or "On since:"
   information in single user queries.  Since both are never displayed
   together, this flag controls the behavior of both.

/MAIL_CHECK  (default)
/NOMAIL_CHECK
   Controls the display of mail information on single user queries.  When
   the check is enabled, either "Has new mail" or "Has no new mail" is
   displayed, as well as the user's forwarding address (if one is set).

/PLAN=(filename[,...])
/NOPLAN
   The file name(s) to recognize as users' "plan" files.  If /NOPLAN is
   specified, no plan files will be displayed.

   Plan files must be located in users' SYS$LOGIN directory, and readable
   through the SYSTEM file protection field.  The default plan file name is
   "PLAN.TXT".  If you specify more than one file name, the first file found
   for any given user is the one that is displayed.

/PORT=port
   The TCP port on which the server should listen for queries.  The default
   is port 79.  Any unused TCP port may be used for testing purposes.  If
   you select a port that is already in use, IUFINGERD will report
   "error: bind" in the log.

/PROJECT=(filename[,...])
/NOPROJECT
   The file name(s) to recognize as users' "project" files.  If /NOPROJECT
   is specified, no project information will be displayed.

   Project files must be located in users' SYS$LOGIN directory, and readable
   through the SYSTEM file protection field.  The default plan file name is
   "PROJECT.TXT".  If you specify more than one file name, the first file
   found for any given user is the one that is displayed.

/PURGE_INTERVAL=interval
   The interval (in seconds) in which the host and user caches are checked
   for expired entries.  The default is 14400 (4 hours).

/RESOLVE_ADDRESSES  (default)
/NORESOLVE_ADDRESSES
   Controls whether or not the server resolves remote IP addresses to
   their respective host names.  This can add considerable time to queries,
   though an attempt is made to optimize this behavior by caching the host
   names.  See /HOST_CACHE for details.

/RFC931
/RFC931=(TIMEOUT=timeout)
/NORFC931  (default)
   Enables or disables checking of client usernames via RFC931.  When
   enabled, if the client machine is running an RFC931 daemon, the
   client username will be logged.  If it is not, the client username
   will appear in the log as "unknown."

   You may also specify a timeout value, which represents the maximum
   number of seconds that IUFINGERD will wait for a reply from the
   remote RFC931 server.  The default timeout is 10 seconds.

   It is not recommended that you always run IUFINGERD with this option
   enabled. It is a valuable option, however, if someone is fingering
   your machine abusively, and you want to determine who it is.

/SORT  (default)
/SORT=(POSITION=position, SIZE=size)
/NOSORT
   Controls the sorting of user information on list queries.  By default,
   the records are sorted using the first 80 columns as the sort key.  You
   may specify an alternate sort key with the POSITION and SIZE keywords.
   For example, you might choose to sort only on the username field, whose
   location and size you specified with /JPI and /FAO.

   Note that the POSITION is an offset, so the first character in a record
   is position 0, not position 1.

   /SORT or /SORT=(...) is required if you also specify /HEADER=SYSTAT.

/TIME_FORMAT=NUMERIC (default)
/TIME_FORMAT=TEXT
   This specifies whether the last login date and time for list queries is
   displayed in a fixed numeric format (MM/YY HH:MM which requires at least
   11 characters) or a variable text format which uses at most 9 characters.

/TITLE="title"
/NOTITLE  (default)
   Specifies a title line to be displayed above the JPI information in
   list queries.

/USER_CACHE=(SIZE=size, TTL=ttl)
   Specifies the user cache configuration.  The only user information that
   is cached is UAF data that is somewhat static, namely home directories
   and owner fields.  This minimizes the number of times that IUFINGERD
   has to read from SYSUAF.DAT.

   For details on sizing the cache, see the section titled "Cache Sizes"
   below.  The default for this value is 100.

   The TTL is the "time to live" for the cache entry, in seconds.  The
   default TTL is 86400 (1 day).  A TTL of 0 represents an infinite
   value, i.e. cache entries will never time out.


Example:

   $ iufingerd -
     /nodebug -
     /plan=(plan., .plan) -
     /project=(project., .project) -
     /header=systat -
     /home_directory -
     /nomail_check -
     /last_login -
     /jpi=(username, uafowner, remote) -
     /fao="!12AZ !20AZ !AZ" -
     /title="Username     Real Name            Remote" -
     /sort=(position=0, size=12) -
     /time_format=numeric -
     /host_cache=(size=1000, ttl=86400) -
     /user_cache=(size=1000, ttl=0) -
     /purge_interval=28800 -
     /resolve_addresses -
     /rfc931=(timeout=30)


------------
Cache Sizes
------------

Although the term "cache size" might seem to be the maximum cache size, this
is not really the case.  It is really the vertical dimension of an array of
linked lists, which is indexed by a simple (yet fast) hashing algorithm.  The
aggregate number of entries that can be cached is limited only by the virtual
memory available to the process.

Think of each cache as an imperfect matrix; the vertical dimension is sized
absolutely by the "SIZE=" parameter, and the horizontal dimension of each
row grows dynamically as needed.

             Grows Dynamically -->
             +--------------------
       ^     | 0,0  0,1  ...   0,c
       |     | 1,0  1,1  ...   1,c
     Sized   |
       by    | .         .
     SIZE=   | .          .
   Parameter | .           .
       |     |
       V     | r,0  r,1  ...   r,c
             +--------------------

For best performance, the desired result is a matrix that becomes square
over time.  This will happen naturally when your cache is sized appropriate.

A "SIZE=" value that is too large may waste some memory.  A value that is
too small may waste some CPU cycles.  Neither event is tragic, but some
performance could be at stake.

To override the default cache size parameters, run IUFINGERD with the /DEBUG
flag for a period of time. It will report the cache indices as it inserts
entries into the cache.

If the indices are spread somewhat evenly throughout the range of zero to
your SIZE= value, the cache is sized appropriately.

If many indices are duplicated frequently, increase the size and observe again.

If many indices are unused, decrease the size and observe again.


------------
Caveats
------------

If you enable host and/or user caching, IUFINGERD can consume a great deal 
of virtual memory.  You are advised to monitor its memory usage and page 
fault habits on your system.  

You may need to modify the RUN/DETACH command in START_IUFINGERD.COM for 
your site.  Qualifiers you might wish to explicitly specify are:

     /WORKING_SET=
     /MAXIMUM_WORKING_SET=
     /EXTENT=

Failure to do so could result in excessive page faulting.  We have seen
as many as 300,000 page faults per day on a heavily-fingered VAX system 
with an un-tuned IUFINGERD process.

You may also need to explicitly specify other RUN/DETACH parameters,
depending on your local SYSGEN parameters.  


------------
Comments
------------

IUFINGERD is an unsupported program.  If you decide to use it, do so
only in strict adherence to the copyright/disclaimer shown at the
top of this file, in COPYRIGHT.TXT, and throughout the source code.

You may send comments, questions, and bug reports via Internet mail
to iufingerd@indiana.edu, or BITNET mail to IUFINGERD@INDIANA.  We
will respond on a time-available basis.  We do appreciate hearing
from you!

Also, if you wish to be notified of future updates and/or bug
reports, please send email to one of the addresses listed above,
and ask to be placed on the list.

Enjoy!

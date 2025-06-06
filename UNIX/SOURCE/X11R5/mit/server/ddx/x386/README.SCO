
Information for SCO Users
-------------------------

Contents
--------
    1)  Binary distribution.
    2)  Source distribution.
    3)  Before Running XFree86.
    4)  Switching Consoles.
    5)  Setting up man pages.
    6)  Using SCO binaries/servers.
    7)  Compiling XFree86 under 3.2.2.
    8)  Compiling XFree86 under 3.2.4.
    9)  Relevant Documentation.
    10) Known problems.
    11) Trouble shooting.
    12) Acknowledgements.


1) Binary Distribution
----------------------

The following files are provided in the binary distribution:

    README.SCO            This file.
    gunzip.Z              The GNU uncompress utility.
    bin.tar.gz            The bin directory, contains most executables.
    lib.tar.gz            The lib/X11 directory (fonts, Config ...).
    devsys.tar.gz         The include file and the libraries.
    lkit.tar.gz           The server link kit.
    man.tar.gz            The formatted man pages.
    utils.tar.gz          Lots of PD utilities provided as is.
    PEX.tar.gz            All files relating to PEX including libraries
                          and header files.

To obtain a minimum XFree86 installation you will require the "bin.tar.gz"
and the "lib.tar.gz" files and optionally "gunzip.Z".  All the files are
compressed with "gzip" except of course "gunzip.Z" which is compressed
using the conventional compress program.

The supplied XFree86 server comes configured for every supported driver
except the Hercules mono driver.  This means that unless you are adding
an unsupported driver it is very unlikely that you will require the
link kit.  There is very little space/memory saving by removing the drivers
which are not in use.

To install the XFree86 binaries just follow these steps.

    1)  Obtain the files you require.

    as root,

    2) create a directory /usr/X386, permissions 755 should do nicely.
    3) cd /usr/X386
    4) extract the files, a minimum distribution for example would require

            gunzip < bin.tar.gz | tar xvf -
            gunzip < lib.tar.gz | tar xvf -
    
    5) extract any other files you downloaded the same way.
    6) if you have installed man pages see the later section on
       setting up man pages.
    7) Look through /usr/X386/lib/X11/etc/INSTALL, especially section 2
       on configuring and using XFree86.   This should allow you to get
       a server up and running.  Before starting the server check in
       the later section "Before Running XFree86", in this document, to
       see if there are any system requirements you have to make for the
       server to operate correctly.

If you intend using the programs from the utilities archive then
you should be aware that they are provided as is.  They are not part of
the XFree86 distribution nor are they part of the standard MIT
distribution.  Every attempt is made to ensure that they work to a usable
degree but they are not perfect.  All programs in the utilities archive
come with a manual entry (which is also in the utilities archive).
Information on the author(s) can usually be located in the man page or by
running the executable.


2) Source Distribution
----------------------

The SCO port comes as part of the standard XFree86 distribution.  Consult
the XFree86 README for more information on the location of sources.


3) Before Running XFree86
-------------------------

Before you can run XFree86 with shared libraries enabled you must
up the shared library count in the kernel.  This is only required for
3.2.2 systems.  To accomplish this log in as root and run:

    cd /etc/conf/cf.d
    ./configure SHLBMAX=6
    cd ../bin
    ./idbuild

The SCO xterm terminfo description is not compatible with the xterm
in the R5 distribution.

If you are going to be using a mouse other than a serial mouse you will
require the appropriate mouse drivers to be installed.  To do this run
"mkdev mouse" and configure it as required.  Then use the mouse device
created in /dev/mouse in your Xconfig file along with the appropriate type.
If you are unsure which device to use the SCO manual pages offer a description
of the various mouse/device associations ("man mouse").

To use a Bus/Keyboard or PS2 mouse you should configure the mouse drivers
under SCO as above using 'mkdev mouse'. You may then use the OsMouse option
in your Xconfig to specify that XFree86 should use the SCO mouse drivers.
For example, replace the line:

        Logitech    "/dev/tty1a"

with

        OsMouse

If you are using an Accelerated Localbus/EISA card you may wish to install
the "dmmap" driver.  This driver allows the server to use a linear address
mapping in high memory when accessing the card. The dmmap driver comes in
two forms, a binary archive which is supplied with the full binary
distribution and a source archive which is located in /usr/X386/lib/X11/etc.
Both distributions provide instructions for installation/removal within the
README file. 


4) Switching Consoles
---------------------

XFree86 uses similar console switching keys as the SCO R4 and R5
servers.  That is, Ctrl-PrntScr takes you to the next console along
from the one X is running on.  If this is the last console it will take
you to console 1.  Those interested in modifying the console switching
should look in mit/server/ddx/x386/common/x386Events.c.


5) Setting up Man Pages
-----------------------

After compiling the tree, or after installing the binary distribution you
can get man to recognise the XFree86 man pages by adding /usr/X386/man to
the MANPATH in /etc/default/man, the line should look similar to:

    MANPATH=/usr/man:/usr/X386/man

This allows all users to view the X man pages.  You may change your own
MANPATH environment variable if you do not want everyone to access the
man pages.

By default the man pages are compressed using "pack" under 3.2.2 and
"compress" under 3.2.4 to conserve space.  If you do not want to compress
the man pages change CompressManPages to NO in your site.def.  Those using
the binary distribution can use "unpack" or "uncompress" as appropriate to
uncompress the man pages.


6) Using SCO binaries/servers.
-------------------------------

XFree86 will accept connections from SCO binaries (R3 upwards) and the
SCO R5 server will also accept connections from XFree86 binaries. This
means you may mix and match the two if you have ODT.  For example you may
still use the Motif window manager (mwm) if you prefer.


7) Compiling XFree86 under 3.2.2
--------------------------------

Currently XFree86 is configured to install under /usr/X386. The binary
distribution will be configured to go into this directory.  If you are
compiling your own XFree86 there is no reason why this cannot be changed.
Check in the X11 config/README on how to change your ProjectRoot.

The defaults are set such that you should only require minimal changes to
the site.def file.  The BOOTSTRAPCFLAGS do most of the work for you. The
defaults will use gcc 2 or above.  They also compile for shared libraries
and setup formatted man pages using compression. The most likely
Parameters you will be interested in changing are:

    1.  What Fonts etc get compiled.
    2.  What gets installed (XdmConfig, FSConfig, ...)

Use gcc, preferably 2.3.3 or later but an earlier version may still do the job.
Configure it with "./configure i386-*-sco".

Make sure you do the fixincludes for gcc.

In order to compile the current XFree86 version for SCO you will have to
create the following header files in the appropriate places.

A) create a /usr/include/sys/uio.h containing the following:

    #ifndef _XFREE86_UIO_
    #define _XFREE86_UIO_ 1

    #ifndef __IOVEC__
    #define __IOVEC__ 1
    typedef struct iovec {
        caddr_t iov_base;
        int iov_len;
    } iovec_t;
    #endif /* __IOVEC__ */

    #endif /* _XFREE86_UIO_ */

B) create a /usr/include/net/errno.h containing

    #ifndef _XFREE86_ERRNO_
    #define _XFREE86_ERRNO_ 1
    #include <errno.h>
    #endif /* _XFREE86_ERRNO_ */

C) create a /usr/include/sys/bsdtypes.h which is empty for now.

D) create a <sys/time.h> under the gnu include directory containing the
   following (this protects the system header file from multiple inclusions)
   You do not have to change the system header file!

    #ifndef _SYS_TIME_H_
    #define _SYS_TIME_H_ 1
    #include </usr/include/sys/time.h>
    #endif /* _SYS_TIME_H_ */

To compile from here, follow the XFree86 README, basically after
obtaining X11r5, applying the XFree86 patches and completing the above
changes you should only need to run:

    make BOOTSTRAPCFLAGS="-DSYSV -DSYSV386 -DSCO" World

xdm doesn't have password checking yet as verify.c under mit/clients/xdm
requires the crypt() library function.  SCO do not ship the crypt library
but you can obtain it if you live in the USA.  If you require password
checking, and you live in the USA, and you have the crypt library look for
SCO_USA defines in the mit/clients/xdm directory to find what needs to be
changed to get this working.  Defining SCO_USA in the CDEBUGFLAGS should
do it.  This will be fixed in a later version.

The Fontserver has not been compiled.  Avoid compiling this by setting the
BuildFontServer in site.def to NO.  The default currently is NO however
this may change.  You should also set the InstallFSConfig to NO so that
the Makefiles do not try to install it. Otherwise its defaults all the way.

The install script mit/util/scripts/bsdinst.sh will not set 's' bit
permissions when installing over the top of a previously installed
XFree86. To fix this apply the following patch from the directory
containing the 'mit' directory.  This is personal taste only, if you
are installing through symbolic links then leave this one alone.

----------8<-----------------8<------------------------------------
*** mit/util/scripts/bsdinst.sh%	Tue Mar  2 14:27:16 1993
--- mit/util/scripts/bsdinst.sh	Tue Mar  2 14:27:17 1993
***************
*** 150,154 ****
  # This rm is commented out because some people want to be able to
  # install through symbolic links.  Uncomment it if it offends you.
! # rm -f $dst/$srcbase
  (cd $srcdir ; $installcmd -f $dst $flags $src)
  
--- 150,154 ----
  # This rm is commented out because some people want to be able to
  # install through symbolic links.  Uncomment it if it offends you.
! rm -f $dst/$srcbase
  (cd $srcdir ; $installcmd -f $dst $flags $src)
----------8<-----------------8<------------------------------------


8) Compiling XFree86 under 3.2.4
--------------------------------

Compiling gcc2.3.3 is accomplished after doing the following.

    ./configure i386-*-sco3.2v4

DO NOT USE "./configure i386-*-sco" it will not work.

Make sure you run the "make fix-includes" after you have installed.

It is recommended that you use a gcc compiled without any changes to the
value of __STDC__.  Some people may have compiled gcc by adding the define
"-DSTDC_VALUE=0" to their CPP_PREDEFINES flag.  If you have done this and
are able to recompile gcc without this change then you should.  Otherwise
refer to the end of this section for instructions on how to fix the XFree86
tree so that it will still compile.


Compiling XFree86,

A)  Create the "net/errno.h" and "sys/bsdtypes.h" as per the 3.2.2
    instructions above.

B)  Check that you have a "sys/uio.h" as there have been reports of
    system which have it and systems which don't!  If you already have
    one it need not be changed.

C)  Edit the gnu "unistd.h" include file and remove the non prototyped
    version of close (Look for "close()" and remove that line), do not
    remove the prototyped declaration.

D)  Create a "sys/stat.h" under the gnu include directory containing
    this:

    #ifndef _XFREE86_STAT
    #define _XFREE86_STAT 1

    #ifndef _NO_PROTOTYPE
    #include </usr/include/sys/stat.h>
    #else
    #undef _NO_PROTOTYPE
    #include </usr/include/sys/stat.h>
    #define _NO_PROTOTYPE 1
    #endif /* _NO_PROTOTYPE */

    #endif /* _XFREE86_STAT */
    
E)  Check the paragraph on site.def in the compiling for 3.2.2 section.

F)  Add a -DSCO324 to the BOOTSTRAPCFLAGS when you do the make World.

        make BOOTSTRAPCFLAGS="-DSYSV -DSYSV386 -DSCO -DSCO324" World
    
    Under 3.2.4 several different options become activated.

    - Compress is used instead of pack for compressing the man pages.
    - The default compile line will contain -D_NO_PROTOTYPE instead
      of -DNO_PROTOTYPE as this is what 3.2.4 expects.

G)  If you have problems with mkshlib reporting an error like

        mkshlib: : cannot exec /bin/sh
    
    when compiling libX11, then the following should fix it,

        rm -f lib/X/libX11.a
        env - make
    
    The error is related to a lack of environment space.

Compiling with __STDC__=0
-------------------------

DO NOT make these changes unless you have gcc configured with STDC_VALUE=0.
You can test for this by compiling this small program.  If it prints out
STDC_VALUE=1 do not make these changes.  If it prints out STDC_VALUE=0 then
you will have to make the following changes.

    main() { printf("STDC_VALUE=%d\n", __STDC__); }

So now you have a gcc compiled with STDC_VALUE=0 which cannot be recompiled
with a STDC_VALUE=1. To compile XFree86 with this compiler you will have to
edit mit/config/x386.cf.  Look for the pattern GCC_STDC_VALUE and replace the
1 by a 0.  For example the line

    #define GCC_STDC_VALUE 1

should become,

    #define GCC_STDC_VALUE 0

It is recommended to use gcc compiled without the STDC_VALUE=0 for building
XFree86.  It is, however, possible with the above changes.  These changes may
become obsolete or invalid in a later version of XFree86.


9) Relevant Documentation
-------------------------

Some relevant documentation for SCO Users and Developers can be found in
the following files.

    README       - the standard XFree86 README (/usr/X386/lib/X11/etc)
    README.SVR3  - Although a lot of this readme is based on Interactive
               a substantial proportion is still relevant.
    All of the VGA/Config documentation.
               /usr/X386/lib/X11/etc/VideoModes.doc and the README
               files for particular video cards.


10) Known Problems
------------------

After running the server you may see some strange characters in your
input to the shell.  This is due to some unprocessed scancodes and is
of no concern.  This will be fixed in a future release.

Using the SCO installed mouse devices with XFree86 may not work (ie, for a
bus mouse).  The best way to use these mouse devices is to specify
"OsMouse" in the Xconfig file.

Not all of the applications in /usr/X386/bin have been debugged.


11) Trouble Shooting
--------------------

Problem:
    Server aborts with the error message,
    "Failed to map video display memory".
Causes:
    Your system does not have the correct /etc/conf/pack.d/cn/class.h,
    You can confirm this by editing the file and looking for the string
    "SVGA", if it is not there then you should re-install this file from
    the "Extended Utilities" diskettes provided with your OS.  If this is
    not possible then installing the "dmmap" driver from the distribution
    may allow the server to operate correctly.

Problem:
    x386install does not work.
Causes:
    You should not be running x386install when using the XFree86 server
    under SCO.  It is used for Interactive (ISC) installations.

Problem:
    The server starts but the screen is not aligned correctly or is shakey
    and impossible to view.
Causes:
    This is most likely due to an incorrect Xconfig setup.  Look for the
    files README.Config  VideoModes.doc (in /usr/X386/lib/X11/etc with 
    the binary distribution).  These files explains how to fix up your 
    video modes.

Problem:
    1) Can only run a limited number of xterms.
    2) xterm does not work but other programs like xclock do work.
Causes:
    Not enough or no pseudo ttys devices are present on your system.
    Depending on which version of SCO you are using you will have to do
    different things.

    3.2.2   cd /etc/conf/cf.d; ./configure and increase the number of
            pseudo ttys.  Rebuild the kernel.
            "ls -al /dev/ttyp0 /dev/ptyp0" to get the major numbers for the
            slave and master drivers.  Use mknod to create the extra
            devices using the naming schemes ttyp[0-9a-f] and ptyp[0-9a-f].
    3.2.4   Run "mkdev ptty" and increase the number od ptty's.

Problem:
    When running curses/termcap applications in an xterm the output gets
    corrupted especially when scrolling.
Causes:
    1) You are running an original 1.3 distribution of XFree86.  Update
       to 2.1.
    2) You have resized the window and not ran "eval `resize`" before using
       your application.  The SCO operating system does not support dynamic
       resizing or xterms fully so this command must be run after resizing
       an xterm in order for curses/termcap applications to operate
       correctly.

Problem:
    1) When starting X it dies with an error "Cannot access a needed shared
       library".
    2) When starting an X application is dies with the above error.
Causes:
    1) You do not have the binaries installed in the correct directory.
       Check that they are in /usr/X386
    2) You have upgraded to a new binary distribution which has a new
       version of the shared libraries which are not compatible with your
       old binaries.  To fix this you will need to re-install the old
       shared libraries or recompile your application against the new
       libraries.

Problem:
    My SCO 3.2.4 system fails to build libXext.a for shared libraries.
Causes:    
    This behaviour was observed when compiling with a STDC_VALUE=0 for gcc.
    mkshlib was core dumping on building the shared library in this case.
    Using a new gcc without the STDC_VALUE=0 or using a 3.2.2 mkshlib
    appears to fix this problem.

Problem:
    When linking against the SCO motif library I get an unresolved external
    for "XtDisplayStringConversionWarning" when using gcc.
Causes:
    The SCO library is compiled with limited length identifiers.  To work
    around this add the following code to your application when compiling
    under XFree86 with gcc and SCO motif.

    #ifdef SCO
    void XtDisplayStringConversionWarnin(dpy, from, toType)
        Display*  dpy;
        String    from;
        String    toType;
    { XtDisplayStringConversionWarning(dpy, from, toType); }
    #endif

Problem:
    When compiling the XFree86 source under 3.2.4, libX11 fails to get
    built with an error like "mkshlib : : cannot exec /bin/sh"
Causes:
    Read the section on compiling for 3.2.4.  To fix this use "env - make".


12) Acknowledgements
--------------------

Thanks to the Core team for their previous and continuing help with the SCO
work.  Many thanks to Stacey Campbell at SCO for all the advice and
insights provided.  Thanks to SCO in general for making information available
for XFree86 development.

Thanks also to Peter Eubert (peter.eubert@iwb.mw.tu-muenchen.dbp.de)
and Kent Hamilton (kenth@stl.scscom.COM) for input on compiling under 3.2.4 
systems.  And of course the beta list for its input on everything.

Special thanks to Jerry Whelan (guru@stasi.bradley.edu) for providing an
ftp site for the binary distribution.

    
$XFree86: mit/server/ddx/x386/README.SCO,v 2.5 1994/03/08 08:31:20 dawes Exp $

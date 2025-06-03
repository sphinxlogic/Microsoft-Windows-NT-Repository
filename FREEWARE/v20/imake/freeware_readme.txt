OpenVMS IMAKE, Build Tools, Makefile generator

I. Summary:

        This freeware saveset contains everything necessary to use Imake on
OpenVMS systems.  Use the following command to restore this directory tree:

        $ backup/log Imake.sav/sav [...]*.*;

This will duplicate the Imake directory tree starting at your current directory. 
The instructions that follow are assuming that you restore this Imake directory
tree as the top level directory on a particular disk.

After restoring the Imake.sav saveset at the top level of one of your disks, you
can run the Imake_login.com procedure specifying a rooted logical name pointing
to your current disk default.   This command file will setup all the proper
logicals to use Imake on your system from any build area.  The following command
shows how this might be done:

        $ @imake_login dka300:[000000.]

or alternately from a development tools directory:

        $ @imake_login dka300:[project.tools.]  ! assuming this is where you
                                                ! were when you restored the
                                                ! Imake saveset as above.

Note the rooted directory specifications used above.


II. Directories:

    [IMAKE...]  - This is the top level Imake directory which holds all the
                  following directories:

    BINARY.DIR  - This directory holds the finished Imake tools, executables, and
                  Imake configuration files for a VMS environment.  The
                  Imake.Rules file is a monster file of slight variations
                  of Imake rules for building all sorts of program targets.  You
                  only really need a couple of these rules, but they exist in
                  all their splendor to show how you can create an Imake rule
                  to build any VMS type program target that you might ever need.

    CPP.DIR     - This is the C preprocessor that Imake uses.

    IMAKE.DIR   - This is the build directory and documentation for Imake.

    MAKEDEPEND.DIR      - This is the build directory for the makedepend image.

    MAKEINCLUDE.DIR     - This directory holds a command file to process output
                          from makedepend and insert it into an MMS makefile.

    UTILS.DIR           - This directory holds a command file to stream/lf an
                          OpenVMS text file that is not.  TPU creates files
                          that by default are not stream/lf.  You may be able
                          to configure TPU to do so.
           
III. Prerequisite Software:

    OpenVMS VAX | Alpha
    MMS
    VAXC, DECC, or DECC++ compiler


IV. Conclusion:

    The images in this saveset were built on an OpenVMS VAX 5.5-2 system with
VAXC.  You may have to play with the CC command on your system a bit to get
things to compile correctly with DECC.  This has not yet been tried, but there
should be no problems.  Once you get a good compile, you are pretty much done
building this stuff until you change platforms.

The CPP and IMAKE build areas have a bootstrap makefile.ini if you can't 
happen to run the existing images.   You build CPP first then Imake itself. 
There are readme files in most of the build directories to help you along.  They
describe some of the things that had to be done to get Imake working.

    The license release for all this software is found in the Imake.c module in
[imake.imake.build]imake.c.  It is a very nice tool.  May you find it useful!


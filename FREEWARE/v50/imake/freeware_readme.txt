IMAKE, Build_Tools, makefile generator

I. Summary:

Imake is a port of an X Windows tool that generates Makefiles, a set 
of cpp macro routines, and an Imakefile input file from a single source 
file.  Imake permits the separation of the operating system dependencies 
from the build scripts.  Imake works in conjunction with -- it does not
replace -- a makefile tool such as MMS.

This freeware kit contains everything necessary to use Imake on OpenVMS 
systems.  Use the following command to restore this directory tree:

        $ BACKUP/LOG IMAKE.BCK/SAVE [...]*.*;

This BACKUP comand will duplicate the Imake directory tree starting at 
your current directory.  

After restoring the Imake.sav saveset, you will need to invoke the 
Imake_login.com command procedure.  This command file will establish
the necessary symbols and logical names used elsewhere in the Imake
environment.  The following command shows how this might be done:

        $ @imake_login

II. Directories:

    [IMAKE]
      This is the top level Imake directory.

    [IMAKE.BINARY]
      This subdirectory holds the finished Imake tools, executables, 
      and Imake configuration files for a OpenVMS environment.  The 
      Imake.Rules file is a monster file of slight variations of Imake 
      rules for building all sorts of program targets.  You only really 
      need a couple of these rules, but they exist in all their 
      splendor to show how you can create an Imake rule to build any 
      OpenVMS type program target that you might ever need.

    [IMAKE.CPP]
       This is the C preprocessor that Imake uses.

    [IMAKE.IMAKE 
       This is the build directory and documentation for Imake.

    [IMAKE.MAKEDEPEND]
       This is the build directory for the makedepend image.

    [IMAKE.MAKEINCLUDE]
       This directory holds a command file to process output
       from makedepend and insert it into an MMS makefile.

    [IMAKE.UTILS]
       This directory holds a command file to stream/lf an OpenVMS 
       text file that is not.  TPU creates files that by default 
       are not stream/lf.  You may be able to configure TPU to do so.
           
III. Prerequisite Software:

    OpenVMS VAX or Alpha
    MMS
    DEC C


IV. Conclusion:

    The object modules and executable images in this saveset were built 
    on an OpenVMS Alpha V7.2 system using DEC C V6.0.  Some adjustments
    of the compilation commands will be necessary, depending on the
    particular package.

    The CPP and IMAKE build areas have a bootstrap makefile.ini if you 
    can't happen to run the existing images.   You build CPP first then 
    Imake itself.  There are readme files in most of the build directories 
    to help you along.  These readme files describe some of the things 
    that had to be done to get Imake working.

    The license release for all this software is found in the Imake.c 
    module in [imake.imake.build]imake.c.  It is a very nice tool.  May 
    you find it useful!


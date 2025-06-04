FLEX, UTILITIES, GNU "fast" (or "flexible") lexical analyzer generator

aug-1995 Sheldon Bishov, bishov@star.enet.dec.com

The source for files was Free Software Foundation, ftp from
anonymous@prep.ai.mit.edu

To build and install see the various *readme*.* files.

Provided outside of the zip file are executabls, reference files, 
a build procedure, and sample flex source code.

Reference files are the following (extracted from readme.vms):

    flex.doc    -- documentation, the "man page" describing flex;

    flex.skl    -- a text file containing flex's default skeleton;
                   with this version of flex, it is for reference only;
                   flex.exe does not need to know where to find it.
 
    flexlexer.h -- header file used for C++ class-based lexers; not
                        needed for ordinary C lexers.

The command procedure, build-flex.com, is provided to make building
executables easier.  This file refers to flexlib.olb in the default
directory; edit it to make changes if the .olb file is located
elsewhere.

Sample source files are counter.flex, a simple line and character 
counter, and scanner.flex, a simple token scanner. Both were extracted
from flex.doc.

The required executables and .obj files are in the [.VAX] and [.ALPHA] 
subdirectories.  They were built on OpenVMS v6.1 (VAX) and v6.2 (Alpha).

The following changes were made in the code:

build-flex.com: new procedure to make easier building executable using flex.

descrip.mms: add /standard=vaxc for decc builds

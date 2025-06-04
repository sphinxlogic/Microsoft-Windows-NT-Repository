DST-SCANNER, Utilities, Scan Vax or Alpha images and display symbol definitions
DST SCANNER 

Chris Chiesa (lvt-cfc@servtech.com) (formerly Chris_F_Chiesa@cup.portal.com)

    The collection of files accompanying this text, make up the "DST
Scanner" utility: when built, they produce three executable images whose
function is to scan an executable image and display, if the information
is available, the names of the source files which were compiled/assembled
and linked to form the image being scanned.  The baseline utility scans
a VAX or Alpha AXP VMS image and displays source file specifications only;
a modified, "details" version scans a VAX image (no corresponding Alpha AXP 
"details" version currently exists) and displays source file specifications
PLUS some RMS information (record/file organization, EOF position) associated 
with the source files.

    This utility does its job by reading the "target" executable image 
(.EXE) file, following the image's internal structure (Image Header, 
Debugger Symbol Table (DST), etc.), and extracting the desired information 
from one particular small subset of the information contained in the 
image's Debugger Symbol Table, or "DST," records.  Note that an image will 
contain DST records _only_ for those contributing source files which were 
compiled (or assembled) with the /DEBUG qualifier, and only if the image 
itself was linked with the /DEBUG qualifier.  

    Note that other software utilities exist which may interfere, or which 
you might worry MIGHT interfere, with the operation of the DST Scanner.  
The STABACCOP program used in creation of Standalone Backup sets on those 
systems which still support them, can be used to copy an executable image 
WITHOUT its Debug Symbol Table records; therefore, the DST Scanner will 
produce NO output upon scanning an image that was copied with STABACCOP.
On the other hand, numerous public-domain utilities exist which clear the
IHD$V_LNKDEBUG bit, and/or modify the transfer-vector array, in an image's 
header, in order to "disable" invocation of the VMS Debugger when executing 
an image which was compiled and linked with /DEBUG.  Use of such utilities 
will NOT prevent the DST Scanner from finding, and emitting, the desired 
information if it was present in the "original" form of the image.

   The format of DST records is "undocumented" by Digital, in addition to
being intricate and subject to change.  It is, however, explained fairly
thoroughly in the comments in the relevant section (DSTRECRDS.REQ) of the
VMS Source Listing, allowing the intrepid programmer to write DST Scanners
and such.  Just don't expect Digital to help you fix it if it breaks at the
next release of VMS! 

    This particular DST Scanner is actually quite minimal, compared to what 
it could be "in theory."  DST records embed a huge amount of information: 
routine names, other symbol names, sufficient information (file specification, 
RMS attributes, creation date, exact EOF position, and ODS-2 File ID) to 
uniquely identify each source file, and the complete source-line-to-instruc-
tion-address table necessary for the VMS Debugger to support source-level
Debugging -- just to name a few.  There are at least a dozen (maybe more) 
types of DST record, and several subrecord types within most of those!  From 
this wealth of information, this particular DST Scanner extracts *ONLY* the 
"source-file-specification strings" and, in the "details" version, RMS 
file-and-record type and EOF-position values.  It should be pretty clear from 
the source code for the "details" version, how to extend it to emit what YOU 
want it to emit -- at least from the "source-file correlation" type of DST 
record this code is designed to find-and-process
    
   There are a couple of other "limitations" -- if you want to call them 
that -- to this utility.  

   First, the DST Scanner outputs to SYS$OUTPUT -only-.  It would be fairly 
simple to add an /OUTPUT qualifier, but I've never gotten around to doing it; 
sorry again!  Fortunately, you can easily get around this limitation by 
defining logical name SYS$OUTPUT to a file _before_ invoking the DST Scanner. 
This comes in very handy as you'll see.

   Second, my experience shows that an image contains a "source-file 
correllation" DST record for EVERY instance of a source file which 
contributed to a /DEBUG compilation at some point in the building of the 
image.  For example, if you compiled several C source files, each of which 
included "hoohah.h," or included something else which included "hoohah.h," 
and then linked the resulting objects into a single executable image, then 
the DST Scanner would find, and display, "HOOHAH.H" several times, not just 
once.  HOOHAH.H would appear at least once for each object whose compilation 
involved "hoohah.h" at ANY point.  This can be a little annoying when 
trying to cross-reference the "raw" DST Scanner output stream to a minimal 
list of source files.  On the plus side, the multiple-occurrence behavior 
makes it easy to notice, for instance, that two different C modules used
two DIFFERENT versions of "hoohah.h" -- perhaps as a result of having been 
compiled at different times, across modifications of "hoohah.h" -- alerting 
you to a potential run-time problem without having to actually execute the 
image!  

   Third, the DST Scanner emits source file specifications in the order it 
encounters source-file-correlation DST records in the image, which seems to 
be a combination of the order in which the compiler encountered the source 
files, and the order in which the linker encountered the objects.  It is NOT 
alphabetical, and multiple appearances of a particular source file do NOT, 
as a rule, occur "in a row" but, rather, "scattered" throughout the image.  
This can make it tricky to find a particular source file you're looking for, 
but can be useful in determining the order in which the compilers and 
linker "put the image together," if you have reason to be concerned about 
it.

   Fourth -- and there's nothing I can do about this -- the DST Scanner can 
*only* find source file specifications if they're actually present, i.e. if 
DST records were generated at compilation/assembly time, and were preserved 
at link time.  It is not necessary that all objects linked into an image be 
compiled with the same /DEBUG (or /NODEBUG) state.  If any object contributing 
to the image was compiled without /DEBUG, no DST records were generated for 
that object, and thus the DST Scanner will NOT find any source-file 
information for that object.  And of course if the image was LINKED without 
/DEBUG, you won't see ANY source file specifications AT ALL because all DST 
records have been omitted from the image!

   The second and third limitations can both be overcome by first directing 
SYS$OUTPUT to a file, then invoking the DST Scanner, and then sorting the 
output file with SORT /NODUPLICATES.  That will give you a nice, tidy,
listing, in alphabetical order with each unique source file specification 
appearing exactly ONCE.

   But enough about that.  How do you build it, and then how do you USE it?

   To build the DST Scanner, first make sure all the files supplied with 
this package reside in the same directory.  Set your default directory to 
that directory and invoke the command procedure file BUILD_DST_SCANNERS.COM:

   $ @BUILD_DST_SCANNERS

This procedure will perform three essential operations:

   1) Define the logical name DSTDIR to point to the directory where the 
      DST Scanner package's files reside;

   2) Build a Macro library containing assembly-time macros necessary for 
      step 3, below;

   3) Assemble the Macro-32 source code for the DST Scanner, and link it to 
      form the DST Scanner executable images;

   4) Add a new DCL command, "DST", to the then-current process.

Note several things.

   First, the logical name DSTDIR must point, at assembly time, to the 
directory where the Macro LIBRARY (created in step 2) resides.  At run-time 
it must point to the directory where the DST Scanner executable images 
reside.  These NEED NOT be the SAME directory, but things will probably go
more smoothly if they ARE.

   Second, you'll note that I keep saying "DST Scanner executable IMAGES,"
plural.  The DST Scanner package "as shipped" consists of three executable 
images:

   VAX_DST_SCANNER.EXE - reads a VAX image, emits source filespecs ONLY

   VAX_DST_SCANNER_DETAILS - reads a VAX image, emits source filespecs AND
       "other details" -- RMS attributes and EOF position, as shipped, but
       is easily modified to emit, say, source-file creation date...

   AXP_DST_SCANNER - reads an Alpha AXP image, emits source filespecs ONLY

Which program gets run, depends on how you invoke the DST Scanner, which
depends on how you issue the DST command.  We'll get to that momentarily.
Note that ALL THREE of these programs can be built, and will run and do
what they're supposed to, under VAX/VMS (built/tested under V5.4-3) and
OpenVMS AXP (built/tested under (cringe) V1.5-1H1).  I -expect- them to be
buildable, executable, and usable under "all" versions of VMS, but like any
sane person I DON'T GUARANTEE it. 

   Third, logical name DSTDIR gets defined, and the DST command verb added 
to your DCL environment, automatically, _only_ when you invoke 
BUILD_DST_SCANNERS.COM.  Thereafter it's up to YOU to define the logical 
and add the DST command verb.  I'll discuss this further in a moment.

   So now that you've built the DST Scanner and have the command verb all 
ready to use, HOW do you USE it?  Like this:

   $ DST [qualifiers] image_filespec

   Qualifiers:

      /VAX

          Scan a VAX/VMS (OpenVMS VAX) executable image

      /AXP

          Scan an OpenVMS Alpha (AXP) executable image

      /DETAILS

          Display source-file RMS attributes and EOF position in addition
          to file specifications.   The /DETAILS qualifier is allowed only
          when scanning a VAX image.

   You can set up either /VAX or /AXP to be the default, as discussed 
below.

   The build-procedure BUILD_DST_SCANNERS.COM will define the logical name 
DSTDIR, and will add the DST command to your DCL environment.  However, the 
next time you log in you will have to define the logical name, and add the 
command verb, yourself.  

   Defining the logical name is done by using either the DEFINE or ASSIGN
command, like so:

       $ DEFINE DSTDIR directory_where_DST_Scanner_executables_reside
or

       $ ASSIGN directory_where_DST_Scanner_executables_reside DSTDIR

   Adding the DCL command verb is done by using either of the two .CLD 
files provided by this package, with the SET COMMAND command.  Assuming the 
.CLD files reside in the directory pointed to by the DSTDIR logical name, 
the command is either:

       $ SET COMMAND DSTDIR:VAX_DST.CLD
or

       $ SET COMMAND DSTDIR:AXP_DST.CLD

The only difference is that VAX_DST.CLD defines the DST command so that 
when no qualifiers are specified the default is /VAX, and that AXP_DST.CLD 
defines the command so that the default is /AXP.

   Consult your system documentation for information on how to provide the 
DST command on a system-wide basis, or to yourself automatically at login 
or in all processes.  

   This should be enough information for you to build and use the DST 
Scanner.  If it is not, you may contact me (at the time of this writing) on 
the Internet at lvt-cfc@servtech.com .  If that address ever ceases to work, 
hopefully you'll still be able to search the Usenet newsgroups for "Chris 
Chiesa" and find me.  :-)

    Chris Chiesa (lvt-cfc@servtech.com)
     September 19, 1996

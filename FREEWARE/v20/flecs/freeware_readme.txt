FLECS, LANGUAGES, Structured FORTRAN Translator

FLECS is a FORTRAN-to-FORTRAN translator which adds some additional
"structured programming" control constructs to FORTRAN.  It was written in
the early 1970s by Terry Beyer of the University of Oregon and is largely
obsolete since the emergence of FORTRAN-77.  Still, many applications 
written in FLECS still exist, thus having the translator available for
OpenVMS systems is still a benefit.

This version contains modifications by several people, including (most
recently) Selden Ball of Cornell University, to whom I am grateful for 
having furnished the sources.  I made some minor corrections to Fortran
syntax that don't affect the behavior of the program.

In the top-level directory are the following files:

	FLECS.MEM         - User's Manual for the FLECS language
	FLECS.HLP         - HELP file that can be inserted into HELPLIB.HLB
	FLECS-ALPHA.EXE   - Alpha executable version of FLECS
	FLECS-VAX.EXE     - VAX (VMS V5.0 or later) executable
	FLECS-ALPHA.OBJ   - Alpha linkable object
	FLECS-VAX.OBJ     - VAX linkable object

To use FLECS, define a foreign command as follows:

  $ FLECS :== $disk:[dir]FLECS-ALPHA (or FLECS-VAX)
  $ FLECS file.FLX (.FLX file type is assumed)

INCLUDE files are expanded and processed unless they have the "/NOLIST"
qualifier in the file specification. In such cases, FLECS simply passes
along the INCLUDE statement for FORTRAN to handle. If a file is passed 
to FLECS with a "/NOI" qualifier, all include files processing is passed 
on to FORTRAN.

See FLECS.HLP for more information on the command syntax.

Sources for FLECS (and RUNOFF sources for the manual) are in the [.SRC]
directory.  To build FLECS, copy the .FOR and .INC files into a directory,
set default there, and use the following commands:

  $ FORTRAN FLECS,SFLECS
  $ LINK FLECS,SFLECS

The compiler may issue some informational messages about "questionable
branch into block", and variables declared but not used.  The former are
due to FLECS' propensity for creating the following control structure:

	IF (.TRUE.) GOTO 99999
	DO WHILE (expr)
 99999  CONTINUE

I've looked at all the cases where this appears and I believe them to be
harmless (though I'd rap the knuckles of any programmer who did this
deliberately.  It would be hazardous if it were an indexed DO loop.)

The FLECS translator sources are also provided in their original .FLX
format (the .FOR files are the result of running FLECS.FLX and SFLECS.FLX
through FLECS).

The FLECS source code and manual are in the public domain.

If you find errors, feel free to send them to me, Steve Lionel, at
lionel@quark.enet.dec.com, and I'll see what I can do.  Please don't ask me
for enhancements or for assistance with the FLECS language.

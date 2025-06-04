This is the complete VAX SCAN system, sources and all. Also present
are VESTED images for Alpha, built from the VAX images. (No guarantees
about these though.)

The files are generally in ZIP archives to preserve VMS file attributes
and conserve space (this is a LARGE submission) but everything is here.

You may want to look over the directory tree at www.decus.org but it
should be possible to get it all going from this material.

- Glenn Everhart
(OVMS SIG Librarian)


Version: 1.2 
Submitted By: Digital Equipment Corporation 
Operating System: VMS V5.0-V5.3 
Source Language: BLISS 

------------------------------------------------------------------------
Abstract: VAX SCAN is a native mode high level language designed for 
text processing that operates under the VMS Operating System. VAX SCAN 
programs can invoke and be invoked by routines written in other VMS 
languages as well as many Run-Time Library routines. 
VAX SCAN is implemented as an optimizing compiler that produces standard 
VMS object modules. These modules can be linked with modules written in 
other VMS languages to produce an executable image that can be executed 
by the VMS RUN command. 
The VAX SCAN language can be divided into two parts; the first part is a 
block-structured language that can be used for traditional algorithmic 
programming, while the second part deals with pattern matching. 
-------------------------------

	VAX SCAN project files

	Notes from the re-archiver


These files are what remains of the VAX SCAN project.  It has been tested,
and it will build with the appropriate support tools installed.  The
product kit files are in the directory [scan.kit.current]scan*.%.

A careful study of what is included here will certainly show loose ends:
links and references to other things that seem to be missing.  This is
true, but without spending too much time polishing the edges, it must be
so.  VAX SCAN was developed in a group that developed many language
products, and there was much sharing of tools and procedures.  Not all of
those are required to deal with VAX SCAN in isolation, and so they were not
included.

Take care not to delete the VCG object files and library.  The SCAN build
procedure does not exercise the VCG build, it just copies it from another
directory.  I am therefore not sure that everything needed to build VCG is
here, nor even that this is the version of VCG's source that matches this
version of SCAN.

-------------------------------------------------------------------------------

If you run the LOGIN_DECUS.COM (even from some account other than [SCAN],
as I have), it will work around the problems caused by having removed
LOGIN.COM et alia from their original environment.  It assumes that the
SCAN stuff is in a top level directory on a disk labelled SCAN, and hence
that the system has defined DISK$SCAN for the device.  disk$scan, scan$disk
and scan$$disk now all refer to the same device.

The SCAN_BUILD command symbol refers to a .COM file that will ask a few
questions and submit a build of SCAN.  Answering "yes" to the "compiler
build" question will build the compiler.  Answering "no" will build the
RTL.

The DWCI code is expected to fail because DWCI is no longer supported.  (Which
is why I simply commented out that section of the MMS file to deal with the
fact that they were written for VAX C, which I don't have anymore.  DECC/vaxc
gets you part way there, but the DWCI support files are still absent, so LINK
can't find everything).

	    Changes to the Public Domain DVI Driver Family
				 for
		     IBM PC Microsoft Version 5.0
			     [17-Nov-87]

Version 5.0 of Microsoft C finally arrived 13-Nov-87, and I spent the
weekend revising Make to work with this compiler.  Previously,
because of the use of non-standard library functions, it worked only
with Wizard C (now defunct, but forming the basis of Borland
International's Turbo C).  Unfortunately, it would work only in the
small memory model, which in practice limited it to Makefiles of
about 150 lines or less.  This was very inconvenient for the TeX DVI
driver family, which required 5 separate Makefiles.  The new version
of Make now works correctly with Microsoft C Version 5.0, removes
some serious size limitations of the old one, and introduces several
new features; they are described in a 00README.TXT file in the PCMAKE
directory.

Microsoft C Version 5.0 is now essentially completely conformant with
October 1986 draft ANSI C.  Version 4.0 supported draft ANSI C
function prototypes only with type names, not variable names; Version
5.0 eliminates this.  Version 4.0 did not support const and volatile
type modifiers; 5.0 does.    In Version 4.0, function type checking was
selected by defining the preprocessor variable LINT_ARGS; it is
enabled by default with Version 5.0.  The Version 5.0 library also
conforms to draft ANSI C.

The addition of draft ANSI C library conformance changes the types of
a number of functions and arguments, so type declarations have been
modified in gblprocs.h.  Previously, the DVI drivers had a single
flag, ANSI, which previously selected ANSI function prototype
declarations.  This has now been replaced by two, ANSI_LIBRARY, to
indicate library conformance, and ANSI_PROTOTYPES, to indicate
support for function prototypes.  Variable names are NOT used in
these prototypes, so the code will still compile with Version 4.0.

Version 4.0 supported two interfaces to the compiler, MSC and CL.
MSC was the only one available in Version 3.0, so I continued to use
it.  Version 5.0 drops MSC and provides only CL.  The Makefile has
been revised to reflect this.

A few more missing type casts have been installed in the code.
The number of data conversion warning messages has been drastically
reduced by the discovery that a declaration  "float x = 1.0" always
produces a warning, but "double x = 1.0" does not.  The Makefile has
been revised to define float to be double for all the code.  The
warnings that remain (about 3 in each driver, instead of dozens)
appear to possibly be compiler artifacts.  More experimentation is
still required.

Besides full draft ANSI C conformance, Version 5.0 introduces some
new optimization levels, and in-line expansion of some important
library functions from the strxxx and memxxx classes.  I am happy to
report that these seem to work successfully with the DVI drivers, and
optimization level -Oailt is now the default, instead of -Od (no
optimization).   Despite the in-line expansion, the .EXE files are 3K
to 7K bytes shorter than they were with Version 4.0.

Except for the scanf() bug which affects the -o#:#:# option
(documented in the user manual), it looked like Version 4.0 had no
other bugs which affected the drivers.  However, in October 1987, we
found a bug which can be reproduced by a very simple example: the TeX
file containing just 

$VW$
\bye

prints V and W correctly, while

$WV$
\bye

prints V correctly, but produces a black blob for W.   Some length
sessions with Codeview revealed that in the second case, the call to
fseek() in charpk() to position to the bitmap for W was returning a
success status, but not positioning correctly.  In fact, subsequent
getc() calls returned EOF!  End-of-file is not intentionally not
checked for, because in that code section, it cannot happen (unless
the  compiler or library are in error).   Since EOF has the value -1,
or 0xFFFF, this produced a bitmap with all bits set to one, and a
black blob resulted.  Further experimentation showed that a rewind()
issued at the end of readfont() could remove that error, and should
be otherwise harmless, even if it is correctly unnecessary.
With Version 5.0, the bug is no longer there!

A version of qsort() has been added for texidx.   keytst has been
updated for convenience, and a version of sleep() added to it so that
it can be run under PC DOS.

LINK 3.61 supplied with Microsoft C Version 5.0 supports two new
options, /FARCAL and /PACKCODE, which cause the linker to try to
combine code segments, and replace far calls by near calls, in an
effort to speed up the code.  I have not yet experimented with this.

Stack size for programs on the IBM PC remains a thorny issue.
Experimentation with EXEMOD and DVIALW showed that DVIALW would run
with a stack size of 5000 (hex) or larger, but not 4800 (hex) or
less.  I have reduced stack size in MAKEFILE.MSC to 8000 (hex), and
now set it with a LINK option, instead of having to run EXEMOD
separately.  With the new Make, a new rule for producing the .SYM
file has been introduced, shortening the makefile.

One user has reported having downloaded DVI files and font files from
a host that stored them as fixed blocked binary records, with the
last (short) block null-padded.  The DVIxxx.EXE files as distributed
will not handle this.  However, code is there in readgf.h and
readpxl.h to handle this case under OS_VAXVMS; just change the "#if
OS_VAXVMS" to "#if (OS_PCDOS | OS_VAXVMS)".  I have not done this in
the master distribution, because I regard such incorrect padding as an
operating system defect.  Users who download fonts from Unix or
TOPS-20 systems will not experience such problems.


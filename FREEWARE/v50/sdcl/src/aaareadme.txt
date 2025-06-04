SDCL, LANGUAGES, Structured DCL Preprocessor

03-Oct-2000, munroe@csworks.com

Many years ago, this preprocessor was written to provide
"structured programming" constructs (if-then-else, loops, etc.)
for DCL.  In many ways it is still more useful than "vanilla"
DCL, in particular for quickly building large, complex DCL
procedures.  It has been used here at Cottage Software Works (and
before that at Acorn Software) for many years and, despite it's
limitations, provides good duty when "non-trivial" DCL
programming needs to be done.

Dick Munroe

02-Jun-95 Dick Munroe munroe@acornsw.com

Changes:

1. added an on statusCondition { statements } block to sdcl.
2. Variable substitution ('varable') and quoted strings are preserved.
3. Loops emit $NEXT and $BREAK variables which are labels to be used
by DCL commands such as read which have /ERROR processing.

Use it in good health.

Dick Munroe
--------------------------------------------------------------------------------
This directory contains the SDCL (Structured DCL) compiler files. These
include the source files and the documentation files. Here is what you
should have if the tape restore worked ok:

AAAREADME.TXT		The file you are reading

BUN.DCL			Example of structured DCL code

DEFS.H, LEX.C, OUTPUT.C, SDCL.C, STACK.C, STMT.C,
  TCODES.H		Files with C source of SDCL compiler

LASER.DCL,LASER.COM	another example of SDCL input and generated ouput.

MAKEFILE.COM		Command procedure that makes SDCL.EXE

SDCL.EXE		The executable file created under VMS 4.1

SDCLPROJ.DOC		A detailed documentation on the inner workings
			of SDCL compiler. Formatted with UNIX nroff.
			Can be printed on any printer.
SDCLPROJ.NR		UNIX nroff source of SDCL.DOC

SDCLPROJ.TEX            The detailed documentation in TeX input form.
MYFORMAT.TEX            File included in SDCL.TEX when latter is run
			through TeX

SDCL.MEM		A brief guide to SDCL and and how to invoke the
			compiler
SDCL.RNO		DSR source for SDCL.MEM

To get SDCL up and running, you need the DEC C compiler. If you have it,
run the command procedure in MAKEFILE.COM and then install SDCL as a
foreign command:

	$ @makefile
	$! now in your login.com, add the line
	$ sdcl :== $[directory-path]:sdcl
	$! replace "directory-path" with the complete directory
	$! path name to where the sdcl.exe resides.

If you donot have the C compiler, you can simply use the executable 
SDCL.EXE included. It was created under VMS 4.1. 

I have compiled the sources with the UNIX C compiler on the SUN workstation
and HP-UX. The program runs without any change. This implies that you donot 
have the DEC C compiler, try using the DECUS C compiler. This may, hopefully
work.

I will welcome any comments, bug reports, fixes and any other suggestions.
If you make any enhancements, please let me.

	Sohail Aslam
	Computer Science Department
	University of Colorado at Colorado Springs
	P.O.Box 7150
	Colorado Springs, CO 80933-7150
	(303) 593-3332


/* DCREADME.TXT */

			dcc version 2.1h, November 6th, 1998

Introduction
------------

dcc is a C checker program, described in the December 1995 issue of ACM SIGPLAN
Notices (see also 'dccarticle.ps' and 'dccarticle.ascii' included files).


Copyright
---------

//  Copyright Ecole Superieure d'Electricite, France, 1995.
//  All Rights Reserved.


Agreement
---------

// TITLE.  Title, ownership rights, and intellectual property rights in and to
// the Software shall remain in Supelec and/or its suppliers. The Software is
// protected by international copyright treaties.
// 
// DISCLAIMER OF WARRANTY.  Since the Software is provided free of charge, the
// Software is provided on an "AS IS" basis, without warranty of any kind,
// including without limitation the warranties of merchantability, fitness for
// a particular purpose and non-infringement. The entire risk as to the quality
// and performance of the Software is borne by you. Should the Software prove
// defective, you and not Supelec assume the entire cost of any service and
// repair. This disclaimer of warranty constitutes an essential part of the
// agreement. SOME STATES/COUNTRIES DO NOT ALLOW EXCLUSIONS OF AN IMPLIED WAR-
// RANTY, SO THIS DISCLAIMER  MAY NOT APPLY TO YOU AND YOU MAY HAVE OTHER LEGAL
// RIGHTS THAT VARY FROM STATE/COUNTRY TO STATE/COUNTRY OR BY JURISDICTION.
// 
// LIMITATION OF LIABILITY.  UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY,
// TORT, CONTRACT, OR OTHERWISE, SHALL SUPELEC OR ITS SUPPLIERS OR RESELLERS
// BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY INDIRECT, SPECIAL, INCIDENTAL,
// OR CONSEQUENTIAL DAMAGES OF ANY CHARACTER INCLUDING, WITHOUT LIMITATION,
// DAMAGES FOR LOSS OF GOODWILL, WORK STOPPAGE, COMPUTER FAILURE OR MALFUNC-
// TION, OR ANY AND ALL OTHER COMMERCIAL DAMAGES OR LOSSES. SOME STATES/
// COUNTRIES DO NOT ALLOW THE EXCLUSION OR LIMITATION OF INCIDENTAL OR CONSE-
// QUENTIAL DAMAGES, SO THIS LIMITATION AND EXCLUSION MAY NOT APPLY TO YOU.


Bugs
----

No guarantee is given that this version of dcc is free of bugs, although every
efforts are made to chase and eliminate them.

If you uncover one (or more...), please let me know (at e-mail dccsupport@
supelec.fr); a bug report should include a description of the problem and a
short source file causing it, along with the version/release number of dcc
(see below, 'dcc options' paragraph) and the platform (machine/system) used.


Files
-----

There are five sets of files:

- program files:
   dccFiles.mng
   configdc.th
   dc.th
   dcblk.c/h/ph
   dcdecl.c/h/ph
   dcdir.c/h/ph
   dcexp.c/h/ph
   dcext.c/h/ph
   dcfmt.c/h
   dcinst.c/h/ph
   dcmsg.txt*
   dcmain.c/h/ph
   dcprag.c/h/ph
   dcrec.c/h/ph
   dcrecdir.c/ph/th
   dctxttok.c/h

- installation files:
   makefile		(UNIX)
   makefile.win32	(WINDOWS) (courtesy of C. Enache, UPB student, Romania)
   createlocalexec	(UNIX)
   installfile		(UNIX)
   descrip.mms		(VMS)
   installdccvms.com	(VMS)

- execution files:
   adjFiles.dcc		(list of adjustment files)
   *.adj	        (adjustment files)
   dynarray.h		(header file for dynamic arrays)
   starter.dccvmsvax
   starter.dccvmsalpha
   starter.dccgccmips
   starter.dccgccalpha
   starter.dcccchp715   (courtesy of F. Mullet, alumnus)
   starter.dccgcclinux  (courtesy of Ch. Duverger, alumnus)
   starter.dccgccsunos  (courtesy of Ch. Duverger, alumnus)
   starter.dccxlcaix	(courtesy of W. Briscoe, freelance, UK)
   starter.dccclwin32	(courtesy of E. Bezine, junior student)

- test files:
   tstdcc
   tstdccvms.com

- files describing dcc:
   dccarticle.ps
   dccarticle.ascii (to be able to obtain 'diff' with previous version(s))

FILES FROM OLD DISTRIBUTIONS SHOULD BE DISCARDED BEFORE INSTALLING A NEW
VERSION (except possibly 'adjFiles.dcc' and local adjustment files).


Configuration, compilation and installation of dcc
--------------------------------------------------

The configuration phase means possible adaptations of files 'configdc.th',
'dccFiles.mng', and 'dcmsg.txt' (see also 'starter.dcc', next paragraph).

File 'configdc.th' serves to configurate dcc (buffer sizes and so on); most
settings should be valid on any machine (except perhaps smaller memory machi-
nes); comments are supposed to be meaningful enough to make clear the use of
each defined symbol.
The symbol LONGLONG should only be defined on platforms having a larger integer
size than 'long'; its value is then the naming of that type (e.g.
#define LONGLONG long long
or
#define LONGLONG __int64
In the latter case, a "#define __int64 long long" should be put in the starter
file (see later)).
If LONGLONG is not defined, the syntax for 'long long' numerical constants
("LL") is not recognized.
EBCDIC character coding could be supported by changing the 'charInfo' array
(file 'dcrec.c').
The macro IsVisibleChar governs what is an 'invisible character'; it is to be
adapted to locally used character set extension.
The SignedChar/SignedIntField symbols may have to be adapted from their default
value.
Input/output is done entirely via fopen/fread/feof/fflush/fputs/fclose/getchar/
setvbuf. The only other system functions used are: malloc/realloc/free, setjmp/
longjmp, exit/abort, system (to pass control to the local compiler), fileno/
isatty, and getenv.

File 'dccFiles.mng' is to be adapted to local conditions; it indicates the
directory where the adjustment files list file (see next paragraph) and the
starter file are to be found, and the system header files directory. The con-
tent of environment variable DCCFILES, if defined, supersedes these values; its
format is: <adjFilesListDir> or <adjFilesListDir> <space> <sysHdrInclFilesDir>.

Files 'dcmsg.###' contain the text of all messages (including errors/
warnings); since these messages are C string literals, they can be changed
at will. ### indicates the language used for messages; at present, there exist
"eng"lish and "fre"nch versions. The file corresponding to the chosen language
should be copied to file 'dcmsg.txt' (default option: "eng").
The character sequence '@x', where x is a digit in the range '1'-'9', is not
outputted as such, but replaced by a text generated by dcc.
The character sequence '@0' toggles a flip-flop telling whether or not to output
by default the '@0' parenthesized message chunk (output if in interactive mode
with 'e'/'E' command, or with options '+zve', '+zvve').

On an UNIX system, once these files set, compilation is done by typing 'make'.
Then test dcc on itself (type 'tstdcc'): this should generate no error nor war-
ning. The 'make install' command installs dcc into the system (first edit file
'installfile' to define where you want it to be; unmodified, this file installs
dcc in /usr/local/bin/, adjustment files (see next paragraph) in /usr/local/
include/DccAdj/, and the remaining 'execution' files in /usr/local/lib/DccExec-
Files/ (see the DccDir symbol in file 'dccFiles.mng', or the first value in
DCCFILES environment variable)). 

On a VMS system, use 'installdccvms.com' file. The following DCL logical names
has to be defined beforehand:

- dccdir		directory where dcc execution files (see next paragraph)
			are installed,
- dccsysincldir		directory where system header files ('stdio.h' etc.) can
			be found.

Also, 'dcc' has to be defined as a "foreign" command.

On Windows95/WindowsNT, with Microsoft VisualC++4.0, use 'makefile.win32' file.
Do not forget to set the INCLUDE, LIB, and PATH environment variables, e.g. :
set INCLUDE=C:\Program Files\DevStudio\VC\include
set LIB=C:\Program Files\DevStudio\VC\lib
set PATH=C:\Program Files\DevStudio\VC\bin;C:\Program Files\DevStudio\SharedIDE
									    \bin
On other systems, you are on your own...

Note 1: each dcc source file begins with an /* <fileName> */ comment, and ends
        with an /* End <fileName> */ comment, to make it easy to check that it
        has not been truncated by error.
Note 2: possible local compiler warnings while compiling dcc can be ignored
        (provided the compiler is mature...).


'Execution' files
-----------------

Several files are needed by dcc to execute correctly:

- a starter file ('starter.dcc'),
- a number of adjustment files ('xxx.adj'),
- an adjustment files list file ('adjFiles.dcc').


Starter file

Its purpose is to define those symbols that are predefined by the local
compiler. It also gives the name of the compiler (via a string literal, that
must be the value of the '__dcc' symbol), and can override local 'exotic'
features (such as the "globalvalue" specifier of VMS C compilers).
Starter files for VMS C compilers (both VAXs and ALPHAs), the ULTRIX gcc compi-
ler, the OSF1 gcc compiler, the HP cc compiler, the LINUX gcc compiler, the
SUNOS gcc compiler, the AIX xlc compiler and the Windows VisualC++ cl compiler
are included. For SUNOS, the system header files directory must be the gcc
header files directory.
A copy (named 'starter.dcc') of the chosen file has to be made.

Note: the compiler called can be changed from the default by redefining the
'__dcc' symbol in dcc command line (-D or /DEF option).


Adjustment files

Their purpose is to amend system header files, so that their declarations are
acceptable to dcc. Conceptually, an adjustment file is appended to the end of
the corresponding header file.

An adjustment file should not declare/define anything not declared/defined by
the corresponding header file (except if something is missing in it, such as
the prototype for the "sbrk" function in stdlib.h), but they can redeclare at
will function prototypes, external objects, typedefs (even already used ones)
and macros.

Any preprocessor feature can be used in them. The 'cdefined' function (similar
to the 'defined' preprocessor function, but answering True if its parameter is
an identifier defined in the compiled program, not in the preprocessor) can be
used to adjust, in a portable manner, common (but non-ANSI imposed, that is, not
compulsory) functions/objects that are locally defined (see for example
'isascii' management in 'ctype.adj').

Should any header file reveal to be unamendable, it can be purely replaced by
the corresponding adjustment file. The corresponding line in file 'adjFiles.dcc'
must then begin with an exclamation point ('!').

By convention, for a system header file named 'xxx.h', the corresponding
adjustment file is named 'xxx.adj'.

Adjustment files for assert.h, ctype.h, curses.h (courtesy of W. Briscoe, free-
lance, UK), limits.h, math.h, setjmp.h, stdarg.h, stddef.h, stdio.h, stdlib.h,
string.h, time.h and unistd.h are included; adjustment files for the X-Window
library are available, but only partially checked.


Adjustment files list file

It gives the path to the adjustment file corresponding to each system header
file needing adaptation.


Test files
----------

These command files just check dcc on its own source files; this check should
not generate any error nor warning.

There are many more non-regression check files, but they are not included.


Article
-------

Unfortunatly, the wrong version of the paper was published in SIGPLAN Notices;
so the right version (in fact updated and somewhat more detailed) is included,
in Postscript form, along with a pure ASCII form (for 'diff' purposes).


Implemented d-pragmas (as of now)
---------------------------------

For the following, a "file identifier" is defined to be composed of a "file
access path" (machine name, directories...), followed first by a "file name"
and then by a "file suffix" that begins on the first dot '.' seen when scan-
ning the file identifier from right to left.
A "header" file is a source file whose file suffix contains the letter 'h' at
least once, and which is included via the '#include' directive; a "system
header" file is a header file whose '#include' directive uses the '<...>' form.
A "body" file is a source file which is neither a header file, nor a file
included by a header file.

/*~BackBranch*/	avoids warning on backward branchs:
			goto alrdDefLabel /*~BackBranch*/;

/*~CastTo <type> */ allows pseudo-cast between parallel types; also to be used
		    to avoid warning on comparison of difference(s) of unsigned
		    int:
			typedef struct {int x;}Ts; typedef Ts Ts1;
			Ts s; Ts1 s1; unsigned int ui1, ui2;
			...
			s1 = /*~CastTo Ts1*/ s;
			if (/*~CastTo unsigned int */ (ui2 - ui1) > 1) ...

/*~ComposingHdr*/ tells that the whole service provided by the included header
		  file is to be part of the service offered by the including
		  header file:
			#include <math.h> /*~ComposingHdr*/

/*~DccCompliant*/ in a system header file, indicates that the (remaining portion
		  of the) file conforms to dcc requirements, and that 'typedefs'
		  will create parallel types if applicable; usable anywhere (in
		  a system header file),

/*~DollarSign*/	at beginning of module; autorizes '$' in identifiers,

/*~DynInit*/	avoids warning on dynamic initialization (by constants) of
		composite objects:
			[auto] struct _s toto = {...} /*~DynInit*/;

/*~ExactCmp*/	avoids warning on floating point comparison for (in)equality:
			if (fltVar == 0 /*~ExactCmp*/) ...

/*~FullEnum*/	asks dcc to warn if not all constants of the (enum) switch type
		has been used as case values; only useful if a 'default' case is
		used (for example to catch possible 'strange' values of enum
		expression):
			default: /*~FullEnum*/ ...

/*~Generic*/	tells dcc that the returned type of a function is compatible
		with any descendant of this type (subtyping) or, in case of
		'void *' returning function, with any pointer:
			int atoi(const char *) /*~Generic*/;
			double sin(double) /*~Generic*/;
			void *malloc(size_t) /*~Generic*/;

/*~IndexType <type> */ specifies type of index values usable for an array,
		       either statically or dynamically allocated (default
		       index type for static arrays: type of bound, except if
		       bound given by a plain arithmetic constant; then any
		       integral arithmetic type), or that can be added to a
		       pointer. To be used in array/pointer declaration:
			  arr[ArrSiz /*~IndexType TcolorIndex */];
			  bool * /*~IndexType Trow*/ * /*~IndexType Tcol*/
							      twoDimSwitchArray;
		       or pointer creation:
			  & /*~IndexType Tcol*/ col

/*~Init <var>*/	tells the initialization-checking algorithm that, from now on,
		<var> can be considered as initialized. Very specific (see X-
		Window applications, or adjustment files). Usable anywhere (in
		the scope of the variable):
			XtSetArg(arg[0], XmNchildren, /*~Init children*/
								     &children);

/*~LiteralCst*/ tells that expressions of a so qualified type accept unnamed
	        constants without warnings:
			typedef int Int /*~LiteralCst*/;

/*~LocalAdr*/	marks a local address so that no warning on returning it from
		a function, or assigning it to a global/external pointer:
			gblPtr = &localObject + 1 /*~ LocalAdr */;

/*~Masking*/	avoids warning if a macro name is the same than an already
		existing identifier:
			#define /*~Masking*/ macroName macroBody

/*~MayModify */	indicates that modifications may occur through a supposedly
		const-pointing pointer (via casting), or through a struct/union
		containing non-const pointers:
			void fct1601(struct _tag x /*~MayModify*/)

/*~NeverReturns*/ specifies that a void function never returns control:
			static void errExit(...) /*~NeverReturns*/;

/*~NoBreak*/	avoids warning if falling through the end of a 'case' statement:
			case C1 : i = 1;  /*~NoBreak*/
			case C2 : i++;  break;

/*~NoDefault*/	avoids warning if no 'default' case at end of 'switch' statement
		(not to be used if type of switch driving expression is an enum
		type of which all constants have been used as case values):
			case Cn : i = ... ;
			/*~NoDefault*/

/*~NonConstExp*/ makes dcc believe that a (parenthesized) expression is not
		 constant:
			if ((~0==-1)/*~NonConstExp*/) ...
		 (notice that the Pascalian form "while (TRUE)" can be advanta-
		  geously replaced by the C idiom "for (;;)"...),

/*~NotUsed*/	indicates that an object or an enum constant or a formal
		parameter (function or macro) is not used:
			enum {Ce1, Ce2 /*~NotUsed*/, Ce3 ...}
			static void shift (Tstring x, TtypeElt y /*~NotUsed*/);
			#define Sink(x /*~ NotUsed */)
			static const char *bof = "Version 3.3.1" /*~NotUsed*/;

/*~NoWarn*/	to be used in front of sections of code not (yet) dcc-compliant 
		(see also /*~Warn*/, /*~PopWarn*/ d-pragmas; warnings are auto-
		matically disabled inside system header files); usable anywhere,

/*~OddCast*/	to make dcc swallow a cast it frowns upon:
			ptrInt = (int * /*~OddCast*/) ptrStruct;

/*~PopWarn*/	goes back to previous Warn/NoWarn state (see /*~Warn*/, /*~No-
		Warn*/ d-pragmas); at least 16 levels kept; usable anywhere,

/*~PortableQM*/ to make dcc swallow a cast it has good reasons to believe not
		portable:
			ptrFloat = (float * /*~PortableQM*/)ptrDbl;

/*~PrivateTo "<fileName>" [, "<fileName>"]* */   indicates that struct/union
		members, or enum constants, declared thereafter are only
		visible from the indicated file(s), or from macros defined in
		the indicated files or called by such macros; usable anywhere
		(in a header file). Scope: until next /*~PrivateTo*/ or
		/*~Public*/ d-pragma; an '#include' of (another) header file
		creates a hole in the scope, for the duration of the include.
	 	File names may contain joker character(s):
			/*~PrivateTo "dc*.c", "dcrec.h" */

/*~PseudoVoid*/	authorizes a non void-returning function to be used as a
		statement:
			char *strcpy(char *x, const char *y) /*~PseudoVoid*/;

/*~Public*/	indicates end of last /*~PrivateTo*/ scope; usable anywhere (in
		a header file),

/*~ResultPtr*/	for (pointer) parameters that are returned as result; implies
		/*~ResultType*/, plus allows "should be 'const'" pointer chec-
		king propagation:
			char *strchr(const char * /*~ResultPtr*/, char);

/*~ResultType*/	indicates that the type of the result of a function call is the
		type (or the highest type) of the current actual parameter(s)
		corresponding to the so qualified formal parameter(s):
			void *realloc(void *old /*~ResultType*/, size_t size);
			Window mergeWindow(Window/*~ResultType*/,
							 Window/*~ResultType*/);

/*~RootType*/	renders invisible from its hierarchy the subtree headed by the
		so qualified (parallel) type:
			typedef unsigned int Talgn /*~RootType*/;

/*~SameValue*/	must qualify enum constants having the same value that a prev-
		ious constant (of the same enum):
		     enum _colors {BegColors, InfraRed=BegColors /*~SameValue*/
			/* ~SameValue optional here, because 'BegColor' is last
			defined constant */,
				   Red, Orange, Yellow, ...,
				   BegVisibleColors=Red /*~SameValue*/}

/*~SideEffectOK*/ tells that a side effect via a macro parameter is OK:
			#define DangerousMin(x, y/*~SideEffectOK*/) (x<y)? x : y
			DangerousMin(oldMin, tab[i++])

/*~SizeOfMemBlk*/ allows to check, for 'malloc'-like functions using 'sizeof'
		as parameter, whether the type of the sizeof argument is the
		type pointed by the receiving pointer:
			void *malloc(size_t /*~SizeOfMemBlk*/);

/*~SizeOK*/	avoids warning when no width limit is specified ('sscanf'):
			(void)sscanf(charPtr1, "%s", charPtr2/*~SizeOK*/);

/*~TypeCombination <listComb>*/ indicates allowed combinations between indepen-
		dant parallel types. The possible operators are: +, -, *, /, %,
		~+ (non-commutative add) and ~* (non-commutative multiply). Be-
		sides, some combinations are automatically deduced:
		- for '+', from Ta + Tb -> Tc are deduced:
			Tb + Ta -> Tc	(1)
			Ta - Tb -> Tc	(2)
		  Line 1 is not deduced in case '~+'; line 2 is deduced only if
		  Tc = Ta.
		- for '*', from Ta * Tb -> Tc are deduced:
`			Tb * Ta -> Tc	(3)
			Tc / Ta -> Tb	(4)
			Tc % Ta -> Tc	(5)
			Tc / Tb -> Ta	(6)
			Tc % Tb -> Tc	(7)
		  Lines 5, 7 are not deduced if either Ta or Tb is floating.
		  Lines 3, 6, 7 are not deduced in case '~*'.
		If necessary, type hierarchy is searched to find an applying
		type combination.
		Usable only outside of any block. Once given, a type combina-
		tion cannot be desactivated.
			typedef float Volt, Amp, Watt, Ohm;
			typedef int Tgen /* 'generic' type */;
			typedef Tgen Tspe;
			/*~TypeCombination Volt*Amp->Watt,
					   Amp*Ohm->Volt,
					   Tspe~+Tgen->Tspe */
			typedef int Tfruit;
			typedef Tfruit Tapple, Tpear;
			/*~TypeCombination Tapple + Tpear -> Tfruit */
			{
			  Volt v;Amp i;Watt p;Ohm r; Tspe spe;Tgen gen;
			  typedef Tapple MacIntosh;
			  MacIntosh macIntosh; Tfruit fruit; Tpear pear;
			  p = r * i * i;  /* OK */
			  p = v * (v / r);  /* OK */
			  spe -= gen;  /* OK; illegal by default, because
				     contrary to dcc default hierarchy rules. */
			  fruit = pear + macIntosh;}  /* idem */

/*~Undef <listIdent>*/     terminates (for dcc) the scope of all indicated iden-
/*~UndefTag <listIdent>*/  tifiers ('general' or 'tag' name space); usable any-
			   where (at block level 0):
				/*~Undef obj3, type22 */

/*~Utility*/	to be used for functions returning a representation type that
		has no reason to be named:
			int scanf(const char *, ...) /*~Utility*/;

/*~VoidToOther*/ autorizes automatic conversion from (non-generic) 'void *' type
		 to any other pointer type, or to read, via scanf, into a 'void 
		 *' variable:
			objPtr = /*~VoidToOther*/ ptrOnVoid;
			scanf("%i", /*~VoidToOther*/ ptrOnVoid);

/*~Warn*/	to be used in front of dcc-compliant code (default state at beg-
		inning of program, except if '-znw' option used); usable anywhe-
		re; see also /*~PopWarn*/, /*~NoWarn*/ d-pragmas,

/*~zif <boolExp> <stringCst> */   causes emission on stderr (at dcc checking
		time) of <stringCst> if <boolExp> is true; can make use of
		special 'zif' functions (see "Software engineering tool" in the
		paper on dcc), and be used anywhere (also inside macros):
			/*~ zif (sizeof(typArray)/sizeof(typArray[0]) !=
			         __extent(TtypName)+1) "Array 'typArray': bad "
								     "length" */

Notes:	- For dcc, a d-pragma is made up of tokens, perfectly macro-substituta-
	  ble; for instance, the /*~LocalAdr*/ d-pragma consists of the three
	  following tokens:

			/*~         (pseudo-token)
			LocalAdr    (identifier)
			*/          (normal token)

	- An empty d-pragma name is legal; the d-pragma is then ignored.
	- D-pragmas are visible with the '+zlt' option.


Indentation rules
-----------------

Indentation is checked only on lines beginning by a declaration or a statement.

- general case:  indentation level is increased on entering a block (just after
		 its opening brace), for the duration of the block, or on ente-
		 ring a substatement (statement controlled by 'if', 'else',
		 'do', 'while', 'for'), for the duration of the substatement.

- special cases: . no change of indentation level for construct "else if", both
						      tokens being on same line;
		 . indentation level increased only once if substatement is a
						  block (outside of macro body);
		 . 'case/default's can be lined up with the corresponding
								       'switch'.


dcc options
-----------

Their list can be obtained by executing a dcc command alone (no argument); the
version/release number is also given. If the prefix '+' is replaced by '-', or
vice versa, the option effect is reversed.

dcc also interprets -I/-D/-U cc options (or their VMS counterparts), besides
passing them to the compiler.

For VMS, only one macro can be defined by a given /DEF, and /DEF,/INCL,/UNDEF
options have to be separated by space(s).

The environment variable DCCDFLTS can contain any part of the command line; its
content is logically added (just after 'dcc') to the beginning of all following
dcc command lines.

A given option is valid for all following files met in command line, unless
reverted; options given after last file name apply only to last file.


Exit status
-----------

dcc provides six different exit statuses, whose values depend on local platform
(see file 'configdc.th'): EXIT_SUCCESS, two EXIT_WARNINGS, two EXIT_ERRORS, and
EXIT_FAILURE (returned when dcc has to stop before end).


Badly implemented features (as of now)
--------------------------------------

- floating constants are recognized lexically, but ill converted if fractional
  part or non-null exponent (only noticeable in array declaration, e.g. "char
  arr[(int)(2.5+6.5)]" => [4], not [9]),
- 'sizeof' does not take into account alignment holes,
- commas in macro body not correctly interpreted if dynamically created,
- portability not checked in all cases.


Unimplemented features (as of now)
----------------------------------

- check for lack of 'volatile' qualifier in the reach of a setjmp()/longjmp()
									   pair,
- check for non-ambiguity of external identifiers,
- size of objects not given ('+zsy' option),
- computation of floating-point constant expressions,
- propagation of /*~MayModify*/ information.


Known bug(s)
------------

Macro are sometimes badly expanded in convoluted cases involving concatenation
(##) and quotation (#).


Main changes with respect to version 2.0:
-----------------------------------------

- several compilation units checkable with same dcc command,
- introduction of DCCDFLTS and DCCFILES environment variables,
- new exit statuses.

'16 bits int' platforms
-----------------------

Portage in progress.


/* End DCREADME.TXT */

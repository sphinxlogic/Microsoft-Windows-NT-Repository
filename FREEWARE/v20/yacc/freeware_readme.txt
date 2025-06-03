YACC, TOOLS, Berkeley yacc

aug-1995 Sheldon Bishov, bishov@star.zko.dec.com

The source for files was University of California at Berkeley.

Builds tested on VAX with VAXC and DECC and Alpha with DECC.

To build, set default to source directory and enter the command:

$ @vmsbuild {param}

where {param} is VAXC (for VAX only) or DECC (for VAX or Alpha).


A command procedure build-yacc.com is provided to make builds from
*.yacc source code easier.  Also provided is calc.yacc, which
produces a simple calculator application.  This code was adapted
from a sample for GNU bison.

The following changes were made in the code:

build-yacc.com: new procedure, makes use of yacc on source files easier

defs.h: Include errno.h, changes to use VMS file extensions vs UNIX (which
         can have >1 dot)

error.c: Format of some continuation was too long for VAXC/DECC

lr0.c: Remove extra static core decl.

main.c: Use free instead of unlink; use logindir/tmp vs. /tmp

skeleton.c: Use fprintf to stderr instead of yyerror

verbose.c: Format of some continuation lines was too long for VAXC/DECC

vmsbuild.com: new procedure; adapted from vmsbuild.com for gawk from
         Free Software Foundation.

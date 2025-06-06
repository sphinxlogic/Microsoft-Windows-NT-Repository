FILES_INFO, UTILITIES, Display all processes that have a particular file open

FILES_INFO V2.0-000

FILES_INFO accepts a filename on the DCL command line and displays all
the processes that have the file open for reading/writing.

Warning: elevated privileges are required; this program does contain
kernel-mode code to access system data structures.

To build an executable, either execute @LINK.COM or go to the [.SOURCE]
subdirectory and rebuild it using MMK.

--------------------------------------------------------------------------------
Files in this directory:

AAAREADME.DOC			This file
AAAREADME.TOO			Revision history
FILES_INFO.ALPHA_OLB		FILES_INFO object library (Alpha AXP)
FILES_INFO.ALPHA_OPT		FILES_INFO linker options file (Alpha AXP)
FILES_INFO.OLB			FILES_INFO object library (VAX)
FILES_INFO.OPT			FILES_INFO linker options file (VAX)
LINK.COM			Command procedure to link the executables

Files in [.SOURCE]:

DESCRIP.MMS			Description file for building FILES_INFO.
FILES_CLD.CLD			DCL command definition source.
FILES_INFO.ALPHA_OPT		FILES_INFO linker options file (Alpha AXP)
FILES_INFO.C			C source for FILES_INFO main routine.
FILES_INFO.OPT			FILES_INFO linker options file (VAX)
FILES_MACRO.MAR			MACRO support routines for FILES_INFO.C.
LOCK_NONPAGED_CODE.MAR		Routine to lockdown code pages.

--------------------------------------------------------------------------------
CONTACTING THE AUTHORS

Comments, suggestions, and questions about this software can be directed to
the authors at one of the following addresses:

Mail:	Edward A. Heinrich
E-mail:	heinrich@BYU.EDU
--------------------
Mail:	Hunter Goatley
E-mail:	goathunter@GOATLEY.COM

--------------------------------------------------------------------------------
COPYRIGHT NOTICE

This software is COPYRIGHT � 1994,  The LOKI Group, Inc. ALL RIGHTS RESERVED.
Permission is granted for not-for-profit redistribution, provided all source
and object code remain unchanged from the original distribution, and that
all copyright notices remain intact.

DISCLAIMER

This software is provided "AS IS".  The authors and the LOKI Group, Inc.
make no representations or warranties with repsect to the software and
specifically disclaim any implied warranties of merchantability or fitness
for any particular purpose.

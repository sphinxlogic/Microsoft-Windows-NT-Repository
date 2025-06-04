GETCMD V3.8, UTILITIES, Display others' command recall buffers

GETCMD V3.8
Copyright © 1992--1998, Hunter Goatley.  All rights reserved.

This kit contains all the files necessary for installing the GETCMD utility.
Requires VMS V5.0 or later (VAX) or OpenVMS Alpha V1.0 or later.

GETCMD.B32 is a BLISS-32 program that will display the DCL command
recall buffer for any interactive process on the system.

BUILDING
--------
To build GETCMD, just execute the LINK.COM procedure:

$ @LINK

If you have BLISS and want to build from the sources, an MMS file is
provided in [.SOURCE].


USAGE
-----
To set up GETCMD, just define a foreign symbol to run it:

$ getcmd :== $dev:[dir]getcmd.exe

where "dev:[dir]" is the device and directory where GETCMD lives.

To use it, simply specify a PID on the command line:

$ getcmd 202000AF

Qualifiers that may be specified are /PAGE, to do pause the output
after each full screen, and /OUTPUT=xxx, to redirect the output to a
file.

LIMITATIONS
-----------
GETCMD will not yet work with processes on other nodes in a cluster,
though this functionality is planned for a future release.

--------------------------------------------------------------------------------
Files in this directory:

AAAREADME.DOC		This file
AAAREADME.TOO		Change history
LINK.COM		Command procedure to link the image
GETCMD.ALPHA_OLB	GETCMD object library for Alpha AXP
GETCMD.ALPHA_OPT	Options file for Alpha AXP
GETCMD.HLP		On-line help for GETCMD
GETCMD.OLB		GETCMD object library for VAX
GETCMD.OPT		Options file for VAX

Files in [.SOURCE]:

CVTHELP.TPU		TPU source to convert .HELP file to .RNH
DESCRIP.MMS		Description file for building GETCMD
GETCMD.B32		BLISS source for main GETCMD routines
GETCMD.ALPHA_OPT	Options file for Alpha AXP
GETCMD.HELP		Source for GETCMD HELP file
GETCMD.OPT		Options file for VAX
GETCMD_CLD.CLD		CLI$ definitions file
GETCMD_MSG.MSG		Messages used by GETCMD
HG_OUTPUT.B32		BLISS source for output routines

--------------------------------------------------------------------------------
CONTACTING THE AUTHOR

Comments, suggestions, and questions about this software can be
directed to the author at one of the following addresses:

Mail:	Hunter Goatley
	P.O.  Box 51745
	Bowling Green, KY 42102-6745

E-mail:  goathunter@PROCESS.COM

--------------------------------------------------------------------------------
COPYRIGHT NOTICE

This software is COPYRIGHT © 1992--1998, HUNTER GOATLEY.  ALL RIGHTS RESERVED.

Permission is granted for not-for-profit redistribution, provided all
source and object code remain unchanged from the original
distribution, and that all copyright notices remain intact.

DISCLAIMER

This software is provided "AS IS".  The author and Process Software make no
representations or warranties with respect to the software and
specifically disclaim any implied warranties of merchantability or fitness
for any particular purpose.

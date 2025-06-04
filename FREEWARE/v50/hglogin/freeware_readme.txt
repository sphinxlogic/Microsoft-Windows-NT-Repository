HGLOGIN, SYSTEM_MGMT, Log in to another account without a password

HGLOGIN (Hunter's LOGIN)
Copyright © 1997, 1998, 2000, Hunter Goatley.  All rights reserved.

------------------------------
HGLOGIN is a rewrite and enhancement of the GLOGIN utility written by
Anthony C. McCracken, Northern Arizona University, in 1992.

HGLOGIN uses the PTD$ routines supplied by VMS and may not work on
older versions of VMS.
------------------------------

HGLOGIN lets privileged users log in to a named account without having
to know the password for that account.  A process running under the
target username is created.  Its input and output are read from a
pseudo-terminal, which is controlled by HGLOGIN.

Unlike BECOME and SWAP, the process created by HGLOGIN is a full
process, with all the privileges, rights identifiers, quotas, DCL
symbols, logical names, etc., as well as anything else that is set up
in the target user's LOGIN.COM.

HGLOGIN requires the target username as a parameter.  If there is
other text on the line, HGLOGIN enters "single command mode." The
remaining text is passed to the new process to be executed by DCL. 
When the command execution completes, the process is automatically
logged out and control returns to the original process.

You can use HGLOGIN to log into DISUSERed accounts by specifying the
qualifier /NOUAF.


INSTALLING HGLOGIN
------------------
To build the executable:

* Execute the following command to produce HGLOGIN.EXE from the supplied
  binaries:

	$ @LINK.COM

* If you have the BLISS sources and the MMS description file available,
  use MMK to compile and link the executable.

* You can install the online help in a help library using a command like
  the following:

        $ LIBRARY/HELP/INSERT SYS$HELP:HELPLIB.HLB HGLOGIN.HLP

Once you have the HGLOGIN.EXE file, you can set up a DCL foreign command
to run HGLOGIN:

	$ hglogin :== $disk:[directory]HGLOGIN.EXE

To log in as user SYSTEM, simply issue:

	$ hglogin system

The effect will be similar to using SET HOST 0 or TELNET to log in to
local system.

To execute a single command under the target username, issue a command
like:

	$ hglogin system mail

In the example above, you will be logged in as SYSTEM and the MAIL
command will be executed in the SYSTEM process.  When you QUIT or EXIT
MAIL, you're automatically logged out of the SYSTEM process.

HGLOGIN and SET TERM/INQUIRE
----------------------------
HGLOGIN's single-command mode will not work properly if either
SYLOGIN.COM or the target user's LOGIN.COM include the command SET
TERMINAL/INQUIRE.  Normally, the commands sent to the process are
purged when the SET TERMINAL/INQUIRE command executes.

In cases like this, you must use the /PROMPT qualifier to tell HGLOGIN
what the DCL prompt will be once the process is logged in.  HGLOGIN
will then wait for the given prompt (which defaults to "$") before
sending its commands to the process.

	$ hglogin/prompt="What?" system mail

There is also a /NOQUIET qualifier that can be given to override
HGLOGIN's "quiet mode" in single-command mode.  HGLOGIN/NOQUIET will
display all the output from the process.

IF you try to use single-command mode and never see any output, type
LOGOUT and then use HGLOGIN/NOQUIET to see the output and prompt.  You
can then use /PROMPT to force HGLOGIN to wait for the DCL prompt.

--------------------------------------------------------------------------------
CONTACTING THE AUTHOR

HGLOGIN was written by Hunter Goatley.  Comments, suggestions, and
questions about this software can be directed to this e-mail address:

	goathunter@GOATLEY.COM

--------------------------------------------------------------------------------
COPYRIGHT NOTICE

This software is COPYRIGHT © 1997, 2000 HUNTER GOATLEY. ALL RIGHTS
RESERVED. Permission is granted for not-for-profit redistribution, provided
all source and object code remain unchanged from the original distribution,
and that all copyright notices remain intact.

DISCLAIMER

This software is provided "AS IS".  The author and Process Software make
no representations or warranties with repsect to the software and
specifically disclaim any implied warranties of merchantability or fitness
for any particular purpose.

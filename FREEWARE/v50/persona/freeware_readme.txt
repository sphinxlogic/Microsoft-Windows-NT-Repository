PERSONA, UTILITIES, Become another user using identifiers

PERSONA (Assume persona of another user)
Copyright © 1999 - Lyle W. West  All rights reserved.

------------------------------
PERSONA is a C rewrite and enhancement of the GLOGIN utility written by
Anthony C. McCracken, Northern Arizona University, in 1992.

PERSONA uses the PTD$ routines supplied by VMS and may not work on
older versions of VMS. Early in the image, if device FTA0: does not
exist, PERSONA will exit with appropriate error message.
------------------------------

PERSONA permits users to login to a named user account without having
to know the password for that account.  A process running under the
target username is created.  Its input and output are read from a
pseudo-terminal, which is controlled by PERSONA.

Unlike BECOME and SWAP, the process created by PERSONA is a full
process, with all the privileges, rights identifiers, quotas, DCL
symbols, logical names, etc. as well as whatever else that is set up
in the target user's LOGIN.COM.

PERSONA requires the target username as a parameter.  Logging out
of the PERSONA account returns the terminal device and user to the
process which originally invoked PERSONA.

You must set up a DCL foreign command to run PERSONA (since I use a
modified version of Joe Meadows CLI routines):

	$ persona == "$DISK:[DIRECTORY]PERSONA.EXE"

To log in as user SYSTEM (assuming required idenifier), simply issue:

	$ persona system

The effect will be similar to using SET HOST 0 or TELNET to connect to
local system, except there will be no Username and Password prompts.

Help is available by entering PERSONA /HELP, which provides user info.
Users of PERSONA must have the privs described in the above help routine,
copy PERSONA.EXE to SYS$SPECIFIC: and use INSTALL.COM to install the 
image. Security is not a real issue because of the identifer requirement,
or the SysMgr password entry.

PERSONA was created to permit nonprivileged users the ability to assume
the PERSONA of another user, provided they have an identifier granted to
the invoking user in the form of PERS_xxxx, where xxxx is the target
username. If the identifier is not granted to the invoking user,
a message indicating user is not authorized to assume the target user's
persona. It is designed to be an installed image with associated privs.

The identifier can be circuvented by use of the /IGNOREID command line
qualifier, where the user is then prompted to enter the system
manager's password.

This release permits minimal logging of PERSONA usage in that if within 
the LNM$SYSTEM table, the logical name PERSONA_LOG is defined and the 
equivalence string is a valid filespec, then entry and exit stats will be
recoded in this file.

Portions of this document were stolen from Hunter Goatley's
HGLOGIN bliss implementation, also based on GLOGIN.

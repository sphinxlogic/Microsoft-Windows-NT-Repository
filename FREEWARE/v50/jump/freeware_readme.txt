JUMP, SYSTEM_MGMT, Become a different user exactly or in part

JUMP is a  program  which  allows  selected  users (Systems Programmers,
Operators and specifically authorised users) to change elements of their
process to those of another user, or to become *exactly* the target user
in  a pseudo-terminal.  Limitations are placed on Operators: they cannot
target users who have more than a certain set of privileges unless  they
are specifically authorised to do so.

All users are validated against an access file to determine any specific
access to be granted or denied  in  addition to any default access (e.g.
for  Operators).   The access file also may contain options relating  to
security monitoring of sessions.

Exact jumps can be monitored with secure log files.

For non-exact jumps, items which are changed to those of the target user
are:

  Username (Systems Programmers only, using the SETUSER or ALL qualifier)
  UIC, Default Directory, Default Disk, LNM$GROUP.

Privileges and process rights are NOT changed.

It is recommended that you thoroughly read the HELP file (JUMP.HLP) and
the example access file (JUMP_ACCESS.DAT) before installing JUMP.

To install JUMP:

  1. Re-link JUMP (NO Traceback) as directed in the build procedure.
  2. Define EXECUTIVE mode SYSTEM table logical names as required (see help).
  3. If required, create the JUMP_ACCESS rights ID.
  4. If required, grant the JUMP_ACCESS rights ID to authorised users.
  5. If the JUMP_ACCESS rights ID is *not* created, define the
        JUMP_DOUBLE_CHECK logical name to be "FALSE".
  6. If required, create/edit the access file (see example file).
  7. INSTALL JUMP with the following privileges:
	CMEXEC, CMKRNL, DETACH (or IMPERSONATE), SYSNAM, SYSPRV
  8. Set access rights on all files as appropriate.
  9. Define a foreign command: $ JUMP :== $device:[directory]JUMP
 10. Boing!

The author of JUMP is

    Jonathan Ridler,
    Information Technology Services,
    The University of Melbourne,
    Victoria, AUSTRALIA, 3010.

    Email: jonathan@unimelb.edu.au

Note: Some ideas in JUMP have been drawn from Eric Wentz's program BECOME.
      The code for pseudo-terminal creation is derived from Anthony
      McCracken's GLOGIN program.  Thanks to Bob Beckerhof for testing and
      helpful suggestions.  Special thanks to Jeremy Begg for code to create
      secure log files and other minor fixes.

JUMP is written entirely in  DEC  Pascal.   The current version has been
built and tested on OpenVMS VAX v7.1 (DEC Pascal v5.6) and OpenVMS Alpha
v7.1-2 (DEC Pascal v5.7).

The current version of JUMP is v3.4 (10-Nov-1999).

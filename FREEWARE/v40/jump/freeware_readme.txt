JUMP v3.0, SYSTEM_MANAGEMENT, Become a different user exactly or in part

JUMP is a  program  which  allows  selected  users (Systems Programmers,
Operators and specifically authorised users) to change elements of their
process to those of another user, or to become *exactly* the target user
in  a pseudo-terminal.  Limitations are placed on Operators: they cannot
target users who have more than a certain set of privileges unless  they
are specifically authorised to do so.

All users except Systems  Programmers  are  validated against an  access
list  file  to determine any specific access to be granted or denied  in
addition to any default access (e.g.  for Operators).

For non-exact jumps, items which are changed to those of the target user
are:

  Username (Systems Programmers only, using the SETUSER or ALL qualifier)
  UIC, Default Directory, Default Disk, LNM$GROUP.
  Privileges and process rights are NOT changed.

To install JUMP:

  1. Re-link JUMP (NO Traceback) as directed in the build procedure.
  2. Define EXECUTIVE mode SYSTEM table logical names as required (see help).
  3. If required, create the JUMP_ACCESS rights ID.
  4. If required, grant the JUMP_ACCESS rights ID to authorised users.
  5. If required, create/edit the user access data file (see example file).
  6. INSTALL JUMP with privileges: CMEXEC, CMKRNL, DETACH, SYSNAM, SYSPRV
  7. Define a foreign command: $ JUMP :== $device:[directory]JUMP
  8. Boing!

The author of JUMP is

    Jonathan Ridler,
    Information Technology Services,
    The University of Melbourne,
    Parkville, Victoria, AUSTRALIA, 3052.

    Email: jonathan@unimelb.edu.au

Note: Some ideas in JUMP have been drawn from Eric Wentz's program BECOME.
      The code for pseudo-terminal creation is derived from Anthony
      McCracken's GLOGIN program.  Thanks to Jeremy Begg for a minor code
      fix and testing, and Bob Beckerhof for testing and helpful suggestions.

JUMP is written entirely in DEC Pascal and has been built and tested on
OpenVMS VAX v6.2 (DEC Pascal v5.4) and OpenVMS Alpha v6.2 and v7.1 (DEC
Pascal v5.5).

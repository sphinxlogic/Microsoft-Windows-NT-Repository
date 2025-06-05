JUMP, SYSTEM_MGMT, JUMP 4.2 Login as a different user without a password

JUMP allows a user to login exactly  as another user without a password.
It  also  allows a more restricted (non-exact) impersonation of  another
user.

Use of JUMP  is  restricted  to  specific  categories  of users: Systems
Programmers, Operators and any specifically authorised users.

All users are validated against an access file to determine any specific
access to be granted or denied  in  addition to any default access (e.g.
for  Operators).   The access file also may contain options relating  to
security monitoring of sessions.

Exact jumps can be monitored with secure log files.

For non-exact jumps, items which are changed to those of the target user
are:

  UIC, Default Directory, Default Disk, LNM$GROUP.

  Username (Systems Programmers only, using the SETUSER or ALL qualifier)

Privileges and process rights are NOT changed.

----

The author and maintainer of JUMP is Jonathan Ridler.
JUMP is maintained on behalf of ITS, The University of Melbourne, AUSTRALIA.

Email: jump-enquiries@unimelb.edu.au

The current version of JUMP is V4.2 2002-08-14 (14-Aug-2002).

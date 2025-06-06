ACM47, GAMES, An aerial combat simulator for X11 mono/multi-user

ACM Flight Simulator,  revision 4.7


What is ACM?
------------

ACM is a LAN-oriented, multiplayer aerial combat simulation.  My main design
objective was to provide source code that could be easily compiled and
executed on a wide variety of platforms.  To that end, acm is written entirely
in C, exploiting the programming features of Unix, X11, and the BSD socket
interface.

Players engage in air to air combat against one another using heat seeking
missiles and cannons.

ACM is implemented as two programs.  The first, named "acm", is a small
program that, when invoked, starts a flight session on a given workstation.
The second component, named "acms", is a server process that manages the
multiplayer environment.  It also manages the flight simulation and
display management that is required.  Most players will prefer to run the
acms process on a back-end server system on their local area network.  Players
at client workstations can then invoke the acm program to begin play.


Riley Rainey (rainey@netcom.com OR riley@atria.com)
Dallas, Texas
November 20, 1994


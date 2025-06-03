SD 01-007  [ 6-JUL-1994]
Copyright © 1994, MadGoat Software.  All rights reserved.

This is my version of SD, a SET DEFAULT replacement based on an old DECUS
submission by Alan Zirkle.  This version implements a stack of the last 10
directories visited, as well as short-cuts for specifying directory specs.

  Set/Show Default Directory

  SD ^      Go up one subdirectory level
  SD @      Go to top level of current directory (enter TOP for help)
  SD .      Go to login default directory and disk
  SD #n     Go to directory in n'th entry of the SD stack
  SD #      Set default to stack entry #1 (toggle stack entries)
  SD >X     Set default to [z.X] when currently in [z.y]
  SD .X     Set default to [current.X]
  SD X.Y.Z  Set default to [X.Y.Z] (enter X for help)
  SD %      Push the current default onto the stack
  SD *      Show the SD stack (enter STACK for help)
  SD ?      Show this SD help information
  SD        Show the current default


To build it, just @LINK.COM or use MMS (MMK) or @BUILD_SD.COM.  For SD
to find its help file, you should set its protection to W:RE and define
the following logical:

	$ copy sd.hlb dev:[dir]/prot=w:re
	$ define/system/exec MADGOAT_SD_HLB dev:[dir]SD.HLB

Then define a foreign symbol:

	$ sd :== $dev:[dir]sd.exe

SD ? gets you help.

This version of SD has some pretty ugly code, but it was originally
designed to be as fast as possible on a VAX.  It has been ported to AXP,
and should really have been rewritten, but it works, so I have bothered
with it.

Hunter
------
Hunter Goatley, VMS Systems Programmer, Western Kentucky University
goathunter@ALPHA.WKU.EDU (or goathunter@WKUVX1.WKU.EDU)

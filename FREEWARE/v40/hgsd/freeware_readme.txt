HGSD V2.3, UTILITIES, Yet another SET DEFAULT replacement

SD V2.3  [31-AUG-1998]
Copyright © 1998, Hunter Goatley.  All rights reserved.

This is my version of SD, a SET DEFAULT replacement based on an old DECUS
submission by Alan Zirkle.  This version implements a stack of the last 16
directories visited, as well as short-cuts for specifying directory specs.

  Set/Show Default Directory

  SD ^      Go up one subdirectory level
  SD @      Go to top level of current directory (enter TOP for help)
  SD .      Go to login default directory and disk
  SD #n     Go to directory in n'th entry of the SD stack
  SD #      Set default to stack entry #1 (toggle stack entries)
  SD >X     Set default to [z.X] when currently in [z.y]
  SD >      Move to alphabetically next directory at same level
  SD <      Move to alphabetically previous directory at same level
  SD .X     Set default to [current.X]
  SD X.Y.Z  Set default to [X.Y.Z] (enter X for help)
  SD %      Push the current default onto the stack
  SD *      Show the SD stack (enter STACK for help)
  SD ?      Show this SD help information
  SD &      Unconceal a directory specification
  SD        Show the current default

SD can also set your prompt equal to the default directory name if the
logical HG_SD_SET_PROMPT is defined.

INSTALLING SD
-------------
To build the executables:

* Execute the following command to produce SD.EXE and SD_PROMPT.EXE from
  the supplied binaries:

	$ @LINK.COM

* If you have the BLISS sources and the MMS description file available,
  use MMK or MMS to compile and link the executable.

Once you have the SD*.EXE files, you can set up a DCL foreign command
to run SD:

	$ sd :== $disk:[directory]SD.EXE

For SD to find its help file, you should set its protection to W:RE
and define the following logical:

	$ copy sd.hlb dev:[dir]/prot=w:re
	$ define/system/exec HG_SD_HLB dev:[dir]SD.HLB

You can also include the file SD.HLP in a local help library to make
it available via the HELP command.

SD ? gets you help.

SETTING THE DCL PROMPT
----------------------
The SET_PROMPT.EXE image contains code to set the DCL prompt to the
name of the current default directory.

	$ sd :== $disk:[directory]SD_PROMPT.EXE

To actually have SD set your prompt, the following logical must be
defined:

	$ DEFINE HG_SD_SET_PROMPT TRUE

Note that you must have CMEXEC privilege in order to set the DCL
prompt. To allow non-privileged users to set their prompts,
SD_PROMPT.EXE can be INSTALLed with CMEXEC privilege:

	$ install add disk:[dir]sd_prompt.exe/open/header/share-
		/priv=(noall,cmexec)

--------------------------------------------------------------------------------
CONTACTING THE AUTHOR

hg SD was written by Hunter Goatley.  This version is a C rewrite
of the original MACRO32 program, written March 14, 1987. Comments,
suggestions, and questions about this software can be directed to the
current maintainers at this e-mail address:

	HG-Bugs@MX.PROCESS.COM

--------------------------------------------------------------------------------
COPYRIGHT NOTICE

This software is COPYRIGHT © 1987, 1997, 1998 HUNTER GOATLEY. ALL RIGHTS
RESERVED. Permission is granted for not-for-profit redistribution, provided
all source and object code remain unchanged from the original distribution,
and that all copyright notices remain intact.

DISCLAIMER

This software is provided "AS IS".  The author, Hunter Goatley, and
Process Software make no representations or warranties with repsect to
the software and specifically disclaim any implied warranties of
merchantability or fitness for any particular purpose.

SHOW_USERS_V12, UTILITIES, Display/KILL/FORCE exit users (enhanced WHO/WHAT program)

DESCRIPTION:
	FORTRAN example of how to use $GETJPI and $PROCESS_SCAN to Select
	Process Information by Username, IMAGE, or NODENAME.  This program uses
	SHOW_USERS_V12.CLD to define the command environment.

USAGE:
	Either copy the correct executable to SYS$SYSTEM: as SHOW_USERS.EXE
	    $ COPY SHOW_USERS_V12.EXE_ALPHA SYS$SYSTEM:SHOW_USERS.EXE)

	- or -

	Define the logical SHOW_USERS.EXE to point to the executable
	    $ DEFINE SHOW_USERS.EXE DKA300:[BUBBA]SHOW_USERS_V12.EXE_VAX

	Then insert the VERB into your DCLTABLES with:
	$ SET COMMAND SHOW_USERS_V12

EXAMPLES:

$ dispusers bri

System: TALIN

Pid      Username Process name Pr CPU    Login time   Image
20400067 BRIVAN   BRIVAN        4  66590 16-FEB 08:37

System: BARFBAG

Pid      Username Process name Pr CPU    Login time   Image
206008E4 BRIVAN   BATCH_713     4    832 28-FEB 14:53 COPY
20600069 BRIVAN   BRIVAN        4 473524 16-FEB 08:37 TYPE

- or -

$ dispusers bri/image=type

System: BARFBAG

Pid      Username Process name Pr CPU    Login time   Image
20600069 BRIVAN   BRIVAN        4 473524 16-FEB 08:37 TYPE

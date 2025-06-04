BOOTSYNC, UTILITIES, Cluster Satellite Bootstrap Sequencer

The purpose of BOOTSYNC is to prevent too many satellite systems from trying
to boot at the same time.

The theory of BOOTSYNC is to run this program as early in the system startup 
as is reasonable; in SYCONFIG, early in SYSTARTUP, or in SYLOGICALS.  The 
program will use the lock manager to synchronize with other copies of itself 
running on other systems in the cluster.

Some logical names can be defined to control BOOTSYNC:

BOOTSYNC$COUNT	(Required) Count of the number of systems to boot at the
		same time.  (Maximum of 32, controlled by a literal.)

BOOTSYNC$NAME	(Optional) Arbitrary string to be added to the resource
		name for the lock manager (usually use boot node name).
		Clusters with more than one boot node can allow each boot
		node to boot the specified number of systems.

BOOTSYNC$RUN	(For debugging) If defined (as anything) force BOOTSYNC
		to run.  Normally, BOOTSYNC will not run unless all the
		following are true:
		· process name is STARTUP (AUTOGEN executes SYCONFIG, which
		  may execute BOOTSYNC)
		· system is a satellite (votes = 0, member of cluster)
		If BOOTSYNC decides not to run, it exits taking no action.
		These tests allow BOOTSYNC to be run as part of a common
		system startup.

Installation instructions:

1.  Place BOOTSYNC.EXE in SYS$MANAGER:, or any other location.
2.  Decide where in the system startup to run BOOTSYNC.
    Recommendation:  SYCONFIG.COM
    Alternatives:    SYSTARTUP_V5.COM, SYLOGICALS.COM
3.  Decide how many systems may boot at the same time.
    Recommendation:  6 to 8 per boot node is a reasonable start.
4.  Insert something like the following into the file you picked in step 2:
	$ DEFINE BOOTSYNC$COUNT 6
	$ DEFINE BOOTSYNC$NAME 'F$GETDVI("SYS$SYSDEVICE","HOST_NAME")
	$ RUN SYS$MANAGER:BOOTSYNC

Building instructions:

BOOTSYNC is in 2 modules, BOOTSYNC.BLI (Bliss) and HANDLER.MAR (Macro32
kernel-mode code for VAX).  Compile the modules, and link them.

HANDLER.MAR may or may not bugcheck the system, and may not be immediately
portable over to OpenVMS Alpha without requiring code modifications.


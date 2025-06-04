KP_LOCKTOOLS, UTILITIES, Tools for monitoring and controlling Lock Manager

(See http://www.geocities.com/keithparris/, particularly
http://www.geocities.com/keithparris/decus_presentations/f2000_vms_lock_manager.ppt
or the latest incarnation of that presentation, for more-detailed background
and theory on these tools.) 

Determine which lock trees are the busiest, cluster-wide:
	LOCK_ACTV.COM - DCL to collect and display lock-rate data immediately
	LOCK_ACTV_DC.COM - DCL to collect lock-rate data only (no analysis)
			   for lower system overhead at time of data collection
	LOCK_ACTV_RG.COM - DCL to generate a report on lock rates later from
		           data collected earlier by LOCK_ACTV_DC.COM
	LCKACT.COM - DCL to run proper version of LCKACT based on VMS version
	LCKACT.MAR - Macro-32 source code for 7.1[-x] and 7.2[-x]
	BUILD_LCKACT.COM - DCL to compile/link LCKACT.MAR

Detect and display lock queues:
	LCKQUE.COM - DCL to run proper version of LCKQUE based on VMS version,
		     and to display lock queues
	LCKQUE.MAR - Macro-32 source code for 7.1[-x] and 7.2[-x]
	BUILD_LCKQUE.COM - DCL to compile/link LCKQUE.MAR

DCL procedures to help determine if lock remastering activity is taking place.
	RBLD.COM - Display remastering-message counts using SDA
	WATCH_RBLD.COM - Using RBLD.COM, monitor counts and note activity
	  (Note that with VMS 7.2 or later, this info is available from
	   SDA> SHOW LOCK/SUMMARY under the "Lock Remaster Counters:" heading)

Determine which node is the lock master for the RMS global buffer lock tree
for a file:
	FINDMASTER_FILE.COM - DCL: given filespec, determine lock master node
	FINDMASTER.MAR - Macro-32 source code
	BUILD_FINDMASTER.COM - Compile/link FINDMASTER.MAR

Generate lots of locks to convince VMS to remaster an RMS global buffer lock
tree: 
	LOTSALOX.MAR - Macro to generate lots of lock requests
	MOVE_MASTER.COM, FORCE_MASTER_FILE.COM - DCL to use LOTSALOX.MAR to
			try to move lock tree to this node
	(REMASTER.MAR is still undergoing testing and is not on this CD
         -- see below for where to find it when it's ready)
------------------------------------------------------------------------------
Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
Feel free to direct questions or comments to me, or send me improved versions.

Look for latest updated versions or fixes at:
  http://encompasserve.org/~parris/ or http://www.geocities.com/keithparris/ 

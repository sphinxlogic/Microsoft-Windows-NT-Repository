KP_LOCKTOOLS, UTILITIES, Tools for monitoring and controlling the VMS Lock Manager

(For DECUS presentations describing the theory behind these tools and how
they operate, see the websites http://www2.openvms.org/kparris/ and
http://www.geocities.com/keithparris/, looking for the latest incarnation
of the presentation which has been entitled at various times things like
'Monitoring and Controlling the VMS Lock Manager or or 'OpenVMS Distributed
Lock Manager Performance'.  As of this writing, the latest version is at: 
www.geocities.com/keithparris/decus_presentations/s2002_dist_lock_mgr_perf.ppt)

* indicates a new tool for the V6 release.  Most of the existing tools have
  been significantly improved for the V6 Freeware CD.

Determine which lock trees are the busiest, cluster-wide:
	LOCK_ACTV.COM - DCL to collect and display lock-rate data immediately
	LOCK_ACTV_DC.COM - DCL to collect lock-rate data only (no analysis)
			   for lower system overhead at time of data collection
	LOCK_ACTV_RG.COM - DCL to generate a report on lock rates later from
		           data collected earlier by LOCK_ACTV_DC.COM
	LCKACT_RUN.COM - DCL to run proper version of LCKACT based on VMS version
	LCKACT.MAR - Macro-32 source code
	BUILD_LCKACT.COM - DCL to compile/link LCKACT.MAR
		Note: SYSGEN parameter PE1 must be either left at its default
		value of 0 or set to a positive value for the counters that
		LCKACT uses to be updated by VMS.  If you have set PE1 to -1
		to disable lock remastering entirely and wish to use this
		tool, I suggest using a value of positive 1 instead.  PE1
		is a dynamic SYSGEN parameter, so no reboot is needed to change
		it.

Detect and display lock queues:
	LCKQUE.COM - Display lock queues.  Now incorporates source code for
		     LCKACT.MAR.  Call with "BUILD" as P1 parameter to compile/
		     link the Macro program.  This replaces the formerly-
		     separate files LCKQUE.COM, LCKQUE.MAR, LCKQUE_RUN.COM, and
		     BUILD_LCKQUE.COM.

	Information on the updates to LCKACT and LCKQUE may be found in the
	file LCKACT_LCKQUE_README.TXT

Measure lock-request latencies across all nodes in a cluster:
  *	LOCKTIME.COM -  DCL to measure lock-request round-trip latencies in
			a cluster.
			Call with BUILD as P1 parameter to compile/link.  Uses
			Roy Davis's LOCKTIME_* program pair.  Generates
			.CSV file with matrix of lock latencies for all
			node-to-node combinations in the cluster, suitable for
			importing into a Microsoft Excel spreadsheet.
			More information is in the file LOCKTIME_README.TXT.

DCL procedures to help determine if lock remastering activity is taking place.
	RBLD.COM - Display remastering-message counts using SDA
	WATCH_RBLD.COM - Using RBLD.COM, monitor counts and note activity
	  (Note that with VMS 7.2 or later, this info is available from
	   SDA> SHOW LOCK/SUMMARY under the "Lock Remaster Counters:" heading,
           and from 7.3 or later with $MONITOR RLOCK)

Determine which node is the lock master for the RMS global buffer lock tree
for a file:
	FINDMASTER_FILE.COM - DCL: given filespec for an RMS file, determine
        lock master node
	FINDMASTER.MAR - Macro-32 source code
	BUILD_FINDMASTER.COM - Compile/link FINDMASTER.MAR

Generate lots of locks to convince VMS to remaster an RMS global buffer lock
tree:
	LOTSALOX.MAR - Macro to generate lots of lock requests
	MOVE_MASTER.COM, FORCE_MASTER_FILE.COM - DCL to use LOTSALOX.MAR to
			try to move lock tree to this node

Map from File Number and device name to file specification:
 *	FILE_ID_TO_NAME.COM - DCL to look up file spec. from file number/disk

The following programs are available upon request, but are not on this CD
-- see below for where to request them.

REMASTER.MAR, a program to trigger lock tree remastering by modifying contents
	of VMS data structures

LCMRM.MAR, a program to report cases where there is pent-up remastering demand,
	that is, VMS would like to remaster a lock tree, but is prevented from
	doing so by a non-zero setting for the SYSGEN parameter PE1.
------------------------------------------------------------------------------
Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
                      or keith.parris@hp.com

I welcome questions, comments, corrections, suggestions, etc. via e-mail.

Because the VMS Freeware CD is updated only periodically, it is advisable to
check for later versions of any tools included here.

Look for latest updated versions or fixes at:
  http://encompasserve.org/~parris/

DECUS presentations describing some of these tools may be found at:
  http://www2.openvms.org/parris/ or http://www.geocities.com/keithparris/ 

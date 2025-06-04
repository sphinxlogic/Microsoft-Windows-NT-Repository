$ ! ****************************************************************************
$ ! *	Copyright (c) 1994 - 2000, Ergonomic Solutions                         *
$ ! *   1-800-ERGO-SOL   www.ergosol.com   info@ergosol.com                    *
$ ! *                                                                          *
$ ! *	R a m P a g e   W a t c h d o g   D i s p a t c h e r	               *
$ ! *                                                                          *
$ ! *   THIS SOFTWARE MAY BE *** FREELY DISTRIBUTED *** AND COPIED PROVIDED    *
$ ! *   NO CHARGE IS MADE FOR USE OR FOR SUCH DISTRIBUTION, AND THIS           *
$ ! *   COPYRIGHT NOTICE IS RETAINED.  MODIFIED FORMS ARE PERMITTED, HOWEVER   *
$ ! *   THE COPYRIGHT NOTICE MUST BE RETAINED, AND THE CHANGES MUST BE FULLY   *
$ ! *   DOCUMENTED BY THE AUTHOR IN THE MODIFICATION HISTORY SECTION BELOW.    *
$ ! *   NO TITLE TO AND OWNERSHIP OF THE SOFTWARE ARE HEREBY TRANSFERRED.      *
$ ! *							                       *
$ ! *   THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT NOTICE   *
$ ! *   AND SHOULD NOT BE CONSTRUED AS A COMMITMENT.  NO RESPONSIBILITY IS     *
$ ! *   ASSUMED FOR THE USE OR RELIABILITY OF THIS SOFTWARE.                   *
$ ! *								               *
$ ! ****************************************************************************
$ !
$ !	Instructions:
$ !
$ !	The RamPage Watchdog Dispatcher performs selected functions at a
$ !     repeated interval to monitor systems activities requiring the attention
$ !	of system managers.  Problem notifications may be delivered via MAIL,
$ !	REPLY, OPCOM, and/or personal pagers.  (NOTE: The RamPage paging
$ !	software is not required for MAIL, REPLY, and OPCOM deliveries.)
$ !
$ !	The following table describes the available functions:
$ !
$ !     -----------------------------------------------------------------------
$ !	Function    Parameter(s)	Description
$ !     -----------------------------------------------------------------------
$ !
$ !	INTERVAL    period [refresh]	Sets the interval period between 
$ !					successive problem checks.  The period
$ !					must be specified as hours, minutes,
$ !					and seconds in the format HH:MM:SS. The
$ !					default interval period is 10 minutes.
$ !
$ !					An optional refresh period may also be
$ !				        specified in the format HH:MM:SS.  This
$ !					value determines how frequently the
$ !					countdown status is refreshed for
$ !				        viewing by RamPage Watchdog Monitor
$ !					utility.  The default refresh period is
$ !					5 seconds.  New values must not be
$ !					greater than the interval period, and
$ !					should be evenly divisible by it.
$ !
$ !	NOTIFY	    username_list delivery
$ !
$ !					Sets the usernames in the given list as
$ !					the receivers of problem reports, with
$ !					delivery method as RAMPAGE, MAIL,
$ !					REPLY, and/or OPCOM.  (NOTE: RAMPAGE
$ !					requires Ergonomic Solutions' RamPage
$ !					paging software).
$ !
$ !	SYSBOOT				Checks and reports the system boot time
$ !					upon startup of the watchdog.  Useful
$ !                                     in detecting unexpected reboots.
$ !
$ !	DISKS	    fill_percentage [device]
$ !
$ !                                     Checks the fill percentage of mounted
$ !					disk drives.  Any which exceed the 
$ !					given value are reported.  A full or
$ !                                     partial device name may be provided to
$ !					override the default of checking all
$ !					disks.  If a partial device name is
$ !					given, all devices whose device names
$ !					begin with what you entered.
$ !	
$ !	NODES 	    node_list [method]	Checks all nodes in the given list and
$ !					reports any that aren't reachable.  If
$ !					a method is provided, it indicates how
$ !					the node(s) are queried for detection
$ !					using the keywords DECNET (default),
$ !					CLUSTER, or NCP.
$ !
$ !	INTRUSIONS  type		Checks the intrusion database and 
$ !					reports on any new entries.  You must 
$ !					specify an intrusion type of ALL, 
$ !					SUSPECT, or INTRUDER.
$ !	
$ !	DEVICE 	    (none)		Checks all known devices and reports
$ !					changes in error counts.
$ !
$ !	LICENSE	    (none)		Checks the license database and reports
$ !					any products due to expire.
$ !
$ !	USERNAME    username_list	Checks all usernames in the given list
$ !					and reports any that are currently on
$ !					the system.
$ !
$ !	PROCESS	    process_list action Checks all processes in the given list
$ !					for actions REPORT_MISSING or
$ !					REPORT_PRESENT.
$ !
$ !	QUEUE	    status queue 	Checks the given queue and reports if 
$ !					the status becomes one of MISSING,
$ !					STOPPED, STALLED, RESETTING, CLOSED, 
$ !					PAUSED, STARTING, STOPPING, or IDLE.
$ !
$ !	PING	    address_list	Checks all remote TCP/IP addresses in 
$ !					the given list and reports any that 
$ !					fail to respond.
$ !
$ !     -----------------------------------------------------------------------
$ !
$ !	Examples Applications:
$ !
$ !	--- Example 1 ---
$ !
$ !	The watchdog enabled by the following functions sets the interval to
$ !	five minutes, and username FOX as the recipient of problem notices
$ !	delivered via OpenVMS mail.  The only function performed is a check for
$ !	disks which exceed 80% of their capacity.
$ !	
$ !		PERFORM INTERVAL 	00:05:00
$ !		PERFORM NOTIFY		"FOX" "MAIL"
$ !		PERFORM	DISKS		80
$ !
$ !	--- Example 2 ---
$ !
$ !	The watchdog enabled by the following functions sets the interval to
$ !	ten minutes, and usernames TYLER and GARRISON as the recipients of
$ !	problem notices delivered via MAIL and REPLY.  Functions checked
$ !	include licensing, inactive nodes, and device errors
$ !
$ !		PERFORM INTERVAL	00:10:00
$ !		PERFORM NOTIFY		"TYLER,GARRISON" "MAIL,REPLY"
$ !		PERFORM LICENSE
$ !		PERFORM NODES		"STAR,SATURN,PISCES,VENUS"
$ !		PERFORM NODES		"JUPITER,MARS,PLUTO,MERCURY"
$ !		PERFORM NODES		"URANUS,EARTH"
$ !		PERFORM DEVICE	
$ !
$ !	--- Example 3 ---
$ !
$ !	The watchdog enabled by the following functions demonstrate how to
$ !	monitor several functions, with changing recipients and delivery
$ !	methods.
$ !
$ !		PERFORM INTERVAL	00:10:00
$ !		
$ !		PERFORM NOTIFY		"WALKER" "MAIL"
$ !		PERFORM	QUEUE		"STOPPED" "INVOICE_PRINTER"
$ !		PERFORM	QUEUE		"STOPPED" "CHECK_PRINTER"
$ !		PERFORM QUEUE		"STOPPED" "PO_PRINTER"
$ !		PERFORM PROCESS		"Payroll_Update" "REPORT_PRESENT"
$ !		
$ !		PERFORM NOTIFY		"GARRISON" "RAMPAGE"
$ !		PERFORM SYSBOOT
$ !		
$ !		PERFORM NOTIFY		"GARRISON" "RAMPAGE,OPCOM"
$ !		PERFORM INTRUSIONS	"ALL"
$ !		PERFORM DEVICE
$ !		PERFORM DISKS		95
$ !		PERFORM USERNAME	"FIELD,SYSTEM"
$ !		PERFORM PROCESS		"OPCOM" "REPORT_MISSING"
$ !		
$ !		PERFORM NOTIFY		"GARRISON" "MAIL,REPLY"
$ !		PERFORM DISKS		75
$ !		PERFORM LICENSE
$ !		PERFORM	NODES		"MICKY,MINNIE,PLUTO,DAFFY"
$ !
$ !
$ !	Application Notes
$ !
$ !     1. To setup the watchdog, specify PERFORM functions, as shown in
$ !	   the examples above, into the section below delimited by a
$ !	   "- Start Marker -" and "- End Marker -".
$ !
$ !	2. To start a watchdog process, execute a command similar to the
$ !	   following at the DCL level, or somewhere within the system startup
$ !	   procedures:
$ !
$ !		$ RUN/DETACHED/UIC=[1,4]				-
$ !			/INPUT=device:[directory]WATCHDOG.COM		-
$ !			/OUTPUT=NL:					-
$ !			/ERROR=device:[directory]WATCHDOG.ERR		-
$ !			/PROCESS_NAME="Watchdog"			-
$ !		        SYS$SYSTEM:LOGINOUT.EXE
$ !
$ !	3. The interval period can't vary between functions, since the interval
$ !	   period pause occurs once at the end of the function list.  However,
$ !	   to perform functions at different interval periods, simply run
$ !	   multiple watchdog processes with separate function lists and
$ !	   unique process names.
$ !
$ !	4. The watchdog owner process must have the following privileges to
$ !	   perform all functions: SECURITY, CMKRNL (intrusions), SYSPRV, OPER
$ !	   (queue queries).
$ !
$ !	5. The RamPage paging software is not required unless you specify
$ !	   RAMPAGE as the delivery method for the NOTIFY function.  For
$ !        additional information on RamPage, contact Ergonomic Solutions:
$ !
$ !		Telephone:  1-800-ERGO-SOL
$ !		   E-Mail:  info@ergosol.com
$ !		      Web:  http://www.ergosol.com
$ !
$ !
$ ! ****************************************************************************
$ ! *                                                                          *
$ ! *	MODIFICATION HISTORY:					               *
$ ! *                                                                          *
$ ! *	  Vers/Author  Date	  Description		                       *
$ ! *								               *
$ ! *  1. V1.1-06 CSL 21-Jul-1995 The INTRUSIONS category was updated due to   *
$ ! *                             parsing errors which occurred in OpenVMS     *
$ ! *                             V5.5 or higher because of changes in spacing *
$ ! *                             in the output of the SHOW INTRUSIONS command.*
$ ! *								               *
$ ! *  2. V1.1-07 CSL 12-Dec-1995 The NODES category was modified to use a     *
$ ! *                             SHOW USER/NODE instead of NCP TELL as the    *
$ ! *                             basis for determining whether a node is      *
$ ! *                             present.  The former method had an undesired *
$ ! *                             side effect of generating long NETSERVER.LOG *
$ ! *                             files.                                       *
$ ! *								               *
$ ! *  3. V1.1-12 CSL 30-Jul-1996 The DISKS category has been significantly    *
$ ! *				  enhanced to properly recognize and report on *
$ ! *				  disk volume sets.  Previously, each disk in  *
$ ! *				  a volume set was incorrectly reported on     *
$ ! *				  individually.			               *
$ ! *								               *
$ ! *  4. V1.1-12 CSL 30-Jul-1996 Previously, the NOTIFY category did not      *
$ ! *				  recognize lowercase delivery options.  Also, *
$ ! *			          the documented examples for the INTERVAL     *
$ ! *				  category implied a value of 0-60 could be    *
$ ! *				  given as a number of seconds, when actually  *
$ ! *                             the required format was HH:MM:SS.  Both      *
$ ! *				  problems have been corrected.                *
$ ! *								               *
$ ! *  5. V1.1-14 CSL 04-Mar-1997 Previously, the documented description and   *
$ ! *				  examples for the DEVICE category implied the *
$ ! *				  keyword DEVICES was an acceptable synonym.   *
$ ! *				  However, any usage of DEVICES caused Watchdog*
$ ! *				  to abort unexpected.  The documentation and  *
$ ! *                             examples have now been updated, and the      *
$ ! *				  Watchdog has been improved to recognize any  *
$ ! *				  misspelled or invalid function categories.   *
$ ! *								               *
$ ! *  6. V1.1-14 CSL 04-Mar-1997 Several problems with the PROCESS category   *
$ ! *				  have been corrected.  First, the action of   *
$ ! *				  REPORT_PRESENT for reporting on existing     *
$ ! *				  processes was not completely implemented.    *
$ ! *				  Next, processes outside the group [1,*]      *
$ ! *				  were not properly categorized due to a       *
$ ! *				  reporting restriction in the OpenVMS SHOW    *
$ ! *				  PROCESS command.  This has been changed by   *
$ ! *				  examining the results of a SHOW SYSTEM       *
$ ! *				  instead.  			               *
$ ! *								               *
$ ! *  7. V1.1-14 CSL 04-Mar-1997 Correction #2 changed the NODES category     *
$ ! *                             detection method for remote DECnet nodes,    *
$ ! *                             however, the change created a new problem by *
$ ! *                             only recognizing OpenVMS systems.  Routers,  *
$ ! *                             servers, and other operating system nodes    *
$ ! *                             are now once again properly recognized by    *
$ ! *                             examining the results of a SHOW NETWORK.     *
$ ! *				  This also means a node can now be recognized *
$ ! *				  by its area.node number (as in 1.5), but one *
$ ! *				  added restriction is that only OpenVMS       *
$ ! *				  routing nodes can use the NODES category for *
$ ! *				  accurate reporting.          	               *
$ ! *								               *
$ ! *  8. V1.2-01 CSL 02-May-1997 The PING category has been enhanced to auto- *
$ ! *				  matically sense the TCP/IP package in use,   *
$ ! *				  and execute the associated ping utility that *
$ ! *				  is provided with that software.              *
$ ! *								               *
$ ! *  9. V1.2-01 CSL 11-Jun-1997 Extensive coding has been added to feed data *
$ ! *                             to the new RamPage Watchdog Monitor utility. *
$ ! *                             This utility provides real-time status       *
$ ! *                             monitoring of the state of the Watchdog      *
$ ! *                             processing.                                  *
$ ! *								               *
$ ! * 10. V1.2-02 CSL 16-Jan-1998 The DISKS category has been enhanced with a  *
$ ! *                             new parameter for designating a device name  *
$ ! *                             to override the default of checking all      *
$ ! *				  disks.  Partial devices names may be given.  *
$ ! *                                                                          *
$ ! * 11. V1.2-02 CSL 25-Mar-1998 The DISKS category was enhanced in change #3 *
$ ! *				  to recognize and report on disk volume sets. *
$ ! *				  However, the 'fill_percentage' parameter was *
$ ! *				  not being passed to the routine performing   *
$ ! *				  the volume set total comparison.  The result *
$ ! *				  was a capacity warning regardless of the     *
$ ! *				  actual fill percentage.  This has been       *
$ ! *				  corrected.			               *
$ ! *								               *
$ ! * 12. V1.2-02 CSL 10-Apr-1998 The DISKS category computation for the disk  *
$ ! *				  fill percentage can fail for large disks due *
$ ! *				  to DCL integer overflow errors.  When this   *
$ ! *				  occurs, the percentage number can exceed     *
$ ! *				  100%.  To correct this without affecting the *
$ ! *				  fill percentage, the number of free and      *
$ ! *				  maximum blocks is now divided by 1000 before *
$ ! *				  performing the arithmetic.  Also, the disk   *
$ ! *				  name now compresses embedded spaces to one   *
$ ! *				  space to permit the proper display of the    *
$ ! *				  disk controller (such as "DKA0:  (HSC001)")  *
$ ! *                             (if present) without showing the unnecessary *
$ ! *				  whitespace.	      	 		       *
$ ! *								               *
$ ! * 13. V2.0-01 CSL 25-Aug-1999 Changes #2 and #7 provided alternative ways  *
$ ! *				  of detecting lost systems in the NODES       *
$ ! *				  category, however, no single method seems to *
$ ! *				  fit all customer environments.  Now, the     *
$ ! *				  NODES category has been enhanced with a new  *
$ ! *				  parameter to specify which detection method  *
$ ! *				  should be used as either DECNET (default),   *
$ ! *				  CLUSTER, or NCP.		       	       *
$ ! *								               *
$ ! * 14. V2.0-02 CSL 09-Jun-2000 An optional refresh period may now be given  *
$ ! *				  with the INTERVAL category to override the   *
$ ! *				  default value of 5 seconds between the       *
$ ! *				  countdown status updates displayed by the    *
$ ! *				  RamPage Watchdog Monitor utility.  Although  *
$ ! *				  the system overhead incurred using the       *
$ ! *				  default value is very small, increasing      *
$ ! *				  this value may significantly reduce the      *
$ ! *				  cumulative CPU and I/O counts over long      *
$ ! *				  periods of system uptime.                    *
$ ! *								               *
$ ! * 15. V2.0-02 CSL 12-Jun-2000 The DEVICE category would not properly parse *
$ ! *				  a device if it's name began with a dollar    *
$ ! *				  sign (as in class names) or if it referenced *
$ ! *				  controller name(s).  For example, errors for *
$ ! *				  a device identified in the SHOW ERROR output *
$ ! *				  as "$30$MUA172: (CTDC1B,CTDC1A)" would not   *
$ ! *				  be reported.  A portion of this correction   *
$ ! *				  was included in change #14, but was not      *
$ ! *				  documented.  However, that correction did    *
$ ! *				  not take into account a device with multiple *
$ ! *				  controllers.  All cases now work correctly.  *
$ ! *				  Also, the problem message now includes the   *
$ ! *				  change in error counts.  For example, the    *
$ ! *				  old style message "Device MUA0 has logged 72 *
$ ! *				  errors." is now reported as "Device MUA0 has *
$ ! *				  logged 3 new errors, 72 total."              *
$ ! *								               *
$ ! * 16. V2.0-02 CSL 03-Oct-2000 The PING category has been updated for UCX   *
$ ! *				  V5.0 and higher, which unlike prior versions *
$ ! *				  now sends an continuous stream of PING data  *
$ ! *				  packets.  This is now handled by using the   *
$ ! *				  UCX PING qualifier /NUMBER=1 to stop after   *
$ ! *				  one packet.  Also, the success status code   *
$ ! *				  is different after V5.0.	               *
$ ! *								               *
$ ! ****************************************************************************
$ !	
$ CALL Setup
$Watchdog_Loop:
$ !---------------------------------------------------------------------------
$ ! (insert desired PERFORM's here)
$ ! - Start Marker -   Place your PERFORMS between these Start and End markers
$ ! - End Marker -     Place your PERFORMS between these Start and End markers
$ !---------------------------------------------------------------------------
$ !
$ ! Wait for designated interval period, with 5 second status countdowns
$ !
$ CURTIME = F$CVTIME("")
$ DEFTIME = F$CVTIME("+0 ''INTERVAL_PERIOD'")
$Interval_Loop:
$ CALL Update_Status "Curr: ''CURTIME', Next: ''DEFTIME'"
$ WAIT 'REFRESH_PERIOD
$ CURTIME = F$CVTIME("")
$ IF CURTIME .LTS. DEFTIME THEN GOTO Interval_Loop
$ !
$ GOTO Watchdog_Loop
$ exit
$
$ !***************************************************************************
$ !
$ !	WATCHDOG:	S y s b o o t
$ !
$ !***************************************************************************
$SYSBOOT:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "SYSBOOT     Checking for recent system reboot..."
$ !
$ IF F$TYPE(BOOTTIME) .NES. "STRING"
$ 	THEN
$	    BOOTTIME == F$GETSYI("BOOTTIME")
$	    CALL Log_Problem "System booted at ''BOOTTIME'."
$ ENDIF
$ !
$ ENDSUBROUTINE
$ !
$ !***************************************************************************
$ !
$ !	WATCHDOG:	D i s k s
$ !
$ !***************************************************************************
$Disks:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "DISKS       Checking disks for ''P1'% capacity..."
$ !
$ ! First, generate a list of all mounted disks
$ !
$ ASSIGN SYS$TEST:WATCHDOG.TMP SYS$OUTPUT
$ SHOW DEVICE/MOUNTED 'P2
$ DEASSIGN SYS$OUTPUT
$ !
$ ! Open the list and check each entry
$ !
$ OPEN/READ/ERROR=Disk_Exit DISK SYS$TEST:WATCHDOG.TMP
$ !
$Disk_ReadLoop:
$ !
$ READ/END_OF_FILE=DISK_EXIT DISK LINE
$ !
$ ! Parse only mounted device entries
$ !
$ IF (F$EXTRACT(24,7,LINE) .EQS. "Mounted")
$   THEN
$     	DISK_NAME = F$EDIT(F$EXTRACT(0,24,LINE),"TRIM,COMPRESS")
$     	DISK_MEMBER	= F$GETDVI(DISK_NAME,"VOLSETMEM")
$	!
$	! Check non-volume-set disk-class devices that are writeable
$	!
$	IF (F$GETDVI(DISK_NAME,"DEVCLASS") .EQ. 1)	.AND.	-
      	    (F$GETDVI(DISK_NAME,"SWL") .EQS. "FALSE")	.AND.	-
	    (DISK_MEMBER .EQS. "FALSE")
$	  THEN
$     	    !	
$     	    ! If we get here, we have a disk to check for a capacity warning
$     	    !	
$     	    DISK_MAXBLOCKS	= (F$GETDVI(DISK_NAME,"MAXBLOCK")/1000)
$     	    DISK_FREEBLOCKS	= (F$GETDVI(DISK_NAME,"FREEBLOCKS")/1000)
$     	    DISK_PERCENTFULL	= (100-(100*DISK_FREEBLOCKS/DISK_MAXBLOCKS))
$	    IF DISK_PERCENTFULL .GE. P1 THEN CALL Log_Problem -
	    "WARNING: Disk ''DISK_NAME' is at ''DISK_PERCENTFULL'% of capacity."
$         ENDIF
$	!
$	! Check volume set members
$	!
$	IF (DISK_MEMBER .EQS. "TRUE") .AND. -
	   (F$GETDVI(DISK_NAME,"VOLNUMBER")) .EQ. 1
$	  THEN
$	      CALL Disks_VS 'P1
$	ENDIF
$	!
$ ENDIF
$ GOTO Disk_Readloop
$ !
$Disk_Exit:
$ CLOSE DISK
$ DELETE/NOLOG SYS$TEST:WATCHDOG.TMP;*
$ ENDSUBROUTINE
$ !
$Disks_VS:
$ SUBROUTINE
$ SET NOON
$ !
$ ! First, generate a list of all mounted disks in the current volume set
$ !
$ ASSIGN SYS$TEST:WATCHDOG.TMP2 SYS$OUTPUT
$ SHOW DEVICE 'DISK_NAME
$ DEASSIGN SYS$OUTPUT
$ !
$ ! Open the list and check each entry
$ !
$ OPEN/READ/ERROR=VS_DONE DISKVS SYS$TEST:WATCHDOG.TMP2
$ !
$ VS_MAXMBLOCKS = 0
$ VS_FREEBLOCKS = 0
$ !
$VS_ReadLoop:
$ !
$ READ/END_OF_FILE=VS_DONE DISKVS LINEVS
$ !
$ ! Parse only mounted device entries
$ !
$ IF (F$EXTRACT(24,7,LINEVS) .EQS. "Mounted")
$   THEN
$     	VS_NAME = F$EDIT(F$EXTRACT(0,24,LINEVS),"TRIM,COMPRESS")
$     	VS_MAXMBLOCKS	= VS_MAXMBLOCKS + (F$GETDVI(VS_NAME,"MAXBLOCK")/1000)
$     	VS_FREEBLOCKS	= VS_FREEBLOCKS + (F$GETDVI(VS_NAME,"FREEBLOCKS")/1000)
$ ENDIF
$ GOTO VS_Readloop
$ !
$VS_Done:
$ !
$ VS_PERCENTFULL	= (100-(100*VS_FREEBLOCKS/VS_MAXMBLOCKS))
$ IF VS_PERCENTFULL .GE. P1 THEN CALL Log_Problem -
   "WARNING: Disk Volume Set ''DISK_NAME' is at ''VS_PERCENTFULL'% of capacity."
$ !
$VS_Exit:
$ !
$ CLOSE DISKVS
$ DELETE/NOLOG SYS$TEST:WATCHDOG.TMP2;*
$ ENDSUBROUTINE
$ !
$ !***************************************************************************
$ !
$ !	WATCHDOG:	N o d e s
$ !
$ !***************************************************************************
$ !
$ ! Detect lost nodes using one of the following methods:
$ !
$ ! DECNET (default)		Detects lost nodes by examining the results
$ !				generated by a $ SHOW NETWORK/OUTPUT command.
$ !				This method works best for detecting DECnet
$ !				systems in a mixed operating system
$ !				environment, as well as other routers and
$ !				servers, however, only OpenVMS routing nodes
$ !				can use this method.
$ !
$ ! CLUSTER			Detects lost nodes by examining the success or
$ !				failure status returned by performing a
$ !				$ SHOW USER/NODE command for a non-existent
$ !				user.  This is the most effective method for
$ !				OpenVMS systems operating in the same cluster
$ !				as the watchdog process.
$ !
$ ! NCP				Detects lost nodes by examining the results
$ !				generated by a $ NCP TELL command.  This is the
$ !				most accurate method for detecting nodes known
$ !				to the NCP database, however, is has the
$ !				undesirable side effect of generating long
$ !				NETSERVER.LOG files.
$ !
$Nodes:
$ SUBROUTINE
$ !
$ IF ("''P2'" .EQS. "CLUSTER")
$     THEN
$	  CALL NodesC 'P1
$	  GOTO Nodes_Exit
$ ENDIF
$ !
$ IF ("''P2'" .EQS. "NCP")
$     THEN
$	  CALL NodesN 'P1
$	  GOTO Nodes_Exit
$ ENDIF
$ !
$ CALL NodesD 'P1
$ !
$Nodes_Exit:
$ ENDSUBROUTINE
$ !
$ !***************************************************************************
$ !
$ !	WATCHDOG:	N o d e s 	(DECnet method)
$ !
$ !***************************************************************************
$ !
$NodesD:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "NODES       Checking for unreachable DECnet nodes..."
$ !
$ ! Setup to loop through all given nodes
$ !
$ NODE_PTR = 0
$ !
$ ! Query the node(s) for connection failures.  If we are not running V5.x or
$ ! V6.x of OpenVMS, then we must be running V7.0 or higher, in which case we
$ ! need to use the /OLD qualifier in order to get a node list from SHOW NET.
$ !
$ OLD = "/OLD"
$ IF (F$EXTRACT(0,2,"''F$GETSYI("VERSION")'") .EQS. "V5") THEN OLD = ""
$ IF (F$EXTRACT(0,2,"''F$GETSYI("VERSION")'") .EQS. "V6") THEN OLD = ""
$ !
$ SHOW NETWORK'OLD'/OUTPUT=SYS$TEST:WATCHDOG.TMP
$ !
$ ! Check to be sure this is a routing node that can detect connection failures
$ !
$ ASSIGN/NOLOG NL: SYS$OUTPUT
$ ASSIGN/NOLOG NL: SYS$ERROR
$ SEARCH SYS$TEST:WATCHDOG.TMP "nonrouting"
$ IF $STATUS .EQ. "%X0000001"
$     THEN
$         DEASSIGN SYS$OUTPUT
$         DEASSIGN SYS$ERROR
$	  IF F$TYPE(NONROUTING) .EQS. ""
$	      THEN
$		  NONROUTING == 1
$		  CALL Log_Problem					-
			"Nonrouting node cannot detect connection failures"
$	  ENDIF
$	  GOTO NodeD_Exit
$     ELSE
$         DEASSIGN SYS$OUTPUT
$         DEASSIGN SYS$ERROR
$ ENDIF
$ IF F$TYPE(NONROUTING) .NES. "" THEN GOTO NodeD_Exit
$ !
$NodeD_Loop:
$ !
$ NODE_NODE = F$ELEMENT(NODE_PTR,",",P1)
$ IF NODE_NODE .EQS. "," THEN GOTO NodeD_Exit
$ !
$ ! Check the given node's connection
$ !
$ ASSIGN/NOLOG NL: SYS$OUTPUT
$ ASSIGN/NOLOG NL: SYS$ERROR
$ SEARCH SYS$TEST:WATCHDOG.TMP "''NODE_NODE'"
$ IF $STATUS .NE. "%X0000001"
$     THEN
$         DEASSIGN SYS$OUTPUT
$         DEASSIGN SYS$ERROR
$	  CALL Log_Problem "Lost DECnet connection to node ''NODE_NODE'"
$     ELSE
$         DEASSIGN SYS$OUTPUT
$         DEASSIGN SYS$ERROR
$ ENDIF
$ !
$ NODE_PTR = NODE_PTR + 1
$ GOTO NodeD_Loop
$ !
$NodeD_Exit:
$ DELETE/NOLOG/NOCONFIRM SYS$TEST:WATCHDOG.TMP;
$ !
$ ENDSUBROUTINE
$ !
$ !***************************************************************************
$ !
$ !	WATCHDOG:	N o d e s 	(Cluster method)
$ !
$ !***************************************************************************
$ !
$NodesC:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "NODES       Checking for unreachable cluster nodes..."
$ !
$ ! Setup to loop through all given nodes
$ !
$ NODE_PTR = 0
$ !
$NodeC_Loop:
$ !
$ NODE_NODE = F$ELEMENT(NODE_PTR,",",P1)
$ IF NODE_NODE .EQS. "," THEN GOTO NodeC_Exit
$ !
$ ! Check each one...
$ !
$ ASSIGN/NOLOG SYS$TEST:WATCHDOG.TMP SYS$OUTPUT
$ ASSIGN/NOLOG NL: SYS$ERROR
$ SHOW USER XXXYYYZZZ/NODE='NODE_NODE'
$ DEASSIGN SYS$OUTPUT
$ DEASSIGN SYS$ERROR
$ ASSIGN/NOLOG NL: SYS$OUTPUT
$ ASSIGN/NOLOG NL: SYS$ERROR
$ SEARCH SYS$TEST:WATCHDOG.TMP "NOSUCHUS"
$ IF $STATUS .NE. "%X0000001"
$     THEN
$	  DEASSIGN SYS$OUTPUT
$	  DEASSIGN SYS$ERROR
$	  CALL Log_Problem "Lost cluster connection to node ''NODE_NODE'"
$     ELSE
$	  DEASSIGN SYS$OUTPUT
$	  DEASSIGN SYS$ERROR
$ ENDIF
$ DELETE/NOLOG/NOCONFIRM SYS$TEST:WATCHDOG.TMP;
$ !
$ NODE_PTR = NODE_PTR + 1
$ GOTO NodeC_Loop
$ !
$NodeC_Exit:
$ !
$ ENDSUBROUTINE
$ !
$ !***************************************************************************
$ !
$ !	WATCHDOG:	N o d e s 	(NCP method)
$ !
$ !***************************************************************************
$ !
$NodesN:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "NODES       Checking for unreachable NCP nodes..."
$ !
$ ! Setup to loop through all given nodes
$ !
$ NODE_PTR = 0
$ NCP :== $SYS$SYSTEM:NCP
$ !
$NodeN_Loop:
$ !
$ NODE_NODE = F$ELEMENT(NODE_PTR,",",P1)
$ IF NODE_NODE .EQS. "," THEN GOTO NodeN_Exit
$ !
$ ! Check each one...
$ !
$ ASSIGN/USER NL: SYS$ERROR
$ ASSIGN/USER NL: SYS$OUTPUT
$ NCP TELL 'NODE_NODE' SHOW NODE 'NODE_NODE' STATUS
$ IF $STATUS .NE. %X10000001 THEN CALL Log_Problem -
	"Lost NCP connection to node ''NODE_NODE'"
$ !
$ NODE_PTR = NODE_PTR + 1
$ GOTO NodeN_Loop
$ !
$NodeN_Exit:
$ !
$ ENDSUBROUTINE
$ !
$ !***************************************************************************
$ !
$ !	WATCHDOG:	I n t r u s i o n s
$ !
$ !***************************************************************************
$Intrusions:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "INTRUSIONS  Checking for intrusions type ''P1'..."
$ !
$ CREATE SYS$TEST:WATCHDOG.TMP
$ ASSIGN/USER NL: SYS$ERROR
$ ASSIGN/USER NL: SYS$OUTPUT
$ SHOW INTRUSION/TYPE='P1'/OUTPUT=SYS$TEST:WATCHDOG.TMP
$ !
$ OPEN/READ/ERROR=Intrusion_End INTR SYS$TEST:WATCHDOG.TMP
$ !
$Intrusion_ReadLoop:
$ !
$ READ/END_OF_FILE=Intrusion_End INTR LINE

$!   NETWORK      SUSPECT       8   15:26:18.37  STAR::LOMBARDI
$!1234567890123456789012345678901234567890123456789012345678901234567890
$!         1         2         3         4         5

$ INT_INT 	= F$EDIT(F$EXTRACT( 0,13,LINE),"TRIM,UPCASE")
$ IF INT_INT .EQS. "INTRUSION" THEN GOTO Intrusion_ReadLoop
$ INT_TYP 	= F$EDIT(F$EXTRACT(14,12,LINE),"TRIM,UPCASE")
$ INT_CNT 	= F$EDIT(F$EXTRACT(27, 6,LINE),"TRIM,UPCASE")
$ INT_EXP 	= F$EDIT(F$EXTRACT(33,12,LINE),"TRIM,UPCASE")
$ INT_SRC 	= F$EDIT(F$EXTRACT(46,30,LINE),"TRIM,UPCASE")
$ INT_PART1 	= "''INT_INT' intrusion, type ''INT_TYP' detected from"
$ INT_PART2 	= "''INT_SRC', with ''INT_CNT' attempts expiring at ''INT_EXP'."
$ INT_TIMESTAMP = F$EXTRACT(0,2,INT_EXP) + F$EXTRACT(3,2,INT_EXP) + -
			F$EXTRACT(6,2,INT_EXP) + F$EXTRACT(9,2,INT_EXP)
$ !
$ IF F$TYPE(INTRUSION_'INT_TIMESTAMP') .NES. "STRING"			-
	THEN CALL Log_Problem "''INT_PART1' ''INT_PART2'"
$ !
$ INTRUSION_'INT_TIMESTAMP' == "Y"
$ !
$ GOTO Intrusion_ReadLoop
$ !
$Intrusion_End:
$ CLOSE INTR
$ DELETE/NOLOG SYS$TEST:WATCHDOG.TMP;*
$ !
$ ENDSUBROUTINE
$ !
$ !***************************************************************************
$ !
$ !	WATCHDOG:	D e v i c e
$ !
$ !***************************************************************************
$Device:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "DEVICE      Checking device error counts..."
$ !
$ CREATE SYS$TEST:WATCHDOG.TMP
$ ASSIGN SYS$TEST:WATCHDOG.TMP SYS$OUTPUT
$ ASSIGN/USER NL: SYS$ERROR
$ SHOW ERROR
$ DEASSIGN SYS$OUTPUT
$ !
$ OPEN/READ/ERROR=Device_End DEVR SYS$TEST:WATCHDOG.TMP
$ !
$Device_ReadLoop:
$ !
$ READ/END_OF_FILE=Device_End DEVR LINE
$ DEVNAME = F$EDIT(F$EXTRACT(0,24,LINE),"TRIM,UPCASE")
$ IF DEVNAME .EQS. "DEVICE" THEN GOTO Device_ReadLoop
$ IF F$EXTRACT(0,1,DEVNAME) .EQS. "%" THEN GOTO Device_ReadLoop
$ IF F$EXTRACT(0,2,DEVNAME) .EQS. "$ " THEN GOTO Device_ReadLoop
$ DEVSYMBOL = DEVNAME - ":" - "   " - "  " - " " - "(" - ")" - "," - " "
$ IF F$TYPE(ERRCNT_'DEVSYMBOL') .EQS. "" THEN ERRCNT_'DEVSYMBOL' == 0
$ DEVERR = F$INTEGER(F$EDIT(F$EXTRACT(35,50,LINE),"TRIM"))
$ IF DEVERR .GT. ERRCNT_'DEVSYMBOL'
$     THEN DEVERRMSG = F$FAO("Device ''DEVNAME' has logged " + -
                        "!UL new error!%S, !UL total.",         -
                        DEVERR-ERRCNT_'DEVSYMBOL',DEVERR)
$          CALL Log_Problem "''DEVERRMSG'"
$	   ERRCNT_'DEVSYMBOL' == F$INTEGER(DEVERR)
$ ENDIF
$ GOTO Device_ReadLoop
$ !
$Device_End:
$ CLOSE DEVR
$ DELETE/NOLOG SYS$TEST:WATCHDOG.TMP;*
$ !
$ ENDSUBROUTINE
$ !

$ !***************************************************************************
$ !
$ !	WATCHDOG:	L i c e n s e
$ !
$ !***************************************************************************
$License:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "LICENSE     Checking licensed products due to expire..."
$ !
$ ASSIGN SYS$TEST:WATCHDOG.TMP SYS$OUTPUT
$ SHOW LICENSE/BRIEF
$ DEASSIGN SYS$OUTPUT
$ !
$ DAY = F$EXTRACT(0,2,F$TIME())
$ !
$ OPEN/READ/ERROR=License_End LIC SYS$TEST:WATCHDOG.TMP
$ !
$License_ReadLoop:
$ READ/END_OF_FILE=License_End LIC LINE
$ MMMYYYY = F$EXTRACT(71,8,LINE)
$ IF F$EXTRACT(3,8,F$TIME()) .EQS. MMMYYYY
$     THEN
$         PRODUCT	= F$EDIT(F$EXTRACT( 0,18,LINE),"TRIM")
$         PRODUCER	= F$EDIT(F$EXTRACT(19,12,LINE),"TRIM")
$         EXPDATE	= F$EDIT(F$EXTRACT(68,11,LINE),"COLLAPSE")
$	  PART1		= "Licensing for ''PRODUCER''s ''PRODUCT' product"
$	  PART2		= "expires on ''EXPDATE'." 
$	  IF F$TYPE(LICENSE_'PRODUCT''DAY')  .NES. "STRING"
$	      THEN
$	  	  CALL Log_Problem "''PART1' ''PART2'"
$	          LICENSE_'PRODUCT''DAY' == "Y"
$	  ENDIF
$ ENDIF
$ GOTO License_ReadLoop
$ !
$License_End:
$ CLOSE LIC
$ DELETE/NOLOG SYS$TEST:WATCHDOG.TMP;*
$ !
$ ENDSUBROUTINE

$ !***************************************************************************

$ !***************************************************************************
$ !
$ !	WATCHDOG:	U s e r n a m e
$ !
$ !***************************************************************************
$Username:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "USERNAME    Checking for users logged on the system..."
$ !
$ ! Setup to loop through all given usernames
$ !
$ USER_PTR = 0
$ !
$User_Loop:
$ !
$ USERNAME = F$ELEMENT(USER_PTR,",",P1)
$ IF USERNAME .EQS. "," THEN GOTO User_Exit
$ !
$ ! Check each one...
$ !
$ ASSIGN/USER NL: SYS$ERROR
$ ASSIGN/USER NL: SYS$OUTPUT
$ SHOW USER 'USERNAME'
$ IF $STATUS .EQ. %X10000001 THEN CALL Log_Problem -
	"Screened username ''USERNAME' has logged on."
$ !
$ USER_PTR = USER_PTR + 1
$ GOTO User_Loop
$ !
$User_Exit:
$ !
$ ENDSUBROUTINE

$ !***************************************************************************
$ !
$ !	WATCHDOG:	P r o c e s s
$ !
$ !***************************************************************************
$Process:
$ SUBROUTINE
$ SET NOON
$ !
$ IF ("''P2'" .EQS. "REPORT_MISSING") THEN -
    CALL Update_Status  "PROCESS     Checking for processes that are missing..."
$ IF ("''P2'" .EQS. "REPORT_PRESENT") THEN -
    CALL Update_Status  "PROCESS     Checking for processes that are present..."
$ !
$ PROC_PTR = 0
$ !
$ SHOW SYSTEM/OUTPUT=SYS$TEST:WATCHDOG.TMP
$ !
$Proc_Loop:
$ !
$ PROCNAME = F$ELEMENT(PROC_PTR,",",P1)
$ IF PROCNAME .EQS. "," THEN GOTO Proc_Exit
$ !
$ ! Check each one...
$ !
$ ASSIGN/NOLOG NL: SYS$OUTPUT
$ ASSIGN/NOLOG NL: SYS$ERROR
$ SEARCH SYS$TEST:WATCHDOG.TMP " ''PROCNAME' "
$ IF $STATUS .EQ. "%X0000001"
$     THEN FOUND = 1
$     ELSE FOUND = 0
$ ENDIF
$ DEASSIGN SYS$OUTPUT
$ DEASSIGN SYS$ERROR
$ !
$ IF (FOUND .EQ. 0) .AND. ("''P2'" .EQS. "REPORT_MISSING") -
      THEN CALL Log_Problem "Process ''PROCNAME' is missing."
$ !
$ IF (FOUND .EQ. 1) .AND. ("''P2'" .EQS. "REPORT_PRESENT") -
      THEN CALL Log_Problem "Process ''PROCNAME' is present."
$ !
$ PROC_PTR = PROC_PTR + 1
$ GOTO Proc_Loop
$ !
$Proc_Exit:
$ DELETE/NOLOG/NOCONFIRM SYS$TEST:WATCHDOG.TMP;
$ !
$ ENDSUBROUTINE

$ !***************************************************************************
$ !
$ !	WATCHDOG:	Q u e u e
$ !
$ !***************************************************************************
$Queue:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "QUEUE       Checking to see if ''P2' is ''P1'..."
$ !
$ IF P1 .EQS. "MISSING"
$     THEN
$	  IF F$GETQUI("DISPLAY_QUEUE","QUEUE_STATUS","''P2'") .EQS. "" THEN -
		CALL Log_Problem "Queue ''P2' is ''P1'"
$ ENDIF
$ !
$ IF P1 .NES. "MISSING"
$     THEN
$	  IF F$GETQUI("DISPLAY_QUEUE","QUEUE_''P1'","''P2'") .EQS. "TRUE" THEN -
		CALL Log_Problem "Queue ''P2' is ''P1'"
$ ENDIF
$ !
$Queue_Exit:
$ !
$ ENDSUBROUTINE

$ !***************************************************************************
$ !
$ !	WATCHDOG:	P i n g
$ !
$ !***************************************************************************
$Ping:
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "PING        Checking remote TCP/IP addresses..."
$ !
$ PING_PTR = 0
$ !
$ !	Determine which Supported TCP/IP package is in use...
$ !
$ !		o MUL - TGV/Cisco's Multinet TCP/IP
$ !		o UCX - Digital's UCX
$ !		o CMU - Carnegie Mellon University's CMU-tek TCP/IP
$ !		o TCP - Process Software's TCPware
$ !		o WOL - Wollongong's TCP/IP
$ !
$ Message = F$ENVIRONMENT("MESSAGE")
$ SET MESSAGE/NOTEXT/NOSEVERITY/NOIDENTIFICATION/NOFACILITY
$ TCP = ""
$ IF TCP .EQS. "" THEN	-
$   TCP = F$EXTRACT(0,F$LENGTH(F$TRNLNM("MULTINET_ROOT","LNM$SYSTEM")),"MUL")
$ IF TCP .EQS. "" THEN	-
$   TCP = F$EXTRACT(0,F$LENGTH(F$GETDVI("BG0:","DEVNAM")),             "UCX")
$ IF TCP .EQS. "" THEN	-
$   TCP = F$EXTRACT(0,F$LENGTH(F$GETDVI("IP0:","DEVNAM")),             "CMU")
$ IF TCP .EQS. "" THEN	-
$   TCP = F$EXTRACT(0,F$LENGTH(F$GETDVI("TCP0:","DEVNAM")),            "TCP")
$ IF TCP .EQS. "" THEN	-
    TCP = F$EXTRACT(0,F$LENGTH(F$GETDVI("INET0:","DEVNAM")),           "WOL")
$ !
$ SET MESSAGE'Message'
$ !
$ ! Setup package parameters
$ !
$ IF TCP .EQS. "MUL" 
$     THEN
$	  PCMD        :== MULTINET PING
$	  PQUAL	        = "/NUMBER=3"
$	  SUCCESSCODE1  = %X00000001
$	  SUCCESSCODE2  = %X10000001
$	  SUCCESSCODE3  = %X00000000
$ ENDIF
$ !
$ IF TCP .EQS. "UCX" 
$     THEN
$	  PCMD        :== UCX PING
$	  PQUAL	        = "/NUMBER=1"	! Required after UCX V5.0 (change #16)
$	  SUCCESSCODE1  = %X130A8093	! ...prior to UCX V5.0
$	  SUCCESSCODE2  = %X10000001	! ......after UCX V5.0    (change #16)
$	  SUCCESSCODE3  = %X00000000
$ ENDIF
$ !
$ IF TCP .EQS. "CMU" 
$     THEN
$	  PCMD        :== IPNCP PING
$	  SUCCESSCODE1  = %X00000001
$	  SUCCESSCODE2  = %X00000000
$	  SUCCESSCODE3  = %X00000000
$ ENDIF
$ !
$ IF TCP .EQS. "TCP" 
$     THEN
$	  PCMD        :== $SYS$SYSROOT:[TCPWARE]PING.EXE
$	  SUCCESSCODE1  = %X10020094
$	  SUCCESSCODE2  = %X00000000
$	  SUCCESSCODE3  = %X00000000
$ ENDIF
$ !
$ IF TCP .EQS. "WOL" 
$	  PCMD        :== PING
$	  SUCCESSCODE1  = %X10000001
$	  SUCCESSCODE2  = %X00000000
$	  SUCCESSCODE3  = %X00000000
$     THEN
$ ENDIF
$ !
$Ping_Loop:
$ !
$ PINGADDR = F$ELEMENT(PING_PTR,",",P1)
$ IF PINGADDR .EQS. "," THEN GOTO Ping_Exit
$ !
$ ! Check each one...
$ !
$ ASSIGN/USER NL: SYS$INPUT
$ ASSIGN/USER NL: SYS$OUTPUT
$ ASSIGN/USER NL: SYS$ERROR
$ !
$ 'PCMD' 'PINGADDR''PQUAL'
$ ! 
$ PINGSTATUS = $STATUS
$ IF (PINGSTATUS .EQ. SUCCESSCODE1) .OR. -
     (PINGSTATUS .EQ. SUCCESSCODE2) .OR. -
     (PINGSTATUS .EQ. SUCCESSCODE3)
$     THEN
$     ELSE
$	  PINGMESSAGE = "Node at address ''PINGADDR' is not responding, "
$	  PINGMESSAGE = "''PINGMESSAGE' PING condition ''PINGSTATUS',"
$	  CALL Log_Problem "''PINGMESSAGE'  ''F$MESSAGE(PINGSTATUS)'."
$ ENDIF
$ !
$ PING_PTR = PING_PTR + 1
$ GOTO Ping_Loop
$ !
$Ping_Exit:
$ !
$ ENDSUBROUTINE

$Setup:
$ SUBROUTINE
$ SET NOON
$ !
$ DELETE 		:== DELETE
$ DELETE/SYMBOL/ALL
$ DELETE/SYMBOL/ALL/GLOBAL
$ !
$ PERFORM 		:== CALL Do_Perform 'P1' 'P2' 'P3' 'P4' 'P5'
$ INTERVAL_PERIOD 	:== "00:10:00"
$ REFRESH_PERIOD        :== "00:00:05"
$ NOTIFY_LIST		:== "SYSTEM"
$ NOTIFY_DELIVERY	:== "OPCOM"
$ !
$ ENDSUBROUTINE

$Do_Perform:
$ SUBROUTINE
$ FLIST1 = "-INTERVAL-NOTIFY-SYSBOOT-DISKS-NODES-INTRUSIONS-"
$ FLIST2 = "DEVICE-LICENSE-USERNAME-PROCESS-QUEUE-PING-"
$ IF F$LOCATE("-''P1'-","''FLIST1'''FLIST2'").EQS.F$LENGTH("''FLIST1'''FLIST2'")
$     THEN
$         CALL Log_Problem						-
		"''P1' is an invalid Watchdog function, check spelling"
$     ELSE
$	  CALL 'P1' 'P2' 'P3' 'P4' 'P5'
$ ENDIF
$ ENDSUBROUTINE

$Update_Status:
$ SUBROUTINE
$ IF F$LENGTH(F$TRNLNM("RAMPAGE$SITE_NAME","RAMPAGE$LOGICALS")) .GT. 0
$     THEN
$ 	  ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''P1'" RAMPAGE$WAT_STATUS
$ 	  WAIT 00:00:01.25
$ ENDIF
$ ENDSUBROUTINE

$Update_History:
$ SUBROUTINE
$ IF F$LENGTH(F$TRNLNM("RAMPAGE$SITE_NAME","RAMPAGE$LOGICALS")) .GT. 0
$     THEN
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H09","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H10
$	  !
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H08","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H09
$	  !
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H07","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H08
$	  !
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H06","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H07
$	  !
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H05","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H06
$	  !
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H04","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H05
$	  !
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H03","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H04
$	  !
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H02","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H03
$	  !
$	  HISTORY :== "''F$TRNLNM("RAMPAGE$WAT_H01","RAMPAGE$LOGICALS")'"
$	  IF F$LENGTH(HISTORY) .GT. 0 THEN -
	      ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS "''HISTORY'" RAMPAGE$WAT_H02
$         !
$ 	  ASSIGN/NOLOG/TABLE=RAMPAGE$LOGICALS 				-
		"''F$EXTRACT(0,20,F$TIME())' ''P1'" RAMPAGE$WAT_H01
$ ENDIF
$ ENDSUBROUTINE

$INTERVAL:
$ SUBROUTINE
$ !
$ INTERVAL_PERIOD :== "''P1'"
$ IF "''P2'" .NES. "" THEN REFRESH_PERIOD :== "''P2'"
$ !
$ ENDSUBROUTINE

$NOTIFY:
$ SUBROUTINE
$ !
$ NOTIFY_LIST 	  :== "''P1'"
$ NOTIFY_DELIVERY :== "''P2'"
$ !
$ IF NOTIFY_LIST .EQS. "" THEN NOTIFY_LIST :== "SYSTEM"
$ IF NOTIFY_DELIVERY .EQS. "" THEN NOTIFY_DELIVERY :== "OPCOM"
$ !
$ ENDSUBROUTINE
$ !
$Log_Problem:
$ !
$ ! "Watchdog Warning on Node " via OPCOM, MAIL, REPLY, RamPage
$ !
$ SUBROUTINE
$ SET NOON
$ !
$ CALL Update_Status  "**Problem** ''P1'"
$ CALL Update_History "''P1'"
$ !
$ MSGTXT :== "Watchdog Warning on node ''F$GETSYI("NODENAME")': ''P1'"
$ NOTIFY_LIST		= F$EDIT(NOTIFY_LIST,"TRIM,UPCASE")
$ NOTIFY_DELIVERY	= F$EDIT(NOTIFY_DELIVERY,"TRIM,UPCASE")
$ !
$ DELIVER_PTR = 0
$ !
$Deliver_Loop:
$ !
$ USERNAME = F$ELEMENT(DELIVER_PTR,",",NOTIFY_LIST)
$ IF USERNAME .EQS. "," THEN GOTO Deliver_Exit
$ !
$ ! Sent to each of the usernames...
$ !
$ ! RamPage Delivery
$ !
$ IF F$LOCATE("RAMPAGE",NOTIFY_DELIVERY) .NE. F$LENGTH(NOTIFY_DELIVERY)
$     THEN
$	  RAMPAGE 'USERNAME "''MSGTXT'"
$ ENDIF
$ !
$ ! OPCOM Delivery
$ !
$ IF F$LOCATE("OPCOM",NOTIFY_DELIVERY) .NE. F$LENGTH(NOTIFY_DELIVERY)
$     THEN
$	  REQUEST "Notice to ''USERNAME': ''MSGTXT'"
$ ENDIF
$ !
$ ! MAIL Delivery
$ !
$ IF F$LOCATE("MAIL",NOTIFY_DELIVERY) .NE. F$LENGTH(NOTIFY_DELIVERY)
$     THEN
$	  MAIL/SUBJECT="''MSGTXT'" NL: 'USERNAME
$ ENDIF
$ !
$ ! REPLY Delivery
$ !
$ IF F$LOCATE("REPLY",NOTIFY_DELIVERY) .NE. F$LENGTH(NOTIFY_DELIVERY)
$     THEN
$	  REPLY/USERNAME='USERNAME "''MSGTXT'"
$ ENDIF
$ !
$ !WRITE SYS$OUTPUT "Delivering notice to ''USERNAME'"
$ !
$ DELIVER_PTR = DELIVER_PTR + 1
$ GOTO Deliver_Loop
$Deliver_Exit:
$ ENDSUBROUTINE

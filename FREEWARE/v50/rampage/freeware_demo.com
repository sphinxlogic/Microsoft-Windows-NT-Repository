$ !
$ !	F R E E W A R E _ D E M O . C O M
$ !
$ !	RamPage Watchdog Dispatcher V2.0-2 by Ergonomic Solutions
$ !
$ !	(800)ERGO-SOL   info@ergosol.com   www.ergosol.com
$ !
$ ! This demo file was created specifically for the OpenVMS Freeware CD.  It
$ ! creates logical names that are similar to the ones created by WATCHDOG so
$ ! we can display the Watchdog Monitor in a realistic fashion.
$ !
$ ! No actual WATCHDOG will run, and no alerts will be generated.
$ !
$ ! Start by getting the current directory location, which we'll use for
$ ! the other demo file references to ensure position independence.
$ !
$ !SET NOON
$ !
$ FDLOC = F$ENVIRONMENT("PROCEDURE")
$ FDNAM = F$PARSE(FDLOC,,,"NAME")
$ FDTYP = F$PARSE(FDLOC,,,"TYPE")
$ FDVER = F$PARSE(FDLOC,,,"VERSION")
$ FDLOC = FDLOC - FDNAM - FDTYP - FDVER
$ !
$ ! Are we on a VAX or Alpha?
$ !
$ IF F$GETSYI("CPU") .LT. 128
$     THEN
$         ARCH = "VAX"
$     ELSE
$         ARCH = "AXP"
$ ENDIF
$ !
$ ! If not already present (i.e. existing RamPage customer or demo installed),
$ ! create logical name table RAMPAGE$LOGICALS to store the simulated
$ ! responses from WATCHDOG.
$ !
$ NEWTBL = "N"
$ IF F$TRNLNM("RAMPAGE$LOGICALS","LNM$SYSTEM_DIRECTORY",,,,"TABLE") .NES. "TRUE"
$   THEN
$     CREATE$/NAME_TABLE RAMPAGE$LOGICALS/PARENT_TABLE=LNM$SYSTEM_DIRECTORY
$     NEWTBL = "Y"
$ ENDIF
$ !
$ ! Also if not already present, create the site name displayed by the monitor
$ !
$ NEWSIT = "N"
$ IF F$LENGTH(F$TRNLNM("RAMPAGE$SITE_NAME","RAMPAGE$LOGICALS")) .EQ. 0
$   THEN
$     ASSIGN$/TABLE=RAMPAGE$LOGICALS "(Your company's name)" RAMPAGE$SITE_NAME
$     NEWSIT = "Y"
$ ENDIF
$ !
$ ! Create a looping setup that simulates the WATCHDOG real-time responses
$ !
$ CREATE$ SYS$SCRATCH:WATCHDOG_SIMULATION.COM
$ DECK$
$ ATR := ASSIGN$/TABLE=RAMPAGE$LOGICALS/NOLOG
$ ASSIGN$/JOB "''F$GETJPI("","PID")'" DEMOPID
$ !
$ ON ERROR THEN EXIT
$ ON WARNING THEN EXIT
$ ON SEVERE_ERROR THEN EXIT
$ !
$ T1 = F$EXTRACT(0,20,F$CVTIME("TODAY-2-05:33:16","ABSOLUTE"))
$ IF F$EXTRACT(19,19,T1) .EQS. "." THEN T1 = " " + F$EXTRACT(0,19,T1)
$ !
$ T2 = F$EXTRACT(0,20,F$CVTIME("TODAY-7-00:05:25","ABSOLUTE"))
$ IF F$EXTRACT(19,19,T2) .EQS. "." THEN T2 = " " + F$EXTRACT(0,19,T2)
$ !
$ T3 = F$EXTRACT(0,20,F$CVTIME("TODAY-9-15:57:56","ABSOLUTE"))
$ IF F$EXTRACT(19,19,T3) .EQS. "." THEN T3 = " " + F$EXTRACT(0,19,T3)
$ !
$ T4 = F$EXTRACT(0,20,F$CVTIME("TODAY-10-03:22:34","ABSOLUTE"))
$ IF F$EXTRACT(19,19,T4) .EQS. "." THEN T4 = " " + F$EXTRACT(0,19,T4)
$ !
$ T5 = F$EXTRACT(0,20,F$CVTIME("TODAY-16-04:13:12","ABSOLUTE"))
$ IF F$EXTRACT(19,19,T5) .EQS. "." THEN T5 = " " + F$EXTRACT(0,19,T5)
$ !
$ T6 = F$EXTRACT(0,20,F$CVTIME("TODAY-16-09:52:25","ABSOLUTE"))
$ IF F$EXTRACT(19,19,T6) .EQS. "." THEN T6 = " " + F$EXTRACT(0,19,T6)
$ !
$ T7 = F$EXTRACT(0,20,F$CVTIME("TODAY-17-01:11:12","ABSOLUTE"))
$ IF F$EXTRACT(19,19,T7) .EQS. "." THEN T7 = " " + F$EXTRACT(0,19,T7)
$ !
$ T8 = F$EXTRACT(0,20,F$GETSYI("BOOTTIME"))
$ IF F$EXTRACT(19,19,T8) .EQS. "." THEN T8 = " " + F$EXTRACT(0,19,T8)
$ !
$ E1 = F$EXTRACT(0,11,F$CVTIME("TODAY+17-","ABSOLUTE"))
$ !
$ ATR "''T1' Device MKB100: has logged 3 new errors, 24 total." 	-
	"RAMPAGE$WAT_H01"
$ ATR "''T2' WARNING: Disk DUA1: is at 96% of capacity."		-
	"RAMPAGE$WAT_H02"
$ ATR "''T3' Queue SYS$BATCH is STOPPED"                        	-
	"RAMPAGE$WAT_H03"
$ ATR "''T4' Node at address 204.25.12.1 is not responding, PING timeout" -
	"RAMPAGE$WAT_H04"
$ ATR "''T5' Lost cluster connection to node SATURN"			-
	"RAMPAGE$WAT_H05"
$ ATR "''T6' NETWORK intrustion, type SUSPECT detected from TCP-IP:204.2" -
	"RAMPAGE$WAT_H06"
$ ATR "''T7' Licensing for DEC's COBOL product expires on ''E1'."	-
	"RAMPAGE$WAT_H07"
$ ATR "''T8' System booted at ''F$GETSYI("BOOTTIME")'"			-
	"RAMPAGE$WAT_H08
$ !
$Loop:
$ !
$ ATR "(Next Watchdog Cycle in 15 seconds)" 				-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "(Next Watchdog Cycle in 10 seconds)" 				-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "(Next Watchdog Cycle in 5 seconds)" 				-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "SYSBOOT     Checking for recent system reboot..." 		-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "DISKS       Checking disks for 95% capacity..." 			-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "NODES       Checking for unreachable cluster nodes..." 		-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "INTRUSIONS  Checking for intrusions type ALL..." 		-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "DEVICE      Checking device error counts..." 			-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "LICENSE     Checking licensed products due to expire..." 	-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "QUEUE       Checking to see if SYS$BATCH is STOPPED..." 		-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ ATR "PING        Checking remote TCP/IP addresses..." 		-
	"RAMPAGE$WAT_STATUS"
$ WAIT 00:00:05
$ !
$ GOTO Loop
$ !
$ EOD
$ ! 
$ SPAWN$/NOLOG/NOWAIT/OUTPUT=NL: @SYS$SCRATCH:WATCHDOG_SIMULATION.COM
$ !
$ ! Run the Watchdog Monitor to give the demo user a taste of what to expect
$ !
$ RUN 'FDLOC'RAMPAGEWDM.EXE_'ARCH' 
$ !
$ ! Clean up stuff we created
$ !
$ STOP$/ID="''F$TRNLNM("DEMOPID")'"
$ DELETE$/NOLOG SYS$SCRATCH:WATCHDOG_SIMULATION.COM;
$ IF NEWSIT .EQS. "Y" THEN DEASSIGN$/TABLE=RAMPAGE$LOGICALS 		-
	RAMPAGE$SITE_NAME
$ IF NEWTBL .EQS. "Y" THEN DEASSIGN$/TABLE=LNM$SYSTEM_DIRECTORY 	-
	RAMPAGE$LOGICALS
$ !
$ EXIT$

$! This file creates the monthly decnet registry report that is 
$! copied to DD7 (133.50.210.10) each month.
$! Notes:
$!  This relies on EMU running. If it is not the report is not
$! produced and the procedure produces an error message.
$! 
$	IF F$SEARCH("REGISTRY.RPT") .NES. "" THEN DEL/NOLOG REGISTRY.RPT;*
$! The following 2 lines define the EMU reporting DCL interface as
$! a foreign command and executes it. Paramater 1 is the report paramater
$! file name and paramater 2 is the output file. 
$! To change the directory this runs in change ONLY paramater2
$ 	REPORT :== $EMU5_EXE:EMU_REPORTING.EXE
$ 	REPORT REGISTRY SYS$LOGIN:REGISTRY.RPT
$! 
$	OPEN/READ/ERR=NO_FILE INPT REGISTRY.RPT
$	OPEN/WRITE OUTP REGISTRY.TXT     
$ READ_LOOP:
$	READ/END=ENDE INPT REC
$	DN4A = F$ELEMENT(1,"|",REC)
$	DN4N = F$ELEMENT(2,"|",REC)
$	DN5N = F$ELEMENT(3,"|",REC)
$	DEV = F$ELEMENT(4,"|",REC)
$	IF F$EXTRACT(0,2,DN4A) .NES. "50" THEN GOTO READ_LOOP
$! Fix for DEC error - Async routers advertise themselves as DECserver 200.
$	IF F$EXTRACT(0,9,DEV) .EQS. "DECserver" THEN DEV = "Async DECRouter"
$	WRITE OUTP F$FAO("!20<!AS!> !20<!AS!> !20<!AS!> !20<!AS!>",DN4A,DN4N,DN5N,DEV)
$	GOTO READ_LOOP
$ ENDE:
$	CLOSE INPT
$	CLOSE OUTP
$	PURGE REGISTRY.TXT
$	EXIT
$ NO_FILE:
$	WRITE SYS$OUTPUT "Reporting failed. Check EMU is running by:"
$	WRITE SYS$OUTPUT "$ @EMU5_SRC:EMU_LOGICALS"
$	WRITE SYS$OUTPUT "$ EMU"
$	WRITE SYS$OUTPUT "If either of these commands fail, contact support"
$	EXIT


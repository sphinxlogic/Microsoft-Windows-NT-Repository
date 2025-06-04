$! This procedure formats the input file (REGISTRY.RPT) and writes 
$! REGISTRY.TXT. The resulting file is then transmitted to DD7 (133.50.210.10)
$! and is the UK portion of the BoA DECnet Registry.
$! 
$!  REGISTRY.RPT is produced by EMU reporting.
$! 
$	OPEN/READ INPT EMU5_RPT:REGISTRY.RPT
$	OPEN/WRITE OUTP REGISTRY.TXT
$	WRITE OUTP "DECnet Registry Input file for ",F$TIME()
$ READ_LOOP:
$	READ/END=ENDE INPT REC
$	IF F$EXTRACT(0,4,REC) .EQS. "||||" THEN GOTO READ_LOOP
$	DN4N = F$ELEMENT(1,"|",REC)
$	DN4A = F$ELEMENT(0,"|",REC)
$	DN5N = F$ELEMENT(2,"|",REC)
$	DEV = F$ELEMENT(3,"|",REC)
$	IF DN4A .EQS. "" THEN GOTO READ_LOOP
$ WRITE OUTP F$FAO("!10<!AS!> !10<!AS!> !20<!AS!>!AS",DN4A,DN4N,DN5N,DEV)
$	GOTO READ_LOOP
$ ENDE:
$	CLOSE INPT
$	CLOSE OUTP
$	EXIT

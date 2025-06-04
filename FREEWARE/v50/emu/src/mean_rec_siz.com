$! File to calculate the mean record size in a file. File must first be:
$! 	$DUMP/REC/OUT=inpt.dat file-to-be-calculated
$! 
$	IF P1 .EQS. "" THEN INQ P1 "File? "
$	OPEN/READ/ERR=NOFILE INPT 'P1
$	RECCNT = 0
$	RECSIZ = 0
$ READ_LOOP:
$	READ/END=ENDE INPT REC
$	IF F$EXTRACT(0,3,REC) .NES. "Rec" THEN GOTO READ_LOOP
$	SIZ = F$ELEMENT(2,"(",REC)
$	SIZ = F$EXTRACT(0,4,SIZ)
$	RECSIZ=RECSIZ+%X'SIZ
$	RECCNT = RECCNT+1
$	GOTO READ_LOOP
$ ENDE:
$	T = RECSIZ/RECCNT
$	WRITE SYS$OUTPUT "Average rec size in ",P1," = ",T
$	CLOSE INPT
$	EXIT
$ NOFILE:
$	WRITE SYS$OUTPUT "Could not open ",P1," Error = ",F$MESSAGE('$STATUS)
$	EXIT


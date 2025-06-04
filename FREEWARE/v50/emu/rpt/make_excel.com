$! File to take EMU report output and format it for EXCEL
$! Changes made:
$!  Strip 1st column (BOXID) off
$!  When boxid changes, insert blank line
$! Optional:
$! If P2 present, it is the max width of any field. 
$! 
$! P1 = File name. If not present it is asked for. If file type is
$! not specified, it is assumed to be .rpt. If specified it must be .rpt

$	IF P1 .EQS. "" THEN INQ P1 "Input file? "
$	IF F$LOCATE(".",P1) .EQ. F$LENGTH(P1) THEN P1 = P1+".RPT"
$	OUTFIL = P1 - ".RPT"+".TXT
$	WRITE SYS$OUTPUT "Reading ",P1
$	WRITE SYS$OUTPUT "Writing ",OUTFIL
$	ELECNT = 0
$	BOXID = " "
$	OPEN/READ INPT 'P1
$	OPEN/WRITE OUTP 'OUTFIL     
$	READ/END=ENDE INPT REC
$ ELE_LOOP:
$! Count the fields. We need this to insert the correct number of seperators in
$! a blank line
$! First rec is headers; That is all fields are present.
$       IF F$ELEMENT('ELECNT',"|",REC).EQS."" THEN GOTO DONE_COUNT
$       ELECNT=ELECNT+1
$       GOTO ELE_LOOP
$ DONE_COUNT:
$       WRITE OUTP F$EXTRACT(F$LOCATE("|",REC)+1,F$LENGTH(REC)-F$LOCATE("|",REC)+1,REC)
$ READ_LOOP:
$	READ/END=ENDE INPT REC
$	IF F$ELEMENT(0,"|",REC) .NES. BOXID
$	THEN
$		WRITE OUTP F$FAO("!''ELECNT'*|")
$	BOXID = F$ELEMENT(0,"|",REC)
$	ENDIF
$	IF P2 .NES. "" THEN GOSUB CHECK_LENGTH
$! Strip 1st column from all recs. When boxid changes insert blank line
$       WRITE OUTP F$EXTRACT(F$LOCATE("|",REC)+1,F$LENGTH(REC)-F$LOCATE("|",REC)+1,REC)
$	GOTO READ_LOOP
$ ENDE:
$	CLOSE INPT
$	CLOSE OUTP
$	EXIT
$ CHECK_LENGTH: 
$! Check len of each element and truncate any that exceed P2
$	TCNT = 1
$	TREC = ""
$	SIZE = F$INTEGER(P2)
$ SUB_LOOP:
$	IF F$LENGTH(F$ELEMENT('TCNT,"|",REC)) .LE. SIZE 
$	THEN 
$		TREC = TREC +"|"+F$ELEMENT('TCNT,"|",REC)
$	ELSE
$ SHO SYM TCNT
$ SHO SYM REC 
$ SHO SYM SIZE
$ SHO SYM TREC
$		TREC = TREC+"|"+F$EXTRACT(0,'SIZE,F$ELEMENT('TCNT,"|",REC))
$	ENDIF
$	TCNT = TCNT +1
$	IF TCNT .LE. ELECNT THEN GOTO SUB_LOOP
$	REC = TREC
$	RETURN
$	EXIT
	

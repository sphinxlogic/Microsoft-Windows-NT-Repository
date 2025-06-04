$! File to extract comments from macro file and produce help version.
$! ;++ in the 1st 3 positions is the signal to start copying
$! ;-- in the 1st 3 positions is the signal to stop.
$! All text between is assumed to be commented text formatted for inclusion
$! is a VMS help library. The 1st char (assumed to be ;) is removed and the
$! remainder of the line is copied to output.
$! output file name is the same as input with extension .HLP
$! 
$ 	IF P1 .EQS. "" THEN INQ P1 "File?(file.type)"
$	IF P1 .EQS. "" THEN EXIT
$	IF F$SEARCH("HLP.TMP") .NES. "" THEN DELETE HLP.TMP;*
$	ASS/USER HLP.TMP SYS$OUTPUT
$	DIR/SIZE 'P1
$	OPEN/READ FILES HLP.TMP
$ READ_LOOP:
$	READ/END=NO_MORE FILES FREC
$	LCNT = 0
$	IF F$LOCATE(";",FREC) .EQ. F$LENGTH(FREC) THEN GOTO READ_LOOP
$	P1 = F$EXTRACT(0,F$LOCATE(".",FREC),FREC)	
$	INFILE = P1+".MAR"
$	OUTFIL = "EMU5_HLP:"+P1+".HLP"
$	WRITE SYS$OUTPUT "Creating ",OUTFIL
$	OPEN/READ INPT 'INFILE
$	OPEN/WRITE OUTP 'OUTFIL
$ LOOP1:
$	READ/END=ENDE INPT REC
$	IF F$EXTRACT(0,3,REC) .NES. ";++" THEN GOTO LOOP1
$ LOOP2:
$	READ/END=ENDE INPT REC
$	IF F$EXTRACT(0,3,REC) .EQS. ";--" THEN GOTO LOOP1
$	LCNT = LCNT+1
$	WRITE OUTP F$EXTRACT(1,F$LENGTH(REC),REC)
$	GOTO LOOP2
$ ENDE:
$	WRITE SYS$OUTPUT LCNT," Lines written to ",OUTFIL
$	CLOSE INPT
$	CLOSE OUTP
$	IF LCNT .EQ. 0 
$	THEN
$		DELFIL=OUTFIL+";*"
$		DELET/NOLOG 'DELFIL
$	ELSE
$	LIBR/REPL EMU5_HLP:EMU_ROUTINES.HLB 'OUTFIL
$	ENDIF
$	GOTO READ_LOOP
$ NO_MORE:
$	CLOSE FILES
$	DEL HLP.TMP;*
$	EXIT

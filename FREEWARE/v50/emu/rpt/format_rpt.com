$! File to apply general formatting to EMU generated reports.
$! The input file is asked for and then scanned to find:
$! 	Number of fields
$! 	Max len of each field
$! The output file (same name as input with .txt extension) is 
$! then produced setting each field to the max found + 1 in len.
$! 
$ START:
$	INQ INP "File [?] For Listing: "
$	IF INP .EQS. "" THEN EXIT
$	IF F$EXTRACT(0,1,INP) .EQS. "?"
$	THEN
$		DIR/PAGE/SIZE *.RPT;*
$		GOTO START
$	ENDIF
$	IF F$LOCATE(".",INP) .EQS. F$LENGTH(INP) THEN INP = INP+".RPT"
$	OUTFIL =  INP - ".RPT"+".TXT"
$	OPEN/READ/ERR=START INPT 'INP'
$	ELECNT = 0
$	CUR = 0
$	READ/END=END_SCAN INPT REC
$ ELE_LOOP:
$! First rec is headers; That is all fields are present.
$       IF F$ELEMENT('ELECNT',"|","''REC'").EQS."" THEN GOTO LEN_LOOP1
$	MAXCNT = "MAXCNT"+F$STRING(ELECNT)
$	'MAXCNT' = 0
$       ELECNT=ELECNT+1
$       GOTO ELE_LOOP
$ ! Set len symbols
$ LEN_LOOP1:
$	LENCNT = "LENCNT"+F$STRING(CUR)
$	MAXCNT = "MAXCNT"+F$STRING(CUR)
$	'LENCNT' = F$LENGTH(F$ELEMENT('CUR',"|",REC))
$	IF 'LENCNT .GT. 'MAXCNT THEN 'MAXCNT = 'LENCNT
$	CUR = CUR+1
$	IF CUR .LT. ELECNT THEN GOTO LEN_LOOP1
$ SCAN_LOOP:
$	CUR = 0
$	READ/END=END_SCAN INPT REC
$ LEN_LOOP2:
$	LENCNT = "LENCNT"+F$STRING(CUR)
$	MAXCNT = "MAXCNT"+F$STRING(CUR)
$	'LENCNT' = F$LENGTH(F$ELEMENT('CUR',"|",REC))
$	IF 'LENCNT .GT. 'MAXCNT THEN 'MAXCNT = 'LENCNT
$	CUR = CUR+1
$	IF CUR .LT. ELECNT THEN GOTO LEN_LOOP2
$	GOTO SCAN_LOOP
$ END_SCAN:
$	CBOXID = ""
$	CLOSE INPT
$	CTRSTR = ""
$	CUR = 0
$	OPEN/READ INPT 'INP'
$	OPEN/WRITE OUTP 'OUTFIL
$ WRITE_LOOP:
$	READ/END=ENDE INPT REC
$	CUR = 0
$	BOXID = F$ELEMENT(0,"|",REC)
$	IF CBOXID .NES. BOXID
$	THEN
$		WRITE OUTP ""
$		CBOXID = BOXID
$	ENDIF
$	RECSTR = ""
$	CTRSTR = ""
$	ELEM = ""
$ CTR_LOOP:
$	MAXCNT = "MAXCNT"+F$STRING(CUR)
$	CTRSTR = CTRSTR+"!"+F$STRING('MAXCNT')+"<!AS!> "
$	ELEM = "ELEM"+F$STRING(CUR)
$	'ELEM = F$ELEMENT('CUR,"|",REC) 
$	RECSTR = RECSTR+ELEM+","
$	CUR = CUR+1
$	IF CUR .LT. ELECNT THEN GOTO CTR_LOOP
$	RECSTR = F$EXTRACT(0,F$LENGTH(RECSTR)-1,RECSTR)
$	WRITE OUTP  F$FAO("''CTRSTR'",'RECSTR)
$	GOTO WRITE_LOOP
$ ENDE:
$	CLOSE INPT
$	CLOSE OUTP
$	EXIT

$! File to read MACRO source files and check that calls using the stack
$! have the right number of arguments. Any filename with wildcards
$!  is /are processed but they MUST be .MAR filetype.
$! 
$! The procedure is not perfect - it won't find all errors nor will it
$! always report accurately. The following assumptions are made:
$! 	Any instruction containing PUSH (that is PUSHL, PUSHAL etc
$! 	counts as 1 item on the stack
$! 	Any paramater containing -(sp) is 1 item on the stack
$! 	Any CALLS instruction is parsed to find the number of arguments
$! 	specified. This number is subtracted from the count of stack items
$! 	and if not - 0 an error is displayed. The error is displayed as:
$! Filename, Line number, call instruction.
$! 
$	IF F$SEARCH("FILES.TMP") .NES. "" THEN DELETE FILES.TMP;*
$ GET_FILE:
$	INQ INP "File(s) to check (Name only): "
$	FILES = INP+".MAR"
$	DIR /COLUMN=1/OUTP=FILES.TMP 'FILES
$	IF .NOT. $STATUS THEN GOTO GET_FILE
$	OPEN SFILES FILES.TMP
$ FILE_LOOP:
$	READ/END=ENDE SFILES FNAME
$	IF F$LOCATE(";",FNAME) .EQ. F$LENGTH(FNAME) THEN GOTO FILE_LOOP
$	OPEN MFILE 'FNAME
$	LCNT = 0
$	ACNT = 0
$ CHECK_LOOP:
$	READ/END=END_FILE MFILE REC
$	LCNT = LCNT + 1
$	IF F$LOCATE("PUSH",F$EDIT(REC,"UPCASE")) .NE. F$LENGTH(REC) 
$	THEN 
$		ACNT = ACNT+1
$!		WRITE SYS$OUTPUT "Arg ",ACNT," = ",REC
$	ENDIF
$	IF F$LOCATE("-(SP)",F$EDIT(REC,"UPCASE")) .NE. F$LENGTH(REC) 
$	THEN 
$		ACNT = ACNT+1
$!		WRITE SYS$OUTPUT "Arg ",ACNT," = ",REC
$	ENDIF
$	IF F$LOCATE("PUSHQ",F$EDIT(REC,"UPCASE")) .NE. F$LENGTH(REC) 
$	THEN 
$		ACNT = ACNT+1
$!		WRITE SYS$OUTPUT "Arg ",ACNT," = ",REC
$	ENDIF
$	IF F$LOCATE("CLRQ",F$EDIT(REC,"UPCASE")) .NE. F$LENGTH(REC) .AND. -
	F$LOCATE("-(SP)",F$EDIT(REC,"UPCASE")) .NE. F$LENGTH(REC) 
$	THEN 
$		ACNT = ACNT+1
$!		WRITE SYS$OUTPUT "Arg ",ACNT," = ",REC
$	ENDIF
$	IF F$LOCATE("+(SP)",F$EDIT(REC,"UPCASE")) .NE. F$LENGTH(REC) 
$	THEN 
$		ACNT = ACNT+1
$!		WRITE SYS$OUTPUT "Arg ",ACNT," = ",REC
$	ENDIF
$	IF F$LOCATE("CALLS",F$EDIT(REC,"UPCASE")) .NE. F$LENGTH(REC) 
$	THEN 
$      		POS = F$LOCATE("#",REC)+1
$      		LEN = F$LOCATE(",",REC)-POS
$      		CCNT = F$INTEGER(F$EXTRACT('POS,'LEN,REC))
$      		WRITE SYS$OUTPUT "Found call with ",CCNT," Aguments (expecting ",ACNT,")"
$		IF ACNT-CCNT .NE. 0 
$	        THEN
$		  WRITE SYS$OUTPUT "Check file ",FNAME," Line ",LCNT," ",REC
$		ENDIF
$		ACNT = 0
$	ENDIF
$	GOTO CHECK_LOOP
$ END_FILE:
$	CLOSE MFILE
$	GOTO FILE_LOOP
$ ENDE:
$	CLOSE SFILES
$	DELETE FILES.TMP;*
$	EXIT

  

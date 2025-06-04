$ ! File to extract message nubers from message listing and create
$ ! macro defs
$ ! 
$	INQ INP "File ?"
$	IF F$LENGTH(INP) .EQ. 0 THEN EXIT
$	INP = F$EXTRACT(0,F$LOCATE(".",INP),INP)+".LIS"
$	OUT = F$EXTRACT(0,F$LOCATE(".",INP),INP)+".DEF"
$	OPEN/READ INPT 'INP'
$	OPEN/WRITE OUTP 'OUT'
$ ! Find Message number
$ READ:
$ 	READ/END=ENDE INPT REC
$	IF F$LENGTH(REC) .LT. 32 THEN GOTO READ
$	IF F$EXTRACT(25,8,REC) .EQS. "        " THEN GOTO READ
$	IF F$EXTRACT(0,4,REC) .NES. "    " THEN GOTO READ
$	IF F$EDIT(F$EXTRACT(48,4,REC),"UPCASE") .EQS. ".FAC" 
$	THEN 
$		FAC = F$EDIT(F$EXTRACT(64,12,REC),"UPCASE")
$		FAC = F$EXTRACT(0,F$LOCATE(",",FAC),FAC)
$		GOTO READ
$	ENDIF
$	NUM =   "<^X"+F$EXTRACT(25,8,REC)+">"
$	NAM =   F$EXTRACT(48,15,REC)
$! sho sym rec
$! sho sym nam
$	NAM =   F$EDIT(F$EXTRACT(0,F$LOCATE("<",NAM),NAM),"COMPRESS")
$! sho sym nam
$	WRITE OUTP "$EQU	MSG_",FAC,"_",NAM,"	",NUM
$	WRITE SYS$OUTPUT "$EQU	MSG_",FAC,"_",NAM,"	",NUM
$	GOTO READ
$ ENDE:
$	CLOSE INPT
$	CLOSE OUTP
$	EXIT
	
$			

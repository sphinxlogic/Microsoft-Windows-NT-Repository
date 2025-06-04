$! File to delete the EMU_CONTROL process thus triggering a controlled
$! shutdown of the EMU system.
$	SAY = "WRITE SYS$OUTPUT"
$	IF P1 .NES. "" THEN GOTO SKIP_PROMPT
$	SAY "This procedure will shutdown the EMU system if run"
$	SAY "from a suitably priveleged account"
$	INQ INPT "Do you wish to continue [N] ?"
$	IF F$EDIT(F$EXTRACT(0,1,INPT),"UPCASE") .NES. "Y" THEN EXIT
$ SKIP_PROMPT:
$	SET UIC [1,4]
$	STOP EMU_CONTROL
$	IF P1 .NES. "" THEN EXIT
$	SAY "Shutdown requested. To ensure shutdown is complete,"
$	SAY "type EMU at the DCL prompt. When the error 
$	SAY "'Fatal Controller error' is received, shutdown has"
$	SAY "finished"
$	EXIT


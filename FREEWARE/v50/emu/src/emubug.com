$ ! File to keep notes in. 
$ ! Setup:
$ ! Put this file in any directory and point logical "EMUBUG_DIR: to it:
$ DEF/JOB EMUBUG_DIR EMU5_DOC
$ ! Then:
$ ! EMUBUG :== "@EMUBUG_DIR:EMUBUG"
$ ! 
$ ! Each invokation of EMUBUG then opens the text file, appends the currrent
$ ! time and allows free form editing. 
$ ! 
$	IF F$TRNLNM("EMUBUG_DIR") .EQS. ""
$	THEN
$		WRITE SYS$OUTPUT "You have not defined the logical"
$		WRITE SYS$OUTPUT "EMUBUG_DIR to point to the directory"
$		WRITE SYS$OUTPUT "where the procedure is"
$		EXIT
$	ENDIF
$	IF F$SEARCH("EMUBUG_DIR:EMUBUG.DAT") .EQS. ""
$	THEN
$		OPEN/WRITE OUTP EMUBUG_DIR:EMUBUG.DAT
$		CLOSE OUTP
$	ENDIF
$	T = F$TIME()
$	OPEN/APPEND OUTP EMUBUG_DIR:EMUBUG.DAT
$	WRITE OUTP "***"
$	WRITE OUTP "***"
$	WRITE OUTP T
$	WRITE OUTP ""
$	CLOSE OUTP
$	ASS/PROC/NOLOG TT SYS$COMMAND 
$	ASS/PROC/NOLOG TT SYS$INPUT   
$	DEASS/PROC SYS$COMMAND 
$	DEASS/PROC SYS$INPUT   
$	EDIT/TPU EMUBUG_DIR:EMUBUG.DAT
$	PURGE/NOLOG EMUBUG_DIR:EMUBUG.DAT
$	RENAME EMUBUG_DIR:EMUBUG.DAT EMUBUG_DIR:EMUBUG.DAT;1 
$ 	EXIT


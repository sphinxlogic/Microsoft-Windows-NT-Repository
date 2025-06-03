$ SAVE_VERIFY = F$VERIFY()
$ SET NOVERIFY
$ !-----------------------------------------------------------------------
$ ! Compile C files in connected directory:
$ !
$ ! Usage: @CCOM prefix compileoptions
$ !
$ ! where prefix is a wild-card specification of the names of .C files
$ ! to be compiled.   
$ !
$ ! Examples:
$ !	Compile everything:
$ !	@CCOM *
$ !
$ !	Compile today's efforts:
$ !	@CCOM * "" /SINCE
$ !
$ !	Compile certain routines changed since yesterday:
$ !	@CCOM F%%%.C,G*.C,*UPD* /LIST/MACHINE_CODE /SINCE=YESTERDAY
$ !
$ ! Note in the  last example that  the extension .C  must be supplied
$ ! for all but the last  file, which gets one  automatically.  In the
$ ! second example, the switch /SINCE means the same as /SINCE:TODAY.
$ ! [27-NOV-86]
$ !-----------------------------------------------------------------------
$ !
$ ON CONTROL_Y THEN $ GOTO DONE
$ !
$ IF P1 .EQS. "" THEN INQUIRE P1 "Wild-card name of C files to compile"
$ !
$ DIRECTORY /VER=1/COL=1/NODATE/NOSIZE/NOHEAD/NOTRAIL/OUT=DIR.TMP'P3' 'P1'.C
$ !
$ IF .NOT. $STATUS THEN GOTO DIRECTORY_ERROR
$ !
$ OPEN /READ DIRFILE DIR.TMP
$ !
$ NEWLINE:
$	READ /END=DONE DIRFILE NAME
$	DEV := 'F$PARSE(NAME,,,"DEVICE")'
$ 	DIR := 'F$PARSE(NAME,,,"DIRECTORY")'
$ 	NAM := 'F$PARSE(NAME,,,"NAME")'
$ 	EXT := 'F$PARSE(NAME,,,"TYPE")'
$	LEADER := 'DEV''DIR''NAM'
$	C_STAMP = F$FILE_ATTRIBUTES("''LEADER'.C","RDT")
$	C_STAMP = F$CVTIME(C_STAMP)
$	OBJ_STAMP = F$SEARCH("''LEADER'.OBJ")
$	IF OBJ_STAMP .NES. "" THEN -
		OBJ_STAMP = F$FILE_ATTRIBUTES("''LEADER'.OBJ","RDT")
$	IF OBJ_STAMP .NES. "" THEN -
		OBJ_STAMP = F$CVTIME(OBJ_STAMP)
$	IF C_STAMP .LES. OBJ_STAMP THEN -
		IF F$MODE() .EQS. "INTERACTIVE" THEN -
			WRITE SYS$OUTPUT "''LEADER'.OBJ is up-to-date"
$	IF C_STAMP .LES. OBJ_STAMP THEN -
		GOTO NEWLINE
$	WRITE SYS$OUTPUT "{",NAME,"}"
$	SET NOON	! Ignore errors
$	DEFINE/USER SYS SYS$LIBRARY	! so #include <sys/foo.h> works
$	CC /NOLIST/DEBUG=(SYMBOLS,TRACEBACK) 'P2' 'NAME' /OBJECT='LEADER'.OBJ
$	SET ON		! Restore error checking
$	GOTO NEWLINE
$ !
$ DONE:
$	CLOSE DIRFILE/ERROR=NOTOPEN
$ !
$ NOTOPEN:
$	SET FILE /PROTECTION=(W:REWD) DIR.TMP;*
$	DELETE DIR.TMP;*
$ !
$ GOTO EXIT
$ !
$ DIRECTORY_ERROR:
$	WRITE SYS$OUTPUT "Error: ''F$MESSAGE()'"
$	SET FILE /PROTECTION=(W:REWD) DIR.TMP;*
$	DELETE DIR.TMP;*
$ !
$ EXIT:
$ IF SAVE_VERIFY THEN SET VERIFY
$ EXIT


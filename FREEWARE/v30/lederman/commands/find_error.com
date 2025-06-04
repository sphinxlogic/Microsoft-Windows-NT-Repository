$! Try all of the message files to look for the translation
$! of an error message
$!
$ ON CONTROL_Y    THEN $ GOTO exitout
$ ON SEVERE_ERROR THEN $ CONTINUE
$!
$ IF F$MODE() .EQS. "BATCH" THEN SET PROCESS /NAME = "Get Message"
$!
$ IF P1 .EQS. ""
$ THEN
$    WRITE SYS$OUTPUT "Can't do much without an error number"
$    EXIT
$ ELSE
$    errno = P1
$ ENDIF
$!
$ bigloop:
$!
$ msgfile = F$SEARCH ("SYS$MESSAGE:*.EXE")
$!
$ IF msgfile .EQS. "" THEN $ GOTO exitout
$!
$ SET MESSAGE 'msgfile'
$!
$ msgtext = F$MESSAGE (errno)
$ noname  = F$EXTRACT (0, 7, msgtext)
$!
$ IF noname .NES. "%NONAME"
$ THEN
$    WRITE SYS$OUTPUT msgfile
$    WRITE SYS$OUTPUT msgtext
$ ENDIF
$!
$ GOTO bigloop
$!
$ exitout:
$!
$ EXIT $STATUS
$!
$!
$! B. Z. Lederman

$ sv = 'f$verify(0)
$!
$! LINK.COM
$!   Creates KILL.EXE from object code.
$!
$ if f$getsyi("HW_MODEL") .ge. 1024
$ then	OBJ = "ALPHA_OBJ"
$ else	OBJ = "OBJ"
$ ENDIF
$!
$ set verify
$ LINK/EXEC=KILL.EXE/NOTRACE KILL.'OBJ',KILL_CLD.'OBJ',KILL_MSG.'OBJ'
$!'f$verify(0)
$ exit 1 .or. f$verify(sv)

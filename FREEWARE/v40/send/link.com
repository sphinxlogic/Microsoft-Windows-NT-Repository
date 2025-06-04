$ sv = 'f$verify(0)
$!
$! LINK.COM
$!   Creates SEND_DECNET.EXE and SEND_SERVER.EXE from object code.
$!
$ if f$getsyi("HW_MODEL") .ge. 1024
$ then	OBJ = "ALPHA_OBJ"
$ else	OBJ = "OBJ"
$ ENDIF
$!
$ set verify
$ LINK/EXEC=SEND_DECNET.EXE/NOTRACE SEND_DECNET.'OBJ',SEND_COMMON.'OBJ',-
				SEND_CLD.'OBJ',SEND_MSG.'OBJ'
$ LINK/EXEC=SEND_SERVER.EXE/NOTRACE SEND_SERVER.'OBJ',SEND_COMMON.'OBJ'
$!'f$verify(0)
$ exit 1 .or. f$verify(sv)

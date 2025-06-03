$ sv = 'f$verify(0)
$!
$! LINK.COM
$!   Creates MGBOOK.EXE from the object code.
$!
$ if f$getsyi("HW_MODEL") .ge. 1024
$ then	OLB = "ALPHA_OLB"
$ else	OLB = "OLB"
$ ENDIF
$!
$ set verify
$ LINK/EXEC=MGBOOK.EXE/NOTRACE MGBOOK.'OLB'/LIBRARY/INCLUDE=MGBOOK,-
	MGBOOK.VERSION/OPT
$!'f$verify(0)
$ exit 1 .or. f$verify(sv)

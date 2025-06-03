$ sv = 'f$verify(0)'
$ axp = f$getsyi("HW_MODEL").ge.1024
$ bliss = "BLISS32EN"
$ if axp
$ then	olb = ".ALPHA_OLB"
$ else	olb = ".OLB"
$ endif
$ set verify
$ LINK/NOTRACE/EXEC=NOTICE.EXE NOTICE'OLB'/INCLUDE=NOTICE/LIBRARY
$ exit 1.or.'f$verify(sv)'

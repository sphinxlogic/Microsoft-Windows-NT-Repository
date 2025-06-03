$ sv = 'f$verify(0)'
$ axp = f$getsyi("HW_MODEL").ge.1024
$ if axp
$ then	olb = ".ALPHA_OLB"
$	sysexe = "/SYSEXE"
$	opt = ".ALPHA_OPT"
$ else	olb = ".OLB"
$	sysexe = ""
$	opt = ".OPT"
$ endif
$ set verify
$ LINK/NOTRACE/EXEC=GETCMD.EXE'SYSEXE' GETCMD'OLB'/INCLUDE=GETCMD/LIBRARY,-
	GETCMD'OPT'/OPT
$ exit 1.or.'f$verify(sv)'

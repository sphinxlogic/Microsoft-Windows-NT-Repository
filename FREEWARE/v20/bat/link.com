$ sv = 'f$verify(0)'
$ axp = f$getsyi("HW_MODEL").ge.1024
$ if axp
$ then	olb = "ALPHA_OLB"
$ else	olb = "OLB"
$ endif
$ write sys$output "Linking BAT...."
$ link/notrace/exec=bat.exe bat.'olb'/include=bat/library
$ exit 1.or.f$verify(sv)

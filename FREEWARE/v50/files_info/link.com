$ axp = f$getsyi("HW_MODEL").ge.1024
$ if axp
$ then	olb = "ALPHA_OLB"
$	opt = "ALPHA_OPT"
$	sysexe = "/SYSEXE"
$ else	olb = "OLB"
$	opt = "OPT"
$	sysexe = ",VAXCRTL.OPT/OPTIONS,SYS$SYSTEM:SYS.STB/SEL"
$ endif
$ link/notrace files_info.'olb'/includ=files_info/library,-
	files_info.'opt/opt'sysexe'
$ exit

$ axp = f$getsyi("HW_MODEL").ge.1024
$ if axp
$ then	olb = ".ALPHA_OLB"
$ else	olb = ".OLB"
$ endif
$ link/notrace/exec=cvtlis.exe cvtlis'olb'/include=cvtlis/library
$ exit

$ sv = 'f$verify(0)'
$ say := write sys$output
$ axp = f$getsyi("HW_MODEL").ge.1024
$ if axp
$ then	obj = ".ALPHA_OBJ"
$ else	obj = ".OBJ"
$ endif
$ say "Linking Xautolock...."
$ link/notrace/exec=xautolock.exe xautolock'obj',wait3'obj',xautolock.opt/opt
$ exit

$ write sys$output "Linking SD...."
$ if f$getsyi("HW_MODEL").ge.1024
$ then	link/notrace sd.alpha_obj
$ else	link/notrace sd.obj
$ endif
$ exit

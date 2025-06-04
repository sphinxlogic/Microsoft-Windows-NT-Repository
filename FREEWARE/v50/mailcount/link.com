$ axp = f$getsyi("HW_MODEL").ge.1024
$ write sys$output "Linking MAILCOUNT...."
$ if axp
$ then
$	link/notrace/exec=mailcount.exe mailcount.alpha_obj,getred.alpha_obj,-
		fparse.alpha_obj
$ else
$	link/notrace/exec=mailcount.exe mailcount.obj,sys$input/options
	getred.obj
	fparse.obj
	sys$share:vaxcrtl.exe/share
$ endif

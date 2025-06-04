$ alpha = f$getsyi("HW_MODEL").ge.1024
$ write sys$output "Linking RCARD...."
$ if alpha
$ then	link/notrace/exec=rcard.exe [.bin-alpha]rcard.obj
$ else	link/notrace/exec=rcard.exe [.bin-vax]rcard.obj,sys$input/options
	sys$library:vaxcrtl.exe/share
$ endif
$ write sys$output "Linking PCARD...."
$ if alpha
$ then	link/notrace/exec=pcard.exe [.bin-alpha]pcard.obj
$ else	link/notrace/exec=pcard.exe [.bin-vax]pcard.obj,sys$input/options
	sys$library:vaxcrtl.exe/share
$ endif
$ exit

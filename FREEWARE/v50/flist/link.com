$ write sys$output "Linking FLIST...."
$ if f$getsyi("HW_MODEL").ge.1024
$ then	link/notrace/exec=flist.exe [.bin-alpha]flist.obj
$ else	link/notrace/exec=flist.exe [.bin-vax]flist.obj,sys$input/opt
sys$share:vaxcrtl.exe/share
$ endif
$ dotpu := EDIT/TPU/NODISPLAY/NOSECTION/NOJOURNAL/COMMAND=[.source]flist.tpu
$ define/user bindir sys$disk:[]
$ dotpu
$ exit

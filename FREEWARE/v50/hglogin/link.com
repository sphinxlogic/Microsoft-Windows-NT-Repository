$ write sys$output "Linking HGLOGIN...."
$ if f$getsyi("HW_MODEL").ge.1024
$ then	link/notrace/exec=hglogin.exe/sysexe [.bin-alpha]hglogin.olb/incl=hglogin/library,[]version.opt/opt
$ else	link/notrace/exec=hglogin.exe [.bin-vax]hglogin.olb/incl=hglogin/library,-
	[]version.opt/opt,sys$input/opt
sys$system:sys.stb/selective_search
$ endif
$ exit

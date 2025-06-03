$ arch_name= f$getsyi ("arch_name")
$ alpha= arch_name.eqs."Alpha"
$ ad= f$search ("''arch_name'.dir").nes.""
$ dir= f$environment ("default")
$ on warning then goto exit
$ if ad 
$ then	set default [.'arch_name']
$ 	srcfile:= [-.src]nmlphasev_support.c
$	nmllib:= nml.olb
$ else	srcfile:= nmlphasev_support.c
$	nmllib:= nml_'arch_name'.olb
$ endif
$!
$ srcdt= f$cvtime(f$file_attr (srcfile,"RDT"))
$ objfile= f$search (f$parse("[].obj;",srcfile))
$ if objfile.nes.""
$ then make= f$cvtime(f$file_attr(objfile,"RDT")).lts.srcdt
$ else make= 1
$ endif
$ if make then cc 'srcfile'
$!
$ srcfile= f$search (f$parse("[].obj;",srcfile))
$ if srcfile.eqs.""
$ then	write sys$output "Object not built, exiting..."
$	exit
$ endif
$ srcdt= f$cvtime(f$file_attr (srcfile,"RDT"))
$ objfile= f$search ("nmlphasev_nml.exe","RDT")
$ if objfile.nes.""
$ then make= f$cvtime(f$file_attr(objfile,"RDT")).lts.srcdt
$ else make= 1
$ endif
$ if make
$ then if alpha
$	then	link/executable=nmlphasev_nml.exe -
			'srcfile','nmllib'/lib/include=nml$main
$	else	link/executable=nmlphasev_nml.exe -
			'srcfile','nmllib'/lib/include=nml$main,-
				sys$system:sys.stb/selective_search
$	endif
$ endif
$exit: set noon
$ set default 'dir'

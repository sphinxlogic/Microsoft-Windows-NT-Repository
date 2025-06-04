$ alpha = f$getsyi("HW_MODEL").ge.1024
$ if alpha
$ then obj = ".alpha_obj"
$	sysexe = "/SYSEXE"
$ else obj = ".obj"
$	sysexe = ",sys$system:sys.stb/selective_search"
$ endif
$ write sys$output "Linking TQE...."
$ link/notrace tqe'obj',lock_code'obj',lock_code.opt/options'sysexe'
$ exit

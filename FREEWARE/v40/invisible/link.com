$ save_verify = 'f$verify(0)'
$ alpha = f$getsyi("HW_MODEL").ge.1024
$ say := write sys$output
$ say "Linking INVISIBLE...."
$ if alpha
$ then sysexe = ".alpha_obj/sysexe"
$ else sysexe = ".vax_obj,sys$system:sys.stb/selective_search"
$ endif
$ link/notrace/exec=invisible.exe invisible'sysexe'
$ exit f$verify(save_verify).or.1

$ arch= f$getsyi ("Arch_name")
$ alpha= arch.eqs."Alpha"
$ if alpha 
$ then	arch_def= "sys$library:arch_defs.mar+"
$	syslink:= /sysexe
$ else arch_def= ""
$	syslink= ",sys$system:sys.stb/selective_search"
$ endif
$ filename:= syspql
$ macsw= ""
$!
$ if f$search ("''arch'.dir;1").eqs."" then create/dir [.'arch']
$ savdir= f$environment ("default")
$ on warning then goto exit
$ set default [.'arch']
$ call compile cc syspql.c
$ call compile cc setsyspql.c
$ call compile macro syspql_init.mar "''arch_def'"
$ call link syspql "@[-]link"
$ call link setsyspql "" "''syslink'"
$!
$exit:	set noon
$	set default 'savdir'
$ 	exit
$!
$compile: subroutine	!p1: compiler p2:source file p3: switches
$ name= f$parse (p2,,,"NAME")
$ do_comp= f$search ("''name'.OBJ").eqs.""
$ if .not. do_comp then do_comp= -
	f$cvtime(f$file("[-.src]''p2'","RDT")).gts. -
	f$cvtime(f$file("''name'.OBJ","RDT"))
$ if do_comp then 'p1'/object=[]'name' 'p3'sys$disk:[-.src]'p2'
$ endsubroutine
$link: subroutine	!p1: exe file
$ do_link= f$search ("''p1'.EXE").eqs.""
$ name= f$parse (p1,,,"NAME")
$ if .not. do_link then do_link= -
	f$cvtime(f$file("''name'.OBJ","RDT")).gts. -
	f$cvtime(f$file("''name'.EXE","RDT"))
$ if do_link
$ then if p2.eqs.""
$ then link /exec='name'.exe 'name'.obj 'p3'
$ else 'p2'
$ endif
$ endif
$ endsubroutine

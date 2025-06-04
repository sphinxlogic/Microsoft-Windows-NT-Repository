$ arch= f$getsyi ("Arch_name")
$ alpha= arch.eqs."Alpha"
$ sfx=""
$ if alpha 
$ then filename:= sys$killdriver
$	macsw= ""
$	srcfile= filename
$	ver= f$extract (1, 80, f$element (0, ".", f$getsyi("version")))
$	if ver.lts."7"  then filename:= 'filename'_v6
$ else filename:= killdriver
$	srcfile= filename
$	macsw= ""
$ endif
$ if f$search ("''arch'.dir;1").eqs."" then create/dir [.'arch']
$ savdir= f$environment ("default")
$ on warning then goto exit
$ set default [.'arch']
$ call compile macro 'srcfile'.mar /object='filename'.obj 'macsw'
$ call compile macro kill.mar
$ call link kill
$ call compile pascal setload.pas
$ call link setload
$ call compile pascal setunload.pas
$ call link setunload
$ do_mac= f$search ("''filename'.EXE").eqs.""
$ if .not.do_mac then do_mac= -
     f$cvtime(f$file("''filename'.OBJ","RDT")).gts. -
     f$cvtime(f$file("''filename'.EXE","RDT"))
$ if do_mac
$ then	if alpha
$	then 	link/share='filename'.exe/sysexe/notrace -
		/map='filename'/full/cross/symbol_table='filename' -
		/section/userlib=proc/native_only/replace/nodemand_zero -
		/NOSYSSHR sys$input:/option
cluster=vmsdriver,,,-
	sys$killdriver.obj , -
	sys$share:VMS$VOLATILE_PRIVATE_INTERFACES/lib/incl=(BUGCHECK_CODES), -
	sys$share:Starlet/include=(SYS$DRIVER_INIT,SYS$DOINIT)
!
!!!PSECT_ATTR=$$$115_driver,wrt
collect=nonpaged_execute_psects/attributes=resident, -
	$$$115_driver
!
PSECT_ATTR=$$$105_prologue,nopic
PSECT_ATTR=$$$110_data,nopic
PSECT_ATTR=$$$115_linkage,wrt,wrt
!!!PSECT_ATTR=$$$105_prologue,exe
!!!PSECT_ATTR=$$$110_data,exe
!!!PSECT_ATTR=$$$115_linkage,wrt,pic,exe
collect=nonpaged_readwrite_psects/attributes=resident, -
	$$$105_prologue,-
	$$$110_data,-
	$$$115_linkage
!
!	ramasse les fixup vectors
!
psect_attr= EXEC$INIT_CODE, NOSHR
collect=initialization_psects/attributes=initialization_code, -
	EXEC$INIT_000, -
	EXEC$INIT_001, -
	EXEC$INIT_002, -
	EXEC$INIT_CODE, -
	EXEC$INIT_LINKAGE, -
	EXEC$INIT_SSTBL_000, -
	EXEC$INIT_SSTBL_001, -
	EXEC$INIT_SSTBL_002
$!
$ 	else	link/share/notrace/nodebug 'filename'.obj
$	endif
$ endif
$ exe= f$parse("''filename'.EXE")
$ define/user killdriver 'exe'
$ if alpha
$ then	mc sysman io connect kill0/driver=killdriver/noadapter/log
$ else	mc sysgen    connect kill0/driver=killdriver/noadapter
$ endif
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
$ if do_comp then 'p1' [-.src]'p2' 'p3'
$ endsubroutine
$link: subroutine	!p1: exe file
$ do_link= f$search ("''p1'.EXE").eqs.""
$ name= f$parse (p1,,,"NAME")
$ if .not. do_link then do_link= -
	f$cvtime(f$file("''name'.OBJ","RDT")).gts. -
	f$cvtime(f$file("''name'.EXE","RDT"))
$ if do_link then link /exec='name'.exe 'name'.obj
$ endsubroutine

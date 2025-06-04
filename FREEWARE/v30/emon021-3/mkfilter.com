$!
$!	P1: fichiers a recompiler
$!	P2: force si recompilation obligatoire
$!	P3: 1 si debug
$!
$ set noon
$ var= f$parse (f$element (0,"/",p1),,,"NAME")'"
$ if var.eqs."" then var= "*"
$ savdir= f$environment ("default")
$ on warning then goto exit
$ on control_y then goto exit
$ proc= f$environment ("procedure")
$ debug= p3
$ if debug
$ then	ccdbgsw:= /noop/debug
$	linkdbgsw:= /debug
$ else	ccdbgsw= ""
$	linkdbgsw= ""
$ endif
$ set default 'f$parse (proc,,,"device")''f$parse(proc,,,"DIRECTORY")'
$!
$ search= "[.src]EMON$FILTER_''var'"
$ opt= p1 - f$element (0,"/",p1)
$ oldsearch=""
$ set noon
$ alpha= f$getsyi ("hw_model").ge.1024
$ arch= f$getsyi ("Arch_name")
$ if .not. $status then arch:= vax
$ set on
$ if arch.eqs."Alpha" 
$ then decc=1
$ else decc= f$search ("sys$system:decc$compiler.exe").nes.""
$ endif
$ if decc then cc:= cc/standard=vaxc/extern=common
$ if f$search ("''arch'.dir;1").eqs."" then create/dir [.'arch']/log
$!
$b: file= f$search ("''search'.c", 1)
$ if file.eqs."" then goto done
$ if file.eqs.oldsearch then goto done
$ oldsearch = file
$ filter_name= f$parse (file,,,"NAME")
$ if f$search ("[.''arch']''filter_name'.OBJ").nes."" then -
     if	f$cvtime(f$file ("[.''arch']''filter_name'.OBJ","RDT")).ges. -
	f$cvtime(f$file ("[.src]''filter_name'.C","RDT")) then -
	   if p2.nes."FORCE" then goto cc_done
$ cc [.src]'filter_name'.c'opt'/obj=[.'arch']'ccdbgsw'
$!
$cc_done:
$ if f$search ("[.''arch']''filter_name'.EXE").nes."" then -
     if	f$cvtime(f$file ("[.''arch']''filter_name'.EXE","RDT")).ges. -
	f$cvtime(f$file ("[.''arch']''filter_name'.OBJ","RDT")) then goto link_done
$ ln:= link /share=[.'arch']'filter_name'.EXE [.'arch']'filter_name'.OBJ'opt',sys$input/opt
$ define/user emon sys$disk:[.'arch']emon.exe
$ if alpha
$ then ln 'linkdbgsw'
emon/share
psect_attr= $CHAR_STRING_CONSTANTS,NOWRT
symbol_vector= (-
	emon$init=procedure,-
	emon$analyze=procedure,-
	emon$cleanup=procedure)
gsmatch = always,1,0	!	appel par LIB$FIND_IMAGE_SYMBOL
$ else ln 'linkdbgsw'
emon/share
psect_attr= $CHAR_STRING_CONSTANTS,NOWRT
universal= emon$init
universal= emon$analyze
universal= emon$cleanup
gsmatch = always,1,0	!	appel par LIB$FIND_IMAGE_SYMBOL
$ endif
$!
$link_done:
$ exe= f$search ("[.''arch']''filter_name'.exe")
$ define/nolog/job 'filter_name' 'f$element(0,";",exe)'
$ goto b
$done:
$ emonimg:= [.'arch']emon.exe
$ define/nolog/job emon 'f$element (0,";",f$search (emonimg))'  !pour debug
$ emondbgimg:= [.'arch']emon_dbg.exe
$ define/nolog/job emon_dbg 'f$element (0,";",f$search (emondbgimg))'  !pour debug
$!
$exit:
$ set default	'savdir'
$ exit

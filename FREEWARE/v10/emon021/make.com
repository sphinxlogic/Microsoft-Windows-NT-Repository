$!
$!	compilateurs testes: 
$!
$!	decc	vax 4.0, alpha 1.3
$!	vaxc
$!
$ proc= f$environment("procedure")
$ dir= f$environment("default")
$ if f$locate ("[",proc).ne.f$length(proc) 
$ then 	set default 'f$element (0, "]",proc )']
$ else	set default 'f$element (0, ">",proc )'>
$ endif
$ arch= f$getsyi ("ARCH_NAME")
$ alpha= arch.eqs."Alpha"
$ if f$search ("''arch'.dir;1").eqs."" then create/directory [.'arch']
$ decc= f$search ("SYS$SYSTEM:DECC$COMPILER.EXE").nes."" .or. alpha
$!
$ if decc
$ then macro_decc= -
",CFLAGS=""/standard=RELAXED_ANSI89/extern_model=COMMON/prefix=all"""
$      define/nolog decc$user_include <->,c$include:
$ else macro_decc=",VAXC=1"
$ endif
$!
$ if alpha
$ then macro_alpha= ",Alpha=1"
$ else macro_alpha= ",VAX=1"
$ endif
$!
$ set def [.'arch']
$ set noon
$ del/sym alpha	! nasty bug
$ on control_y then goto abend
$ mms/macro=(arch='arch' 'macro_decc' 'macro_alpha')/descrip=[-]descrip.mms
$abend:
$ st= $status
$ set noon
$ on control_y then continue
$ set default 'dir'
$ exit 'st'

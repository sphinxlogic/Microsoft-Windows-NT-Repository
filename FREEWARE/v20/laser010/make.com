$ dir= f$environment ("default")
$ proc= f$environment ("procedure")
$ on control_y then goto razdef
$ set noon
$ alpha= f$getsyi ("HW_MODEL").ge.1024
$ if alpha
$ then arch:= alpha
$ else arch:= vax
$ endif
$! arch= f$getsyi ("arch_name")
$ decc= f$search ("SYS$SYSTEM:DECC$COMPILER.EXE").nes.""
$ if decc
$ then mmsdefine= "''arch'=1,decc=1,cflags=""/PREFIX=ALL_ENTRIES"""
$ else mmsdefine:= 'arch'=1
$ endif
$!
$ if f$search ("''arch'.dir;1").eqs."" then create/dir [.'arch']
$ set default 'f$parse (proc,,,"device")''f$parse (proc,,,"directory")'
$ set default [.'arch']
$ delete/symbol arch
$ delete/symbol alpha
$ delete/symbol decc
$ mms/description=[-]descrip.mms/macro=('mmsdefine')
$razdef:
$ set default 'dir'

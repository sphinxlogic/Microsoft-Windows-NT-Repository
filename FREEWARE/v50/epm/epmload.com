$ sv = 'f$verify(0)'
$ alpha = f$getsyi("HW_MODEL").ge.1024
$ curproc = f$environment("PROCEDURE")
$ devdir = f$parse(curproc,"","","DEVICE")+f$parse(curproc,"","","DIRECTORY")
$ if alpha
$ then
$	mcr sysman io connect epma0:/noadapter/driver='devdir'epmdriver.exe
$ else
$	mcr sysgen connect epma0:/noadapter/driver='devdir'epmdriver.exe
$ endif
$ write sys$output "EPMDRIVER loaded"
$ exit 1.or.f$verify(sv)

$ ON ERROR THEN CONTINUE
$ ON CONTROL_Y THEN CONTINUE
$ write sys$output "Press Control-Y to exit"
$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq. 2
$ then
$ xfish := $'xdev''xdir'xfish
$ else
$ xfish := $'xdev''xdir'xfish_vax
$ endif
$ xfish 
$ done:

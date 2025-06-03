$ on control_y then goto done
$ write sys$output "Press Control-Y to exit"
$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq. 2
$ then
$ xtga := $'xdev''xdir'xtga
$ else
$ xtga := $'xdev''xdir'xtga_vax
$ endif
$ xtga 'xdev''xdir'test.tga
$ done:
$ exit

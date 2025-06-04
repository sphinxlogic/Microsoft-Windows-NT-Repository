$ on control_y then goto done
$ write sys$output "Press Control-Y to exit"
$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq. 2
$ then
$ xgif := $'xdev''xdir'xgif
$ else
$ xgif := $'xdev''xdir'xgif_vax
$ endif
$ xgif := $'xdev''xdir'xgif
$ xgif 'xdev''xdir'marble_jar.gif
$ done:
$ exit

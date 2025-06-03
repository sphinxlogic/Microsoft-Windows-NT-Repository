$ on control_y then goto done
$ write sys$output "Press Control-Y to exit"
$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq. 2
$ then
$ xanim := $'xdev''xdir'xanim_alpha
$ else
$ xanim := $'xdev''xdir'xanim_vax
$ endif
$ xanim 'xdev''xdir'planetsidedemo.anim
$ done:
$ exit

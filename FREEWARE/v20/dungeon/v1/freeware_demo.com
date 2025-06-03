$ on control_y then goto done
$ a = f$env("procedure")
$ dev = F$PARSE(a,,,"DEVICE") + F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq. 2
$ then
$ dungeon :== $'dev'dungeon_tv
$ else
$ dungeon :== $'dev'dungeon
$ endif
$ set noverify
$ define/nolog dindx 'dev'dindx.dat
$ define/nolog dtext 'dev'dtext.dat
$ define/nolog sys$input sys$command
$ dungeon
$ deassign dindx
$ deassign dtext
$ deassign sys$input
$ done:
$ exit

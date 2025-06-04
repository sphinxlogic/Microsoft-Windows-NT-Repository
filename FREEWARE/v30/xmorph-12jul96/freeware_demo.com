$ on control_y then goto done
$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$if arch .eq. 2
$  then
$    cpu = "AXP"
$  else
$    cpu = "VAX"
$ endif
$ xmorph :== $'xdev''xdir'xmorph_'cpu'
$ xmorph -start 'xdev''xdir'alice.tga -finish 'xdev''xdir'alex.tga
$ done:
$ exit

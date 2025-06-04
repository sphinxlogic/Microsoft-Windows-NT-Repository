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
$ xboing :== $'xdev''xdir'xboing23_'cpu'
$ define/user XBOING23_DIR 'xdev''xdir'
$if arch .eq. 2
$  then
$    xboing -sync -speed 1
$  else
$    xboing -speed 5
$ endif
$ done:
$ exit

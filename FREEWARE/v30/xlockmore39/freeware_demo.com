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
$ xlock :== $'xdev''xdir'xlock_'cpu'
$ xlock -nolock -mode flag
$ xlock -nolock -mode forest
$ xlock -nolock -mode eyes
$ xlock -nolock -mode clock
$ xlock -nolock -mode grav
$ xlock -nolock -mode kaleid
$ xlock -nolock -mode laser
$ xlock -nolock -mode bouboule
$ xlock -nolock -mode slip
$ xlock -nolock -mode flag
$ done:
$ exit

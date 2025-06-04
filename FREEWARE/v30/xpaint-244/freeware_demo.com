$ on control_y then goto done
$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$if arch .eq. 2
$  then
$    cpu = "AXP"
$  else
$ write sys$output "This demo is built for Alpha only"
$ wait 0:0:0.8
$ exit
$ endif
$ xpaint :== $'xdev''xdir'xpaint_'cpu'
$ xpaint 'xdev''xdir'egypt1.jpg
$ done:
$ exit

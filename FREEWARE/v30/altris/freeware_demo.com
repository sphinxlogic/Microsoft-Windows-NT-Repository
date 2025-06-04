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
$ altetris :== $'xdev''xdir'altetris_'cpu'
$ hextris  :== $'xdev''xdir'hextris_'cpu'
$ tertris  :== $'xdev''xdir'tertris_'cpu'
$ welltris :== $'xdev''xdir'welltris_'cpu'
$!
$ altetris
$ wait 00:00:01
$ hextris
$ wait 00:00:01
$ tertris
$ wait 00:00:01
$ welltris
$ done:
$ exit

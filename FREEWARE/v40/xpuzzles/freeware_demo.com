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
$ xcubes :== $'xdev''xdir'xcubes_'cpu'
$ xdino :== $'xdev''xdir'xdino_'cpu'
$ xhexagons :== $'xdev''xdir'xhexagons_'cpu'
$ xmball :== $'xdev''xdir'xmball_'cpu'
$ xmlink :== $'xdev''xdir'xmlink_'cpu'
$ xoct :== $'xdev''xdir'xoct_'cpu'
$ xpyraminx :== $'xdev''xdir'xpyraminx_'cpu'
$ xrubik :== $'xdev''xdir'xrubik_'cpu'
$ xskewb :== $'xdev''xdir'xskewb_'cpu'
$ xtriangles :== $'xdev''xdir'xtriangles_'cpu'
$!
$ xcubes
$ wait 00:00:01
$ xdino
$ wait 00:00:01
$ xhexagons
$ wait 00:00:01
$ xmball
$ wait 00:00:01
$ xmlink
$ wait 00:00:01
$ xoct
$ wait 00:00:01
$ xpyraminx
$ wait 00:00:01
$ xrubik
$ wait 00:00:01
$ xskewb
$ wait 00:00:01
$ xtriangles
$ done:
$ exit

$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq.2
$ then
$ximq :== $'xdev''xdir'ximq
$ else
$ximq :== $'xdev''xdir'ximq_vax
$ endif
$ ! XIMQ$ must normally be defined, but for demo purposes, we
$ ! will just display a single image from a NASA CD that is in
$ ! this kit.
$ximq -file 'xdev''xdir'C4384655.IMQ

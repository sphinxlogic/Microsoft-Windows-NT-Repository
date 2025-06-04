$ on control_y then goto done
$ ! run povray, buffer a lot, use test.pov as input, generate test.tga as
$ ! output, make the height and width 500x500, display the preview, be noisy
$ a = f$env("procedure")
$ dev = F$PARSE(a,,,"DEVICE") + F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$ if arch .eq. 2
$ then
$ povray :== $'dev'povray
$ else
$ povray :== $'dev'povray_vax
$ endif
$ povray +b10000 +i'dev'test.pov +o'dev'test.tga +h500 +w500 +d +V +P +l'dev'
$ done:
$ exit

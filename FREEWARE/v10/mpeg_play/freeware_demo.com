$ on control_y then goto done
$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$ if arch .ne. 2
$ then
$ write sys$output "This demo is built for Alpha only"
$ wait 0:0:0.10
$ exit
$ endif
$ mpeg_play := $'xdev''xdir'mpeg_play
$ mpeg_play -shmem_off 'xdev''xdir'ftball.mpg -loop -quiet
$ done:
$ exit

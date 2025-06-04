$ ! Procedure: BUILD_SCANLOCKS.COM
$ set noverify
$ set noon
$ say = "write sys$output"
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ if (vax)
$   then
$   arch_name = "VAX"
$   linkkk = "link/notrace"
$ else
$ arch_name = "ALPHA"
$ linkkk = "link/notrace/nonative_only"
$ endif !(vax)
$ linkkk SCAN_FILE_LOCKS.'arch_name'_OBJ,sys$input:/options
dbslibrary:dbssysrtl/lib
$ exitt 1

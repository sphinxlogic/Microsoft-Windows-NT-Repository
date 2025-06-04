$!
$ here = f$parse("Z.Z;",f$environment("Procedure")) - "Z.Z;"
$ ddev = f$parse(here,,,"device")
$ if (f$trnlnm(ddev-":") .nes. "") then ddev = f$trnlnm(ddev-":")
$ ddir = f$parse(here,,,"directory")
$ here = ddev+ddir - "][" - "000000." - ".000000"
$!
$ set default 'here'
$ DEFINE CRINOIDLOG_FILE  "''HERE'CRINOID.LOG"
$!
$ debug = f$trnlnm("CRINOIDLOG_DEBUG")
$ flag = "/nodebug"
$ if debug .nes. ""
$ then
$     flag = "/debug"
$     set display/create/transp=tcpip/node='debug'
$     f = f$search("CRINOID_home:dbg$logger.ini")
$     if f .nes. ""
$     then
$         define dbg$init   CRINOID_home:dbg$logger.ini
$     endif
$ endif
$!
$ ext = ".EXE"
$ if f$getsyi("ARCH_NAME") .nes. "VAX" then ext = ".AXE"
$ SET PROC/NAME="CRINOIDLOG"
$ SET MESSAGE 'HERE'CRINOID_MSG'EXT'
$ run'flag' logger'ext'
$!

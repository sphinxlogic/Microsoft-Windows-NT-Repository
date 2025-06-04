$! 'f$verify(0)
$ here = f$parse("Z.Z;",f$environment("Procedure")) - "Z.Z;"
$ ddev = f$parse(here,,,"device")
$ if (f$trnlnm(ddev-":") .nes. "") then ddev = f$trnlnm(ddev-":")
$ ddir = f$parse(here,,,"directory")
$ here = ddev+ddir - "][" - "000000." - ".000000"
$!
$ ext = ".EXE"
$ if f$getsyi("ARCH_NAME") .nes. "VAX" then ext = ".AXE"
$ SET MESSAGE 'HERE'CRINOID_MSG'EXT'
$!
$ run/nodebug 'here'stub'ext'
$ stop/id=0

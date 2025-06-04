$!
$!  shut down CRINOID
$!
$!      @SHUTDOWN           shuts down CRINOID, any STUBs hanging around
$!      @SHUTDOWN LOGGER    shuts down CRINOIDLOG
$!      @SHUTDOWN DEINSTALL shuts down CRINOID, STUBSs, deinstalls
$!      @SHUTDOWN FULL      =LOGGER+DEINSTALL
$!
$!
$ oprv = f$setprv("WORLD,CMKRNL,SYSNAM")
$ if .not. f$privilege("WORLD") .or. .not. f$privilege("CMKRNL") .or. .not. f$privilege("SYSNAM")
$ then
$   write sys$output "CRINOID shutdown.com needs WORLD,CMKRNL,SYSNAM privs"
$   EXIT 44
$ ENDIF
$!
$ here = f$parse("Z.Z;",f$environment("Procedure")) - "Z.Z;"
$ ddev = f$parse(here,,,"device")
$ if (f$trnlnm(ddev-":") .nes. "") then ddev = f$trnlnm(ddev-":")
$ ddir = f$parse(here,,,"directory")
$ here = ddev+ddir - "][" - "000000." - ".000000"
$ if p1 .eqs. "LOGGER"
$ then
$   call shutdown_logger
$   exit
$ endif
$!
$ x = f$search(here+"CRINOID.USER")
$ if x .eqs. ""
$ then
$   write sys$output "CRINOID SHUTDOWN -- ERROR: CRINOID.USER file not found!"
$   x = f$setprv(oprv)
$   exit
$ endif
$ open/read fd 'x
$ read fd user
$ read fd node
$ read fd pname
$ close fd
$ user = f$edit(user,"trim,upcase")
$ node = f$edit(node,"trim,upcase")
$!
$ ctx = ""
$ x = f$context("PROCESS",ctx,"NODENAME",node,"EQL")
$ x = f$context("PROCESS",ctx,"USERNAME",user,"EQL")
$ pidlist = ""
$ loop:
$   pid = f$pid(ctx)
$   if pid .eqs. "" then goto eloop
$   name = ""
$   name = f$getjpi(pid,"PRCNAM")
$   if name .eqs. pname
$   then
$       write sys$output "Stopping ''pname' (PID:''pid')"
$       stop/id='pid'
$       goto loop
$   endif
$   if name .eqs. "CRINOIDLOG" .and. p1 .eqs. "FULL"
$   then
$       call shutdown_logger 'pid'
$   endif
$   if f$extract(0,4,name) .eqs. "STUB"
$   then
$       write sys$output "Stopping Zombie'd STUB (PID:''pid')"
$       stop/id='pid'
$       goto loop
$   endif
$ got loop
$ eloop:
$!
$!  remove any installed images
$!
$ if p1 .eqs. "FULL" .or. p1 .eqs. "DEINSTALL"
$ then
$   ext = ".EXE"
$   if f$getsyi("ARCH_NAME") .nes. "VAX" then ext = ".AXE"
$   call remove_image 'here'CRINOID'ext'
$   call remove_image 'here'LOGGER'ext'
$   call remove_image 'here'STUB'ext'
$ endif
$ x = f$setprv(oprv)
$ exit
$!
$ remove_image: subroutine
$   if .not. f$file(p1,"KNOWN") then exit 1
$   write sys$output "deinstalling ''p1'"
$   x = "install"
$   x remove 'p1'
$   exit
$ endsubroutine
$!
$!  shut down the logger, nicely if possible
$!
$ shutdown_logger: subroutine
$   pid = p1
$   if f$trnlnm("CRINOIDLOG_MBX") .eqs. ""
$   then
$       write sys$output "CRINOIDLOG not running."
$       exit
$   endif
$   write sys$output "Trying for nice shutdown of CRINOIDLOG"
$   define/system crinoidlog_command stop
$   j = 12
$   wait 00:00:01
$   loop2:
$      if f$trnlnm("CRINOIDLOG_COMMAND") .eqs. "" then goto eloop2
$      j = j - 1
$      if j .eq. 0 then goto eloop2a
$      wait 00:00:05
$   goto loop2
$!  ---not responding---
$   eloop2a:
$   deass/system crinoidlog_command
$   if (pid .nes. "")
$   then
$       write sys$output "Stopping CRINOIDLOG (PID:''pid')"
$       stop/id='pid'
$   endif
$   eloop2:
$   exit
$ endsubroutine

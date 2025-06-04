$!^^^^^^^^^^installation procedure should insert definitions of
$!          CRINOID_USERNAME, CRINOID_MGR,  CRINOID_SERVICE, above.
$!
$!  installation of CRINOID images, optionally start the server
$!  put this file in the CRINOID_home directory
$!
$!  @STARTUP              !! installs/replaces program images
$!  @STARTUP RUN          !! also starts CRINOID with faked request
$!
$!---------------------------------------------------------------
$!
$!  check privs
$!
$ op = f$setprv("SYSNAM,SYSPRV,CMKRNL")
$ if .not. f$privilege("SYSNAM,SYSPRV,CMKRNL")
$ then
$   write sys$output "Must have SYSNAM, SYSPRV and CMKRNL to install CRINOID images"
$   x = f$setprv(op)
$   exit 44
$ endif
$!
$!----------------------------------------------------------------------
$!
$ if f$type(CRINOID_manager)       !! prepended during install
$ then
$     DEFINE/SYSTEM CRINOID_MGR "''CRINOID_manager'"
$ endif
$!
$!----------------------------------------------------------------
$!
$!  image installation
$!
$ ext = ".EXE"
$ if f$getsyi("ARCH_NAME") .nes. "VAX" then ext = ".AXE"
$!
$!  get rid of rooted logicals, etc.
$!
$ here = f$parse("Z.Z;",f$environment("Procedure")) - "Z.Z;"
$ ddev = f$parse(here,,,"device")
$ if (f$trnlnm(ddev-":") .nes. "") then ddev = f$trnlnm(ddev-":")
$ ddir = f$parse(here,,,"directory")
$ here = ddev+ddir - "][" - "000000." - ".000000"
$!
$!  as of 7.2? 7.1? need "impersonate" priv to use persona services
$!
$ need_cmkrnl = 0
$ open/read fd stub.opt
$ loop:
$   read/end=eloop fd line
$   line = f$edit(line,"upcase")
$   if f$locate("NEED_CMKRNL",line) .lt. f$len(line) then need_cmkrnl = 1
$ goto loop
$ eloop:
$ close fd
$!-----------------------------------------------------------------
$!
$ stubprivs = "SYSPRV,DETACH"
$ if need_cmkrnl then stubprivs = stubprivs + ",CMKRNL"
$!
$ define/user sys$output nl:
$ define/user sys$error  nl:
$ x = f$privilege("IMPERSONATE")
$ x = f$integer($STATUS) .and. 1
$ type nl:                              !! reset the define/user's
$ if x then stubprivs = subprivs + ",IMPERSONATE"
$!
$!-----------------------------------------------------------------
$ call install_image 'here'CRINOID'ext'  /priv=(LOG_IO,PHY_IO,SYSPRV,SYSNAM,DETACH,WORLD,SYSLCK)
$ call install_image 'here'LOGGER'ext'   /priv=(SYSNAM,SYSLCK)
$ call install_image 'here'STUB'ext'     /priv=('stubprivs')
$!-----------------------------------------------------------------
$!
$!  if requested, start the server
$!
$ if p1 .eqs. "RUN"
$ then
$   IF F$TYPE(CRINOID_USERNAME) .EQS. ""
$   then
$       WRITE SYS$OUTPUT "Can't start CRINOID, don't have USERNAME set...use browser request"
$       goto done
$   endif
$   IF F$TYPE(CRINOID_SERVICE) .EQS. ""
$   then
$       WRITE SYS$OUTPUT "Can't start CRINOID, don't have SERVICE set...use browser request"
$       goto done
$   endif
$   SET MESSAGE 'HERE'CRINOID_MSG'EXT'
$   startup :== $'HERE'STARTUP'ext'
$   write sys$output "Starting CRINOID server..."
$   startup "''crinoid_username'" "''crinoid_service'"
$ endif
$!-----------------------------------------------------------------
$ done:
$ x = f$setprv(op)
$ EXIT
$!
$ install_image: subroutine
$ action = "ADD"
$ if f$file_attributes(P1,"KNOWN") then action = "REPLACE"
$ write sys$output "doing INSTALL ''action' ''p1'"
$ set noon
$ install 'action' 'p1' 'p2' 'p3' 'p4'
$ on error then exit
$ exit
$ endsubroutine

$!
$! CRINOID starter      p1 = process_name
$!
$ here = f$parse("Z.Z;",f$environment("Procedure")) - "Z.Z;"
$ ddev = f$parse(here,,,"device")
$ if (f$trnlnm(ddev-":") .nes. "") then ddev = f$trnlnm(ddev-":")
$ ddir = f$parse(here,,,"directory")
$ here = ddev+ddir - "][" - "000000." - ".000000"
$ root = here - "]" + ".]"
$!
$ pname = "CRINOID"             !! desired name for this process
$ if p1 .nes. "" then pname = p1
$ set noon
$ SET PROC/NAME="''pname'"                  !! try to set
$ nname = f$getjpi("","PRCNAM")             !! did we get it?
$ if f$edit(nname,"trim,upcase") .nes. f$edit(pname,"trim,upcase") then logout
$ on error then exit
$!
$ pid = f$getjpi("","PID")
$ write sys$output "''pname': Pid ''pid'"
$!
$!  debug flags
$!
$ L_MAIN     = %X00000100
$ L_NETIO    = %X00000200
$ L_LOCKING  = %X00000400
$ L_PROC     = %X00000800
$ L_PIPE     = %X00001000
$ L_MBXQ     = %X00002000
$ L_MEM      = %X00004000
$!
$ L_CRITICAL = 0
$ L_ERROR    = 1
$ L_WARNING  = 2
$ L_INFO     = 3
$ L_TRACE    = 4
$ L_BABBLE   = 5
$ L_DEBUG    = 6
$!
$ LOGLEV = L_WARNING
$! LOGLEV = L_MAIN+L_INFO
$! LOGLEV = L_BABBLE
$! LOGLEV = L_DEBUG
$! LOGLEV = L_PROC+L_TRACE
$!
$ DEFINE/TRANS=CONCEAL CRINOID_ROOT       'root'
$ DEFINE               CRINOID_HOME       'here'
$ DEFINE               CRINOID_LOGLEVEL   'loglev'
$ set default CRINOID_HOME:
$ ext = ".EXE"
$ if f$getsyi("ARCH_NAME") .nes. "VAX" then ext = ".AXE"
$!
$ debug = f$trnlnm("CRINOID_DEBUG")
$ flag = "/nodebug"
$ if debug .nes. ""
$ then
$     flag = "/debug"
$     set display/create/transp=tcpip/node='debug'
$     f = f$search("CRINOID_home:dbg$CRINOID.ini")
$     if f .nes. ""
$     then
$         define dbg$init   CRINOID_home:dbg$CRINOID.ini
$     endif
$ endif
$ SET MESSAGE 'HERE'CRINOID_MSG'EXT'
$!
$!  write process information to CRINOID.USER
$!
$ user = f$getjpi("","USERNAME")
$ node = f$getsyi("NODENAME")
$ oname = f$getjpi("","PRCNAM")             !! old name
$ open/write fd CRINOID.USER
$ write fd user
$ write fd node
$ write fd nname
$ close fd
$ purge/keep=1 CRINOID.user
$ rename CRINOID.user; ;1
$ set noon
$!
$!  run CRINOID
$!
$ DEFINE/USER SYS$ERROR CRINOID_HOME:CRINOID_ERR_'PID'.LIS
$ run'flag' 'here'CRINOID'ext'
$ iss = $STATUS
$ set proc/name="''oname'"
$!
$!  if error, report completion status
$!
$ f = f$search("CRINOID_HOME:CRINOID_ERR_''PID'.LIS;")
$ if f .eqs. "" then f = "NL:"
$ if .not.(f$int(iss).and.1 )
$ then
$   request/to=network "''pname' PID:''pid' completion status %x''iss'"
$!
$   if f$trnlnm("CRINOIDLOG_STDERR_MBX") .nes. ""
$   then
$       open/write/share/err=skipit fd CRINOIDlog_stderr_mbx
$       write fd "CRINOID.COM ''pname' PID:''pid' completion status %x''iss'"
$       copy 'F' fd:
$       close fd
$   endif
$ skipit:
$!
$   if f$trnlnm("CRINOID_MGR") .nes. ""
$   then
$       mail 'F' CRINOID_MGR  /subj="CRINOID ''pid' abnormal termination"
$   endif
$ endif
$ IF F .NES. "NL:" THEN DELETE 'F'
$ exit

$!'f$verify(0)
$ call set_process_name
$ here = f$parse("Z.Z;",f$environment("Procedure")) - "Z.Z;"
$ ddev = f$parse(here,,,"device")
$ if (f$trnlnm(ddev-":") .nes. "") then ddev = f$trnlnm(ddev-":")
$ ddir = f$parse(here,,,"directory")
$ here = ddev+ddir - "][" - "000000." - ".000000"
$ root = here - "]" + ".]"
$!
$ L_MAIN     = %X00000100
$ L_NETIO    = %X00000200
$ L_LOCKING  = %X00000400
$ L_PROC     = %X00000800
$ L_PIPE     = %X00001000
$ L_MBXQ     = %X00002000
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
$! LOGLEV = L_BABBLE
$! LOGLEV = L_DEBUG+L_PIPE
$!
$ DEFINE/NOLOG               TENTACLE_LOGLEVEL   'loglev'
$ DEFINE/NOLOG/TRANS=CONCEAL CRINOID_ROOT   'root'
$ DEFINE/NOLOG               CRINOID_HOME   CRINOID_ROOT:[000000]
$ debug = f$trnlnm("CRINOID_TENTACLE_DEBUG")
$ open/read/share fd CRINOID_home:tentacle.opt
$ loop1:
$   read/end=eloop1 fd line
$   line = f$edit(line,"collapse,upcase")
$   if f$element(0,"=",line) .eqs. "!PERL_SETUP"
$   then
$!      avoid logical redefine messages
$       if f$trnlnm("PERL_ROOT","LNM$PROCESS")  .nes. "" then deassign/process perl_root
$       if f$trnlnm("PERLSHR","LNM$PROCESS")    .nes. "" then deassign/process perlshr
$       if f$trnlnm("DBGPERLSHR","LNM$PROCESS") .nes. "" then deassign/process dbgperlshr
$!
$       x = f$element(1,"=",line)
$       if f$search(x) .nes. "" then @'x'
$       goto eloop1
$   endif
$ goto loop1
$ eloop1:
$ close fd
$!
$!  local definition to set up PGPLOT   ... modify to suit
$!
$ if f$getsyi("NODENAME") .eqs. "DUHEP3"
$ then
$    util_setup == "@public$root:[setup]setup/output=NL: "
$    util_setup pgplot520
$ endif
$!
$ flag = "/nodebug"
$ if debug .nes. ""
$ then
$     flag = "/debug"
$     set display/create/transp=tcpip/node='debug'
$     f = f$search("CRINOID_home:dbg$tentacle.ini")
$     if f .nes. ""
$     then
$         define/NOLOG dbg$init   CRINOID_home:dbg$tentacle.ini
$     endif
$ endif
$!
$ ext = ".EXE"
$ if f$getsyi("ARCH_NAME") .nes. "VAX" then ext = ".AXE"
$ SET MESSAGE 'HERE'CRINOID_MSG'EXT'
$ run'flag' CRINOID_home:tentacle'ext'
$ stop/id=0
$!----------------------------------------------------------------------
$!
$!  set process name
$!
$ set_process_name: subroutine
$   MYPID  = F$GETJPI(0,"PID")
$   MYUIC  = F$GETJPI(0,"UIC")
$   MYNAME = F$EDIT(F$GETJPI(0,"USERNAME"),"COLLAPSE,UPCASE")
$   J = 0
$   TRY_NAME:
$     J = J + 1
$     X = F$FAO("!SL",J)
$     Y = F$FAO("!2ZL",J)
$     IF F$LENGTH(X) .GT. F$LENGTH(Y) THEN Y = X
$     NAME = F$EXTRACT(0,15-4-F$LENGTH(Y),MYNAME)
$     NAME = NAME + "/CGI" + Y
$     CONTEXT = ""
$     LOOP:
$         PID = F$PID(CONTEXT)
$         IF PID .EQS. "" THEN GOTO SEARCH_DONE
$         IF PID .EQS. MYPID THEN GOTO LOOP
$         UIC = F$GETJPI(PID,"UIC")
$         IF UIC .NES. MYUIC THEN GOTO LOOP
$         PNAME = F$GETJPI(PID,"PRCNAM")
$         IF PNAME .NES. NAME THEN GOTO LOOP
$     GOTO TRY_NAME
$   SEARCH_DONE:
$   SET MESSAGE/NOFACIL/NOSEVER/NOIDENT/NOTEXT
$   SET NOON
$   SET PROC/NAME="''NAME'"
$   FOO = F$INTEGER($STATUS) .AND. 1
$   ON ERROR THEN EXIT
$   SET MESSAGE/FACIL/SEVER/IDENT/TEXT
$   IF .NOT. FOO THEN GOTO TRY_NAME
$   EXIT 1
$ endsubroutine

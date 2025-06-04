$!
$!  Sets logicals to have CRINOID components run under VMS debugger
$!  ...note that if running in the debugger, the accounts used for
$!  CRINOID must have privs, since we can't INSTALL images with traceback.
$!
$ on error then goto cleanup
$ on control_y then goto cleanup
$ call get_display
$ read/end=cleanup/prompt="Debugging DEC/X-Windows display on node? [''machine']: " sys$command d
$ d = f$edit(d,"collapse,upcase")
$ if d .eqs. "" then d = machine
$ call setdebug "CRINOIDLOG debug?" CRINOIDLOG_DEBUG "''d'"
$ call setdebug "CRINOID debug?" CRINOID_DEBUG "''d'"
$ call setdebug "TENTACLE debug?" CRINOID_TENTACLE_DEBUG "''d'"
$ cleanup:
$ if f$type(machine) .nes. "" then delete/symbol/global machine
$!---------------------------------------------------------------------
$! NOT fully general, but handles UCX and Multinet
$!
$ get_display: subroutine
$   term  = f$getjpi("","terminal")
$   ttdev = f$getdvi(term,"TT_PHYDEVNAM")
$   tttyp = f$extract(1,3,ttdev)
$!
$!  UCX
$!
$ if tttyp .eqs. "TNA"
$ then
$   aa=F$GETDVI(term,"TT_ACCPORNAM")
$   bb=f$element(1," ",aa)
$   machine == bb
$ endif
$!
$!  MultiNet
$!
$ if tttyp .eqs. "NTY"
$ then
$   host  = f$getdvi(term,"TT_ACCPORNAM")-"]"-"["
$   machine == host
$ endif
$!
$!  DECWindows local
$!
$ if tttyp .eqs. "FTA"
$ then
$   machine == f$trnlnm("UCX$INET_HOSTADDR")
$ endif
$ exit
$ endsubroutine
$!
$ setdebug: subroutine  ! prompt logical display
$   def = "N"
$   if f$trnlnm(p2) .nes. "" then def = "Y"
$   q1:
$   read/end=nah/prompt="''p1' [''def']: " sys$command yn
$   yn = f$edit(yn,"collapse,upcase")
$   if yn .eqs. "" then yn = def
$   yn = f$extract(0,1,yn)
$   if yn .eq. "Y"
$   then
$       define/system 'p2' "''p3'"
$       exit
$   endif
$   if yn .eq. "N"
$   then
$       if def .eq. "Y" then deassign/system 'p2'
$       exit
$   endif
$ goto q1
$ nah:
$   exit
$ endsubroutine

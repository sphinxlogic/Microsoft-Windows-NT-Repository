$! File to build RESET_PASSWD executable (Vax or Alpha)
$! Created 24-Jan-1999 17:14 using MKMF V1.07-0227 (Dec C)
$! Logicals/Qualifiers used to create this file:
$!      /LOG=RESET_PASSWD.LOG
$
$ arch = 0
$
$ If f$getsyi("HW_MODEL").ge.1024 then arch = 1
$
$  If f$mode() .eqs. "BATCH"
$   Then
$     If arch
$      Then set def dka100:[lwest.source.tools.reset_passwd] ! Axp host
$      Else set def dka100:[lww.source.tools.reset_passwd]   ! Vax host
$     Endif
$  Endif
$
$  write sys$output f$time()
$
$  If P1 .eqs. "DBG" .or. P1 .eqs. "DEBUG"
$   Then
$     If arch
$      Then mms /macro=("__dbg__=1","__axp__=1") /desc=descrip.mms
$      Else mms /macro=("__dbg__=1") /desc=descrip.mms
$     Endif
$   Else
$     If arch
$      Then mms /macro=("__axp__=1") /desc=descrip.mms
$      Else mms /desc=descrip.mms
$     Endif
$  Endif
$
$  if f$mode() .nes. "BATCH" then write sys$output f$time()
$  exit

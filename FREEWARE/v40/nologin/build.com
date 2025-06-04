$! File to build NOLOGIN executable
$! Created 24-Mar-1998 17:14 using MKMF V1.07-0227 (Dec C)
$! Logicals/Qualifiers used to create this file:
$!      /LOG=NOLOGIN.LOG
$
$  If f$mode() .eqs. "BATCH" 
$   Then
$     If f$getsyi("nodename") .eqs. "VIKING"
$      Then set def DKA100:[LWW2.C.VAXSYS.DEV.NOLOGIN]
$      Else set def DKA100:[LWW.SRC.NOLOGIN]
$     Endif
$  Endif
$
$  write sys$output f$time()
$! P1 = DBG - compile with debug options
$  IF P1 .eqs. "DBG" .or. P1 .eqs. "DEBUG"
$   THEN
$      mms /macro=("dbg=1")
$    ELSE
$      mms 
$  ENDIF
$  exit

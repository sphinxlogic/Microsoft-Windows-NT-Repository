$! File to build MINISM executable
$! Created 28-Feb-1995 20:18 using MKMF V1.07-0227 (Dec C)
$! Logicals/Qualifiers used to create this file:
$!	/EXECUTABLE=MINISM
$!	/LIBFILE=USR$TOOLS:VCCOPT_DECW.OPT
$
$ mms = "MMS"
$ if f$mode() .eqs. "BATCH" then -
$       set def dka100:[LWW.DECW.MOTIF.LWW.MSM]
$
$  write sys$output f$time()
$! P1 = DBG - compile with debug options
$  IF P1 .eqs. "DBG" .or. P1 .eqs. "DEBUG"
$   THEN
$     IF P2 .eqs. "MSMDBG"
$      THEN mms /MACRO=("DBG=1","MSMDBG=1")
$      ELSE mms /MACRO=("DBG=1")
$     ENDIF
$   ELSE        ! not debug
$     IF P1 .eqs. "MSMDBG"
$      THEN mms /MACRO=("MSMDBG=1")
$      ELSE mms
$     ENDIF
$  ENDIF
$  exit

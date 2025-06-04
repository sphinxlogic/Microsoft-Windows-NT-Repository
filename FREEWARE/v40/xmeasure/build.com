$! File to build MEASURE executable
$! Created 02-Nov-1993 23:16 using MKMF V1.04-0716
$! Logicals/Qualifiers used to create this file:
$!	/EXECUTABLE=MEASURE
$!	/LIBFILE=USR$TOOLS:VCCOPT_DECW.OPT
$!	/LOG=MEASURE.LOG
$
$  if f$mode() .eqs. "BATCH" then -
$   set def DKA100:[LWW.DECW.MOTIF.LWW.XMEASURE]
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

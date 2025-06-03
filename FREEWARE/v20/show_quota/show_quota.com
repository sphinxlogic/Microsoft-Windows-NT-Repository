$!----------------------------------------------------------------------------
$! This procedure shows the given quota and the remaining quota of a process. 
$! It uses the process id as first parameter. 
$!
$! Take the results as an idea of how much quota is used. Do not take all 
$! the values for granted because they are updated in a certain interval.
$! Specialy the peak values may differ from what SHOW PROCESS/ACCOUNTING 
$! does give you.
$!
$!
$! Written 1990 by A.Schwarz
$! Softwaresupport Digital Equipment Corporation Switzerland
$!
$! Modified 1992 (with Ideas of Eric)
$! ADDED SYSGEN VALUES (MAX and PQL)
$! ADDED recalculation of vaules after subprocess creatation.(Quota sharing)
$!
$!----------------------------------------------------------------------------
$ waittime := "00:00:02" 	! Time till new values are read and displayed.
$!======================
$!
$ on control_y then gosub set_flag
$ on error then goto end
$ on warning then goto end
$ set broadcast=none
$ flag := false
$ ini_time = F$TIME()
$!
$! User def. symbols
$!
$ IF "''P1'" .EQS. "" THEN P1 = F$GETJPI("","PID")
$ A = """
$ SIMPLE_PID = P1
$ PID = A+SIMPLE_PID+A
$! delete screen
$!===============
$ ESC[0,7]=27
$ WRITE SYS$OUTPUT "''ESC'[2J"
$ WRITE SYS$OUTPUT "''ESC'[0;0H"
$ WRITE SYS$OUTPUT " "
$!
$BASE:
$!
$ FIRST_LOOP := TRUE
$!
$ NAME  	= F$GETJPI('PID,"PRCNAM")
$ ASTLM   	= F$GETJPI('PID,"ASTLM")
$ FILLM 	= F$GETJPI('PID,"FILLM")
$ DIOLM 	= F$GETJPI('PID,"DIOLM")
$ BIOLM	 	= F$GETJPI('PID,"BIOLM")
$ BYTLM 	= F$GETJPI('PID,"BYTLM")
$ TQLM 		= F$GETJPI('PID,"TQLM")
$ ENQLM 	= F$GETJPI('PID,"ENQLM")
$ MODE 		= F$GETJPI('PID,"MODE")
$ PRIB		= F$GETJPI('PID,"PRIB")
$ PRCLM		= F$GETJPI('PID,"PRCLM")
$ LOGINTIM	= F$GETJPI('PID,"LOGINTIM")
$ PGFLQUOTA     = F$GETJPI('PID,"PGFLQUOTA")
$ WSDEFAULT 	= F$GETJPI('PID,"DFWSCNT")
$ WSQUOTA   	= F$GETJPI('PID,"WSQUOTA")
$ WSEXTENT  	= F$GETJPI('PID,"WSEXTENT")
$ WSMAX		= F$GETSYI("WSMAX")
$ PQL_DWSDEFAULT= F$GETSYI("PQL_DWSDEFAULT")
$ PQL_DWSQUOTA	= F$GETSYI("PQL_DWSQUOTA")
$ PQL_DWSEXTENT	= F$GETSYI("PQL_DWSEXTENT")
$ VIRTUALPAGECNT= F$GETSYI("VIRTUALPAGECNT")
$ PRCCNT	= F$GETJPI('PID,"PRCCNT")
$ PRCCNT_INI    = PRCCNT
$!
$LOOP:
$!
$!
$ STATE 	= 		F$GETJPI('PID,"STATE")
$ IMAGE 	= 		F$GETJPI('PID,"IMAGNAME")
$ ASTCNT  	= 'ASTLM - 	F$GETJPI('PID,"ASTCNT")
$ FILCNT 	= 'FILLM - 	F$GETJPI('PID,"FILCNT")
$ DIOCNT 	= 'DIOLM - 	F$GETJPI('PID,"DIOCNT")
$ BIOCNT 	= 'BIOLM - 	F$GETJPI('PID,"BIOCNT")
$ BYTCNT 	= 'BYTLM -      F$GETJPI('PID,"BYTCNT")
$ TQCNT 	= 'TQLM  - 	F$GETJPI('PID,"TQCNT")
$ ENQCNT 	= 'ENQLM -  	F$GETJPI('PID,"ENQCNT")
$ PAGFILCNT	= 'PGFLQUOTA - 	F$GETJPI('PID,"PAGFILCNT")
$ WSSIZE    	=          	F$GETJPI('PID,"WSSIZE")
$ GLOBAL    	= 		F$GETJPI('PID,"GPGCNT")
$ PROCESS   	= 		F$GETJPI('PID,"PPGCNT")
$ FAULTS    	= 		F$GETJPI('PID,"PAGEFLTS")
$ CPUTIM    	= 		F$GETJPI('PID,"CPUTIM")
$ CPU_SEC	= CPUTIM / 100
$ CPU_HUND	= CPUTIM - (CPU_SEC * 100)
$ CPUTIME = "''CPU_SEC'.''CPU_HUND'"
$!
$ IF FIRST_LOOP THEN GOSUB INIT
$!
$! Get max used quotas
$!=====================
$!
$ IF 'ASTCNT    .gt. 'MAX_ASTCNT    THEN MAX_ASTCNT    = 'ASTCNT
$ IF 'FILCNT    .gt. 'MAX_FILCNT    THEN MAX_FILCNT    = 'FILCNT
$ IF 'DIOCNT    .gt. 'MAX_DIOCNT    THEN MAX_DIOCNT    = 'DIOCNT
$ IF 'BIOCNT    .gt. 'MAX_BIOCNT    THEN MAX_BIOCNT    = 'BIOCNT
$ IF 'BYTCNT    .gt. 'MAX_BYTCNT    THEN MAX_BYTCNT    = 'BYTCNT
$ IF 'TQCNT     .gt. 'MAX_TQCNT     THEN MAX_TQCNT     = 'TQCNT
$ IF 'ENQCNT    .gt. 'MAX_ENQCNT    THEN MAX_ENQCNT    = 'ENQCNT
$ IF 'PAGFILCNT .gt. 'MAX_PAGFILCNT THEN MAX_PAGFILCNT = 'PAGFILCNT
$ IF 'WSSIZE    .gt. 'MAX_WSSIZE    THEN MAX_WSSIZE    = 'WSSIZE
$ IF 'GLOBAL    .gt. 'MAX_GLOBAL    THEN MAX_GLOBAL    = 'GLOBAL
$!
$! Print quotas and counts
$!========================
$!
$! WRITE SYS$OUTPUT 
$!
$ TEXT = F$FAO("!ASProcess Name: !AS   State: !AS   Actual time: !AS", -
    "''ESC'[0;0H",NAME, STATE, F$TIME(),"    ")
$ WRITE SYS$OUTPUT TEXT
$ IMAGE	= F$PARSE(IMAGE,,,"NAME")
$!
$ TEXT = F$FAO("Image Name: !AS   PID: !AS Mode: !AS !AS", -
    IMAGE, SIMPLE_PID, MODE,"    ")
$ WRITE SYS$OUTPUT TEXT
$ TEXT = F$FAO("!10AS!AS!AS!30AS!2SL!1AS!2SL", -
          "CpuTime :",CPUTIME," seconds","   SubprocessLimit/Count : ", PRCLM, "/", PRCCNT)
$ WRITE SYS$OUTPUT TEXT
$ WRITE SYS$OUTPUT " "
$ WRITE SYS$OUTPUT "Process Quota Information:"
$ WRITE SYS$OUTPUT -
  "             Quota     Used    (pct.)  MAX_Used since ''ini_time'"
$ PERCENT = ( ASTCNT * 100 / ASTLM ) 
$ TEXT = F$FAO("!10AS!3(8SL)%!8SL", "ASTLM", ASTLM, ASTCNT, PERCENT, MAX_ASTCNT)
$ WRITE SYS$OUTPUT TEXT
$ PERCENT = ( FILCNT * 100 / FILLM )
$ TEXT = F$FAO("!10AS!3(8SL)%!8SL", "FILLM", FILLM, FILCNT, PERCENT, MAX_FILCNT)
$ WRITE SYS$OUTPUT TEXT
$ PERCENT = ( DIOCNT * 100 / DIOLM )
$ TEXT = F$FAO("!10AS!3(8SL)%!8SL", "DIOLM", DIOLM, DIOCNT, PERCENT, MAX_DIOCNT)
$ WRITE SYS$OUTPUT TEXT
$ PERCENT = ( BIOCNT * 100 / BIOLM )
$ TEXT = F$FAO("!10AS!3(8SL)%!8SL", "BIOLM", BIOLM, BIOCNT, PERCENT, MAX_BIOCNT)
$ WRITE SYS$OUTPUT TEXT
$ PERCENT = ( BYTCNT * 100 / BYTLM )
$ TEXT = F$FAO("!10AS!3(8SL)%!8SL", "BYTLM", BYTLM, BYTCNT, PERCENT, MAX_BYTCNT)
$ WRITE SYS$OUTPUT TEXT
$ PERCENT = ( ENQCNT * 100 / ENQLM )
$ TEXT = F$FAO("!10AS!3(8SL)%!8SL", "ENQLM", ENQLM, ENQCNT, PERCENT, MAX_ENQCNT)
$ WRITE SYS$OUTPUT TEXT
$ PERCENT = ( TQCNT * 100 / TQLM )
$ TEXT = F$FAO("!10AS!3(8SL)%!8SL", "TQLM", TQLM, TQCNT, PERCENT,MAX_TQCNT)
$ WRITE SYS$OUTPUT TEXT
$ PERCENT = ( PAGFILCNT * 100 / PGFLQUOTA )
$ TEXT = F$FAO("!10AS!3(8SL)%!8SL!8SL!18AS", -
	 "PGFLQUOTA", PGFLQUOTA, PAGFILCNT,  -  
	  PERCENT,MAX_PAGFILCNT,VIRTUALPAGECNT,"  VIRTUALPAGECNT")
$ WRITE SYS$OUTPUT TEXT
$ WRITE SYS$OUTPUT ""
$ WRITE SYS$OUTPUT "Working Set Information:      "
$ WRITE SYS$OUTPUT "                     Max_size "
$ TEXT = F$FAO("!10AS!8SL!33SL!15AS", -
	"WSEXTENT", WSEXTENT, PQL_DWSEXTENT,"  PQL_DWSEXTENT")
$ WRITE SYS$OUTPUT TEXT
$ TEXT = F$FAO("!10AS!8SL!33SL!15AS", -
	"WSQUOTA", WSQUOTA, PQL_DWSQUOTA,"  PQL_DWSQUOTA")
$ WRITE SYS$OUTPUT TEXT
$ TEXT = F$FAO("!10AS!8SL!33SL!15AS", -
	"WSDEFAULT", WSDEFAULT, PQL_DWSDEFAULT,"  PQL_DWSDEFAULT")
$ WRITE SYS$OUTPUT TEXT
$ TEXT = F$FAO("!10AS!2(8SL)!25SL!15AS", -
	 "WSSIZE", WSSIZE, MAX_WSSIZE, WSMAX,"  WSMAX")
$ WRITE SYS$OUTPUT TEXT
$ TOTAL = GLOBAL + PROCESS
$ TEXT = F$FAO("!10AS!8SL", "PAGES", TOTAL)
$ WRITE SYS$OUTPUT TEXT
$ TEXT = F$FAO("!10AS!8SL", "FAULTS", FAULTS)
$ WRITE SYS$OUTPUT TEXT
$ WAIT 'waittime
$ if flag then goto end
$!
$!
$ PRCCNT	= 		F$GETJPI('PID,"PRCCNT")
$ IF PRCCNT .EQ. PRCCNT_INI THEN GOTO BASE
$!
$ GOTO LOOP
$!------------------------------------------------------------------------------
$!
$end:
$!
$ set broadcast=all
$ WRITE SYS$OUTPUT "''ESC'[22;0H"
$ exit
$!
$!------------------------------------------------------------------------------
$set_flag:
$!
$ flag := true
$ return
$!------------------------------------------------------------------------------
$!
$INIT:
$!
$ MAX_ASTCNT    = 'ASTCNT
$ MAX_FILCNT    = 'FILCNT
$ MAX_DIOCNT    = 'DIOCNT
$ MAX_BIOCNT    = 'BIOCNT
$ MAX_BYTCNT    = 'BYTCNT
$ MAX_TQCNT     = 'TQCNT
$ MAX_ENQCNT    = 'ENQCNT
$ MAX_PAGFILCNT = 'PAGFILCNT
$ MAX_WSSIZE    = 'WSSIZE
$ MAX_GLOBAL    = 'GLOBAL
$!
$ FIRST_LOOP    := FALSE
$ RETURN




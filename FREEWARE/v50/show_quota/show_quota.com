$!----------------------------------------------------------------------------
$! This procedure shows the given quota and the remaining quota of a process. 
$!
$! USAGE:   $ @SHOW_QUOTA pid
$!                or
$!          $ @SHOW_QUOTA process
$!          Note: the process must be the same UIC as the SHOW_QUOTA user.    
$!
$! P1: Is process ID or process name
$! P2: Is the interval time in seconds (maximum value 59 sec)
$!     The default interval time is 1 seconds.
$!
$! Take the results as an idea of how much quota is used. Do not take all 
$! the values for granted because they are updated in a certain interval.
$! Specialy the peak values may differ from what SHOW PROCESS/ACCOUNTING 
$! does give you.
$!
$! V 1.0 Written 1990 by A.Schwarz
$! Softwaresupport Digital Equipment Corporation Switzerland
$!
$! V 1.1 Modified 1992 (with Ideas of Eric)
$! ADDED SYSGEN VALUES (MAX and PQL)
$! ADDED recalculation of vaules after subprocess creation.(Quota sharing)
$! 
$! V 2.0 Modified 1994 by Peter Kurth
$! ADDED "Change" and Intervall
$!
$! V 2.1 Modified 1997 added PQL_MWS.. and change to reinit of BYTLM due to
$! pooled quota. Added Version Display and fractions of seconds waittime
$!
$! V 2.2 Modified 1999 by Geoff Kingsmill
$! Modified to ensure that BYTLM is increased but never decreased. 
$!   Unfortunately there appears to be no way to find the original processes 
$!   BYTLM using f$getsyi(). I did think about getting it from SDA however I 
$!   decided that the workaround contained here were good enough.
$! Changed default interval to 1 second
$! Shorten time fields so display does not get distorted
$! Lengthen various fields which were being trucated
$! Fixed up control_c so the screen is not corrupted
$! P1 can now include process name or pid
$!
$! V 2.3 Modified Aug-2000 by Geoff Kingsmill
$! Prevented DCL errors when a process was stuck in a Resource Wait
$!
$  Version := "2.3"
$!----------------------------------------------------------------------------
$!
$ default_wait_time = "00:00:01"
$!
$ on control_y then gosub end
$ on error then goto end
$ on warning then goto end
$ set broadcast=none
$ flag := false
$ ini_time = f$extract(0,20,F$TIME())
$!
$! User def. symbols
$!
$ WS := write sys$output
$! Default is current process..
$ IF "''P1'" .NES. ""
$   THEN
$   TEMP = F$CONTEXT("PROCESS",CTX,"PRCNAM","''P1'","EQL")
$   PID = F$PID(CTX)
$   IF PID .EQS. ""
$     THEN
$     GOTO CONT1
$   ELSE
$     P1 = PID
$   ENDIF
$   TEMP = F$CONTEXT("PROCESS",CTX,"CANCEL")
$ ENDIF
$cont1:
$ IF "''P1'" .EQS. "" THEN P1 = F$GETJPI("","PID")
$!
$! Time till new values are read and displayed.
$!
$ HUND_SEC = F$EXTRACT(0,2,F$ELEMENT(1,".",P2)+"00")
$ P2       = F$INTEGER(F$ELEMENT(0,".",P2))
$ IF ((P2 .EQ. 0) .AND. (F$INTEGER(HUND_SEC) .eq. 0)) .OR. (P2 .GE. 60)
$ THEN 
$   waittime = "''default_wait_time'"
$ ELSE
$   IF (P2 .LT. 60) .AND. (P2 .GE. 10) 
$   THEN 
$	waittime = "00:00:''P2'"	
$   ELSE 
$	waittime = "00:00:0''P2'"	
$   ENDIF
$ ENDIF
$ waittime = f$extract(0,f$locate(".",waittime),waittime)
$!
$ A = """
$ SIMPLE_PID = P1
$ PID = A+SIMPLE_PID+A
$!
$!
$! delete screen
$!===============
$ ESC[0,7]=27
$ saved_page_size = f$getdvi("tt:","tt_page")
$ saved_page_width = f$getdvi("tt:","devbufsiz")
$ if saved_page_size .lt. 24 
$ then 
$   ws "''esc'[24t"  ! sets terminal page size
$   set term/page=24
$ endif
$ if saved_page_width .lt. 80 then set term/width=80
$ WS "''ESC'[2J"
$ WS "''ESC'[0;0H"
$ WS " "
$!
$ FIRST_LOOP := TRUE
$ BYTLM 	= F$GETJPI('PID,"BYTLM")
$!
$BASE:
$!
$ NAME  	= F$GETJPI('PID,"PRCNAM")
$ PRCLM		= F$GETJPI('PID,"PRCLM")
$ ASTLM   	= F$GETJPI('PID,"ASTLM")
$ FILLM 	= F$GETJPI('PID,"FILLM")
$ DIOLM 	= F$GETJPI('PID,"DIOLM")
$ BIOLM	 	= F$GETJPI('PID,"BIOLM")
$ TQLM 		= F$GETJPI('PID,"TQLM")
$ ENQLM 	= F$GETJPI('PID,"ENQLM")
$ MODE 		= F$GETJPI('PID,"MODE")
$ PRIB		= F$GETJPI('PID,"PRIB")
$ LOGINTIM	= F$GETJPI('PID,"LOGINTIM")
$ PGFLQUOTA     = F$GETJPI('PID,"PGFLQUOTA")
$ WSDEFAULT 	= F$GETJPI('PID,"DFWSCNT")
$ WSQUOTA   	= F$GETJPI('PID,"WSQUOTA")
$ WSEXTENT  	= F$GETJPI('PID,"WSEXTENT")
$ WSMAX		= F$GETSYI("WSMAX")
$ PQL_MWSDEFAULT= F$GETSYI("PQL_MWSDEFAULT")
$ PQL_MWSQUOTA	= F$GETSYI("PQL_MWSQUOTA")
$ PQL_MWSEXTENT	= F$GETSYI("PQL_MWSEXTENT")
$ VIRTUALPAGECNT= F$GETSYI("VIRTUALPAGECNT")
$ PRCCNT	= F$GETJPI('PID,"PRCCNT")
$ PRCCNT_INI    = PRCCNT
$!
$!
$LOOP:
$ WS "''esc'[0;24H"
$ STATE 	= 		F$GETJPI('PID,"STATE")
$ IMAGE 	= 		F$GETJPI('PID,"IMAGNAME")
$ ASTCNT  	= 'ASTLM - 	F$GETJPI('PID,"ASTCNT")
$ FILCNT 	= 'FILLM - 	F$GETJPI('PID,"FILCNT")
$ DIOCNT 	= 'DIOLM - 	F$GETJPI('PID,"DIOCNT")
$ BIOCNT 	= 'BIOLM - 	F$GETJPI('PID,"BIOCNT")
$! Certain pool buffer allocations such as WCB's etc. are deducted from BYTLM
$! rather than BYTCNT. As there is no supported way to get the inital BYTLM we
$! need to always preserve the largest BYTLM and reflect the increase in
$! the maximum bytes used MAX_BYTCNT.
$ BYTLM_NEW     = F$GETJPI('PID,"BYTLM")
$ if BYTLM_NEW .gt. BYTLM
$ then
$   ! if the actual bytlm increases then we were actually using more bytlm
$   ! than we originally thought. Therefore, we need to increase the maximum
$   ! bytes used inline with the increased bytlm.
$   ! note: we only ever increase bytlm, we never decrease it.
$   MAX_BYTCNT    = 'MAX_BYTCNT + BYTLM_NEW - BYTLM
$   BYTLM = BYTLM_NEW
$ endif
$ BYTCNT 	= 'BYTLM -      F$GETJPI('PID,"BYTCNT")
$ TQCNT 	= 'TQLM  - 	F$GETJPI('PID,"TQCNT")
$ ENQCNT 	= 'ENQLM -  	F$GETJPI('PID,"ENQCNT")
$ PAGFILCNT	= 'PGFLQUOTA - 	F$GETJPI('PID,"PAGFILCNT")
$ WSSIZE    	=          	F$GETJPI('PID,"WSSIZE")
$! if the process is in a resource wait then wssize may return a null
$ if wssize .eqs. "" then wssize = -1
$! GLOBAL    	= 		F$GETJPI('PID,"GPGCNT")
$! PROCESS   	= 		F$GETJPI('PID,"PPGCNT")
$ TOTAL		= F$GETJPI('PID,"PPGCNT") + F$GETJPI('PID,"GPGCNT")
$ FAULTS    	= 		F$GETJPI('PID,"PAGEFLTS")
$! if the process is in a resource wait then faults may return a null
$ if faults .eqs. "" then faults = "*"
$ CPUTIM    	= 		F$GETJPI('PID,"CPUTIM")
$ CPU_SEC	= CPUTIM / 100
$ CPU_HUND	= CPUTIM - (CPU_SEC * 100)
$ CPUTIME       = "        ''CPU_SEC'.''CPU_HUND'"
$ CPUTIME=F$EXTRACT(F$LENGTH(CPUTIME)-8,8,CPUTIME)
$!
$ IF FIRST_LOOP THEN GOSUB INIT
$!
$!
$! Get max used quotas
$!====================
$!
$ IF 'PRCCNT    .gt. 'MAX_PRCCNT    THEN MAX_PRCCNT    = 'PRCCNT
$ IF 'ASTCNT    .gt. 'MAX_ASTCNT    THEN MAX_ASTCNT    = 'ASTCNT
$ IF 'FILCNT    .gt. 'MAX_FILCNT    THEN MAX_FILCNT    = 'FILCNT
$ IF 'DIOCNT    .gt. 'MAX_DIOCNT    THEN MAX_DIOCNT    = 'DIOCNT
$ IF 'BIOCNT    .gt. 'MAX_BIOCNT    THEN MAX_BIOCNT    = 'BIOCNT
$ IF 'BYTCNT    .gt. 'MAX_BYTCNT    THEN MAX_BYTCNT    = 'BYTCNT
$ IF 'TQCNT     .gt. 'MAX_TQCNT     THEN MAX_TQCNT     = 'TQCNT
$ IF 'ENQCNT    .gt. 'MAX_ENQCNT    THEN MAX_ENQCNT    = 'ENQCNT
$ IF 'PAGFILCNT .gt. 'MAX_PAGFILCNT THEN MAX_PAGFILCNT = 'PAGFILCNT
$ IF 'WSSIZE    .gt. 'MAX_WSSIZE    THEN MAX_WSSIZE    = 'WSSIZE
$ IF 'TOTAL	.gt. 'MAX_TOTAL     THEN MAX_TOTAL     = 'TOTAL
$!
$!
$! Get delta quotas
$!=================
$!
$ IF 'ASTCNT .nes. 'OLD_ASTCNT
$ THEN
$   DELTA_ASTCNT = ASTCNT - OLD_ASTCNT
$   OLD_ASTCNT   = ASTCNT
$ ELSE
$   DELTA_ASTCNT = 0
$ ENDIF
$ IF 'FILCNT .nes. 'OLD_FILCNT
$ THEN
$   DELTA_FILCNT = FILCNT - OLD_FILCNT
$   OLD_FILCNT   = FILCNT
$ ELSE
$   DELTA_FILCNT = 0
$ ENDIF
$ IF 'DIOCNT .nes. 'OLD_DIOCNT
$ THEN
$   DELTA_DIOCNT = DIOCNT - OLD_DIOCNT
$   OLD_DIOCNT = DIOCNT
$ ELSE
$   DELTA_DIOCNT = 0
$ ENDIF
$ IF 'BIOCNT .nes. 'OLD_BIOCNT
$ THEN
$   DELTA_BIOCNT = BIOCNT - OLD_BIOCNT
$   OLD_BIOCNT = BIOCNT
$ ELSE
$   DELTA_BIOCNT = 0
$ ENDIF
$ IF 'BYTCNT .nes. 'OLD_BYTCNT
$ THEN
$   DELTA_BYTCNT = BYTCNT - OLD_BYTCNT
$   OLD_BYTCNT = BYTCNT
$ ELSE
$   DELTA_BYTCNT = 0
$ ENDIF
$ IF 'TQCNT .nes. 'OLD_TQCNT
$ THEN
$   DELTA_TQCNT = TQCNT - OLD_TQCNT
$   OLD_TQCNT = TQCNT
$ ELSE
$   DELTA_TQCNT = 0
$ ENDIF
$ IF 'ENQCNT .nes. 'OLD_ENQCNT
$ THEN
$   DELTA_ENQCNT = ENQCNT - OLD_ENQCNT
$   OLD_ENQCNT = ENQCNT
$ ELSE
$   DELTA_ENQCNT = 0
$ ENDIF
$ IF 'PAGFILCNT .nes. 'OLD_PAGFILCNT
$ THEN
$   DELTA_PAGFILCNT = PAGFILCNT - OLD_PAGFILCNT
$   OLD_PAGFILCNT = PAGFILCNT
$ ELSE
$   DELTA_PAGFILCNT = 0
$ ENDIF
$ IF 'WSSIZE .nes. 'OLD_WSSIZE
$ THEN
$   DELTA_WSSIZE = WSSIZE - OLD_WSSIZE
$   OLD_WSSIZE = WSSIZE
$ ELSE
$   DELTA_WSSIZE = 0
$ ENDIF
$ IF 'TOTAL .nes. 'OLD_TOTAL
$ THEN
$   DELTA_TOTAL = TOTAL- OLD_TOTAL
$   OLD_TOTAL = TOTAL
$ ELSE
$   DELTA_TOTAL = 0
$ ENDIF
$!
$!
$! Print quotas and counts
$!========================
$!
$! WS 
$!
$ TEXT = F$FAO("!ASProcess Name: !20AS        PID: !8AS    !20AS", -
         "''ESC'[0;0H", NAME, SIMPLE_PID, f$extract(0,20,F$TIME()) )
$ WS TEXT
$!
$ IMAGE	= F$PARSE(IMAGE,,,"NAME")
$ TEXT = F$FAO("Image Name: !25AS     State: !5AS     Mode: !AS", -
         IMAGE, STATE, MODE)
$ WS TEXT
$!
$ TEXT = F$FAO("!41AS CpuTime: !8AS seconds", -
         "",CPUTIME)
$ WS TEXT
$!
$ TEXT = F$FAO("Process Quota Information:")
$ WS TEXT
$!
$ WS "              Quota     Used (prc)  Changed  MaxUsed since ''ini_time'"
$!
$ PERCENT = ( PRCCNT * 100 / PRCLM )
$ TEXT = F$FAO("!10AS!2(9SL)!5SL%!2(9SL)", -
         "PRCLM", PRCLM, PRCCNT, PERCENT, DELTA_PRCCNT, MAX_PRCCNT)
$ WS TEXT
$!
$ PERCENT = ( ASTCNT * 100 / ASTLM )
$ if astlm .eqs. "" .or. astlm .eq. -1 .or. astlm .eqs. "*"
$ then
$   astlm = "*"
$   astcnt = "*"
$   percent = "*"
$   max_astcnt = "*"
$ endif
$ TEXT = F$FAO("!10AS!2(9SL)!5SL%!2(9SL)", -
         "ASTLM", ASTLM, ASTCNT, PERCENT, DELTA_ASTCNT, MAX_ASTCNT)
$ if astlm .eqs. "" .or. astlm .eq. -1 .or. astlm .eqs. "*"
$ then
$   astlm = -1
$   astcnt = -1
$   percent = -1
$   max_astcnt = -1
$ endif
$ WS TEXT
$!
$ PERCENT = ( FILCNT * 100 / FILLM )
$ TEXT = F$FAO("!10AS!2(9SL)!5SL%!2(9SL)", -
         "FILLM", FILLM, FILCNT, PERCENT, DELTA_FILCNT, MAX_FILCNT)
$ WS TEXT
$!
$ PERCENT = ( DIOCNT * 100 / DIOLM )
$ TEXT = F$FAO("!10AS!2(9SL)!5SL%!2(9SL)", -
         "DIOLM", DIOLM, DIOCNT, PERCENT, DELTA_DIOCNT, MAX_DIOCNT)
$ WS TEXT
$!
$ PERCENT = ( BIOCNT * 100 / BIOLM )
$ TEXT = F$FAO("!10AS!2(9SL)!5SL%!2(9SL)", -
         "BIOLM", BIOLM, BIOCNT, PERCENT, DELTA_BIOCNT, MAX_BIOCNT)
$ WS TEXT
$!
$ PERCENT = ( BYTCNT * 100 / BYTLM )
$ TEXT = F$FAO("!10AS!2(9SL)!5SL%!2(9SL)", -
         "BYTLM", BYTLM, BYTCNT, PERCENT, DELTA_BYTCNT, MAX_BYTCNT)
$ WS TEXT
$!
$ PERCENT = ( ENQCNT * 100 / ENQLM )
$ TEXT = F$FAO("!10AS!2(9SL)!5SL%!2(9SL)", -
         "ENQLM", ENQLM, ENQCNT, PERCENT, DELTA_ENQCNT, MAX_ENQCNT)
$ WS TEXT
$!
$ PERCENT = ( TQCNT * 100 / TQLM )
$ TEXT = F$FAO("!10AS!2(9SL)!5SL%!2(9SL)", -
         "TQLM", TQLM, TQCNT, PERCENT, DELTA_TQCNT, MAX_TQCNT)
$ WS TEXT
$!
$ PERCENT = ( PAGFILCNT * 100 / PGFLQUOTA )
$ TEXT = F$FAO("!10AS!9SL!9SL!5SL%!9SL!9SL!12SL!AS", -
	 "PGFLQUOTA", PGFLQUOTA, PAGFILCNT, PERCENT, -  
	  DELTA_PAGFILCNT, MAX_PAGFILCNT,VIRTUALPAGECNT," VIRTUALPAGECNT")
$ WS TEXT
$!
$ WS ""
$ WS "Working Set Information:"
$ WS -
  "              Quota           Used  Changed  MaxUsed"
$ TEXT = F$FAO("!11AS!8SL!34* !11SL!AS", -
	"WSEXTENT", WSEXTENT, PQL_MWSEXTENT," PQL_MWSEXTENT")
$ WS TEXT
$!
$ TEXT = F$FAO("!11AS!8SL!34* !11SL!AS", -
	"WSQUOTA", WSQUOTA, PQL_MWSQUOTA," PQL_MWSQUOTA")
$ WS TEXT
$!
$ TEXT = F$FAO("!11AS!8SL!34* !11SL!AS", -
	"WSDEFAULT", WSDEFAULT, PQL_MWSDEFAULT," PQL_MWSDEFAULT")
$ WS TEXT
$!
$! wssize may return null if job is in a resource wait.
$! rather than fail simply display "*"
$! with displayed set value to -1 to prevent other dcl errors
$ if wssize .eqs. "" .or. wssize .eq. -1 .or. wssize .eqs. "*"
$ then
$   wssize = "*"
$   delta_wssize = "*"
$   max_wssize = "*"
$ endif
$ TEXT = F$FAO("!11AS!13*  !9SL!9SL!9SL!2* !10SL!AS", -
         "WSSIZE", WSSIZE, DELTA_WSSIZE, MAX_WSSIZE, WSMAX," WSMAX")
$ if wssize .eqs. "" .or. wssize .eq. -1 .or. wssize .eqs. "*"
$ then
$   wssize = -1
$   delta_wssize = -1
$   max_wssize = -1
$ endif
$ TEXT = F$FAO("!11AS!13* !10SL!9SL!9SL!2* !10SL!AS", -
	 "WSSIZE", WSSIZE, DELTA_WSSIZE, MAX_WSSIZE, WSMAX," WSMAX")
$ WS TEXT
$!
$ TEXT = F$FAO("!11AS!13* !10SL!9SL!9SL", -
         "PAGES", TOTAL, DELTA_TOTAL, MAX_TOTAL)
$ WS TEXT
$!
$ TEXT = F$FAO("!11AS!13*  !9SL", -
         "FAULTS", FAULTS)
$ WS TEXT
$!
$ WAIT 'waittime
$ NAME          = F$GETJPI('PID,"PRCNAM")
$!
$!
$ PRCCNT	= 		F$GETJPI('PID,"PRCCNT")
$ IF PRCCNT .NE. PRCCNT_INI 
$ THEN 
$    BYTLM 	= F$GETJPI('PID,"BYTLM")
$!   TQLM 		= F$GETJPI('PID,"TQLM")
$!   ENQLM 	= F$GETJPI('PID,"ENQLM")
$!   MODE 		= F$GETJPI('PID,"MODE")
$!   PRIB		= F$GETJPI('PID,"PRIB")
$    PRCLM		= F$GETJPI('PID,"PRCLM")
$    PRCCNT	= F$GETJPI('PID,"PRCCNT")
$    PRCCNT_INI    = PRCCNT
$ ENDIF
$!
$ GOTO LOOP
$!------------------------------------------------------------------------------
$!
$end:
$!
$ WS "''ESC'[23;0H"
$ if saved_page_size .ne. f$getdvi("tt:","tt_page") then -
     set term/page='saved_page_size'
$ if saved_page_width .ne. f$getdvi("tt:","devbufsiz") then -
     set term/width='saved_page_width'
$ set broadcast=all
$ exit
$!
$!------------------------------------------------------------------------------
$!
$INIT:
$!
$ MAX_PRCCNT    = 'PRCCNT
$ OLD_PRCCNT	= 'PRCCNT
$ DELTA_PRCCNT	= 0
$ MAX_ASTCNT    = 'ASTCNT
$ OLD_ASTCNT	= 'ASTCNT
$ DELTA_ASTCNT	= 0
$ MAX_FILCNT    = 'FILCNT
$ OLD_FILCNT    = 'FILCNT
$ DELTA_FILCNT	= 0
$ MAX_DIOCNT    = 'DIOCNT
$ OLD_DIOCNT    = 'DIOCNT
$ DELTA_DIOCNT	= 0
$ MAX_BIOCNT    = 'BIOCNT
$ OLD_BIOCNT    = 'BIOCNT
$ DELTA_BIOCNT	= 0
$ MAX_BYTCNT    = 'BYTCNT
$ OLD_BYTCNT    = 'BYTCNT
$ DELTA_BYTCNT	= 0
$ MAX_TQCNT     = 'TQCNT
$ OLD_TQCNT     = 'TQCNT
$ DELTA_TQCNT	= 0
$ MAX_ENQCNT    = 'ENQCNT
$ OLD_ENQCNT    = 'ENQCNT
$ DELTA_ENQCNT	= 0
$ MAX_PAGFILCNT = 'PAGFILCNT
$ OLD_PAGFILCNT = 'PAGFILCNT
$ DELTA_PAGFILCNT = 0
$ MAX_WSSIZE    = 'WSSIZE
$ OLD_WSSIZE    = 'WSSIZE
$ DELTA_WSSIZE	= 0
$ MAX_TOTAL	= 'TOTAL
$ OLD_TOTAL	= 'TOTAL
$ DELTA_TOTAL	= 0
$!
$ FIRST_LOOP    := FALSE
$ RETURN

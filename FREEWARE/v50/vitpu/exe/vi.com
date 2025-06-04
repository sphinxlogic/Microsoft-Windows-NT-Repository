$!
$!	VI.COM
$!
$!	Written by Gregg Wonderly  30-OCT-1986
$!
$!  A command file to run VI as a subprocess to the current DCL process.
$!
$!	Check to see if the process already exists
$!
$		VI = "EDIT/TPU"
$!   OR VI = "VI" if you are using VI.EXE
$!
$		TT = F$GETDVI("SYS$COMMAND","DEVNAM") - "_" - "-" - ":"
$		EDIT_NAME = "Edit_" + TT
$		PRIV_LIST = F$SETPRV("NOGROUP,NOWORLD")
$		PID = 0
$!
$	LOOP:
$		MPID = F$PID(PID)
$		PROC = F$GETJPI(MPID,"PRCNAM")
$!
$! If process is there, then go attach to it
$!
$		IF PROC .EQS. EDIT_NAME THEN GOTO ATTACH
$		IF PID .NE. 0 THEN GOTO LOOP
$!
$! If we fall through, then no such process, so spawn the editor
$!
$	SPAWN:
$		PRIV_LIST = F$SETPRV(PRIV_LIST)
$		DEFINE/NOLOG SYS$INPUT SYS$COMMAND:
$		T1 = F$EDIT(P2 + " " + P3 + " " + P4 + " " + P5 + " " -
                    + P6 + " " + P7, "UPCASE,COLLAPSE")
$		T1 = "''T1' ''P1'"
$		DEFINE/JOB/NOLOG TPU$ATTACH_NAME "''f$getjpi("","prcnam")'"
$		DEFINE/JOB/NOLOG TPU$SEARCH " "
$		DEFINE/JOB/NOLOG TPU$LINE " "
$		DEFINE/JOB/NOLOG TPU$OUTPUT " "
$		DEFINE/JOB/NOLOG TPU$ARGS " "
$		DEFINE/JOB/NOLOG TPU$NEWCMD " "
$		IF ("''P8'" .EQS. "NOSPAWN") THEN GOTO NOSPAWN
$			WRITE SYS$ERROR "[Spawning a new kept editor]"
$			SPAWN/PROCESS="''EDIT_NAME'"/NOLOG VI 'T1'
$			SAVED_MSG = F$ENVIRONMENT("MESSAGE")
$			SET MESSAGE/NOFACILITY/NOIDENTIFICATION/NOSEVERITY/NOTEXT
$			SET PROCESS/SUSPEND "''EDIT_NAME'"
$			SET MESSAGE 'SAVED_MSG'
$			WRITE SYS$ERROR "[Back from edit]"
$			GOTO DONE
$!
$!  Startup the editor without spawning
$!
$	NOSPAWN:
$		DEFINE/NOLOG/JOB TPU$NOSUBPROC 1
$		VI 'T1'
$		DEASSIGN/JOB TPU$NOSUBPROC
$		GOTO DONE
$!
$! Come here if subprocess already exists.
$!
$	ATTACH:
$!
$!	For the attach, we allow some parameters to be passed to the editor,
$!  other than the filename.  Specifically, a string to search for, or
$!  a line number to go to.
$!
$		PRIV_LIST = F$SETPRV(PRIV_LIST)
$		LINENO = " "
$		SEARCH = " "
$		OUTPUT = " "
$		NEWCMD = " "
$		LIST== F$EDIT ("''P1' ''P2' ''P3' ''P4' ''P5' ''P6' ''P7'", -
												"UPCASE,TRIM,COMPRESS")
$ 		START = F$LOCATE ("/OUT", LIST)
$ 		IF (START .EQ. F$LENGTH(LIST)) THEN GOTO NOOUT
$			REMAIN = F$EXTRACT (START,255,LIST)
$			END = F$LOCATE ("=", REMAIN)
$			IF (END .NE. F$LENGTH (REMAIN)) THEN GOTO OUT_1
$				WRITE SYS$ERROR F$MESSAGE(%X38150)
$				WRITE SYS$ERROR " \OUTPUT\"
$				EXIT
$!
$		OUT_1:
$			OUTPUT=F$EXTRACT(END+1,F$LOCATE(" ",REMAIN)-END-1,REMAIN)
$			LIST = F$EXTRACT (0,START,LIST) + -
					F$EXTRACT(END+2+F$LENGTH(OUTPUT),255,REMAIN)
$!
$	NOOUT:
$		IF (F$EXTRACT(0,1,P1) .NES. "+") THEN GOTO NO_LINE
$			IF (F$EXTRACT(1,1,P1) .NES. "/") .AND. -
					(F$EXTRACT(1,1,P1) .NES. "?") THEN GOTO NO_SEARCH
$				SEARCH = F$EXTRACT (1,255,P1)
$				P1=" "
$				GOTO NO_ALTS
$!
$	NO_SEARCH:     
$		IF (F$EXTRACT(1,1,P1) .NES. "$") THEN GOTO NO_CMD
$			NEWCMD = F$EXTRACT (2,255,P1)
$			P1=" "
$			GOTO NO_ALTS
$	NO_CMD:
$		LINENO = F$EXTRACT (1,255,P1)
$		p1=""
$!
$	NO_ALTS:
$		LIST== F$EDIT ("''P2' ''P3' ''P4' ''P5' ''P6' ''P7'", -
									"UPCASE,COMPRESS,TRIM")
$	NO_LINE:
$!
$!  Parameters are passed through the JOB logical table.
$!
$		DEFINE/JOB/NOLOG TPU$SEARCH "''SEARCH'"
$		DEFINE/JOB/NOLOG TPU$LINE "''LINENO'"
$		DEFINE/JOB/NOLOG TPU$OUTPUT "''OUTPUT'"
$		DEFINE/JOB/NOLOG TPU$ARGS " ''LIST'"
$		DEFINE/JOB/NOLOG TPU$NEWCMD "''NEWCMD'"
$		DEFINE/JOB/NOLOG TPU$ATTACH_NAME "''f$getjpi("","prcnam")'"
$!
$!  Restore all prior privileges.
$!
$		WRITE SYS$ERROR "[Attaching to editor]"
$		DEFINE/NOLOG SYS$INPUT SYS$COMMAND:
$		SET PROCESS/RESUME "''EDIT_NAME'"
$		ATTACH "''EDIT_NAME'"
$		WRITE SYS$ERROR "[Back from edit]"
$		SAVED_MSG = F$ENVIRONMENT("MESSAGE")
$		SET MESSAGE/NOFACILITY/NOIDENTIFICATION/NOSEVERITY/NOTEXT
$		SET PROCESS/SUSPEND "''EDIT_NAME'"
$		SET MESSAGE 'SAVED_MSG'
$!
$!  Exit the editor, deleting the journal files.
$!
$   DONE:
$		DEASSIGN/JOB TPU$ATTACH_NAME
$		DEASSIGN/JOB TPU$SEARCH
$		DEASSIGN/JOB TPU$LINE
$		DEASSIGN/JOB TPU$OUTPUT
$		DEASSIGN/JOB TPU$ARGS
$		DEASSIGN/JOB TPU$NEWCMD
$		EXIT

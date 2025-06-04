$!
$! This procedure takes the necessary source files and copies them to the
$! current majordomo kit directory tree.
$ DEFDIR = F$ENVIRONMENT("DEFAULT")
$ COP :== COPY/LOG
$ PUR := PURGE/LOG
$!
$! Get directory that we reside in.
$ MYPROC = F$ENVIRONMENT("PROCEDURE")
$ MYDEV = F$PARSE(MYPROC,,,"DEVICE","SYNTAX_ONLY")
$ MYDIR = F$PARSE(MYPROC,,,"DIRECTORY","SYNTAX_ONLY")
$ MYDIR = MYDEV + MYDIR
$!
$! Check to see if UCX$MAJORDOMO_* logicals have been defined. If no
$! then issue warning and crap out.
$ IF F$TRNLNM("UCX$MAJORDOMO_HOME") .EQS. ""
$ THEN
$   TYPE SYS$INPUT
You don't seem to have run SYS$MANAGER:UCX$MAJORDOMO_STARTUP.COM as evidenced by
the fact that the logical UCX$MAJORDOMO_HOME is undefined. Please look at
VMS_README.TXT and run SYS$MANAGER:UCX$MAJORDOMO_STARTUP.COM as directed
therein. Then run this procedure again.
$ GOTO ERROR_EXIT
$ ENDIF
$!
$ ON ERROR THEN GOTO ERROR_EXIT
$!
$! Set def to directory where we reside.
$ SET DEF 'MYDIR'
$!
$! This is a hack. If there is a PIPE_MAILSHR_REGISTRY.DAT there already
$! we just tromp on it.
$ WRITE SYS$OUTPUT "Copying PIPE_MAILSHR_REGISTRY.DAT SYS$COMMON:[SYSEXE]"
$ COP PIPE_MAILSHR_REGISTRY.DAT SYS$COMMON:[SYSEXE]
$!
$! Purge old files if user says to
$ ASKPURGE:
$ INQUIRE RESP "Do you want to purge the old versions of the files just copied? "
$ RESP = F$EDIT(RESP, "UPCASE, TRIM")
$ CHAR = F$EXTRACT(0, 1, RESP)
$ IF CHAR .NES. "Y" .AND. CHAR .NES. "N" THEN GOTO ASKPURGE
$ DOPURGE:
$ IF CHAR .EQS. "Y"
$ THEN
$   PUR SYS$COMMON:[SYSEXE]PIPE_MAILSHR_REGISTRY.DAT
$ ENDIF
$!
$ EXIT_PROC:
$ SET DEF 'DEFDIR'
$ EXIT
$!
$ ERROR_EXIT:
$ GOTO EXIT_PROC

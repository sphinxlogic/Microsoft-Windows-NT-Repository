$ DEFDIR = F$ENVIRONMENT("DEFAULT")
$ COP := COPY/LOG
$ PUR := PURGE/LOG
$!
$! Get directory that install.com resides in.
$ MYPROC = F$ENVIRONMENT("PROCEDURE")
$ MYDEV = F$PARSE(MYPROC,,,"DEVICE","SYNTAX_ONLY")
$ MYDIR = F$PARSE(MYPROC,,,"DIRECTORY","SYNTAX_ONLY")
$ MYDIR = MYDEV + MYDIR
$!
$ ON ERROR THEN GOTO ERROR_EXIT
$!
$! Are we AXP or VAX?
$ ARCHITECTURE = F$GETSYI("ARCH_NAME")
$ IF ARCHITECTURE .EQS. "VAX"
$ THEN
$   EXE_DIR := [.VAX]
$ ELSE
$   EXE_DIR := [.AXP]
$ ENDIF
$!
$! Set def to directory where install.com resides.
$ SET DEF 'MYDIR'
$!
$! And now to the architecture sub directory
$ SET DEF 'EXE_DIR'
$!
$! Copy the files to their system directories.
$ COP PIPE_MAILSHR.EXE SYS$COMMON:[SYSLIB]
$!
$! Copy SYS$MANAGER file(s):
$ SET DEF [-]
$ COP MAILPIPE_STARTUP.COM SYS$COMMON:[SYSMGR]
$!
$! Install the new mailpipe
$ WRITE SYS$OUTPUT "Running MAILPIPE_STARTUP.COM"
$ @SYS$MANAGER:MAILPIPE_STARTUP.COM
$!
$! Purge old files if user says to
$ ASKPURGE:
$ INQUIRE RESP "Do you want to purge old versions (if any) of mailpipe files from your system disk? "
$ RESP = F$EDIT(RESP, "UPCASE, TRIM")
$ CHAR = F$EXTRACT(0, 1, RESP)
$ IF CHAR .NES. "Y" .AND. CHAR .NES. "N" THEN GOTO ASKPURGE
$ DOPURGE:
$ IF CHAR .EQS. "Y"
$ THEN
$   PUR SYS$LIBRARY:PIPE_MAILSHR.EXE
$   PUR SYS$MANAGER:MAILPIPE_STARTUP.COM
$ ENDIF
$!
$! Warn them to edit their systartup file.
$ WARNEDIT:
$ TYPE SYS$INPUT
You need to edit your VMS systartup file to do

   @SYS$MANAGER:MAILPIPE_STARTUP.COM

This procedure has already copied MAILPIPE_STARTUP.COM to SYS$MANAGER.

$ INQUIRE RESP "Have you read this? Type "Y" to continue"
$ RESP = F$EDIT(RESP, "UPCASE, TRIM")
$ CHAR = F$EXTRACT(0, 1, RESP)
$ IF CHAR .NES. "Y" .AND. CHAR .NES. "N" THEN GOTO WARNEDIT
$!
$ EXIT_PROC:
$ SET DEF 'DEFDIR'
$ EXIT
$!
$ ERROR_EXIT:
$ GOTO EXIT_PROC

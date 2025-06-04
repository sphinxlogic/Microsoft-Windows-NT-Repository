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
$ COP UCX$SMTP_SFF.EXE SYS$COMMON:[SYSEXE]
$!
$! Purge old files if user says to
$ ASKPURGE:
$ INQUIRE RESP "Do you want to purge old versions (if any) of SFF files from your system disk? "
$ RESP = F$EDIT(RESP, "UPCASE, TRIM")
$ CHAR = F$EXTRACT(0, 1, RESP)
$ IF CHAR .NES. "Y" .AND. CHAR .NES. "N" THEN GOTO ASKPURGE
$ DOPURGE:
$ IF CHAR .EQS. "Y"
$ THEN
$   PUR SYS$SYSTEM:UCX$SMTP_SFF.EXE
$ ENDIF
$!
$! Warn them to edit their systartup file.
$ WARNEDIT:
$ TYPE SYS$INPUT
You need to edit your SYS$MANAGER:SYLOGIN.COM file to do

   $ if f$search("SYS$COMMON:[SYSEXE]UCX$SMTP_SFF.EXE") .nes. "" -
           then sff :== $SYS$COMMON:[SYSEXE]UCX$SMTP_SFF.EXE

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

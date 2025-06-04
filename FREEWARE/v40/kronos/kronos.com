$ write sys$output " "
$ write sys$output "++++ Starting Kronos."
$ !
$ ! Make sure Kronos isn't already running...
$ !
$ context = ""
$LOOP: pid = f$pid(context)
$ if pid .eqs. "" then goto DONE
$ name = f$getjpi(pid,"PRCNAM")
$ if name .eqs. "Kronos" 
$    then 
$       if "''p1'" .eqs. "RESTART"
$          then
$          stop/ident='pid'
$          goto done
$       else
$          write sys$output "Kronos already running !"
$          write sys$output " Use @KRONOS RESTART if you want to restart it."
$          exit
$       endif
$    endif
$ goto LOOP
$DONE:
$ !
$ ! OK start him up
$ !
$ DEFINE/CHARACTERISTIC  KRONOS  1
$ !define kronos_debug on   ! to turn on debuggin
$ SET QUEUE BAT$MEDIUM/CHARACTERISTIC=(1)
$ DEFINE/SYSTEM/TRANS=CONCEAL  KRONOS_ROOT  SYS$SYSDEVICE:[SYS0.SYSMGR.KRONOS.]
$ DEFINE/SYSTEM                KRONOS_DIR   SYS$SPECIFIC:[SYSMGR.KRONOS]
$ PURGE/KEEP=4 KRONOS_DIR:ERROR.LOG
$ RUN KRONOS_DIR:KRONOS   /UIC=[1,4]   /PRIVILEGES=SYSPRV -
  /ERROR=KRONOS_DIR:KRONOS.LOG   /PRIORITY=8   /process="Kronos"
$ !
$ ! start self-checking... delete previous job (if any)
$ !
$ RUN KRONOS_DIR:FIND_JOB
$ IF $STATUS .NE. 3 THEN DELETE/ENTRY='KDENTRY'
$ !
$ SUBMIT/AFTER="TOMORROW+2"/CHARACTERISTIC=1/QUE=BAT$MEDIUM/NOLOG -
     KRONOS_DIR:KRONOS_DISASTER
$ !

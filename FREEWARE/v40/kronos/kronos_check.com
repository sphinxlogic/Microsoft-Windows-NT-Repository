$ !
$ ! This file is submitted at 1:00 each morning and afternoon.  It will delete 
$ ! the pending copy of KRONOS_DISASTER.COM and resubmit it for 2:00 (13 hours
$ ! from now).
$ !
$ ! "FIND_JOB" will set the job entry number for KRONOS_DISASTER into symbol
$ ! "KDENTRY".
$ !
$ RUN KRONOS_DIR:FIND_JOB
$ ST = $STATUS
$ IF ST .EQ. 3 
$    THEN
$       MAIL/SUBJECT="Curious" KRONOS_DIR:CURIOUS.DAT -
            IRENE,RAGOSTA,HOLST
$ ENDIF
$ !
$ SUBMIT/AFTER="+13"/CHARACTERISTIC=1/QUEUE=BAT$MEDIUM/NOLOG -
     KRONOS_DIR:KRONOS_DISASTER
$ !
$ IF ST .NE. 3 THEN DELETE/ENTRY='KDENTRY'

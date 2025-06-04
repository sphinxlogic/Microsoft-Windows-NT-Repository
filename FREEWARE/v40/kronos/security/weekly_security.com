$ ! Check operator's log for potential security problems
$ !
$ SET DEFAULT KRONOS_ROOT:[SECURITY]
$ !
$ ! OPEN NEW OPERATOR'S AND AUDIT LOG FILES
$ !
$ run new_oper
$ SET AUDIT/SERVER=NEW_LOG
$ !
$ ! ANALYZE SECURITY LOG
$ !
$ analyze /audit/full/out=audit.log SYS$MANAGER:security_audit.audit$journal;-1
$ purge/keep=4 sys$common:[sysmgr]security_audit.audit$journal
$ !
$ ! LET THE TROOPS KNOW
$ !
$ MAIL/SUBJECT=SECURITY_CHECK audit.log  -
  system,operator
$ PURGE/KEEP=2 audit.log
$ PURGE/KEEP=4 SYS$SPECIFIC:[SYSMGR]OPERATOR.LOG

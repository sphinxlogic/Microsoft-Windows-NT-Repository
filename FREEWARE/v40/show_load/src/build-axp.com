$! BUILD-AXP.COM - assumes LAVDRIVER installed from MultiNet
$
$ FORTRAN SHOW_LOAD_AVERAGE
$ FORTRAN POSSESS_IDENTIFIER
$ FORTRAN QSORT
$
$ MESSAGE SHOW_LOAD_AVERAGE /OBJECT=MESSAGE
$
$ LINK /NOTRACEBACK SHOW_LOAD_AVERAGE,MESSAGE,POSSESS_IDENTIFIER,QSORT
$
$ FORTRAN ANNOUNCE_DAEMON
$ FORTRAN AST_HANDLER
$ FORTRAN QUEUE_AST
$ FORTRAN GET_JOBCNT
$ FORTRAN SEND_MAILBOX
$
$ LINK /SYSEXE -
       ANNOUNCE_DAEMON, -
       MESSAGE,- 
       AST_HANDLER, -
       QUEUE_AST, -
       GET_JOBCNT, -
       SEND_MAILBOX
$
$ COPY SHOW_LOAD_AVERAGE.EXE SYS$COMMON:[SYSEXE]/PROTECTION=W:E
$ COPY ANNOUNCE_DAEMON.EXE SYS$COMMON:[SYSEXE]/PROTECTION=W
$ COPY ANNOUNCE_DAEMON.COM SYS$COMMON:[SYSMGR]/PROTECTION=W
$
$ EXIT

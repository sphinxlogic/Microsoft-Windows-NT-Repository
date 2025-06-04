$!
$!  put in the CRINOID_home directory ...
$!
$ x = f$trnlnm("CRINOIDLOG_STDERR_MBX")
$ if x .eqs. ""
$ then
$   write sys$output "CRINOIDLOG appears not to be running..."
$   exit
$ endif
$ define/sys CRINOIDLOG_COMMAND NEWLOG
$ open/write/err=loop fd CRINOIDlog_stderr_mbx
$ write fd "CRINOIDLOG_COMMAND: New logfile request from DCL"
$ close fd
$ J = 12
$ wait 00:00:01
$ LOOP:
$   x = f$trnlnm("CRINOIDLOG_COMMAND","LNM$SYSTEM_TABLE")
$   if (x .EQS. "") then goto done
$   J = J - 1
$   IF (J .LE. 0)
$   THEN
$       WRITE SYS$OUTPUT "Timeout waiting for CRINOIDLOG"
$       DEASSIGN/SYSTEM CRINOIDLOG_COMMAND
$       GOTO DONE
$   ENDIF
$   wait 00:00:05
$ GOTO LOOP
$ done:
$ exit 1

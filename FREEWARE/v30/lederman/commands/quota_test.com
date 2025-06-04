$ ON CONTROL_Y    THEN $ GOTO CTRLY
$ ON WARNING      THEN $ GOTO WAITER
$ ON ERROR        THEN $ GOTO WAITER
$ ON SEVERE_ERROR THEN $ GOTO WAITER
$ WRITE SYS$OUTPUT "[H[J"
$ LOOP:
$  WRITE SYS$OUTPUT "[H"
$  SHOW PROCESS/QUOTA/ID='P1
$  save_stat = '$STATUS
$  IF (save_stat .AND. 1) .EQ. 1 THEN $ GOTO LOOP
$!
$ WAITER:
$ WRITE SYS$OUTPUT "[20;1H" + F$MESSAGE(save_stat)
$ IF save_stat .EQ. %X100008E8 THEN $ EXIT
$ IF save_stat .EQ. %X8E8 THEN $ EXIT
$ SHOW SYMBOL save_stat
$ exit
$ GOTO LOOP
$!
$ CTRLY:
$ WRITE SYS$OUTPUT "[20;1H"
$ EXIT
$!
$! To monitor a process to determine what quota is being exhausted,
$! start the process on terminal 'A' and get it's Process ID
$!
$! Start this command procedure on terminal 'B' using the PID as an argument.
$! For example: @QUOTA_TEST.COM 99999999
$! 
$! When the process running on terminal 'A' depletes one of the UAF quotas,
$! the corresponding display on terminal 'B' will be reduced to zero or
$! very close to zero, and the error "xxx-F-EXQUOTA, Quota Exceeded" will
$! be issued to terminal 'A'.  The quota that was reduced to zero is the
$! quota that needs to be addressed to resolve the failure.
$! 
$! This procedure will not diagnose all possible EXQUOTA problems.  For
$! example, depletion of pooled quotas will not be completely tested. In
$! addition, other quotas can be examined by replacing the SHOW
$! PROCESS/QUOTAS statement in the example above with SHOW MEMORY, or SHOW
$! PROCESS/MEMORY.
$!
$! Extracted from a DSIN article

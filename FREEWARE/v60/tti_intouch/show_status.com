$! SHOW_STATUS.COM - Show INTOUCH user status information
$!
$! Copyright (c) 1992 Touch Technologies, Inc.
$
$
$ quote[0,8]=34
$ if f$trnlnm("sys$scratch") .eqs. "" then define/process sys$scratch sys$disk
$ write sys$output "Show INTOUCH User Status"
$ write sys$output ""
$ write sys$output -
 "PID      Username     Procedure            Status Information"
$ write sys$output -
 "-------- ------------ -------------------- -----------------------------------"
$ create sys$scratch:show_status.tmp
$ define/user sys$output sys$scratch:show_status.tmp
$ show logical/table=lnm$job_* intouch_run_status
$ open myfile sys$scratch:show_status.tmp/error=done/read
$ loop:
$   read myfile raw_line/end=done
$   text = f$element(3, quote, raw_line)
$   write sys$output "''text'"
$ goto loop
$ done:
$ close myfile
$ delete sys$scratch:show_status.tmp;*
$ exit
$
$ no_users:
$ write sys$output "--No INTOUCH users--"
$ exit

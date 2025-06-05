$! ISHOW.COM - Show INTOUCH user information
$!
$! Copyright (c) 1990 Touch Technologies, Inc.
$
$
$ quote[0,8]=34
$ if f$trnlnm("sys$scratch") .eqs. "" then define/process sys$scratch sys$disk
$ write sys$output "Show INTOUCH Users"
$ write sys$output ""
$ write sys$output -
 "PID      Username     Procedure            Comment"
$ write sys$output -
 "-------- ------------ -------------------- ------------------------------"
$ create sys$scratch:ishow.tmp
$ define/user sys$output sys$scratch:ishow.tmp
$ show logical/table=lnm$job_* intouch_data
$ open myfile sys$scratch:ishow.tmp/error=done/read
$ loop:
$   read myfile raw_line/end=done
$   text = f$element(3, quote, raw_line)
$   write sys$output "''text'"
$ goto loop
$ done:
$ close myfile
$ delete sys$scratch:ishow.tmp;*
$ exit
$
$ no_users:
$ write sys$output "--No INTOUCH users--"
$ exit

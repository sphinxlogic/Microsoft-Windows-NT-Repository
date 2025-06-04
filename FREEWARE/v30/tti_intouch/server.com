$! SERVER.COM - INTOUCH Guider Server Process
$! Copyright (c) 1989,1993 Touch Technologies, Inc.
$!
$! We read a command from the mailbox and then execute it.
$! The easiest way to terminate this process
$! is to send to the mailbox "LOGOUT"  or  "EXIT".
$
$ set noon
$ @tti_run:intouch.com
$ define/process sys$input   "tti_mbx:"/nolog
$ define/process sys$command "tti_mbx:"/nolog
$ open/read cmd_file        tti_mbx:
$
$ set noverify
$ loop:
$   read cmd_file z0/end_of_file=loop
$   cmd_line == z0
$   if  cmd_line .eqs. "&"   then  goto loop
$   if  cmd_line .eqs. "^W"  then  goto loop
$   if  cmd_line .eqs. "^Y"  then  goto loop
$   'cmd_line'
$ goto loop

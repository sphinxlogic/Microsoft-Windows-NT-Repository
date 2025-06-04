$ save_verify = 'f$verify(0)'
$!
$!  Startup file for NOTICE
$!  Copyright © 1992--1994, MadGoat Software.  All rights reserved.
$!
$ notice_exe := WKU$ROOT:[EXE]NOTICE.EXE			!****LOCAL
$ define/system/exec notice_library WKU$ROOT:[HELP]NOTICE.TLB	!****LOCAL
$ if f$search(notice_exe).nes.""
$ then	if f$file_attributes(notice_exe,"KNOWN")
$	then cmd = "replace"
$	else cmd = "add"
$	endif
$	install 'cmd' 'notice_exe'/open/header/share
$ endif
$ common_exit:
$	exit f$verify(save_verify).or.1

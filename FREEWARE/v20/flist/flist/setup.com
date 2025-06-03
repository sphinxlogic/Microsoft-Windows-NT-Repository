$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! Copyright 1993 by Thomas E. Dickey.  All Rights Reserved.                   !
$!                                                                             !
$! You may freely copy or redistribute this software, so long as there is no   !
$! profit made from its use, sale trade or reproduction. You may not change    !
$! this copyright notice, and it must be included in any copy made.            !
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! $Id: setup.com,v 1.2 1993/04/22 16:18:36 tom Exp $
$	verify = F$VERIFY(0)
$!+
$! defines foreign-commands for testing, automatically from contents of [.bin]
$!-
$	path = F$ENVIRONMENT("procedure")
$	path = "''F$EXTRACT(0,F$LOCATE("]",path),path)'.bin]"
$	name = ""
$ loop_com:
$	last = name
$	name = F$SEARCH("''path'*.com;")
$	if name .nes. "" .and. name .nes. last
$	then
$		name = F$EXTRACT(0,F$LOCATE(";",name),name)
$		'F$PARSE(name,,,"NAME","SYNTAX_ONLY") :== " @ ''name'"
$		goto loop_com
$	endif
$
$ loop_exe:
$	last = name
$	name = F$SEARCH("''path'*.exe;")
$	if name .nes. "" .and. name .nes. last
$	then
$		name = F$EXTRACT(0,F$LOCATE(";",name),name)
$		'F$PARSE(name,,,"NAME","SYNTAX_ONLY") :== "$ ''name'"
$		goto loop_exe
$	endif
$
$	verify = F$VERIFY(verify)

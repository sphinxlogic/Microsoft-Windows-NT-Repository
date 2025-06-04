$!
$!***********************************************************************
$!
$!
$! PROCESS_CALLS.COM
$!
$!	This procedure shows the call tree of the target process.
$!
$!
$! VERSION:      01.00
$!
$! AUTHOR:       Kari Salminen
$!
$! DATE:         02-MAR-1996
$!
$!
$! MODIFIED BY:
$!
$!	V1.1    17-APR-1996	Kari Salminen
$!				Add SHOW PROC/REG , /CHAN and STACK
$!
$!
$!
$!
$!***********************************************************************
$!
$ if p1 .eqs. "" then goto error1
$!
$ on warning then goto exit
$ on error   then goto exit
$!
$ wo = "WRITE SYS$OUTPUT"
$!
$ wo (" ")
$ wo ("              *****************************")
$ wo ("              *  PROCESS_CALLS.COM  V1.1  *")
$ wo ("              *  Show process call tree   *")
$ wo ("              *****************************")
$ wo (" ")
$ wo (" ")
$ wo ("   This procedure shows the call tree of the target process. The target")
$ wo ("   process MUST BE HANGING, in other case the information is incorrect.")
$ wo (" ")
$!
$ search_string = p1
$ len = f$length(search_string)
$!
$!
$!***************************************************
$!
$ ANALYZE/SYSTEM
SET OUT SDA.TMP
SHO SUM
SET OUT SYS$OUTPUT
EXIT
$!
$!***************************************************
$!
$ scratch_file = "SDA.TMP"
$!
$  open/read/error=error tempfile 'scratch_file'
$!
$ next_line:
$!
$  read/error=error/END_OF_FILE=not_found  tempfile string
$!
$  string = f$edit(string,"upcase,compress")
$!
$ if string .eqs. "" then goto next_line
$!
$  tmpstr = f$extract(15, len , string)
$!
$ if tmpstr .eqs. "" then goto next_line
$ if search_string .nes. tmpstr then goto next_line
$!
$ process = tmpstr
$ pid = f$extract(10, 4, string)
$!
$ write sys$output (" PROCESS = ''tmpstr'   PID = ''pid'")
$!
$ close tempfile
$!
$!***************************************************
$!
$  open/write/error=error tempfile SDA1.TMP
$!
$  write/error=error tempfile "SET OUT SDA2.TMP"
$  write/error=error tempfile "SET PROC/IND=''pid'"
$  write/error=error tempfile "SHOW PROCESS"
$  write/error=error tempfile "SHOW CALL"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "SHOW CALL/NEXT"
$  write/error=error tempfile "EXIT"
$!
$ close tempfile
$!
$ define/user sys$output NL:
$ ANALYZE/SYSTEM
@SDA1.TMP
$!
$!***************************************************
$!
$!        Procedure Entry: 00000000 00104D10
$!12345678901234567890123456789012345678901234
$!        Return address on stack = 00000000 001130A8
$!                 1234567890123456
$ search_string = "Procedure Entry:"
$ len = f$length(search_string)
$!
$ search_string2 = "Return address o"
$ len2 = f$length(search_string)
$!
$!***************************************************
$!
$ open/write/error=error tempfile1 SDA3.TMP
$!
$ first_time = 0
$!
$  write/error=error tempfile1 "SET OUT PROCESS_CALLS.TMP"
$  write/error=error tempfile1 "READ/EXEC"
$  write/error=error tempfile1 "READ SYS$LOADABLE_IMAGES:SYSDEF.STB"
$  write/error=error tempfile1 "SET PROC/IND=''pid'"
$  write/error=error tempfile1 "SHOW PROCESS"
$  write/error=error tempfile1 "SHOW PROCESS/REG"
$  write/error=error tempfile1 "SHOW PROCESS/CHAN"
$  write/error=error tempfile1 "SHOW STACK"
$!
$!
$ scratch_file = "SDA2.TMP"
$!
$ open/read/error=error tempfile 'scratch_file'
$!
$ next_line:
$!
$  read/error=error/END_OF_FILE=end tempfile string
$!
$ if string .eqs. "" then goto next_line
$ if string .eqs. "EOF" then goto end
$!
$ tmpstr  = f$extract(8, len , string)
$!
$ if tmpstr .eqs. "" then goto next_line
$ if search_string .nes. tmpstr then goto next_string
$!
$ addr = f$extract(34, 8, string)
$!
$!	 write sys$output (" ENTRY  = ''tmpstr'   ADDRESS = ''addr'")
$!
$ if first_time .EQ. 0 then write/error=error tempfile1 "SHOW CALL"
$ if first_time .EQ. 1 then write/error=error tempfile1 "SHOW CALL/NEXT"
$!
$ first_time = 1
$!
$  write/error=error tempfile1 "MAP ''addr'"
$!
$ goto next_line
$!
$!**************************
$!
$ next_string:
$!
$ tmpstr2 = f$extract(8, len2 , string)
$!
$ if search_string2 .nes. tmpstr2 then goto next_line
$!
$ addr2 = f$extract(43, 8, string)
$!
$!	 write sys$output (" RETURN = ''tmpstr2'   ADDRESS = ''addr2'")
$!
$  write/error=error tempfile1 "MAP ''addr2'"
$!
$!
$ goto next_line
$!
$!
$!***************************************************
$!
$ error:
$!
$ close tempfile
$ close tempfile1
$ goto exit
$!
$ error1:
$!
$ write sys$output (" Process name is required ==> @PROCESS_CALLS MY_PROC")
$!
$ goto exit
$!
$ not_found:
$!
$ close tempfile
$!
$ write sys$output (" Could not find a process with name ''p1'")
$!
$ goto exit
$!
$!***************************************************
$!
$ end:
$!
$ write/error=error tempfile1 "EXIT"
$!
$ close tempfile
$ close tempfile1
$!
$ define/user sys$output NL:
$ ANALYZE/SYSTEM
@SDA3.TMP
$!
$ write sys$output (" ")
$ write sys$output (" The process call tree was written into file PROCESS_CALLS.TMP")
$ write sys$output (" ")
$!
$!***************************************************
$!
$ exit:
$!
$ exit
$!

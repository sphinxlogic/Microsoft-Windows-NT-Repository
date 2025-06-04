$ ! Procedure:	UPDATE_LIBRARY_BASIC.COM
$ set noon
$ say = "write sys$output"
$ hw_model = f$getsyi("HW_MODEL")
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ alpha = (f$getsyi("HW_MODEL") .ge. 1024)
$ architecture = "VAX"
$ if (alpha) then architecture = "ALPHA"
$ library_name = "SYS_OBJECTS.''architecture'_OLB"
$ obj = "''architecture'_OBJ"
$ say "%Updating ''library_name' from *.BAS"
$ compile_command = "basic/object=.''obj'/variant=''hw_model'"
$ filename = "*.bas"
$loop:
$   filefound = f$search(filename)
$   if (filefound .eqs. "") then goto end_loop
$   name = f$parse(filefound,,,"name")
$   say "  ... ''name'"
$   'compile_command' 'name'
$   library 'library_name' 'name'.'obj'
$ goto loop
$end_loop:
$ delete/nolog *.'obj';*
$ say ""
$ library/compress 'library_name'/output='library_name'
$!$ library/list 'library_name'
$ exitt 1

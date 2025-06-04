$ ! Procedure:	BUILD_PATCH.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ on error then goto bail_out
$ say = "write sys$output"
$ define/process/nolog dbslibrary sys$disk:[]
$ call build_it patch_disk
$ call build_it patch_file
$bail_out:
$ if (f$trnlnm("DBSLIBRARY","LNM$PROCESS_TABLE") .nes. "") then -
$   deassign dbslibrary
$ !'f$verify(__vfy_saved)'
$ exitt 1
$build_it: subroutine
$ set noon
$ say = "write sys$output"
$ filespec = f$edit(P1,"COLLAPSE,UPCASE")
$ if (filespec .eqs. "")
$   then
$   say "%Nothing to build"
$ else
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ options_file = f$getjpi("","PID") + ".OPTIONS"
$ open/write options 'options_file'
$ if (vax)
$   then
$   arch_name = "VAX"
$   linkk = "link"
$   write options "sys$share:tecoshr/shareable"
$ else
$ arch_name = "ALPHA"
$ linkk = "link/sysexe=selective/nonative_only"
$ write options "sys$share:tecoshr_tv/shareable"
$ endif !(vax)
$ close/nolog options
$ filename = f$parse(filespec,,,"NAME")
$ objectfile = filename + ".''arch_name'_OBJ"
$ executablefile = filename + ".''arch_name'_EXE
$ libraryfile = "SYS_OBJECTS.''arch_name'_OLB"
$ say "%Compiling ''filename' . . ."
$ macroo/object='objectfile' 'filename'
$ say "%Linking ''filename' . . ."
$ linkk/executable='executablefile'/notraceback/nouserlibrary -
	'objectfile'-
	,'libraryfile'/library-
	,'options_file'/options
$ endif !(filespec .eqs. "")
$exit_build_it:
$ close/nolog options
$ if (f$search(options_file) .nes. "") then deletee/nolog 'options_file';*
$ exitt 1
$ endsubroutine

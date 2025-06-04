$v = f$verify(0)
$!
$!  
$!  BUILD.COM - To build image SHS.EXE & Help Facility SHS.HLB
$!		<jshen1@ford.com>
$!
$ on error then goto error_handler
$write sys$output " Compiling SHS.FOR ..."
$fortran/nolist/nodebug/obj=shs.obj shs.for
$write sys$output " Compiling INIT_CLI.FOR ..."
$fortran/nolist/nodebug/obj=init_cli.obj init_cli.for
$write sys$output " Compiling SHS_FIND_HELP.FOR ..."
$fortran/nolist/nodebug/obj=shs_find_help.obj shs_find_help.for
$write sys$output " Compiling GETJPI_BASIC.FOR ..."
$fortran/nolist/nodebug/obj=getjpi_basic.obj getjpi_basic.for
$write sys$output " Compiling GETJPI_EXTEND.FOR ..."
$fortran/nolist/nodebug/obj=getjpi_extend.obj getjpi_extend.for
$write sys$output " Compiling SHS_INITIATE_SORT.FOR ..."
$fortran/nolist/nodebug/obj=shs_initiate_sort.obj shs_initiate_sort.for
$write sys$output " Compiling SHS_QSORT.FOR ..."
$fortran/nolist/nodebug/obj=shs_qsort.obj shs_qsort.for
$write sys$output " Compiling SHS_CLI.CLD ..."
$set command/obj=shs_cli.obj shs_cli.cld
$write sys$output " Assemling SHSDEF.MAR ..."
$macro/nolist/nodebug/obj=shsdef.obj shsdef.mar
$write sys$output " Linking image SHS.EXE ..."
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! To link the object files against system object library, uncomment
$! the following command and comment out the very next LINK command.
$! See AAAREADME.TXT for details.
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!link/notrace/nodebug/nomap/nosysshr/exec=shs.exe shs.obj, init_cli.obj, -
$!	shs_find_help.obj, getjpi_basic.obj, getjpi_extend.obj, -
$!	shs_initiate_sort.obj, shs_qsort.obj, shsdef.obj, shs_cli.obj, -
$!	sys$library:starlet/lib, sys$library:imagelib/lib/include=lbrshr, -
$!	sys$input/opt
$!identification="V1.1-000"
$link/notrace/nodebug/nomap/exec=shs.exe shs.obj, shs_find_help.obj, -
	getjpi_basic.obj, getjpi_extend.obj, shs_initiate_sort.obj, -
	shs_qsort.obj, shsdef.obj, init_cli.obj, shs_cli.obj, sys$input/opt
identification="V1.1-000"
$if f$search("SHS.HLB") .eqs. "" 
$then 
$  write sys$output " Building SHS.HLB help library..."
$  library/Create/Help shs.hlb
$  library/replace/help shs.hlb shs.hlp
$endif
$goto exit_entry
$error_handler:
$write sys$output " Clean up ..."
$if f$search("SHS.OBJ") .nes. "" then delete/nolog/noconfirm shs.obj;*
$if f$search("SHSDEF.OBJ") .nes. "" then delete/nolog/noconfirm shsdef.obj;*
$if f$search("SHS_CLI.OBJ") .nes. "" then delete/nolog/noconfirm shs_cli.obj;*
$if f$search("INIT_CLI.OBJ") .nes. "" then delete/nolog/noconfirm init_cli.obj;*
$if f$search("SHS_FIND_HELP.OBJ") .nes. ""  then $-
	delete/nolog/noconfirm shs_find_help.obj;*
$if f$search("GETJPI_BASIC.OBJ") .nes. "" then $-
	delete/nolog/noconfirm getjpi_basic.obj;*
$if f$search("GETJPI_EXTEND.OBJ") .nes. "" then $-
	delete/nolog/noconfirm getjpi_extend.obj;*
$if f$search("SHS_QSORT.OBJ") .nes. "" then $-
	delete/nolog/noconfirm shs_qsort.obj;*
$if f$search("SHS_INITIATE_SORT.OBJ") .nes. "" then $-
	delete/nolog/noconfirm shs_initiate_sort.obj;*
$exit_entry:
$exit 1+0*f$verify(v)

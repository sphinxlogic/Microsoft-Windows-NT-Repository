$! vsetup.com - customized setup for VToolkit build environment
$!
$!*****************************************************************************
$! VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
$! (c) Steve Klein							      *
$! Unpublished rights reserved under the copyright laws of the United States. *
$!*****************************************************************************
$!
$!      P1      additional C compiler switches
$!      P2      additional UIL switches
$!
$! Modification History:
$!   9-Sep-1993 (sjk) VToolkit V2.
$!  05-Apr-1990 (sjk) Initial entry.
$
$! Set up target directories, check for their existence, and default to current directory if bad
$
$ t = F$EDIT(F$GETSYI("ARCH_NAME"),"UPCASE")	! platform
$
$ vtk_obj_dir		:== [-.'t'.obj]	! where the objects go (relative to the source directory)
$ vtk_exe_dir		:== [-.'t'.exe]	! where the executables go (relative to the source directory)
$
$ on severe then continue
$ define/user sys$output nl:
$ define/user sys$error nl:
$ directory/out=nl: 'vtk_obj_dir'
$ if ($status .ne. 1) .and. ($status .ne. %X10018290) then vtk_obj_dir :== []
$
$ on severe then continue
$ define/user sys$output nl:
$ define/user sys$error nl:
$ directory/out=nl: 'vtk_exe_dir'
$ if ($status .ne. 1) .and. ($status .ne. %X10018290) then vtk_exe_dir :== []
$
$ vtk_compiler		:== DECC
$! vtk_compiler		:== VAXC
$
$ vtk_cc_options 	:== /obj='vtk_obj_dir'/nolis/opt=noinline
$ if "''vtk_compiler'" .eqs. "DECC" then vtk_cc_options :== 'vtk_cc_options'/nowarn/prefix=all
$
$! vtk_cc_command	:== cc				! for systems that only have VAXC installed
$ vtk_cc_command	:== cc/'vtk_compiler'		! for all other systems
$
$ vcc :== 'vtk_cc_command' 'vtk_cc_options' 'p1'
$
$! Modify the following line to select your favorite UIL compiler options
$
$ muil :== uil/ver=motif11/out='vtk_exe_dir'/nolis'p2'
$ define/nolog uil$include sys$disk:[],decw$include

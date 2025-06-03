$! vlink.com - common MOTIF/VMS link command file
$!
$!	P1	main object module name
$!      P2      additional LINK switches
$!
$!*****************************************************************************
$! VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
$! (c) Steve Klein							      *
$! Unpublished rights reserved under the copyright laws of the United States. *
$!*****************************************************************************
$!
$! Modification History:
$!    09-Sep-1993 (sjk) VToolkit V2.
$!    03-Apr-1991 (sjk) MOTIF V1.0 update.
$!    14-May-1990 (sjk) MOTIF BL1.1 port.
$!    05-Apr-1990 (sjk) Initial entry.
$
$ define/user vtk_library	'vtk_obj_dir'vtklib.olb		! to pass into options file
$ link 'vtk_obj_dir''p1'/exe='vtk_exe_dir'/nomap,sys$input/opt'p2
psect_attr=XMQMOTIF,noshr,lcl
vtk_library/lib
sys$library:decw$xmlibshr/share
sys$library:decw$xtshr/share
sys$library:decw$xlibshr/share

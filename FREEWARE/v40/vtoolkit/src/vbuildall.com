$! vbuildall.com - MOTIF/VMS build command file for VToolkit
$!
$!	P1	additional C compiler switches
$!	P2	additional UIL switches
$!	P3	additional LINK switches
$!
$!*****************************************************************************
$! VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
$! (c) Steve Klein							      *
$! Unpublished rights reserved under the copyright laws of the United States. *
$!*****************************************************************************
$!
$! Modification History:
$!   9-Sep-1993 (sjk) VToolkit V2.
$!  05-Apr-1990 (sjk) Initial entry.
$
$ on severe then continue
$ set noon
$
$ @vsetup 'p1 'p2
$
$ vcc	tlist		
$ vcc	vautorepeat	
$ vcc	vdrag		
$ vcc	vfake		
$ vcc	vframe		
$ vcc	vheader		
$ vcc	vhist		
$ vcc	vlist		
$ vcc	vtkaddinput	
$ vcc	vtkcursor	
$ vcc	vtkmainloop	
$ vcc	vtksavegeometry	
$ vcc	vtkutil		
$
$ lib/cre 'vtk_obj_dir'vtklib	-
	'vtk_obj_dir'tlist,vautorepeat,vdrag,vfake,vframe,-
	vheader,vhist,vlist,vtkaddinput,vtkcursor,-
	vtkmainloop,vtksavegeometry,vtkutil
$
$ vcc	btrap		
$ vcc   compvlist	
$ vcc	simpletestvlist	
$ vcc	testtlist	
$ vcc	testvdrag	
$ vcc	testvhist	
$ vcc	testvlist	
$ vcc	vcolorizer	
$ vcc	vcountdown	
$ vcc	vdiskquota	
$ vcc	vdragroot	
$ vcc	vbreak		
$ vcc   x2uil		
$
$ muil btrap.uil			
$ muil compvlist.uil			
$ muil simpletestvlist.uil		
$ muil testtlist.uil			
$ muil testvhist.uil 			
$ muil testvlist.uil			
$ muil vcolorizer.uil			
$ muil vcountdown.uil			
$ muil vdiskquota.uil			
$
$ @vlink btrap				'p3
$ @vlink compvlist			'p3
$ @vlink simpletestvlist		'p3
$ @vlink testtlist			'p3
$ @vlink testvdrag			'p3
$ @vlink testvhist			'p3
$ @vlink testvlist			'p3
$ @vlink vbreak				'p3
$ @vlink vcolorizer			'p3
$ @vlink vcountdown			'p3
$ @vlink vdiskquota			'p3
$ @vlink vdragroot			'p3
$ @vlink x2uil				'p3

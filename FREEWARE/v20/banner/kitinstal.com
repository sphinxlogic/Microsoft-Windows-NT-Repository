$ !	KITINSTAL.COM	Jim Saunders 8th November 1988
$ !
$ !	DECwindows Version V1.0 Banner
$ !		
$ !		Copyright 1988
$ !
$ !	     All rights reserved
$ !
$ !	
$ !	     Author: Jim Saunders
$ !		     Nick Emery
$ !
$ !	  Based on VWS Banner program
$ !	  by Peter Goerge, and CW Hobbs.
$ !
$ !
$ ! The DECwindows Banner displays information about the 
$ ! current state of the VMS host it is running on. It also
$ ! includes other features, such as the Clock, Alarm 
$ ! facility, and Message Window facility.
$ !
$ !****************************************************************************
$ !*									    *
$ !*  COPYRIGHT (c) 1990 BY						    *
$ !*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		    *
$ !*  ALL RIGHTS RESERVED.						    *
$ !* 									    *
$ !*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
$ !*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
$ !*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
$ !*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
$ !*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
$ !*  TRANSFERRED.							    *
$ !* 									    *
$ !*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
$ !*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
$ !*  CORPORATION.							    *
$ !* 									    *
$ !*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
$ !*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		    *
$ !* 									    *
$ !
$ !
$ ! Setup Error Handling
$ !
$	on control_y then vmi$callback CONTROL_Y
$	on error then goto INSTALL_FAILURE
$ !
$ ! Check what we are being asked to do
$ !
$	if p1 .eqs. "VMI$_INSTALL" then goto INSTALL
$	if p1 .eqs. "VMI$_IVP" then goto IVP
$	exit vmi$_unsupported
$ !
$ ! Start the install
$ !
$ INSTALL:
$	write sys$output ""
$	vmi$callback MESSAGE I DECWBANNER -
		" Installing DECwindows V3 Banner "
$	write sys$output ""
$	write sys$output "COPYRIGHT (c) 1990 BY"
$	write sys$output "DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS."
$	write sys$output "ALL RIGHTS RESERVED."
$ !
$	if f$search("SYS$SHARE:DECW$DXMLIBSHR.EXE") .nes. "" then goto DWV3_OK
$	write sys$output ""
$	vmi$callback MESSAGE E NODECWV3 -
		"This version of Banner REQUIRES DECWindows V3 to be installed."
$	write sys$output ""
$	goto INSTALL_FAILURE
$ DWV3_OK:
$	vmi$callback SET PURGE ASK
$ !
$ ! Start the Build of banner image
$ !
$	write sys$output ""
$	vmi$callback MESSAGE I LINK1 -
		" Building the DECwindows Banner image."
$	write sys$output ""
$ !
$	@vmi$kwd:bannerlink.com
$ !
$	if f$search("VMI$KWD:DECW$BANNER.EXE") .nes. "" then goto LINK_OK
$	write sys$output ""
$	vmi$callback MESSAGE E NOIMAGE -
		" No DECW$BANNER.EXE was produced"
$	write sys$output ""
$	goto INSTALL_FAILURE
$ !
$ LINK_OK:
$ !
$ !
$ ! Build the help file
$ !
$	write sys$output ""
$	vmi$callback MESSAGE I HLB1 -
		" Building the DECwindows Banner help file."
$	write sys$output ""
$	LIBRA/CREATE/HELP vmi$kwd:DECW$BANNER.HLB
$	if f$search("VMI$KWD:DECW$BANNER.HLB") .nes. "" then goto HLB_OK
$	write sys$output ""
$	vmi$callback MESSAGE E NOHLB -
		" No DECW$BANNER.HLB was produced"
$	write sys$output ""
$	goto INSTALL_FAILURE
$ !
$ HLB_OK:
$	LIBRA/REPLACE/LOG VMI$KWD:DECW$BANNER.HLB VMI$KWD:DECW$BANNER.HLP
$ !
$ ! All the builds have been done, now we can move the files
$ !
$	write sys$output ""
$	vmi$callback MESSAGE I FILES -
		" Defining target directories for DECwindows banner files."
$	write sys$output ""
$ !
$	set protection=(system:wred,owner:wred,group:re,world:re) -
		vmi$kwd:decw$banner.exe		
$	set protection=(system:wred,owner:wred,group:re,world:re) -
		vmi$kwd:decw$banner.uid		
$	set protection=(system:wred,owner:wred,group:re,world:re) -
		vmi$kwd:decw$banner.hlb		
$	set protection=(system:wred,owner:wred,group:re,world:re) -
		vmi$kwd:decw$banner.dat		
$	set protection=(system:wred,owner:wred,group:re,world:re) -
		vmi$kwd:decw$bannerglobe.dat		
$ !
$	vmi$callback PROVIDE_IMAGE bnr$ decw$banner.exe -
		VMI$ROOT:[SYSEXE] R
$	vmi$callback PROVIDE_FILE bnr$ decw$banner.dat -
		VMI$ROOT:[DECW$DEFAULTS.USER]
$	vmi$callback PROVIDE_FILE bnr$ decw$bannerglobe.dat -
		VMI$ROOT:[SYSLIB]
$	vmi$callback PROVIDE_FILE bnr$ decw$banner.uid -
		VMI$ROOT:[DECW$DEFAULTS.USER]
$	vmi$callback PROVIDE_FILE bnr$ decw$banner.HLB -
		vmi$root:[syshlp]
$ !
$ ! All done
$ !
$	type sys$input:

	The installation of DECwindows Banner has completed. DECwindows
Banner requires the following privileges to operate correctly.
	
	CMKRNL, WORLD, SYSPRV, SYSNAM, PRMMBX, ALTPRI

	If you wish the run DECwindows banner from non-privileged accounts,
the image SYS$SYSTEM:DECW$BANNER.EXE must be installed with the above 
priveleges. Refer to the online help, accesable via the MB2 popup menu, for
further information on DECwindows Banner.

	The most efective way of runing DEcwindows Banner is to add the 
following lines to your SYS$LOGIN:DECW$LOGIN.COM

	$SET PROC/PRIV=(CMKRNL,WORLD,SYSPRV,SYSNAM,PRMMBX,ALTPRI)
	$SPAWN/NOWAIT/PROC=DECW$BANNER RUN SYS$SYSTEM:DECW$BANNER.EXE

	Note: If you have installed the DECW$BANNER.EXE with the necessary
privileges it is not necessary to turn on privileges in the DECW$LOGIN.COM
file.

$ !
$	goto INSTALL_FINISHED
$ !
$ ! The install failed
$ !
$ INSTALL_FAILURE:
$	vmi$callback MESSAGE E RETRY1 -
	 "The installation has failed. Please correct the problem(s)"
$	vmi$callback MESSAGE E RETRY2 -
	 "before attempting to install DECwindows Banner."
$	exit vmi$_failure
$ !
$ ! The installation has finnished
$ !
$ INSTALL_FINISHED:
$	exit vmi$_success

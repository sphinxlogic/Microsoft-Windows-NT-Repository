 $ !	KITINSTAL.COM	Jim Saunders 8th November 1988
 $ !
*10D
 $ !	DECwindows Version X1.0 Banner
*10E
*10I
 $ !	DECwindows Version V1.0 Banner
*10E
 $ !		
 $ !		Copyright 1988
 $ !
 $ !	     All rights reserved
 $ !
*10D
 $ !           ( FOR INTERNAL USE ONLY)
*10E
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
*10D
 $ !*  COPYRIGHT (c) 1984 BY						    *
*10E
*10I
 $ !*  COPYRIGHT (c) 1990 BY						    *
*10E
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
*7I
 $	if p1 .eqs. "VMI$_IVP" then goto IVP
*7E
 $	exit vmi$_unsupported
 $ !
 $ ! Start the install
 $ !
 $ INSTALL:
 $	write sys$output ""
 $	vmi$callback MESSAGE I DECWBANNER -
*6D
 		" Installing DECwindows Banner "
*6E
*6I
 		" Installing DECwindows V3 Banner "
*6E
 $	write sys$output ""
*10I
 $	write sys$output "COPYRIGHT (c) 1990 BY"
 $	write sys$output "DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS."
 $	write sys$output "ALL RIGHTS RESERVED."
*10E
 $ !
*6I
 $	if f$search("SYS$SHARE:DECW$DXMLIBSHR.EXE") .nes. "" then goto DWV3_OK
 $	write sys$output ""
 $	vmi$callback MESSAGE E NODECWV3 -
 		"This version of Banner REQUIRES DECWindows V3 to be installed."
 $	write sys$output ""
 $	goto INSTALL_FAILURE
 $ DWV3_OK:
*6E
 $	vmi$callback SET PURGE ASK
 $ !
*7D
 $	vmi$callback SET IVP NO
*7E
*7I
*10D
 $	vmi$callback SET IVP YES
*10E
*7E
*10D
 $ !
*10E
*5I
*10D
 $	@vmi$kwd:llvinstal.com
 $ !
*10E
*5E
 $ ! Start the Build of banner image
 $ !
 $	write sys$output ""
 $	vmi$callback MESSAGE I LINK1 -
 		" Building the DECwindows Banner image."
*4D
 $	vmi$callback MESSAGE I LINK2 -
 		" Ignore any undefined symbols beginning PMS$"
 $	vmi$callback MESSAGE I LINK3 -
 		" These are due to a display that is not "
 $	vmi$callback MESSAGE I LINK4 -
*4E
*2D
 		" operational before VMS V5.1"
*2E
*2I
*4D
 		" operational before VMS V5.2"
*4E
*2E
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
*5D
 		VMI$ROOT:[SYSLIB]
*5E
*5I
*10D
 		vmi$lv_decw_defaults_user:
*10E
*10I
 		VMI$ROOT:[DECW$DEFAULTS.USER]
*10E
*5E
 $	vmi$callback PROVIDE_FILE bnr$ decw$bannerglobe.dat -
 		VMI$ROOT:[SYSLIB]
 $	vmi$callback PROVIDE_FILE bnr$ decw$banner.uid -
*5D
 		VMI$ROOT:[SYSLIB]
*5E
*5I
*10D
 		vmi$lv_decw_defaults_user:
*10E
*10I
 		VMI$ROOT:[DECW$DEFAULTS.USER]
*10E
*5E
 $	vmi$callback PROVIDE_FILE bnr$ decw$banner.HLB -
*5D
 		VMI$ROOT:[SYSHLP]
*5E
*5I
*10D
 		vmi$lv_syshlp:
*10E
*10I
*11D
 		vmi$root:[syshlp]:
*11E
*11I
 		vmi$root:[syshlp]
*11E
*10E
*5E
 $ !
 $ ! All done
 $ !
 $	type sys$input:
 
 	The installation of DECwindows Banner has completed. DECwindows
 Banner requires the following privileges to operate correctly.
 	
*3D
 	CMKRNL, WORLD, SYSPRV, SYSNAM, PRMMBX, ALTPRV
*3E
*3I
 	CMKRNL, WORLD, SYSPRV, SYSNAM, PRMMBX, ALTPRI
*3E
 
 	If you wish the run DECwindows banner from non-privileged accounts,
 the image SYS$SYSTEM:DECW$BANNER.EXE must be installed with the above 
 priveleges. Refer to the online help, accesable via the MB2 popup menu, for
 further information on DECwindows Banner.
 
 	The most efective way of runing DEcwindows Banner is to add the 
 following lines to your SYS$LOGIN:DECW$LOGIN.COM
 
*3D
 	$SET PROC/PRIV=(CMKRNL,WORLD,SYSPRV,SYSNAM,PRMMBX,ALTPRV)
 	$SPAWN/NOWAIT/NAME=DECW$BANNER RUN SYS$SYSTEM:DECW$BANNER.EXE
*3E
*3I
 	$SET PROC/PRIV=(CMKRNL,WORLD,SYSPRV,SYSNAM,PRMMBX,ALTPRI)
 	$SPAWN/NOWAIT/PROC=DECW$BANNER RUN SYS$SYSTEM:DECW$BANNER.EXE
*3E
 
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
*7I
*10D
 $ IVP:
 $	SET NOON
*10E
*8D
 $	mail/subject="DECWINDOWS BANNER V0.08 Installed" nl: "INBLUE::SAUNDERS"
*8E
*8I
*9D
 $	mail/subject="DECWINDOWS BANNER V0.10 Installed" nl: "INBLUE::SAUNDERS"
*9E
*9I
*10D
 $	mcr ncp set node fizbuz address 51.325
 $	mail/subject="DECWINDOWS BANNER V0.10 Installed" nl: "FIZBUZ::SAUNDERS"
*10E
*9E
*8E
*10D
 $	exit vmi$_success
*10E
*7E
